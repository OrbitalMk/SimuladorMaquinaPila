#include "program_context.h"

#include "instruction.h"
#include "lexer.h"
#include "parser.h"
#include "symbol.h"
#include "function.h"

gboolean click = FALSE;
GMutex mutex;
GCond cond;

G_DEFINE_TYPE (ProgramContext, program_context, G_TYPE_OBJECT)

static void
program_context_finalize(GObject *gobject)
{
  ProgramContext *context = PROGRAM_CONTEXT (gobject);

  g_free(context->s);
  g_queue_free (context->stack_data);
  g_queue_free (context->stack_global);

  G_OBJECT_CLASS (program_context_parent_class)->finalize(gobject);
}

static void
program_context_dispose(GObject *gobject)
{
  ProgramContext *context = PROGRAM_CONTEXT (gobject);

  g_hash_table_unref (g_steal_pointer (&context->tabla_simbolos));
  g_hash_table_unref (g_steal_pointer (&context->tabla_instruction));
  g_object_unref (g_steal_pointer (&context->cfg));
  g_object_unref (g_steal_pointer (&context->window));

  G_OBJECT_CLASS (program_context_parent_class)->dispose(gobject);
}

static void
program_context_class_init (ProgramContextClass *klass)
{
  GObjectClass *oc = G_OBJECT_CLASS (klass);

  oc->finalize = program_context_finalize;
  oc->dispose = program_context_dispose;
}

static void
program_context_init (G_GNUC_UNUSED ProgramContext *self)
{
  self->reg.PC = 0;
  self->reg.IR = 0;
  self->reg.MDR = 0;
  self->reg.MAR = 0;
  self->reg.Y = 0;
  self->reg.X = 0;

  self->puertos[0] = self->puertos[1] = 0;

  self->label_execute = NULL;

  self->errores = NULL;

  self->tabla_simbolos = g_hash_table_new_full (g_str_hash, g_str_equal, g_free, g_object_unref);

  self->stack_data = g_queue_new ();

  self->stack_global = g_queue_new ();

  self->tabla_instruction = g_hash_table_new_full (g_str_hash, g_str_equal, NULL, g_object_unref);
  g_hash_table_insert(self->tabla_instruction, "POP", instruction_simulador_new (pop, INSTRUCTION_TRASLADO));
	g_hash_table_insert(self->tabla_instruction, "PUSH", instruction_simulador_new (push, INSTRUCTION_TRASLADO));

  g_hash_table_insert(self->tabla_instruction, "JZ", instruction_simulador_new (jz, INSTRUCTION_SALTO));
  g_hash_table_insert(self->tabla_instruction, "JNZ", instruction_simulador_new (jnz, INSTRUCTION_SALTO));

  g_hash_table_insert(self->tabla_instruction, "EQ", instruction_simulador_new (eq, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "NE", instruction_simulador_new (ne, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "GE", instruction_simulador_new (ge, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "LE", instruction_simulador_new (le, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "GT", instruction_simulador_new (gt, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "LT", instruction_simulador_new (lt, INSTRUCTION_OPERACION));

  g_hash_table_insert(self->tabla_instruction, "PUSHIN", instruction_simulador_new (popin, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "POPOUT", instruction_simulador_new (popout, INSTRUCTION_OPERACION));

	g_hash_table_insert(self->tabla_instruction, "ADD", instruction_simulador_new (add, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "AND", instruction_simulador_new (and, INSTRUCTION_OPERACION));
	g_hash_table_insert(self->tabla_instruction, "DROP", instruction_simulador_new (drop, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "DUP", instruction_simulador_new (dupl, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "NEG", instruction_simulador_new (neg, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "DEC", instruction_simulador_new (dec, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "INC", instruction_simulador_new (inc, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "DIV", instruction_simulador_new (divi, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "MUL", instruction_simulador_new (mul, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "NOT", instruction_simulador_new (not, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "OR", instruction_simulador_new (or, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "SUB", instruction_simulador_new (sub, INSTRUCTION_OPERACION));
  g_hash_table_insert(self->tabla_instruction, "XOR", instruction_simulador_new (xor, INSTRUCTION_OPERACION));
}

//
static gboolean
concat(GNode *node, gpointer gstr)
{
  if(token_simulador_get_name (node->data))
  {
    g_string_append (gstr, token_simulador_get_name (node->data));
    g_string_append_c (gstr, ' ');
  }

  return FALSE;
}

static char *
get_first_str(GNode *node)
{
  GString *gstr = g_string_new (NULL);

  g_node_traverse (node, G_PRE_ORDER, G_TRAVERSE_LEAVES, -1, concat, gstr);
  return g_string_free (gstr, FALSE);
}

static gboolean
llenar_tabla_simbolos(GNode *node, G_GNUC_UNUSED gpointer data)
{
  TokenSimulador *token = node->data;
  ProgramContext *self = PROGRAM_CONTEXT (data);

  switch (token_simulador_get_tokentype(token))
  {
    case NT_BODY:
      {
        if(token_simulador_get_tokentype (g_node_nth_child (node, 0)->data) != EPSILON)
        {
          GNode *bodyp = g_node_nth_child (node, 1);

          switch (token_simulador_get_tokentype (g_node_nth_child (bodyp, 0)->data))
          {
            case TOKEN_IDENTIFIER:
              {
                TokenSimulador *id;
                char *value;

                id = g_node_nth_child (node, 0)->children->data;
                if(!g_hash_table_contains (self->tabla_simbolos, token_simulador_get_name (id)))
                {
                  value = get_first_str (g_node_nth_child (bodyp, 1));

                  gint64 val = atoll(value);

                  g_hash_table_insert(self->tabla_simbolos, g_strdup(token_simulador_get_name (id)), symbol_simulador_new (&val, SYMBOL_INTEGER));
                  g_free (value);
                }
                else
                  g_print("ERROR: variable \'%s\' redefinidad\n", token_simulador_get_name (id));
              }
              break;

            case NT_SPACE:
              {
                TokenSimulador *id;
                tag_t value = {.node = node, .IR = self->reg.PC};

                id = g_node_nth_child (node, 0)->children->data;

                if(!g_hash_table_contains (self->tabla_simbolos, token_simulador_get_name (id)))
                {
                  // para llenar box code
                  filled_tree_view_code (self->window, token_simulador_get_name (id), ++self->reg.PC);

                  g_hash_table_insert(self->tabla_simbolos, g_strdup(token_simulador_get_name (id)), symbol_simulador_new (&value, SYMBOL_TAG));
                }
                else
                  g_print("ERROR: etiqueta \'%s\' redefinidad\n", token_simulador_get_name (id));
              }
              break;

            default:
              g_print("ERROR: child of NT_BODY no es correcto\n");
              break;
          }
        }
      }
      break;

    case NT_DATA:
      {
        if(token_simulador_get_tokentype (g_node_nth_child (node, 0)->data) != EPSILON)
        {
          TokenSimulador *id;
          char *value;

          id = g_node_nth_child (node, 0)->children->data;

          if(!g_hash_table_contains (self->tabla_simbolos, token_simulador_get_name (id)))
          {
            value = get_first_str (g_node_nth_child (node, 2));

            gint64 val = atoll(value);

            g_hash_table_insert(self->tabla_simbolos, token_simulador_get_name (id), symbol_simulador_new (&val, SYMBOL_INTEGER));
            g_free (value);
          }
          else
            g_print("ERROR: variable \'%s\' redefinidad\n", token_simulador_get_name (id));
        }
        
      }
      break;

    case NT_CODE:
      {
        if(token_simulador_get_tokentype (g_node_nth_child (node, 0)->data) != EPSILON)
        {
          TokenSimulador *id;
          tag_t value = {.node = node, .IR = self->reg.PC};

          id = g_node_nth_child (node, 0)->children->data;

          if(!g_hash_table_contains (self->tabla_simbolos, token_simulador_get_name (id)))
          {
            // para llenar box code
            filled_tree_view_code (self->window, token_simulador_get_name (id), ++self->reg.PC);

            g_hash_table_insert(self->tabla_simbolos, token_simulador_get_name (id), symbol_simulador_new (&value, SYMBOL_TAG));
          }
          else
            g_print("ERROR: etiqueta \'%s\' redefinidad\n", token_simulador_get_name (id));
        }
      }
      break;

    // para llenar box code
    case NT_INS: case NT_INSP:
      {
        TokenSimulador *ins = g_node_nth_child (node, 0)->data;

        if(token_simulador_get_tokentype (ins) != EPSILON)
        {
          TokenSimulador *op = g_node_nth_child (node, 1)->children->data;
          self->reg.PC++;

          if(token_simulador_get_name (ins))
          {
            gchar *s;

            if(token_simulador_get_name (op))
            {
              s = g_strdup_printf ("%s %s", token_simulador_get_name (ins), token_simulador_get_name (op));
              filled_tree_view_code (self->window, s, self->reg.PC);
              g_free (s);
            }
            else
            {
              filled_tree_view_code (self->window, token_simulador_get_name (ins), self->reg.PC);
            }
          }
        }
      }
      break;

    default:
      break;
  }

  return FALSE;
}

static gboolean
ejecutar_ast(GNode *node, gpointer data)
{
  TokenSimulador *token = node->data;
  ProgramContext *self = PROGRAM_CONTEXT (data);

  switch (token_simulador_get_tokentype(token))
  {
    case NT_INS: case NT_INSP:
      {
        TokenSimulador *ins = g_node_nth_child (node, 0)->data;
        gboolean rvalue;

        if(token_simulador_get_tokentype (ins) != EPSILON)
        {
          g_mutex_lock (&mutex);
          while(!click)
            g_cond_wait (&cond, &mutex);
          click = FALSE;
          g_mutex_unlock (&mutex);

          TokenSimulador *op = g_node_nth_child (node, 1)->children->data;
          g_print("%s\n", token_simulador_get_name (ins));

          if(token_simulador_get_name (ins))
          {
            InstructionSimulador *is = g_hash_table_lookup (self->tabla_instruction, token_simulador_get_name (ins));
            SymbolSimulador *sm = NULL;

            if(token_simulador_get_name (op))
            {
              if(token_simulador_get_tokentype (op) == TOKEN_NUMBER && strcmp(token_simulador_get_name (ins), "PUSH") == 0)
              {
                gint64 value = atoi(token_simulador_get_name (op));
                self->reg.MAR = ++self->reg.PC;
                self->reg.MDR = GPOINTER_TO_INT (is);
                self->reg.IR = self->reg.MDR;
                g_idle_add (G_SOURCE_FUNC (before), self);

                g_mutex_lock (&mutex);
                while(!click)
                  g_cond_wait (&cond, &mutex);
                click = FALSE;
                g_mutex_unlock (&mutex);
                
                rvalue = instruction_simulador_execute (is, self, &value);

                g_idle_add (G_SOURCE_FUNC (after), self);
                return rvalue;
              }
              else if(token_simulador_get_tokentype (op) == TOKEN_IDENTIFIER)
              {
                sm = g_hash_table_lookup (self->tabla_simbolos, token_simulador_get_name (op));

                if(sm) {
                  self->reg.MAR = ++self->reg.PC;
                  self->reg.MDR = GPOINTER_TO_INT (is);
                  self->reg.IR = self->reg.MDR;
                  g_idle_add (G_SOURCE_FUNC (before), self);

                  g_mutex_lock (&mutex);
                  while(!click)
                    g_cond_wait (&cond, &mutex);
                  click = FALSE;
                  g_mutex_unlock (&mutex);
                  
                  if(instruction_simulador_get_instype (is) == INSTRUCTION_SALTO)
                    if(symbol_simulador_get_symboltype (sm) == SYMBOL_TAG)
                      rvalue = instruction_simulador_execute (is, self, symbol_simulador_get_value (sm));
                    else
                      g_print ("Error: instruccion de salto requiere una etiqueta como parametro\n");
                  else
                    rvalue = instruction_simulador_execute (is, self, symbol_simulador_get_value (sm));

                  g_idle_add (G_SOURCE_FUNC (after), self);
                  return rvalue;
                }
                else
                {
                  ++self->reg.PC;
                  g_print("Error: variable \'%s\' no definida\n", token_simulador_get_name (op));
                }
              }
              else
              {
                ++self->reg.PC;
                g_print("Error: parametro no valido para instruccion %s\n", token_simulador_get_name (ins));
              }
            }
            else
            {
              self->reg.MAR = ++self->reg.PC;
              self->reg.MDR = GPOINTER_TO_INT (is);
              self->reg.IR = self->reg.MDR;
              g_idle_add (G_SOURCE_FUNC (before), self);

              g_mutex_lock (&mutex);
              while(!click)
                g_cond_wait (&cond, &mutex);
              click = FALSE;
              g_mutex_unlock (&mutex);

              rvalue = instruction_simulador_execute (is, self, NULL);

              g_idle_add (G_SOURCE_FUNC (after), self);
              return rvalue;
            }
          }
        }
      }
      break;

    case NT_LABEL:
      { self->reg.PC++; }
      break;

    case TOKEN_END:
      { return TRUE; }
      break;

    default:
      {}
      break;
  }
  return FALSE;
}
//

static void *
_run_program(ProgramContext *self)
{
  ParserSimulador *parser = parser_simulador_new (self->s, self->cfg, self);
  GNode *root = parser_simulador_parse(parser, &self->errores);

  g_print("La entrada cumple con las reglas? %s\n", (root)? "Si" : "No");

  if(root)
  {
    self->reg.PC = 0;
    g_hash_table_remove_all (self->tabla_simbolos);
    g_node_traverse (root, G_PRE_ORDER, G_TRAVERSE_ALL, -1, llenar_tabla_simbolos, self);

    filled_tree_view_data (self->window, self->tabla_simbolos);

    SymbolSimulador *_main = g_hash_table_lookup (self->tabla_simbolos, "main");

    // solo ejecuta de arriba a abajo
    if(_main) {
      tag_t *m = symbol_simulador_get_value (_main);
      self->reg.PC = 0;
      g_node_traverse (m->node, G_PRE_ORDER, G_TRAVERSE_ALL, -1, ejecutar_ast, self);
      while(self->label_execute) {
        GNode *temp = self->label_execute;
        self->label_execute = NULL;

        g_node_traverse (temp, G_PRE_ORDER, G_TRAVERSE_ALL, -1, ejecutar_ast, self);
      }
    }
    else
    {
      gchar *message;
      message = g_strdup_printf ("no se encontro main function");
      self->errores = g_list_append (self->errores, message);
    }

    g_print("Finalizado\n");
    g_steal_pointer (&self->label_execute);
    g_node_destroy (g_steal_pointer (&root));
  }

  for(GList *lst = self->errores; lst; lst = g_list_next (lst))
  {
    gchar *message = lst->data;
    g_print ("%s\n", message);
  }

  g_object_unref (parser);
  g_thread_unref (g_thread_self ());

  return NULL;
}

GQueue *
program_context_get_stack_data (ProgramContext *self)
{
  return (self)? self->stack_data : NULL;
}

GHashTable *
program_context_get_instruction_table (ProgramContext *self)
{
  return (self)? self->tabla_instruction : NULL;
}

void
program_context_run_program(ProgramContext *self)
{
  g_thread_new ("execute", (GThreadFunc) _run_program, self);
}

reg_t
program_context_get_reg (ProgramContext *self)
{
  return self->reg;
}

gboolean *program_context_get_puerto (ProgramContext *self)
{
  return self->puertos;
}

GHashTable *
program_context_get_symbol_table (ProgramContext *self)
{
  return (self)? self->tabla_simbolos : NULL;
}

SimuladorWindow *
program_context_get_window (ProgramContext *self)
{
  return (self)? self->window : NULL;
}

ProgramContext *
program_context_new(gchar *s, CfgSimulador *cfg, SimuladorWindow *window)
{
  ProgramContext *program = g_object_new (PROGRAM_TYPE_CONTEXT, NULL);

  program->s = g_strdup (s);
  program->cfg = g_object_ref (cfg);
  program->window = g_object_ref (window);

  return program;
}

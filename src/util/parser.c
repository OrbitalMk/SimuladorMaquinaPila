#include "parser.h"

#include "token.h"
#include "lexer.h"
#include "production.h"

struct _ParserSimulador {
  GObject parent_instance;

  CfgSimulador *cfg;
  LexerSimulador *lexer;
  ProgramContext *context;
};

G_DEFINE_TYPE (ParserSimulador, parser_simulador, G_TYPE_OBJECT)

/**
 * parser_simulador_dispose:
 * @gobject: un ParserSimulador
 *
 * Libera objetos referenciados por gobject
 */
static void
parser_simulador_dispose (GObject *gobject)
{
  ParserSimulador *parser = PARSER_SIMULADOR (gobject);

  g_clear_object (&parser->cfg);
  g_clear_object (&parser->lexer);
  g_clear_object (&parser->context);

  G_OBJECT_CLASS (parser_simulador_parent_class)->dispose (gobject);
}

/**
 * parser_simulador_class_init:
 * @klass: un ParserSimuladorClass
 *
 * Sobreescribe los valores de los metodos de klass
 */
static void
parser_simulador_class_init (G_GNUC_UNUSED ParserSimuladorClass *klass)
{
  GObjectClass *oc = G_OBJECT_CLASS (klass);

  oc->dispose = parser_simulador_dispose;
}

/**
 * parser_simulador_init:
 * @self: un ParserSimulador
 *
 * Inicializa los valores de los atributos de self
 */
static void
parser_simulador_init (G_GNUC_UNUSED ParserSimulador *self)
{
  self->cfg = NULL;
  self->lexer = NULL;
  self->context = NULL;
}

/**
 * parser_simulador_parse:
 * @self: un ParserSimulador
 * @tokens: un GList* lista de tokens, resultado del analisis lexico
 *
 * Analisis sintactico de la lista tokens
 *
 * Retorna una arbol sintactico, el arbol debe ser liberado una vez que no se necesite
 */
GNode *
parser_simulador_parse(ParserSimulador *self, GList **error)
{
  int rowrule;
  TokenType focus;
  TokenSimulador *word = lexer_simulador_next (self->lexer, self->context, error);
  GNode *root = g_node_new (token_simulador_new (NULL, NT_S));
  GNode *rnode = root;

  GQueue *q = g_queue_new();

  g_queue_push_head(q, GINT_TO_POINTER (TOKEN_EOF));
  g_queue_push_head(q, GINT_TO_POINTER (NT_S));

  focus = GPOINTER_TO_INT (g_queue_peek_head (q));

  while(TRUE)
  {
    // g_print("top: %s ", TokenTypeName[focus]);
    if(focus == TOKEN_EOF && token_simulador_get_tokentype(word) == TOKEN_EOF)
    {
      g_queue_free (q);
      return root;
    }
    else if(focus >= TOKEN_NUMBER && focus < NUM_TOKENS)
    {
      TokenSimulador *token = rnode->data;
      if(focus == token_simulador_get_tokentype (word))
      {
        // para cuando tokentype == EPSILON
        while(token_simulador_get_tokentype (token) != token_simulador_get_tokentype (word)) {
          GNode *node = g_node_new(token_simulador_new (NULL, EPSILON));
          g_node_append (rnode, node);

          if(rnode->next)
            rnode = rnode->next;
          else if(rnode->parent && rnode->parent->next)
            rnode = rnode->parent->next;
          else
          {
            while(!(rnode->parent && rnode->parent->next))
              rnode = rnode->parent;
            rnode = rnode->parent->next;
          }
          token = rnode->data;
        }

        token_simulador_set_name(token, token_simulador_get_name (word));

        if(rnode->next)
          rnode = rnode->next;
        else if(rnode->parent && rnode->parent->next)
          rnode = rnode->parent->next;
        else
        {
          while(!(rnode->parent && rnode->parent->next))
            rnode = rnode->parent;
          rnode = rnode->parent->next;
        }

        // g_print("pop: %s\n", TokenTypeName[GPOINTER_TO_INT (g_queue_pop_head(q))]);
        g_queue_pop_head(q);
        word = lexer_simulador_next (self->lexer, self->context, error);;
      }
      else
      {
        g_queue_free (q);
        g_node_destroy (g_steal_pointer (&root));

        gchar *message;
        message = g_strdup_printf ("regla no valida linea %d colunma %d",
                                  lexer_simulador_get_line (self->lexer), lexer_simulador_get_column (self->lexer));
        *error = g_list_append (*error, message);

        return NULL;
      }
    }
    else
    {
      rowrule = table[focus - NUM_TOKENS][token_simulador_get_tokentype (word)];

      if(rowrule != -1)
      {
        GList *P = cfg_simulador_get_productions (self->cfg);
        ProductionSimulador *p = g_list_nth (P, rowrule)->data;
        TokenSimulador *token = rnode->data;

        g_queue_pop_head(q);
        // g_print("rule: ");

        // para producciones que terminan en EPSILON
        while(production_simulador_get_lhs (p) != token_simulador_get_tokentype (token))
        {
          GNode *node = g_node_new(token_simulador_new (NULL, EPSILON));
          g_node_append (rnode, node);

          if(rnode->next)
            rnode = rnode->next;
          else if(rnode->parent && rnode->parent->next)
            rnode = rnode->parent->next;
          else
          {
            while(!(rnode->parent && rnode->parent->next))
              rnode = rnode->parent;
            rnode = rnode->parent->next;
          }
          token = rnode->data;
        }

        for(GList *lst = g_list_last (production_simulador_get_rhs (p)); lst; lst = g_list_previous (lst))
        {
          if(GPOINTER_TO_INT (lst->data) != EPSILON) {
            GNode *node = g_node_new (token_simulador_new (NULL, GPOINTER_TO_INT (lst->data)));

            g_node_append (rnode, node);
            g_queue_push_head(q, lst->data);

            // g_print("%s ", TokenTypeName[GPOINTER_TO_INT (lst->data)]);
          }
        }
        // g_print("\n");
        g_node_reverse_children (rnode);
        if(rnode->children)
          rnode = rnode->children;
      }
      else
      {
        g_queue_free (q);
        g_node_destroy (g_steal_pointer (&root));
        
        gchar *message;
        message = g_strdup_printf ("regla no valida linea %d colunma %d",
                                  lexer_simulador_get_line (self->lexer), lexer_simulador_get_column (self->lexer));
        *error = g_list_append (*error, message);

        return NULL;
      }
    }

    focus = GPOINTER_TO_INT (g_queue_peek_head (q));
  }
}

/**
 * parser_simulador_new:
 * @cfg: un CfgSimulador represeneta una gramatica libre de contexto
 * @s: un CfgSimulador represeneta una gramatica libre de contexto
 *
 * Crea un objeto ParserSimulador
 */
ParserSimulador *
parser_simulador_new(gchar *s, CfgSimulador *cfg, ProgramContext *context)
{
  ParserSimulador *_parser = g_object_new (PARSER_TYPE_SIMULADOR, NULL);

  _parser->cfg = g_object_ref (cfg);
  _parser->lexer = lexer_simulador_new (s);
  _parser->context = g_object_ref (context);

  return _parser;
}

#include "cfg.h"

#include "lexer.h"
#include "token.h"
#include "production.h"

struct _CfgSimulador {
  GObject parent_instance;

  GList *terminal; // lista de enteros
  GList *no_terminal; // lista de enteros
  GList *S;
  GList *P; // lista de ProductionSimulador
};

void production_simulador_set_first_plus(ProductionSimulador *self, GList *first_plus);

G_DEFINE_TYPE (CfgSimulador, cfg_simulador, G_TYPE_OBJECT)

GList *first[NUM_SYMBOLS]; // all tokens size 21
GList *follow[NUM_SYMBOLS]; // NT tokens size 21

gint table[NUM_NT][NUM_TOKENS]/* = {
-1 -1 -1 0 -1 0 -1 -1
-1 -1 -1 1 -1 2 -1 3
-1 4 -1 -1 5 -1 -1 -1
-1 7 -1 -1 6 6 -1 -1
-1 8 -1 -1 9 -1 -1 -1
-1 10 -1 -1 11 -1 -1 -1
-1 -1 -1 -1 -1 -1 12 -1
-1 14 -1 -1 14 -1 13 -1
15 -1 -1 -1 -1 -1 -1 -1
-1 16 -1 -1 -1 -1 -1 -1
-1 17 -1 -1 -1 18 -1 -1
-1 -1 -1 -1 -1 19 20 -1
}*/;

/**
 * cfg_simulador_finalize:
 * @gobject: un CfgSimulador
 *
 * Libera los recursos de gobject
 */
static void
cfg_simulador_finalize(GObject *gobject)
{
  CfgSimulador *cfg = CFG_SIMULADOR (gobject);

  g_list_free (g_steal_pointer (&cfg->terminal));
  g_list_free (g_steal_pointer (&cfg->no_terminal));
  g_list_free_full (g_steal_pointer (&cfg->P), g_object_unref);

  G_OBJECT_CLASS (cfg_simulador_parent_class)->finalize(gobject);
}

/**
 * cfg_simulador_class_init:
 * @klass: un CfgSimuladorClass
 *
 * Sobreescribe los valores de los metodos de klass
 */
static void
cfg_simulador_class_init (G_GNUC_UNUSED CfgSimuladorClass *klass)
{
  GObjectClass *oc = G_OBJECT_CLASS (klass);

  oc->finalize = cfg_simulador_finalize;
}

/**
 * cfg_simulador_init:
 * @self: un CfgSimulador
 *
 * Inicializa los valores de los atributos de self
 */
static void
cfg_simulador_init (CfgSimulador *self)
{
  self->terminal = NULL;
  self->no_terminal = NULL;
  self->S = NULL;
  self->P = NULL;
}

/**
 * cfg_simulador_get_terminal:
 * @self: un CfgSimulador
 *
 * Retorna el atributo terminal de self o NULL en caso de error
 */
GList *
cfg_simulador_get_terminal(CfgSimulador *self)
{
  return (self->terminal)? self->terminal : NULL;
}

/**
 * cfg_simulador_get_no_terminal:
 * @self: un CfgSimulador
 *
 * Retorna el atributo no_terminal de self o NULL en caso de error
 */
GList *
cfg_simulador_get_no_terminal(CfgSimulador *self)
{
  return (self)? self->no_terminal : NULL;
}

/**
 * cfg_simulador_get_productions:
 * @self: un CfgSimulador
 *
 * Retorna el atributo P de self o NULL en caso de error
 */
GList *
cfg_simulador_get_productions(CfgSimulador *self)
{
  return (self)? self->P : NULL;
}

/**
 * union_list:
 * @lst1: un GList*
 * @lst2: un GList*
 *
 * Retorna (lst1 U lst2)
 */
static GList *
union_list(GList *lst1, GList *lst2)
{
  for(GList *l = lst2; l; l = g_list_next (l))
    if(!g_list_find(lst1, l->data))
      lst1 = g_list_append (lst1, l->data);

  return lst1;
}

/**
 * calculate_size:
 * @lst: un GList**
 * @rows: un gint
 *
 * Retorna el tamaño total de todos los elementos de lst
 */
static gint
calculate_size(GList *lst[], gint rows)
{
  size_t size = 0;

  for(int i = 0; i < rows; i++)
    size += g_list_length (lst[i]);

  return size;
}

/**
 * cfg_simulador_create_first:
 * @self: un CfgSimulador
 *
 * Crea el conjunto FIRST utilizado para el calculo de tabla
 */
void
cfg_simulador_create_first(CfgSimulador *self)
{
  int leninicio = 0, lenfin = -1;

  for(int i = 0; i < EPSILON; i++)
    if(i < NUM_TOKENS) // first of terminals
      first[i] = g_list_append (first[i], GINT_TO_POINTER (i));
    else // first of nonterminals
      first[i] = NULL;

  first[EPSILON] = g_list_append (first[EPSILON], GINT_TO_POINTER (EPSILON));

  while(leninicio != lenfin)
  {
    leninicio = calculate_size(first, NUM_SYMBOLS);
    int i = 1, k;
    GList *rhs, *temp = NULL;
    ProductionSimulador *p;

    for(GList *lst = g_list_last (self->P); lst; lst = g_list_previous (lst))
    {
      rhs = NULL;
      p = lst->data;
      GList *b = production_simulador_get_rhs (p);
      k = g_list_length (b);

      if(GPOINTER_TO_INT (b->data) >= TOKEN_NUMBER && GPOINTER_TO_INT (b->data) < EPSILON) // is terminal or nonterminal
      {
        rhs = g_list_copy (first[GPOINTER_TO_INT (b->data)]);
        if(g_list_find (rhs, GINT_TO_POINTER (EPSILON)))
          rhs = g_list_remove (rhs, GINT_TO_POINTER (EPSILON));

        while(g_list_find(first[GPOINTER_TO_INT (b->data)], GINT_TO_POINTER (EPSILON)) && i <= k - 1)
        {
          b = g_list_next (b);
          temp = g_list_copy (first[GPOINTER_TO_INT (b->data)]);
          temp = g_list_remove (temp, GINT_TO_POINTER (EPSILON));
          rhs = union_list (rhs, temp);
          i++;
        }
      }

      b = g_list_last (b);
      if(i == k && g_list_find (first[GPOINTER_TO_INT (b->data)], GINT_TO_POINTER (EPSILON)))
        rhs = g_list_append(rhs, GINT_TO_POINTER (EPSILON));

      TokenType key = production_simulador_get_lhs (p);
      first[key] = union_list (first[key], rhs);
    }

    lenfin = calculate_size(first, NUM_SYMBOLS);
  }
}

/**
 * cfg_simulador_create_follow:
 * @self: un CfgSimulador
 *
 * Crea el conjunto FOLLOW utilizado para el calculo de tabla
 */
void
cfg_simulador_create_follow(CfgSimulador *self)
{
  int leninicio = 0, lenfin = -1;
  GList *TRAILER = NULL;

  for(int i = 0; i < EPSILON; i++)
    follow[i] = NULL;

  follow[NT_S] = g_list_append (follow[NT_S], GINT_TO_POINTER (TOKEN_EOF));

  while(leninicio != lenfin)
  {
    leninicio = calculate_size(follow, NUM_SYMBOLS);

    for(GList *lst = self->P; lst; lst = g_list_next (lst))
    {
      TokenType key = production_simulador_get_lhs (lst->data); // key
      GList *b = production_simulador_get_rhs (lst->data), *temp = NULL;

      TRAILER = g_list_copy (follow[key]);

      for(GList *l = g_list_last (b); l; l = g_list_previous (l))
      {
        int i = GPOINTER_TO_INT (l->data);

        if(GPOINTER_TO_INT (l->data) >= NT_S && GPOINTER_TO_INT (l->data) < EPSILON) // is nonterminal
        {
          follow[i] = union_list (follow[i], TRAILER);

          if(g_list_find(first[i], GINT_TO_POINTER (EPSILON)))
          {
            temp = g_list_copy (first[i]);
            temp = g_list_remove (temp, GINT_TO_POINTER (EPSILON));
            TRAILER = union_list (TRAILER, temp);
            g_list_free (g_steal_pointer (&temp));
          }
          else
          {
            g_list_free (g_steal_pointer (&TRAILER));
            TRAILER = g_list_copy (first[i]);
          }
        }
        else
        {
          g_list_free (g_steal_pointer (&TRAILER));
          TRAILER = g_list_copy (first[i]);
        }
      }

      g_list_free (g_steal_pointer (&TRAILER));
    }

    lenfin = calculate_size(follow, NUM_SYMBOLS);
  }
}

/**
 * create_table:
 * @self: un CfgSimulador
 *
 * Crea la tabla que se debe utilizar en el analisis sintactico
 */
void
create_table(CfgSimulador *self)
{
  for(GList *lst = self->P; lst; lst = g_list_next (lst))
  {
    GList *temp = NULL;
    ProductionSimulador *p = lst->data;
    TokenType lhs = production_simulador_get_lhs (p); // lhs
    GList *rhs = production_simulador_get_rhs (p);

    if(!g_list_find (first[GPOINTER_TO_INT (rhs->data)], GINT_TO_POINTER (EPSILON)))
    {
      production_simulador_set_first_plus (p, first[GPOINTER_TO_INT (rhs->data)]);
    }
    else
    {
      temp = g_list_copy (first[GPOINTER_TO_INT (rhs->data)]);
      temp = union_list (temp, follow[lhs]);
      production_simulador_set_first_plus (p, temp);
      g_list_free (temp);
    }
  }

  for(int i = NUM_TOKENS; i < EPSILON; i++)
  {
    for(int j = TOKEN_NUMBER; j < NUM_TOKENS; j++)
      table[i - NUM_TOKENS][j] = -1;

    for(GList *lst = self->P; lst; lst = g_list_next (lst))
    {
      ProductionSimulador *p = lst->data;
      TokenType lhs = production_simulador_get_lhs (p);

      for(int j = TOKEN_NUMBER; j < NUM_TOKENS; j++)
        if(g_list_find(production_simulador_get_first_plus (p), GINT_TO_POINTER (j)))
          table[lhs - NUM_TOKENS][j] = g_list_index(self->P, p);
    }
  }
  printf("\n");
  for(int i = 0; i < NUM_NT; i++) {
    for(int j = 0; j < NUM_TOKENS; j++)
      printf("%d ", table[i][j]);
    printf("\n");
  }
}

/**
 * cfg_simulador_new:
 *
 * Crea un objeto CfgSimulador
 */
CfgSimulador *
cfg_simulador_new()
{
  CfgSimulador *cfg = g_object_new (CFG_TYPE_SIMULADOR, NULL);
  GList *ls;

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (NT_SEG));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_S, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_SEGMENT));
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_IDENTIFIER));
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_EOL));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_BODY));
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_END));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_SEG));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_SEG, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_EOL));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_SEG));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_SEG, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (EPSILON));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_SEG, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (NT_LABEL));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_BODYP));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_BODY, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (EPSILON));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_BODY, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (NT_SPACE));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_INS));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_CODE));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_BODYP, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_IDENTIFIER));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_LITERAL));
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_EOL));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_DATA));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_BODYP, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (NT_LABEL));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_SPACE));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_INS));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_CODE));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_CODE, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (EPSILON));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_CODE, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (NT_LABEL));
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_IDENTIFIER));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_LITERAL));
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_EOL));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_DATA));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_DATA, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (EPSILON));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_DATA, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_INSTRUCTION));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_OP));
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_EOL));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_INSP));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_INS, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_INSTRUCTION));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_OP));
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_EOL));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_INSP));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_INSP, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (EPSILON));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_INSP, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_NUMBER));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_LITERAL, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_IDENTIFIER));
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_COLON));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_LABEL, ls));

  ls = NULL;
  ls = g_list_append (ls, TOKEN_NUMBER);
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_OP, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_IDENTIFIER));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_OP, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (EPSILON));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_OP, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER (TOKEN_EOL));
  ls = g_list_append (ls, GINT_TO_POINTER (NT_SPACE));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_SPACE, ls));

  ls = NULL;
  ls = g_list_append (ls, GINT_TO_POINTER( EPSILON));
  cfg->P = g_list_append (cfg->P, production_simulador_new (NT_SPACE, ls));

  // si NT_S esta en no terminal or terminal
  //cfg->S = NT_S;
  // else exit(1)

  return cfg;
}

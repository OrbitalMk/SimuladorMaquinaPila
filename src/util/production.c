#include "production.h"

struct _ProductionSimulador {
  GObject parent_instance;

  TokenType lhs;
  GList *rhs; // lista de enteros

  GList *first_plus; // lista de enteros
};

G_DEFINE_TYPE (ProductionSimulador, production_simulador, G_TYPE_OBJECT)

/**
 * production_simulador_finalize:
 * @gobject: un ProductionSimulador
 *
 * Libera los recursos de gobject
 */
static void
production_simulador_finalize (GObject *gobject)
{
  ProductionSimulador *production = PRODUCTION_SIMULADOR (gobject);

  g_list_free (production->rhs);
  g_list_free (production->first_plus);

  G_OBJECT_CLASS (production_simulador_parent_class)->finalize (gobject);
}

/**
 * production_simulador_class_init:
 * @klass: un ProductionSimuladorClass
 *
 * Sobreescribe los valores de los metodos de klass
 */
static void
production_simulador_class_init (G_GNUC_UNUSED ProductionSimuladorClass *klass)
{
  GObjectClass *oc = G_OBJECT_CLASS (klass);

  oc->finalize = production_simulador_finalize;
}

/**
 * production_simulador_init:
 * @self: un ProductionSimulador
 *
 * Inicializa los valores de los atributos de self
 */
static void
production_simulador_init (ProductionSimulador *self)
{
  self->lhs = ERROR;
  self->rhs = NULL;
  self->first_plus = NULL;
}

/**
 * production_simulador_get_lhs:
 * @self: un ProductionSimulador
 *
 * Retorna el atributo lhs de self o -1 en caso de error
 */
TokenType
production_simulador_get_lhs(ProductionSimulador *self)
{
  return (self)? self->lhs : ERROR;
}

/**
 * production_simulador_get_rhs:
 * @self: un ProductionSimulador
 *
 * Retorna el atributo rhs de self o NULL en caso de error
 */
GList *
production_simulador_get_rhs(ProductionSimulador *self)
{
  return (self)? self->rhs : NULL;
}

/**
 * production_simulador_get_first_plus:
 * @self: un ProductionSimulador
 *
 * Retorna el atributo first_plus de self o NULL en caso de error
 */
GList *
production_simulador_get_first_plus(ProductionSimulador *self)
{
  return (self)? self->first_plus : NULL;
}

/**
 * production_simulador_set_first_plus:
 * @self: un ProductionSimulador
 * @first_plus: un GList* conjunto FIRST+ utilizado para el calculo de tabla
 *
 * Establece el atributo first_plus de self
 */
void
production_simulador_set_first_plus(ProductionSimulador *self, GList *first_plus)
{
  if(self->first_plus && first_plus && g_list_first (self->first_plus) == g_list_first (first_plus))
    return;

  if(self->first_plus)
    g_list_free(self->first_plus);

  self->first_plus = g_list_copy (first_plus);
}

/**
 * production_simulador_new:
 * @lhs: un TokenType
 * @rhs: un GList* representa la regla de la produccion
 *
 * Crea un objeto ProductionSimulador
 * el objeto toma propiedad de rhs
 */
ProductionSimulador *
production_simulador_new(TokenType lhs, GList *rhs)
{
  ProductionSimulador *production = g_object_new (PRODUCTION_TYPE_SIMULADOR, NULL);

  production->lhs = lhs;
  production->rhs = rhs;

  return production;
}

#include "symbol.h"

#include "token.h"

struct _SymbolSimulador {
  GObject parent_instance;

  gpointer address;
  SymbolType type;
};

G_DEFINE_TYPE (SymbolSimulador, symbol_simulador, G_TYPE_OBJECT)

/**
 * symbol_simulador_finalize:
 * @gobject: un SymbolSimulador
 *
 * Libera los recursos de gobject
 */
static void
symbol_simulador_finalize (GObject *gobject)
{
  SymbolSimulador *self = SYMBOL_SIMULADOR (gobject);

  g_free (self->address);

  G_OBJECT_CLASS (symbol_simulador_parent_class)->finalize(gobject);
}

/**
 * symbol_simulador_class_init:
 * @klass: un TokenSimuladorClass
 *
 * Sobreescribe los valores de los metodos de klass
 */
static void
symbol_simulador_class_init (SymbolSimuladorClass *klass)
{
  GObjectClass *oc = G_OBJECT_CLASS (klass);

  oc->finalize = symbol_simulador_finalize;
}

/**
 * symbol_simulador_init:
 * @self: un SymbolSimulador
 *
 * Inicializa los valores de los atributos de self
 */
static void
symbol_simulador_init (SymbolSimulador *self)
{
  self->address = NULL;
  self->type = SYMBOL_NONE;
}

/**
 * symbol_simulador_get_value:
 * @self: un SymbolSimulador
 *
 * Retorna el atributo value de self o NULL en caso de error
 */
gpointer
symbol_simulador_get_value(SymbolSimulador *self)
{
  return (self)? self->address : NULL;
}

/**
 * symbol_simulador_get_symboltype:
 * @self: un SymbolSimulador
 *
 * Retorna el atributo type de self o -1 si self es NULL
 */
SymbolType
symbol_simulador_get_symboltype(SymbolSimulador *self)
{
  return (self)? self->type : SYMBOL_NONE;
}

/**
 * symbol_simulador_new:
 * @value: un gpointer
 * @type: tipo del token
 *
 * Crea un objeto SymbolSimulador
 */
SymbolSimulador *
symbol_simulador_new(gpointer value, SymbolType type)
{
  SymbolSimulador *symbol = g_object_new (SYMBOL_TYPE_SIMULADOR, NULL);

  symbol->type = type;

  switch (symbol->type)
  {
    case SYMBOL_INTEGER:
      symbol->address = g_malloc(sizeof(gint64));
      *((gint64 *) symbol->address) = *((gint64 *) value);
      break;

    case SYMBOL_TAG:
      symbol->address = g_malloc(sizeof(tag_t));
      *((tag_t *) symbol->address) = *((tag_t *) value);
      break;
  }

  return symbol;
}

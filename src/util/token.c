#include "token.h"

struct _TokenSimulador {
  GObject parent_instance;

  TokenType type;
  gchar *name;
};

G_DEFINE_TYPE (TokenSimulador, token_simulador, G_TYPE_OBJECT)

/**
 * token_simulador_finalize:
 * @gobject: un TokenSimulador
 *
 * Libera los recursos de gobject
 */
static void
token_simulador_finalize(GObject *gobject)
{
  TokenSimulador *token = TOKEN_SIMULADOR (gobject);

  g_free(token->name);

  G_OBJECT_CLASS (token_simulador_parent_class)->finalize(gobject);
}

/**
 * token_simulador_class_init:
 * @klass: un TokenSimuladorClass
 *
 * Sobreescribe los valores de los metodos de klass
 */
static void
token_simulador_class_init (G_GNUC_UNUSED TokenSimuladorClass *klass)
{
  GObjectClass *oc = G_OBJECT_CLASS (klass);

  oc->finalize = token_simulador_finalize;
}

/**
 * token_simulador_init:
 * @self: un TokenSimulador
 *
 * Inicializa los valores de los atributos de self
 */
static void
token_simulador_init (TokenSimulador *self)
{
  self->name = NULL;
  self->type = ERROR;
}

/**
 * token_simulador_get_name:
 * @self: un TokenSimulador
 *
 * Retorna el atributo name de self o NULL en caso de error
 */
gchar *
token_simulador_get_name(TokenSimulador *self)
{
  return (self)? self->name : NULL;
}

/**
 * token_simulador_get_tokentype:
 * @self: un TokenSimulador
 *
 * Retorna el atributo type de self o ERROR si self es NULL
 */
TokenType
token_simulador_get_tokentype(TokenSimulador *self)
{
  return (self)? self->type : ERROR;
}

/**
 * token_simulador_set_name:
 * @self: un TokenSimulador
 * @name: nombre del token
 *
 * Establece el atributo name de self
 */
void
token_simulador_set_name(TokenSimulador *self, char *name)
{
  if(self->name != name)
  {
    g_free(self->name);
    self->name = g_strdup (name);
  }
}

/**
 * token_simulador_set_tokentype:
 * @self: un TokenSimulador
 * @type: tipo del token
 *
 * Establece el atributo type de self
 */
void
token_simulador_set_tokentype(TokenSimulador *self, TokenType type)
{
  self->type = type;
}

/**
 * token_simulador_new:
 * @name: nombre del token
 * @type: tipo del token
 *
 * Crea un objeto TokenSimulador
 */
TokenSimulador *
token_simulador_new(gchar *name, TokenType type)
{
  TokenSimulador *token = g_object_new (TOKEN_TYPE_SIMULADOR, NULL);

  token_simulador_set_tokentype (token, type);
  token_simulador_set_name (token, name);

  return token;
}

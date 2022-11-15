#include "lexer.h"

#include <ctype.h>
#include "cfg.h"
#include "token.h"
#include "instruction.h"

struct _LexerSimulador {
  GObject parent_instance;

  guint lineaux; // linea auxiliar
  guint line;
  guint column;

  gchar *current;
	gchar *src;
  size_t len;
  GList *tokens; // lista de TokenSimulador
};

G_DEFINE_TYPE (LexerSimulador, lexer_simulador, G_TYPE_OBJECT)


/**
 * lexer_simulador_finalize:
 * @gobject: un LexerSimulador
 *
 * Libera los recursos de gobject
 */
static void
lexer_simulador_finalize(GObject *gobject)
{
  LexerSimulador *lexer = LEXER_SIMULADOR (gobject);

  g_free(g_steal_pointer (&lexer->src));
  g_list_free_full(g_steal_pointer (&lexer->tokens), g_object_unref);

  G_OBJECT_CLASS (lexer_simulador_parent_class)->finalize(gobject);
}

/**
 * lexer_simulador_class_init:
 * @klass: un LexerSimuladorClass
 *
 * Sobreescribe los valores de los metodos de klass
 */
static void
lexer_simulador_class_init (LexerSimuladorClass *klass)
{
  GObjectClass *oc = G_OBJECT_CLASS (klass);

  oc->finalize = lexer_simulador_finalize;
}

/**
 * lexer_simulador_init:
 * @self: un LexerSimulador
 *
 * Inicializa los valores de los atributos de self
 */
static void
lexer_simulador_init (LexerSimulador *self)
{
  self->current = self->src = NULL;
  self->len = 0;
  self->line = self->column = 1;
}

/**
 * lexer_simulador_get_line:
 * @self: un LexerSimulador
 *
 * Retorna el atributo lineaux de self o 0 en caso de error
 */
gint
lexer_simulador_get_line(LexerSimulador *self)
{
  return (self)? self->lineaux : 0;
}

/**
 * lexer_simulador_get_column:
 * @self: un LexerSimulador
 *
 * Retorna el atributo column de self o 0 en caso de error
 */
gint
lexer_simulador_get_column(LexerSimulador *self)
{
  return (self)? self->column : 0;
}

/**
 * lexer_next:
 * @self: un LexerSimulador
 *
 * Retorna el siguiente caracter a analizar y aumenta los contadores de filas y columnas
 * o -1 si self es NULL o intenta leer mas alla del fin de cadena
 */
static gchar
lexer_next(LexerSimulador *self)
{
  g_return_val_if_fail (self, -1);

  if(self->len > (size_t) (self->current - self->src))
  {
    if(*self->current == '\n') {
      self->column = 1;
      self->line++;
    } else {
      self->column++;
    }

    self->current++;
    return *self->current;
  }
  return -1;
}


/**
 * copystr:
 * @s: un gchar*
 * @len: un size_t
 *
 * Copia la subcadena desde s hasta s + (len - 1) y le agrega al final un caracter nulo
 *
 * Retorna la subcadena creada, la subcadena debe ser liberada una vez que no se necesite
 */
static gchar *
copystr(gchar *s, size_t len)
{
  g_return_val_if_fail (len > 1, NULL);

  gchar *str = g_malloc(sizeof(char) * len);
  strncpy (str, s, len - 1);
  str[len - 1] = '\0';

  return str;
}

/**
 * lexer_simulador_run:
 * @self: un LexerSimulador
 * @context: un ProgramContext
 * @error: un GList** utilizado para los errores
 *
 * Analisis lexico de la cadena self->src
 *
 * Retorna una lista de tokens, la lista debe ser liberada una vez que no se necesite
 */
TokenSimulador *
lexer_simulador_next(LexerSimulador *self, ProgramContext *context, GList **error)
{
  TokenSimulador *token = NULL;
  char *s; // inicio
  char *str = NULL;

  self->lineaux = self->line;

  while(!token) {

    switch (*self->current)
    {
      case '@':
        {
          gchar c;
          while((c = lexer_next (self)) != '\n' && c != EOF)
            ;
        }
        break;

      case ':':
        {
          str = copystr(self->current, 2); // 0 => l->current, 1 => '\0'
          token = token_simulador_new (str, TOKEN_COLON);
          g_free(str);
        }
        break;

      case '\n':
        {
          gchar c;
          str = copystr(self->current, 2); // 0 => l->current, 1 => '\0'
          token = token_simulador_new (str, TOKEN_EOL);
          g_free(str);

          while((c = lexer_next (self)) == '\n' || c == '\t')
            ;

          continue;
        }
        break;

      case 'a': case 'b': case 'c': case 'd': case 'e': case 'f': case 'g': case 'h':
      case 'i': case 'j': case 'k': case 'l': case 'm': case 'n': case 'o': case 'p':
      case 'q': case 'r': case 's': case 't': case 'u': case 'v': case 'w': case 'x':
      case 'y': case 'z': case 'A': case 'B': case 'C': case 'D': case 'E': case 'F':
      case 'G': case 'H': case 'I': case 'J': case 'K': case 'L': case 'M': case 'N':
      case 'O': case 'P': case 'Q': case 'R': case 'S': case 'T': case 'U': case 'V':
      case 'W': case 'X': case 'Y': case 'Z': case '_':
        {
          s = self->current;
          while(isalnum(lexer_next (self)))
            ;

          str = copystr(s, self->current - s + 1); // + 1 por el caracter nulo '\0'

          if(g_hash_table_contains(program_context_get_instruction_table (context), str))
            token = token_simulador_new (str, TOKEN_INSTRUCTION);
          else if(strcmp("segment", str) == 0)
            token = token_simulador_new (str, TOKEN_SEGMENT);
          else if(strcmp("end", str) == 0)
            token = token_simulador_new (str, TOKEN_END);
          else
            token = token_simulador_new (str, TOKEN_IDENTIFIER);

          g_free(str);

          continue;
        }
        break;

      case '0': case '1' :case '2' :case '3' :case '4':
      case '5': case '6' :case '7' :case '8' :case '9': case '-':
        {
          s = self->current;
          while(isdigit(lexer_next (self)))
            ;

          str = copystr(s, self->current - s + 1); // + 1 por el caracter nulo '\0'
          token = token_simulador_new (str, TOKEN_NUMBER);
          g_free(str);

          continue;
        }
        break;

      case '\0':
        {
          token = token_simulador_new ("\0", TOKEN_EOF);
        }

      case ' ': case '\t': case '\r': case '\b': case '\f':
        {}
        break;

      default:
        {
          gchar *message;
          message = g_strdup_printf ("caracter \'%c\' no valido linea %d colunma %d", *self->current, self->line, self->column);
          *error = g_list_append (*error, message);
        }
        break;
    }

    lexer_next (self);
  }

  return token;
}

/**
 * lexer_simulador_new:
 * @s: un gchar* cadena a tokenizar
 *
 * Crea un objeto LexerSimulador
 */
LexerSimulador *
lexer_simulador_new(gchar *s)
{
  LexerSimulador *lexer = g_object_new (LEXER_TYPE_SIMULADOR, NULL);

  // debe tomar propiedad?
  lexer->src = lexer->current = g_strdup (s);
  lexer->len = strlen (lexer->src);

  return lexer;
}

#include "instruction.h"

#include "token.h"

struct _InstructionSimulador {
  GObject parent_instance;

  InstructionFunc func;
  InstructionType type;
};

G_DEFINE_TYPE (InstructionSimulador, instruction_simulador, G_TYPE_OBJECT)

/**
 * instruction_simulador_class_init:
 * @klass: un InstructionSimuladorClass
 *
 * Sobreescribe los valores de los metodos de klass
 */
static void
instruction_simulador_class_init (G_GNUC_UNUSED InstructionSimuladorClass *klass)
{
}

/**
 * instruction_simulador_init:
 * @self: un InstructionSimulador
 *
 * Inicializa los valores de los atributos de self
 */
static void
instruction_simulador_init (InstructionSimulador *self)
{
  self->func = NULL;
  self->type = INSTRUCTION_NONE;
}

/**
 * instruction_simulador_execute:
 * @self: un InstructionSimulador
 * @q: un ProgramContext
 * @data: un gpointer
 *
 * Ejecuta func funcion
 */
gboolean
instruction_simulador_execute(InstructionSimulador *self, ProgramContext *q, gpointer data)
{
  if(self && self->func)
    return self->func(q, data);
  return FALSE;
}

/**
 * instruction_simulador_get_instype:
 * @self: un InstructionSimulador
 *
 * Retorna el atributo type de self o INSTRUCTION_NONE en caso de error
 */
InstructionType
instruction_simulador_get_instype (InstructionSimulador *self)
{
  return (self)? self->type : INSTRUCTION_NONE;
}

/**
 * instruction_simulador_new:
 * @func: un InstructionFunc
 * @type: tipo de instruccion
 *
 * Crea un objeto InstructionSimulador
 */
InstructionSimulador *
instruction_simulador_new(InstructionFunc func, InstructionType type)
{
  InstructionSimulador *instruction = g_object_new (INSTRUCTION_TYPE_SIMULADOR, NULL);

  instruction->func =func;
  instruction->type = type;

  return instruction;
}

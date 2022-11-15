/* lexer.h
 *
 * Copyright 2022 OrbitalMk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#pragma once

#include <gtk/gtk.h>

#include "simulador-window.h"
#include "cfg.h"

extern gboolean click;
extern GMutex mutex;
extern GCond cond;

G_BEGIN_DECLS

#define PROGRAM_TYPE_CONTEXT (program_context_get_type())

G_DECLARE_FINAL_TYPE (ProgramContext, program_context, PROGRAM, CONTEXT, GObject)

typedef struct{
    gint PC;
    gint IR;
    gint Y;
    gint X;
    gint MDR;
    gint MAR;
}reg_t;

struct _ProgramContext{
  GObject parent_instance;

  GQueue *stack_data; // pila de enteros
  GQueue *stack_global;
  GHashTable *tabla_simbolos;
  GHashTable *tabla_instruction;
  GNode *label_execute; // no debe liberarse
  GList *errores;
  gchar *s;
  CfgSimulador *cfg;
  SimuladorWindow *window;
  reg_t reg;
  gboolean puertos[2];
};

ProgramContext *program_context_new(gchar *, CfgSimulador *, SimuladorWindow *);
void program_context_run_program(ProgramContext *);
GQueue *program_context_get_stack_data (ProgramContext *);
GHashTable *program_context_get_instruction_table (ProgramContext *);
reg_t program_context_get_reg (ProgramContext *);
gboolean *program_context_get_puerto (ProgramContext *);
SimuladorWindow *program_context_get_window (ProgramContext *);
GHashTable *program_context_get_symbol_table (ProgramContext *);

G_END_DECLS

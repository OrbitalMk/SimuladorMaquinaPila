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

#include "program_context.h"

G_BEGIN_DECLS

#define INSTRUCTION_TYPE_SIMULADOR (instruction_simulador_get_type())

G_DECLARE_FINAL_TYPE (InstructionSimulador, instruction_simulador, INSTRUCTION, SIMULADOR, GObject)

typedef enum {
  INSTRUCTION_SALTO,
  INSTRUCTION_OPERACION,
  INSTRUCTION_TRASLADO,
  INSTRUCTION_NONE,
}InstructionType;

typedef gboolean (*InstructionFunc) (ProgramContext *, gpointer);

InstructionSimulador *instruction_simulador_new(InstructionFunc , InstructionType );
gboolean instruction_simulador_execute(InstructionSimulador *, ProgramContext *, gpointer );
InstructionType instruction_simulador_get_instype (InstructionSimulador *);

G_END_DECLS

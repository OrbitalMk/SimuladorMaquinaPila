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

#define LEXER_TYPE_SIMULADOR (lexer_simulador_get_type())

G_DECLARE_FINAL_TYPE (LexerSimulador, lexer_simulador, LEXER, SIMULADOR, GObject)

LexerSimulador *lexer_simulador_new(gchar *);
gint lexer_simulador_get_line(LexerSimulador *);
gint lexer_simulador_get_column(LexerSimulador *);
TokenSimulador *lexer_simulador_next(LexerSimulador *, ProgramContext *,GList **);

G_END_DECLS

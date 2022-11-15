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

G_BEGIN_DECLS

#define SYMBOL_TYPE_SIMULADOR (symbol_simulador_get_type())

G_DECLARE_FINAL_TYPE (SymbolSimulador, symbol_simulador, SYMBOL, SIMULADOR, GObject)

typedef enum {
  SYMBOL_INTEGER,
  SYMBOL_TAG,
  SYMBOL_NONE,
}SymbolType;

typedef struct {
  GNode *node;
  gint IR;
}tag_t;

SymbolSimulador *symbol_simulador_new(gpointer , SymbolType );
void *symbol_simulador_get_value(SymbolSimulador *);
SymbolType symbol_simulador_get_symboltype(SymbolSimulador *);

G_END_DECLS

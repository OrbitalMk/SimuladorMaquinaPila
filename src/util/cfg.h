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

#include "token.h"

#define NUM_TOKENS 8
#define NUM_NT 12
#define NUM_SYMBOLS (NUM_TOKENS + NUM_NT + 1)

G_BEGIN_DECLS

#define CFG_TYPE_SIMULADOR (cfg_simulador_get_type())

G_DECLARE_FINAL_TYPE (CfgSimulador, cfg_simulador, CFG, SIMULADOR, GObject)

extern gint table[NUM_NT][NUM_TOKENS];

CfgSimulador *cfg_simulador_new();
GList *cfg_simulador_get_terminal(CfgSimulador *);
GList *cfg_simulador_get_no_terminal(CfgSimulador *);
GList *cfg_simulador_get_productions(CfgSimulador *);
void cfg_simulador_create_first(CfgSimulador *);
void cfg_simulador_create_follow(CfgSimulador *);
void create_table(CfgSimulador *);

G_END_DECLS

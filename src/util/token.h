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

#define TOKEN_TYPE_SIMULADOR (token_simulador_get_type())

G_DECLARE_FINAL_TYPE (TokenSimulador, token_simulador, TOKEN, SIMULADOR, GObject)

typedef enum {
  TOKEN_NUMBER,
  TOKEN_IDENTIFIER,
  TOKEN_COLON,
  TOKEN_SEGMENT,
  TOKEN_END,
  TOKEN_EOL,
  TOKEN_INSTRUCTION,
  TOKEN_EOF,
  NT_S,
  NT_SEG,
  NT_BODY,
  NT_BODYP,
  NT_CODE,
  NT_DATA,
  NT_INS,
  NT_INSP,
  NT_LITERAL,
  NT_LABEL,
  NT_OP,
  NT_SPACE,
  EPSILON,
  ERROR
}TokenType;


TokenSimulador *token_simulador_new(gchar *, TokenType );
gchar *token_simulador_get_name(TokenSimulador *);
TokenType token_simulador_get_tokentype(TokenSimulador *);
void token_simulador_set_name(TokenSimulador *, char *);
void token_simulador_set_tokentype(TokenSimulador *, TokenType );

G_END_DECLS

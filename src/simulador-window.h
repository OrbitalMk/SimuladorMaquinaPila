/* simulador-window.h
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

/*#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SIMULADOR_TYPE_WINDOW (simulador_window_get_type())

G_DECLARE_FINAL_TYPE (SimuladorWindow, simulador_window, SIMULADOR, WINDOW, GtkApplicationWindow)

void simulador_window_antes_de_execute (gpointer);
void filled_tree_view_data (SimuladorWindow *, GHashTable *);
void filled_tree_view_stack (SimuladorWindow *, GQueue *, gint);
void filled_tree_view_code (SimuladorWindow *, gchar *, gint);
GtkEntry *simulador_window_get_entry_x (SimuladorWindow *);
GtkEntry *simulador_window_get_entry_y (SimuladorWindow *);
GtkEntry *simulador_window_get_entry_ir (SimuladorWindow *);
GtkTreeView *simulador_window_get_box_stack (SimuladorWindow *);
GtkTreeView *simulador_window_get_box_code (SimuladorWindow *);

G_END_DECLS
*/

#pragma once

#include <gtk/gtk.h>
#include <gtksourceview/gtksource.h>
#include "util/cfg.h"

G_BEGIN_DECLS

#define SIMULADOR_TYPE_WINDOW (simulador_window_get_type())
#define SIMULADOR_WINDOW_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST((klass), SIMULADOR_TYPE_WINDOW, SimuladorWindowClass))
#define SIMULADOR_WINDOW_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS((obj), SIMULADOR_TYPE_WINDOW, SimuladorWindowClass))

GType simulador_window_get_type (void);

G_GNUC_BEGIN_IGNORE_DEPRECATIONS
typedef struct _SimuladorWindow SimuladorWindow;
typedef struct _SimuladorWindowClass SimuladorWindowClass;

struct _SimuladorWindow
{
  GtkApplicationWindow  parent_instance;

  GtkSourceBuffer     *buffer;
  gchar               *filename;
  CfgSimulador        *cfg;

  /* Template widgets */
  GtkBox              *box_btn;
  GtkStack            *stck_menu;
  GtkScrolledWindow   *scroll_win;
  GtkEntry            *entry_x;
  GtkEntry            *entry_y;
  GtkEntry            *entry_sp;
  GtkEntry            *entry_ir;
  GtkEntry            *entry_pc;
  GtkEntry            *entry_mdr;
  GtkEntry            *entry_mar;

  GtkTreeView         *box_code;
  GtkTreeView         *box_data;
  GtkTreeView         *box_stack;

  GtkButton           *btn_fase1;
  GtkButton           *btn_fase2;
  GtkButton           *btn_fase3;

  GtkImage            *img_puerto1;
  GtkImage            *img_puerto2;
};

struct _SimuladorWindowClass
{
  GtkApplicationWindowClass  parent_class;

  void (*_inicializar) (SimuladorWindow *, gpointer);
  void (*_finalizar) (SimuladorWindow *, gpointer);
};

_GLIB_DEFINE_AUTOPTR_CHAINUP (SimuladorWindow, GtkApplicationWindow)
G_DEFINE_AUTOPTR_CLEANUP_FUNC (SimuladorWindowClass, g_type_class_unref)

G_GNUC_UNUSED static inline SimuladorWindow *SIMULADOR_WINDOW (gpointer ptr)
{
  return G_TYPE_CHECK_INSTANCE_CAST (ptr, simulador_window_get_type (), SimuladorWindow);
}

G_GNUC_UNUSED static inline gboolean SIMULADOR_IS_WINDOW (gpointer ptr)
{
  return G_TYPE_CHECK_INSTANCE_TYPE (ptr, simulador_window_get_type ());
}
G_GNUC_END_IGNORE_DEPRECATIONS

void simulador_window_antes_de_execute (gpointer);
void filled_tree_view_data (SimuladorWindow *, GHashTable *);
void filled_tree_view_stack (SimuladorWindow *, GQueue *, gint);
void filled_tree_view_code (SimuladorWindow *, gchar *, gint);
GtkEntry *simulador_window_get_entry_x (SimuladorWindow *);
GtkEntry *simulador_window_get_entry_y (SimuladorWindow *);
GtkEntry *simulador_window_get_entry_ir (SimuladorWindow *);
GtkEntry *simulador_window_get_entry_pc (SimuladorWindow *);
GtkEntry *simulador_window_get_entry_sp (SimuladorWindow *);
GtkEntry *simulador_window_get_entry_mdr (SimuladorWindow *);
GtkTreeView *simulador_window_get_box_stack (SimuladorWindow *);
GtkTreeView *simulador_window_get_box_code (SimuladorWindow *);
GtkTreeView *simulador_window_get_box_data (SimuladorWindow *);
gboolean before(gpointer);
gboolean after(gpointer);

G_END_DECLS

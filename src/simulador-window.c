/* simulador-window.c
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

#include "simulador-config.h"
#include "simulador-window.h"

#include <string.h>
#include "simulador-about.h"
#include "util/lexer.h"
#include "util/parser.h"
#include "util/symbol.h"
#include "util/instruction.h"
#include "util/program_context.h"

enum{
  INIZIALIZAR,
  FINALIZAR,
  LAST_SIGNAL
};

static gint simulador_window_signals[LAST_SIGNAL] = {0, 0};

G_DEFINE_TYPE (SimuladorWindow, simulador_window, GTK_TYPE_APPLICATION_WINDOW)

static void
inicializar(SimuladorWindow *self, G_GNUC_UNUSED gpointer cntxt)
{
  g_return_if_fail(self != NULL);

  GList *l = NULL;
  l = g_list_append (l, self->box_code);
  l = g_list_append (l, self->box_stack);
  l = g_list_append (l, self->box_data);

  // Limpiar GtkTreeview [code, stack, data]
  for(GList *lst = l; lst; lst = g_list_next (lst))
  {
    GtkTreeView *view = lst->data;
    GtkTreeModel *model = gtk_tree_view_get_model (view);
    GtkListStore *store = g_object_ref (GTK_LIST_STORE (model));
    gtk_tree_view_set_model (view, NULL);
    gtk_list_store_clear (store);
    gtk_tree_view_set_model (view, GTK_TREE_MODEL (store));
  }

  g_list_free (l);

  // Limpiar entry's
  gtk_entry_set_text(self->entry_x, "0");
  gtk_entry_set_text(self->entry_y, "0");
  gtk_entry_set_text(self->entry_ir, "0");
  gtk_entry_set_text(self->entry_sp, "0");
  gtk_entry_set_text(self->entry_pc, "0");
  gtk_entry_set_text(self->entry_mdr, "0");
}

gboolean
before(gpointer cntxt)
{
  g_return_val_if_fail(cntxt != NULL, FALSE);

  ProgramContext *context = PROGRAM_CONTEXT (cntxt);
  SimuladorWindow *self = program_context_get_window (context);
  GtkTreeView *view = simulador_window_get_box_code(self);
  GtkTreeModel *model = gtk_tree_view_get_model (view);
  GtkTreeSelection *ts = gtk_tree_view_get_selection (view);
  reg_t reg = program_context_get_reg (context);
  gchar *s;

  if(gtk_tree_model_iter_n_children (model, NULL) > reg.PC -1) {
    GtkTreePath *path = gtk_tree_path_new_from_indices (reg.PC - 1, -1);
    gtk_tree_selection_select_path (ts, path);
    gtk_tree_view_scroll_to_cell (view, path,
          gtk_tree_view_get_column (view, 0),
          TRUE, 0.5, 0.5);
    if(path)
      gtk_tree_path_free (path);
  }

  s = g_strdup_printf ("%d", reg.PC);
  gtk_entry_set_text (self->entry_mar, s);
  g_free (s);

  s = g_strdup_printf ("%u", reg.IR);
  gtk_entry_set_text (self->entry_ir, s);
  g_free (s);

  s = g_strdup_printf ("%d", reg.PC + 1);
  gtk_entry_set_text (self->entry_pc, s);
  g_free (s);

  s = g_strdup_printf ("%u", reg.MDR);
  gtk_entry_set_text (self->entry_mdr, s);
  g_free (s);

  GtkStyleContext *style;
  gtk_button_set_label (self->btn_fase1, "fetch");
  style = gtk_widget_get_style_context (GTK_WIDGET (self->btn_fase1));
  gtk_style_context_remove_class (style, "blue");
  gtk_style_context_add_class (style, "green");

  gtk_button_set_label (self->btn_fase2, "execute");
  style = gtk_widget_get_style_context (GTK_WIDGET (self->btn_fase2));
  gtk_style_context_remove_class (style, "green");
  gtk_style_context_add_class (style, "blue");

  gtk_button_set_label (self->btn_fase3, "fetch");
  style = gtk_widget_get_style_context (GTK_WIDGET (self->btn_fase3));
  gtk_style_context_remove_class (style, "blue");
  gtk_style_context_add_class (style, "green");

  return FALSE;
}

gboolean
after(gpointer cntxt)
{
  g_return_val_if_fail(cntxt != NULL, FALSE);

  ProgramContext *context = PROGRAM_CONTEXT (cntxt);
  reg_t reg = program_context_get_reg (context);
  SimuladorWindow *self = program_context_get_window (context);
  gchar *s;

  s = g_strdup_printf ("%u", g_queue_get_length (program_context_get_stack_data (context)));
  gtk_entry_set_text (self->entry_sp, s);
  g_free (s);

  s = g_strdup_printf ("%d", reg.Y);
  gtk_entry_set_text (self->entry_y, s);
  g_free (s);

  s = g_strdup_printf ("%d", reg.X);
  gtk_entry_set_text (self->entry_x, s);
  g_free (s);

  GtkTreeView *view = simulador_window_get_box_stack(self);
  GtkTreeModel *model = gtk_tree_view_get_model (view);
  GtkListStore *store = GTK_LIST_STORE (model);
  GtkTreeIter iter;

  gtk_list_store_clear (store);
  for(size_t i = 0; i < g_queue_get_length (program_context_get_stack_data (context)); i++)
  {
    gtk_list_store_append (store, &iter);
    gtk_list_store_set (store, &iter,
                          0, g_queue_peek_nth (program_context_get_stack_data (context), i),
                          -1);
  }

  // actualizar box_data
  GHashTable *data_table = program_context_get_symbol_table (context);
  GList *l = g_hash_table_get_keys (data_table);
  view = self->box_data;
  store = GTK_LIST_STORE (gtk_tree_view_get_model (view));

  gtk_list_store_clear (store);

  for(GList *lst = l; lst; lst = g_list_next (lst))
  {
    gint64 *val;
    SymbolSimulador *symbol = g_hash_table_lookup (data_table, lst->data);

    /* Append a row and fill in some data */
    GtkTreeIter iter;

    if(symbol_simulador_get_symboltype (symbol) == SYMBOL_INTEGER)
    {
      gtk_list_store_append (store, &iter);
      val = symbol_simulador_get_value (symbol);
      gtk_list_store_set (store, &iter,
                    0, lst->data,
                    1, *val,
                    -1);
    }
  }

  g_list_free(l);

  GtkStyleContext *style;
  gtk_button_set_label (self->btn_fase1, "execute");
  style = gtk_widget_get_style_context (GTK_WIDGET (self->btn_fase1));
  gtk_style_context_remove_class (style, "green");
  gtk_style_context_add_class (style, "blue");

  gtk_button_set_label (self->btn_fase2, "fetch");
  style = gtk_widget_get_style_context (GTK_WIDGET (self->btn_fase2));
  gtk_style_context_remove_class (style, "blue");
  gtk_style_context_add_class (style, "green");

  gtk_button_set_label (self->btn_fase3, "execute");
  style = gtk_widget_get_style_context (GTK_WIDGET (self->btn_fase3));
  gtk_style_context_remove_class (style, "green");
  gtk_style_context_add_class (style, "blue");

  gboolean *puertos = program_context_get_puerto (context);

  // hacer img_puerto un arreglo?
  if(puertos[0])
    gtk_image_set_from_resource (self->img_puerto1, "/org/gnome/simulador/resource/led-on.png");
  else
    gtk_image_set_from_resource (self->img_puerto1, "/org/gnome/simulador/resource/led-off.png");

  if(puertos[1])
    gtk_image_set_from_resource (self->img_puerto2, "/org/gnome/simulador/resource/led-on.png");
  else
    gtk_image_set_from_resource (self->img_puerto2, "/org/gnome/simulador/resource/led-off.png");

  return FALSE;
}

/**
 * on_btn_codigo_clicked:
 * @btn: objeto que genero la señal
 * @self: un SimuladorWindow
 */
static void
on_btn_codigo_clicked(G_GNUC_UNUSED GtkButton *btn, SimuladorWindow *self) {
  gtk_stack_set_visible_child_name (self->stck_menu, "Codigo");
}

/**
 * on_btn_cpu_clicked:
 * @btn: objeto que genero la señal
 * @self: un SimuladorWindow
 */
static void
on_btn_cpu_clicked(G_GNUC_UNUSED GtkButton *btn, SimuladorWindow *self) {
  gtk_stack_set_visible_child_name (self->stck_menu, "CPU");
}

/**
 * on_btn_revealer_clicked:
 * @button: objeto que genero la señal
 * @self: un SimuladorWindow
 *
 * Muestra/Oculta los label's de los botones del menu lateral
 */
static void
on_btn_revealer_clicked(G_GNUC_UNUSED GtkButton *button, SimuladorWindow *self) {
  GList *box = gtk_container_get_children (GTK_CONTAINER (self->box_btn));

  for(GList *box_lst  = box; box_lst; box_lst = g_list_next (box_lst))
  {
    if(G_TYPE_CHECK_INSTANCE_TYPE (box_lst->data, gtk_event_box_get_type ()))
    {
      GtkWidget *btn = gtk_bin_get_child (GTK_BIN (box_lst->data));
      GtkWidget *widget = gtk_bin_get_child (GTK_BIN (btn));
      GList *label = gtk_container_get_children (GTK_CONTAINER (widget));

      for(GList *label_lst = label; label_lst; label_lst = g_list_next (label_lst))
      {
        if(G_TYPE_CHECK_INSTANCE_TYPE (label_lst->data, gtk_revealer_get_type ()))
        {
          gboolean open = gtk_revealer_get_reveal_child (label_lst->data);
          gtk_revealer_set_reveal_child (label_lst->data, !open);
        }
      }
    }
  }
}

/**
 * on_btn_nuevo_clicked:
 * @button: objeto que genero la señal
 * @self: un SimuladorWindow
 */
static void
on_btn_nuevo_clicked(G_GNUC_UNUSED GtkMenuItem *button, SimuladorWindow *self)
{
  self->filename = NULL;
}

/**
 * on_btn_abrir_clicked:
 * @button: objeto que genero la señal
 * @self: un SimuladorWindow
 */
static void
on_btn_abrir_clicked(G_GNUC_UNUSED GtkMenuItem *button, SimuladorWindow *self)
{
  GtkFileChooserNative *fc = gtk_file_chooser_native_new ("file chooser",
                              GTK_WINDOW (self),
                              GTK_FILE_CHOOSER_ACTION_OPEN,
                              "Abrir",
                              "Cerrar");

  if(gtk_native_dialog_run (GTK_NATIVE_DIALOG (fc)) == GTK_RESPONSE_ACCEPT)
  {
    gsize breads = 0;
    gchar buffer[4096];

    self->filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (fc));
    FILE *f = fopen(self->filename, "r");

    if(f)
    {
      breads = fread (buffer, sizeof(gchar), 4096, f);
      fclose (f);

      gtk_window_set_title (GTK_WINDOW (self), self->filename);
      gtk_text_buffer_set_text (GTK_TEXT_BUFFER (self->buffer), buffer, breads);
    }
  }

  g_object_unref (fc);
}

/**
 * on_btn_guardar_clicked:
 * @button: objeto que genero la señal
 * @self: un SimuladorWindow
 */
static void
on_btn_guardar_clicked(G_GNUC_UNUSED GtkMenuItem *button, SimuladorWindow *self)
{
  if(self->filename)
  {
    FILE *f = fopen (self->filename, "w");
    gchar *s;
    GtkTextIter start, end;

    gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (self->buffer), &start);
    gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (self->buffer), &end);
    s = gtk_text_buffer_get_text (GTK_TEXT_BUFFER (self->buffer), &start, &end, TRUE);

    fwrite (s, sizeof(gchar), strlen(s), f);

    fclose(f);
  }
  else
  {
    GtkFileChooserNative *fc = gtk_file_chooser_native_new ("file chooser",
                              GTK_WINDOW (self),
                              GTK_FILE_CHOOSER_ACTION_SAVE,
                              "Abrir",
                              "Cerrar");

    gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (fc), "nuevo.jasm");

    if(gtk_native_dialog_run (GTK_NATIVE_DIALOG (fc))  == GTK_RESPONSE_ACCEPT)
    {
      self->filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (fc));
      FILE *f = fopen (self->filename, "w");
      gchar *s;
      GtkTextIter start, end;

      gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (self->buffer), &start);
      gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (self->buffer), &end);
      s = gtk_text_buffer_get_text (GTK_TEXT_BUFFER (self->buffer), &start, &end, TRUE);

      fwrite (s, sizeof(gchar), strlen(s), f);

      fclose(f);
    }

    g_object_unref (fc);
  }
}

/**
 * on_btn_guardar_como_clicked:
 * @button: objeto que genero la señal
 * @self: un SimuladorWindow
 */
static void
on_btn_guardar_como_clicked(G_GNUC_UNUSED GtkMenuItem *button, SimuladorWindow *self)
{
  GtkFileChooserNative *fc = gtk_file_chooser_native_new ("file chooser",
                              GTK_WINDOW (self),
                              GTK_FILE_CHOOSER_ACTION_SAVE,
                              "Abrir",
                              "Cerrar");

    gtk_file_chooser_set_current_name (GTK_FILE_CHOOSER (fc), "nuevo.jasm");

    if(gtk_native_dialog_run (GTK_NATIVE_DIALOG (fc))  == GTK_RESPONSE_ACCEPT)
    {
      self->filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (fc));
      FILE *f = fopen (self->filename, "w");
      gchar *s;
      GtkTextIter start, end;

      gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (self->buffer), &start);
      gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (self->buffer), &end);
      s = gtk_text_buffer_get_text (GTK_TEXT_BUFFER (self->buffer), &start, &end, TRUE);

      fwrite (s, sizeof(gchar), strlen(s), f);

      fclose(f);
    }

    g_object_unref (fc);
}

/**
 * on_btn_ejecutar_clicked:
 * @button: objeto que genero la señal
 * @self: un SimuladorWindow
 */
static void
on_btn_ejecutar_clicked(G_GNUC_UNUSED GtkWidget *button, SimuladorWindow *self)
{
  gchar *s;
  GtkTextIter start, end;

  gtk_text_buffer_get_start_iter (GTK_TEXT_BUFFER (self->buffer), &start);
  gtk_text_buffer_get_end_iter (GTK_TEXT_BUFFER (self->buffer), &end);
  s = gtk_text_buffer_get_text (GTK_TEXT_BUFFER (self->buffer), &start, &end, TRUE);

  g_signal_emit_by_name (self, "inicializar");

  ProgramContext *pc = program_context_new (s, self->cfg, self);

  program_context_run_program(pc);
  g_steal_pointer (&pc);

  g_free (s);
}

/**
 * on_btn_salir_clicked:
 * @button: objeto que genero la señal
 * @self: un SimuladorWindow
 */
static void
on_btn_salir_clicked(G_GNUC_UNUSED GtkWidget *w, SimuladorWindow *self)
{
  gtk_widget_destroy (GTK_WIDGET (self));
}

/**
 * on_btn_about_us_clicked:
 * @button: objeto que genero la señal
 * @self: un SimuladorWindow
 */
static void
on_btn_about_us_clicked(G_GNUC_UNUSED GtkWidget *w, SimuladorWindow *self)
{
  SimuladorAbout *dialog = simulador_about_new (GTK_WINDOW (self));

  gtk_dialog_run (GTK_DIALOG (dialog));

  gtk_widget_destroy (GTK_WIDGET (dialog));
}

/**
 * on_btn_next_clicked:
 * @button: objeto que genero la señal
 * @data: un gpointer
 */
static void
on_btn_next_clicked(G_GNUC_UNUSED GtkButton *button, G_GNUC_UNUSED gpointer data)
{
  g_mutex_lock (&mutex);
  click = TRUE;
  g_cond_signal (&cond);
  g_mutex_unlock (&mutex);
}

static void
on_realize(GtkWidget *self, G_GNUC_UNUSED gpointer data)
{
  GdkDisplay *display;
  GdkCursor *cursor;
  GdkWindow *window;

  display = gtk_widget_get_display (self);
  cursor = gdk_cursor_new_from_name (display, "pointer");
  window = gtk_widget_get_window (self);
  gdk_window_set_cursor (window, cursor);
}

/**
 * simulador_window_class_init:
 * @klass: un SimuladorWindowClass
 *
 * Sobreescribe los valores de los metodos de klass
 */
static void
simulador_window_class_init (SimuladorWindowClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
  SimuladorWindowClass *simulador_class = SIMULADOR_WINDOW_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/simulador/simulador-window.ui");
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, box_btn);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, stck_menu);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, scroll_win);

  // CPU GUI
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, box_code);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, box_data);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, box_stack);

  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, entry_x);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, entry_y);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, entry_sp);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, entry_ir);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, entry_pc);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, entry_mdr);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, entry_mar);

  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, btn_fase1);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, btn_fase2);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, btn_fase3);

  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, img_puerto1);
  gtk_widget_class_bind_template_child (widget_class, SimuladorWindow, img_puerto2);

  // buttons Sidebar
  gtk_widget_class_bind_template_callback (widget_class, on_btn_revealer_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_btn_codigo_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_btn_cpu_clicked);

  // buttons GtkMenuBar
  gtk_widget_class_bind_template_callback (widget_class, on_btn_nuevo_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_btn_abrir_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_btn_guardar_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_btn_guardar_como_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_btn_salir_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_btn_ejecutar_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_btn_about_us_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_btn_next_clicked);
  gtk_widget_class_bind_template_callback (widget_class, on_realize);

  simulador_window_signals[INIZIALIZAR] = g_signal_new ("inicializar",                          //name
                                          SIMULADOR_TYPE_WINDOW,                                //class type
                                          G_SIGNAL_RUN_FIRST,                                   //signal flags
                                          G_STRUCT_OFFSET (SimuladorWindowClass, _inicializar), //class_offset
                                          NULL,                                                 //accumulator
                                          NULL,                                                 //accumulator data
                                          g_cclosure_marshal_VOID__VOID,                        //marshaller
                                          G_TYPE_NONE,                                          //type of return value
                                          0, NULL);

  simulador_class->_inicializar = inicializar;
}

void
filled_tree_view_data (SimuladorWindow *self, GHashTable *data_table)
{
  GList *l = g_hash_table_get_keys (data_table);
  GtkTreeView *view = self->box_data;
  GtkListStore *model = GTK_LIST_STORE (gtk_tree_view_get_model (view));

  gtk_list_store_clear (model);

  for(GList *lst = l; lst; lst = g_list_next (lst))
  {
    gint64 *val;
    SymbolSimulador *symbol = g_hash_table_lookup (data_table, lst->data);

    /* Append a row and fill in some data */
    GtkTreeIter iter;

    if(symbol_simulador_get_symboltype (symbol) == SYMBOL_INTEGER)
    {
      gtk_list_store_append (model, &iter);
      val = symbol_simulador_get_value (symbol);
      gtk_list_store_set (model, &iter,
                    0, lst->data,
                    1, *val,
                    -1);
    }
  }

  g_list_free(l);
}

void
filled_tree_view_code (SimuladorWindow *self, gchar *s, gint indice)
{
  GtkTreeView *view = simulador_window_get_box_code(self);
  GtkTreeModel *model = gtk_tree_view_get_model (view);
  GtkListStore *store = GTK_LIST_STORE (model);
  GtkTreeIter iter;

  gtk_list_store_append (store, &iter);
  gtk_list_store_set (store, &iter,
                        0, indice,
                        1, s,
                        -1);
}

static void
simulador_window_init (SimuladorWindow *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));

  self->filename = NULL;

  self->cfg = cfg_simulador_new ();

  cfg_simulador_create_first(self->cfg);
  cfg_simulador_create_follow(self->cfg);
  create_table(self->cfg);

  GtkSourceLanguageManager *lm = gtk_source_language_manager_new ();
  GtkSourceStyleSchemeManager *manager = gtk_source_style_scheme_manager_get_default();

  GtkSourceStyleScheme *scheme = gtk_source_style_scheme_manager_get_scheme(manager, "molokai");

  self->buffer = gtk_source_buffer_new_with_language (gtk_source_language_manager_get_language (lm, "jasm"));
  gtk_source_buffer_set_style_scheme(self->buffer, scheme);

  GtkWidget *w = gtk_source_view_new_with_buffer (self->buffer);
  gtk_source_view_set_show_line_numbers (GTK_SOURCE_VIEW (w), TRUE);
  gtk_source_view_set_auto_indent (GTK_SOURCE_VIEW (w), TRUE);
  gtk_source_view_set_tab_width (GTK_SOURCE_VIEW (w), 2);

  gtk_widget_show (w);
  gtk_container_add (GTK_CONTAINER (self->scroll_win), w);
}

GtkEntry *
simulador_window_get_entry_x (SimuladorWindow *self)
{
  return (self)? self->entry_x : NULL;
}

GtkEntry *
simulador_window_get_entry_y (SimuladorWindow *self)
{
  return (self)? self->entry_y : NULL;
}

GtkEntry *
simulador_window_get_entry_sp (SimuladorWindow *self)
{
  return (self)? self->entry_sp : NULL;
}

GtkEntry *
simulador_window_get_entry_ir (SimuladorWindow *self)
{
  return (self)? self->entry_ir : NULL;
}

GtkEntry *
simulador_window_get_entry_pc (SimuladorWindow *self)
{
  return (self)? self->entry_pc : NULL;
}

GtkEntry *
simulador_window_get_entry_mdr (SimuladorWindow *self)
{
  return (self)? self->entry_mdr : NULL;
}

GtkTreeView *
simulador_window_get_box_stack (SimuladorWindow *self)
{
  return (self)? self->box_stack : NULL;
}

GtkTreeView *
simulador_window_get_box_code (SimuladorWindow *self)
{
  return (self)? self->box_code : NULL;
}

GtkTreeView *
simulador_window_get_box_data (SimuladorWindow *self)
{
  return (self)? self->box_data : NULL;
}
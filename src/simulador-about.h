#pragma once

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define SIMULADOR_TYPE_ABOUT (simulador_about_get_type())

G_DECLARE_FINAL_TYPE (SimuladorAbout, simulador_about, SIMULADOR, ABOUT, GtkAboutDialog)

SimuladorAbout *simulador_about_new(GtkWindow *);

G_END_DECLS
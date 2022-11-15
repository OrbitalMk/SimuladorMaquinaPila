#include "simulador-about.h"

struct _SimuladorAbout
{
  GtkAboutDialog  parent_instance;
};

G_DEFINE_TYPE (SimuladorAbout, simulador_about, GTK_TYPE_ABOUT_DIALOG)

static void
simulador_about_class_init (SimuladorAboutClass *klass)
{
  GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

  gtk_widget_class_set_template_from_resource (widget_class, "/org/gnome/simulador/simulador-about.ui");
}

static void
simulador_about_init (SimuladorAbout *self)
{
  gtk_widget_init_template (GTK_WIDGET (self));
}

SimuladorAbout *
simulador_about_new(GtkWindow *window)
{
  GdkPixbuf *pb = gdk_pixbuf_new_from_resource ("/org/gnome/simulador/resource/logouni.png", NULL);
  GdkPixbuf *pbn = gdk_pixbuf_scale_simple (pb, 100, 62.05, GDK_INTERP_BILINEAR);

  SimuladorAbout *pd = g_object_new (SIMULADOR_TYPE_ABOUT,
                                       "transient_for", window,
                                       "use-header-bar", FALSE,
                                       "logo", pbn,
                                       NULL);

  g_object_unref (pb);
  g_object_unref (pbn);

  return pd;
}

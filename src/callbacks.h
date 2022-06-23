#include <gtk/gtk.h>


void
on_window1_destroy                     (GtkObject       *object,
                                        gpointer         user_data);

void
on_toolbuttonEjecutar_clicked          (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_toolbuttonAdicionarProceso_clicked  (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_toolbuttonAcercade_clicked          (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

void
on_toolbuttonSalir_clicked             (GtkToolButton   *toolbutton,
                                        gpointer         user_data);

gboolean
on_drawingarea1_configure_event        (GtkWidget       *widget,
                                        GdkEventConfigure *event,
                                        gpointer         user_data);

gboolean
on_drawingarea1_expose_event           (GtkWidget       *widget,
                                        GdkEventExpose  *event,
                                        gpointer         user_data);

void
on_aboutdialog_destroy                 (GtkObject       *object,
                                        gpointer         user_data);

#include <stdlib.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>

#ifndef __EQUALIZER_H
#define __EQUALIZER_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _EQData {
    gchar *name;
    gdouble value[10];
} EQData;

void core_set_eq_effect(gdouble *fq);
void eq_init();
void create_equalizer();
void set_equalizer(GtkAdjustment *adjustment, gpointer data);
void close_equalizer(GtkButton *widget, gpointer data);
void equalizer_combox_changed(GtkComboBox *widget, gpointer data);
gboolean eq_set_by_user(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer data) ;
void equalizer_load_setting(GtkButton *button, gpointer data);
void equalizer_save_setting(GtkButton *button, gpointer data) ;

#ifdef __cplusplus
};
#endif

#endif //__EQUALIZER_H

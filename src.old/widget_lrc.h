#include <gtk/gtk.h>
#include "common.h"

#ifndef __WIDGET_LRC_H
#define __WIDGET_LRC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iceplayer_widget_lrc {
    GtkWidget *window;
    GtkWidget *eventbox;
    GtkWidget *table;
    GtkWidget *fixed;
    GtkWidget *label;

    GtkWidget *top_eventbox;
    GtkWidget *top_image;
    GtkWidget *buttom_eventbox;
    GtkWidget *buttom_image;
    GtkWidget *left_eventbox;
    GtkWidget *left_image;
    GtkWidget *right_eventbox;
    GtkWidget *right_image;
    GtkWidget *top_left_eventbox;
    GtkWidget *top_left_image;
    GtkWidget *buttom_left_eventbox;
    GtkWidget *buttom_left_image;
    GtkWidget *top_right_eventbox;
    GtkWidget *top_right_image;
    GtkWidget *buttom_right_eventbox;
    GtkWidget *buttom_right_image;

    GtkWidget *back_eventbox;
    GtkWidget *back_image;
    GtkWidget *close_eventbox;
    GtkWidget *close_image;
    GtkWidget *min_eventbox;
    GtkWidget *min_image;
    GtkWidget *max_eventbox;
    GtkWidget *max_image;
    GtkWidget *above_eventbox;
    GtkWidget *above_image;
} iceplayer_widget_lrc;
void updata_widget_lrc_bg();
extern gchar cen_lrc[STRINGS_LENGTH];
extern cairo_surface_t *iceplayer_widget_lrc_image;
gchar * get_iceplayer_widget_lrc_back_pic();
extern iceplayer_widget_lrc widget_lrc;
extern gint lrc_line_number_to_show;

gboolean close_Independent();
gboolean min_Independent();
gboolean max_Independent();
void widget_menu(GtkWidget *widget,GdkEventButton *event);
gboolean resize_Independent (GtkWidget *widget,GdkEventButton *event,gint type);
gboolean widget_lrc_size_changed();
void widget_lrc_above();
void widget_lrc_Independent_init() ;
void widget_lrc_refresh(gboolean force);
void widget_lrc_init();
void update_widget_lrc_bg();
gboolean widget_lrc_show();

#ifdef __cplusplus
};
#endif

#endif //__WIDGET_LRC_H

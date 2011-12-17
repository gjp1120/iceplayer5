#include <gtk/gtk.h>

#ifndef __DESKTOP_LRC_H
#define __DESKTOP_LRC_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iceplayer_desktop_lrc {
    GtkWidget *window;
    GtkWidget *eventbox;
    gboolean show_lock_background;
} iceplayer_desktop_lrc;

extern iceplayer_desktop_lrc desktop_lrc;

void desktop_lrc_init() ;
gboolean desktop_lrc_show() ;

#ifdef __cplusplus
};
#endif

#endif //__DESKTOP_LRC_H

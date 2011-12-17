#include <gst/gst.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <dirent.h>
#include <glib.h>
#include <iconv.h>
#include <gdk/gdk.h>
#include <gdk/gdkwindow.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <glib/gtypes.h>
#include <gtk/gtkaccelgroup.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <libgen.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#ifndef __COMMON_H
#define __COMMON_H

#ifdef __cplusplus
#include <iostream>
extern std::string version;
extern "C" {
#endif

#define print_debug(X) g_print("Debug:%s\n",_(X));
  //考虑效率因素，还是不要在调试输出里使用gettext了
  //#define print_programming(X) //g_print("Programming:%s",_(X));
#define print_programming(X) //g_print("Programming:%s",(X));
#define dialog(X)								\
  {												\
    GtkWidget *dialog =							\
      gtk_message_dialog_new(NULL, GTK_DIALOG_DESTROY_WITH_PARENT,	\
			     GTK_MESSAGE_QUESTION,			\
			     GTK_BUTTONS_OK,_(X));			\
    gtk_dialog_run((GtkDialog *)dialog);			\
    gtk_widget_destroy(dialog);						\
  }

#define popup_menu(X,Y,Z)			\
{									\
  gtk_menu_popup (GTK_MENU(X),NULL,NULL,						\
				  NULL,NULL,3,gtk_get_current_event_time());	\
    gint x,y;								\
    gint w,h;								\
    gint sx,sy;								\
    gtk_window_get_size ((GtkWindow *)gtk_widget_get_toplevel((GtkWidget *)X), \
			 &w,&h);					\
    gdk_window_get_pointer(NULL, &x, &y, NULL);				\
    gtk_window_get_position((GtkWindow *)				\
			    gtk_widget_get_toplevel((GtkWidget *)X),	\
			    &sx, &sy);					\
    if (h >= (gdk_screen_height() - y))					\
      gtk_window_move((GtkWindow *)gtk_widget_get_toplevel((GtkWidget *)X), \
		      sx,sy-Z);						\
    else								\
      gtk_window_move((GtkWindow *)gtk_widget_get_toplevel((GtkWidget *)X), \
		      sx,sy+Z);						\
    if (w >= (gdk_screen_width() - x))					\
      gtk_window_move((GtkWindow *)gtk_widget_get_toplevel((GtkWidget *)X), \
		      sx-Y,sy);						\
    else								\
      gtk_window_move((GtkWindow *)gtk_widget_get_toplevel((GtkWidget *)X), \
		      sx+Y,sy);						\
  }

#define CONFIG_READ_FAILED				\
  if(error != NULL) {					\
    print_error("读取配置出错！%s\n",error->message);	\
    error = NULL;}

#define SKIN_CONFIG_READ_FAILED					\
  if(error != NULL) {						\
    print_error("读取皮肤配置出错！%s\n",error->message);	\
    return FALSE;}

#define G_SOURCE_FUNC(X) ((GSourceFunc) (X))

void print_error (const char *format, ...);

///fixme:使用enum类型来实现
///*/
#define RESIZE_TOP_LEFT             0
#define RESIZE_TOP                  1
#define RESIZE_TOP_RIGHT            2
#define RESIZE_LEFT                 3
#define RESIZE_RIGHT                4
#define RESIZE_BUTTOM_LEFT          5
#define RESIZE_BUTTOM               6
#define RESIZE_BUTTOM_RIGHT         7
#define RESIZE_NONE                 8
///*/

#define SWITCH_LIST                 0
#define SWITCH_LRC                  1
#define SWITCH_MUSICBOX             2
#define SWITCH_CONFIG               3
#define SWITCH_HELP                 4

#define CONTROL_NONE                5

#define CONTROL_MENU                6
#define CONTROL_MINI                7
#define CONTROL_MIN                 8
#define CONTROL_MAX                 9
#define CONTROL_CLOSE               10

#define CONTROL_PLAY                11
#define CONTROL_PAUSE               12
#define CONTROL_PREV                13
#define CONTROL_NEXT                14
#define CONTROL_STOP                15
#define CONTROL_OPEN                16
#define CONTROL_PROGRESS            17
#define CONTROL_VOLUME              18

#define WIDGET_LRC_CONTROL_ABOVE            19
#define WIDGET_LRC_CONTROL_MIN              20
#define WIDGET_LRC_CONTROL_MAX              21
#define WIDGET_LRC_CONTROL_CLOSE            22

#define WIDGET_LRC_RESIZE_TOP_LEFT             23
#define WIDGET_LRC_RESIZE_TOP                  24
#define WIDGET_LRC_RESIZE_TOP_RIGHT            25
#define WIDGET_LRC_RESIZE_LEFT                 26
#define WIDGET_LRC_RESIZE_RIGHT                27
#define WIDGET_LRC_RESIZE_BUTTOM_LEFT          28
#define WIDGET_LRC_RESIZE_BUTTOM               29
#define WIDGET_LRC_RESIZE_BUTTOM_RIGHT         30
#define WIDGET_LRC_RESIZE_NONE                 31

#define MINI_PLAY            32
#define MINI_PAUSE           33
#define MINI_PREV            34
#define MINI_NEXT            35
#define MINI_STOP            36
#define MINI_CLOSE           37
#define MINI_MINI            38
#define MINI_LISTS           39
#define MINI_SONGS           40

///*/
#define ADD_NONE            0

#define ADD_OPEN_BUTTON     1
#define ADD_FILES           2
#define ADD_FOLDER          3
///*/
#define PROGRESS_LEFT    0
#define PROGRESS_RIGHT   1
#define PROGRESS_NODE    2
///*/
#define LRC_SEARCH_WEB_WWW_LRC123_COM 0
///*/
#define MUSIC_WEB_MP3_BAIDU_COM       0
///*/
#define PLAY_MODE_NORMAL  0
#define PLAY_MODE_UNORMAL 1
#define PLAY_MODE_RADOM   2
#define PLAY_MODE_ONE     3
///*/
#define CONFIG_SW_NORMAL    0
#define CONFIG_SW_PLAY      1
#define CONFIG_SW_SKIN      2
#define CONFIG_SW_LRC       3
#define CONFIG_SW_MUSIC_BOX 4
#define CONFIG_SW_QUICK     5
///*/
#define WINDOWS_STATE_SHOW                 0
#define WINDOWS_STATE_HIDE                 GDK_WINDOW_STATE_WITHDRAWN
#define WINDOWS_STATE_ICONIFIED            GDK_WINDOW_STATE_ICONIFIED
#define WINDOWS_STATE_SHOW_ALL_WORK_PLACES 8
#define WINDOWS_STATE_HIDE_ALL_WORK_PLACES 10
///*/
#define WIDGET_LRC_SHOW_TYPE_INDEPENDENT 0
#define WIDGET_LRC_SHOW_TYPE_INTEGRATION 1
#define WIDGET_LRC_SHOW_TYPE_HPANED      2
///*/
#define STRINGS_LENGTH                   1536
///*/
#define SEARCH_LRC_STATE_NONE              0
#define SEARCH_LRC_STATE_SEARCH            1
#define SEARCH_LRC_STATE_NET_SOCKET        2
#define SEARCH_LRC_STATE_NET_DNS           3
#define SEARCH_LRC_STATE_NET_SOCKET_ERROR  4
#define SEARCH_LRC_STATE_NET_DNS_ERROR     5
#define SEARCH_LRC_STATE_NET_CONNECT_ERROR 6
#define SEARCH_LRC_STATE_SEARCHED          7
#define SEARCH_LRC_STATE_NOT_FOUND         8
#define SEARCH_LRC_STATE_DOWNLOADING       9
#define SEARCH_LRC_STATE_DOWNLOADED        10
#define SEARCH_LRC_STATE_CLOSE             11
#define SEARCH_LRC_STATE_NET_CONNECT       12
#define SEARCH_LRC_STATE_FAILED            13
///*/
#define SHORT_CUTS_SONGS              0
#define SHORT_CUTS_LISTS              1
#define SHORT_CUTS_ICEPLAYER          2

#include <glib/gi18n.h>
#include <libintl.h>
#define GETTEXT_PACKAGE "iceplayer"
#define LOCALEDIR "mo"
#define ICON  "@datadir@/iceplayer/iceplayer.png"

extern gchar verison[STRINGS_LENGTH];

GtkWidget *gtk_image_new_from_file_new(const char *dir,const char *picture);
GdkPixbuf *gdk_pixbuf_new_from_file_new(const char *dir,const char *picture);
void environment_init();
void gtk_fixed_move_new(GtkWidget *fixed,GtkWidget *widget,gint x,gint y);
void print_notify(const gchar *title, const gchar *text) ;
GdkPixbuf *gdk_pixbuf_set_alpha(GdkPixbuf *src, gint alpha);
cairo_surface_t *cairo_image_surface_create_from_png_new(cairo_surface_t* s,
							 gchar *uri);
gboolean ref_cairo_surface_t( cairo_surface_t  *h) ;
void g_free_n(gpointer er);

gchar *g_key_file_get_string_new(GKeyFile *key_file,
								 const gchar *group_name,
								 const gchar *key,
								 GError **error);

void gtk_container_remove_new (GtkContainer *container ,GtkWidget *widget);

#ifdef __cplusplus
};
#endif

#endif //__COMMON_H

#include <gtk/gtk.h>
#include "common.h"

#ifndef __SINGLE_H
#define __SINGLE_H


#ifdef __cplusplus
extern "C" {
#endif

extern cairo_surface_t *iceplayer_image;
extern gboolean in_add_dialog;
extern gboolean move_widget_lrc_window_with_main_window;
///some func:
gchar * get_iceplayer_back_pic();
gboolean on_iceplayer_expose();
void change_picture_on_press_and_release_event(GdkEventButton * event,gint type) ;
gboolean fit_size();
gboolean size_changed();
gboolean change_com();
void add_file(GSList *files,gchar lists_name[STRINGS_LENGTH]) ;
void add_folder(gchar lists_name[STRINGS_LENGTH],gchar path[STRINGS_LENGTH],gboolean cycle,gboolean new_lists);
gboolean was_music_file(gchar *d_name);
void add_dialog(GtkWidget *widget,gint type) ;
void add_menu();
void menu();
void update_focus_lists();
void update_lists_focus_song() ;
void update_focus_song_from_playing_song() ;
void update_focus_song_from_selection();
gchar *get_selection_song();
gchar *get_playing_song();
gboolean show() ;
gboolean hide(gpointer data);
gboolean window_state_change();
gboolean quit () ;
gboolean single_quit(GtkWidget *widget,GdkEvent *event,gpointer user_data);
gboolean resize (GtkWidget *widget,GdkEventButton *event,gint type);
gboolean change_cursor (GtkWidget *widget,GdkEventCrossing *event,gint type);
gboolean short_cuts (GtkWidget *widget,GdkEventKey *event,gint type ) ;
gboolean move (GtkWidget *widget,GdkEventButton *event,gpointer user_data);
gboolean single_menu (GtkWidget *widget,GdkEventButton *event,gpointer user_data) ;
gboolean change_picture (GtkWidget *widget,GdkEventCrossing *event,gint type);
gboolean above();
gboolean mini();
gboolean min (GtkWidget *widget,GdkEventButton *event,gpointer user_data) ;
gboolean max (GtkWidget *widget,GdkEventButton *event,gpointer user_data);
gboolean single_prev (GtkWidget *widget,GdkEventButton *event,gpointer user_data);
gboolean single_play (GtkWidget *widget,GdkEventButton *event,gpointer user_data);
gboolean single_next (GtkWidget *widget,GdkEventButton *event,gpointer user_data);
gboolean single_stop (GtkWidget *widget,GdkEventButton *event,gpointer user_data);
gboolean single_add (GtkWidget *widget,GdkEventButton *event,gint type);
gboolean jump_to_pos(gpointer data);
gboolean progress (GtkWidget *widget,GdkEventButton *event,gint type);
gboolean progress_scroll(GtkWidget *widget,GdkEventScroll *event,gpointer user_data) ;
gboolean change_volume(GtkWidget *vscale) ;
gboolean volume (GtkWidget *widget,GdkEventButton *event,gpointer user_data) ;
gboolean volume_scroll(GtkWidget *widget,GdkEventScroll *event,gpointer user_data) ;
gboolean lists_treview_change_cursor (GtkTreeView *treeview,gpointer user_data);
gboolean lists_treeview_click (GtkWidget *widget,GdkEventButton *event,gpointer user_data);
gboolean lists_treeview_drag_data_received (GtkWidget *widget,GdkDragContext *drag_context,gint x,gint y,GtkSelectionData *data,guint info,guint time,gpointer user_data);
gboolean lists_treeview_drag_end (GtkWidget *widget,GdkDragContext *drag_context,gpointer user_data) ;
gboolean songs_treeview_change_cursor (GtkTreeView *treeview,gpointer user_data);
gboolean songs_treeview_click (GtkWidget *widget,GdkEventButton *event,gpointer user_data);
void change_widget_lrc_ui(GtkWidget *widget,gint type);
gboolean switch_button (GtkWidget *widget,GdkEventButton *event,gint type) ;
gboolean main_window_menu();
gboolean  list_menu_jump_to_playing_song(gpointer data);
void list_menu_play_selection();
void list_menu_rename();
void list_menu_open_dir_of_song();
void list_menu_man_tools (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_open_with_audacious ();
void list_menu_open_with_amarok_item (GtkMenuItem *menuitem,gpointer user_data) ;
void list_menu_open_with_totem (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_deletet_selection (GtkMenuItem *menuitem,gboolean delete_to_trash);
void list_menu_delete_focus_lists (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_deletet_same_item(GtkMenuItem *menuitem,gpointer user_data);
void list_menu_delete_no_exist_item (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_real_delete (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_id3 (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_switch_to_lrc (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_show_desktop_lrc(GtkMenuItem *menuitem,gpointer user_data) ;
void list_menu_search_lrc (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_lock_desktop_lrc(GtkMenuItem *menuitem,gpointer user_data);
void list_menu_edit_lrc (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_sort_songs (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_sort_with_file_name (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_un_sort_with_file_name (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_sort_with_id3_editor (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_sort_with_id3_song_name (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_sort_with_radom(GtkMenuItem *menuitem,gpointer user_data);
void list_menu_play_mode(GtkMenuItem *menuitem,gint type);
void list_menu_change_play_mode_other_lists (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_search_local_song (GtkMenuItem *menuitem,gpointer user_data);
void list_menu_search_music_box(GtkMenuItem *menuitem,gpointer user_data);
void show_list_menu ();
void show_lists_menu();

#ifdef __cplusplus
};
#endif


#endif //__SINGLE_H

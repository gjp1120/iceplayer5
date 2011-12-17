#include <gtk/gtk.h>
#include "common.h"

#ifndef __LIST_H
#define __LIST_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iceplayer_songs
{
  struct iceplayer_songs* next;
  gchar uri[STRINGS_LENGTH];
} iceplayer_songs;

typedef struct iceplayer_lists
{
  struct iceplayer_lists* next;
  gchar name[STRINGS_LENGTH];
  gchar focus_song[STRINGS_LENGTH];
  struct iceplayer_songs* songs;
} iceplayer_lists;

typedef struct iceplayer_list_menu
{
  GtkWidget *list_menu_window;
  GtkWidget *list_menu_fixed;
  GtkWidget *list_menu_back_eventbox;
  GtkWidget *list_menu_back_image;
  GtkWidget *list_menu_vbox;
  GtkWidget *list_menu_hbox;
  GtkWidget *list_menu_prev_button;
  GtkWidget *list_menu_prev_button_image;
  GtkWidget *list_menu_play_button;
  GtkWidget *list_menu_play_button_image;
  GtkWidget *list_menu_next_button;
  GtkWidget *list_menu_next_button_image;
  GtkWidget *list_menu_stop_button;
  GtkWidget *list_menu_stop_button_image;
  GtkWidget *list_menu_open_button;
  GtkWidget *list_menu_open_button_image;
  GtkWidget *list_menu_menu_button;
  GtkWidget *list_menu_menu_button_image;
  GtkWidget *list_menu_hseparator_1;
  GtkWidget *list_menu_jump_to_playing_song_button;
  GtkWidget *list_menu_jump_to_playing_song_button_hbox;
  GtkWidget *list_menu_jump_to_playing_song_button_image;
  GtkWidget *list_menu_jump_to_playing_song_button_label;
  GtkWidget *list_menu_deal_with_playing_song_button;
  GtkWidget *list_menu_deal_with_playing_song_button_hbox;
  GtkWidget *list_menu_deal_with_playing_song_button_image;
  GtkWidget *list_menu_deal_with_playing_song_button_label;
  GtkWidget *list_menu_deal_with_playing_song_button_label2;
  GtkWidget *list_menu_hseparator_2;
  GtkWidget *list_menu_play_selection_button;
  GtkWidget *list_menu_play_selection_button_hbox;
  GtkWidget *list_menu_play_selection_button_image;
  GtkWidget *list_menu_play_selection_button_label;
  GtkWidget *list_menu_deal_with_selection_button;
  GtkWidget *list_menu_deal_with_selection_button_hbox;
  GtkWidget *list_menu_deal_with_selection_button_image;
  GtkWidget *list_menu_deal_with_selection_button_label;
  GtkWidget *list_menu_deal_with_selection_button_label2;
  GtkWidget *list_menu_delete_button;
  GtkWidget *list_menu_delete_button_hbox;
  GtkWidget *list_menu_delete_button_image;
  GtkWidget *list_menu_delete_button_label;
  GtkWidget *list_menu_delete_button_label2;
  GtkWidget *list_menu_id3_button;
  GtkWidget *list_menu_id3_button_hbox;
  GtkWidget *list_menu_id3_button_image;
  GtkWidget *list_menu_id3_button_label;
  GtkWidget *list_menu_hseparator_3;
  GtkWidget *list_menu_lrc_button;
  GtkWidget *list_menu_lrc_button_hbox;
  GtkWidget *list_menu_lrc_button_image;
  GtkWidget *list_menu_lrc_button_label;
  GtkWidget *list_menu_lrc_button_label2;
  GtkWidget *list_menu_list_button;
  GtkWidget *list_menu_list_button_hbox;
  GtkWidget *list_menu_list_button_image;
  GtkWidget *list_menu_list_button_label;
  GtkWidget *list_menu_list_button_label2;
  GtkWidget *list_menu_mode_button;
  GtkWidget *list_menu_mode_button_hbox;
  GtkWidget *list_menu_mode_button_image;
  GtkWidget *list_menu_mode_button_label;
  GtkWidget *list_menu_mode_button_label2;

} iceplayer_list_menu;
extern iceplayer_list_menu list_menu;
extern iceplayer_lists main_lists;//根列表
extern gchar focus_song[STRINGS_LENGTH];//所有列表中与focus_song相同的歌曲将高亮显示，播放时播放 focus_song。
extern gchar focus_lists[STRINGS_LENGTH];//所有列表中名字与focus_lists相同的列表将高亮显示
extern gchar playing_song[30];//正在播放的项目的 GtkTreePath 的 string
extern gchar playing_list[STRINGS_LENGTH];//正在播放的列表的名字
void init_lists();
void init_songs();
gboolean list_init_lists();
gchar *add_new_lists(gchar name[STRINGS_LENGTH]);
iceplayer_lists *find_lists_from_name(gchar name[STRINGS_LENGTH]);
void show_lists(gchar *new_list);
iceplayer_lists *find_lists_from_next(iceplayer_lists *lists);
void delete_lists(iceplayer_lists *lists);
gboolean show_songs_foreach(GtkTreeModel *model,GtkTreePath *path,GtkTreeIter *iter,gchar new_song[STRINGS_LENGTH]);
void show_songs(gchar new_song[STRINGS_LENGTH]);
gboolean  rename_lists(gchar old_name[STRINGS_LENGTH],gchar new_name[STRINGS_LENGTH]);
gchar *songs_uri_get_show_name(gchar *uri);
void on_treeview_drag_data_received (GtkWidget *widget, GdkDragContext *context,gint x, gint y, GtkSelectionData *selection_data, guint info, guint timestamp);
void update_songs_order();
iceplayer_lists *find_lists_from_random();
gboolean save_lists();
gboolean read_lists();
gchar *get_lists_file_path ();
const gchar *rename_songs(const gchar *old_uri, const gchar *new_name);

#ifdef __cplusplus
};
#endif

#endif //__LIST_H

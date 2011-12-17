#include <gtk/gtk.h>

#ifndef __UI_H
#define __UI_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iceplayer_skin{
gboolean use_default_skin;
gchar un_iceplayer_version[200];
gchar title[200];
gchar editor[200];
gchar version[200];
gchar time[200];
gchar link[200];
gchar about[2000];
///height不可拉长
gint main_height;
gint main_width;
///x负值为从右往左计,y负值为从下到上计
gint control_play_x;
gint control_play_y;
gint control_play_width;
gint control_play_height;

gint control_prev_x;
gint control_prev_y;
gint control_prev_width;
gint control_prev_height;

gint control_next_x;
gint control_next_y;
gint control_next_width;
gint control_next_height;

gint control_stop_x;
gint control_stop_y;
gint control_stop_width;
gint control_stop_height;

gint control_open_x;
gint control_open_y;
gint control_open_width;
gint control_open_height;
///x负值为从右往左计,y负值为从下到上计
gint control_close_x;
gint control_close_y;
gint control_close_width;
gint control_close_height;

gint control_max_x;
gint control_max_y;
gint control_max_width;
gint control_max_height;

gint control_mini_x;
gint control_mini_y;
gint control_mini_width;
gint control_mini_height;

gint control_min_x;
gint control_min_y;
gint control_min_width;
gint control_min_height;

gint control_above_x;
gint control_above_y;
gint control_above_width;
gint control_above_height;

gint control_menu_x;
gint control_menu_y;
gint control_menu_width;
gint control_menu_height;
///左上角的坐标和右下角的坐标，x负值为从右往左计,y负值为从下到上计
gint progress_left_top_x;
gint progress_left_top_y;
gint progress_right_buttom_x;
gint progress_right_buttom_y;
///音量
gint volume_x;
gint volume_y;
gint volume_width;
gint volume_height;
///迷你
gint mini_w;
gint mini_h;
gint mini_close_w;
gint mini_close_h;
gint mini_close_x;
gint mini_close_y;
gint mini_mini_w;
gint mini_mini_h;
gint mini_mini_x;
gint mini_mini_y;
gint mini_label_x;
gint mini_label_y;
gint mini_label_w;
gint mini_label_h;
gint mini_play_x;
gint mini_play_y;
gint mini_play_w;
gint mini_play_h;
gint mini_next_x;
gint mini_next_y;
gint mini_next_w;
gint mini_next_h;
gint mini_prev_x;
gint mini_prev_y;
gint mini_prev_w;
gint mini_prev_h;
gint mini_stop_x;
gint mini_stop_y;
gint mini_stop_w;
gint mini_stop_h;
gint mini_lists_x;
gint mini_lists_y;
gint mini_lists_w;
gint mini_lists_h;
gint mini_songs_x;
gint mini_songs_y;
gint mini_songs_w;
gint mini_songs_h;

gint info_name_x;
gint info_name_y;
gint info_time_x;
gint info_time_y;
}iceplayer_skin;
typedef struct iceplayer_widget{
  gint switch_type;
  GtkWidget *window;
  GtkWidget *eventbox;
  GtkStatusIcon *trayIcon;
  GtkWidget *hpaned;
  GtkWidget *fixed;
  GtkWidget *back_eventbox_main;
  GtkWidget *back_eventbox_widget_lrc_hpaned;
  GtkWidget *back_eventbox_widget_lrc_integration;
  GtkWidget *back_eventbox_switch;
  GtkWidget *back_eventbox_music_box;
  GtkWidget *back_eventbox_configure;
  GtkWidget *back_eventbox_help;
  GtkWidget *back_image;
  GtkWidget *table;
  GtkWidget *notebook;
  GtkWidget *main_fixed;
  GtkWidget *menu_eventbox;
  GtkWidget *menu_image;
  GtkWidget *mini_fixed;
  GtkWidget *mini_eventbox;
  GtkWidget *mini_image;
  GtkWidget *min_eventbox;
  GtkWidget *min_image;
  GtkWidget *max_eventbox;
  GtkWidget *max_image;
  GtkWidget *close_eventbox;
  GtkWidget *close_image;
  GtkWidget *above_eventbox;
  GtkWidget *above_image;
  GtkWidget *prev_eventbox;
  GtkWidget *prev_image;
  GtkWidget *play_eventbox;
  GtkWidget *play_image;
  GtkWidget *next_eventbox;
  GtkWidget *next_image;
  GtkWidget *stop_eventbox;
  GtkWidget *stop_image;
  GtkWidget *open_eventbox;
  GtkWidget *open_image;

  GtkWidget *progress_hbox;

  GtkWidget *progress_left_eventbox;
  GtkWidget *progress_left_image;

  GtkWidget *progress_node_eventbox;
  GtkWidget *progress_node_image;

  GtkWidget *progress_right_eventbox;
  GtkWidget *progress_right_image;

  GtkWidget *name_label;
  GtkWidget *time_label;

  GtkWidget *volume_eventbox;
  GtkWidget *volume_image;

  GtkWidget *list_hpaned;
  GtkWidget *list_lists_scrolledwindow;
  GtkWidget *list_lists_treeview;
  GtkWidget *list_songs_scrolledwindow;
  GtkWidget *list_songs_treeview;

  GtkWidget *widget_lrc_label;
  GtkWidget *widget_lrc_label_hpaned;
  GtkWidget *widget_lrc_scrolledwindow;
  GtkWidget *widget_lrc_scrolledwindow_hpaned;
  GtkWidget *widget_lrc_fixed;
  GtkWidget *widget_lrc_fixed_hpaned;
  GtkWidget *widget_lrc_backimage;
  GtkWidget *widget_lrc_backimage_hpaned;

  GtkWidget *switch_hbox;

  GtkWidget *switch_list_eventbox;
  GtkWidget *switch_list_image;

  GtkWidget *switch_lrc_eventbox;
  GtkWidget *switch_lrc_image;

  GtkWidget *switch_music_box_eventbox;
  GtkWidget *switch_music_box_image;

  GtkWidget *switch_configure_eventbox;
  GtkWidget *switch_configure_image;

  GtkWidget *switch_help_eventbox;
  GtkWidget *switch_help_image;

  GtkWidget *resize_left_image;
  GtkWidget *resize_left_eventbox;

  GtkWidget *resize_top_eventbox;
  GtkWidget *resize_top_image;

  GtkWidget *resize_buttom_eventbox;
  GtkWidget *resize_buttom_image;

  GtkWidget *resize_right_eventbox;
  GtkWidget *resize_right_image;

  GtkWidget *resize_buttom_left_image;
  GtkWidget *resize_buttom_left_eventbox;

  GtkWidget *resize_buttom_right_image;
  GtkWidget *resize_buttom_right_eventbox;

  GtkWidget *resize_top_right_image;
  GtkWidget *resize_top_right_eventbox;

  GtkWidget *resize_top_left_image;
  GtkWidget *resize_top_left_eventbox;

  GtkWidget *mini_window;
  GtkWidget *mini_win_eventbox;
  GtkWidget *mini_mini_eventbox;
  GtkWidget *mini_mini_image;
  GtkWidget *mini_close_eventbox;
  GtkWidget *mini_close_image;
  GtkWidget *mini_prev_eventbox;
  GtkWidget *mini_prev_image;
  GtkWidget *mini_play_eventbox;
  GtkWidget *mini_play_image;
  GtkWidget *mini_next_eventbox;
  GtkWidget *mini_next_image;
  GtkWidget *mini_stop_eventbox;
  GtkWidget *mini_stop_image;
  GtkWidget *mini_songs_eventbox;
  GtkWidget *mini_songs_image;
  GtkWidget *mini_lists_eventbox;
  GtkWidget *mini_lists_image;

}iceplayer_widget;

extern iceplayer_skin skin;
extern iceplayer_widget iceplayer;
extern gint windows_state;
void updata_iceplayer_bg() ;
void ui_init() ;
void ui_init_skin();
gboolean progress_refresh();
gchar *get_skin_dir();
void tray_menu();
void skin_do() ;
void skin_del();
void skin_add();
void skin_info() ;
void progress_refresh_init() ;
gboolean update_iceplayer_bg();
gboolean read_skin_configure();
void set_skin_configure_to_iceplayer_widget();

#ifdef __cplusplus
};
#endif


#endif //__UI_H

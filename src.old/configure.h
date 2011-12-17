#include "common.h"

#ifndef __CONFIGURE_H
#define __CONFIGURE_H

#ifdef __cplusplus
extern "C" {
#endif

#define CONFIG_READ_FAILED_int(X,Y) if(error!=NULL){error=NULL;X=Y;}
#define CONFIG_READ_FAILED_dou(X,Y) if(error!=NULL){error=NULL;X=Y;}
#define CONFIG_READ_FAILED_str(X,Y) if(error!=NULL){error=NULL;strcpy(X,Y);}

typedef struct iceplayer_config_widget {
    GtkWidget *sw_vbox;
    GtkWidget *sw_hbox;
    GtkWidget *sw_normal_button;
    GtkWidget *sw_play_button;
    GtkWidget *sw_skin_button;
    GtkWidget *sw_lrc_button;
    GtkWidget *sw_music_button;
    GtkWidget *sw_quick_button;

    GtkWidget *notebook;
    GtkWidget *normal_table;
    GtkWidget *auto_update_checkbutton;
    GtkWidget *show_in_taskbar_checkbutton;
    GtkWidget *decorated;
    GtkWidget *show_tray_checkbutton;
    GtkWidget *always_above_checkbutton;
    GtkWidget *show_in_all_work_places_checkbutton;
    GtkWidget *back_pic_entry;
    GtkWidget *back_pic_label;
    GtkWidget *lists_fontbutton;
    GtkWidget *lists_front_color_button;
    GtkWidget *lists_back_color_button;
    GtkWidget *info_name_fontbutton;
    GtkWidget *info_time_fontbutton;
    GtkWidget *label14;
    GtkWidget *label4;
    GtkWidget *vbox1;
    GtkWidget *open_and_play_checkbutton;
    GtkWidget *notify_checkbutton;
    GtkWidget *last_pos_checkbutton;
    GtkWidget *open_and_min_checkbutton;
    GtkWidget *failed_and_stop_checkbutton;
    GtkWidget *auto_jump_to_playing_checkbutton;
    GtkWidget *label15;
    GtkWidget *label5;
    GtkWidget *hbox1;
    GtkWidget *table3;
    GtkWidget *skin_do_button;
    GtkWidget *delete_button;
    GtkWidget *skin_add_button;
    GtkWidget *skin_info_button;
    GtkWidget *skin_scrolledwindow;
    GtkWidget *skin_treeview;
    GtkWidget *label16;
    GtkWidget *label6;
    GtkWidget *lrc_notebook;
    GtkWidget *table4;
    GtkWidget *lrc_scrolledwindow;
    GtkWidget *use_lrc_checkbutton;
    GtkWidget *use_search_lrc_checkbutton;
    GtkWidget *search_lrc_time_label;
    GtkWidget *search_lrc_time_entry;
    GtkWidget *search_lrc_web_button;
    GtkWidget *label21;
    GtkWidget *local_lrc_path_3_entry;
    GtkWidget *local_lrc_path_2_entry;
    GtkWidget *local_lrc_path_1_entry;
    GtkWidget *search_lrc_key_word_button;
    GtkWidget *search_lrc_key_word_help_button;
    GtkWidget *label20;
    GtkWidget *label19;
    GtkWidget *label18;
    GtkWidget *search_lrc_with_filename_and_id3_checkbutton;
    GtkWidget *lrc_download_path_entry;
    /*GtkWidget *download_lrc_path_file_button;
    GtkWidget *local_lrc_path_2_button;
    GtkWidget *local_lrc_path_1_button;
    GtkWidget *local_lrc_path_3_button;*/
    GtkWidget *hseparator5;
    GtkWidget *edit_search_lrc_checkbutton;
    GtkWidget *label10;
    GtkWidget *table5;
    GtkWidget *label22;
    GtkWidget *label23;
    GtkWidget *label24;
    GtkWidget *widget_lrc_colorbutton_1;
    GtkWidget *widget_lrc_colorbutton_2;
    GtkWidget *widget_lrc_fontbutton;
    GtkWidget *widget_back_pic_entry;
    GtkWidget *widget_back_pic_label;
    GtkWidget *label11;
    GtkWidget *table6;
    GtkWidget *desktop_lrc_colorbutton_4;
    GtkWidget *desktop_lrc_colorbutton_5;
    GtkWidget *use_desktop_lrc_checkbutton;
    GtkWidget *desktop_lrc_lock_checkbutton;
    GtkWidget *desktop_lrc_use_kalaok_checkbutton;
    GtkWidget *desktop_lrc_above_checkbutton;
    GtkWidget *desktop_lrc_word_stroke_entry;
    GtkWidget *desktop_lrc_word_stroke_label;
    GtkWidget *lists_use_system_theme_checkbutton;
    GtkWidget *desktop_lrc_word_stroke_colorbutton;
    GtkWidget *desktop_lrc_word_stroke_colorbutton_label;
    GtkWidget *label31;
    GtkWidget *label30;
    GtkWidget *label29;
    GtkWidget *label28;
    GtkWidget *hseparator1;
    GtkWidget *label27;
    GtkWidget *label26;
    GtkWidget *hseparator2;
    GtkWidget *label25;
    GtkWidget *hseparator3;
    GtkWidget *desktop_lrc_colorbutton_1;
    GtkWidget *desktop_lrc_colorbutton_2;
    GtkWidget *desktop_lrc_colorbutton_3;
    GtkWidget *desktop_lrc_colorbutton_6;
    GtkWidget *hseparator4;
    GtkWidget *desktop_lrc_fontbutton;
    GtkWidget *desktop_lrc_label;
    GtkWidget *label12;
    GtkWidget *hseparator_lists;
    GtkWidget *label7;
    GtkWidget *table7;
    GtkWidget *label33;
    GtkWidget *music_download_path_entry;
    GtkWidget *music_download_path_button;
    GtkWidget *music_box_web_button;
    GtkWidget *label8;
    GtkWidget *table8;
    GtkWidget *label35;
    GtkWidget *label37;
    GtkWidget *label38;
    GtkWidget *label39;
    GtkWidget *label40;
    GtkWidget *label41;
    GtkWidget *label42;
    GtkWidget *label43;
    GtkWidget *label44;
    GtkWidget *label45;
    GtkWidget *label46;
    GtkWidget *label47;
    GtkWidget *label48;
    GtkWidget *label49;
    GtkWidget *label50;
    GtkWidget *label51;
    GtkWidget *label52;
    GtkWidget *info_name_entry_x;
    GtkWidget *info_name_entry_y;
    GtkWidget *info_time_entry_x;
    GtkWidget *info_time_entry_y;
    GtkWidget *quick_shortcut_entry_1;
    GtkWidget *quick_shortcut_entry_2;
    GtkWidget *quick_shortcut_entry_3;
    GtkWidget *quick_shortcut_entry_4;
    GtkWidget *quick_shortcut_entry_5;
    GtkWidget *quick_shortcut_entry_6;
    GtkWidget *quick_shortcut_entry_7;
    GtkWidget *label34;
    GtkWidget *window_alpha_hscale;
    GtkWidget *desktop_lrc_read_config_button;
    GtkWidget *desktop_lrc_wirte_config_button;
} iceplayer_config_widget;

typedef struct iceplayer_config {
///
    gint x,y,width,height;
    gint list_hpaned_pos;
    gint hpaned_pos;
    gdouble volume;
    gint show_page;
    gint play_mode;
    gboolean auto_jump_other_lists;
    gchar back_image[STRINGS_LENGTH];
///
    gboolean lists_use_system_theme;
    gchar lists_font[50];
    gchar lists_front_color[50];
    gchar lists_back_color[50];
    gchar info_name_font[50];
    gchar info_time_font[50];
///
    gint widget_lrc_show_type ;
    gint widget_lrc_Independent_x;
    gint widget_lrc_Independent_y;
    gint widget_lrc_Independent_width;
    gint widget_lrc_Independent_height;
    gboolean widget_lrc_Independent_above;
    gchar widget_lrc_Independent_back_image[STRINGS_LENGTH];
///
    gboolean auto_update;
    gboolean task_bar;
    gboolean decorated;
    gboolean above;
    gboolean tray;
    gboolean all_work_places;

    gboolean last_pos;
    gdouble last_pos_value;
    gboolean begin_min;
    gboolean begin_play;
    gboolean notify;
    gboolean failed_stop;
    gboolean auto_jump_to_play;
    gdouble eq[10];

    gchar skin[STRINGS_LENGTH];

    gboolean lrc;
    gboolean search_lrc;
    gchar search_lrc_time[STRINGS_LENGTH];
    gint web_site_to_search;
    gboolean search_filename_or_id3;
    gboolean edit_search_word;
    gchar download_lrc_path[STRINGS_LENGTH];
    gchar search_lrc_path_1[STRINGS_LENGTH];
    gchar search_lrc_path_2[STRINGS_LENGTH];
    gchar search_lrc_path_3[STRINGS_LENGTH];

    gchar widget_lrc_font[50];
    gchar widget_lrc_front_color[50];
    gchar widget_lrc_back_color[50];

    gboolean desktop_lrc;
    gboolean desktop_lrc_kalaok;
    gboolean desktop_lrc_above;
    gint desktop_lrc_x ;
    gint desktop_lrc_y ;
    gint desktop_lrc_width ;
    gint desktop_lrc_height ;
    gboolean desktop_lrc_lock ;
    gchar desktop_lrc_font[50];
    gchar desktop_lrc_front_up_color[50];
    gchar desktop_lrc_front_cen_color[50];
    gchar desktop_lrc_front_down_color[50];
    gchar desktop_lrc_back_up_color[50];
    gchar desktop_lrc_back_cen_color[50];
    gchar desktop_lrc_back_down_color[50];
    gboolean desktop_lrc_one_line_or_two_line;
    gint desktop_lrc_word_stroke;
    gchar desktop_lrc_word_stroke_color[50];

    gint music_web_site;
    gchar download_path[STRINGS_LENGTH];

    gchar quick_show_and_hide[50];
    gchar quick_play_and_pause [50];
    gchar quick_previous [50];
    gchar quick_next [50];
    gchar quick_stop [50];
    gchar quick_quit [50];
    gchar quick_research_lrc[50];

    gint mini;
    gint mini_x;
    gint mini_y;
    gint mini_back[STRINGS_LENGTH];
    gchar mini_font[50];

    gint info_name_x;
    gint info_time_x;
    gint info_name_y;
    gint info_time_y;

    gdouble window_alpha;
} iceplayer_config;

extern iceplayer_config configure;
extern iceplayer_config_widget configure_widget;

void config_init_variable();//读取配置中的变量
void config_ui_init();
gboolean config_read() ;
gboolean config_wirte() ;
void skin_menu();
gboolean change_alpha(GtkWidget *hscale) ;
gboolean configure_to_iceplayer_widget() ;
gboolean configure_wirte();
gchar *get_configure_path();
gboolean update_configure();
void configure_init_variable();
void configure_init();
void configure_ui_init();

#ifdef __cplusplus
};
#endif


#endif //__CONFIGURE_H

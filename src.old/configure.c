#include <gtk/gtk.h>
#include "configure.h"
#include "common.h"
#include "single.h"
#include "widget_lrc.h"
#include "ui.h"
#include "desktop_lrc.h"
#include "lrc.h"
#include "list.h"
#include "mini.h"

iceplayer_config configure;
iceplayer_config_widget configure_widget;

gboolean change_alpha(GtkWidget *hscale) {
    print_programming(("change_alpha\n"));
    static gboolean lock = FALSE;
    if (lock == TRUE)
        return FALSE;
    lock = TRUE;

    if (!gdk_screen_is_composited(gdk_screen_get_default ())) {
        return FALSE;
    }

    configure.window_alpha = gtk_range_get_value(GTK_RANGE(hscale));
    iceplayer_image = cairo_image_surface_create_from_png_new(iceplayer_image,
							      get_iceplayer_back_pic());

    if (configure.widget_lrc_show_type != WIDGET_LRC_SHOW_TYPE_INTEGRATION
	&& configure.widget_lrc_show_type != WIDGET_LRC_SHOW_TYPE_HPANED)
      iceplayer_widget_lrc_image =					\
	cairo_image_surface_create_from_png_new(iceplayer_widget_lrc_image,
						get_iceplayer_widget_lrc_back_pic());

    gtk_widget_queue_draw(iceplayer.window);
    if (GTK_IS_WIDGET(widget_lrc.window))
      gtk_widget_queue_draw(widget_lrc.window);
    lock = FALSE;
    return FALSE;
}

gchar *get_configure_path() {
    print_programming(("get_configure_path\n"));
    static gchar configure_path[120];
    if (!configure_path[0]) {
        strcpy(configure_path,g_get_home_dir());
        strcat(configure_path,"/.iceplayer/configure.conf");//-");
        //strcat(configure_path,verison);
        //strcat(configure_path,".conf");
    }
    return configure_path;
}

gboolean update_configure() {
    print_programming(("update_configure\n"));

    if (GTK_IS_WIDGET(iceplayer.window) &&
	(windows_state == WINDOWS_STATE_SHOW ||
	 windows_state == WINDOWS_STATE_SHOW_ALL_WORK_PLACES))
      {
        gtk_window_get_size(GTK_WINDOW(iceplayer.window),
			    &configure.width,&configure.height);
        gtk_window_get_position(GTK_WINDOW(iceplayer.window),
				&configure.x,&configure.y);
      }

    if (GTK_IS_WIDGET(desktop_lrc.window)&&configure.desktop_lrc) {
      gtk_window_get_size(GTK_WINDOW(desktop_lrc.window),
			  &configure.desktop_lrc_width,
			  &configure.desktop_lrc_height);
      gtk_window_get_position(GTK_WINDOW(desktop_lrc.window),
			      &configure.desktop_lrc_x,
			      &configure.desktop_lrc_y);
    }

    configure.list_hpaned_pos = \
      gtk_paned_get_position(GTK_PANED(iceplayer.list_hpaned));
    configure.hpaned_pos = gtk_paned_get_position(GTK_PANED(iceplayer.hpaned));

    configure.auto_update = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.auto_update_checkbutton));
    configure.task_bar = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.show_in_taskbar_checkbutton));
    configure.decorated = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.decorated));
    configure.tray = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.show_tray_checkbutton));
    configure.above = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.always_above_checkbutton));
    configure.all_work_places = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.show_in_all_work_places_checkbutton));
    configure.begin_min = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.open_and_min_checkbutton));
    configure.last_pos = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.last_pos_checkbutton));
    configure.begin_play = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.open_and_play_checkbutton));
    configure.notify = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.notify_checkbutton));
    configure.failed_stop = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.failed_and_stop_checkbutton));
    configure.auto_jump_to_play = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.auto_jump_to_playing_checkbutton));
    configure.lrc = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.use_lrc_checkbutton));
    gboolean selr = configure.search_lrc ;
    configure.search_lrc = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.use_search_lrc_checkbutton));
    if (selr!=configure.search_lrc&&configure.search_lrc==TRUE)
        lrc();
    strcpy(configure.search_lrc_time,
	   gtk_entry_get_text(GTK_ENTRY(configure_widget.search_lrc_time_entry)));
    configure.search_filename_or_id3 = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.search_lrc_with_filename_and_id3_checkbutton));
    configure.edit_search_word = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.edit_search_lrc_checkbutton));
    configure.desktop_lrc = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.use_desktop_lrc_checkbutton));
    configure.desktop_lrc_lock = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.desktop_lrc_lock_checkbutton));
    configure.desktop_lrc_kalaok = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.desktop_lrc_use_kalaok_checkbutton));
    configure.desktop_lrc_above = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.desktop_lrc_above_checkbutton));
    configure.lists_use_system_theme = \
      gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(configure_widget.lists_use_system_theme_checkbutton));

    strcpy(configure.download_lrc_path,
	   gtk_entry_get_text(GTK_ENTRY(configure_widget.lrc_download_path_entry)));
    strcpy(configure.search_lrc_path_1,
	   gtk_entry_get_text(GTK_ENTRY(configure_widget.local_lrc_path_1_entry)));
    strcpy(configure.search_lrc_path_2,
	   gtk_entry_get_text(GTK_ENTRY(configure_widget.local_lrc_path_2_entry)));
    strcpy(configure.search_lrc_path_3,
	   gtk_entry_get_text(GTK_ENTRY(configure_widget.local_lrc_path_3_entry)));

    strcpy(configure.info_name_font,
	   gtk_font_button_get_font_name(GTK_FONT_BUTTON(configure_widget.info_name_fontbutton)));
    strcpy(configure.info_time_font,
	   gtk_font_button_get_font_name(GTK_FONT_BUTTON(configure_widget.info_time_fontbutton)));

    GdkColor color;
    strcpy(configure.widget_lrc_font,
	   gtk_font_button_get_font_name(GTK_FONT_BUTTON(configure_widget.widget_lrc_fontbutton)));
    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.widget_lrc_colorbutton_1), &color);
    strcpy(configure.widget_lrc_front_color,
	   gtk_color_selection_palette_to_string(&color,1));
    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.widget_lrc_colorbutton_2), &color);
    strcpy(configure.widget_lrc_back_color,
	   gtk_color_selection_palette_to_string(&color,1));

    strcpy(configure.desktop_lrc_font,
	   gtk_font_button_get_font_name(GTK_FONT_BUTTON(configure_widget.desktop_lrc_fontbutton)));

    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_1), &color);
    strcpy(configure.desktop_lrc_front_up_color,
	   gtk_color_selection_palette_to_string(&color,1));
    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_2), &color);
    strcpy(configure.desktop_lrc_front_cen_color,
	   gtk_color_selection_palette_to_string(&color,1));
    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_3), &color);
    strcpy(configure.desktop_lrc_front_down_color,
	   gtk_color_selection_palette_to_string(&color,1));
    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_4), &color);
    strcpy(configure.desktop_lrc_back_up_color,
	   gtk_color_selection_palette_to_string(&color,1));
    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_5), &color);
    strcpy(configure.desktop_lrc_back_cen_color,
	   gtk_color_selection_palette_to_string(&color,1));
    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_6), &color);
    strcpy(configure.desktop_lrc_back_down_color,
	   gtk_color_selection_palette_to_string(&color,1));
    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_word_stroke_colorbutton), &color);
    strcpy(configure.desktop_lrc_word_stroke_color,
	   gtk_color_selection_palette_to_string(&color,1));
    configure.desktop_lrc_word_stroke = \
      atoi(gtk_entry_get_text(GTK_ENTRY(configure_widget.desktop_lrc_word_stroke_entry)));

    strcpy(configure.lists_font,
	   gtk_font_button_get_font_name(GTK_FONT_BUTTON(configure_widget.lists_fontbutton)));
    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.lists_back_color_button), &color);
    strcpy(configure.lists_back_color,
	   gtk_color_selection_palette_to_string(&color,1));
    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.lists_front_color_button), &color);
    strcpy(configure.lists_front_color,
	   gtk_color_selection_palette_to_string(&color,1));

    strcpy( configure.download_path, gtk_entry_get_text(GTK_ENTRY(configure_widget.music_download_path_entry)));

    strcpy(configure.quick_show_and_hide, gtk_entry_get_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_1)));
    strcpy(configure.quick_play_and_pause, gtk_entry_get_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_2)));
    strcpy(configure.quick_previous, gtk_entry_get_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_3)));
    strcpy(configure.quick_next, gtk_entry_get_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_4)));
    strcpy(configure.quick_stop, gtk_entry_get_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_5 )));
    strcpy(configure.quick_quit, gtk_entry_get_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_6)));
    strcpy(configure.quick_research_lrc, gtk_entry_get_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_7)));
    strcpy(configure.widget_lrc_Independent_back_image, gtk_entry_get_text(GTK_ENTRY(configure_widget.widget_back_pic_entry )));
    strcpy(configure.back_image, gtk_entry_get_text(GTK_ENTRY(configure_widget.back_pic_entry)));

    g_timeout_add(5, configure_to_iceplayer_widget, NULL);
    return FALSE;
}


//type 1=read 0=wirte
gchar *get_desk_config_path(gint type) {
    print_programming(("get_desk_config_path\n"));
    GtkWidget *filechooserdialog;
    GtkWidget *dialog_vbox;
    GtkWidget *hbox1;
    GtkWidget *dialog_action_area;
    GtkWidget *button1;
    GtkWidget *button2;

    if (type == 1) {
        filechooserdialog = \
	  gtk_file_chooser_dialog_new("", NULL,
				      GTK_FILE_CHOOSER_ACTION_OPEN, NULL,NULL);
    } else
        filechooserdialog = \
	  gtk_file_chooser_dialog_new("", NULL,
				      GTK_FILE_CHOOSER_ACTION_SAVE, NULL,NULL);
    gtk_container_set_border_width(GTK_CONTAINER(filechooserdialog), 5);
    gtk_window_set_position(GTK_WINDOW(filechooserdialog), GTK_WIN_POS_CENTER);
    gtk_window_set_type_hint(GTK_WINDOW(filechooserdialog),
			     GDK_WINDOW_TYPE_HINT_DIALOG);
    GdkPixbuf *IcePlayer_icon_pixbuf = gdk_pixbuf_new_from_file(ICON, NULL);
    gtk_window_set_icon(GTK_WINDOW(filechooserdialog),
			(GdkPixbuf *)IcePlayer_icon_pixbuf);
    gtk_window_set_keep_above(GTK_WINDOW (filechooserdialog),TRUE);
    dialog_vbox = GTK_DIALOG(filechooserdialog)->vbox;
    gtk_widget_show(dialog_vbox);
    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox1);
    gtk_box_pack_start(GTK_BOX(dialog_vbox), hbox1, FALSE, FALSE, 0);

    dialog_action_area = GTK_DIALOG(filechooserdialog)->action_area;
    gtk_widget_show(dialog_action_area);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area),
			      GTK_BUTTONBOX_END);
    button1 = gtk_button_new_from_stock("gtk-cancel");
    gtk_widget_show(button1);
    gtk_dialog_add_action_widget(GTK_DIALOG(filechooserdialog),
				 button1, GTK_RESPONSE_CANCEL);
    GTK_WIDGET_SET_FLAGS(button1, GTK_CAN_DEFAULT);
    button2 = gtk_button_new_from_stock("gtk-open");
    gtk_widget_show(button2);
    gtk_dialog_add_action_widget(GTK_DIALOG(filechooserdialog),
				 button2, GTK_RESPONSE_OK);
    GTK_WIDGET_SET_FLAGS(button2, GTK_CAN_DEFAULT);
    gtk_widget_grab_default(button2);
    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, _("Config File"));
    gtk_file_filter_add_pattern(filter,"*.[Cc][Oo][Nn][Ff]");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooserdialog),filter);

    static gchar * path_used= NULL;
    /*下次打开对话框时便回到上一文件夹*/
    if (path_used)
      gtk_file_chooser_set_current_folder((GtkFileChooser *) filechooserdialog,
					  path_used);
    gtk_window_set_focus (GTK_WINDOW(filechooserdialog),button2);

    in_add_dialog = TRUE;
    gint aw = gtk_dialog_run(GTK_DIALOG(filechooserdialog));

    path_used = \
      gtk_file_chooser_get_current_folder((GtkFileChooser *) filechooserdialog);

    if (aw == -5) {
        static gchar path[STRINGS_LENGTH];
        strcpy(path,
	       gtk_file_chooser_get_filename((GtkFileChooser *)
					     filechooserdialog));
        gtk_widget_destroy(filechooserdialog);
        return path;
    } else {
        gtk_widget_destroy(filechooserdialog);
        return NULL;
    }

}

gboolean wirte_desktop_lrc_config() {
    print_programming(("wirte_desktop_lrc_config\n"));
    GKeyFile *keyfile;
    gchar *file_buf = NULL;
    gsize length;
    gint wirte=0;
    gchar *uri=NULL;

    uri = get_desk_config_path(wirte);
    if (uri == NULL)
        return FALSE;
    if (!strstr(uri,".conf"))
        strcat(uri,".conf");
    FILE *file=fopen(uri,"r");
    if (file) {
        fclose(file);
        GtkWidget *dialog = \
	  gtk_message_dialog_new(NULL,
				 GTK_DIALOG_DESTROY_WITH_PARENT,
				 GTK_MESSAGE_QUESTION,
				 GTK_BUTTONS_OK_CANCEL,
				 _("File is exist, overwrite?"));
        gint res = gtk_dialog_run((GtkDialog *)dialog);
        gtk_widget_destroy(dialog);
        //gtk_widget_destroy(dialog);
        if (res!=-5) {
            return FALSE;
        }
    }
    file=fopen(uri,"w");
    fputs("[DeskTop Lyric]\n",file);
    fclose(file);
    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile, uri, 0, NULL)) {
        print_error("保存失败，可能是权限问题\n");
        g_key_file_free(keyfile);
        return FALSE;
    } else {
///
        g_key_file_set_integer(keyfile, "桌面歌词",
			       "使用桌面歌词", configure.desktop_lrc);
        g_key_file_set_integer(keyfile, "桌面歌词",
			       "锁定桌面歌词", configure.desktop_lrc_lock);
        g_key_file_set_integer(keyfile, "桌面歌词",
			       "桌面歌词卡啦OK效果", configure.desktop_lrc_kalaok);
        g_key_file_set_integer(keyfile, "桌面歌词",
			       "桌面歌词最前", configure.desktop_lrc_above);
        g_key_file_set_integer(keyfile, "桌面歌词",
			       "桌面歌词X", configure.desktop_lrc_x   );
        g_key_file_set_integer(keyfile, "桌面歌词",
			       "桌面歌词Y", configure.desktop_lrc_y );
        g_key_file_set_integer(keyfile, "桌面歌词",
			       "桌面歌词Width", configure.desktop_lrc_width );
        g_key_file_set_integer(keyfile, "桌面歌词",
			       "桌面歌词Height", configure.desktop_lrc_height );
        g_key_file_set_string(keyfile, "桌面歌词",
			      "桌面歌词字体",  configure.desktop_lrc_font   );
        g_key_file_set_string(keyfile, "桌面歌词", "桌面歌词前景色一",
			      configure.desktop_lrc_front_up_color);
        g_key_file_set_string(keyfile, "桌面歌词", "桌面歌词前景色二",
			      configure.desktop_lrc_front_cen_color);
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词前景色三",
			      configure.desktop_lrc_front_down_color  );
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词背景色一",
			      configure.desktop_lrc_back_up_color  );
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词背景色二",
			      configure.desktop_lrc_back_cen_color);
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词背景色三",
			      configure.desktop_lrc_back_down_color  );
        g_key_file_set_integer(keyfile, "桌面歌词","桌面歌词字体描边宽度",
			       configure.desktop_lrc_word_stroke );
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词字体描边颜色",
			      configure.desktop_lrc_word_stroke_color  );

        file_buf = g_key_file_to_data(keyfile, &length, NULL);
        g_file_set_contents(uri, file_buf, -1, NULL);

        g_key_file_free(keyfile);
        g_free_n(file_buf);
    }
    return TRUE;
}

//fixme:1.应只在安装时进行搜索并写入配置文件，代码中应从配置文件中读取该设置
//      2.目前无法检测/usr/local目录中的程序
void edit_search_lrc_key() {
    gchar *editor;
    FILE *file = fopen("/usr/bin/gedit","r");

    if(file) {
        fclose(file);
        editor="gedit";
    } else {
        file = fopen("/usr/bin/kedit","r");
        if(file) {
            fclose(file);
            editor="kedit";
        } else {
            file = fopen("/usr/bin/mousepad","r");
            if(file) {
                fclose(file);
                editor="mousepad";
            } else {
                file = fopen("/usr/bin/leafpad","r");
                if(file) {
                    fclose(file);
                    editor="leafpad";
                } else {
                    editor=NULL;
                }
            }
        }
    }

 if(editor!=NULL){
    gchar *command = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(command,editor);
    strcat(command," ~/.iceplayer/black_list.conf");
    system(command);
    g_free_n(command);
 }else{
        GtkWidget *dialog_real = \
	  gtk_message_dialog_new_with_markup(NULL,
					     GTK_DIALOG_DESTROY_WITH_PARENT,
					     GTK_MESSAGE_QUESTION,
					     GTK_BUTTONS_OK,
					     "抱歉，没有找到你计算机上面的编辑器程序，包括gedit、kedit、leafpad、mousepad都无法找到，请手动编辑 ~/.iceplayer/black_list.conf");
        gtk_dialog_run((GtkDialog *)dialog_real);
        gtk_widget_destroy(dialog_real);
 }
}

void configure_music_web() {}
void configure_file_button() {}

void configure_to_configure_widget() {
    print_programming(("configure_to_configure_widget\n"));
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.auto_update_checkbutton), configure.auto_update);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.show_in_taskbar_checkbutton), configure.task_bar);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.decorated), configure.decorated);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.show_tray_checkbutton), configure.tray);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.always_above_checkbutton), configure.above);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.show_in_all_work_places_checkbutton), configure.all_work_places);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.open_and_min_checkbutton), configure.begin_min);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.last_pos_checkbutton), configure.last_pos);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.open_and_play_checkbutton), configure.begin_play);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.notify_checkbutton), configure.notify);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.failed_and_stop_checkbutton), configure.failed_stop);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.auto_jump_to_playing_checkbutton), configure.auto_jump_to_play);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_lrc_checkbutton), configure.lrc);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_search_lrc_checkbutton), configure.search_lrc);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.search_lrc_with_filename_and_id3_checkbutton), configure.search_filename_or_id3);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_desktop_lrc_checkbutton), configure.desktop_lrc);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.edit_search_lrc_checkbutton), configure.edit_search_word);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.desktop_lrc_lock_checkbutton), configure.desktop_lrc_lock);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.desktop_lrc_use_kalaok_checkbutton), configure.desktop_lrc_kalaok);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.desktop_lrc_above_checkbutton), configure.desktop_lrc_above);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.lists_use_system_theme_checkbutton), configure.lists_use_system_theme);

    gtk_entry_set_text(GTK_ENTRY(configure_widget.search_lrc_time_entry),
		       configure.search_lrc_time);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.lrc_download_path_entry ),
		       configure.download_lrc_path);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.local_lrc_path_1_entry ),
		       configure.search_lrc_path_1);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.local_lrc_path_2_entry ),
		       configure.search_lrc_path_2);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.local_lrc_path_3_entry ),
		       configure.search_lrc_path_3);

    gtk_font_button_set_font_name(GTK_FONT_BUTTON(configure_widget.info_name_fontbutton), configure.info_name_font);
    gtk_font_button_set_font_name(GTK_FONT_BUTTON(configure_widget.info_time_fontbutton), configure.info_time_font);

    GdkColor *color;
    gtk_font_button_set_font_name(GTK_FONT_BUTTON(configure_widget.lists_fontbutton), configure.lists_font);
    gtk_color_selection_palette_from_string(configure.lists_front_color,
					    &color,NULL);
    gtk_color_button_set_color (GTK_COLOR_BUTTON(configure_widget.lists_front_color_button), color);
    gtk_color_selection_palette_from_string(configure.lists_back_color,
					    &color,NULL);
    gtk_color_button_set_color (GTK_COLOR_BUTTON(configure_widget.lists_back_color_button), color);

    gtk_font_button_set_font_name(GTK_FONT_BUTTON(configure_widget.widget_lrc_fontbutton),configure.widget_lrc_font);
    gtk_color_selection_palette_from_string(configure.widget_lrc_front_color,
					    &color,NULL);
    gtk_color_button_set_color (GTK_COLOR_BUTTON(configure_widget.widget_lrc_colorbutton_1), color);
    gtk_color_selection_palette_from_string(configure.widget_lrc_back_color,
					    &color,NULL);
    gtk_color_button_set_color (GTK_COLOR_BUTTON(configure_widget.widget_lrc_colorbutton_2), color);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.widget_back_pic_entry),
		       configure.widget_lrc_Independent_back_image );
    gtk_entry_set_text(GTK_ENTRY(configure_widget.back_pic_entry),
		       configure.back_image);
    gchar a[50];
    sprintf(a,"%d",configure.desktop_lrc_word_stroke);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.desktop_lrc_word_stroke_entry),a);

    gtk_font_button_set_font_name(GTK_FONT_BUTTON(configure_widget.desktop_lrc_fontbutton),configure.desktop_lrc_font);
    gtk_color_selection_palette_from_string(configure.desktop_lrc_front_up_color,&color,NULL);
    gtk_color_button_set_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_1), color);
    gtk_color_selection_palette_from_string(configure.desktop_lrc_front_cen_color,&color,NULL);
    gtk_color_button_set_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_2), color);
    gtk_color_selection_palette_from_string(configure.desktop_lrc_front_down_color,&color,NULL);
    gtk_color_button_set_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_3), color);
    gtk_color_selection_palette_from_string(configure.desktop_lrc_back_up_color,&color,NULL);
    gtk_color_button_set_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_4), color);
    gtk_color_selection_palette_from_string(configure.desktop_lrc_back_cen_color,&color,NULL);
    gtk_color_button_set_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_5), color);
    gtk_color_selection_palette_from_string(configure.desktop_lrc_back_down_color,&color,NULL);
    gtk_color_button_set_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_6), color);
    gtk_color_selection_palette_from_string(configure.desktop_lrc_word_stroke_color,&color,NULL);
    gtk_color_button_set_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_word_stroke_colorbutton), color);

    gtk_entry_set_text(GTK_ENTRY(configure_widget.music_download_path_entry), configure.download_path);

    gtk_entry_set_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_1), configure.quick_show_and_hide);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_2), configure.quick_play_and_pause);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_3), configure.quick_previous);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_4), configure.quick_next);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_5), configure.quick_stop);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_7), configure.quick_research_lrc);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.quick_shortcut_entry_6), configure.quick_quit);

    gchar info_text[256];
    sprintf(info_text,"%d",configure.info_name_x);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.info_name_entry_x), info_text);
    sprintf(info_text,"%d",configure.info_name_y);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.info_name_entry_y), info_text);
    sprintf(info_text,"%d",configure.info_time_x);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.info_time_entry_x), info_text);
    sprintf(info_text,"%d",configure.info_time_y);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.info_time_entry_y), info_text);
}

gboolean read_desktop_lrc_config() {
    print_programming(("read_desktop_lrc_config\n"));
    GKeyFile *keyfile;
    GError *error=NULL;
    gint read=1;
    gchar *uri=NULL;

    keyfile = g_key_file_new();
    uri = get_desk_config_path(read);
    if (uri==NULL)
        return FALSE;
    if (!g_key_file_load_from_file(keyfile,uri,0,NULL)) {
      dialog(_("不是标准的配置文件，标准的配置文件请直接复制 ~/.iceplayer/configure.conf 的桌面歌词部分，或者参考导出的桌面配置文件"));
        g_key_file_free(keyfile);
        return FALSE;
    } else {
///
        configure.desktop_lrc = g_key_file_get_integer(keyfile, "桌面歌词",
						       "使用桌面歌词", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc ,1 )
        configure.desktop_lrc_lock = g_key_file_get_integer(keyfile,
							    "桌面歌词",
							    "锁定桌面歌词",
							    &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_lock ,0 )
        configure.desktop_lrc_kalaok = g_key_file_get_integer(keyfile,
							      "桌面歌词",
							      "桌面歌词卡啦OK效果",
							      &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_kalaok ,1 )
        configure.desktop_lrc_above = g_key_file_get_integer(keyfile,
							     "桌面歌词",
							     "桌面歌词最前",
							     &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_above , 1)
        configure.desktop_lrc_x = g_key_file_get_integer(keyfile,
							 "桌面歌词",
							 "桌面歌词X", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_x , 117)
        configure.desktop_lrc_y = g_key_file_get_integer(keyfile,
							 "桌面歌词",
							 "桌面歌词Y", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_y ,543 )
        configure.desktop_lrc_width = g_key_file_get_integer(keyfile,
							     "桌面歌词",
							     "桌面歌词Width",
							     &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_width ,1011 )
        configure.desktop_lrc_height = g_key_file_get_integer(keyfile,
							      "桌面歌词",
							      "桌面歌词Height",
							      &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_height ,61 )
        strcpy(configure.desktop_lrc_font,
	       g_key_file_get_string_new(keyfile, "桌面歌词",
					 "桌面歌词字体", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_font , "Sans Bold 20")
        strcpy(configure.desktop_lrc_front_up_color,
	       g_key_file_get_string_new(keyfile, "桌面歌词",
					 "桌面歌词前景色一", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_front_up_color ,"#E3CD9A")
        strcpy(configure.desktop_lrc_front_cen_color,
	       g_key_file_get_string_new(keyfile, "桌面歌词",
					 "桌面歌词前景色二", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_front_cen_color,
			       "#F5A91E")
        strcpy(configure.desktop_lrc_front_down_color,
	       g_key_file_get_string_new(keyfile, "桌面歌词",
					 "桌面歌词前景色三", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_front_down_color,"#DE9856")
        strcpy(configure.desktop_lrc_back_up_color,
	       g_key_file_get_string_new(keyfile, "桌面歌词",
					 "桌面歌词背景色一", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_back_up_color , "#7973B1")
        strcpy(configure.desktop_lrc_back_cen_color,
	       g_key_file_get_string_new(keyfile, "桌面歌词",
					 "桌面歌词背景色二", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_back_cen_color , "#6971A4")
        strcpy(configure.desktop_lrc_back_down_color,
	       g_key_file_get_string_new(keyfile, "桌面歌词",
					 "桌面歌词背景色三", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_back_down_color, "#566891")
        configure.desktop_lrc_word_stroke = \
	  g_key_file_get_integer(keyfile, "桌面歌词",
				 "桌面歌词字体描边宽度", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_word_stroke , 8)
        strcpy(configure.desktop_lrc_word_stroke_color,
	       g_key_file_get_string_new(keyfile, "桌面歌词",
					 "桌面歌词字体描边颜色",&error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_word_stroke_color, "#000000")
        g_key_file_free(keyfile);
    }
    configure_to_configure_widget();
    configure_to_iceplayer_widget();
    return TRUE;
}

gboolean configure_to_iceplayer_widget() {
    print_programming(("configure_to_iceplayer_widget\n"));
    gtk_window_resize(GTK_WINDOW(iceplayer.window),
		      configure.width,configure.height);
    gtk_window_move(GTK_WINDOW(iceplayer.window),configure.x,configure.y);
    if (configure.widget_lrc_show_type == WIDGET_LRC_SHOW_TYPE_HPANED &&
	configure.show_page == SWITCH_LRC)
        configure.show_page = SWITCH_LIST;
    if (configure.widget_lrc_show_type == WIDGET_LRC_SHOW_TYPE_INDEPENDENT&&
	configure.show_page == SWITCH_LRC)
        configure.show_page=SWITCH_LIST;
    //gtk_notebook_set_current_page(iceplayer.notebook,configure.show_page);
    gtk_paned_set_position(GTK_PANED(iceplayer.hpaned),configure.hpaned_pos );
    gtk_paned_set_position(GTK_PANED(iceplayer.list_hpaned),
			   configure.list_hpaned_pos );

    gchar *path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(path,"above");
    if (configure.above) {
        strcat(path,".on");
        gtk_window_set_keep_above(GTK_WINDOW(iceplayer.window),configure.above); //总在最前
    }
    if (configure.desktop_lrc_lock) {
        GdkPixmap *input_mask = gdk_pixmap_new (NULL, 1, 1, 1);
        GdkGC *gc = gdk_gc_new (input_mask);
        GdkColor color;
        color.pixel = 1.0;
        gdk_gc_set_foreground (gc, &color);
        gdk_draw_rectangle (input_mask, gc, TRUE, 0, 0, 1, 1);
        gtk_widget_input_shape_combine_mask (desktop_lrc.window, NULL, 0, 0);
        gtk_widget_input_shape_combine_mask (desktop_lrc.window, input_mask, 0, 0);
        g_object_unref (input_mask);
        g_object_unref (gc);
    } else {
        gtk_widget_input_shape_combine_mask (desktop_lrc.window, NULL, 0, 0);
    }
    if (!configure.lists_use_system_theme) {
        gtk_widget_set_sensitive(configure_widget.lists_back_color_button,TRUE);
        gtk_widget_set_sensitive(configure_widget.lists_fontbutton,TRUE);
        gtk_widget_set_sensitive(configure_widget.lists_front_color_button,TRUE);

        GdkColor *color;
        gtk_color_selection_palette_from_string(configure.lists_back_color,&color,NULL);
        gtk_widget_modify_base(iceplayer.list_songs_treeview,GTK_STATE_NORMAL,color);
        gtk_widget_modify_base(iceplayer.list_lists_treeview,GTK_STATE_NORMAL,color);

        g_free_n(color);
        color=NULL;
        gtk_color_selection_palette_from_string(configure.lists_front_color,&color,NULL);
        gtk_widget_modify_text(iceplayer.list_songs_treeview,GTK_STATE_NORMAL,color);
        gtk_widget_modify_text(iceplayer.list_lists_treeview,GTK_STATE_NORMAL,color);
        g_free_n(color);

        PangoFontDescription *pfd;
        pfd = pango_font_description_from_string(configure.lists_font);
        gtk_widget_modify_font (iceplayer.list_lists_treeview, pfd);
        gtk_widget_modify_font (iceplayer.list_songs_treeview, pfd);
        gtk_widget_set_size_request(iceplayer.list_lists_treeview,0,0);
        gtk_widget_set_size_request(iceplayer.list_songs_treeview,0,0);
        pango_font_description_free (pfd);

    } else {
        gtk_widget_set_sensitive(configure_widget.lists_back_color_button,FALSE);
        gtk_widget_set_sensitive(configure_widget.lists_fontbutton,FALSE);
        gtk_widget_set_sensitive(configure_widget.lists_front_color_button,FALSE);

        GtkRcStyle * style = gtk_rc_style_copy(gtk_widget_get_modifier_style(iceplayer.window));
        gtk_widget_modify_style (iceplayer.list_lists_treeview,style);
        gtk_widget_modify_style (iceplayer.list_songs_treeview,style);
        g_object_unref(style);
    }

    gtk_range_set_value(GTK_RANGE(configure_widget.window_alpha_hscale),
			configure.window_alpha);

    g_timeout_add(5, G_SOURCE_FUNC(change_alpha),
		  configure_widget.window_alpha_hscale);

    PangoAttrList *title_attr_list;
    static PangoAttribute *title_attr;

    PangoFontDescription *pfd;

    pfd = pango_font_description_from_string(configure.info_name_font);
    gtk_widget_modify_font (iceplayer.name_label, pfd);
    gtk_widget_set_size_request(iceplayer.name_label,-1,pango_font_description_get_size(pfd) / 800);
    title_attr_list = pango_attr_list_new();
    title_attr = pango_attr_size_new_absolute( pango_font_description_get_size(pfd) );
    //title_attr[1] = pango_attr_weight_new(PANGO_WEIGHT_NORMAL);
    pango_attr_list_insert(title_attr_list, title_attr);
    //pango_attr_list_insert(title_attr_list, title_attr[1]);
    gtk_label_set_attributes(GTK_LABEL(iceplayer.name_label), title_attr_list);
    pango_attr_list_unref(title_attr_list);
    pango_font_description_free (pfd);

//17000
    pfd = pango_font_description_from_string(configure.info_time_font);
    gtk_widget_modify_font (iceplayer.time_label, pfd);
    gtk_widget_set_size_request(iceplayer.time_label,-1,pango_font_description_get_size(pfd) /800);
    title_attr_list = pango_attr_list_new();
    title_attr = pango_attr_size_new_absolute(pango_font_description_get_size(pfd) );
    //  title_attr[1] = pango_attr_weight_new(PANGO_WEIGHT_NORMAL);
    pango_attr_list_insert(title_attr_list, title_attr);
    // pango_attr_list_insert(title_attr_list, title_attr[1]);
    gtk_label_set_attributes(GTK_LABEL(iceplayer.time_label), title_attr_list);
    pango_attr_list_unref(title_attr_list);
    pango_font_description_free (pfd);

    gtk_status_icon_set_visible(GTK_STATUS_ICON(iceplayer.trayIcon),configure.tray);

    gchar *dir= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(dir,get_skin_dir());
    gtk_container_remove_new(GTK_CONTAINER(iceplayer.above_eventbox),iceplayer.above_image);
    iceplayer.above_image = gtk_image_new_from_file_new(dir,path);
    g_free_n(path);
    g_free_n(dir);
    gtk_widget_show(iceplayer.above_image);
    gtk_container_add(GTK_CONTAINER(iceplayer.above_eventbox),iceplayer.above_image);

    gtk_window_set_keep_above(GTK_WINDOW(desktop_lrc.window),configure.desktop_lrc_above);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(iceplayer.window),!configure.task_bar);//任务栏
    gtk_window_set_decorated(GTK_WINDOW(iceplayer.window),configure.decorated);//外框

    if (configure.all_work_places)gtk_window_stick (GTK_WINDOW(iceplayer.window));//工作区
    else gtk_window_unstick  (GTK_WINDOW(iceplayer.window));
    gtk_window_set_skip_pager_hint(GTK_WINDOW(iceplayer.window),!configure.all_work_places);

    if (GTK_IS_WIDGET(desktop_lrc.window)) {
        if (configure.desktop_lrc&&configure.lrc) {
	  widget_lrc_refresh(TRUE);
	  gtk_widget_show(desktop_lrc.window);
	  gtk_window_move(GTK_WINDOW(desktop_lrc.window),
			  configure.desktop_lrc_x,configure.desktop_lrc_y);
	  gtk_window_resize(GTK_WINDOW(desktop_lrc.window),
			    configure.desktop_lrc_width,
			    configure.desktop_lrc_height);
        } else {
	  gtk_label_set_text(GTK_LABEL(iceplayer.widget_lrc_label),"");
	  gtk_label_set_text(GTK_LABEL(widget_lrc.label),"");
	  gtk_widget_hide(desktop_lrc.window);
        }
    }
    gint x,y;
    gint w,h;

    gtk_window_get_position(GTK_WINDOW(widget_lrc.window),&x,&y);
    gtk_window_get_size(GTK_WINDOW(widget_lrc.window),&w,&h);

    if ((x!=0)&&(y!=0)&&(w!=170)&&(h!=250)&&!configure.lrc&&configure.widget_lrc_show_type==WIDGET_LRC_SHOW_TYPE_INDEPENDENT) {


        configure.widget_lrc_Independent_x=x;
        configure.widget_lrc_Independent_y=y;
        configure.widget_lrc_Independent_width=w;
        configure.widget_lrc_Independent_height=h;
        gtk_widget_hide(widget_lrc.window);

    } else {
        if (configure.widget_lrc_show_type == WIDGET_LRC_SHOW_TYPE_INDEPENDENT) {
            gtk_widget_show_all(widget_lrc.window);
            gtk_window_move(GTK_WINDOW(widget_lrc.window),
			    configure.widget_lrc_Independent_x,
			    configure.widget_lrc_Independent_y);
            gtk_window_resize(GTK_WINDOW(widget_lrc.window),
			      configure.widget_lrc_Independent_width,
			      configure.widget_lrc_Independent_height);
            gtk_widget_set_usize(widget_lrc.label,
				 configure.widget_lrc_Independent_width-4,
				 configure.widget_lrc_Independent_height-4);
        } else {
            if (configure.widget_lrc_show_type != WIDGET_LRC_SHOW_TYPE_HPANED) {
                if (GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned))
		  gtk_container_remove_new(GTK_CONTAINER(iceplayer.hpaned),
					   iceplayer.widget_lrc_label_hpaned);
            }
            gtk_widget_hide(widget_lrc.window);
        }
    }
    ///
    pfd = pango_font_description_from_string(configure.widget_lrc_font);
    if (GTK_IS_WIDGET (iceplayer.widget_lrc_label) )gtk_widget_modify_font (iceplayer.widget_lrc_label, pfd);
    if (GTK_IS_WIDGET (widget_lrc.label) )gtk_widget_modify_font (widget_lrc.label, pfd);
    if (GTK_IS_WIDGET (iceplayer.widget_lrc_label_hpaned) )gtk_widget_modify_font (iceplayer.widget_lrc_label_hpaned, pfd);

    widget_lrc_word_height = 3 * pango_font_description_get_size(pfd) / PANGO_SCALE;
    pango_font_description_free (pfd);

    pfd = pango_font_description_from_string(configure.desktop_lrc_font);
    desktop_lrc_word_height = 3 * pango_font_description_get_size(pfd) / PANGO_SCALE;
    pango_font_description_free (pfd);

    sscanf(gtk_entry_get_text(GTK_ENTRY(configure_widget.info_name_entry_x)),
	   "%d",&configure.info_name_x);
    sscanf(gtk_entry_get_text(GTK_ENTRY(configure_widget.info_name_entry_y)),
	   "%d",&configure.info_name_y);

    sscanf(gtk_entry_get_text(GTK_ENTRY(configure_widget.info_time_entry_x)),
	   "%d",&configure.info_time_x);
    sscanf(gtk_entry_get_text(GTK_ENTRY(configure_widget.info_time_entry_y)),
	   "%d",&configure.info_time_y);

    if (!(configure.info_name_x==0&&configure.info_name_y==0)) {
        gtk_fixed_move_new(iceplayer.main_fixed,
			   iceplayer.name_label,configure.info_name_x,
			   configure.info_name_y);
    } else {
        gtk_fixed_move_new(iceplayer.main_fixed,
			   iceplayer.name_label,skin.info_name_x,
			   skin.info_name_y);
    }
    if (!(configure.info_time_x==0&&configure.info_time_y==0)) {
        gtk_fixed_move_new(iceplayer.main_fixed,
			   iceplayer.time_label,
			   configure.info_time_x,configure.info_time_y);
    } else {
        gtk_fixed_move_new(iceplayer.main_fixed,
			   iceplayer.time_label,
			   skin.info_time_x,skin.info_time_y);
    }

    return FALSE;
}

void configure_sw_page(gpointer * one,gint type) {
    print_programming(("configure_sw_page\n"));
    gtk_notebook_set_current_page(GTK_NOTEBOOK(configure_widget.notebook),type);
}

void configure_ui_init() {
    print_programming(("configure_ui_init\n"));
    configure_widget.sw_vbox = gtk_vbox_new(FALSE,0);
    gtk_widget_show (configure_widget.sw_vbox);

    configure_widget.sw_hbox = gtk_hbox_new(FALSE,0);
    gtk_widget_show (configure_widget.sw_hbox);

    configure_widget.notebook = gtk_notebook_new ();
    gtk_widget_show (configure_widget.notebook);
    gtk_notebook_set_show_tabs (GTK_NOTEBOOK (configure_widget.notebook), FALSE);
    gtk_notebook_set_show_border (GTK_NOTEBOOK (configure_widget.notebook), FALSE);
    gtk_container_add (GTK_CONTAINER (configure_widget.sw_vbox), configure_widget.notebook);

    gtk_box_pack_start  (GTK_BOX (configure_widget.sw_vbox), configure_widget.sw_hbox, FALSE, TRUE, 0);

    configure_widget.sw_normal_button = gtk_button_new_with_label(_("常规"));
    gtk_widget_show(configure_widget.sw_normal_button);
    gtk_box_pack_start (GTK_BOX (configure_widget.sw_hbox), configure_widget.sw_normal_button, TRUE, TRUE, 0);
    //gtk_widget_set_size_request (configure_widget.sw_normal_button,0, 0);

    configure_widget.sw_play_button= gtk_button_new_with_label(_("播放"));
    gtk_widget_show(configure_widget.sw_play_button);
    gtk_box_pack_start  (GTK_BOX (configure_widget.sw_hbox), configure_widget.sw_play_button, TRUE, TRUE, 0);
    // gtk_widget_set_size_request (configure_widget.sw_play_button,0, 0);

    configure_widget.sw_skin_button = gtk_button_new_with_label(_("界面"));
    gtk_widget_show(configure_widget.sw_skin_button);
    gtk_box_pack_start  (GTK_BOX (configure_widget.sw_hbox), configure_widget.sw_skin_button, TRUE, TRUE, 0);
    //gtk_widget_set_size_request (configure_widget.sw_skin_button,0, 0);

    configure_widget.sw_lrc_button = gtk_button_new_with_label(_("歌词"));
    gtk_widget_show(configure_widget.sw_lrc_button);
    gtk_box_pack_start  (GTK_BOX (configure_widget.sw_hbox), configure_widget.sw_lrc_button, TRUE, TRUE, 0);
    //gtk_widget_set_size_request (configure_widget.sw_lrc_button,0, 0);

    configure_widget.sw_music_button = gtk_button_new_with_label(_("音乐盒"));
    //gtk_widget_show(configure_widget.sw_music_button);
    // gtk_box_pack_start  (GTK_CONTAINER (configure_widget.sw_hbox), configure_widget.sw_music_button, TRUE, TRUE, 0);
    //gtk_widget_set_size_request (configure_widget.sw_music_button,0, 0);

    configure_widget.sw_quick_button = gtk_button_new_with_label(_("快捷键"));
    gtk_widget_show(configure_widget.sw_quick_button);
    gtk_box_pack_start  (GTK_BOX (configure_widget.sw_hbox), configure_widget.sw_quick_button, TRUE, TRUE, 0);
    //gtk_widget_set_size_request (configure_widget.sw_quick_button,0, 0);


    configure_widget.normal_table = gtk_table_new (3, 2, FALSE);
    gtk_widget_show (configure_widget.normal_table);
    gtk_container_add (GTK_CONTAINER (configure_widget.notebook), configure_widget.normal_table);
    gtk_container_set_border_width (GTK_CONTAINER (configure_widget.normal_table), 10);

    configure_widget.auto_update_checkbutton = gtk_check_button_new_with_mnemonic (_("开启自动更新"));
    gtk_widget_show (configure_widget.auto_update_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.normal_table), configure_widget.auto_update_checkbutton, 0, 1, 0, 1,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);

    configure_widget.show_in_taskbar_checkbutton = gtk_check_button_new_with_mnemonic (_("任务栏"));
    gtk_widget_show (configure_widget.show_in_taskbar_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.normal_table), configure_widget.show_in_taskbar_checkbutton, 1, 2, 0, 1,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);

    configure_widget.decorated = gtk_check_button_new_with_mnemonic (_("GTK 外框架"));
    gtk_widget_show (configure_widget.decorated);
    gtk_table_attach (GTK_TABLE (configure_widget.normal_table), configure_widget.decorated, 0, 1, 1, 2,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);

    configure_widget.show_tray_checkbutton = gtk_check_button_new_with_mnemonic (_("通知区域"));
    gtk_widget_show (configure_widget.show_tray_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.normal_table), configure_widget.show_tray_checkbutton, 1, 2, 1, 2,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);

    configure_widget.always_above_checkbutton = gtk_check_button_new_with_mnemonic (_("总在最前"));
    gtk_widget_show (configure_widget.always_above_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.normal_table), configure_widget.always_above_checkbutton, 0, 1, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.show_in_all_work_places_checkbutton = gtk_check_button_new_with_mnemonic (_("所有工作区"));
    gtk_widget_show (configure_widget.show_in_all_work_places_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.normal_table), configure_widget.show_in_all_work_places_checkbutton, 1, 2, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.back_pic_label = gtk_label_new (_("背景,如/a/1.png\n留空为主题:"));
    gtk_widget_show (configure_widget.back_pic_label);
    gtk_table_attach (GTK_TABLE (configure_widget.normal_table), configure_widget.back_pic_label, 0, 1, 3, 4,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.back_pic_label), 0, 0.5);

    configure_widget.back_pic_entry = gtk_entry_new ();
    gtk_widget_show (configure_widget.back_pic_entry);
    gtk_table_attach (GTK_TABLE (configure_widget.normal_table), configure_widget.back_pic_entry, 1, 2,3,4,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_usize(configure_widget.back_pic_entry,25,25);
    /*
        configure_widget.hseparator_lists = gtk_hseparator_new ();
        gtk_widget_show (configure_widget.hseparator_lists);
        gtk_table_attach (GTK_TABLE (configure_widget.normal_table), configure_widget.hseparator_lists, 0, 2, 5, 6,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 5);
    */

    configure_widget.label4 = gtk_label_new (_("\n\345\270\270\350\247\204\n"));
    gtk_widget_show (configure_widget.label4);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (configure_widget.notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (configure_widget.notebook), 0), configure_widget.label4);

    configure_widget.vbox1 = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (configure_widget.vbox1);
    gtk_container_add (GTK_CONTAINER (configure_widget.notebook), configure_widget.vbox1);
    gtk_container_set_border_width (GTK_CONTAINER (configure_widget.vbox1), 10);

    configure_widget.open_and_play_checkbutton = gtk_check_button_new_with_mnemonic (_("开启即播放"));
    gtk_widget_show (configure_widget.open_and_play_checkbutton);
    gtk_box_pack_start (GTK_BOX (configure_widget.vbox1), configure_widget.open_and_play_checkbutton, FALSE, FALSE, 0);

    configure_widget.failed_and_stop_checkbutton = gtk_check_button_new_with_mnemonic (_("不能播放时  选中:停止 否:下一首"));
    gtk_widget_show (configure_widget.failed_and_stop_checkbutton);
    gtk_box_pack_start (GTK_BOX (configure_widget.vbox1), configure_widget.failed_and_stop_checkbutton, FALSE, FALSE, 0);

    configure_widget.auto_jump_to_playing_checkbutton = gtk_check_button_new_with_mnemonic (_("播放时列表焦点跟随"));
    gtk_widget_show (configure_widget.auto_jump_to_playing_checkbutton);
    gtk_box_pack_start (GTK_BOX (configure_widget.vbox1), configure_widget.auto_jump_to_playing_checkbutton, FALSE, FALSE, 0);

    configure_widget.open_and_min_checkbutton = gtk_check_button_new_with_mnemonic (_("开启即最小化"));
    //configure_widget.open_and_min_checkbutton = gtk_check_button_new_with_mnemonic (_("勾选-开启即最小化/关闭-恢复关闭前状态"));
    gtk_widget_show (configure_widget.open_and_min_checkbutton);
    gtk_box_pack_start (GTK_BOX (configure_widget.vbox1), configure_widget.open_and_min_checkbutton, FALSE, FALSE, 0);

    configure_widget.last_pos_checkbutton= gtk_check_button_new_with_mnemonic (_("记录上次播放位置"));
    gtk_widget_show (configure_widget.last_pos_checkbutton);
    gtk_box_pack_start (GTK_BOX (configure_widget.vbox1), configure_widget.last_pos_checkbutton, FALSE, FALSE, 0);

    configure_widget.notify_checkbutton = gtk_check_button_new_with_mnemonic (_("切歌notify通知"));
    gtk_widget_show (configure_widget.notify_checkbutton);
    gtk_box_pack_start (GTK_BOX (configure_widget.vbox1), configure_widget.notify_checkbutton, FALSE, FALSE, 0);


    configure_widget.label5 = gtk_label_new (_("\n音乐盒\n"));
    gtk_widget_show (configure_widget.label5);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (configure_widget.notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (configure_widget.notebook), 1), configure_widget.label5);

    configure_widget.hbox1 = gtk_hbox_new (FALSE, 0);
    gtk_widget_show (configure_widget.hbox1);
    gtk_container_add (GTK_CONTAINER (configure_widget.notebook), configure_widget.hbox1);

    configure_widget.table3 = gtk_table_new (3, 2, FALSE);
    gtk_widget_show (configure_widget.table3);
    gtk_box_pack_start (GTK_BOX (configure_widget.hbox1), configure_widget.table3, TRUE, TRUE, 0);
    gtk_container_set_border_width (GTK_CONTAINER (configure_widget.table3), 10);

    configure_widget.skin_do_button = gtk_button_new_with_label ("皮肤选择");
    gtk_widget_show (configure_widget.skin_do_button);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.skin_do_button, 0, 3, 4, 5,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 8, 8);
    /*
        configure_widget.delete_button = gtk_button_new_from_stock ("gtk-remove");
        gtk_widget_show (configure_widget.delete_button);
        gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.delete_button, 1, 2, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

        configure_widget.skin_add_button = gtk_button_new_from_stock ("gtk-add");
        gtk_widget_show (configure_widget.skin_add_button);
        gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.skin_add_button, 0, 1, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

        configure_widget.skin_info_button = gtk_button_new_from_stock ("gtk-info");
        gtk_widget_show (configure_widget.skin_info_button);
        gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.skin_info_button, 1, 2, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

        configure_widget.skin_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
        gtk_widget_show (configure_widget.skin_scrolledwindow);
        gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.skin_scrolledwindow, 0, 2, 0, 1,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
        gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (configure_widget.skin_scrolledwindow), GTK_SHADOW_NONE);
        gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (configure_widget.skin_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

        configure_widget.skin_treeview = gtk_tree_view_new ();
        gtk_widget_show (configure_widget.skin_treeview);
        gtk_container_add (GTK_CONTAINER (configure_widget.skin_scrolledwindow), configure_widget.skin_treeview);
    */


    configure_widget.lists_use_system_theme_checkbutton =  gtk_check_button_new_with_mnemonic (_("列表使用系统主题\n(部分系统中颜色可能无法正确更改)"));
    gtk_widget_show (configure_widget.lists_use_system_theme_checkbutton);

    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.lists_use_system_theme_checkbutton, 0, 3, 0,1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.lists_fontbutton = gtk_font_button_new ();
    gtk_widget_show (configure_widget.lists_fontbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.lists_fontbutton, 1, 3, 1, 2,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_usize(configure_widget.lists_fontbutton,25,25);

    configure_widget.lists_front_color_button = gtk_color_button_new ();
    gtk_widget_show (configure_widget.lists_front_color_button);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.lists_front_color_button, 1, 3, 2, 3,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_usize(configure_widget.lists_front_color_button,25,25);

    configure_widget.lists_back_color_button = gtk_color_button_new ();
    gtk_widget_show (configure_widget.lists_back_color_button);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.lists_back_color_button, 1, 3, 3 ,4,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_usize(configure_widget.lists_back_color_button,25,25);

    configure_widget.label41 = gtk_label_new (_("列表字体"));
    gtk_widget_show (configure_widget.label41);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.label41, 0, 1, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label41), 0, 0.5);

    configure_widget.label42 = gtk_label_new (_("列表前景色"));
    gtk_widget_show (configure_widget.label42);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.label42, 0, 1,2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label42), 0, 0.5);

    configure_widget.label43 = gtk_label_new (_("列表背景色"));
    gtk_widget_show (configure_widget.label43);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.label43, 0, 1, 3, 4,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label43), 0, 0.5);

    configure_widget.label48 = gtk_label_new (_("播放名字体"));
    gtk_widget_show (configure_widget.label48);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.label48, 0, 1, 14, 15,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label48), 0, 0.5);

    configure_widget.label49 = gtk_label_new (_("时间进度字体"));
    gtk_widget_show (configure_widget.label49);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.label49, 0, 1, 15, 16,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label49), 0, 0.5);

    configure_widget.info_name_fontbutton = gtk_font_button_new ();
    gtk_widget_show (configure_widget.info_name_fontbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.info_name_fontbutton, 1, 3, 14, 15,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_usize(configure_widget.info_name_fontbutton,25,25);

    configure_widget.info_time_fontbutton = gtk_font_button_new ();
    gtk_widget_show (configure_widget.info_time_fontbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.info_time_fontbutton, 1, 3, 15, 16,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_usize(configure_widget.info_time_fontbutton,25,25);

    configure_widget.label50 = gtk_label_new (_("窗口透明度(需要混合效果支持)"));
    gtk_widget_show (configure_widget.label50);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.label50, 0, 1, 16, 17,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label50), 0, 0.5);

    configure_widget.window_alpha_hscale = gtk_hscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 255, 0, 0, 0)));
    gtk_widget_show (configure_widget.window_alpha_hscale);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.window_alpha_hscale, 1, 3, 16, 17,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    //gtk_widget_set_size_request (window_alpha_hscale, 18, 100);
    gtk_scale_set_draw_value (GTK_SCALE (configure_widget.window_alpha_hscale), FALSE);
    gtk_scale_set_value_pos (GTK_SCALE (configure_widget.window_alpha_hscale), GTK_POS_BOTTOM);
    gtk_range_set_inverted (GTK_RANGE (configure_widget.window_alpha_hscale), TRUE);

    g_signal_connect ((gpointer) configure_widget.window_alpha_hscale, "button_release_event",G_CALLBACK (change_alpha),NULL);

    if (!gdk_screen_is_composited(gdk_screen_get_default ())) {
        gtk_widget_set_sensitive(configure_widget.label50,FALSE);
        gtk_widget_set_sensitive(configure_widget.window_alpha_hscale,FALSE);
    }

    configure_widget.label51 = gtk_label_new (_("播放名坐标(格式为 x,y )可负值"));
    gtk_widget_show (configure_widget.label51);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.label51, 0, 1, 17, 18,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label51), 0, 0.5);

    configure_widget.info_name_entry_x = gtk_entry_new ();
    gtk_widget_show (configure_widget.info_name_entry_x);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.info_name_entry_x, 1, 2,17,18,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_size_request(configure_widget.info_name_entry_x,0,-1);

    configure_widget.info_name_entry_y = gtk_entry_new ();
    gtk_widget_show (configure_widget.info_name_entry_y);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.info_name_entry_y, 2, 3,17,18,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_size_request(configure_widget.info_name_entry_y,0,-1);

    configure_widget.label52 = gtk_label_new (_("时间进度坐标(留空为皮肤默认)"));
    gtk_widget_show (configure_widget.label52);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.label52, 0, 1, 18, 19,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label52), 0, 0.5);

    configure_widget.info_time_entry_x = gtk_entry_new ();
    gtk_widget_show (configure_widget.info_time_entry_x);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.info_time_entry_x, 1, 2,18,19,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_size_request(configure_widget.info_time_entry_x,0,-1);

    configure_widget.info_time_entry_y = gtk_entry_new ();
    gtk_widget_show (configure_widget.info_time_entry_y);
    gtk_table_attach (GTK_TABLE (configure_widget.table3), configure_widget.info_time_entry_y, 2, 3,18,19,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_widget_set_size_request(configure_widget.info_time_entry_y,0,-1);

    configure_widget.label6 = gtk_label_new (_("\n\347\232\256\350\202\244\n"));
    gtk_widget_show (configure_widget.label6);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (configure_widget.notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (configure_widget.notebook), 2), configure_widget.label6);

    configure_widget.lrc_scrolledwindow =  gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (configure_widget.lrc_scrolledwindow);
    gtk_container_add (GTK_CONTAINER (configure_widget.notebook),
		       configure_widget.lrc_scrolledwindow);
    gtk_scrolled_window_set_shadow_type(GTK_SCROLLED_WINDOW(configure_widget.lrc_scrolledwindow), GTK_SHADOW_NONE);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (configure_widget.lrc_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

    configure_widget.lrc_notebook = gtk_notebook_new ();
    gtk_widget_show (configure_widget.lrc_notebook);
    gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(configure_widget.lrc_scrolledwindow), configure_widget.lrc_notebook);
    gtk_notebook_set_tab_pos(GTK_NOTEBOOK(configure_widget.lrc_notebook),
			     GTK_POS_LEFT);

    configure_widget.table4 = gtk_table_new (14, 3, FALSE);
    gtk_widget_show (configure_widget.table4);
    gtk_container_add (GTK_CONTAINER (configure_widget.lrc_notebook),
		       configure_widget.table4);
    gtk_container_set_border_width (GTK_CONTAINER(configure_widget.table4), 10);

    configure_widget.hseparator5 = gtk_hseparator_new ();
    gtk_widget_show (configure_widget.hseparator5);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.hseparator5, 0, 2, 7, 8,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 5);

    configure_widget.local_lrc_path_3_entry = gtk_entry_new ();
    gtk_widget_show (configure_widget.local_lrc_path_3_entry);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.local_lrc_path_3_entry, 1, 2, 11, 12,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_invisible_char (GTK_ENTRY (configure_widget.local_lrc_path_3_entry), 9679);
    gtk_widget_set_size_request(configure_widget.local_lrc_path_3_entry,0,-1);

    configure_widget.local_lrc_path_2_entry = gtk_entry_new ();
    gtk_widget_show (configure_widget.local_lrc_path_2_entry);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.local_lrc_path_2_entry, 1, 2, 10, 11,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_invisible_char (GTK_ENTRY (configure_widget.local_lrc_path_2_entry), 9679);
    gtk_widget_set_size_request(configure_widget.local_lrc_path_2_entry,0,-1);

    configure_widget.local_lrc_path_1_entry = gtk_entry_new ();
    gtk_widget_show (configure_widget.local_lrc_path_1_entry);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.local_lrc_path_1_entry, 1, 2, 9, 10,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_invisible_char (GTK_ENTRY (configure_widget.local_lrc_path_1_entry), 9679);
    gtk_widget_set_size_request(configure_widget.local_lrc_path_1_entry,0,-1);

    configure_widget.lrc_download_path_entry = gtk_entry_new ();
    gtk_widget_show (configure_widget.lrc_download_path_entry);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.lrc_download_path_entry, 0, 2, 6, 7,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_invisible_char (GTK_ENTRY (configure_widget.lrc_download_path_entry), 9679);

    configure_widget.label18 = gtk_label_new ((_("\344\270\213\350\275\275\347\233\256\345\275\225\357\274\210\347\225\231\347\251\272\344\270\272\346\255\214\346\233\262\347\233\256\345\275\225\357\274\211\357\274\232")));
    gtk_widget_show (configure_widget.label18);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.label18, 0, 2, 5, 6,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label18), 0, 0.5);

    configure_widget.label46 = gtk_label_new (_("以下都搜索不到，搜索歌曲目录"));
    gtk_widget_show (configure_widget.label46);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.label46, 0, 2, 8, 9,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label46), 0, 0.5);

    configure_widget.label19 = gtk_label_new (_("搜索目录一(留空忽略)"));
    gtk_widget_show (configure_widget.label19);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.label19, 0, 1,9, 10,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label19), 0, 0.5);

    configure_widget.label20 = gtk_label_new (_("搜索目录二(留空忽略)"));
    gtk_widget_show (configure_widget.label20);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.label20, 0, 1, 10, 11,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label20), 0, 0.5);

    configure_widget.label21 = gtk_label_new (_("搜索目录三(留空忽略)"));
    gtk_widget_show (configure_widget.label21);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.label21, 0, 1, 11, 12,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label21), 0, 0.5);

    configure_widget.search_lrc_key_word_help_button=gtk_button_new();
    gtk_widget_show (configure_widget.search_lrc_key_word_help_button);
    gtk_button_set_label(GTK_BUTTON(configure_widget.search_lrc_key_word_help_button),"->黑名单功能说明与帮助");
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.search_lrc_key_word_help_button, 1, 2, 12, 13,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.search_lrc_key_word_button=gtk_button_new();
    gtk_widget_show (configure_widget.search_lrc_key_word_button);
    gtk_button_set_label(GTK_BUTTON(configure_widget.search_lrc_key_word_button),"设定搜索歌词黑名单");
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.search_lrc_key_word_button, 0, 1, 12, 13,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);


    configure_widget.edit_search_lrc_checkbutton = gtk_check_button_new_with_mnemonic ((_("无结果时弹对话框")));
    gtk_widget_show (configure_widget.edit_search_lrc_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.edit_search_lrc_checkbutton, 0, 1, 4, 5,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.search_lrc_with_filename_and_id3_checkbutton = gtk_check_button_new_with_mnemonic (_("(是:文件名,否:标签)搜索"));
    gtk_widget_show (configure_widget.search_lrc_with_filename_and_id3_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.search_lrc_with_filename_and_id3_checkbutton, 1, 2, 4, 5,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    //configure_widget.search_lrc_web_button = gtk_button_new_with_mnemonic (_("\345\274\225\346\223\216\357\274\232www.LRC123.com"));
    //gtk_widget_show (configure_widget.search_lrc_web_button);
    //gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.search_lrc_web_button, 0, 2, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.use_search_lrc_checkbutton = gtk_check_button_new_with_mnemonic (_("使用搜索功能"));
    gtk_widget_show (configure_widget.use_search_lrc_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.use_search_lrc_checkbutton, 0, 1, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.search_lrc_time_label = gtk_label_new (_("搜索时限(0为不提示自动下载)"));
    gtk_widget_show (configure_widget.search_lrc_time_label);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.search_lrc_time_label, 0, 2, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.search_lrc_time_label), 0, 0.5);

    configure_widget.search_lrc_time_entry = gtk_entry_new ();
    gtk_widget_show (configure_widget.search_lrc_time_entry);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.search_lrc_time_entry, 0, 2, 3, 4,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.use_lrc_checkbutton = gtk_check_button_new_with_mnemonic (_("使用歌词"));
    gtk_widget_show (configure_widget.use_lrc_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table4), configure_widget.use_lrc_checkbutton, 1, 2, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label10 = gtk_label_new (_("\n歌词\n"));
    gtk_widget_show (configure_widget.label10);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (configure_widget.lrc_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (configure_widget.lrc_notebook), 0), configure_widget.label10);

    configure_widget.table5 = gtk_table_new (3, 2, FALSE);
    gtk_widget_show (configure_widget.table5);
    gtk_container_add (GTK_CONTAINER (configure_widget.lrc_notebook), configure_widget.table5);
    gtk_container_set_border_width (GTK_CONTAINER (configure_widget.table5), 10);

    configure_widget.label22 = gtk_label_new (_("字体:"));
    gtk_widget_show (configure_widget.label22);
    gtk_table_attach (GTK_TABLE (configure_widget.table5), configure_widget.label22, 0, 1, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label22), 0, 0.5);

    configure_widget.label23 = gtk_label_new (_("高亮颜色:"));
    gtk_widget_show (configure_widget.label23);
    gtk_table_attach (GTK_TABLE (configure_widget.table5), configure_widget.label23, 0, 1, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label23), 0, 0.5);

    configure_widget.label24 = gtk_label_new (_("普通颜色:"));
    gtk_widget_show (configure_widget.label24);
    gtk_table_attach (GTK_TABLE (configure_widget.table5), configure_widget.label24, 0, 1, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label24), 0, 0.5);

    configure_widget.widget_back_pic_label = gtk_label_new (_("独立窗口背景\n(如/a/1.jpg)(留空为主题):"));
    gtk_widget_show (configure_widget.widget_back_pic_label);
    gtk_table_attach (GTK_TABLE (configure_widget.table5), configure_widget.widget_back_pic_label, 0, 2, 3, 4,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.widget_back_pic_label), 0, 0.5);

    configure_widget.widget_back_pic_entry = gtk_entry_new ();
    gtk_widget_show (configure_widget.widget_back_pic_entry);
    gtk_table_attach (GTK_TABLE (configure_widget.table5), configure_widget.widget_back_pic_entry, 0, 2,4,5,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.widget_lrc_colorbutton_1 = gtk_color_button_new ();
    gtk_widget_show (configure_widget.widget_lrc_colorbutton_1);
    gtk_table_attach (GTK_TABLE (configure_widget.table5), configure_widget.widget_lrc_colorbutton_1, 1, 2, 1, 2,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.widget_lrc_colorbutton_2 = gtk_color_button_new ();
    gtk_widget_show (configure_widget.widget_lrc_colorbutton_2);
    gtk_table_attach (GTK_TABLE (configure_widget.table5), configure_widget.widget_lrc_colorbutton_2, 1, 2, 2, 3,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.widget_lrc_fontbutton = gtk_font_button_new ();
    gtk_widget_show (configure_widget.widget_lrc_fontbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table5), configure_widget.widget_lrc_fontbutton, 1, 2, 0, 1,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label11 = gtk_label_new (_("窗口\n歌词"));
    gtk_widget_show (configure_widget.label11);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (configure_widget.lrc_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (configure_widget.lrc_notebook), 1), configure_widget.label11);

    configure_widget.table6 = gtk_table_new (14, 2, FALSE);
    gtk_widget_show (configure_widget.table6);
    gtk_container_add (GTK_CONTAINER (configure_widget.lrc_notebook), configure_widget.table6);
    gtk_container_set_border_width (GTK_CONTAINER (configure_widget.table6), 5);

    configure_widget.hseparator1 = gtk_hseparator_new ();
    gtk_widget_show (configure_widget.hseparator1);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.hseparator1, 0, 5, 8, 9,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0 ), 0, 0);

    configure_widget.use_desktop_lrc_checkbutton = gtk_check_button_new_with_mnemonic (_("桌面歌词"));
    gtk_widget_show (configure_widget.use_desktop_lrc_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.use_desktop_lrc_checkbutton, 0, 3, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.desktop_lrc_lock_checkbutton = gtk_check_button_new_with_mnemonic (_("锁定"));
    gtk_widget_show (configure_widget.desktop_lrc_lock_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_lock_checkbutton, 3, 5, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.desktop_lrc_use_kalaok_checkbutton = gtk_check_button_new_with_mnemonic (_("卡拉OK效果"));
    gtk_widget_show (configure_widget.desktop_lrc_use_kalaok_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_use_kalaok_checkbutton, 0, 3, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.desktop_lrc_above_checkbutton = gtk_check_button_new_with_mnemonic (_("总在最前"));
    gtk_widget_show (configure_widget.desktop_lrc_above_checkbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_above_checkbutton, 3, 5, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);


    configure_widget.label26 = gtk_label_new (_("高亮上："));
    gtk_widget_show (configure_widget.label26);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.label26, 0, 1, 5, 6,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label26), 0, 0.0);
    //gtk_widget_set_size_request(configure_widget.label26,0,-1);

    configure_widget.desktop_lrc_colorbutton_1 = gtk_color_button_new ();
    gtk_widget_show (configure_widget.desktop_lrc_colorbutton_1);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_colorbutton_1, 1, 2, 5, 6,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label27 = gtk_label_new (_("高亮中："));
    gtk_widget_show (configure_widget.label27);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.label27, 0, 1, 6, 7,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label27), 0, 0.0);
    //gtk_widget_set_size_request(configure_widget.label27,0,-1);

    configure_widget.desktop_lrc_colorbutton_2 = gtk_color_button_new ();
    gtk_widget_show (configure_widget.desktop_lrc_colorbutton_2);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_colorbutton_2, 1, 2, 6, 7,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label28 = gtk_label_new (_("高亮下："));
    gtk_widget_show (configure_widget.label28);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.label28, 0, 1, 7, 8,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label28), 0, 0.0);
    //gtk_widget_set_size_request(configure_widget.label28,0,-1);

    configure_widget.desktop_lrc_colorbutton_3 = gtk_color_button_new ();
    gtk_widget_show (configure_widget.desktop_lrc_colorbutton_3);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_colorbutton_3, 1, 2, 7, 8,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label29 = gtk_label_new (_("普通上："));
    gtk_widget_show (configure_widget.label29);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.label29, 3, 4, 5, 6,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label29), 0, 0.0);
// gtk_widget_set_size_request(configure_widget.label29,0,-1);

    configure_widget.desktop_lrc_colorbutton_4 = gtk_color_button_new ();
    gtk_widget_show (configure_widget.desktop_lrc_colorbutton_4);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_colorbutton_4, 4, 5, 5, 6,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label30 = gtk_label_new (_("普通中："));
    gtk_widget_show (configure_widget.label30);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.label30, 3, 4, 6, 7,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label30), 0, 0.0);
// gtk_widget_set_size_request(configure_widget.label30,0,-1);

    configure_widget.desktop_lrc_colorbutton_5 = gtk_color_button_new ();
    gtk_widget_show (configure_widget.desktop_lrc_colorbutton_5);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_colorbutton_5, 4, 5, 6, 7,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label31 = gtk_label_new (_("普通下："));
    gtk_widget_show (configure_widget.label31);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.label31, 3, 4, 7, 8,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label31), 0, 0.0);
// gtk_widget_set_size_request(configure_widget.label31,0,-1);

    configure_widget.desktop_lrc_colorbutton_6 = gtk_color_button_new ();
    gtk_widget_show (configure_widget.desktop_lrc_colorbutton_6);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_colorbutton_6, 4, 5, 7, 8,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);

    configure_widget.hseparator2 = gtk_hseparator_new ();
    gtk_widget_show (configure_widget.hseparator2);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.hseparator2, 0, 5, 4, 5,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);


    configure_widget.hseparator3 = gtk_hseparator_new ();
    gtk_widget_show (configure_widget.hseparator3);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.hseparator3, 0, 5, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 5);

    //configure_widget.hseparator4 = gtk_hseparator_new ();
    // gtk_widget_show (configure_widget.hseparator4);
    //gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.hseparator4, 0, 2, 12, 13,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 5);

    configure_widget.label25 = gtk_label_new (_("字体："));
    gtk_widget_show (configure_widget.label25);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.label25, 0, 2, 3, 4,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label25), 0, 0.0);

    configure_widget.desktop_lrc_fontbutton = gtk_font_button_new ();
    gtk_widget_show (configure_widget.desktop_lrc_fontbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_fontbutton, 2, 5, 3, 4,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);
    gtk_font_button_set_use_font (GTK_FONT_BUTTON (configure_widget.desktop_lrc_fontbutton), TRUE);

    configure_widget.desktop_lrc_word_stroke_label = gtk_label_new(_("描边宽度"));
    gtk_widget_show(configure_widget.desktop_lrc_word_stroke_label);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_word_stroke_label, 0, 1, 13, 14,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.desktop_lrc_word_stroke_label), 0, 0.5);

    configure_widget.desktop_lrc_word_stroke_entry = gtk_entry_new();
    gtk_widget_show(configure_widget.desktop_lrc_word_stroke_entry);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_word_stroke_entry, 1, 5, 13, 14,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);
    //gtk_widget_set_size_request(configure_widget.desktop_lrc_word_stroke_entry,10,10);

    configure_widget.desktop_lrc_word_stroke_colorbutton_label = gtk_label_new (_("描边颜色"));
    gtk_widget_show (configure_widget.desktop_lrc_word_stroke_colorbutton_label);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_word_stroke_colorbutton_label, 0, 1, 14, 15,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.desktop_lrc_word_stroke_colorbutton_label), 0, 0.5);

    configure_widget.desktop_lrc_word_stroke_colorbutton = gtk_color_button_new ();
    gtk_widget_show (configure_widget.desktop_lrc_word_stroke_colorbutton);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_word_stroke_colorbutton, 1, 5, 14, 15,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);

    configure_widget.desktop_lrc_read_config_button = gtk_button_new_with_label(_("读取配置..."));
    gtk_widget_show(configure_widget.desktop_lrc_read_config_button);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_read_config_button, 0, 1, 15, 16,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);

    configure_widget.desktop_lrc_wirte_config_button = gtk_button_new_with_label(_("保存配置到..."));
    gtk_widget_show(configure_widget.desktop_lrc_wirte_config_button);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_wirte_config_button, 1, 5, 15, 16,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);

    configure_widget.desktop_lrc_label = gtk_label_new ("");
    gtk_label_set_selectable(GTK_LABEL(configure_widget.desktop_lrc_label), TRUE);
    gtk_widget_show (configure_widget.desktop_lrc_label);
    gtk_table_attach (GTK_TABLE (configure_widget.table6), configure_widget.desktop_lrc_label, 0, 4, 17, 18,(GtkAttachOptions) (0),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label12 = gtk_label_new (_("桌面\n歌词"));
    gtk_widget_show (configure_widget.label12);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (configure_widget.lrc_notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (configure_widget.lrc_notebook), 2), configure_widget.label12);

    configure_widget.label7 = gtk_label_new (_("\n音乐盒\n"));
    gtk_widget_show (configure_widget.label7);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (configure_widget.notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (configure_widget.notebook), 3), configure_widget.label7);

    configure_widget.table7 = gtk_table_new (2, 3, FALSE);
    gtk_widget_show (configure_widget.table7);
    gtk_container_add (GTK_CONTAINER (configure_widget.notebook), configure_widget.table7);
    gtk_container_set_border_width (GTK_CONTAINER (configure_widget.table7), 10);

    configure_widget.label33 = gtk_label_new (_("\344\270\213\350\275\275\347\233\256\345\275\225\357\274\232"));
    gtk_widget_show (configure_widget.label33);
    gtk_table_attach (GTK_TABLE (configure_widget.table7), configure_widget.label33, 0, 1, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_misc_set_alignment (GTK_MISC (configure_widget.label33), 0, 0.5);

    configure_widget.music_download_path_entry = gtk_entry_new ();
    gtk_widget_show (configure_widget.music_download_path_entry);
    gtk_table_attach (GTK_TABLE (configure_widget.table7), configure_widget.music_download_path_entry, 1, 2, 1, 2,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_invisible_char (GTK_ENTRY (configure_widget.music_download_path_entry), 9679);

    configure_widget.music_download_path_button = gtk_button_new_from_stock ("gtk-open");
    gtk_widget_show (configure_widget.music_download_path_button);
    gtk_table_attach (GTK_TABLE (configure_widget.table7), configure_widget.music_download_path_button, 2, 3, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.music_box_web_button = gtk_button_new_with_mnemonic (_("\345\274\225\346\223\216\357\274\232Mp3.Baidu.com \357\274\210\347\231\276\345\272\246\357\274\211"));
    gtk_widget_show (configure_widget.music_box_web_button);
    gtk_table_attach (GTK_TABLE (configure_widget.table7), configure_widget.music_box_web_button, 0, 3, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 10, 5);

    configure_widget.label8 = gtk_label_new (_("\n快捷键\n"));
    gtk_widget_show (configure_widget.label8);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (configure_widget.notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (configure_widget.notebook), 4), configure_widget.label8);

    configure_widget.table8 = gtk_table_new (6, 2, FALSE);
    gtk_widget_show (configure_widget.table8);
    gtk_container_add (GTK_CONTAINER (configure_widget.notebook), configure_widget.table8);
    gtk_container_set_border_width (GTK_CONTAINER (configure_widget.table8), 10);

    configure_widget.label35 = gtk_label_new (_("显示/隐藏"));
    gtk_widget_show (configure_widget.label35);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.label35, 0, 1, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label37 = gtk_label_new (_("播放/暂停"));
    gtk_widget_show (configure_widget.label37);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.label37, 0, 1, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label38 = gtk_label_new (_("上一首"));
    gtk_widget_show (configure_widget.label38);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.label38, 0, 1, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label39 = gtk_label_new (_("下一首"));
    gtk_widget_show (configure_widget.label39);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.label39, 0, 1, 3, 4,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label40 = gtk_label_new (_("停止"));
    gtk_widget_show (configure_widget.label40);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.label40, 0, 1, 4, 5,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label44 = gtk_label_new (_("重新搜索歌词"));
    gtk_widget_show (configure_widget.label44);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.label44, 0, 1, 5, 6,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.label41 = gtk_label_new (_("退出"));
    gtk_widget_show (configure_widget.label41);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.label41, 0, 1, 6, 7,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    configure_widget.quick_shortcut_entry_1 = gtk_entry_new ();
    gtk_widget_show (configure_widget.quick_shortcut_entry_1);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.quick_shortcut_entry_1, 1, 2, 0, 1,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_text (GTK_ENTRY (configure_widget.quick_shortcut_entry_1), "<Ctrl><Shift>i");
    gtk_entry_set_invisible_char (GTK_ENTRY (configure_widget.quick_shortcut_entry_1), 9679);

    configure_widget.quick_shortcut_entry_2 = gtk_entry_new ();
    gtk_widget_show (configure_widget.quick_shortcut_entry_2);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.quick_shortcut_entry_2, 1, 2, 1, 2,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_text (GTK_ENTRY (configure_widget.quick_shortcut_entry_2), "<Ctrl><Shift>p");
    gtk_entry_set_invisible_char (GTK_ENTRY (configure_widget.quick_shortcut_entry_2), 9679);

    configure_widget.quick_shortcut_entry_3 = gtk_entry_new ();
    gtk_widget_show (configure_widget.quick_shortcut_entry_3);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.quick_shortcut_entry_3, 1, 2, 2, 3,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_text (GTK_ENTRY (configure_widget.quick_shortcut_entry_3), "<Ctrl><Shift>b");
    gtk_entry_set_invisible_char (GTK_ENTRY (configure_widget.quick_shortcut_entry_3), 9679);

    configure_widget.quick_shortcut_entry_4 = gtk_entry_new ();
    gtk_widget_show (configure_widget.quick_shortcut_entry_4);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.quick_shortcut_entry_4, 1, 2, 3, 4,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_text (GTK_ENTRY (configure_widget.quick_shortcut_entry_4), "<Ctrl><Shift>n");
    gtk_entry_set_invisible_char (GTK_ENTRY (configure_widget.quick_shortcut_entry_4), 9679);

    configure_widget.quick_shortcut_entry_5 = gtk_entry_new ();
    gtk_widget_show (configure_widget.quick_shortcut_entry_5);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.quick_shortcut_entry_5, 1, 2, 4, 5,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_text (GTK_ENTRY (configure_widget.quick_shortcut_entry_5), "<Ctrl><Shift>s");
    gtk_entry_set_invisible_char (GTK_ENTRY (configure_widget.quick_shortcut_entry_5), 9679);

    configure_widget.quick_shortcut_entry_7 = gtk_entry_new ();
    gtk_widget_show (configure_widget.quick_shortcut_entry_7);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.quick_shortcut_entry_7, 1, 2, 5, 6,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_text (GTK_ENTRY (configure_widget.quick_shortcut_entry_7), "<Ctrl><Shift>d");
    gtk_entry_set_invisible_char (GTK_ENTRY (configure_widget.quick_shortcut_entry_7), 9679);

    configure_widget.quick_shortcut_entry_6 = gtk_entry_new ();
    gtk_widget_show (configure_widget.quick_shortcut_entry_6);
    gtk_table_attach (GTK_TABLE (configure_widget.table8), configure_widget.quick_shortcut_entry_6, 1, 2, 6, 7,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_text (GTK_ENTRY (configure_widget.quick_shortcut_entry_6), "<Ctrl><Shift>q");
    gtk_entry_set_invisible_char (GTK_ENTRY (configure_widget.quick_shortcut_entry_6), 9679);

    configure_widget.label34 = gtk_label_new ("\n\345\277\253\346\215\267\351\224\256\n");
    gtk_widget_show (configure_widget.label34);
    gtk_notebook_set_tab_label (GTK_NOTEBOOK (configure_widget.notebook), gtk_notebook_get_nth_page (GTK_NOTEBOOK (configure_widget.notebook), 5), configure_widget.label34);

    gtk_widget_set_size_request(configure_widget.notebook,0,0);
    gtk_widget_set_size_request(configure_widget.lrc_notebook,0,0);
    configure_to_configure_widget();

    g_signal_connect ((gpointer) configure_widget.auto_update_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.show_in_taskbar_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.decorated, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.show_tray_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.always_above_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.show_in_all_work_places_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.open_and_min_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.notify_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.last_pos_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.open_and_play_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.failed_and_stop_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.auto_jump_to_playing_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.use_lrc_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.use_search_lrc_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.search_lrc_with_filename_and_id3_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.edit_search_lrc_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.use_desktop_lrc_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.desktop_lrc_lock_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.desktop_lrc_use_kalaok_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.desktop_lrc_above_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.desktop_lrc_fontbutton, "font_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.widget_lrc_fontbutton, "font_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.info_name_fontbutton, "font_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.info_time_fontbutton, "font_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.lists_fontbutton, "font_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.lists_use_system_theme_checkbutton, "toggled",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.music_box_web_button, "clicked",G_CALLBACK (configure_music_web),NULL);

    //g_signal_connect ((gpointer) configure_widget.download_lrc_path_file_button, "clicked",G_CALLBACK (configure_file_button),NULL);
    // g_signal_connect ((gpointer) configure_widget.local_lrc_path_2_button, "clicked",G_CALLBACK (configure_file_button),NULL);
    //g_signal_connect ((gpointer) configure_widget.local_lrc_path_1_button, "clicked",G_CALLBACK (configure_file_button),NULL);
    // g_signal_connect ((gpointer) configure_widget.local_lrc_path_3_button, "clicked",G_CALLBACK (configure_file_button),NULL);
    //g_signal_connect ((gpointer) configure_widget.music_download_path_button, "clicked",G_CALLBACK (configure_file_button),NULL);

    g_signal_connect ((gpointer) configure_widget.desktop_lrc_colorbutton_1, "color_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.desktop_lrc_colorbutton_2, "color_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.desktop_lrc_colorbutton_3, "color_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.desktop_lrc_colorbutton_4, "color_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.desktop_lrc_colorbutton_5, "color_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.desktop_lrc_colorbutton_6, "color_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.desktop_lrc_word_stroke_colorbutton, "color_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.widget_lrc_colorbutton_1, "color_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.widget_lrc_colorbutton_2, "color_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.lists_back_color_button, "color_set",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.lists_front_color_button, "color_set",G_CALLBACK (update_configure),NULL);

    void search_lrc_key_help() {
        GtkWidget *dialog_real = gtk_message_dialog_new_with_markup(NULL,GTK_DIALOG_DESTROY_WITH_PARENT    ,GTK_MESSAGE_QUESTION,   GTK_BUTTONS_OK,"\t点击左边的设定搜索歌词黑名单按钮,会出现记事本程序，\n\t在文件里面的每一行都会作为关键字，如果播放的某歌曲的路径中包含这个关键字就会被忽略搜索歌词，\n\t比如有文件:/home/ice/music/a/a.mp3 就可以通过加一行 \n/music/a/\n ——作为关键字来屏蔽文件夹， \nmusic/a/a.mp3\n—— 就可以屏蔽文件， \na.mp3\n——可以屏蔽所有的名字里面有 a.mp3 的歌曲，nna.mp3 也会被屏蔽。\n\t被屏蔽后，在播放被屏蔽的歌曲时通过notify来通知。\n\t也可以手动编辑 ~/.iceplayer/black_list.conf来进行屏蔽歌词搜索");
        gtk_dialog_run((GtkDialog *)dialog_real);
        gtk_widget_destroy(dialog_real);
    }

    g_signal_connect ((gpointer) configure_widget.search_lrc_key_word_help_button , "clicked",G_CALLBACK (search_lrc_key_help),NULL);
    g_signal_connect ((gpointer) configure_widget.search_lrc_key_word_button, "clicked",G_CALLBACK (edit_search_lrc_key),NULL);

    g_signal_connect ((gpointer) configure_widget.desktop_lrc_read_config_button , "clicked",G_CALLBACK (read_desktop_lrc_config),NULL);
    g_signal_connect ((gpointer) configure_widget.desktop_lrc_wirte_config_button, "clicked",G_CALLBACK (wirte_desktop_lrc_config),NULL);
    g_signal_connect ((gpointer) configure_widget.sw_normal_button, "clicked",G_CALLBACK (configure_sw_page),(gpointer *)CONFIG_SW_NORMAL);
    g_signal_connect ((gpointer) configure_widget.sw_play_button, "clicked",G_CALLBACK (configure_sw_page),(gpointer *)CONFIG_SW_PLAY);
    g_signal_connect ((gpointer) configure_widget.sw_skin_button, "clicked",G_CALLBACK (configure_sw_page),(gpointer *)CONFIG_SW_SKIN);
    g_signal_connect ((gpointer) configure_widget.sw_lrc_button, "clicked",G_CALLBACK (configure_sw_page),(gpointer *)CONFIG_SW_LRC);
    g_signal_connect ((gpointer) configure_widget.sw_music_button, "clicked",G_CALLBACK (configure_sw_page),(gpointer *)CONFIG_SW_MUSIC_BOX);
    g_signal_connect ((gpointer) configure_widget.sw_quick_button, "clicked",G_CALLBACK (configure_sw_page),(gpointer *)CONFIG_SW_QUICK);
    /*
        g_signal_connect ((gpointer) configure_widget.skin_do_button, "clicked",G_CALLBACK (skin_do),NULL);
        g_signal_connect ((gpointer) configure_widget.delete_button, "clicked",G_CALLBACK (skin_del),NULL);
        g_signal_connect ((gpointer) configure_widget.skin_add_button, "clicked",G_CALLBACK (skin_add),NULL);
        g_signal_connect ((gpointer) configure_widget.skin_info_button, "clicked",G_CALLBACK (skin_info),NULL);
    */
    g_signal_connect ((gpointer) configure_widget.quick_shortcut_entry_1,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.quick_shortcut_entry_2,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.quick_shortcut_entry_3,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.quick_shortcut_entry_4,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.quick_shortcut_entry_5,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.quick_shortcut_entry_6,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.quick_shortcut_entry_7,"changed",G_CALLBACK (update_configure),NULL);

    g_signal_connect ((gpointer) configure_widget.local_lrc_path_1_entry,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.local_lrc_path_2_entry,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.local_lrc_path_3_entry,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.widget_back_pic_entry,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.back_pic_entry,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.desktop_lrc_word_stroke_entry,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.lrc_download_path_entry,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.local_lrc_path_1_entry,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.local_lrc_path_2_entry,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.local_lrc_path_3_entry,"changed",G_CALLBACK (update_configure),NULL);

    g_signal_connect ((gpointer) configure_widget.search_lrc_time_entry,"changed",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.skin_do_button, "clicked",G_CALLBACK (skin_menu),NULL);

    g_signal_connect ((gpointer) configure_widget.info_name_entry_x,"key-release-event",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.info_time_entry_x,"key-release-event",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.info_name_entry_y,"key-release-event",G_CALLBACK (update_configure),NULL);
    g_signal_connect ((gpointer) configure_widget.info_time_entry_y,"key-release-event",G_CALLBACK (update_configure),NULL);
}
void configure_init() {
    print_programming(("configure_init\n"));
    configure_to_iceplayer_widget();
}
gboolean configure_read() {
    print_programming(("configure_read\n"));
    GKeyFile *keyfile;
    GError *error=NULL;
    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile,get_configure_path(),0,NULL)) {
        print_error(("无法加载配置文件，请检查配置文件是否完整！\n"));
        g_key_file_free(keyfile);
        return FALSE;
    } else {
///
        configure.x=g_key_file_get_integer(keyfile, "常规", "X", &error);
        CONFIG_READ_FAILED_int(configure.x ,117 )
        configure.y=g_key_file_get_integer(keyfile, "常规", "Y", &error);
        CONFIG_READ_FAILED_int(configure.y ,92 )
        configure.width=g_key_file_get_integer(keyfile, "常规", "Width", &error);
        CONFIG_READ_FAILED_int(configure.width,341 )
        configure.height=g_key_file_get_integer(keyfile, "常规", "Height", &error);
        CONFIG_READ_FAILED_int(configure.height ,451 )
        configure.hpaned_pos=g_key_file_get_integer(keyfile, "常规", "歌词分栏位置", &error);
        CONFIG_READ_FAILED_int(configure.hpaned_pos ,332 )
        configure.list_hpaned_pos=g_key_file_get_integer(keyfile, "常规", "列表分栏位置", &error);
        CONFIG_READ_FAILED_int(configure.list_hpaned_pos ,42 )
        configure.volume=g_key_file_get_double(keyfile, "常规", "音量", &error);
        CONFIG_READ_FAILED_dou(configure.volume ,1 )
        configure.show_page = g_key_file_get_integer(keyfile, "常规", "显示页面", &error);
        CONFIG_READ_FAILED_int(configure.show_page ,0 )
        configure.play_mode = g_key_file_get_integer(keyfile, "常规", "播放模式", &error);
        CONFIG_READ_FAILED_int(configure.play_mode ,0 )
        configure.auto_jump_other_lists = g_key_file_get_integer(keyfile, "常规", "跨列表", &error);
        CONFIG_READ_FAILED_int(configure.auto_jump_other_lists ,0 )
        configure.window_alpha=g_key_file_get_double(keyfile, "常规", "窗口透明度", &error);
        CONFIG_READ_FAILED_dou(configure.window_alpha , 240)
///
        /*
        [常规]
        [播放]
        [皮肤]
        [歌词]
        [窗口歌词]
        [桌面歌词]
        [音乐盒]
        [快捷键]
        [迷你模式]
        [信息]
        */
        configure.auto_update = g_key_file_get_integer(keyfile, "常规", "开启自动更新", &error);
        CONFIG_READ_FAILED_int(configure.auto_update ,1 )
        configure.task_bar = g_key_file_get_integer(keyfile, "常规", "显示于任务栏", &error);
        CONFIG_READ_FAILED_int(configure.task_bar , 1)
        configure.decorated = g_key_file_get_integer(keyfile, "常规", "GTK外框架", &error);
        CONFIG_READ_FAILED_int(configure.decorated ,0 )
        configure.tray = g_key_file_get_integer(keyfile, "常规", "显示于通知区域", &error);
        CONFIG_READ_FAILED_int(configure.tray, 1)
        configure.above = g_key_file_get_integer(keyfile, "常规", "总在最前", &error);
        CONFIG_READ_FAILED_int(configure.above, 0)
        configure.all_work_places = g_key_file_get_integer(keyfile, "常规", "显示于所有工作区", &error);
        CONFIG_READ_FAILED_int(configure.all_work_places , 1)
        strcpy(configure.back_image, g_key_file_get_string_new(keyfile, "常规", "背景图片", &error));
        CONFIG_READ_FAILED_str(configure.back_image , "")
///
        configure.lists_use_system_theme = g_key_file_get_integer(keyfile, "常规", "列表使用系统主题", &error);
        CONFIG_READ_FAILED_int(configure.lists_use_system_theme ,1 )
        strcpy(configure.lists_font, g_key_file_get_string_new(keyfile, "常规", "列表自定义字体", &error));
        CONFIG_READ_FAILED_str(configure.lists_font , "Sans 10")
        strcpy(configure.lists_front_color, g_key_file_get_string_new(keyfile, "常规", "列表自定义前景色", &error));
        CONFIG_READ_FAILED_str(configure.lists_front_color , "#000000")
        strcpy(configure.lists_back_color, g_key_file_get_string_new(keyfile, "常规", "列表自定义背景色", &error));
        CONFIG_READ_FAILED_str(configure.lists_back_color , "#FFFFFF")
///
        configure.begin_play = g_key_file_get_integer(keyfile, "播放", "开启即播放", &error);
        CONFIG_READ_FAILED_int(configure.begin_play ,1 )
        configure.notify = g_key_file_get_integer(keyfile, "播放", "切歌notify通知", &error);
        CONFIG_READ_FAILED_int(configure.notify ,1 )
        configure.last_pos = g_key_file_get_integer(keyfile, "播放", "记录上次播放位置", &error);
        CONFIG_READ_FAILED_int(configure.last_pos ,1 )
        configure.last_pos_value = g_key_file_get_double(keyfile, "播放", "播放位置", &error);
        CONFIG_READ_FAILED_dou(configure.last_pos_value ,0 )
        configure.begin_min = g_key_file_get_integer(keyfile, "播放", "开启即最小化", &error);
        CONFIG_READ_FAILED_int(configure.begin_min , 0)
        strcpy(focus_lists,g_key_file_get_string_new(keyfile, "播放", "最后使用的列表", &error));
        CONFIG_READ_FAILED_str(focus_lists , "")
        sprintf(playing_song,"%d",g_key_file_get_integer(keyfile, "播放", "最后使用的歌曲序号", &error));
        CONFIG_READ_FAILED_str(playing_song , "0")
        configure.failed_stop = g_key_file_get_integer(keyfile, "播放", "不能播放时是否停止", &error);
        CONFIG_READ_FAILED_int(configure.failed_stop , 1)
        configure.auto_jump_to_play = g_key_file_get_integer(keyfile, "播放", "播放时列表焦点跟随", &error);
        CONFIG_READ_FAILED_int(configure.auto_jump_to_play ,1 )
        gsize length;
        gdouble *eq=g_key_file_get_double_list(keyfile, "播放", "均衡器", &length,&error);
        if (error!=NULL) {
            gint n=0;
            while (n<10) {
                configure.eq[n++]=0;
            }
            error = NULL;
        } else {
            gint n=0;
            while (n<10) {
                configure.eq[n++]=*eq;
                eq++;
            }
        }

///
        strcpy(configure.skin, g_key_file_get_string_new(keyfile, "皮肤", "皮肤名", &error));
        CONFIG_READ_FAILED_str(configure.skin , "mac_slax")
///
        configure.lrc =g_key_file_get_integer(keyfile, "歌词", "使用歌词", &error);
        CONFIG_READ_FAILED_int(configure.lrc , 1)
        configure.search_lrc = g_key_file_get_integer(keyfile, "歌词", "使用搜索功能", &error);
        CONFIG_READ_FAILED_int(configure.search_lrc , 1)
        strcpy(configure.search_lrc_time, g_key_file_get_string_new(keyfile, "歌词","歌词搜索时限", &error));
        CONFIG_READ_FAILED_str(configure.search_lrc_time , "15")
        configure.web_site_to_search =g_key_file_get_integer(keyfile, "歌词", "搜索引擎", &error);
        CONFIG_READ_FAILED_int(configure.web_site_to_search ,1 )
        configure.search_filename_or_id3 =g_key_file_get_integer(keyfile, "歌词", "以文件名搜索", &error);
        CONFIG_READ_FAILED_int(configure.search_filename_or_id3 ,1 )
        configure.edit_search_word =g_key_file_get_integer(keyfile, "歌词", "修改关键字", &error);
        CONFIG_READ_FAILED_int(configure.edit_search_word , 1)
        strcpy(configure.download_lrc_path, g_key_file_get_string_new(keyfile, "歌词","歌词下载目录", &error));
        CONFIG_READ_FAILED_str(configure.download_lrc_path , "")
        strcpy(configure.search_lrc_path_1, g_key_file_get_string_new(keyfile, "歌词","匹配本地歌词目录1", &error));
        CONFIG_READ_FAILED_str(configure.search_lrc_path_1 , "")
        strcpy(configure.search_lrc_path_2, g_key_file_get_string_new(keyfile, "歌词","匹配本地歌词目录2", &error));
        CONFIG_READ_FAILED_str(configure.search_lrc_path_2 , "")
        strcpy(configure.search_lrc_path_3, g_key_file_get_string_new(keyfile, "歌词","匹配本地歌词目录3", &error));
        CONFIG_READ_FAILED_str(configure.search_lrc_path_3 , "")

        strcpy(configure.widget_lrc_font, g_key_file_get_string_new(keyfile, "窗口歌词","窗口歌词字体", &error));
        CONFIG_READ_FAILED_str(configure.widget_lrc_font , "Sans 10")
        strcpy(configure.widget_lrc_front_color, g_key_file_get_string_new(keyfile, "窗口歌词","窗口歌词颜色一", &error));
        CONFIG_READ_FAILED_str(configure.widget_lrc_front_color , "#DA441C")
        strcpy(configure.widget_lrc_back_color, g_key_file_get_string_new(keyfile, "窗口歌词","窗口歌词颜色二", &error));
        CONFIG_READ_FAILED_str(configure.widget_lrc_back_color , "#000000")

        configure.widget_lrc_show_type= g_key_file_get_integer(keyfile, "窗口歌词","使用方式(独立窗口或者一体化窗口等)", &error);
        CONFIG_READ_FAILED_int(configure.widget_lrc_show_type ,0 )
        configure.widget_lrc_Independent_x= g_key_file_get_integer(keyfile, "窗口歌词","其X", &error);
        CONFIG_READ_FAILED_int(configure.widget_lrc_Independent_x ,458 )
        configure.widget_lrc_Independent_y= g_key_file_get_integer(keyfile, "窗口歌词","其Y", &error);
        CONFIG_READ_FAILED_int(configure.widget_lrc_Independent_y ,92 )
        configure.widget_lrc_Independent_width= g_key_file_get_integer(keyfile, "窗口歌词","其W", &error);
        CONFIG_READ_FAILED_int(configure.widget_lrc_Independent_width ,315 )
        configure.widget_lrc_Independent_height= g_key_file_get_integer(keyfile, "窗口歌词","其H", &error);
        CONFIG_READ_FAILED_int(configure.widget_lrc_Independent_height ,451 )
        configure.widget_lrc_Independent_above= g_key_file_get_integer(keyfile, "窗口歌词","其是否最前", &error);
        CONFIG_READ_FAILED_int(configure.widget_lrc_Independent_above , 0)
        strcpy(configure.widget_lrc_Independent_back_image,g_key_file_get_string_new(keyfile, "窗口歌词","其背景图片", &error));
        CONFIG_READ_FAILED_str(configure.widget_lrc_Independent_back_image , "")

        configure.desktop_lrc = g_key_file_get_integer(keyfile, "桌面歌词","使用桌面歌词", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc ,1 )
        configure.desktop_lrc_lock = g_key_file_get_integer(keyfile, "桌面歌词","锁定桌面歌词", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_lock ,0 )
        configure.desktop_lrc_kalaok = g_key_file_get_integer(keyfile, "桌面歌词","桌面歌词卡啦OK效果", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_kalaok ,1 )
        configure.desktop_lrc_above = g_key_file_get_integer(keyfile, "桌面歌词","桌面歌词最前", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_above , 1)
        configure.desktop_lrc_x = g_key_file_get_integer(keyfile, "桌面歌词","桌面歌词X", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_x , 117)
        configure.desktop_lrc_y = g_key_file_get_integer(keyfile, "桌面歌词","桌面歌词Y", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_y ,543 )
        configure.desktop_lrc_width =g_key_file_get_integer(keyfile, "桌面歌词","桌面歌词Width", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_width ,1011 )
        configure.desktop_lrc_height =g_key_file_get_integer(keyfile, "桌面歌词","桌面歌词Height", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_height ,61 )
        strcpy(configure.desktop_lrc_font, g_key_file_get_string_new(keyfile, "桌面歌词","桌面歌词字体", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_font , "Sans Bold 20")
        strcpy(configure.desktop_lrc_front_up_color, g_key_file_get_string_new(keyfile, "桌面歌词","桌面歌词前景色一", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_front_up_color , "#CBFE86")
        strcpy(configure.desktop_lrc_front_cen_color, g_key_file_get_string_new(keyfile, "桌面歌词","桌面歌词前景色二", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_front_cen_color , "#99FE11")
        strcpy(configure.desktop_lrc_front_down_color, g_key_file_get_string_new(keyfile, "桌面歌词","桌面歌词前景色三", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_front_down_color , "#BEFE0A")
        strcpy(configure.desktop_lrc_back_up_color, g_key_file_get_string_new(keyfile, "桌面歌词","桌面歌词背景色一", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_back_up_color , "#5DA71B")
        strcpy(configure.desktop_lrc_back_cen_color, g_key_file_get_string_new(keyfile, "桌面歌词","桌面歌词背景色二", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_back_cen_color , "#42AA00")
        strcpy(configure.desktop_lrc_back_down_color, g_key_file_get_string_new(keyfile, "桌面歌词","桌面歌词背景色三", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_back_down_color , "#126C00")
        configure.desktop_lrc_word_stroke = g_key_file_get_integer(keyfile, "桌面歌词","桌面歌词字体描边宽度", &error);
        CONFIG_READ_FAILED_int(configure.desktop_lrc_word_stroke , 8)
        strcpy(configure.desktop_lrc_word_stroke_color, g_key_file_get_string_new(keyfile, "桌面歌词","桌面歌词字体描边颜色", &error));
        CONFIG_READ_FAILED_str(configure.desktop_lrc_word_stroke_color , "#000000")
///
        // configure.music_web_site = g_key_file_get_integer(keyfile, "音乐盒","引擎", NULL);
        strcpy(configure.download_path, g_key_file_get_string_new(keyfile, "音乐盒","歌曲下载目录", &error));
        CONFIG_READ_FAILED_str(configure.download_path , "")
///
        strcpy(configure.quick_show_and_hide, g_key_file_get_string_new(keyfile, "快捷键","显示或隐藏", &error));
        CONFIG_READ_FAILED_str(configure.quick_show_and_hide , "<Ctrl><Shift>i")
        strcpy(configure.quick_play_and_pause, g_key_file_get_string_new(keyfile, "快捷键","播放或暂停", &error));
        CONFIG_READ_FAILED_str(configure.quick_play_and_pause , "<Ctrl><Shift>p")
        strcpy(configure.quick_previous , g_key_file_get_string_new(keyfile, "快捷键","上一首", &error));
        CONFIG_READ_FAILED_str(configure.quick_previous , "<Ctrl><Shift>b")
        strcpy(configure.quick_next , g_key_file_get_string_new(keyfile, "快捷键","下一首", &error));
        CONFIG_READ_FAILED_str(configure.quick_next , "<Ctrl><Shift>n")
        strcpy(configure.quick_stop, g_key_file_get_string_new(keyfile, "快捷键","停止", &error));
        CONFIG_READ_FAILED_str(configure.quick_stop , "<Ctrl><Shift>s")
        strcpy(configure.quick_research_lrc , g_key_file_get_string_new(keyfile, "快捷键","重新搜索歌词", &error));
        CONFIG_READ_FAILED_str(configure.quick_research_lrc , "<Ctrl><Shift>d")
        strcpy(configure.quick_quit , g_key_file_get_string_new(keyfile, "快捷键","退出", &error));
        CONFIG_READ_FAILED_str(configure.quick_quit , "<Ctrl><Shift>q")
        configure.mini=g_key_file_get_integer(keyfile, "迷你模式","使用迷你模式", &error);
        CONFIG_READ_FAILED_int(configure.mini ,0 )
        configure.mini_x = g_key_file_get_integer(keyfile, "迷你模式","迷你X", &error);
        CONFIG_READ_FAILED_int(configure.mini_x ,174 )
        configure.mini_y = g_key_file_get_integer(keyfile, "迷你模式","迷你Y", &error);
        CONFIG_READ_FAILED_int(configure.mini_y ,460 )

        strcpy( configure.info_name_font,g_key_file_get_string_new(keyfile, "信息","名称信息字体", &error));
        CONFIG_READ_FAILED_str(configure.info_name_font , "Sans 15")
        strcpy(configure.info_time_font,g_key_file_get_string_new(keyfile, "信息","时间信息字体", &error));;
        CONFIG_READ_FAILED_str(configure.info_time_font , "Sans 12")

        configure.info_name_x = g_key_file_get_integer(keyfile, "信息","名称信息X", &error);
        CONFIG_READ_FAILED_int(configure.info_name_x ,7 )
        configure.info_name_y = g_key_file_get_integer(keyfile, "信息","名称信息Y", &error);
        CONFIG_READ_FAILED_int(configure.info_name_y ,18 )

        configure.info_time_x = g_key_file_get_integer(keyfile, "信息","时间信息X", &error);
        CONFIG_READ_FAILED_int(configure.info_time_x ,-90)
        configure.info_time_y = g_key_file_get_integer(keyfile, "信息","时间信息Y", &error);
        CONFIG_READ_FAILED_int(configure.info_time_y ,-25 )
///
        g_key_file_free(keyfile);
        configure_wirte();
    }
    return TRUE;
}

gboolean configure_wirte() {
    print_programming("configure_wirte\n"                                                    );
    GKeyFile *keyfile;
    gchar *file_buf = NULL;
    gsize length;
    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile,get_configure_path(),0,NULL)) {
        print_error("无法加载配置文件，请检查配置文件是否完整！\n");
        g_key_file_free(keyfile);
        return FALSE;
    } else {
///
        g_key_file_set_integer(keyfile, "常规", "X", configure.x);
        g_key_file_set_integer(keyfile, "常规", "Y", configure.y);
        g_key_file_set_integer(keyfile, "常规", "Width", configure.width);
        g_key_file_set_integer(keyfile, "常规", "Height", configure.height);
        g_key_file_set_integer(keyfile, "常规", "歌词分栏位置", configure.hpaned_pos);
        g_key_file_set_integer(keyfile, "常规", "列表分栏位置", configure.list_hpaned_pos);
        g_key_file_set_double(keyfile, "常规", "音量", configure.volume );
        g_key_file_set_double(keyfile, "常规", "窗口透明度", configure.window_alpha );
        g_key_file_set_integer(keyfile, "常规", "显示页面", configure.show_page );
        g_key_file_set_integer(keyfile, "常规", "播放模式", configure.play_mode  );
        g_key_file_set_integer(keyfile, "常规", "跨列表", configure.auto_jump_other_lists );

        g_key_file_set_integer(keyfile, "常规", "开启自动更新", configure.auto_update  );
        g_key_file_set_integer(keyfile, "常规", "显示于任务栏", configure.task_bar);
        g_key_file_set_integer(keyfile, "常规", "GTK外框架", configure.decorated   );
        g_key_file_set_integer(keyfile, "常规", "总在最前", configure.above   );
        g_key_file_set_integer(keyfile, "常规", "显示于通知区域", configure.tray  );
        g_key_file_set_integer(keyfile, "常规", "显示于所有工作区", configure.all_work_places );
        g_key_file_set_string (keyfile, "常规", "背景图片",configure.back_image);
        g_key_file_set_integer(keyfile, "常规", "列表使用系统主题", configure.lists_use_system_theme);
        g_key_file_set_string(keyfile, "常规", "列表自定义字体", configure.lists_font);
        g_key_file_set_string(keyfile, "常规", "列表自定义前景色", configure.lists_front_color);
        g_key_file_set_string(keyfile, "常规", "列表自定义背景色", configure.lists_back_color);

        g_key_file_set_integer(keyfile, "播放", "开启即播放", configure.begin_play  );
        g_key_file_set_integer(keyfile, "播放", "切歌notify通知", configure.notify  );
        g_key_file_set_integer(keyfile, "播放", "记录上次播放位置", configure.last_pos  );
        g_key_file_set_double(keyfile, "播放", "播放位置", configure.last_pos_value  );
        g_key_file_set_integer(keyfile, "播放", "开启即最小化", configure.begin_min );
        g_key_file_set_string(keyfile,  "播放", "最后使用的列表", playing_list);
        g_key_file_set_integer(keyfile, "播放", "最后使用的歌曲序号", atoi(playing_song));
        g_key_file_set_integer(keyfile, "播放", "不能播放时是否停止", configure.failed_stop   );
        g_key_file_set_integer(keyfile, "播放", "播放时列表焦点跟随", configure.auto_jump_to_play );
        g_key_file_set_double_list(keyfile, "播放", "均衡器", configure.eq,10);

        g_key_file_set_string(keyfile, "皮肤", "皮肤名",  configure.skin);

        g_key_file_set_integer(keyfile, "歌词", "使用歌词", configure.lrc  );
        g_key_file_set_integer(keyfile, "歌词", "使用搜索功能", configure.search_lrc  );
        g_key_file_set_string(keyfile, "歌词","歌词搜索时限", configure.search_lrc_time   );
        g_key_file_set_integer(keyfile, "歌词", "搜索引擎", configure.web_site_to_search  );
        g_key_file_set_integer(keyfile, "歌词", "以文件名搜索", configure.search_filename_or_id3);
        g_key_file_set_integer(keyfile, "歌词", "修改关键字", configure.edit_search_word );

        g_key_file_set_string(keyfile, "歌词","歌词下载目录", configure.download_lrc_path   );
        g_key_file_set_string(keyfile, "歌词","匹配本地歌词目录1",  configure.search_lrc_path_1     );
        g_key_file_set_string(keyfile, "歌词","匹配本地歌词目录2",  configure.search_lrc_path_2   );
        g_key_file_set_string(keyfile, "歌词","匹配本地歌词目录3",  configure.search_lrc_path_3     );

        g_key_file_set_string(keyfile, "窗口歌词","窗口歌词字体",  configure.widget_lrc_font  );
        g_key_file_set_string(keyfile, "窗口歌词","窗口歌词颜色一",  configure.widget_lrc_front_color  );
        g_key_file_set_string(keyfile, "窗口歌词","窗口歌词颜色二",  configure.widget_lrc_back_color );
        g_key_file_set_integer(keyfile, "窗口歌词","使用方式(独立窗口或者一体化窗口等)", configure.widget_lrc_show_type);
        g_key_file_set_integer(keyfile, "窗口歌词","其X", configure.widget_lrc_Independent_x);
        g_key_file_set_integer(keyfile, "窗口歌词","其Y", configure.widget_lrc_Independent_y);
        g_key_file_set_integer(keyfile, "窗口歌词","其W", configure.widget_lrc_Independent_width);
        g_key_file_set_integer(keyfile, "窗口歌词","其H",  configure.widget_lrc_Independent_height);
        g_key_file_set_integer(keyfile, "窗口歌词","其是否最前", configure.widget_lrc_Independent_above);
        g_key_file_set_string (keyfile, "窗口歌词","其背景图片", configure.widget_lrc_Independent_back_image);

        g_key_file_set_integer(keyfile, "桌面歌词","使用桌面歌词", configure.desktop_lrc  );
        g_key_file_set_integer(keyfile, "桌面歌词","锁定桌面歌词", configure.desktop_lrc_lock  );
        g_key_file_set_integer(keyfile, "桌面歌词","桌面歌词卡啦OK效果", configure.desktop_lrc_kalaok  );
        g_key_file_set_integer(keyfile, "桌面歌词","桌面歌词最前", configure.desktop_lrc_above  );
        g_key_file_set_integer(keyfile, "桌面歌词","桌面歌词X", configure.desktop_lrc_x   );
        g_key_file_set_integer(keyfile, "桌面歌词","桌面歌词Y", configure.desktop_lrc_y );
        g_key_file_set_integer(keyfile, "桌面歌词","桌面歌词Width", configure.desktop_lrc_width );
        g_key_file_set_integer(keyfile, "桌面歌词","桌面歌词Height", configure.desktop_lrc_height );
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词字体",  configure.desktop_lrc_font   );
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词前景色一",  configure.desktop_lrc_front_up_color   );
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词前景色二",  configure.desktop_lrc_front_cen_color     );
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词前景色三",  configure.desktop_lrc_front_down_color  );
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词背景色一",  configure.desktop_lrc_back_up_color  );
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词背景色二",  configure.desktop_lrc_back_cen_color);
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词背景色三",  configure.desktop_lrc_back_down_color  );
        g_key_file_set_integer(keyfile, "桌面歌词","桌面歌词字体描边宽度",configure.desktop_lrc_word_stroke );
        g_key_file_set_string(keyfile, "桌面歌词","桌面歌词字体描边颜色",  configure.desktop_lrc_word_stroke_color  );

        g_key_file_set_integer(keyfile, "音乐盒","引擎", configure.music_web_site   );
        g_key_file_set_string(keyfile, "音乐盒","歌曲下载目录",  configure.download_path   );

        g_key_file_set_string(keyfile, "快捷键","显示或隐藏",  configure.quick_show_and_hide  );
        g_key_file_set_string(keyfile, "快捷键","播放或暂停",  configure.quick_play_and_pause    );
        g_key_file_set_string(keyfile, "快捷键","上一首",  configure.quick_previous       );
        g_key_file_set_string(keyfile, "快捷键","下一首",  configure.quick_next      );
        g_key_file_set_string(keyfile, "快捷键","停止",  configure.quick_stop    );
        g_key_file_set_string(keyfile, "快捷键","重新搜索歌词",  configure.quick_research_lrc   );
        g_key_file_set_string(keyfile, "快捷键","退出",  configure.quick_quit   );

        g_key_file_set_integer(keyfile, "迷你模式","使用迷你模式", configure.mini   );
        g_key_file_set_integer(keyfile, "迷你模式","迷你X", configure.mini_x   );
        g_key_file_set_integer(keyfile, "迷你模式","迷你Y", configure.mini_y   );

        g_key_file_set_string(keyfile, "信息","名称信息字体", configure.info_name_font   );
        g_key_file_set_string(keyfile, "信息","时间信息字体", configure.info_time_font   );

        g_key_file_set_integer(keyfile, "信息","名称信息X", configure.info_name_x   );
        g_key_file_set_integer(keyfile, "信息","名称信息Y", configure.info_name_y   );
        g_key_file_set_integer(keyfile, "信息","时间信息X", configure.info_time_x   );
        g_key_file_set_integer(keyfile, "信息","时间信息Y", configure.info_time_y   );

        file_buf = g_key_file_to_data(keyfile, &length, NULL);
        g_file_set_contents(get_configure_path(), file_buf, -1, NULL);

        g_key_file_free(keyfile);
    }
    if (file_buf)g_free_n(file_buf);
    return TRUE;
}
void change_skin(GtkWidget *wi,gchar *n) {
  print_programming(("change_skin\n"));
  configure.show_page = SWITCH_LIST;

  if (n!=NULL) {
    strcpy(configure.skin,n);
  }

  inline void remove(GtkWidget *widget) {
    gtk_container_remove_new(GTK_CONTAINER(widget->parent), widget);
  }
  inline void add(GtkWidget *parent,GtkWidget *widget) {
    gtk_widget_show(widget);
    gtk_container_add(GTK_CONTAINER(parent),widget);
  }
  remove(iceplayer.menu_image );

  //remove(iceplayer.back_image );
  remove(iceplayer.mini_image );
  remove(iceplayer.above_image );
  remove(iceplayer.min_image );
  remove(iceplayer.max_image );
  remove(iceplayer.close_image );
  remove(iceplayer.prev_image );
  remove(iceplayer.play_image );
  remove(iceplayer.next_image );
  remove(iceplayer.stop_image );

  remove(iceplayer.mini_mini_image );
  remove(iceplayer.mini_close_image );
  remove(iceplayer.mini_prev_image );
  remove(iceplayer.mini_play_image );
  remove(iceplayer.mini_next_image );
  remove(iceplayer.mini_lists_image );
  remove(iceplayer.mini_songs_image );
  remove(iceplayer.mini_stop_image );

  remove(iceplayer.open_image );
  remove(iceplayer.progress_left_image );
  remove(iceplayer.progress_node_image );
  remove(iceplayer.progress_right_image );
  remove(iceplayer.volume_image );
  remove(iceplayer.switch_list_image );
  remove(iceplayer.switch_lrc_image );
  remove(iceplayer.switch_music_box_image );
  remove(iceplayer.switch_configure_image );
  remove(iceplayer.switch_help_image );
  //remove(iceplayer.resize_left_image );
  //remove(iceplayer.resize_top_image );
  //remove(iceplayer.resize_buttom_image );
  //remove(iceplayer.resize_right_image );

  remove(iceplayer.resize_buttom_left_image );
  remove(iceplayer.resize_buttom_right_image );
  remove(iceplayer.resize_top_right_image );
  remove(iceplayer.resize_top_left_image );

  remove(widget_lrc.above_image );
  remove(widget_lrc.min_image );
  remove(widget_lrc.max_image );
  remove(widget_lrc.close_image);

  //remove(widget_lrc.left_image );
  //remove(widget_lrc.right_image );
  //remove(widget_lrc.top_image );
  //remove(widget_lrc.buttom_image );
  remove(widget_lrc.top_left_image);
  remove(widget_lrc.top_right_image);
  remove(widget_lrc.buttom_left_image);
  remove(widget_lrc.buttom_right_image);

  ui_init_skin();
  read_skin_configure();

  add (iceplayer.menu_eventbox, iceplayer.menu_image);
  add (iceplayer.mini_eventbox, iceplayer.mini_image);
  add (iceplayer.above_eventbox, iceplayer.above_image);
  add (iceplayer.min_eventbox, iceplayer.min_image);
  add (iceplayer.max_eventbox, iceplayer.max_image);
  add (iceplayer.close_eventbox, iceplayer.close_image);
  add (iceplayer.prev_eventbox, iceplayer.prev_image);
  add (iceplayer.play_eventbox, iceplayer.play_image);
  add (iceplayer.next_eventbox, iceplayer.next_image);
  add (iceplayer.stop_eventbox, iceplayer.stop_image);

  add (iceplayer.mini_mini_eventbox, iceplayer.mini_mini_image);
  add (iceplayer.mini_close_eventbox, iceplayer.mini_close_image);
  add (iceplayer.mini_prev_eventbox, iceplayer.mini_prev_image);
  add (iceplayer.mini_play_eventbox, iceplayer.mini_play_image);
  add (iceplayer.mini_next_eventbox, iceplayer.mini_next_image);
  add (iceplayer.mini_lists_eventbox, iceplayer.mini_lists_image);
  add (iceplayer.mini_songs_eventbox, iceplayer.mini_songs_image);
  add (iceplayer.mini_stop_eventbox, iceplayer.mini_stop_image);

  add (iceplayer.open_eventbox, iceplayer.open_image);
  add (iceplayer.progress_left_eventbox, iceplayer.progress_left_image);
  add (iceplayer.progress_node_eventbox, iceplayer.progress_node_image);
  add (iceplayer.progress_right_eventbox, iceplayer.progress_right_image);
  add (iceplayer.volume_eventbox, iceplayer.volume_image);
  add (iceplayer.switch_list_eventbox, iceplayer.switch_list_image);
  add (iceplayer.switch_lrc_eventbox, iceplayer.switch_lrc_image);
  add (iceplayer.switch_music_box_eventbox, iceplayer.switch_music_box_image);
  add (iceplayer.switch_configure_eventbox, iceplayer.switch_configure_image);
  add (iceplayer.switch_help_eventbox, iceplayer.switch_help_image);
  add (iceplayer.resize_buttom_left_eventbox, iceplayer.resize_buttom_left_image);
  add (iceplayer.resize_buttom_right_eventbox, iceplayer.resize_buttom_right_image);
  //add (iceplayer.resize_left_eventbox, iceplayer.resize_left_image);
  add (iceplayer.resize_top_left_eventbox, iceplayer.resize_top_left_image);
  //add (iceplayer.resize_top_eventbox, iceplayer.resize_top_image);
  add (iceplayer.resize_top_right_eventbox, iceplayer.resize_top_right_image);
  // add (iceplayer.resize_buttom_eventbox, iceplayer.resize_buttom_image);
  //add (iceplayer.resize_right_eventbox, iceplayer.resize_right_image);

  add (widget_lrc.above_eventbox,widget_lrc.above_image);
  add (widget_lrc.min_eventbox,widget_lrc.min_image);
  add (widget_lrc.max_eventbox,widget_lrc.max_image);
  add (widget_lrc.close_eventbox,widget_lrc.close_image);

  //add (widget_lrc.left_eventbox,widget_lrc.left_image);
  //add (widget_lrc.right_eventbox,widget_lrc.right_image);
  //add (widget_lrc.top_eventbox,widget_lrc.top_image);
  //add (widget_lrc.buttom_eventbox,widget_lrc.buttom_image);
  add (widget_lrc.top_left_eventbox,widget_lrc.top_left_image);
  add (widget_lrc.top_right_eventbox,widget_lrc.top_right_image);
  add (widget_lrc.buttom_left_eventbox,widget_lrc.buttom_left_image);
  add (widget_lrc.buttom_right_eventbox,widget_lrc.buttom_right_image);

  updata_iceplayer_bg();
  updata_widget_lrc_bg();

  widget_lrc_size_changed();

  progress_refresh_init();
  set_skin_configure_to_iceplayer_widget();

  gtk_fixed_move_new (iceplayer.mini_fixed, iceplayer.mini_close_eventbox,  skin.mini_close_x, skin.mini_close_y);
  gtk_fixed_move_new (iceplayer.mini_fixed, iceplayer.mini_mini_eventbox,  skin.mini_mini_x, skin.mini_mini_y);
  gtk_fixed_move_new (iceplayer.mini_fixed, iceplayer.mini_prev_eventbox, skin.mini_prev_x, skin.mini_prev_y);
  gtk_fixed_move_new (iceplayer.mini_fixed, iceplayer.mini_play_eventbox, skin.mini_play_x,skin.mini_play_y);
  gtk_fixed_move_new (iceplayer.mini_fixed, iceplayer.mini_next_eventbox, skin.mini_next_x, skin.mini_next_y);
  gtk_fixed_move_new (iceplayer.mini_fixed, iceplayer.mini_stop_eventbox, skin.mini_stop_x, skin.mini_stop_y);
  gtk_fixed_move_new (iceplayer.mini_fixed, iceplayer.mini_songs_eventbox,  skin.mini_songs_x, skin.mini_songs_y);
  gtk_fixed_move_new (iceplayer.mini_fixed, iceplayer.mini_lists_eventbox,  skin.mini_lists_x, skin.mini_lists_y);

  gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.name_label,  skin.info_name_x, skin.info_name_y);
  gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.time_label,  skin.info_time_x, skin.info_time_y);

  configure.info_name_x = skin.info_name_x;
  configure.info_name_y = skin.info_name_y;
  configure.info_time_x = skin.info_time_x;
  configure.info_time_y = skin.info_time_y;

    gchar info_text[256];
    sprintf(info_text,"%d",configure.info_name_x);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.info_name_entry_x),info_text);
    sprintf(info_text,"%d",configure.info_name_y);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.info_name_entry_y),info_text);
    sprintf(info_text,"%d",configure.info_time_x);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.info_time_entry_x),info_text);
    sprintf(info_text,"%d",configure.info_time_y);
    gtk_entry_set_text(GTK_ENTRY(configure_widget.info_time_entry_y),info_text);

    updata_mini_image();
    gtk_widget_queue_draw(iceplayer.mini_window);
    gtk_window_resize(GTK_WINDOW(iceplayer.mini_window), skin.mini_w, skin.mini_h);

    fit_size();
    update_configure();
}

void configure_init_variable() {
    print_programming(("configure_init_variable\n"));
    configure_read();
    desktop_lrc_word_height = 30;
    widget_lrc_word_height = 30;
}

void skin_menu() {
    print_programming(("skin_menu\n"));
    GtkWidget * menu = gtk_menu_new();

    gchar *path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(path,g_get_home_dir());
    strcat(path,"/.iceplayer/skin/");
    gchar *skin_dir= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

    DIR *dir=opendir(path);
    struct dirent *ptr=readdir(dir);
    while (ptr) {
        if (ptr->d_name[0]!='.') {
            gchar *path2= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
            strcpy(path2,path);
            strcat(path2,ptr->d_name);
            DIR * dir2 = opendir(path2);
            g_free_n(path2);
            if (dir2) {
                closedir(dir2);
                strcpy(skin_dir," ~/.iceplayer | ");
                GtkWidget *menu_item = gtk_image_menu_item_new_with_label(strcat(skin_dir,ptr->d_name));
                gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_item);
                g_signal_connect(menu_item ,"activate",G_CALLBACK(change_skin),(gpointer)ptr->d_name);
            }
        }
        ptr=readdir(dir);
    }

    strcpy(path,"@datadir@/iceplayer/skin/");

    dir=opendir(path);
    ptr=readdir(dir);
    while (ptr) {
        if (ptr->d_name[0]!='.') {
            gchar *path2= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
            strcpy(path2,path);
            strcat(path2,ptr->d_name);
            DIR * dir2 = opendir(path2);
            g_free_n(path2);
            if (dir2) {
                closedir(dir2);
                strcpy(skin_dir," @datadir@/iceplayer | ");
                GtkWidget *menu_item = gtk_image_menu_item_new_with_label(strcat(skin_dir,ptr->d_name));
                gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_item);
                g_signal_connect(menu_item ,"activate",G_CALLBACK(change_skin),(gpointer)ptr->d_name);
            }
        }
        ptr=readdir(dir);
    }

    gtk_widget_show_all(menu);
    popup_menu(GTK_MENU(menu),1,1);
    g_free_n(path);
    g_free_n(skin_dir);
}


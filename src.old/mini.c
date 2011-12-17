#include "core.h"
#include "single.h"
#include "list.h"
#include "ui.h"

static cairo_surface_t *image = NULL;

gchar *get_mini_back() {
    print_programming(("get_mini_back\n"));
    static gchar path[STRINGS_LENGTH];
    strcpy(path,get_skin_dir());
    strcat(path,"mini_main.png");
    return path;
}
void updata_mini_image(){
    image = cairo_image_surface_create_from_png_new(image,get_mini_back());
}
gboolean on_mini_expose() {
    print_programming(("on_mini_expose\n"));
    GdkPixbuf *pixbuf = NULL;
    GdkBitmap *bitmap = NULL;
    GdkPixmap *pixmap = NULL;

    pixbuf = gdk_pixbuf_new_from_file(get_mini_back(), NULL);       // gdk函数读取png文件

    if(!gdk_screen_is_composited(gdk_screen_get_default ())) {
        gdk_pixbuf_render_pixmap_and_mask(pixbuf, &pixmap, &bitmap, 128);    // alpha小于128认为透明
    } else {
        gdk_pixbuf_render_pixmap_and_mask(pixbuf, &pixmap, &bitmap, 0);
    }
    gtk_widget_shape_combine_mask(iceplayer.mini_window, bitmap, 0, 0);          // 设置透明蒙板


    cairo_t *cr;
    cr = gdk_cairo_create(iceplayer.mini_win_eventbox->window);
    cairo_set_source_surface(cr, image, 0, 0);
    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
    cairo_paint(cr);
    cairo_destroy(cr);

    return FALSE;
}
void on_mini_lists_button_press(GtkWidget *menu) {
    print_programming(("on_mini_lists_button_press\n"));
    strcpy(focus_lists,gtk_label_get_text (GTK_LABEL (GTK_BIN (menu)->child)));
    show_lists(NULL);
    show_songs(NULL);
   // list_menu_jump_to_playing_song();
}
void single_lists() {
    print_programming(("single_lists\n"));
    GtkWidget * menu=gtk_menu_new();
    iceplayer_lists *lists=&main_lists;

    while(lists) {
        GtkWidget *menu_ =gtk_image_menu_item_new_with_label(_(lists->name));
        if(strcmp(lists->name,playing_list)==0)
            gtk_label_set_markup ((GtkLabel *)GTK_BIN (menu_)->child, g_markup_printf_escaped ("<span weight=\"bold\">%s</span>",gtk_label_get_text (GTK_LABEL (GTK_BIN (menu_)->child))));
        gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_);
        g_signal_connect((gpointer) menu_ ,"activate" ,G_CALLBACK(on_mini_lists_button_press),NULL);
        lists=lists->next;
    }

    gtk_widget_show_all(menu);
    popup_menu(menu,2,2);
}
void on_mini_songs_button_press(GtkWidget *menu ,gchar *uri) {
    print_programming(("on_mini_songs_button_press\n"));
    strcpy(focus_song,uri);
    gint a=0;
    gchar n[STRINGS_LENGTH];
    strcpy(n,gtk_label_get_text (GTK_LABEL (GTK_BIN (menu)->child)));
    sscanf(n,"%d:",&a);
    strcpy(playing_list,focus_lists);
    sprintf(playing_song,"%d",a);
    play_uri();
    show_lists(NULL);
    show_songs(NULL);
    list_menu_jump_to_playing_song(NULL);
}
void single_songs() {
    print_programming(("single_songs\n"));
    GtkWidget * menu=gtk_menu_new();
    iceplayer_lists *lists=find_lists_from_name(focus_lists);
    iceplayer_songs *songs =lists->songs;
    gint a=0;
    gchar n[STRINGS_LENGTH];
    while(songs) {
        sprintf(n,"%d:%s",a,songs_uri_get_show_name(songs->uri));
        GtkWidget *menu_ =gtk_image_menu_item_new_with_label(n);

        if(strcmp(n,playing_song)==0)
            gtk_label_set_markup ((GtkLabel *)GTK_BIN (menu_)->child, g_markup_printf_escaped ("<span weight=\"bold\">%s</span>",n));
        gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_);
        g_signal_connect((gpointer) menu_ ,"activate" ,G_CALLBACK(on_mini_songs_button_press),(gpointer *)songs->uri);
        songs=songs->next;
        a++;
    }

    gtk_widget_show_all(menu);
    popup_menu(menu,2,2);

    ///FIXME:下面是对mini模式的一个更新，自动调节菜单位置使得鼠标位置在正在播放的歌曲上面，在11.04无法使用，仅能在10.10以下？
    /*

    gtk_menu_popup (GTK_MENU(menu),NULL,NULL,NULL,NULL,3,gtk_get_current_event_time());
    gint x,y;
    gint w,h;
    gint sx,sy;
    gtk_window_get_size ( (GtkWindow *)gtk_widget_get_toplevel((GtkWidget *)menu),&w,&h);
    gint h2=h;
    h2=h2/a;
    h2=h2*atoi(playing_song);

    gdk_window_get_pointer( NULL,&x,&y,NULL);
    gtk_window_get_position( (GtkWindow *)gtk_widget_get_toplevel((GtkWidget *)menu),&sx,&sy);
    if (h>=gdk_screen_height()-y)
        gtk_window_move((GtkWindow *) gtk_widget_get_toplevel((GtkWidget *)menu),sx,sy-h2);
    else
        gtk_window_move((GtkWindow *)gtk_widget_get_toplevel((GtkWidget *)menu),sx,sy+h2);

    if (w>=gdk_screen_width()-x)
        gtk_window_move((GtkWindow *)gtk_widget_get_toplevel((GtkWidget *)menu),sx-2,sy);
    else
        gtk_window_move((GtkWindow *)gtk_widget_get_toplevel((GtkWidget *)menu),sx+2,sy);

*/
}
void init_mini() {
    print_debug("初始化 mini");
    GdkScreen *screen;
    GdkColormap *colormap;

    image = cairo_image_surface_create_from_png_new(image,get_mini_back());
    iceplayer.mini_window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title (GTK_WINDOW (iceplayer.mini_window), _("iceplayer mini"));
    gtk_window_set_icon(GTK_WINDOW(iceplayer.mini_window),(GdkPixbuf *)gdk_pixbuf_new_from_file("/usr/share/iceplayer/iceplayer.png",NULL));
    gtk_window_set_decorated(GTK_WINDOW(iceplayer.mini_window), FALSE);
    gtk_widget_set_app_paintable(iceplayer.mini_window, TRUE);
    gtk_window_set_keep_above(GTK_WINDOW(iceplayer.mini_window),TRUE);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(iceplayer.mini_window),TRUE);//任务栏
    gtk_window_resize(GTK_WINDOW(iceplayer.mini_window), skin.mini_w, skin.mini_h);
    gtk_window_stick (GTK_WINDOW(iceplayer.mini_window));//工作区
    gtk_window_set_skip_pager_hint(GTK_WINDOW(iceplayer.mini_window),FALSE);

    iceplayer.mini_win_eventbox = gtk_event_box_new ();
    gtk_widget_set_has_window(iceplayer.mini_win_eventbox, TRUE);
    gtk_widget_set_app_paintable(iceplayer.mini_win_eventbox, TRUE);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.mini_win_eventbox), TRUE);
    gtk_widget_set_usize(iceplayer.mini_win_eventbox,skin.mini_w,skin.mini_h);

    iceplayer.mini_fixed=gtk_fixed_new();
    gtk_container_add (GTK_CONTAINER (iceplayer.mini_window), iceplayer.mini_win_eventbox);
    gtk_container_add (GTK_CONTAINER (iceplayer.mini_win_eventbox), iceplayer.mini_fixed);

    iceplayer.mini_close_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.mini_close_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.mini_close_eventbox), iceplayer.mini_close_image);
    gtk_fixed_put(GTK_FIXED(iceplayer.mini_fixed),
		  iceplayer.mini_close_eventbox,
		  skin.mini_close_x,skin.mini_close_y);

    iceplayer.mini_lists_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.mini_lists_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.mini_lists_eventbox), iceplayer.mini_lists_image);
    gtk_fixed_put(GTK_FIXED(iceplayer.mini_fixed),
		  iceplayer.mini_lists_eventbox,
		  skin.mini_lists_x,skin.mini_lists_y);

    iceplayer.mini_mini_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.mini_mini_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.mini_mini_eventbox), iceplayer.mini_mini_image);
    gtk_fixed_put(GTK_FIXED(iceplayer.mini_fixed),
		  iceplayer.mini_mini_eventbox,
		  skin.mini_mini_x,skin.mini_mini_y);

    iceplayer.mini_next_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.mini_next_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.mini_next_eventbox), iceplayer.mini_next_image);
    gtk_fixed_put(GTK_FIXED(iceplayer.mini_fixed),
		  iceplayer.mini_next_eventbox,
		  skin.mini_next_x,skin.mini_next_y);

    iceplayer.mini_play_eventbox= gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.mini_play_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.mini_play_eventbox), iceplayer.mini_play_image);
    gtk_fixed_put(GTK_FIXED(iceplayer.mini_fixed),
		  iceplayer.mini_play_eventbox,
		  skin.mini_play_x,skin.mini_play_y);

    iceplayer.mini_prev_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.mini_prev_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.mini_prev_eventbox), iceplayer.mini_prev_image);
    gtk_fixed_put(GTK_FIXED(iceplayer.mini_fixed),
		  iceplayer.mini_prev_eventbox,
		  skin.mini_prev_x,skin.mini_prev_y);

    iceplayer.mini_stop_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.mini_stop_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.mini_stop_eventbox), iceplayer.mini_stop_image);
    gtk_fixed_put(GTK_FIXED(iceplayer.mini_fixed),
		  iceplayer.mini_stop_eventbox,
		  skin.mini_stop_x,skin.mini_stop_y);

    iceplayer.mini_songs_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.mini_songs_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.mini_songs_eventbox), iceplayer.mini_songs_image);
    gtk_fixed_put(GTK_FIXED(iceplayer.mini_fixed),
		  iceplayer.mini_songs_eventbox,
		  skin.mini_songs_x,skin.mini_songs_y);

    inline void ass(GtkWidget *widget,GdkEventButton *event,gpointer user_data)
    {
      if (event->button==3)
	main_window_menu();
      if (event->button==1)
	gtk_window_begin_move_drag(GTK_WINDOW(iceplayer.mini_window),1,event->x_root,event->y_root,event->time);
    }

    g_signal_connect(G_OBJECT(iceplayer.mini_win_eventbox), "expose-event",G_CALLBACK(on_mini_expose), NULL);

    g_signal_connect(G_OBJECT(iceplayer.mini_win_eventbox), "button_press_event",G_CALLBACK(ass), NULL);

    g_signal_connect ((gpointer) iceplayer.mini_prev_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_PREV);
    g_signal_connect ((gpointer) iceplayer.mini_prev_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_PREV);
    g_signal_connect ((gpointer) iceplayer.mini_play_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_PLAY);
    g_signal_connect ((gpointer) iceplayer.mini_play_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_PLAY);
    g_signal_connect ((gpointer) iceplayer.mini_next_eventbox,"enter_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_NEXT);
    g_signal_connect ((gpointer) iceplayer.mini_next_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_NEXT);
    g_signal_connect ((gpointer) iceplayer.mini_songs_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_SONGS);
    g_signal_connect ((gpointer) iceplayer.mini_songs_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_SONGS);
    g_signal_connect ((gpointer) iceplayer.mini_stop_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_STOP);
    g_signal_connect ((gpointer) iceplayer.mini_stop_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_STOP);
    g_signal_connect ((gpointer) iceplayer.mini_lists_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_LISTS);
    g_signal_connect ((gpointer) iceplayer.mini_lists_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_LISTS);
    g_signal_connect ((gpointer) iceplayer.mini_mini_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_MINI);
    g_signal_connect ((gpointer) iceplayer.mini_mini_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_MINI);
    g_signal_connect ((gpointer) iceplayer.mini_close_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_CLOSE);
    g_signal_connect ((gpointer) iceplayer.mini_close_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)MINI_CLOSE);

    g_signal_connect ((gpointer) iceplayer.mini_prev_eventbox,   "button_press_event",G_CALLBACK (single_prev),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_play_eventbox,   "button_press_event",G_CALLBACK (single_play),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_next_eventbox,   "button_press_event",G_CALLBACK (single_next),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_stop_eventbox,   "button_press_event",G_CALLBACK (single_stop),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_prev_eventbox,   "button_release_event",G_CALLBACK (single_prev),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_play_eventbox,   "button_release_event",G_CALLBACK (single_play),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_next_eventbox,   "button_release_event",G_CALLBACK (single_next),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_stop_eventbox,   "button_release_event",G_CALLBACK (single_stop),NULL);

    g_signal_connect ((gpointer) iceplayer.mini_lists_eventbox,   "button_press_event",G_CALLBACK (single_lists),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_songs_eventbox,   "button_press_event",G_CALLBACK (single_songs),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_mini_eventbox,   "button_press_event",G_CALLBACK (mini),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_close_eventbox,   "button_press_event",G_CALLBACK (quit),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_lists_eventbox,   "button_release_event",G_CALLBACK (single_lists),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_songs_eventbox,   "button_release_event",G_CALLBACK (single_songs),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_mini_eventbox,   "button_release_event",G_CALLBACK (mini),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_close_eventbox,   "button_press_event",G_CALLBACK (quit),NULL);


    screen = gtk_widget_get_screen(iceplayer.mini_window);       // 重要
    colormap = gdk_screen_get_rgba_colormap(screen);
    gtk_widget_set_colormap(iceplayer.mini_window, colormap);

    gtk_widget_show_all(iceplayer.mini_window);
}

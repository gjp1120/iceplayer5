#include "about.h"
#include "core.h"
#include "widget_lrc.h"
#include "single.h"
#include "list.h"
#include "ui.h"
#include "configure.h"
#include "lrc.h"
#include "common.h"

iceplayer_skin	skin;
iceplayer_widget iceplayer;
gint windows_state;
gboolean use_usr_skin;
///widget:

void updata_iceplayer_bg() {
    print_programming(("updata_iceplayer_bg\n"));
    gtk_widget_queue_draw(iceplayer.window);
}

void ui_init() {
    print_programming("ui_init\n");
    print_debug("建立主要窗体");
    gtk_window_set_default_icon_from_file(ICON, NULL);
    iceplayer.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_title (GTK_WINDOW (iceplayer.window), _("iceplayer 音乐播放器"));
    gtk_window_stick (GTK_WINDOW(iceplayer.window));

    gtk_window_set_icon(GTK_WINDOW(iceplayer.window),(GdkPixbuf *)gdk_pixbuf_new_from_file(ICON, NULL));
    iceplayer.trayIcon=gtk_status_icon_new_from_file("@datadir@/iceplayer/iceplayer_tray.png");
    gtk_status_icon_set_tooltip(iceplayer.trayIcon,"iceplayer by jasy.ice@163.com");
    gtk_status_icon_set_visible(GTK_STATUS_ICON(iceplayer.trayIcon),TRUE);
    gtk_widget_set_app_paintable(iceplayer.window, TRUE);
    iceplayer_image = cairo_image_surface_create_from_png(get_iceplayer_back_pic());

    iceplayer.table = gtk_table_new (5, 3, FALSE);
    iceplayer.eventbox = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(iceplayer.eventbox), TRUE);
    gtk_widget_set_has_window(iceplayer.eventbox, TRUE);
    gtk_widget_set_app_paintable(iceplayer.eventbox, TRUE);
    gtk_container_add(GTK_CONTAINER(iceplayer.eventbox), iceplayer.table);
    gtk_container_add (GTK_CONTAINER (iceplayer.window), iceplayer.eventbox);

    iceplayer.main_fixed = gtk_fixed_new ();
    gtk_table_attach (GTK_TABLE (iceplayer.table), iceplayer.main_fixed, 1, 2, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);

    iceplayer.back_eventbox_main = gtk_event_box_new ();
    gtk_fixed_put (GTK_FIXED (iceplayer.main_fixed), iceplayer.back_eventbox_main, 0, 0);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.back_eventbox_main), FALSE);

    iceplayer.menu_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.menu_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.menu_eventbox), iceplayer.menu_image);
///control
    iceplayer.mini_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.mini_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.mini_eventbox), iceplayer.mini_image);

    iceplayer.above_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.above_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.above_eventbox), iceplayer.above_image);

    iceplayer.min_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.min_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.min_eventbox), iceplayer.min_image);

    iceplayer.max_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.max_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.max_eventbox), iceplayer.max_image);

    iceplayer.close_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.close_eventbox), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.close_eventbox), iceplayer.close_image);
///control
    iceplayer.prev_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.prev_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.prev_eventbox), iceplayer.prev_image);

    iceplayer.play_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.play_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.play_eventbox), iceplayer.play_image);

    iceplayer.next_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.next_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.next_eventbox), iceplayer.next_image);

    iceplayer.stop_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.stop_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.stop_eventbox), iceplayer.stop_image);

    iceplayer.open_eventbox = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.open_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.open_eventbox), iceplayer.open_image);
///progress
    iceplayer.progress_hbox = gtk_hbox_new (FALSE, 0);

    iceplayer.progress_left_eventbox = gtk_event_box_new ();
    gtk_box_pack_start (GTK_BOX (iceplayer.progress_hbox), iceplayer.progress_left_eventbox, TRUE, TRUE, 0);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.progress_left_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.progress_left_eventbox), iceplayer.progress_left_image);

    iceplayer.progress_node_eventbox = gtk_event_box_new ();
    gtk_box_pack_start (GTK_BOX (iceplayer.progress_hbox), iceplayer.progress_node_eventbox, TRUE, TRUE, 0);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.progress_node_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.progress_node_eventbox), iceplayer.progress_node_image);

    iceplayer.progress_right_eventbox = gtk_event_box_new ();
    gtk_box_pack_start (GTK_BOX (iceplayer.progress_hbox), iceplayer.progress_right_eventbox, TRUE, TRUE, 0);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.progress_right_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.progress_right_eventbox), iceplayer.progress_right_image);

    gtk_widget_set_size_request(iceplayer.progress_left_eventbox,-1,-1);
    gtk_widget_set_size_request(iceplayer.progress_node_eventbox,-1,-1);
///info
    iceplayer.name_label = gtk_label_new ("iceplayer - my music player");
    gtk_misc_set_alignment (GTK_MISC (iceplayer.name_label), 0, 0);
    gtk_label_set_single_line_mode (GTK_LABEL (iceplayer.name_label), TRUE);

    iceplayer.time_label = gtk_label_new ("00:00-00:00");
    gtk_misc_set_alignment (GTK_MISC (iceplayer.time_label), 1, 1);
    gtk_label_set_single_line_mode (GTK_LABEL (iceplayer.name_label), TRUE);

    iceplayer.volume_eventbox = gtk_event_box_new ();
    gtk_fixed_put (GTK_FIXED (iceplayer.main_fixed), iceplayer.volume_eventbox, 280, 75);
    gtk_widget_set_size_request (iceplayer.volume_eventbox, 15, 15);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.volume_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.volume_eventbox), iceplayer.volume_image);
///
    iceplayer.hpaned = gtk_hpaned_new ();
    gtk_container_set_border_width (GTK_CONTAINER (iceplayer.hpaned), 5);
    GTK_WIDGET_UNSET_FLAGS (iceplayer.hpaned, GTK_CAN_FOCUS);
    //gtk_paned_set_position (GTK_PANED (iceplayer.hpaned), 230);
///songs
    iceplayer.widget_lrc_label_hpaned = gtk_label_new("");
    gtk_label_set_justify(GTK_LABEL(iceplayer.widget_lrc_label_hpaned),GTK_JUSTIFY_CENTER);
    gtk_misc_set_alignment (GTK_MISC (iceplayer.widget_lrc_label_hpaned), 0.5, 0.5);
    gtk_widget_set_size_request (iceplayer.widget_lrc_label_hpaned,0, 0);

    iceplayer.back_eventbox_widget_lrc_hpaned = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.back_eventbox_widget_lrc_hpaned), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.back_eventbox_widget_lrc_hpaned), iceplayer.widget_lrc_label_hpaned);

    gtk_paned_pack2 (GTK_PANED (iceplayer.hpaned), iceplayer.back_eventbox_widget_lrc_hpaned, TRUE, TRUE);
    iceplayer.list_hpaned = gtk_hpaned_new ();
    GTK_WIDGET_UNSET_FLAGS (iceplayer.list_hpaned, GTK_CAN_FOCUS);
    gtk_paned_set_position (GTK_PANED (iceplayer.list_hpaned), 80);
    gtk_paned_pack1 (GTK_PANED (iceplayer.hpaned), iceplayer.list_hpaned, FALSE, TRUE);

    iceplayer.list_songs_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    //gtk_widget_show(iceplayer.list_songs_scrolledwindow);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (iceplayer.list_songs_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (iceplayer.list_songs_scrolledwindow), GTK_SHADOW_IN);
    gtk_widget_set_size_request (iceplayer.list_songs_scrolledwindow,0, 0);

    iceplayer.list_songs_treeview = gtk_tree_view_new ();
    //gtk_widget_show(iceplayer.list_songs_treeview);
    gtk_container_add (GTK_CONTAINER (iceplayer.list_songs_scrolledwindow), iceplayer.list_songs_treeview);
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(iceplayer.list_songs_treeview), TRUE);
    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (iceplayer.list_songs_treeview), FALSE);
    gtk_tree_selection_set_mode(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_songs_treeview)),GTK_SELECTION_MULTIPLE);
    gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(iceplayer.list_songs_treeview),FALSE);
    gtk_widget_set_size_request (iceplayer.list_songs_treeview,0, 0);

    iceplayer.list_lists_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (iceplayer.list_lists_scrolledwindow), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (iceplayer.list_lists_scrolledwindow), GTK_SHADOW_IN);
    gtk_widget_set_size_request (iceplayer.list_lists_scrolledwindow,0, 0);

    iceplayer.list_lists_treeview = gtk_tree_view_new ();
    gtk_container_add (GTK_CONTAINER (iceplayer.list_lists_scrolledwindow), iceplayer.list_lists_treeview);
    gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (iceplayer.list_lists_treeview), FALSE);
    gtk_tree_view_set_enable_tree_lines(GTK_TREE_VIEW(iceplayer.list_lists_treeview),FALSE);
    gtk_widget_set_size_request (iceplayer.list_lists_treeview,0, 0);

    gtk_paned_pack2 (GTK_PANED (iceplayer.list_hpaned), iceplayer.list_songs_scrolledwindow, TRUE, TRUE);
    gtk_paned_pack1 (GTK_PANED (iceplayer.list_hpaned), iceplayer.list_lists_scrolledwindow, FALSE, TRUE);

    GtkTargetEntry dest_targets[] = { {(char*) "text/uri-list", 0, 0} };
    GtkTargetList* list = gtk_drag_dest_get_target_list( GTK_WIDGET (iceplayer.list_songs_treeview));
    gtk_target_list_add_uri_targets(list, (guint)dest_targets);
    gtk_drag_dest_set_target_list(GTK_WIDGET(iceplayer.list_songs_treeview),list);

///switch > lrc
    iceplayer.widget_lrc_label = gtk_label_new ("");
    gtk_label_set_justify(GTK_LABEL(iceplayer.widget_lrc_label),GTK_JUSTIFY_CENTER);
    gtk_misc_set_alignment (GTK_MISC (iceplayer.widget_lrc_label), 0.5, 0.5);
    gtk_widget_set_size_request (iceplayer.widget_lrc_label,0, 0);
///switch > configure
    configure_ui_init();
///switch_notebook
    iceplayer.notebook = gtk_notebook_new ();
    gtk_table_attach (GTK_TABLE (iceplayer.table), iceplayer.notebook, 1, 2, 2, 3,(GtkAttachOptions) (GTK_EXPAND|GTK_FILL),(GtkAttachOptions) (GTK_EXPAND|GTK_FILL), 0, 0);
    GTK_WIDGET_UNSET_FLAGS (iceplayer.notebook, GTK_CAN_FOCUS);
    gtk_notebook_set_show_tabs (GTK_NOTEBOOK (iceplayer.notebook), FALSE);
    gtk_notebook_set_show_border (GTK_NOTEBOOK (iceplayer.notebook), FALSE);

    iceplayer.back_eventbox_widget_lrc_integration = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.back_eventbox_widget_lrc_integration), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.back_eventbox_widget_lrc_integration), iceplayer.widget_lrc_label);

    iceplayer.back_eventbox_music_box = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.back_eventbox_music_box), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.back_eventbox_music_box), gtk_label_new("未完成"));

    iceplayer.back_eventbox_configure = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.back_eventbox_configure), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.back_eventbox_configure), configure_widget.sw_vbox);

    gtk_container_add (GTK_CONTAINER (iceplayer.notebook), iceplayer.hpaned);
    gtk_container_add (GTK_CONTAINER (iceplayer.notebook), iceplayer.back_eventbox_widget_lrc_integration);
    gtk_container_add (GTK_CONTAINER (iceplayer.notebook), iceplayer.back_eventbox_music_box);
    gtk_container_add (GTK_CONTAINER (iceplayer.notebook), iceplayer.back_eventbox_configure);

    GtkWidget *label =gtk_label_new("所有窗口:\n\t修改位置：鼠标左键拖动\n\t大小 按住Ctrl+鼠标左键拖动\n\n桌面歌词:\n\t抗锯齿的桌面歌词需混合效果。方法有：\n\t<Compiz(需显卡驱动)>\n\t<Metacity(需Gnome)>\n\t<xcompmgr>  <kwin>\n\t加关键字composite搜索便是。\n\n窗口歌词:\n\t下面的切换栏的\"歌词\"右键切换模式\n\n列表排序，歌词搜索，播放模式在列表右键菜单");

    iceplayer.back_eventbox_help = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.back_eventbox_help), FALSE);
    gtk_container_add (GTK_CONTAINER (iceplayer.back_eventbox_help), label);

    gtk_container_add (GTK_CONTAINER (iceplayer.notebook), iceplayer.back_eventbox_help);
    gtk_widget_set_size_request (label,10, 10);
///switch
    iceplayer.back_eventbox_switch = gtk_event_box_new ();
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.back_eventbox_switch), FALSE);
    gtk_table_attach (GTK_TABLE (iceplayer.table), iceplayer.back_eventbox_switch , 1, 2, 3, 4,(GtkAttachOptions) (GTK_EXPAND|GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);

    iceplayer.switch_hbox = gtk_hbox_new (TRUE, 0);
    gtk_container_add (GTK_CONTAINER (iceplayer.back_eventbox_switch), iceplayer.switch_hbox);

    iceplayer.switch_list_eventbox = gtk_event_box_new ();
    gtk_box_pack_start (GTK_BOX (iceplayer.switch_hbox), iceplayer.switch_list_eventbox, TRUE, FALSE, 0);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.switch_list_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.switch_list_eventbox), iceplayer.switch_list_image);

    iceplayer.switch_lrc_eventbox = gtk_event_box_new ();
    gtk_box_pack_start (GTK_BOX (iceplayer.switch_hbox), iceplayer.switch_lrc_eventbox,  TRUE, FALSE, 0);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.switch_lrc_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.switch_lrc_eventbox), iceplayer.switch_lrc_image);

    iceplayer.switch_music_box_eventbox = gtk_event_box_new ();
    gtk_box_pack_start (GTK_BOX (iceplayer.switch_hbox), iceplayer.switch_music_box_eventbox,  TRUE, FALSE, 0);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.switch_music_box_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.switch_music_box_eventbox), iceplayer.switch_music_box_image);

    iceplayer.switch_configure_eventbox = gtk_event_box_new ();
    gtk_box_pack_start (GTK_BOX (iceplayer.switch_hbox), iceplayer.switch_configure_eventbox, TRUE, FALSE, 0);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.switch_configure_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.switch_configure_eventbox), iceplayer.switch_configure_image);

    iceplayer.switch_help_eventbox = gtk_event_box_new ();
    gtk_box_pack_start (GTK_BOX (iceplayer.switch_hbox), iceplayer.switch_help_eventbox,  TRUE, FALSE, 0);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.switch_help_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.switch_help_eventbox), iceplayer.switch_help_image);
///resize
    iceplayer.resize_buttom_left_eventbox = gtk_event_box_new ();
    gtk_table_attach (GTK_TABLE (iceplayer.table), iceplayer.resize_buttom_left_eventbox, 0, 1, 4, 5,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_widget_set_size_request (iceplayer.resize_buttom_left_eventbox, 2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.resize_buttom_left_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.resize_buttom_left_eventbox), iceplayer.resize_buttom_left_image);

    iceplayer.resize_buttom_right_eventbox = gtk_event_box_new ();
    gtk_table_attach (GTK_TABLE (iceplayer.table), iceplayer.resize_buttom_right_eventbox, 2, 3, 4, 5,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_widget_set_size_request (iceplayer.resize_buttom_right_eventbox, 2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.resize_buttom_right_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.resize_buttom_right_eventbox), iceplayer.resize_buttom_right_image);

    iceplayer.resize_left_eventbox = gtk_event_box_new ();
    gtk_table_attach (GTK_TABLE (iceplayer.table), iceplayer.resize_left_eventbox, 0, 1, 1, 4,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_widget_set_size_request (iceplayer.resize_left_eventbox, 2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.resize_left_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.resize_left_eventbox), iceplayer.resize_left_image);

    iceplayer.resize_top_left_eventbox = gtk_event_box_new ();
    gtk_table_attach (GTK_TABLE (iceplayer.table), iceplayer.resize_top_left_eventbox, 0, 1, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_widget_set_size_request (iceplayer.resize_top_left_eventbox, 2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.resize_top_left_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.resize_top_left_eventbox), iceplayer.resize_top_left_image);

    iceplayer.resize_top_eventbox = gtk_event_box_new ();
    gtk_table_attach (GTK_TABLE (iceplayer.table), iceplayer.resize_top_eventbox, 1, 2, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_widget_set_size_request (iceplayer.resize_top_eventbox, 2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.resize_top_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.resize_top_eventbox), iceplayer.resize_top_image);

    iceplayer.resize_top_right_eventbox = gtk_event_box_new ();
    gtk_table_attach (GTK_TABLE (iceplayer.table), iceplayer.resize_top_right_eventbox, 2, 3, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_widget_set_size_request (iceplayer.resize_top_right_eventbox, 2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.resize_top_right_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.resize_top_right_eventbox), iceplayer.resize_top_right_image);

    iceplayer.resize_buttom_eventbox = gtk_event_box_new ();
    gtk_table_attach (GTK_TABLE (iceplayer.table), iceplayer.resize_buttom_eventbox, 1, 2, 4, 5,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_widget_set_size_request (iceplayer.resize_buttom_eventbox, 2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.resize_buttom_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.resize_buttom_eventbox), iceplayer.resize_buttom_image);
    gtk_widget_set_size_request (iceplayer.resize_buttom_image, -1, 10);

    iceplayer.resize_right_eventbox = gtk_event_box_new ();
    gtk_table_attach (GTK_TABLE (iceplayer.table), iceplayer.resize_right_eventbox, 2, 3, 1, 4,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);
    gtk_widget_set_size_request (iceplayer.resize_right_eventbox,2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.resize_right_eventbox), FALSE);

    gtk_container_add (GTK_CONTAINER (iceplayer.resize_right_eventbox), iceplayer.resize_right_image);

    inline gboolean change_current_page() {
        configure.show_page = gtk_notebook_get_current_page(GTK_NOTEBOOK(iceplayer.notebook));
        return FALSE;
    }
    g_signal_connect ((gpointer) iceplayer.window, "window_state_event",G_CALLBACK(window_state_change),NULL);
    g_signal_connect ((gpointer) iceplayer.window, "delete_event" ,G_CALLBACK (single_quit),NULL);
    g_signal_connect ((gpointer) iceplayer.window, "configure_event"   ,G_CALLBACK (size_changed),NULL);
    g_signal_connect ((gpointer) iceplayer.window, "key_press_event"   ,G_CALLBACK (short_cuts),(gpointer)SHORT_CUTS_ICEPLAYER);

    g_signal_connect ((gpointer) iceplayer.eventbox, "expose-event"   ,G_CALLBACK (on_iceplayer_expose),NULL);

    g_signal_connect ((gpointer) iceplayer.window, "composited-changed"   ,G_CALLBACK (change_com),NULL);

    inline void show_or_hide() {
        if ( windows_state != WINDOWS_STATE_SHOW&&windows_state!=WINDOWS_STATE_SHOW_ALL_WORK_PLACES ) show();
        else hide(NULL);
    }
    g_signal_connect((gpointer) iceplayer.trayIcon ,"activate" ,G_CALLBACK(show_or_hide), NULL);
    g_signal_connect((gpointer) iceplayer.trayIcon ,"popup-menu" ,G_CALLBACK(tray_menu),NULL);

    g_signal_connect ((gpointer) iceplayer.hpaned,          "button_release_event"   ,G_CALLBACK (size_changed),NULL);
    g_signal_connect ((gpointer) iceplayer.list_hpaned,          "button_release_event"   ,G_CALLBACK (size_changed),NULL);
    g_signal_connect ((gpointer) iceplayer.back_eventbox_widget_lrc_integration,      "button_press_event",G_CALLBACK (move),NULL);
    g_signal_connect ((gpointer) iceplayer.back_eventbox_switch,      "button_press_event",G_CALLBACK (move),NULL);
    g_signal_connect ((gpointer) iceplayer.back_eventbox_widget_lrc_hpaned,      "button_press_event",G_CALLBACK (move),NULL);
    g_signal_connect ((gpointer) iceplayer.back_eventbox_main,   "button_press_event",G_CALLBACK (move),NULL);
    g_signal_connect ((gpointer) iceplayer.back_eventbox_configure,   "button_press_event",G_CALLBACK (move),NULL);
    g_signal_connect ((gpointer) iceplayer.back_eventbox_music_box,   "button_press_event",G_CALLBACK (move),NULL);
    g_signal_connect ((gpointer) iceplayer.back_eventbox_help,   "button_press_event",G_CALLBACK (move),NULL);
    g_signal_connect ((gpointer) iceplayer.notebook,      "change_current_page",G_CALLBACK (change_current_page),NULL);
///control
    g_signal_connect ((gpointer) iceplayer.menu_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_MENU);
    g_signal_connect ((gpointer) iceplayer.menu_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_MENU);
    g_signal_connect ((gpointer) iceplayer.mini_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_MINI);
    g_signal_connect ((gpointer) iceplayer.mini_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_MINI);
    g_signal_connect ((gpointer) iceplayer.min_eventbox,    "enter_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_MIN);
    g_signal_connect ((gpointer) iceplayer.min_eventbox,    "leave_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_MIN);
    g_signal_connect ((gpointer) iceplayer.max_eventbox,    "enter_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_MAX);
    g_signal_connect ((gpointer) iceplayer.max_eventbox,    "leave_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_MAX);
    g_signal_connect ((gpointer) iceplayer.close_eventbox,  "enter_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_CLOSE);
    g_signal_connect ((gpointer) iceplayer.close_eventbox,  "leave_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_CLOSE);
    g_signal_connect ((gpointer) iceplayer.menu_eventbox,   "button_press_event",G_CALLBACK (menu),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_eventbox,   "button_press_event",G_CALLBACK (mini),NULL);
    g_signal_connect ((gpointer) iceplayer.above_eventbox,   "button_press_event",G_CALLBACK (above),NULL);
    g_signal_connect ((gpointer) iceplayer.min_eventbox,    "button_press_event",G_CALLBACK (min),NULL);
    g_signal_connect ((gpointer) iceplayer.max_eventbox,    "button_press_event",G_CALLBACK (max),NULL);
    g_signal_connect ((gpointer) iceplayer.close_eventbox,  "button_press_event",G_CALLBACK (quit),NULL);
    g_signal_connect ((gpointer) iceplayer.menu_eventbox,   "button_release_event",G_CALLBACK (menu),NULL);
    g_signal_connect ((gpointer) iceplayer.mini_eventbox,   "button_release_event",G_CALLBACK (mini),NULL);
    g_signal_connect ((gpointer) iceplayer.min_eventbox,    "button_release_event",G_CALLBACK (min),NULL);
    g_signal_connect ((gpointer) iceplayer.max_eventbox,    "button_release_event",G_CALLBACK (max),NULL);
    g_signal_connect ((gpointer) iceplayer.close_eventbox,  "button_release_event",G_CALLBACK (quit),NULL);
///control
    g_signal_connect ((gpointer) iceplayer.prev_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_PREV);
    g_signal_connect ((gpointer) iceplayer.prev_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_PREV);
    g_signal_connect ((gpointer) iceplayer.play_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_PLAY);
    g_signal_connect ((gpointer) iceplayer.play_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_PLAY);
    g_signal_connect ((gpointer) iceplayer.next_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_NEXT);
    g_signal_connect ((gpointer) iceplayer.next_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_NEXT);
    g_signal_connect ((gpointer) iceplayer.stop_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_STOP);
    g_signal_connect ((gpointer) iceplayer.stop_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_STOP);
    g_signal_connect ((gpointer) iceplayer.open_eventbox,   "enter_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_OPEN);
    g_signal_connect ((gpointer) iceplayer.open_eventbox,   "leave_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_OPEN);
    g_signal_connect ((gpointer) iceplayer.prev_eventbox,   "button_press_event",G_CALLBACK (single_prev),NULL);
    g_signal_connect ((gpointer) iceplayer.play_eventbox,   "button_press_event",G_CALLBACK (single_play),NULL);
    g_signal_connect ((gpointer) iceplayer.next_eventbox,   "button_press_event",G_CALLBACK (single_next),NULL);
    g_signal_connect ((gpointer) iceplayer.stop_eventbox,   "button_press_event",G_CALLBACK (single_stop),NULL);
    g_signal_connect ((gpointer) iceplayer.open_eventbox,   "button_press_event",G_CALLBACK (single_add),NULL);
    g_signal_connect ((gpointer) iceplayer.prev_eventbox,   "button_release_event",G_CALLBACK (single_prev),NULL);
    g_signal_connect ((gpointer) iceplayer.play_eventbox,   "button_release_event",G_CALLBACK (single_play),NULL);
    g_signal_connect ((gpointer) iceplayer.next_eventbox,   "button_release_event",G_CALLBACK (single_next),NULL);
    g_signal_connect ((gpointer) iceplayer.stop_eventbox,   "button_release_event",G_CALLBACK (single_stop),NULL);
    g_signal_connect ((gpointer) iceplayer.open_eventbox,   "button_release_event",G_CALLBACK (single_add),(gpointer)ADD_OPEN_BUTTON);
///progress
    g_signal_connect ((gpointer) iceplayer.progress_left_eventbox, "button_press_event",G_CALLBACK (progress),(gpointer)PROGRESS_LEFT);
    g_signal_connect ((gpointer) iceplayer.progress_left_eventbox, "scroll-event", G_CALLBACK(progress_scroll), NULL);
    g_signal_connect ((gpointer) iceplayer.progress_node_eventbox, "button_press_event",G_CALLBACK (progress),(gpointer)PROGRESS_NODE);
    g_signal_connect ((gpointer) iceplayer.progress_node_eventbox, "button_release_event",G_CALLBACK (progress),(gpointer)PROGRESS_NODE);
    g_signal_connect ((gpointer) iceplayer.progress_node_eventbox, "scroll-event", G_CALLBACK(progress_scroll), NULL);
    g_signal_connect ((gpointer) iceplayer.progress_node_eventbox, "enter_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_PROGRESS);
    g_signal_connect ((gpointer) iceplayer.progress_node_eventbox, "leave_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_PROGRESS);
    g_signal_connect ((gpointer) iceplayer.progress_right_eventbox, "button_press_event",G_CALLBACK (progress),(gpointer)PROGRESS_RIGHT);
    g_signal_connect ((gpointer) iceplayer.progress_right_eventbox, "scroll-event", G_CALLBACK(progress_scroll), NULL);

    g_signal_connect ((gpointer) iceplayer.volume_eventbox,     "button_press_event",G_CALLBACK (volume),NULL);
    g_signal_connect ((gpointer) iceplayer.volume_eventbox,     "button_release_event",G_CALLBACK (volume),NULL);
    g_signal_connect ((gpointer) iceplayer.volume_eventbox,     "enter_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_VOLUME);
    g_signal_connect ((gpointer) iceplayer.volume_eventbox,     "leave_notify_event",G_CALLBACK (change_picture),(gpointer)CONTROL_VOLUME);
    g_signal_connect ((gpointer) iceplayer.volume_eventbox,     "scroll-event", G_CALLBACK(volume_scroll), NULL);
///list:
    g_signal_connect ((gpointer) iceplayer.list_lists_treeview,     "cursor_changed",G_CALLBACK (lists_treview_change_cursor),NULL);
    g_signal_connect ((gpointer) iceplayer.list_lists_treeview,     "button_press_event",G_CALLBACK (lists_treeview_click),NULL);
    g_signal_connect ((gpointer) iceplayer.list_lists_treeview,     "drag_data_received",G_CALLBACK (lists_treeview_drag_data_received),NULL);
    g_signal_connect ((gpointer) iceplayer.list_lists_treeview,     "drag_end",G_CALLBACK (lists_treeview_drag_end),NULL);
    g_signal_connect ((gpointer) iceplayer.list_lists_treeview,     "button_release_event",G_CALLBACK (lists_treeview_click),NULL);

    g_signal_connect ((gpointer) iceplayer.list_songs_treeview,     "cursor_changed",G_CALLBACK (songs_treeview_change_cursor),NULL);
    g_signal_connect ((gpointer) iceplayer.list_songs_treeview,     "button_press_event",G_CALLBACK (songs_treeview_click),NULL);
    g_signal_connect ((gpointer) iceplayer.list_songs_treeview,     "button_release_event",G_CALLBACK (songs_treeview_click),NULL);
    g_signal_connect ((gpointer) iceplayer.list_songs_treeview     ,"drag_end"  ,G_CALLBACK(update_songs_order),NULL);
    g_signal_connect ((gpointer) iceplayer.list_songs_treeview     ,"drag_data_received"  ,G_CALLBACK(on_treeview_drag_data_received),NULL);
///switch
    g_signal_connect ((gpointer) iceplayer.switch_list_eventbox,    "enter_notify_event",G_CALLBACK (change_picture),(gpointer)SWITCH_LIST);
    g_signal_connect ((gpointer) iceplayer.switch_list_eventbox,    "leave_notify_event",G_CALLBACK (change_picture),(gpointer)SWITCH_LIST);
    g_signal_connect ((gpointer) iceplayer.switch_lrc_eventbox,     "enter_notify_event",G_CALLBACK (change_picture),(gpointer)SWITCH_LRC);
    g_signal_connect ((gpointer) iceplayer.switch_lrc_eventbox,     "leave_notify_event",G_CALLBACK (change_picture),(gpointer)SWITCH_LRC);
    g_signal_connect ((gpointer) iceplayer.switch_music_box_eventbox, "enter_notify_event",G_CALLBACK (change_picture),(gpointer)SWITCH_MUSICBOX);
    g_signal_connect ((gpointer) iceplayer.switch_music_box_eventbox, "leave_notify_event",G_CALLBACK (change_picture),(gpointer)SWITCH_MUSICBOX);
    g_signal_connect ((gpointer) iceplayer.switch_configure_eventbox,  "enter_notify_event",G_CALLBACK (change_picture),(gpointer)SWITCH_CONFIG);
    g_signal_connect ((gpointer) iceplayer.switch_configure_eventbox,  "leave_notify_event",G_CALLBACK (change_picture),(gpointer)SWITCH_CONFIG);
    g_signal_connect ((gpointer) iceplayer.switch_help_eventbox,    "enter_notify_event",G_CALLBACK (change_picture),(gpointer)SWITCH_HELP);
    g_signal_connect ((gpointer) iceplayer.switch_help_eventbox,    "leave_notify_event",G_CALLBACK (change_picture),(gpointer)SWITCH_HELP);
    g_signal_connect ((gpointer) iceplayer.switch_list_eventbox,    "button_press_event",G_CALLBACK (switch_button),(gpointer)SWITCH_LIST);
    g_signal_connect ((gpointer) iceplayer.switch_lrc_eventbox,     "button_press_event",G_CALLBACK (switch_button),(gpointer)SWITCH_LRC);
    g_signal_connect ((gpointer) iceplayer.switch_music_box_eventbox, "button_press_event",G_CALLBACK (switch_button),(gpointer)SWITCH_MUSICBOX);
    g_signal_connect ((gpointer) iceplayer.switch_configure_eventbox,  "button_press_event",G_CALLBACK (switch_button),(gpointer)SWITCH_CONFIG);
    g_signal_connect ((gpointer) iceplayer.switch_help_eventbox,    "button_press_event",G_CALLBACK (switch_button),(gpointer)SWITCH_HELP);
    g_signal_connect ((gpointer) iceplayer.switch_list_eventbox,    "button_release_event",G_CALLBACK (switch_button),(gpointer)SWITCH_LIST);
    g_signal_connect ((gpointer) iceplayer.switch_lrc_eventbox,     "button_release_event",G_CALLBACK (switch_button),(gpointer)SWITCH_LRC);
    g_signal_connect ((gpointer) iceplayer.switch_music_box_eventbox, "button_release_event",G_CALLBACK (switch_button),(gpointer)SWITCH_MUSICBOX);
    g_signal_connect ((gpointer) iceplayer.switch_configure_eventbox,  "button_release_event",G_CALLBACK (switch_button),(gpointer)SWITCH_CONFIG);
    g_signal_connect ((gpointer) iceplayer.switch_help_eventbox,    "button_release_event",G_CALLBACK (switch_button),(gpointer)SWITCH_HELP);

    g_signal_connect ((gpointer) iceplayer.resize_buttom_left_eventbox,     "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_BUTTOM_LEFT);
    g_signal_connect ((gpointer) iceplayer.resize_buttom_left_eventbox,     "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) iceplayer.resize_buttom_left_eventbox,     "button_press_event",G_CALLBACK (resize),(gpointer)RESIZE_BUTTOM_LEFT);
    g_signal_connect ((gpointer) iceplayer.resize_buttom_right_eventbox,    "button_press_event",G_CALLBACK (resize),(gpointer)RESIZE_BUTTOM_RIGHT);
    g_signal_connect ((gpointer) iceplayer.resize_buttom_right_eventbox,    "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_BUTTOM_RIGHT);
    g_signal_connect ((gpointer) iceplayer.resize_buttom_right_eventbox,    "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) iceplayer.resize_left_eventbox, "button_press_event",G_CALLBACK (resize),(gpointer)RESIZE_LEFT);
    g_signal_connect ((gpointer) iceplayer.resize_left_eventbox, "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_LEFT);
    g_signal_connect ((gpointer) iceplayer.resize_left_eventbox, "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) iceplayer.resize_top_left_eventbox, "button_press_event",G_CALLBACK (resize),(gpointer)RESIZE_TOP_LEFT);
    g_signal_connect ((gpointer) iceplayer.resize_top_left_eventbox, "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_TOP_LEFT);
    g_signal_connect ((gpointer) iceplayer.resize_top_left_eventbox, "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) iceplayer.resize_top_eventbox, "button_press_event",G_CALLBACK (resize),(gpointer)RESIZE_TOP);
    g_signal_connect ((gpointer) iceplayer.resize_top_eventbox, "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_TOP);
    g_signal_connect ((gpointer) iceplayer.resize_top_eventbox, "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) iceplayer.resize_top_right_eventbox, "button_press_event",G_CALLBACK (resize),(gpointer)RESIZE_TOP_RIGHT);
    g_signal_connect ((gpointer) iceplayer.resize_top_right_eventbox, "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_TOP_RIGHT);
    g_signal_connect ((gpointer) iceplayer.resize_top_right_eventbox, "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) iceplayer.resize_buttom_eventbox,  "button_press_event",G_CALLBACK (resize),(gpointer)RESIZE_BUTTOM);
    g_signal_connect ((gpointer) iceplayer.resize_buttom_eventbox,  "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_BUTTOM);
    g_signal_connect ((gpointer) iceplayer.resize_buttom_eventbox,  "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) iceplayer.resize_right_eventbox,   "button_press_event",G_CALLBACK (resize),(gpointer)RESIZE_RIGHT);
    g_signal_connect ((gpointer) iceplayer.resize_right_eventbox,   "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_RIGHT);
    g_signal_connect ((gpointer) iceplayer.resize_right_eventbox,   "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);

    set_skin_configure_to_iceplayer_widget();

    GdkScreen *screen;
    GdkColormap *colormap;

    screen = gtk_widget_get_screen(iceplayer.window);       // 重要
    colormap = gdk_screen_get_rgba_colormap(screen);
    gtk_widget_set_colormap(iceplayer.window, colormap);

    //g_timeout_add(10,update_iceplayer_bg,NULL);
}
///skin:
gchar *get_skin_dir() {
    print_programming("get_skin_dir\n");
    static gchar dir[STRINGS_LENGTH];
    strcpy(dir,"");
    if (skin.use_default_skin) {
        strcpy(dir,"@datadir@/iceplayer/skin/blue_life_slax/");
    } else {
        if (use_usr_skin) {
            strcpy(dir,"@datadir@/iceplayer/skin/");
            strcat(dir,configure.skin);
        } else {
            strcpy(dir,g_get_home_dir());
            strcat(dir,"/.iceplayer/skin/");
            strcat(dir,configure.skin);
        }
        strcat(dir,"/");
    }
    return dir;
}
gboolean read_skin_configure() {
    print_programming(("read_skin_configure\n"));
    GKeyFile *keyfile;
    GError *error=NULL;
    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile,strcat(get_skin_dir(),"皮肤设置.conf"),0,&error)) {
        skin.use_default_skin=TRUE;
        print_error("无法加载皮肤配置文件%s，请检查！错误代码:%s，将使用默认皮肤\n",strcat(get_skin_dir(),"皮肤设置.conf"),error->message);
        if (!g_key_file_load_from_file(keyfile,strcat(get_skin_dir(),"皮肤设置.conf"),0,&error)) {
            print_error("无法加载皮肤配置文件%s，请重新安装程序！错误代码:%s\n",strcat(get_skin_dir(),"皮肤设置.conf"),error->message);
            g_key_file_free(keyfile);
            return FALSE;
        } else {
            strcpy(configure.skin,"blue_life_slax");
        }
    }
///
    strcpy(skin.title , g_key_file_get_string(keyfile, "信息","名称", &error));
    SKIN_CONFIG_READ_FAILED
    strcpy(skin.editor , g_key_file_get_string(keyfile, "信息","作者", &error));
    SKIN_CONFIG_READ_FAILED
    strcpy(skin.version , g_key_file_get_string(keyfile, "信息","版本", &error));
    SKIN_CONFIG_READ_FAILED
    strcpy(skin.time , g_key_file_get_string(keyfile, "信息","时间", &error));
    SKIN_CONFIG_READ_FAILED
    strcpy(skin.link , g_key_file_get_string(keyfile, "信息","链接", &error));
    SKIN_CONFIG_READ_FAILED
    strcpy(skin.about , g_key_file_get_string(keyfile, "信息","说明", &error));
    SKIN_CONFIG_READ_FAILED

    skin.main_height=g_key_file_get_integer(keyfile, "主窗体", "main_height", &error);
    SKIN_CONFIG_READ_FAILED
    skin.main_width=g_key_file_get_integer(keyfile, "主窗体", "main_width", &error);
    SKIN_CONFIG_READ_FAILED

    skin.control_play_x=g_key_file_get_integer(keyfile, "控制栏", "control_play_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_play_y=g_key_file_get_integer(keyfile, "控制栏", "control_play_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_play_width=g_key_file_get_integer(keyfile, "控制栏", "control_play_width", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_play_height=g_key_file_get_integer(keyfile, "控制栏", "control_play_height", &error);
    SKIN_CONFIG_READ_FAILED

    skin.control_prev_x=g_key_file_get_integer(keyfile, "控制栏", "control_prev_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_prev_y=g_key_file_get_integer(keyfile, "控制栏", "control_prev_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_prev_width=g_key_file_get_integer(keyfile, "控制栏", "control_prev_width", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_prev_height=g_key_file_get_integer(keyfile, "控制栏", "control_prev_height", &error);
    SKIN_CONFIG_READ_FAILED

    skin.control_next_x=g_key_file_get_integer(keyfile, "控制栏", "control_next_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_next_y=g_key_file_get_integer(keyfile, "控制栏", "control_next_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_next_width=g_key_file_get_integer(keyfile, "控制栏", "control_next_width", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_next_height=g_key_file_get_integer(keyfile, "控制栏", "control_next_height", &error);
    SKIN_CONFIG_READ_FAILED

    skin.control_stop_x=g_key_file_get_integer(keyfile, "控制栏", "control_stop_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_stop_y=g_key_file_get_integer(keyfile, "控制栏", "control_stop_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_stop_width=g_key_file_get_integer(keyfile, "控制栏", "control_stop_width", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_stop_height=g_key_file_get_integer(keyfile, "控制栏", "control_stop_height", &error);
    SKIN_CONFIG_READ_FAILED

    skin.control_open_x=g_key_file_get_integer(keyfile, "控制栏", "control_open_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_open_y=g_key_file_get_integer(keyfile, "控制栏", "control_open_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_open_width=g_key_file_get_integer(keyfile, "控制栏", "control_open_width", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_open_height=g_key_file_get_integer(keyfile, "控制栏", "control_open_height", &error);
    SKIN_CONFIG_READ_FAILED


    skin.control_close_x = g_key_file_get_integer(keyfile, "窗体控制栏", "control_close_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_close_y = g_key_file_get_integer(keyfile, "窗体控制栏", "control_close_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_close_width = g_key_file_get_integer(keyfile, "窗体控制栏", "control_close_width", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_close_height = g_key_file_get_integer(keyfile, "窗体控制栏", "control_close_height", &error);
    SKIN_CONFIG_READ_FAILED

    skin.control_max_x=g_key_file_get_integer(keyfile, "窗体控制栏", "control_max_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_max_y=g_key_file_get_integer(keyfile, "窗体控制栏", "control_max_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_max_width=g_key_file_get_integer(keyfile, "窗体控制栏", "control_max_width", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_max_height=g_key_file_get_integer(keyfile, "窗体控制栏", "control_max_height", &error);
    SKIN_CONFIG_READ_FAILED

    skin.control_min_x=g_key_file_get_integer(keyfile, "窗体控制栏", "control_min_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_min_y=g_key_file_get_integer(keyfile, "窗体控制栏", "control_min_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_min_width=g_key_file_get_integer(keyfile, "窗体控制栏", "control_min_width", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_min_height=g_key_file_get_integer(keyfile, "窗体控制栏", "control_min_height", &error);
    SKIN_CONFIG_READ_FAILED

    skin.control_mini_x=g_key_file_get_integer(keyfile, "窗体控制栏", "control_mini_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_mini_y=g_key_file_get_integer(keyfile, "窗体控制栏", "control_mini_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_mini_width=g_key_file_get_integer(keyfile, "窗体控制栏", "control_mini_width", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_mini_height=g_key_file_get_integer(keyfile, "窗体控制栏", "control_mini_height", &error);
    SKIN_CONFIG_READ_FAILED

    skin.control_above_x=g_key_file_get_integer(keyfile, "窗体控制栏", "control_above_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_above_y=g_key_file_get_integer(keyfile, "窗体控制栏", "control_above_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_above_width=g_key_file_get_integer(keyfile, "窗体控制栏", "control_above_width", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_above_height=g_key_file_get_integer(keyfile, "窗体控制栏", "control_above_height", &error);
    SKIN_CONFIG_READ_FAILED

    skin.control_menu_x=g_key_file_get_integer(keyfile, "窗体控制栏", "control_menu_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_menu_y=g_key_file_get_integer(keyfile, "窗体控制栏", "control_menu_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_menu_width=g_key_file_get_integer(keyfile, "窗体控制栏", "control_menu_width", &error);
    SKIN_CONFIG_READ_FAILED
    skin.control_menu_height=g_key_file_get_integer(keyfile, "窗体控制栏", "control_menu_height", &error);
    SKIN_CONFIG_READ_FAILED


    skin.progress_left_top_x=g_key_file_get_integer(keyfile, "进度栏", "progress_left_top_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.progress_left_top_y=g_key_file_get_integer(keyfile, "进度栏", "progress_left_top_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.progress_right_buttom_x=g_key_file_get_integer(keyfile, "进度栏", "progress_right_buttom_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.progress_right_buttom_y=g_key_file_get_integer(keyfile, "进度栏", "progress_right_buttom_y", &error);
    SKIN_CONFIG_READ_FAILED

    skin.volume_x=g_key_file_get_integer(keyfile, "音量", "volume_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.volume_y=g_key_file_get_integer(keyfile, "音量", "volume_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.volume_width=g_key_file_get_integer(keyfile, "音量", "volume_width", &error);
    SKIN_CONFIG_READ_FAILED
    skin.volume_height=g_key_file_get_integer(keyfile, "音量", "volume_height", &error);
    SKIN_CONFIG_READ_FAILED
///
    skin.mini_w=g_key_file_get_integer(keyfile, "迷你", "mini_w", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_h=g_key_file_get_integer(keyfile, "迷你", "mini_h", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_close_w=g_key_file_get_integer(keyfile, "迷你", "mini_close_w", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_close_h=g_key_file_get_integer(keyfile, "迷你", "mini_close_h", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_close_x=g_key_file_get_integer(keyfile, "迷你", "mini_close_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_close_y=g_key_file_get_integer(keyfile, "迷你", "mini_close_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_mini_w=g_key_file_get_integer(keyfile, "迷你", "mini_mini_w", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_mini_h=g_key_file_get_integer(keyfile, "迷你", "mini_mini_h", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_mini_x=g_key_file_get_integer(keyfile, "迷你", "mini_mini_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_mini_y=g_key_file_get_integer(keyfile, "迷你", "mini_mini_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_label_x=g_key_file_get_integer(keyfile, "迷你", "mini_label_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_label_y=g_key_file_get_integer(keyfile, "迷你", "mini_label_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_label_w=g_key_file_get_integer(keyfile, "迷你", "mini_label_w", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_label_h=g_key_file_get_integer(keyfile, "迷你", "mini_label_h", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_play_x=g_key_file_get_integer(keyfile, "迷你", "mini_play_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_play_y=g_key_file_get_integer(keyfile, "迷你", "mini_play_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_play_w=g_key_file_get_integer(keyfile, "迷你", "mini_play_w", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_play_h=g_key_file_get_integer(keyfile, "迷你", "mini_play_h", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_next_x=g_key_file_get_integer(keyfile, "迷你", "mini_next_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_next_y=g_key_file_get_integer(keyfile, "迷你", "mini_next_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_next_w=g_key_file_get_integer(keyfile, "迷你", "mini_next_w", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_next_h=g_key_file_get_integer(keyfile, "迷你", "mini_next_h", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_prev_x=g_key_file_get_integer(keyfile, "迷你", "mini_prev_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_prev_y=g_key_file_get_integer(keyfile, "迷你", "mini_prev_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_prev_w=g_key_file_get_integer(keyfile, "迷你", "mini_prev_w", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_prev_h=g_key_file_get_integer(keyfile, "迷你", "mini_prev_h", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_stop_x=g_key_file_get_integer(keyfile, "迷你", "mini_stop_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_stop_y=g_key_file_get_integer(keyfile, "迷你", "mini_stop_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_stop_w=g_key_file_get_integer(keyfile, "迷你", "mini_stop_w", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_stop_h=g_key_file_get_integer(keyfile, "迷你", "mini_stop_h", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_lists_x=g_key_file_get_integer(keyfile, "迷你", "mini_lists_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_lists_y=g_key_file_get_integer(keyfile, "迷你", "mini_lists_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_lists_w=g_key_file_get_integer(keyfile, "迷你", "mini_lists_w", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_lists_h=g_key_file_get_integer(keyfile, "迷你", "mini_lists_h", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_songs_x=g_key_file_get_integer(keyfile, "迷你", "mini_songs_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_songs_y=g_key_file_get_integer(keyfile, "迷你", "mini_songs_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_songs_w=g_key_file_get_integer(keyfile, "迷你", "mini_songs_w", &error);
    SKIN_CONFIG_READ_FAILED
    skin.mini_songs_h=g_key_file_get_integer(keyfile, "迷你", "mini_songs_h", &error);
    SKIN_CONFIG_READ_FAILED

    skin.info_name_x=g_key_file_get_integer(keyfile, "info", "info_name_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.info_name_y=g_key_file_get_integer(keyfile, "info", "info_name_y", &error);
    SKIN_CONFIG_READ_FAILED
    skin.info_time_x=g_key_file_get_integer(keyfile, "info", "info_time_x", &error);
    SKIN_CONFIG_READ_FAILED
    skin.info_time_y=g_key_file_get_integer(keyfile, "info", "info_time_y", &error);
    SKIN_CONFIG_READ_FAILED

    g_key_file_free(keyfile);
    return TRUE;

}
void set_skin_configure_to_iceplayer_widget() {
    print_programming(("set_skin_configure_to_iceplayer_widget\n"));
    gtk_widget_set_size_request (iceplayer.window, skin.main_width, skin.main_height+4);
    gtk_widget_set_size_request (iceplayer.back_eventbox_main,skin.main_width, skin.main_height);
    gtk_widget_set_size_request (iceplayer.main_fixed,skin.main_width, skin.main_height);
    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.menu_eventbox, skin.control_menu_x, skin.control_menu_y);
    gtk_widget_set_size_request (iceplayer.menu_eventbox, skin.control_menu_width, skin.control_menu_height);
    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.above_eventbox, skin.control_above_x, skin.control_above_y);
    gtk_widget_set_size_request (iceplayer.above_eventbox,skin.control_above_width , skin.control_above_height);
    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.min_eventbox, skin.control_min_x, skin.control_min_y);
    gtk_widget_set_size_request (iceplayer.min_eventbox, skin.control_min_width, skin.control_min_height);
    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.max_eventbox, skin.control_max_x,  skin.control_max_y);
    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.mini_eventbox, skin.control_mini_x, skin.control_mini_y);
    gtk_widget_set_size_request (iceplayer.mini_eventbox, skin.control_mini_width, skin.control_mini_height);
    gtk_widget_set_size_request (iceplayer.max_eventbox,  skin.control_max_width,  skin.control_max_height);
    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.close_eventbox,  skin.control_close_x, skin.control_close_y);
    gtk_widget_set_size_request (iceplayer.close_eventbox,skin.control_close_width,skin.control_close_height);
    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.prev_eventbox, skin.control_prev_x, skin.control_prev_y);
    gtk_widget_set_size_request (iceplayer.prev_eventbox, skin.control_prev_width, skin.control_prev_height);

    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.play_eventbox, skin.control_play_x,skin.control_play_y);
    gtk_widget_set_size_request (iceplayer.play_eventbox, skin.control_play_width,skin.control_play_height);
    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.next_eventbox, skin.control_next_x, skin.control_next_y);
    gtk_widget_set_size_request (iceplayer.next_eventbox, skin.control_next_width, skin.control_next_height);
    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.stop_eventbox, skin.control_stop_x, skin.control_stop_y);
    gtk_widget_set_size_request (iceplayer.stop_eventbox, skin.control_stop_width, skin.control_stop_height);
    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.open_eventbox, skin.control_open_x, skin.control_open_y);
    gtk_widget_set_size_request (iceplayer.open_eventbox, skin.control_open_width, skin.control_open_height);
    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.progress_hbox, skin.progress_left_top_x, skin.progress_left_top_y);
    gtk_fixed_move_new (iceplayer.main_fixed, iceplayer.volume_eventbox, skin.volume_x, skin.volume_y);
    gtk_widget_set_size_request (iceplayer.volume_eventbox, skin.volume_width, skin.volume_height);
    gint info_name_x=0,info_time_x=0,info_name_y=0,info_time_y=0;

    sscanf(gtk_entry_get_text(GTK_ENTRY(configure_widget.info_name_entry_x)),"%d",&info_name_x);
    sscanf(gtk_entry_get_text(GTK_ENTRY(configure_widget.info_name_entry_y)),"%d",&info_name_y);

    sscanf(gtk_entry_get_text(GTK_ENTRY(configure_widget.info_time_entry_x)),"%d",&info_time_x);
    sscanf(gtk_entry_get_text(GTK_ENTRY(configure_widget.info_time_entry_y)),"%d",&info_time_y);

    if(!(info_name_x==0&&info_name_y==0)) {
        gtk_fixed_move_new(iceplayer.main_fixed,iceplayer.name_label,info_name_x,info_name_y);
    } else {
        gtk_fixed_move_new(iceplayer.main_fixed,iceplayer.name_label,skin.info_name_x,skin.info_name_y);
    }

    if(!(info_time_x==0&&info_time_y==0)) {
        gtk_fixed_move_new(iceplayer.main_fixed,iceplayer.time_label,info_time_x,info_time_y);
    } else {
        gtk_fixed_move_new(iceplayer.main_fixed,iceplayer.time_label,skin.info_time_x,skin.info_time_y);
    }
}
static void set_skin_image( ) {
    print_programming("set_skin_image\n");
    gchar dir[STRINGS_LENGTH];
    strcpy(dir,get_skin_dir());

    iceplayer.menu_image = gtk_image_new_from_file_new(dir,"menu");
    iceplayer.mini_image = gtk_image_new_from_file_new(dir,"mini");
    iceplayer.above_image = gtk_image_new_from_file_new(dir,"above");
    iceplayer.min_image = gtk_image_new_from_file_new(dir,"min");
    iceplayer.max_image = gtk_image_new_from_file_new(dir,"max");
    iceplayer.close_image = gtk_image_new_from_file_new(dir,"close");
    iceplayer.prev_image = gtk_image_new_from_file_new(dir,"prev");
    iceplayer.play_image = gtk_image_new_from_file_new(dir,"pause");
    iceplayer.next_image = gtk_image_new_from_file_new(dir,"next");
    iceplayer.stop_image = gtk_image_new_from_file_new(dir,"stop");
    iceplayer.open_image = gtk_image_new_from_file_new(dir,"open");

    iceplayer.mini_mini_image = gtk_image_new_from_file_new(dir,"mini.mini");
    iceplayer.mini_close_image = gtk_image_new_from_file_new(dir,"mini.close");
    iceplayer.mini_prev_image = gtk_image_new_from_file_new(dir,"mini.prev");
    iceplayer.mini_play_image = gtk_image_new_from_file_new(dir,"mini.pause");
    iceplayer.mini_next_image = gtk_image_new_from_file_new(dir,"mini.next");
    iceplayer.mini_stop_image = gtk_image_new_from_file_new(dir,"mini.stop");
    iceplayer.mini_songs_image = gtk_image_new_from_file_new(dir,"mini.songs");
    iceplayer.mini_lists_image = gtk_image_new_from_file_new(dir,"mini.lists");

    iceplayer.progress_left_image = gtk_image_new_from_file_new(dir,"progress.left");
    iceplayer.progress_node_image = gtk_image_new_from_file_new(dir,"progress.node");
    iceplayer.progress_right_image = gtk_image_new_from_file_new(dir,"progress.right");
    iceplayer.volume_image = gtk_image_new_from_file_new(dir,"volume");
    iceplayer.switch_list_image = gtk_image_new_from_file_new(dir,"button.list");
    iceplayer.switch_lrc_image = gtk_image_new_from_file_new(dir,"button.lrc");
    iceplayer.switch_music_box_image = gtk_image_new_from_file_new(dir,"button.musicbox");
    iceplayer.switch_configure_image = gtk_image_new_from_file_new(dir,"button.config");
    iceplayer.switch_help_image = gtk_image_new_from_file_new(dir,"button.help");
    iceplayer.resize_left_image = gtk_image_new_from_file_new(dir,"left");
    iceplayer.resize_top_image = gtk_image_new_from_file_new(dir,"top");
    iceplayer.resize_buttom_image = gtk_image_new_from_file_new(dir,"buttom");
    iceplayer.resize_right_image = gtk_image_new_from_file_new(dir,"right");

    iceplayer.resize_buttom_left_image = gtk_image_new_from_file_new(dir,"buttom_left");
    iceplayer.resize_buttom_right_image = gtk_image_new_from_file_new(dir,"buttom_right");
    iceplayer.resize_top_right_image = gtk_image_new_from_file_new(dir,"top_right");
    iceplayer.resize_top_left_image = gtk_image_new_from_file_new(dir,"top_left");

    widget_lrc.above_image = gtk_image_new_from_file_new(dir,"above");
    widget_lrc.min_image = gtk_image_new_from_file_new(dir,"min");
    widget_lrc.max_image = gtk_image_new_from_file_new(dir,"max");
    widget_lrc.close_image= gtk_image_new_from_file_new(dir,"close");

    //widget_lrc.back_image = gtk_image_new_from_file_new(dir,"main");
    widget_lrc.left_image = gtk_image_new_from_file_new(dir,"left");
    widget_lrc.right_image = gtk_image_new_from_file_new(dir,"right");
    widget_lrc.top_image = gtk_image_new_from_file_new(dir,"top");
    widget_lrc.buttom_image = gtk_image_new_from_file_new(dir,"buttom");
    widget_lrc.top_left_image= gtk_image_new_from_file_new(dir,"top_left");
    widget_lrc.top_right_image= gtk_image_new_from_file_new(dir,"top_right");
    widget_lrc.buttom_left_image= gtk_image_new_from_file_new(dir,"buttom_left");
    widget_lrc.buttom_right_image= gtk_image_new_from_file_new(dir,"buttom_right");

    if (!read_skin_configure()) {
        dialog(_("iceplayer\n读取皮肤参数失败！查看详细原因请从终端中打开程序。"));
        exit(0);
    }
}
void ui_init_skin() {
    print_programming("ui_init_skin\n");
    print_debug("搜索home目录皮肤");

    //FILE *skin_configure_file = open_skin_configure();
    //if (read_configure_file(skin_configure_file)) {
    skin.use_default_skin=FALSE;
    use_usr_skin = FALSE;
    gchar path[STRINGS_LENGTH];
    strcpy(path,get_skin_dir());
    print_debug(path);
    strcat(path,"play.png");
    FILE *file = fopen(path,"r");
    if (file) {
        fclose(file);
        print_debug("初始化home目录皮肤成功");
        skin.use_default_skin=FALSE;
        use_usr_skin = FALSE;
    } else {
        print_debug("初始化home目录皮肤失败");
        print_debug("搜索usr目录皮肤");
        use_usr_skin=TRUE;
        strcpy(path,get_skin_dir());
        print_debug(path);
        strcat(path,"play.png");
        file = fopen(path,"r");
        if (file) {
            fclose(file);
            skin.use_default_skin=FALSE;
            use_usr_skin=TRUE;
            print_debug("初始化usr目录皮肤成功");
        } else {
            print_error("无法加载皮肤配置文件%s，请检查！将使用默认皮肤\n",strcat(get_skin_dir(),"皮肤设置.conf"));
            strcpy(configure.skin,"blue_life_slax");
            skin.use_default_skin=TRUE;
        }
    }
    set_skin_image();

}
///progress:
gboolean progress_refresh() {
    ///  "<cue><FILE:%s><TRACK:%s><TITLE:%s><PERFORMER:%s><INDEX_A:%s><INDEX_B:%s><cue_uri:%s>",
    print_programming("progress_refresh\n");
    if (core.state==GST_STATE_PLAYING||core.state ==GST_STATE_PAUSED ) {
        gint width = 0,height = 0;
        gst_element_query_position(core.pipeline, &core.fmt, &core.pos);


        static gint timer = 5;
        if (timer==5) {
            timer=0;
            gst_element_query_duration(core.pipeline, &core.fmt, &core.length);
        } else {
            timer++;
        }
        if (core.pos==core.length||core.length==0) return TRUE;

        height = configure.width-40-iceplayer.progress_node_image->allocation.width;
        if (!strncmp(focus_song,"<cue>",5)) {
            //cue
            gint min,sec,mir;
            min = (core.pos )/60000000000;
            sec = ((core.pos/10000000) % 6000) / 100;
            mir = (core.pos/10000000) % 100;
            gint min_pre,sec_pre,mir_pre,min_end,sec_end,mir_end;
            static gchar p[STRINGS_LENGTH];
            if (!strstr(focus_song,"<INDEX_B:NONE>")) {
                strcpy(p,strstr(focus_song,"<INDEX_A"));
                sscanf(p,"<INDEX_A:%d:%d:%d><INDEX_B:%d:%d:%d>%*s",&min_pre,&sec_pre,&mir_pre,&min_end,&sec_end,&mir_end);
            } else {
                strcpy(p,strstr(focus_song,"<INDEX_A:"));
                p[18]=0;
                gchar a[30];
                a[0]=p[9];
                a[1]=p[10];
                a[2]=0;
                min_pre=atoi(a);
                a[0]=p[12];
                a[1]=p[13];
                a[2]=0;
                sec_pre=atoi(a);
                a[0]=p[15];
                a[1]=p[16];
                a[2]=0;
                mir_pre=atoi(a);
                min_end = (core.length )/60000000000;
                sec_end = ((core.length/10000000) % 6000) / 100;
                mir_end = (core.length/10000000) % 100;
            }

            if ((min_end==min&&sec_end<sec&&sec_end>sec-3)||(min_end==min&&sec_end==sec&&mir_end<mir)) {
                if (core.pipeline) gst_element_set_state(GST_ELEMENT(core.pipeline), GST_STATE_NULL);
                core.state = GST_STATE_NULL;
                switch_songs();
            } else {
                width = height*((gdouble)((min-min_pre)*6000+(sec-sec_pre)*100+(mir-mir_pre) )/(gdouble)((min_end-min_pre)*6000+(sec_end-sec_pre)*100+(mir_end-mir_pre)));
                if (width<0)
                    width = 0;
            }
            //FIXME: cue的显示时间暂时只是整个歌曲的时间
            static gchar A[60];
            static gchar B[25];
            g_snprintf(A,24,"%02u %02u:%02u%d",GST_TIME_ARGS(core.pos));
            g_snprintf(B,24,"%02u %02u:%02u%d",GST_TIME_ARGS(core.length));

            A[8] = 0;
            strcpy(A,strstr(A," "));
            B[8] = 0;
            strcpy(B,strstr(B," "));
            strcat(A,"-");
            strcat(A,B);

            gtk_label_set_text(GTK_LABEL(iceplayer.time_label),A);
        } else {
            width = height*((gdouble) core.pos/(gdouble)core.length);
            static gchar A[60];
            static gchar B[25];
            g_snprintf(A,24,"%02u %02u:%02u%d",GST_TIME_ARGS(core.pos));
            g_snprintf(B,24,"%02u %02u:%02u%d",GST_TIME_ARGS(core.length));

            A[8] = 0;
            strcpy(A,strstr(A," "));
            B[8] = 0;
            strcpy(B,strstr(B," "));
            strcat(A,"-");
            strcat(A,B);

            gtk_label_set_text(GTK_LABEL(iceplayer.time_label),A);
        }
        height =height- width;
        gtk_widget_set_size_request(iceplayer.progress_left_eventbox,width,iceplayer.progress_hbox->allocation.height);
        gtk_widget_set_size_request(iceplayer.progress_right_eventbox,height,iceplayer.progress_hbox->allocation.height);

    } else {
        gtk_widget_set_size_request(iceplayer.progress_left_eventbox,1,iceplayer.progress_hbox->allocation.height);
        gtk_widget_set_size_request(iceplayer.progress_right_eventbox,iceplayer.progress_hbox->allocation.width-iceplayer.progress_node_image->allocation.width,iceplayer.progress_hbox->allocation.height);
    }
    return TRUE;
}
void progress_refresh_init() {
    print_programming("progress_refresh_init\n");
    static gchar dir[STRINGS_LENGTH];
    strcpy(dir,get_skin_dir());

    gint width, height;
    width = gdk_screen_get_width(gdk_screen_get_default());

    height = iceplayer.progress_hbox->allocation.height;
    gtk_container_remove_new (GTK_CONTAINER (iceplayer.progress_left_eventbox), iceplayer.progress_left_image);
    iceplayer.progress_left_image=gtk_image_new_from_pixbuf(gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file_new(dir,"progress.left"),width,height,GDK_INTERP_HYPER));
    gtk_widget_show (iceplayer.progress_left_image);
    gtk_container_add (GTK_CONTAINER (iceplayer.progress_left_eventbox), iceplayer.progress_left_image);

    gtk_container_remove_new (GTK_CONTAINER (iceplayer.progress_right_eventbox), iceplayer.progress_right_image);
    iceplayer.progress_right_image=gtk_image_new_from_pixbuf(gdk_pixbuf_scale_simple(gdk_pixbuf_new_from_file_new(dir,"progress.right"),width,height,GDK_INTERP_HYPER));
    gtk_widget_show (iceplayer.progress_right_image);
    gtk_container_add (GTK_CONTAINER (iceplayer.progress_right_eventbox), iceplayer.progress_right_image);

    inline gboolean progress_init_refresh() {
        static gint a=0;
        a++;
        if (a>=50) {
            a=0;
            return FALSE;
        }
        progress_refresh();
        gtk_widget_set_size_request(iceplayer.progress_left_eventbox,0,-1);
        gtk_widget_set_size_request(iceplayer.progress_right_eventbox,iceplayer.progress_hbox->allocation.width-iceplayer.progress_node_image->allocation.width,-1);
        return TRUE;
    }

    g_timeout_add(5,progress_init_refresh,NULL);

    core.fmt = GST_FORMAT_TIME;
}
void tray_menu() {
    print_programming("tray_menu\n");
    GtkWidget *tray_menu = gtk_menu_new ();
    GtkWidget *tray_show_or_hide;
    GtkWidget *tray_play_or_pause;
    GtkWidget *tray_prev;
    GtkWidget *tray_next;
    GtkWidget *tray_stop;
    GtkWidget *tray_open;
    GtkWidget *tray_mini;
    GtkWidget *tray_configure;
    GtkWidget *tray_about;
    GtkWidget *tray_quit;
    GtkWidget *image ;
    GtkWidget *lrc_item;
    GtkWidget *lrc_item_menu;


    if (windows_state!=WINDOWS_STATE_SHOW&&windows_state!=WINDOWS_STATE_SHOW_ALL_WORK_PLACES)tray_show_or_hide = gtk_image_menu_item_new_with_mnemonic ("显示");
    else
        tray_show_or_hide = gtk_image_menu_item_new_with_mnemonic ("隐藏");
    gtk_widget_show (tray_show_or_hide);
    gtk_menu_append (GTK_MENU_SHELL (tray_menu), tray_show_or_hide);

    image = gtk_image_new_from_stock (GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tray_show_or_hide), image);

    if (core.state==GST_STATE_PLAYING)tray_play_or_pause = gtk_image_menu_item_new_with_mnemonic ("暂停");
    else tray_play_or_pause = gtk_image_menu_item_new_with_mnemonic ("播放");
    gtk_widget_show (tray_play_or_pause);
    gtk_menu_append (GTK_MENU_SHELL (tray_menu), tray_play_or_pause);

    image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PLAY, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tray_play_or_pause), image);

    tray_next = gtk_image_menu_item_new_with_mnemonic ("下一首");
    gtk_widget_show (tray_next);
    gtk_menu_append (GTK_MENU_SHELL (tray_menu), tray_next);

    image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_NEXT, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tray_next), image);

    tray_prev = gtk_image_menu_item_new_with_mnemonic ("上一首");
    gtk_widget_show (tray_prev);
    gtk_menu_append (GTK_MENU_SHELL (tray_menu), tray_prev);

    image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_PREVIOUS, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tray_prev), image);

    tray_stop = gtk_image_menu_item_new_with_mnemonic ("停止");
    gtk_widget_show (tray_stop);
    gtk_menu_append (GTK_MENU_SHELL (tray_menu), tray_stop);

    image = gtk_image_new_from_stock (GTK_STOCK_MEDIA_STOP, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tray_stop), image);

    tray_open = gtk_image_menu_item_new_with_mnemonic ("打开");
    gtk_widget_show (tray_open);
    gtk_menu_append (GTK_MENU_SHELL (tray_menu), tray_open);

    image = gtk_image_new_from_stock (GTK_STOCK_OPEN, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tray_open), image);

    ///

    inline void lock() {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.desktop_lrc_lock_checkbutton),!configure.desktop_lrc_lock);
        update_configure();
    }
    inline void close_desk_lrc() {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_desktop_lrc_checkbutton),!configure.desktop_lrc);
        update_configure();
    }
    inline void close_lrc() {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_lrc_checkbutton),!configure.lrc);
        update_configure();
        lrc();
    }

    inline void research_lrc() {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_search_lrc_checkbutton),!configure.search_lrc);
        update_configure();
    }
    ///

    lrc_item = gtk_image_menu_item_new_with_label (_("歌词"));
    gtk_widget_show (lrc_item);
    gtk_container_add (GTK_CONTAINER (tray_menu), lrc_item);

    image = gtk_image_new_from_stock ("gtk-select-all", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lrc_item), image);

    lrc_item_menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (lrc_item), lrc_item_menu);

    GtkWidget *menu_lrc;
    if (configure.lrc)menu_lrc = gtk_image_menu_item_new_with_mnemonic(_("关闭歌词"));
    else menu_lrc = gtk_image_menu_item_new_with_mnemonic(_("使用歌词"));
    gtk_menu_shell_append(GTK_MENU_SHELL(lrc_item_menu),menu_lrc);

    if (!configure.lrc)image = gtk_image_new_from_stock (GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
    else image = gtk_image_new_from_stock (GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_lrc), image);

    g_signal_connect ((gpointer) menu_lrc, "activate",G_CALLBACK (close_lrc),NULL);

    ///
    if (configure.lrc) {
        GtkWidget *menu_research_lrc = NULL;
        GtkWidget *menu_research = NULL;

        if (configure.search_lrc)menu_research_lrc = gtk_image_menu_item_new_with_mnemonic(_("关闭搜索歌词功能"));
        else menu_research_lrc = gtk_image_menu_item_new_with_mnemonic(_("使用搜索歌词功能"));
        gtk_menu_shell_append(GTK_MENU_SHELL(lrc_item_menu),menu_research_lrc);

        if (!configure.search_lrc)image = gtk_image_new_from_stock (GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
        else image = gtk_image_new_from_stock (GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
        gtk_widget_show (image);
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_research_lrc), image);
        if (configure.search_lrc) {
            menu_research = gtk_image_menu_item_new_with_mnemonic(_("重新搜索歌词"));
            gtk_menu_shell_append(GTK_MENU_SHELL(lrc_item_menu),menu_research);

            image = gtk_image_new_from_stock (GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU);
            gtk_widget_show (image);
            gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_research), image);
        }
        g_signal_connect ((gpointer) menu_research_lrc, "activate",G_CALLBACK (research_lrc),NULL);
        if (configure.search_lrc)g_signal_connect ((gpointer) menu_research, "activate",G_CALLBACK (search_lrc_pre),NULL);

        GtkWidget *menu_desktop_lrc;
        if (configure.desktop_lrc)menu_desktop_lrc = gtk_image_menu_item_new_with_mnemonic(_("关闭桌面歌词"));
        else menu_desktop_lrc = gtk_image_menu_item_new_with_mnemonic(_("打开桌面歌词"));
        gtk_menu_shell_append(GTK_MENU_SHELL(lrc_item_menu),menu_desktop_lrc);

        if (!configure.desktop_lrc)image = gtk_image_new_from_stock (GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
        else image = gtk_image_new_from_stock (GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
        gtk_widget_show (image);
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_desktop_lrc), image);

        GtkWidget *menu_desktop_lrc_lock;
        if (configure.desktop_lrc_lock)menu_desktop_lrc_lock = gtk_image_menu_item_new_with_mnemonic(_("解锁桌面歌词"));
        else menu_desktop_lrc_lock = gtk_image_menu_item_new_with_mnemonic(_("锁定桌面歌词"));
        if (configure.desktop_lrc)gtk_menu_shell_append(GTK_MENU_SHELL(lrc_item_menu),menu_desktop_lrc_lock);

        image = gtk_image_new_from_stock (GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU);
        gtk_widget_show (image);
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_desktop_lrc_lock), image);

        g_signal_connect ((gpointer) menu_desktop_lrc, "activate",G_CALLBACK (close_desk_lrc),NULL);
        g_signal_connect ((gpointer) menu_desktop_lrc_lock, "activate",G_CALLBACK (lock),NULL);
    }
    tray_configure = gtk_image_menu_item_new_with_mnemonic (_("配置"));
    gtk_widget_show (tray_configure);
    gtk_menu_append (GTK_MENU_SHELL (tray_menu), tray_configure);

    image = gtk_image_new_from_stock (GTK_STOCK_PROPERTIES, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tray_configure), image);

    tray_mini = gtk_image_menu_item_new_with_mnemonic (_("迷你模式"));
    gtk_widget_show (tray_mini);
    gtk_menu_append (GTK_MENU_SHELL (tray_menu), tray_mini);

    image = gtk_image_new_from_stock (GTK_STOCK_PROPERTIES, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tray_mini), image);

    GtkWidget * menu_update=gtk_image_menu_item_new_with_label(_("检查更新"));
    image = gtk_image_new_from_stock ("gtk-refresh", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_update), image);

    GtkWidget *menu_web =gtk_image_menu_item_new_with_label(_("iceplayer论坛.."));
    image = gtk_image_new_from_stock (GTK_STOCK_NETWORK, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_web), image);

    GtkWidget *menu_help =gtk_image_menu_item_new_with_label(_("帮助"));
    image = gtk_image_new_from_stock ("gtk-help", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_help), image);

    //gtk_menu_shell_append(GTK_MENU_SHELL(tray_menu),menu_update);
    gtk_menu_shell_append(GTK_MENU_SHELL(tray_menu),menu_web);
    gtk_menu_shell_append(GTK_MENU_SHELL(tray_menu),menu_help);
    inline void go_to_iceplayer_forum() {
        gtk_show_uri(NULL,"http://www.iceplayer.org", 0,NULL);
    }
    g_signal_connect((gpointer) menu_web ,"activate" ,G_CALLBACK(go_to_iceplayer_forum),NULL);
    inline void jump_help() {
        show();
        gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),SWITCH_HELP);
        configure.show_page=SWITCH_HELP;
    }
    g_signal_connect((gpointer) menu_help ,"activate" ,G_CALLBACK(jump_help),NULL);

    tray_about = gtk_image_menu_item_new_with_mnemonic (_("关于"));
    gtk_widget_show (tray_about);
    gtk_menu_append (GTK_MENU_SHELL (tray_menu), tray_about);

    image = gtk_image_new_from_stock (GTK_STOCK_ABOUT, GTK_ICON_SIZE_MENU);
    gtk_widget_show(image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tray_about), image);

    tray_quit = gtk_image_menu_item_new_with_mnemonic (_("退出"));
    gtk_widget_show (tray_quit);
    gtk_menu_append (GTK_MENU_SHELL (tray_menu), tray_quit);

    image = gtk_image_new_from_stock (GTK_STOCK_QUIT, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (tray_quit), image);

    inline void show_or_hide() {
        if ( windows_state != WINDOWS_STATE_SHOW&&windows_state != WINDOWS_STATE_SHOW_ALL_WORK_PLACES) show();
        else hide(NULL);
    }

    inline void jump_configure() {
        show();
        gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),SWITCH_CONFIG);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(configure_widget.notebook),CONFIG_SW_NORMAL);
        configure.show_page=SWITCH_CONFIG;
    }
    g_signal_connect ((gpointer) tray_show_or_hide, "activate",G_CALLBACK (show_or_hide),NULL);
    g_signal_connect ((gpointer) tray_about, "activate",G_CALLBACK (show_about_dialog),NULL);
    g_signal_connect ((gpointer) tray_mini, "activate",G_CALLBACK (mini),NULL);
    g_signal_connect ((gpointer) tray_configure, "activate",G_CALLBACK (jump_configure),NULL);
    g_signal_connect ((gpointer) tray_stop, "activate",G_CALLBACK (stop),NULL);
    g_signal_connect ((gpointer) tray_next, "activate",G_CALLBACK (next),NULL);
    g_signal_connect ((gpointer) tray_open, "activate",G_CALLBACK (add_menu),NULL);
    g_signal_connect ((gpointer) tray_play_or_pause, "activate",G_CALLBACK (play),NULL);
    g_signal_connect ((gpointer) tray_prev, "activate",G_CALLBACK (prev),NULL);
    g_signal_connect ((gpointer) tray_quit, "activate",G_CALLBACK (quit),NULL);

    gtk_widget_show_all(tray_menu);
    popup_menu(tray_menu,0,0);
}
///unused for skin:
void skin_do() {}
void skin_del() {}
void skin_add() {}
void skin_info() {}

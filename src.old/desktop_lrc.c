#include <gtk/gtk.h>
#include "core.h"
#include "widget_lrc.h"
#include "single.h"
#include "desktop_lrc.h"
#include "list.h"
#include "ui.h"
#include "configure.h"
#include "lrc.h"
#include "common.h"

iceplayer_desktop_lrc desktop_lrc;

gboolean desktop_lrc_show_without_composited() {
    print_programming("desktop_lrc_show_without_composited\n");
    GdkWindow *desklrc_win=NULL;
    GdkPixmap *pixmap=NULL;
    gint width=0, height=0;
    cairo_t *cr=NULL;
    PangoLayout *layout=NULL;
    PangoFontDescription *desc=NULL;

    desklrc_win = desktop_lrc.eventbox->window;

    gdk_drawable_get_size(GDK_DRAWABLE(desklrc_win), &width, &height);
    pixmap = gdk_pixmap_new(NULL, width, height, 1);

    gint line = 0;
    if (!desktop_lrc.show_lock_background) {
        cr = gdk_cairo_create(pixmap);
        layout = pango_cairo_create_layout(cr);
        if (lrc_text[0][0]) {
            line = get_lrc_number();
            if (strlen(lrc_text_get_text(lrc_text[line]))>1) {
                pango_layout_set_text (layout,lrc_text_get_text(lrc_text[line]),-1);
                ////print_debug(lrc_text_get_text(lrc_text[line]));
                //g_print("\t歌词行数%d\n",line);
            } else {
                pango_layout_set_text (layout,"……", -1);
            }
        } else {
            if (!core.pipeline&&core.state != GST_STATE_PLAYING) {
                pango_layout_set_text (layout,"Iceplayer 音乐播放器", -1);
            } else {
                if (configure.search_lrc) {
                    switch (lrc_search_data.search_lrc_state) {
                    case SEARCH_LRC_STATE_NONE:
                        pango_layout_set_text (layout,_("iceplayer 音乐播放器"), -1);
                        break;
                    case SEARCH_LRC_STATE_SEARCH:
                        pango_layout_set_text (layout,_("搜索歌词.."), -1);
                        break;
                    case SEARCH_LRC_STATE_NET_SOCKET:
                        pango_layout_set_text (layout,_("Soctet.."), -1);
                        break;
                    case SEARCH_LRC_STATE_NET_DNS:
                        pango_layout_set_text (layout,_("解析DNS.."), -1);
                        break;
                    case SEARCH_LRC_STATE_NET_CONNECT:
                        pango_layout_set_text (layout,_("连接服务器.."), -1);
                        break;
                    case SEARCH_LRC_STATE_NET_SOCKET_ERROR:
                        pango_layout_set_text (layout,_("Soctet失败！网络连接？速度过慢？"), -1);
                        break;
                    case SEARCH_LRC_STATE_NET_DNS_ERROR:
                        pango_layout_set_text (layout,_("解析DNS失败！网络连接？速度过慢？"), -1);
                        break;
                    case SEARCH_LRC_STATE_NET_CONNECT_ERROR:
                        pango_layout_set_text (layout,_("连接失败！www.lrc123.com无法连接？"), -1);
                        break;
                    case SEARCH_LRC_STATE_SEARCHED:
                        pango_layout_set_text (layout,_("搜索成功.."), -1);
                        break;
                    case  SEARCH_LRC_STATE_NOT_FOUND:
                        pango_layout_set_text (layout,_("没有结果"), -1);
                        break;
                    case SEARCH_LRC_STATE_DOWNLOADING:
                        pango_layout_set_text (layout,_("下载歌词(窗体会暂时无反应).."), -1);
                        break;
                    case SEARCH_LRC_STATE_DOWNLOADED:
                        pango_layout_set_text (layout,_("下载成功.."), -1);
                        break;
                    case SEARCH_LRC_STATE_CLOSE:
                        pango_layout_set_text (layout,_("本地无匹配歌词！"), -1);
                        break;
                    case SEARCH_LRC_STATE_FAILED:
                        pango_layout_set_text (layout,_("下载失败..可能是下载的歌词文件内容不正确或者错误"), -1);
                        break;
                    }
                } else {
                    pango_layout_set_text (layout,_("本地无匹配歌词！搜索已关闭。"), -1);
                }
            }
        }

        desc = pango_font_description_from_string(configure.desktop_lrc_font);
        pango_layout_set_font_description(layout, desc);
        pango_font_description_free(desc);
        cairo_save(cr);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
        cairo_set_source_rgba(cr, 1.0, 1.0, 1.0, 0.0);
        cairo_paint(cr);
        cairo_restore (cr);
        cairo_move_to(cr, 0,0);
        pango_cairo_show_layout(cr, layout);
        cairo_destroy(cr);
        g_object_unref(layout);
        gdk_window_shape_combine_mask(desklrc_win, pixmap, 0, 0);
        g_object_unref(pixmap);
    } else {
        gdk_window_shape_combine_mask( desktop_lrc.eventbox->window, NULL, 0, 0);
    }
    return TRUE;
}
gboolean desktop_lrc_show() {

    print_programming("desktop_lrc_show\n");
    //print_debug("进入desktop_lrc_show");

    if (!gdk_screen_is_composited(gdk_screen_get_default ())) {
        //print_debug("没有显卡效果");
        desktop_lrc_show_without_composited();
    } else {
        //print_debug("有显卡效果");
        gdk_window_shape_combine_mask(desktop_lrc.eventbox->window, NULL, 0, 0);
    }
    //抓取鼠标坐标和窗口当前区域进行比对,从而完成那个鼠标在上面就半透明的效果
    gboolean show_half=FALSE;
    gint shux=0,shuy=0;
    gdk_window_get_pointer (gdk_get_default_root_window(),&shux,&shuy,NULL);
    if(shux>configure.desktop_lrc_x&&shux<configure.desktop_lrc_x+configure.desktop_lrc_width&&shuy>configure.desktop_lrc_y&&shux<configure.desktop_lrc_y+configure.desktop_lrc_height)
        show_half=TRUE;

    cairo_t *ctx= gdk_cairo_create(desktop_lrc.eventbox->window);

    gint width = 0;
    gint height = 0;
    gdk_drawable_get_size(desktop_lrc.eventbox->window, &width, &height);
    height = desktop_lrc_word_height*4/7;
    gtk_window_resize(GTK_WINDOW(desktop_lrc.window), width, height);

    if ((!configure.desktop_lrc_lock&&desktop_lrc.show_lock_background)||(gtk_notebook_get_current_page(GTK_NOTEBOOK(configure_widget.lrc_notebook))==2&&gtk_notebook_get_current_page(GTK_NOTEBOOK(iceplayer.notebook))==SWITCH_CONFIG&&gtk_notebook_get_current_page(GTK_NOTEBOOK(configure_widget.notebook))==3&&!configure.desktop_lrc_lock)) {
        //print_debug("画背景");
        cairo_set_source_rgba(ctx,0,0,0,0.5);
        cairo_rectangle(ctx,0,0,(gdouble)width,(gdouble)height);
        cairo_fill(ctx);
    }

    cairo_set_source_rgba(ctx,0,0,0,0);

    PangoLayout *layout = pango_cairo_create_layout(ctx);
    gint line = 0;
    //print_debug("解析歌词");
    if (lrc_text[0][0]) {
        line = get_lrc_number();
        if (strlen(lrc_text_get_text(lrc_text[line]))>1) {
            pango_layout_set_text (layout,lrc_text_get_text(lrc_text[line]),-1);
        } else {
            pango_layout_set_text (layout,"……", -1);
        }
    } else {
        if (!core.pipeline&&core.state != GST_STATE_PLAYING) {
            pango_layout_set_text (layout,"Iceplayer 音乐播放器", -1);
        } else {
            if (configure.search_lrc) {
                switch (lrc_search_data.search_lrc_state) {
                case SEARCH_LRC_STATE_NONE:
                    pango_layout_set_text (layout,_("iceplayer 音乐播放器"), -1);
                    break;
                case SEARCH_LRC_STATE_SEARCH:
                    pango_layout_set_text (layout,_("搜索歌词.."), -1);
                    break;
                case SEARCH_LRC_STATE_NET_SOCKET:
                    pango_layout_set_text (layout,_("Soctet.."), -1);
                    break;
                case SEARCH_LRC_STATE_NET_DNS:
                    pango_layout_set_text (layout,_("解析DNS.."), -1);
                    break;
                case SEARCH_LRC_STATE_NET_CONNECT:
                    pango_layout_set_text (layout,_("连接服务器.."), -1);
                    break;
                case SEARCH_LRC_STATE_NET_SOCKET_ERROR:
                    pango_layout_set_text (layout,_("Soctet失败！网络连接？速度过慢？"), -1);
                    break;
                case SEARCH_LRC_STATE_NET_DNS_ERROR:
                    pango_layout_set_text (layout,_("解析DNS失败！网络连接？速度过慢？"), -1);
                    break;
                case SEARCH_LRC_STATE_NET_CONNECT_ERROR:
                    pango_layout_set_text (layout,_("连接失败！www.lrc123.com无法连接？"), -1);
                    break;
                case SEARCH_LRC_STATE_SEARCHED:
                    pango_layout_set_text (layout,_("搜索成功.."), -1);
                    break;
                case  SEARCH_LRC_STATE_NOT_FOUND:
                    pango_layout_set_text (layout,_("没有结果"), -1);
                    break;
                case SEARCH_LRC_STATE_DOWNLOADING:
                    pango_layout_set_text (layout,_("下载歌词(窗体会暂时无反应).."), -1);
                    break;
                case SEARCH_LRC_STATE_DOWNLOADED:
                    pango_layout_set_text (layout,_("下载成功.."), -1);
                    break;
                case SEARCH_LRC_STATE_CLOSE:
                    pango_layout_set_text (layout,_("本地无匹配歌词！"), -1);
                    break;
                case SEARCH_LRC_STATE_FAILED:
                    pango_layout_set_text (layout,_("下载失败..可能是下载的歌词文件内容不正确或者错误"), -1);
                    break;
                }
            } else {
                pango_layout_set_text (layout,_("本地无匹配歌词！搜索已关闭。"), -1);
            }

        }
    }

    cairo_move_to(ctx, 0, 0);
    PangoFontDescription *pfd = pango_font_description_from_string( configure.desktop_lrc_font);
    pango_layout_set_font_description (layout, pfd);
    pango_font_description_free(pfd);
    pango_layout_get_size(layout,&width,NULL);
    width=width/800;

    //pango_cairo_update_layout(ctx, layout);
    pango_cairo_layout_path(ctx, layout);
    cairo_clip(ctx);

    static GdkColor color;
    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_1), &color);
    cairo_pattern_t *pat = cairo_pattern_create_linear(0, 0, 0, height);

    if (show_half) {
      gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_4), &color);
        cairo_pattern_add_color_stop_rgba(pat, 0.3, (gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0, 0.2);
        gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_5), &color);
        cairo_pattern_add_color_stop_rgba(pat, 0.5, (gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0, 0.2);
        gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_6), &color);
        cairo_pattern_add_color_stop_rgba(pat, 0.7, (gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0, 0.2);

    } else {
      gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_4), &color);
      cairo_pattern_add_color_stop_rgba(pat, 0.3, (gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0, 1.0);
      gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_5), &color);
      cairo_pattern_add_color_stop_rgba(pat, 0.5, (gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0, 1.0);
      gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_6), &color);
      cairo_pattern_add_color_stop_rgba(pat, 0.7, (gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0, 1.0);
    }
    cairo_set_source(ctx, pat);
    cairo_rectangle(ctx, 0, 0, width, height);
    cairo_fill(ctx);

    if (configure.desktop_lrc_kalaok&&lrc_text[0][0]) {
        //print_debug("卡拉ok");
        gint min1,sec1,mir1;
        gint min2,sec2,mir2;
        gint min3,sec3,mir3;

        sscanf(lrc_text[line],"[%02d:%02d.%02d]%*s",&min1,&sec1,&mir1);
        if (core.pos<0)return FALSE;
        min2 = (core.pos )/ 60000000000;
        sec2 = ((core.pos/10000000) % 6000) / 100;
        mir2 = (core.pos/10000000) % 100;
        if (!strncmp(focus_song,"<cue>",5)) {
            gint min_pre,sec_pre,mir_pre;
            gchar * p= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
            if (!strstr(focus_song,"<INDEX_B:NONE>")) {
                strcpy(p,strstr(focus_song,"<INDEX_A"));
                sscanf(p,"<INDEX_A:%d:%d:%d><INDEX_B:%*d:%*d:%*d>%*s",&min_pre,&sec_pre,&mir_pre);
            } else {
                strcpy(p,strstr(focus_song,"<INDEX_A:"));
                p[18]=0;
                gchar *a= (gchar *)g_malloc(5*sizeof(gchar *));
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
                g_free_n(a);
            }
            g_free_n(p);
            min2 = min2-min_pre;
            sec2 = sec2-sec_pre;
            mir2 = mir2-mir_pre;
        }
        if (lrc_text[line+1][0]) {
            sscanf(lrc_text[line+1],"[%02d:%02d.%02d]%*s",&min3,&sec3,&mir3);
        } else {
            min3=min2;
            sec3=sec2;
            mir3=mir3;
        }
        gdouble up_time=(min2-min1)*6000+(sec2-sec1)*100+(mir2-mir1);
        gdouble down_time=(min3-min1)*6000+(sec3-sec1)*100+(mir3-mir1);
        if (up_time<=0||down_time<=0||up_time>down_time) {
            width=1;
        } else {
            width=width*up_time/down_time;
        }
        cairo_pattern_destroy(pat);
        pat = cairo_pattern_create_linear(0, 0, 0, height);

        if (show_half) {
	  gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_1), &color);
	  cairo_pattern_add_color_stop_rgba(pat, 0.3, (gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0, 0.2);
	  gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_2), &color);
	  cairo_pattern_add_color_stop_rgba(pat, 0.5, (gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0, 0.2);
	  gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_3), &color);
	  cairo_pattern_add_color_stop_rgba(pat, 0.7, (gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0, 0.2);
        } else {
	  gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_1), &color);
	  cairo_pattern_add_color_stop_rgba(pat, 0.3, (gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0, 1.0);
	  gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_2), &color);
	  cairo_pattern_add_color_stop_rgba(pat, 0.5, (gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0, 1.0);
	  gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_colorbutton_3), &color);
	  cairo_pattern_add_color_stop_rgba(pat, 0.7, (gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0, 1.0);
        }

        cairo_set_source(ctx, pat);

        cairo_rectangle(ctx, 0, 0, width, height);
        cairo_fill(ctx);
    }
    if (configure.desktop_lrc_word_stroke) {
        pango_cairo_update_layout(ctx, layout);
        pango_cairo_layout_path(ctx, layout);

        gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.desktop_lrc_word_stroke_colorbutton), &color);
        if (show_half) {
            cairo_set_source_rgba (ctx,(gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0,0.2);
        } else {
            cairo_set_source_rgba (ctx,(gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0,1);
        }
        cairo_set_line_width (ctx, (gdouble)((gdouble)configure.desktop_lrc_word_stroke/10.0));
        cairo_stroke_preserve(ctx);
        cairo_clip(ctx);

    }

    g_object_unref(layout);
    cairo_pattern_destroy(pat);
    cairo_destroy(ctx);

    //print_debug("退出desktop_lrc_show");
    return FALSE;
}
void desktop_lrc_init() {
    print_programming("desktop_lrc_init\n");
    desktop_lrc.window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_skip_taskbar_hint(GTK_WINDOW(desktop_lrc.window),TRUE);
    gtk_window_set_decorated(GTK_WINDOW(desktop_lrc.window),FALSE);
    gtk_window_set_keep_above(GTK_WINDOW(desktop_lrc.window),TRUE);
    gtk_window_stick (GTK_WINDOW(desktop_lrc.window));
    gtk_window_set_skip_pager_hint(GTK_WINDOW(desktop_lrc.window),TRUE);
    gtk_widget_set_app_paintable(desktop_lrc.window, TRUE);
    gtk_window_set_title(GTK_WINDOW(desktop_lrc.window), _("iceplayer 桌面歌词"));
    gtk_window_move(GTK_WINDOW(desktop_lrc.window),
		    configure.desktop_lrc_x, configure.desktop_lrc_y);
    gtk_widget_set_size_request(desktop_lrc.window,100,30);
    gtk_window_resize(GTK_WINDOW(desktop_lrc.window),
		      configure.desktop_lrc_width,
		      configure.desktop_lrc_height);
    gtk_widget_add_events(desktop_lrc.window,GDK_ALL_EVENTS_MASK);
    gtk_widget_set_colormap(desktop_lrc.window,
			    gdk_screen_get_rgba_colormap(gtk_widget_get_screen(desktop_lrc.window)));
    gtk_widget_realize(desktop_lrc.window);
    gtk_window_set_icon(GTK_WINDOW(desktop_lrc.window),(GdkPixbuf *)gdk_pixbuf_new_from_file(ICON, NULL));
    desktop_lrc.eventbox  = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(desktop_lrc.eventbox), TRUE);
    gtk_widget_set_has_window(desktop_lrc.eventbox, TRUE);
    gtk_widget_set_app_paintable(desktop_lrc.eventbox, TRUE);
    gtk_container_add (GTK_CONTAINER (desktop_lrc.window), desktop_lrc.eventbox);

    inline gboolean show_lock_back() {
        desktop_lrc.show_lock_background = TRUE;
        return FALSE;
    }
    inline gboolean hide_lock_back() {
        desktop_lrc.show_lock_background = FALSE;
        return FALSE;
    }
    inline gboolean save_pos_and_size() {
      gtk_window_get_size(GTK_WINDOW(desktop_lrc.window),
			  &configure.desktop_lrc_width,
			  &configure.desktop_lrc_height);
      gtk_window_get_position(GTK_WINDOW(desktop_lrc.window),
			      &configure.desktop_lrc_x,
			      &configure.desktop_lrc_y);
        return FALSE;
    }
    inline void move_or_resize_desktop_lrc(GtkWidget *widget,GdkEventButton *event) {
        if (event->button==3) {
            GtkWidget * menu = gtk_menu_new();

            GtkWidget *menu_lock = gtk_image_menu_item_new_with_mnemonic(_("锁定（解锁可以从托盘图标或配置处解锁）"));
            gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_lock);

            GtkWidget *menu_close = gtk_image_menu_item_new_with_mnemonic(_("关闭（打开可以从托盘图标或配置处打开）"));
            gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_close);

            GtkWidget *menu_jump_to_configure = gtk_image_menu_item_new_with_mnemonic(_("跳到配置处"));
            gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_jump_to_configure);

            GtkWidget *menu_tips = gtk_image_menu_item_new_with_mnemonic(_("Tips:移动请按左键，大小请加Ctrl"));
            gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_tips);

            inline void lock() {
	      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.desktop_lrc_lock_checkbutton),TRUE);
	      update_configure();
            }
            inline void close() {
	      gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_desktop_lrc_checkbutton),FALSE);
                update_configure();
            }
            inline void jump() {
                show();
                gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),
					      SWITCH_CONFIG);
                gtk_notebook_set_current_page(GTK_NOTEBOOK(configure_widget.notebook),CONFIG_SW_LRC);
                gtk_notebook_set_current_page(GTK_NOTEBOOK(configure_widget.lrc_notebook),2);
            }
            g_signal_connect ((gpointer) menu_lock, "activate",G_CALLBACK (lock),NULL);
            g_signal_connect ((gpointer) menu_close, "activate",G_CALLBACK (close),NULL);
            g_signal_connect ((gpointer) menu_jump_to_configure, "activate",G_CALLBACK (jump),NULL);

            gtk_widget_show_all(menu);
            popup_menu(GTK_MENU(menu),1,1);
        } else {
            if (!configure.desktop_lrc_lock&&desktop_lrc.show_lock_background) {
                if (event->state==GDK_CONTROL_MASK)
                    gtk_window_begin_resize_drag(GTK_WINDOW(desktop_lrc.window),GDK_WINDOW_EDGE_EAST,1,event->x_root,event->y_root,event->time);
                else
                    gtk_window_begin_move_drag(GTK_WINDOW(desktop_lrc.window),1,event->x_root,event->y_root,event->time);
            }
        }
    }
    inline gboolean desktop_lrc_expose() {
        cairo_t *ctx;
        ctx = gdk_cairo_create(desktop_lrc.eventbox->window);
        cairo_set_source_rgba (ctx, 0.0, 0.0, 0.0, 0.0);
        cairo_set_operator(ctx, CAIRO_OPERATOR_SOURCE);
        cairo_paint(ctx);
        cairo_destroy(ctx);

        desktop_lrc_show( );
        return TRUE;
    }
    g_signal_connect(G_OBJECT(desktop_lrc.window), "configure_event" ,G_CALLBACK(save_pos_and_size), NULL) ;
    g_signal_connect(G_OBJECT(desktop_lrc.eventbox), "expose-event" ,G_CALLBACK(desktop_lrc_expose), NULL) ;
    g_signal_connect(G_OBJECT(desktop_lrc.window), "button-press-event" , G_CALLBACK(move_or_resize_desktop_lrc), NULL);
    g_signal_connect(G_OBJECT(desktop_lrc.window), "enter-notify-event", G_CALLBACK(show_lock_back), NULL);
    g_signal_connect(G_OBJECT(desktop_lrc.window), "leave-notify-event",  G_CALLBACK(hide_lock_back), NULL);

    gtk_widget_show_all(desktop_lrc.window);
    return ;
}

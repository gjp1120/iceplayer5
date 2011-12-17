#include <gtk/gtk.h>
#include "widget_lrc.h"
#include "single.h"
#include "ui.h"
#include "lrc.h"

gint line_number;
gint lrc_line_number_to_show;
GtkWidget **label;
iceplayer_widget_lrc widget_lrc;
gint change_widget_lrc;
gboolean widget_lrc_lock=FALSE;
gchar up_lrc[100][STRINGS_LENGTH];
gchar cen_lrc[STRINGS_LENGTH];
gchar down_lrc[100][STRINGS_LENGTH];

cairo_surface_t *iceplayer_widget_lrc_image = NULL;
gboolean get_iceplayer_widget_lrc_back_pic_lock=FALSE;

void updata_widget_lrc_bg() {
    print_programming("updata_widget_lrc_bg\n");
    if (GTK_IS_WIDGET(widget_lrc.window))gtk_widget_queue_draw(widget_lrc.window);
}
gchar * get_iceplayer_widget_lrc_back_pic() {
    print_programming("get_iceplayer_widget_lrc_back_pic\n");
    if (get_iceplayer_widget_lrc_back_pic_lock==TRUE)
        return "/tmp/iceplayer_widget_lrc_main.png";
    get_iceplayer_widget_lrc_back_pic_lock=TRUE;

    static GdkPixbuf *pixbuf;

    static gchar dir[STRINGS_LENGTH];
    if (configure.widget_lrc_Independent_back_image[0]!= '\0') {
        FILE *file = fopen(configure.widget_lrc_Independent_back_image,"r");
        if (file) {
            fclose(file);
            strcpy(dir,configure.widget_lrc_Independent_back_image);
        } else {
            strcpy(dir,get_skin_dir());
            strcat(dir,"main.png");
        }
    } else {

        strcpy(dir,get_skin_dir());
        strcat(dir,"main.png");
    }

    pixbuf = gdk_pixbuf_new_from_file(dir,NULL);

    if (gdk_screen_is_composited(gdk_screen_get_default ())) {
        pixbuf = gdk_pixbuf_set_alpha(pixbuf,configure.window_alpha);
        gdk_pixbuf_save (pixbuf, "/tmp/iceplayer_widget_lrc_main.png", "png", NULL, NULL);
    } else {
        gdk_pixbuf_save (pixbuf, "/tmp/iceplayer_widget_lrc_main.png", "png", NULL, NULL);
    }

    g_object_unref(pixbuf);

    get_iceplayer_widget_lrc_back_pic_lock=FALSE;

    return "/tmp/iceplayer_widget_lrc_main.png";
}

gboolean widget_lrc_show() {
    print_programming("widget_lrc_show\n");
    if (!configure.lrc)
        return FALSE;

    if (configure.show_page!=SWITCH_LRC&&configure.widget_lrc_show_type==WIDGET_LRC_SHOW_TYPE_INTEGRATION)
        return FALSE;

    if ((configure.show_page!=SWITCH_LRC&&configure.show_page!=SWITCH_LIST)&&configure.widget_lrc_show_type==WIDGET_LRC_SHOW_TYPE_HPANED)
        return FALSE;

    cairo_t *cr;
    if (configure.widget_lrc_show_type!=WIDGET_LRC_SHOW_TYPE_INTEGRATION && configure.widget_lrc_show_type!=  WIDGET_LRC_SHOW_TYPE_HPANED) {
        cr = gdk_cairo_create(widget_lrc.eventbox->window);
    } else {
        cr = gdk_cairo_create(iceplayer.eventbox->window);
    }

    PangoLayout *layout;
    layout = pango_cairo_create_layout(cr);
    PangoFontDescription *pfd = pango_font_description_from_string( configure.widget_lrc_font);
    pango_layout_set_font_description (layout, pfd);
    pango_font_description_free(pfd);

    cairo_move_to(cr, 0, 0);
    gint n=0;
    gint a=0;

    GdkColor color;
    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.widget_lrc_colorbutton_2),&color);
    cairo_set_source_rgb (cr,(gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0);


    while (n<= lrc_line_number_to_show ) {
        if (configure.widget_lrc_show_type==  WIDGET_LRC_SHOW_TYPE_HPANED)
            cairo_move_to(cr, 15+gtk_paned_get_position(GTK_PANED(iceplayer.hpaned)), iceplayer.main_fixed->allocation.height + a * widget_lrc_word_height+10);
        else if (configure.widget_lrc_show_type==  WIDGET_LRC_SHOW_TYPE_INTEGRATION)
            cairo_move_to(cr, 15, iceplayer.main_fixed->allocation.height + a * widget_lrc_word_height+10);
        else
            cairo_move_to(cr, 15, a * widget_lrc_word_height+10);
        if (up_lrc[n][0]!=0) {
            pango_layout_set_text (layout,up_lrc[n], -1);
            pango_cairo_show_layout(cr, layout);
        }
        n++;
        a++;
    }

    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.widget_lrc_colorbutton_1),&color);
    cairo_set_source_rgb (cr,(gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0);

    if (configure.widget_lrc_show_type==  WIDGET_LRC_SHOW_TYPE_HPANED)
        cairo_move_to(cr, 15+gtk_paned_get_position(GTK_PANED(iceplayer.hpaned)), iceplayer.main_fixed->allocation.height + a * widget_lrc_word_height+10);
    else if (configure.widget_lrc_show_type==  WIDGET_LRC_SHOW_TYPE_INTEGRATION)
        cairo_move_to(cr, 15, iceplayer.main_fixed->allocation.height + a * widget_lrc_word_height+10);
    else
        cairo_move_to(cr, 15, a * widget_lrc_word_height+10);

    pango_layout_set_text (layout,cen_lrc, -1);
    pango_cairo_show_layout(cr, layout);

    a++;

    gtk_color_button_get_color (GTK_COLOR_BUTTON(configure_widget.widget_lrc_colorbutton_2),&color);
    cairo_set_source_rgb (cr,(gdouble)color.red/65535.0, (gdouble)color.green/65535.0, (gdouble)color.blue/65535.0);

    n=0;

    while (n<=lrc_line_number_to_show) {
        if (configure.widget_lrc_show_type==  WIDGET_LRC_SHOW_TYPE_HPANED)
            cairo_move_to(cr, 15+gtk_paned_get_position(GTK_PANED(iceplayer.hpaned)), iceplayer.main_fixed->allocation.height + a * widget_lrc_word_height+10);
        else if (configure.widget_lrc_show_type==  WIDGET_LRC_SHOW_TYPE_INTEGRATION)
            cairo_move_to(cr, 15, iceplayer.main_fixed->allocation.height + a * widget_lrc_word_height+10);
        else
            cairo_move_to(cr, 15, a * widget_lrc_word_height+10);
        if (down_lrc[n][0]!=0) {
            pango_layout_set_text (layout,down_lrc[n], -1);
            pango_cairo_show_layout(cr, layout);
        }
        n++;
        a++;
    }


    g_object_unref(layout);
    cairo_destroy(cr);

    widget_lrc_lock=FALSE;
    return FALSE;
}
/*
*反应iceplayer_widget_lrc主窗口的expose信号，主要是画背景图片
*支持半透明
*/
gboolean on_iceplayer_widget_lrc_expose() {
    print_programming("on_iceplayer_widget_lrc_expose\n");
    //因为在启动读配置的时候，会执行一次这个，那时窗口还没建立，所以要排除这两个情况

    if (!GDK_IS_WINDOW(gtk_widget_get_window(widget_lrc.window))||!iceplayer_widget_lrc_image) {
        return FALSE;
    }
//widget_lrc_lock=TRUE;
    if (configure.widget_lrc_show_type!=WIDGET_LRC_SHOW_TYPE_INTEGRATION && configure.widget_lrc_show_type!=  WIDGET_LRC_SHOW_TYPE_HPANED) {
        if (iceplayer_widget_lrc_image==NULL)
            return FALSE ;
        cairo_t *cr;
        cr = gdk_cairo_create(widget_lrc.back_eventbox->window);
        /*
            缩放
            */
        gint height = cairo_image_surface_get_height(iceplayer_widget_lrc_image);
        gint width = cairo_image_surface_get_width(iceplayer_widget_lrc_image);

        if ((gdouble)configure.widget_lrc_Independent_width/(gdouble)configure.widget_lrc_Independent_height*(gdouble)height<(gdouble)width) {
            cairo_scale(cr,(gdouble)configure.widget_lrc_Independent_height/(gdouble)height,(gdouble)configure.widget_lrc_Independent_height/(gdouble)height);
        } else {
            cairo_scale(cr,(gdouble)configure.widget_lrc_Independent_width/(gdouble)width,(gdouble)configure.widget_lrc_Independent_width/(gdouble)width);
        }
        /*
        缩放
        */
        cairo_set_source_surface(cr, iceplayer_widget_lrc_image, 0, 0);
        cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);
        cairo_move_to(cr, 0, 0);
        cairo_paint(cr);
        cairo_destroy(cr);
    }

//if(G_IS_OBJECT(cr))g_object_unref(cr);
    //g_timeout_add(10,widget_lrc_show,NULL);
    widget_lrc_show();

    return FALSE;
}
/*
*由图片和eventbox组成的按钮，其图片变化的信号分发函数
*type决定了其是哪一个按钮，比如CONTROL_STOP就是停止按钮的
*/
void widet_lrc_move_or_reize(GtkWidget *widget,GdkEventButton *event) {
    print_programming("widet_lrc_move_or_reize\n");
    if (event->state==GDK_CONTROL_MASK)
        gtk_window_begin_resize_drag(GTK_WINDOW(widget_lrc.window),GDK_WINDOW_EDGE_SOUTH_EAST,1,event->x_root,event->y_root,event->time);
    else
        gtk_window_begin_move_drag(GTK_WINDOW(widget_lrc.window),1,event->x_root,event->y_root,event->time);
}
gboolean close_Independent() {
    print_programming("close_Independent\n");
    configure.widget_lrc_show_type = WIDGET_LRC_SHOW_TYPE_INTEGRATION;
    gtk_window_get_position(GTK_WINDOW(widget_lrc.window),&configure.widget_lrc_Independent_x,&configure.widget_lrc_Independent_y);
    gtk_window_get_size(GTK_WINDOW(widget_lrc.window),&configure.widget_lrc_Independent_width,&configure.widget_lrc_Independent_height);
    gtk_widget_hide(widget_lrc.window);

    gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),SWITCH_LRC);
    configure.show_page=SWITCH_LRC;
    widget_lrc_refresh(TRUE);
    return FALSE;
}
gboolean min_Independent() {
    print_programming("min_Independent\n");
    gtk_window_iconify (GTK_WINDOW(widget_lrc.window) );
    return FALSE;
}
gboolean max_Independent() {
    print_programming("max_Independent\n");
    if (GTK_WINDOW(widget_lrc.window)->maximize_initially) {
        gtk_window_unmaximize(GTK_WINDOW(widget_lrc.window) );
    } else {
        gtk_window_maximize (GTK_WINDOW(widget_lrc.window) );
    }
    return FALSE;
}
void widget_menu(GtkWidget *widget,GdkEventButton *event) {
    print_programming("widget_menu\n");
    if (event->button==3) {
        main_window_menu();
    }
}
gboolean resize_Independent (GtkWidget *widget,GdkEventButton *event,gint type) {
    print_programming("resize_Independent\n");
    if (type == RESIZE_NONE)
        return FALSE;
    gtk_window_begin_resize_drag(GTK_WINDOW(widget_lrc.window),type,event->button,event->x_root,event->y_root,event->time);
    return FALSE;
}
gboolean widget_lrc_size_changed() {
    print_programming("widget_lrc_size_changed\n");
    gint x,y;
    gint w,h;

    gtk_window_get_position(GTK_WINDOW(widget_lrc.window),&x,&y);
    gtk_window_get_size(GTK_WINDOW(widget_lrc.window),&w,&h);

    if (x!=0) configure.widget_lrc_Independent_x=x;
    if (y!=0)configure.widget_lrc_Independent_y=y;
    if (w!=170)configure.widget_lrc_Independent_width=w;
    if (h!=250)configure.widget_lrc_Independent_height=h;


    gchar *dir= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(dir,get_skin_dir());

    gtk_widget_set_usize(widget_lrc.fixed,configure.widget_lrc_Independent_width,configure.widget_lrc_Independent_height);
    gtk_widget_set_usize(widget_lrc.table,configure.widget_lrc_Independent_width,configure.widget_lrc_Independent_height);
    gtk_widget_set_usize(widget_lrc.back_eventbox,configure.widget_lrc_Independent_width,configure.widget_lrc_Independent_height);

    gtk_widget_set_size_request (widget_lrc.above_image,0, 0);
    gtk_widget_set_size_request (widget_lrc.above_eventbox,skin.control_above_width, skin.control_above_height);
    gtk_widget_set_size_request (widget_lrc.max_image,0,0);
    gtk_widget_set_size_request (widget_lrc.max_eventbox,skin.control_max_width,  skin.control_max_height);
    gtk_widget_set_size_request (widget_lrc.min_image,0, 0);
    gtk_widget_set_size_request (widget_lrc.min_eventbox, skin.control_min_width, skin.control_min_height);
    gtk_widget_set_size_request (widget_lrc.close_image,0, 0);
    gtk_widget_set_size_request (widget_lrc.close_eventbox,skin.control_close_width, skin.control_close_height);

    inline void gtk_fixed_move_new2(GtkFixed *fixed,GtkWidget *widget,gint x,gint y) {
        if (widget->parent)
            gtk_fixed_move(fixed,widget,x>=0?x:configure.widget_lrc_Independent_width-4+x,y>=0?y:configure.widget_lrc_Independent_height+y);
        else
            gtk_fixed_put(fixed,widget,x>=0?x:configure.widget_lrc_Independent_width-4+x,y>=0?y:configure.widget_lrc_Independent_height+y);
    }

    gtk_fixed_move_new2 (GTK_FIXED (widget_lrc.fixed), widget_lrc.close_eventbox,skin.control_close_x,skin.control_close_y);
    gtk_fixed_move_new2 (GTK_FIXED (widget_lrc.fixed), widget_lrc.min_eventbox, skin.control_min_x, skin.control_min_y);
    gtk_fixed_move_new2 (GTK_FIXED (widget_lrc.fixed), widget_lrc.max_eventbox,skin.control_max_x, skin.control_max_y);
    gtk_fixed_move_new2 (GTK_FIXED (widget_lrc.fixed), widget_lrc.above_eventbox, skin.control_above_x, skin.control_above_y);


    gtk_container_remove_new((GtkContainer *)widget_lrc.left_eventbox,widget_lrc.left_image);
    gtk_container_remove_new((GtkContainer *)widget_lrc.right_eventbox,widget_lrc.right_image);
    gtk_container_remove_new((GtkContainer *)widget_lrc.buttom_eventbox,widget_lrc.buttom_image);
    gtk_container_remove_new((GtkContainer *)widget_lrc.top_eventbox,widget_lrc.top_image);

    GdkPixbuf *pixbuf1=NULL,*pixbuf2=NULL;

    pixbuf1 = gdk_pixbuf_new_from_file_new(dir,"buttom");
    pixbuf2 = gdk_pixbuf_scale_simple(pixbuf1,configure.widget_lrc_Independent_width-4,2,GDK_INTERP_HYPER);
    widget_lrc.buttom_image=gtk_image_new_from_pixbuf(pixbuf2);
    g_object_unref(pixbuf1);
    g_object_unref(pixbuf2);

    pixbuf1 = gdk_pixbuf_new_from_file_new(dir,"left");
    pixbuf2 = gdk_pixbuf_scale_simple(pixbuf1,2,configure.widget_lrc_Independent_height-4,GDK_INTERP_HYPER);
    widget_lrc.left_image=gtk_image_new_from_pixbuf(pixbuf2);
    g_object_unref(pixbuf1);
    g_object_unref(pixbuf2);

    pixbuf1 = gdk_pixbuf_new_from_file_new(dir,"right");
    pixbuf2 = gdk_pixbuf_scale_simple(pixbuf1,2,configure.widget_lrc_Independent_height-4,GDK_INTERP_HYPER);
    widget_lrc.right_image=gtk_image_new_from_pixbuf(pixbuf2);
    g_object_unref(pixbuf1);
    g_object_unref(pixbuf2);

    pixbuf1 = gdk_pixbuf_new_from_file_new(dir,"top");
    pixbuf2 = gdk_pixbuf_scale_simple(pixbuf1,configure.widget_lrc_Independent_width-4,2,GDK_INTERP_HYPER);
    widget_lrc.top_image=gtk_image_new_from_pixbuf(pixbuf2);
    g_object_unref(pixbuf1);
    g_object_unref(pixbuf2);

    gtk_widget_show(widget_lrc.buttom_image);
    gtk_widget_show(widget_lrc.right_image);
    gtk_widget_show(widget_lrc.left_image);
    gtk_widget_show(widget_lrc.top_image);
    gtk_container_add((GtkContainer *)widget_lrc.left_eventbox,widget_lrc.left_image);
    gtk_container_add((GtkContainer *)widget_lrc.right_eventbox,widget_lrc.right_image);
    gtk_container_add((GtkContainer *)widget_lrc.buttom_eventbox,widget_lrc.buttom_image);
    gtk_container_add((GtkContainer *)widget_lrc.top_eventbox,widget_lrc.top_image);

    gtk_widget_set_usize(widget_lrc.label,configure.widget_lrc_Independent_width-4,configure.widget_lrc_Independent_height-4);

    GdkModifierType mask;
    gdk_window_get_pointer (iceplayer.window->window,&x, &y, &mask);

    if(mask != GDK_BUTTON1_MASK)
        updata_widget_lrc_bg();

    g_free_n(dir);
    return FALSE;
}
void widget_lrc_above() {
    print_programming("widget_lrc_above\n");
    gchar *path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(path,"above");
    if (!configure.widget_lrc_Independent_above)
        strcat(path,".on");

    gchar *dir= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(dir,get_skin_dir());
    gtk_container_remove_new(GTK_CONTAINER(widget_lrc.above_eventbox),widget_lrc.above_image);
    widget_lrc.above_image = gtk_image_new_from_file_new(dir,path);
    g_free_n(dir);
    g_free_n(path);
    gtk_widget_show(widget_lrc.above_image);
    gtk_container_add(GTK_CONTAINER(widget_lrc.above_eventbox),widget_lrc.above_image);

    gtk_window_set_keep_above(GTK_WINDOW(widget_lrc.window),!configure.widget_lrc_Independent_above);
    configure.widget_lrc_Independent_above=!configure.widget_lrc_Independent_above;
}
void widget_lrc_Independent_init() {
    print_programming("widget_lrc_Independent_init\n");

    widget_lrc.window = gtk_window_new (GTK_WINDOW_TOPLEVEL);
    gtk_window_set_icon(GTK_WINDOW(widget_lrc.window),(GdkPixbuf *)gdk_pixbuf_new_from_file(ICON, NULL));
    gtk_window_set_title (GTK_WINDOW (widget_lrc.window), _("iceplayer 窗口歌词"));
    gtk_window_set_decorated(GTK_WINDOW(widget_lrc.window),FALSE);
    gtk_window_stick (GTK_WINDOW(widget_lrc.window));
    gtk_window_set_skip_pager_hint(GTK_WINDOW(widget_lrc.window),TRUE);
    gtk_widget_set_app_paintable(widget_lrc.window, TRUE);
    gtk_widget_add_events(widget_lrc.window,GDK_ALL_EVENTS_MASK);
    gtk_widget_set_size_request (widget_lrc.window, 170,250);

    //gtk_widget_show (widget_lrc.fixed);
    widget_lrc.eventbox = gtk_event_box_new();
    gtk_event_box_set_visible_window(GTK_EVENT_BOX(widget_lrc.eventbox), TRUE);
    gtk_widget_set_has_window(widget_lrc.eventbox, TRUE);
    gtk_widget_set_app_paintable(widget_lrc.eventbox, TRUE);
    gtk_container_add (GTK_CONTAINER (widget_lrc.window), widget_lrc.eventbox);

    widget_lrc.fixed = gtk_fixed_new ();
    gtk_container_add(GTK_CONTAINER(widget_lrc.eventbox), widget_lrc.fixed);
    gtk_widget_set_size_request (widget_lrc.fixed, 170,250);
    gtk_window_resize (GTK_WINDOW(widget_lrc.window), configure.widget_lrc_Independent_width,configure.widget_lrc_Independent_height);

    widget_lrc.back_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.back_eventbox);
    gtk_fixed_put (GTK_FIXED (widget_lrc.fixed), widget_lrc.back_eventbox, 0, 0);
    gtk_widget_set_size_request (widget_lrc.back_eventbox, 170,250);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.back_eventbox), FALSE);

    ////gtk_widget_show (widget_lrc.back_image);
    //gtk_widget_set_size_request (widget_lrc.back_image, 170,250);
    //gtk_container_add (GTK_CONTAINER (widget_lrc.back_eventbox), widget_lrc.back_image);
// gtk_misc_set_alignment (GTK_MISC (widget_lrc.back_image), 0, 0);

    widget_lrc.label = gtk_label_new("");
    //gtk_widget_show(widget_lrc.label);
    gtk_label_set_justify(GTK_LABEL(widget_lrc.label),GTK_JUSTIFY_CENTER);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.label), 0.5, 0.5);
    gtk_widget_set_size_request(widget_lrc.label,0,0);

    widget_lrc.table = gtk_table_new (3, 3, FALSE);
    //gtk_widget_show (widget_lrc.table);
    gtk_fixed_put (GTK_FIXED (widget_lrc.fixed), widget_lrc.table, 0, 0);
    gtk_widget_set_size_request (widget_lrc.table, 170,250);
    widget_lrc.close_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.close_eventbox);

    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.close_eventbox), FALSE);

    //gtk_widget_show (widget_lrc.close_image);

    gtk_container_add (GTK_CONTAINER (widget_lrc.close_eventbox), widget_lrc.close_image);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.close_image), 0, 0);

    widget_lrc.min_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.min_eventbox);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.min_eventbox), FALSE);

    //gtk_widget_show (widget_lrc.min_image);
    gtk_container_add (GTK_CONTAINER (widget_lrc.min_eventbox), widget_lrc.min_image);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.min_image), 0, 0);

    widget_lrc.max_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.max_eventbox);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.max_eventbox), FALSE);

    //gtk_widget_show (widget_lrc.max_image);
    gtk_container_add (GTK_CONTAINER (widget_lrc.max_eventbox), widget_lrc.max_image);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.max_image), 0, 0);

    widget_lrc.above_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.above_eventbox);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.above_eventbox), FALSE);

    //gtk_widget_show (widget_lrc.above_image);
    gtk_container_add (GTK_CONTAINER (widget_lrc.above_eventbox), widget_lrc.above_image);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.above_image), 0, 0);

    widget_lrc.top_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.top_eventbox);
    gtk_widget_set_size_request (widget_lrc.top_eventbox, 2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.top_eventbox), FALSE);
    //gtk_widget_show (widget_lrc.top_image);
    gtk_widget_set_size_request (widget_lrc.top_image, 2,2);
    gtk_container_add (GTK_CONTAINER (widget_lrc.top_eventbox), widget_lrc.top_image);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.top_image), 0, 0);

    widget_lrc.buttom_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.buttom_eventbox);

    gtk_widget_set_size_request (widget_lrc.buttom_eventbox,2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.buttom_eventbox), FALSE);
    //gtk_widget_show (widget_lrc.buttom_image);
    gtk_widget_set_size_request (widget_lrc.buttom_image, 2,2);
    gtk_container_add (GTK_CONTAINER (widget_lrc.buttom_eventbox), widget_lrc.buttom_image);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.buttom_image), 0, 0);

    widget_lrc.buttom_left_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.buttom_left_eventbox);
    gtk_widget_set_size_request (widget_lrc.buttom_left_eventbox,2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.buttom_left_eventbox), FALSE);
    //gtk_widget_show (widget_lrc.buttom_left_image);
    gtk_widget_set_size_request (widget_lrc.buttom_left_image, 2,2);
    gtk_container_add (GTK_CONTAINER (widget_lrc.buttom_left_eventbox), widget_lrc.buttom_left_image);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.buttom_left_image), 0, 0);

    widget_lrc.buttom_right_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.buttom_right_eventbox);
    gtk_widget_set_size_request (widget_lrc.buttom_right_eventbox,2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.buttom_right_eventbox), FALSE);
    //gtk_widget_show (widget_lrc.buttom_right_image);
    gtk_widget_set_size_request (widget_lrc.buttom_right_image, 2,2);
    gtk_container_add (GTK_CONTAINER (widget_lrc.buttom_right_eventbox), widget_lrc.buttom_right_image);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.buttom_right_image), 0, 0);

    widget_lrc.left_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.left_eventbox);
    gtk_widget_set_size_request (widget_lrc.left_eventbox, 2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.left_eventbox), FALSE);
    //gtk_widget_show (widget_lrc.left_image);
    gtk_widget_set_size_request (widget_lrc.left_image, 2,2);
    gtk_container_add (GTK_CONTAINER (widget_lrc.left_eventbox), widget_lrc.left_image);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.left_image), 0, 0);
    widget_lrc.right_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.right_eventbox);
    gtk_widget_set_size_request (widget_lrc.right_eventbox, 2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.right_eventbox), FALSE);
    //gtk_widget_show (widget_lrc.right_image);
    gtk_widget_set_size_request (widget_lrc.right_image, 2,2);
    gtk_container_add (GTK_CONTAINER (widget_lrc.right_eventbox), widget_lrc.right_image);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.right_image), 0, 0);
    widget_lrc.top_left_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.top_left_eventbox);

    gtk_widget_set_size_request (widget_lrc.top_left_eventbox, 2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.top_left_eventbox), FALSE);
    //gtk_widget_show (widget_lrc.top_left_image);
    gtk_widget_set_size_request (widget_lrc.top_left_image, 2,2);
    gtk_container_add (GTK_CONTAINER (widget_lrc.top_left_eventbox), widget_lrc.top_left_image);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.top_left_image), 0, 0);
    widget_lrc.top_right_eventbox = gtk_event_box_new ();
    //gtk_widget_show (widget_lrc.top_right_eventbox);

    gtk_widget_set_size_request (widget_lrc.top_right_eventbox, 2,2);
    gtk_event_box_set_visible_window (GTK_EVENT_BOX (widget_lrc.top_right_eventbox), FALSE);
    //gtk_widget_show (widget_lrc.top_right_image);
    gtk_widget_set_size_request (widget_lrc.top_right_image,2,2);
    gtk_container_add (GTK_CONTAINER (widget_lrc.top_right_eventbox), widget_lrc.top_right_image);
    gtk_misc_set_alignment (GTK_MISC (widget_lrc.top_right_image), 0, 0);

    gtk_widget_set_size_request (widget_lrc.above_image,0, 0);
    gtk_widget_set_size_request (widget_lrc.above_eventbox,skin.control_above_width, skin.control_above_height);
    gtk_widget_set_size_request (widget_lrc.max_image,0,0);
    gtk_widget_set_size_request (widget_lrc.max_eventbox,skin.control_max_width,  skin.control_max_height);
    gtk_widget_set_size_request (widget_lrc.min_image,0, 0);
    gtk_widget_set_size_request (widget_lrc.min_eventbox, skin.control_min_width, skin.control_min_height);
    gtk_widget_set_size_request (widget_lrc.close_image,0, 0);
    gtk_widget_set_size_request (widget_lrc.close_eventbox,skin.control_close_width, skin.control_close_height);

    gtk_fixed_move_new (widget_lrc.fixed, widget_lrc.close_eventbox,skin.control_close_x,skin.control_close_y);
    gtk_fixed_move_new (widget_lrc.fixed, widget_lrc.min_eventbox, skin.control_min_x, skin.control_min_y);
    gtk_fixed_move_new (widget_lrc.fixed, widget_lrc.max_eventbox,skin.control_max_x, skin.control_max_y);
    gtk_fixed_move_new (widget_lrc.fixed, widget_lrc.above_eventbox, skin.control_above_x, skin.control_above_y);


    gtk_table_attach (GTK_TABLE (widget_lrc.table), widget_lrc.right_eventbox, 2, 3, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) ( GTK_FILL), 0, 0);
    gtk_table_attach (GTK_TABLE (widget_lrc.table), widget_lrc.buttom_right_eventbox, 2, 3, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) ( GTK_FILL), 0, 0);
    gtk_table_attach (GTK_TABLE (widget_lrc.table), widget_lrc.buttom_left_eventbox, 0, 1, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) ( GTK_FILL), 0, 0);
    gtk_table_attach (GTK_TABLE (widget_lrc.table), widget_lrc.left_eventbox, 0, 1, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) ( GTK_FILL), 0, 0);
    gtk_table_attach (GTK_TABLE (widget_lrc.table), widget_lrc.top_eventbox, 1, 2, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) ( GTK_FILL), 0, 0);
    gtk_table_attach (GTK_TABLE (widget_lrc.table), widget_lrc.top_left_eventbox, 0, 1, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) ( GTK_FILL), 0, 0);
    gtk_table_attach (GTK_TABLE (widget_lrc.table), widget_lrc.label, 1, 2, 1, 2,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_table_attach (GTK_TABLE (widget_lrc.table), widget_lrc.buttom_eventbox, 1, 2, 2, 3,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) ( GTK_FILL), 0, 0);
    gtk_table_attach (GTK_TABLE (widget_lrc.table), widget_lrc.top_right_eventbox, 2, 3, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) ( GTK_FILL), 0, 0);
    ///


    GdkScreen *screen;
    GdkColormap *colormap;

    screen = gtk_widget_get_screen(widget_lrc.window);       // 重要
    colormap = gdk_screen_get_rgba_colormap(screen);
    gtk_widget_set_colormap(widget_lrc.window, colormap);


    g_signal_connect ((gpointer) widget_lrc.window, "delete_event"      ,G_CALLBACK (close_Independent),NULL);
    g_signal_connect ((gpointer) widget_lrc.window, "focus_in_event"   ,G_CALLBACK (widget_lrc_size_changed),NULL);
    g_signal_connect ((gpointer) widget_lrc.window, "configure_event"   ,G_CALLBACK (widget_lrc_size_changed),NULL);
    g_signal_connect ((gpointer) widget_lrc.window, "button_press_event"   ,G_CALLBACK (widget_menu),NULL);
    g_signal_connect ((gpointer) widget_lrc.eventbox, "expose-event"   ,G_CALLBACK (on_iceplayer_widget_lrc_expose),NULL);


    g_signal_connect ((gpointer) widget_lrc.back_eventbox,     "button_press_event",G_CALLBACK (widet_lrc_move_or_reize),NULL);

    g_signal_connect ((gpointer) widget_lrc.above_eventbox,       "button_press_event",G_CALLBACK (widget_lrc_above),(gpointer)WIDGET_LRC_CONTROL_ABOVE);

    g_signal_connect ((gpointer) widget_lrc.close_eventbox,     "enter_notify_event",G_CALLBACK (change_picture),(gpointer)WIDGET_LRC_CONTROL_CLOSE);
    g_signal_connect ((gpointer) widget_lrc.close_eventbox,     "leave_notify_event",G_CALLBACK (change_picture),(gpointer)WIDGET_LRC_CONTROL_CLOSE);
    g_signal_connect ((gpointer) widget_lrc.close_eventbox,     "button_press_event",G_CALLBACK (close_Independent),NULL);

    g_signal_connect ((gpointer) widget_lrc.min_eventbox,       "enter_notify_event",G_CALLBACK (change_picture),(gpointer)WIDGET_LRC_CONTROL_MIN);
    g_signal_connect ((gpointer) widget_lrc.min_eventbox,        "leave_notify_event",G_CALLBACK (change_picture),(gpointer)WIDGET_LRC_CONTROL_MIN);
    g_signal_connect ((gpointer) widget_lrc.min_eventbox,       "button_press_event",G_CALLBACK (min_Independent),NULL);

    g_signal_connect ((gpointer) widget_lrc.max_eventbox,       "enter_notify_event",G_CALLBACK (change_picture),(gpointer)WIDGET_LRC_CONTROL_MAX);
    g_signal_connect ((gpointer) widget_lrc.max_eventbox,       "leave_notify_event",G_CALLBACK (change_picture),(gpointer)WIDGET_LRC_CONTROL_MAX);
    g_signal_connect ((gpointer) widget_lrc.max_eventbox,       "button_press_event",G_CALLBACK (max_Independent),NULL);

    g_signal_connect ((gpointer) widget_lrc.top_eventbox,       "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_TOP);
    g_signal_connect ((gpointer) widget_lrc.top_eventbox,       "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) widget_lrc.top_eventbox,       "button_press_event",G_CALLBACK (resize_Independent),(gpointer)RESIZE_TOP);

    g_signal_connect ((gpointer) widget_lrc.buttom_eventbox,       "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_BUTTOM);
    g_signal_connect ((gpointer) widget_lrc.buttom_eventbox,       "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) widget_lrc.buttom_eventbox,       "button_press_event",G_CALLBACK (resize_Independent),(gpointer)RESIZE_BUTTOM);

    g_signal_connect ((gpointer) widget_lrc.left_eventbox,       "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_LEFT);
    g_signal_connect ((gpointer) widget_lrc.left_eventbox,       "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) widget_lrc.left_eventbox,       "button_press_event",G_CALLBACK (resize_Independent),(gpointer)RESIZE_LEFT);

    g_signal_connect ((gpointer) widget_lrc.right_eventbox,       "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_RIGHT);
    g_signal_connect ((gpointer) widget_lrc.right_eventbox,       "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) widget_lrc.right_eventbox,       "button_press_event",G_CALLBACK (resize_Independent),(gpointer)RESIZE_RIGHT);

    g_signal_connect ((gpointer) widget_lrc.buttom_left_eventbox,       "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_BUTTOM_LEFT);
    g_signal_connect ((gpointer) widget_lrc.buttom_left_eventbox,       "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) widget_lrc.buttom_left_eventbox,       "button_press_event",G_CALLBACK (resize_Independent),(gpointer)RESIZE_BUTTOM_LEFT);

    g_signal_connect ((gpointer) widget_lrc.buttom_right_eventbox,       "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_BUTTOM_RIGHT);
    g_signal_connect ((gpointer) widget_lrc.buttom_right_eventbox,       "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) widget_lrc.buttom_right_eventbox,       "button_press_event",G_CALLBACK (resize_Independent),(gpointer)RESIZE_BUTTOM_RIGHT);

    g_signal_connect ((gpointer) widget_lrc.top_left_eventbox,       "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_TOP_LEFT);
    g_signal_connect ((gpointer) widget_lrc.top_left_eventbox,       "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) widget_lrc.top_left_eventbox,       "button_press_event",G_CALLBACK (resize_Independent),(gpointer)RESIZE_TOP_LEFT);

    g_signal_connect ((gpointer) widget_lrc.top_right_eventbox,       "enter_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_TOP_RIGHT);
    g_signal_connect ((gpointer) widget_lrc.top_right_eventbox,       "leave_notify_event",G_CALLBACK (change_cursor),(gpointer)RESIZE_NONE);
    g_signal_connect ((gpointer) widget_lrc.top_right_eventbox,       "button_press_event",G_CALLBACK (resize_Independent),(gpointer)RESIZE_TOP_RIGHT);

}
void widget_lrc_refresh(gboolean force) {
    print_programming("widget_lrc_refresh\n");
    if (widget_lrc_lock)
        return;

    switch (configure.widget_lrc_show_type) {
    case WIDGET_LRC_SHOW_TYPE_HPANED: {
        if (configure.hpaned_pos>configure.width-20) {
            gtk_paned_set_position(GTK_PANED(iceplayer.hpaned),configure.width-20>0?configure.width-20:0);
            configure.hpaned_pos= configure.width-20;
        }
        gint height =  configure.height-iceplayer.switch_hbox->allocation.height;
        gint  width = configure.width-configure.hpaned_pos-20;
        if (iceplayer.switch_hbox->allocation.width+20>configure.width) {
            width = iceplayer.widget_lrc_label_hpaned->allocation.width - (iceplayer.switch_hbox->allocation.width - configure.width) ;
        }
        //if (GTK_IS_WIDGET (iceplayer.widget_lrc_label_hpaned) )gtk_widget_set_usize(iceplayer.widget_lrc_label_hpaned,(width>0?width:1),(height>0?height:1));
        height =  configure.height-iceplayer.switch_hbox->allocation.height-50;
        lrc_line_number_to_show = (gdouble)(height>0?height:1)/ (gdouble)widget_lrc_word_height/2.0;
        if (configure.show_page!=SWITCH_LIST&&configure.show_page!=SWITCH_LRC)
            return ;
        break;
    }
    case WIDGET_LRC_SHOW_TYPE_INTEGRATION:

        // gtk_widget_set_usize(iceplayer.widget_lrc_label,configure.width-64, configure.height-iceplayer.main_fixed->allocation.height-iceplayer.switch_hbox->allocation.height-24);
        if (GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned)&&GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned->parent->parent)&&GTK_IS_WIDGET(iceplayer.hpaned)&&iceplayer.back_eventbox_widget_lrc_hpaned->parent&&iceplayer.back_eventbox_widget_lrc_hpaned==iceplayer.hpaned->parent)gtk_container_remove_new(GTK_CONTAINER(iceplayer.hpaned),iceplayer.back_eventbox_widget_lrc_hpaned);
        lrc_line_number_to_show = ((gdouble)iceplayer.list_songs_scrolledwindow->allocation.height-50) / (gdouble)widget_lrc_word_height/2.0 ;
        if (configure.show_page!=SWITCH_LRC) {
            return ;
        }
        break;
    case WIDGET_LRC_SHOW_TYPE_INDEPENDENT:
        //gtk_widget_set_usize(iceplayer.widget_lrc_label,configure.width-64, configure.height-iceplayer.main_fixed->allocation.height-iceplayer.switch_hbox->allocation.height-24);
        if (GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned)&&GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned->parent->parent))gtk_container_remove_new(GTK_CONTAINER(iceplayer.hpaned),iceplayer.back_eventbox_widget_lrc_hpaned);
        lrc_line_number_to_show = ((gdouble)configure.widget_lrc_Independent_height-50) / (gdouble)widget_lrc_word_height/2.0;

        break;
    }
    line_number = get_lrc_number();

    /*
        switch (configure.widget_lrc_show_type) {
        case WIDGET_LRC_SHOW_TYPE_HPANED:
            label = &iceplayer.widget_lrc_label_hpaned;
            break;
        case WIDGET_LRC_SHOW_TYPE_INTEGRATION:
            label = &iceplayer.widget_lrc_label;
            break;
        case WIDGET_LRC_SHOW_TYPE_INDEPENDENT:
            label = &widget_lrc.label;
            break;
        }*/
    if (lrc_text[0][0]) {
        if (strcmp(lrc_text[0],_("[00:00.00]iceplayer 音乐播放器(black list lrc)"))==0) {
            strcpy(cen_lrc,_("iceplayer 音乐播放器(black list lrc)"));
            gint n=0;
            while (2*n<=lrc_line_number_to_show) {
                up_lrc[n][0]=0;
                down_lrc[n++][0]=0;
            }
            widget_lrc_lock=TRUE;
            if (configure.widget_lrc_show_type==WIDGET_LRC_SHOW_TYPE_INTEGRATION || configure.widget_lrc_show_type ==  WIDGET_LRC_SHOW_TYPE_HPANED)
                gtk_widget_queue_draw(iceplayer.window);
            //         on_iceplayer_expose();
            else {
                if (GTK_IS_WIDGET(widget_lrc.window))gtk_widget_queue_draw(widget_lrc.window);
            }
            return;
        }
        /* if (!force) {
               static gint oo=0;
               if (line_number<oo)
                   oo = line_number;
               if (line_number==0&&oo>=0) {
                   oo=-5;
               } else {
                   if (line_number==0&&oo<0) {
                       if (oo==-1)
                           return ;
                       oo++;
                   } else {
                       if (line_number>0&&oo==line_number) {
                           return ;
                       } else {
                           oo = line_number;
                       }
                   }
               }
           }*/

        //inline gboolean set_label()
        //{8
        gint n=0,u=0,d=0;
        //gint m=0;
        up_lrc[0][0]=0;
        n= line_number - lrc_line_number_to_show-1 ;
//g_print("%d_%d\n",line_number, lrc_line_number_to_show );
//g_print("\n\n\n\n\n\n",n);
        while (n<line_number) {

            //  if (lrc_text[n][0]=='['){
            if (n>=0) {
                strcpy(up_lrc[u++],lrc_text_get_text(lrc_text[n]));
            } else {
                strcpy(up_lrc[u++],"");
            }
            //g_print("%d__%s\n",n,up_lrc[u]);
            n++;
        }
        up_lrc[u][0]=0;
//g_print("__n=%d_line_number + lrc_line_number_to_show=%d__u=%d_\n",n,line_number + lrc_line_number_to_show,u);

        cen_lrc[0]=0;
        // if (lrc_text[n][0]=='[')
        strcpy(cen_lrc,lrc_text_get_text(lrc_text[n]));
        //   else
        // strcpy(cen_lrc,"");
        n++;

        down_lrc[0][0]=0;
        while (n<=line_number + lrc_line_number_to_show ) {
            if (lrc_text[n][0]=='[')
                strcpy(down_lrc[d],lrc_text_get_text(lrc_text[n]));
            else
                strcpy(down_lrc[d],"");
            n++;
            d++;
        }
        down_lrc[d][0]=0;

        widget_lrc_lock=TRUE;


        if (configure.widget_lrc_show_type==WIDGET_LRC_SHOW_TYPE_INTEGRATION || configure.widget_lrc_show_type ==  WIDGET_LRC_SHOW_TYPE_HPANED)
            gtk_widget_queue_draw(iceplayer.window);
        //         on_iceplayer_expose();
        else {
            if (GTK_IS_WIDGET(widget_lrc.window))gtk_widget_queue_draw(widget_lrc.window);
        }
        //widget_lrc_show();
        /*
                sprintf(markup,"<span  foreground='%s'>%s</span><span  foreground='%s'>%s</span><span  foreground='%s'>%s</span>",configure.widget_lrc_back_color,up_lrc,configure.widget_lrc_front_color,cen_lrc,configure.widget_lrc_back_color,down_lrc);
                //gtk_label_set_markup ((GtkLabel  *)(*label),markup) ;
                //gtk_label_set_label_internal ((GtkLabel  *)(*label), markup);
                // g_free_n ((GtkLabel  *)(*label)->label);
        //sprintf(markup,"%s%s%s",up_lrc,cen_lrc,down_lrc);
                gtk_label_set_markup ((GtkLabel  *)(*label),markup) ;
                //g_print("%s\n",markup);
                ///pango_layout_set_text (((GtkLabel  *)(*label))->layout,markup,-1);

                //pango_attr_list_ref (((GtkLabel  *)(*label))->attrs);
                //((GtkLabel  *)(*label))->effective_attrs = ((GtkLabel  *)(*label))->attrs;
                //gtk_label_compose_effective_attrs(*label);
                ///gtk_widget_queue_resize (GTK_WIDGET (*label));*/

        if (GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned))gtk_label_set_text(GTK_LABEL(iceplayer.widget_lrc_label_hpaned),"");
        if (GTK_IS_WIDGET(iceplayer.widget_lrc_label))gtk_label_set_text(GTK_LABEL(iceplayer.widget_lrc_label),"");
        if (GTK_IS_WIDGET(widget_lrc.label))gtk_label_set_text(GTK_LABEL(widget_lrc.label),"");

        return;

        /*  static gchar markup[10000];
          gint n=0;
          while(lrc_text[n][0] )
            {
              strcat(markup,lrc_text_get_text(lrc_text[n]));
              strcat(markup,"\n");
              n++;
            }
          gtk_label_set_text ((GtkLabel  *)(*label),markup) ;
        }*/
        //inline gboolean set_mark(){
        //g_thread_create((void *)set_label,NULL,FALSE,NULL);
        //    return FALSE;
        //}

        //g_timeout_add(20,set_mark,NULL);
    } else {

        if (configure.search_lrc) {
            switch (lrc_search_data.search_lrc_state) {
            case SEARCH_LRC_STATE_NONE:
                strcpy(cen_lrc,_("iceplayer 音乐播放器") );
                break;
            case SEARCH_LRC_STATE_SEARCH:
                strcpy(cen_lrc, _("搜索歌词.."));
                break;
            case SEARCH_LRC_STATE_NET_SOCKET:
                strcpy(cen_lrc, _("Soctet.."));
                break;
            case SEARCH_LRC_STATE_NET_DNS:
                strcpy(cen_lrc,_("解析DNS..") );
                break;
            case SEARCH_LRC_STATE_NET_CONNECT:
                strcpy(cen_lrc, _("连接服务器.."));
                break;
            case SEARCH_LRC_STATE_NET_SOCKET_ERROR:
                strcpy(cen_lrc,_("Soctet失败！网络连接？速度过慢？") );
                break;
            case SEARCH_LRC_STATE_NET_DNS_ERROR:
                strcpy(cen_lrc,  _("解析DNS失败！网络连接？速度过慢？"));
                break;
            case SEARCH_LRC_STATE_NET_CONNECT_ERROR:
                strcpy(cen_lrc, _("连接失败！www.lrc123.com无法连接？") );
                break;
            case SEARCH_LRC_STATE_SEARCHED:
                strcpy(cen_lrc, _("搜索成功..") );
                break;
            case  SEARCH_LRC_STATE_NOT_FOUND:
                strcpy(cen_lrc,_("没有结果") );
                break;
            case SEARCH_LRC_STATE_DOWNLOADING:
                strcpy(cen_lrc,_("下载歌词(窗体会暂时无反应)..") );
                break;
            case SEARCH_LRC_STATE_DOWNLOADED:
                strcpy(cen_lrc, _("下载成功.."));
                break;
            case SEARCH_LRC_STATE_CLOSE:
                strcpy(cen_lrc,_("本地无匹配歌词！")  );
                break;
            case SEARCH_LRC_STATE_FAILED:
                strcpy(cen_lrc, _("下载失败..可能是下载的歌词文件内容不正确或者错误"));
                break;
            }

        } else {
            strcpy(cen_lrc,_("本地无匹配歌词！搜索已关闭。") );
        }
        /*
                    gboolean hide_te(){

                    gchar path[STRINGS_LENGTH];
                    strcpy(path,get_lrc_path(focus_song));
                    if (strcmp(path,"NULL")) {
                        strcpy(searching_lrc,"");

                        if (lrc_file_to_lrc_text(path)==FALSE) {
                            lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_FAILED;
                        }
                        widget_lrc_init();
                    }else{
                         lrc_search_data.search_lrc_state =SEARCH_LRC_STATE_NONE;
                    }

                        return FALSE;
                    }

                    g_timeout_add(5000,hide_te,NULL);*/
        /*
                if (GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned))gtk_label_set_text(iceplayer.widget_lrc_label_hpaned,te);
                if (GTK_IS_WIDGET(iceplayer.widget_lrc_label))gtk_label_set_text(iceplayer.widget_lrc_label,te);
                if (GTK_IS_WIDGET(widget_lrc.label))gtk_label_set_text(widget_lrc.label,te);*/

        gint n=0;
        while (n<=lrc_line_number_to_show) {
            up_lrc[n][0]=0;
            down_lrc[n++][0]=0;
        }


        widget_lrc_lock=TRUE;
        if (configure.widget_lrc_show_type==WIDGET_LRC_SHOW_TYPE_INTEGRATION || configure.widget_lrc_show_type ==  WIDGET_LRC_SHOW_TYPE_HPANED)
            gtk_widget_queue_draw(iceplayer.window);
        //         on_iceplayer_expose();
        else {
            if (GTK_IS_WIDGET(widget_lrc.window))gtk_widget_queue_draw(widget_lrc.window);
        }
    }

}
void widget_lrc_init() {
    print_programming("widget_lrc_init\n");
    widget_lrc_refresh(TRUE);
}

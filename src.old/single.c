/*
*
*
*
*
*    信号的反馈与实现
*    信号的建立在ui.c的ui_init()的下面
*
*
*
*
*/

#include "about.h"
#include "core.h"
#include "keybinding.h"
#include "widget_lrc.h"
#include "single.h"
#include "id3.h"
#include "list.h"
#include "ui.h"
#include "configure.h"
#include "instance.h"
#include "lrc.h"
#include "common.h"
#include "equalizer.h"

gboolean in_add_dialog;
gint switch_focus;
gboolean in_some_wm_that_cant_rec_window_state_event=FALSE;
cairo_surface_t *iceplayer_image = NULL;

///some func:
gchar * get_iceplayer_back_pic() {
    print_programming(("get_iceplayer_back_pic\n"));

    static gboolean lock=FALSE;
    if (lock==TRUE)
        return "/tmp/iceplayer_main.png";
    lock=TRUE;

    GdkPixbuf *pixbuf;

    gchar *dir= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    if (configure.back_image[0]!='\0') {
        FILE *file = fopen(configure.back_image,"r");
        if (file) {
            fclose(file);
            strcpy(dir,configure.back_image);
        } else {
            strcpy(dir,get_skin_dir());
            strcat(dir,"main.png");
        }
    } else {
        strcpy(dir,get_skin_dir());
        strcat(dir,"main.png");
    }
    pixbuf = gdk_pixbuf_new_from_file(dir,NULL);
    g_free_n(dir);

    if (gdk_screen_is_composited(gdk_screen_get_default ())) {
        pixbuf = gdk_pixbuf_set_alpha(pixbuf,configure.window_alpha);
        gdk_pixbuf_save (pixbuf, "/tmp/iceplayer_main.png", "png", NULL, NULL);
    } else {
        gdk_pixbuf_save (pixbuf, "/tmp/iceplayer_main.png", "png", NULL, NULL);
    }
    g_object_unref(pixbuf);

    lock=FALSE;


    return "/tmp/iceplayer_main.png";
    /*
    static gchar dir[STRINGS_LENGTH];
    strcpy(dir,get_skin_dir());
    strcat(dir,"main.png");
    return dir;*/
}
/*
*反应iceplayer主窗口的expose信号，主要是画背景图片
*支持半透明
*/
gboolean on_iceplayer_expose() {
    print_programming(("on_iceplayer_expose\n"));

    //因为在启动读配置的时候，会执行一次这个，那时窗口还没建立，所以要排除这两个情况
    if (!GDK_IS_DRAWABLE(iceplayer.window->window))
        return FALSE;
    if (iceplayer_image==NULL)
        return FALSE;
    cairo_t *cr;
    cr = gdk_cairo_create(iceplayer.eventbox->window);
    /*
    缩放
    */
    gint height = cairo_image_surface_get_height(iceplayer_image);
    gint width = cairo_image_surface_get_width(iceplayer_image);

    if ((gdouble)configure.width/(gdouble)configure.height*(gdouble)height<(gdouble)width) {
        cairo_scale(cr,(gdouble)configure.height/(gdouble)height,(gdouble)configure.height/(gdouble)height);
    } else {
        cairo_scale(cr,(gdouble)configure.width/(gdouble)width,(gdouble)configure.width/(gdouble)width);
    }
    /*
    缩放
    */
    cairo_set_source_surface(cr, iceplayer_image, 0, 0);

    cairo_set_operator(cr, CAIRO_OPERATOR_SOURCE);

    cairo_paint(cr);
    cairo_destroy(cr);
//窗口歌词集成和独占模式
    //if (configure.widget_lrc_show_type==WIDGET_LRC_SHOW_TYPE_INTEGRATION || configure.widget_lrc_show_type==  WIDGET_LRC_SHOW_TYPE_HPANED)on_iceplayer_widget_lrc_expose();
//if(G_IS_OBJECT(cr))g_object_unref(cr);

    if (configure.widget_lrc_show_type==WIDGET_LRC_SHOW_TYPE_INTEGRATION || configure.widget_lrc_show_type==  WIDGET_LRC_SHOW_TYPE_HPANED)
        widget_lrc_show();
    return FALSE;
}
/*
*由图片和eventbox组成的按钮，其图片变化的信号分发函数
*type决定了其是哪一个按钮，比如CONTROL_STOP就是停止按钮的
*/
void change_picture_on_press_and_release_event(GdkEventButton * event,gint type) {
    print_programming(("change_picture_on_press_and_release_event\n"));
    if (event == NULL) {
        change_picture(NULL,(GdkEventCrossing *)0,type);
        return;
    }
    if (event->type == GDK_BUTTON_PRESS)
        change_picture(NULL,(GdkEventCrossing *)0,type);
    if (event->type == GDK_BUTTON_RELEASE)
        change_picture(NULL,(GdkEventCrossing *)1,type);
    return ;
}
//调整皮肤边界
gboolean fit_size(){
    //记录窗口大小位置
    static gint width,height;

    gtk_window_get_size(GTK_WINDOW(iceplayer.window),&configure.width,&configure.height);
    gtk_window_get_position(GTK_WINDOW(iceplayer.window),&configure.x,&configure.y);

    if (width==configure.width&&height==configure.height)
        return FALSE;

    width= configure.width  ;
    height =configure.height;

    gchar *dir = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(dir,get_skin_dir());

    gtk_widget_set_usize(iceplayer.main_fixed,width-4,skin.main_height);
    gtk_widget_set_size_request (iceplayer.progress_hbox, width-40, -1);
    if (GTK_IS_WIDGET(iceplayer.widget_lrc_label))gtk_widget_set_size_request (iceplayer.widget_lrc_label, 1, 1);
    if (GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned))gtk_widget_set_size_request (iceplayer.widget_lrc_label_hpaned, 1, 1);
    gtk_widget_set_usize(iceplayer.back_eventbox_main,width-4,skin.main_height);

    gtk_container_remove_new((GtkContainer *)iceplayer.resize_left_eventbox,iceplayer.resize_left_image);
    gtk_container_remove_new((GtkContainer *)iceplayer.resize_right_eventbox,iceplayer.resize_right_image);
    gtk_container_remove_new((GtkContainer *)iceplayer.resize_buttom_eventbox,iceplayer.resize_buttom_image);
    gtk_container_remove_new((GtkContainer *)iceplayer.resize_top_eventbox,iceplayer.resize_top_image);

    GdkPixbuf *pixbuf1=NULL,*pixbuf2=NULL;

    pixbuf1 = gdk_pixbuf_new_from_file_new(dir,"buttom");
    pixbuf2 = gdk_pixbuf_scale_simple(pixbuf1,width-4,2,GDK_INTERP_HYPER);
    iceplayer.resize_buttom_image=gtk_image_new_from_pixbuf(pixbuf2);
    g_object_unref(pixbuf1);
    g_object_unref(pixbuf2);

    pixbuf1 = gdk_pixbuf_new_from_file_new(dir,"left");
    pixbuf2 = gdk_pixbuf_scale_simple(pixbuf1,2,height-4,GDK_INTERP_HYPER);
    iceplayer.resize_left_image=gtk_image_new_from_pixbuf(pixbuf2);
    g_object_unref(pixbuf1);
    g_object_unref(pixbuf2);

    pixbuf1 = gdk_pixbuf_new_from_file_new(dir,"right");
    pixbuf2 = gdk_pixbuf_scale_simple(pixbuf1,2,height-4,GDK_INTERP_HYPER);
    iceplayer.resize_right_image=gtk_image_new_from_pixbuf(pixbuf2);
    g_object_unref(pixbuf1);
    g_object_unref(pixbuf2);

    pixbuf1 = gdk_pixbuf_new_from_file_new(dir,"top");
    pixbuf2 = gdk_pixbuf_scale_simple(pixbuf1,width-4,2,GDK_INTERP_HYPER);
    iceplayer.resize_top_image=gtk_image_new_from_pixbuf(pixbuf2);
    g_object_unref(pixbuf1);
    g_object_unref(pixbuf2);

    gtk_widget_show(iceplayer.resize_buttom_image);
    gtk_widget_show(iceplayer.resize_right_image);
    gtk_widget_show(iceplayer.resize_left_image);
    gtk_widget_show(iceplayer.resize_top_image);
    gtk_container_add((GtkContainer *)iceplayer.resize_left_eventbox,iceplayer.resize_left_image);
    gtk_container_add((GtkContainer *)iceplayer.resize_right_eventbox,iceplayer.resize_right_image);
    gtk_container_add((GtkContainer *)iceplayer.resize_buttom_eventbox,iceplayer.resize_buttom_image);
    gtk_container_add((GtkContainer *)iceplayer.resize_top_eventbox,iceplayer.resize_top_image);

    g_free_n(dir);

    return TRUE;
}
/*
*当主窗口的大小有变化的时候就会调用这个函数
*主要是记录窗口大小、调整窗口的一些皮肤的大小、位置
*/
gboolean size_changed() {
    print_programming("size_changed\n");

    set_skin_configure_to_iceplayer_widget();

    if(!fit_size())//调整皮肤元素
        return FALSE;

    configure.list_hpaned_pos = gtk_paned_get_position(GTK_PANED(iceplayer.list_hpaned));
    configure.hpaned_pos = gtk_paned_get_position(GTK_PANED(iceplayer.hpaned));

    updata_iceplayer_bg();
    return FALSE;
}
/*

切换显卡混合效果的信号

*/
gboolean change_com() {
    print_programming(("change_com\n"));

    iceplayer_image = cairo_image_surface_create_from_png_new(iceplayer_image,get_iceplayer_back_pic());
    if (configure.widget_lrc_show_type!=WIDGET_LRC_SHOW_TYPE_INTEGRATION && configure.widget_lrc_show_type!=  WIDGET_LRC_SHOW_TYPE_HPANED)
        iceplayer_widget_lrc_image = cairo_image_surface_create_from_png_new(iceplayer_widget_lrc_image,get_iceplayer_widget_lrc_back_pic());
    gtk_widget_queue_draw(iceplayer.window);
    {
        if (GTK_IS_WIDGET(widget_lrc.window))gtk_widget_queue_draw(widget_lrc.window);
    }
    return FALSE;
}
/*
*添加文件到列表里面，files是个链表结构，里面的data是歌曲的路径，next暂时不用
*lists_name是需要添加进去的列表名，如果找不到这个名字对应的列表，就新建一个
*/
void add_file(GSList *files,gchar lists_name[STRINGS_LENGTH]) {
    print_programming(" add_file\n");
    //寻找对应的列表
    gchar *uri = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    iceplayer_lists *lists = find_lists_from_name(lists_name);
    if (!lists||!lists->name[0]) {
        if (!lists_name[0])
            strcpy(lists_name,"新建列表");
        strcpy(lists_name,add_new_lists(lists_name));
        lists = find_lists_from_name(lists_name);
        strcpy(focus_lists,lists_name);
    }
    iceplayer_songs *songs_tmp = lists->songs;

    //让songs_tmp指向列表最后一项，并且是空值。
    if (!songs_tmp) {
        lists->songs = (iceplayer_songs *)malloc(sizeof(iceplayer_songs));
        lists->songs->next = NULL;
        lists->songs->uri[0]=0;
        songs_tmp = lists->songs;
    } else {
        if (!songs_tmp->uri[0]) {
            lists->songs = (iceplayer_songs *)malloc(sizeof(iceplayer_songs));
            lists->songs->next = NULL;
            lists->songs->uri[0]=0;
            songs_tmp = lists->songs;
        } else {
            while (songs_tmp&&songs_tmp->next&&songs_tmp->uri[0]) {
                if (songs_tmp->next==NULL)break;
                songs_tmp = songs_tmp->next;
            }
            songs_tmp->next= (iceplayer_songs *)malloc(sizeof(iceplayer_songs));
            songs_tmp->next->next= NULL;
            songs_tmp->next->uri[0]=0;
            songs_tmp=songs_tmp->next;
        }
    }
    //开始添加
    while (files) {
        strcpy(uri,files->data);
//cue:
        if (strstr(uri,".cue")!=NULL) {
            gchar main_performer[STRINGS_LENGTH];
            gchar cue_text[STRINGS_LENGTH];
            gchar wave_file[STRINGS_LENGTH];
            gchar track[100][STRINGS_LENGTH];
            gchar title[100][STRINGS_LENGTH];
            gchar performer[100][STRINGS_LENGTH];
            gchar index[100][STRINGS_LENGTH];

            gint num=0;
            gint num2=0;
            FILE *file=fopen(uri,"r");
            while (fgets(cue_text,STRINGS_LENGTH,file)) {
                if (!g_utf8_validate(cue_text,-1,NULL)) {
                    gsize r,w;
                    gchar *str = g_convert(cue_text,-1,"UTF-8","GBK",&r,&w,NULL);
                    if (str)  strcpy(cue_text,str);
                }
                gint n =0;
                while (cue_text[n]) {
                    if (cue_text[n]=='"')
                        cue_text[n]=' ';
                    if (cue_text[n]=='/')
                        cue_text[n]='&';
                    n++;
                }
                //解析cue参数
                if (strstr(cue_text,"FILE")!=NULL) {
                    strcpy( wave_file,"");
                    sscanf(cue_text,"%*[^ ]%s%*[^'\"']",wave_file);
                }
                if (strstr(cue_text,"  TRACK")!=NULL) {
                    strcpy( track[num],"");
                    sscanf(strstr(cue_text,"TRACK"),"%*[^ ]%s%*s",track[num]);
                }
                if (strstr(cue_text,"    TITLE")!=NULL) {
                    strcpy( title[num],"");
                    sscanf(strstr(cue_text,"TITLE"),"%*s%*[ ]%[^'\"']%*s",title[num]);
                    gint nj = strlen(title[num])-1;
                    while (title[num][nj]==' '||title[num][nj]=='\r'||title[num][nj]=='\n') {
                        title[num][nj]=0;
                        nj--;
                    }
                }
                if (strstr(cue_text,"PERFORMER")!=NULL) {
                    strcpy(main_performer,"");
                    sscanf(cue_text,"%*s%*[ ]%[^'\"']%*s",main_performer);
                    gint nj = strlen(main_performer)-1;
                    while (main_performer[nj]==' '||main_performer[nj]=='\r'||main_performer[nj]=='\n') {
                        main_performer[nj]=0;
                        nj--;
                    }
                }
                if (strstr(cue_text,"    PERFORMER")!=NULL) {
                    strcpy( performer[num2],"");
                    sscanf(strstr(cue_text,"PERFORMER"),"%*s%*[ ]%[^'\"']%*[ ]",performer[num2++]);
                    gint nj = strlen(performer[num2-1])-1;
                    while (performer[num2-1][nj]==' '||performer[num2-1][nj]=='\r'||performer[num2-1][nj]=='\n') {
                        performer[num2-1][nj]=0;
                        nj--;
                    }
                }
                if (strstr(cue_text,"    INDEX 01")!=NULL) {
                    strcpy( index[num],"");
                    sscanf(cue_text,"    INDEX 01 %s",index[num++]);
                }
            }
            gint n=0;
            while (n<num) {
                if (n==num-1) {
                    sprintf(songs_tmp->uri,"<cue><FILE:%s><TRACK:%s><TITLE:%s><PERFORMER:%s><INDEX_A:%s><INDEX_B:NONE><cue_uri:%s>",
                            wave_file,track[n],title[n],num2==num?performer[n]:main_performer,index[n],uri);
                } else {
                    sprintf(songs_tmp->uri,"<cue><FILE:%s><TRACK:%s><TITLE:%s><PERFORMER:%s><INDEX_A:%s><INDEX_B:%s><cue_uri:%s>",
                            wave_file,track[n],title[n],num2==num?performer[n]:main_performer,index[n],index[n+1],uri);
                }
                gint nj =0;
                while (songs_tmp->uri[nj]) {
                    if (songs_tmp->uri[nj]=='\n'||songs_tmp->uri[nj]=='\r')
                        songs_tmp->uri[nj]=' ';
                    nj++;
                }
                if (n+1<num) {
                    songs_tmp->next = (iceplayer_songs *)malloc(sizeof(iceplayer_songs));
                    songs_tmp->next->next=NULL;
                    songs_tmp->next->uri[0]=0;
                    songs_tmp = songs_tmp->next;
                }
                n++;
            }
        } else {
            //非cue文件
            strcpy(songs_tmp->uri,uri);
        }
        //一些收尾工作
        if (files->next) {
            songs_tmp->next= (iceplayer_songs *)malloc(sizeof(iceplayer_songs));
            songs_tmp->next->next= NULL;
            songs_tmp->next->uri[0]=0;
            songs_tmp=songs_tmp->next;
        }
        //songs_tmp->next = NULL;
        files = files->next;
        continue ;
    }
    g_free_n(uri);
    return ;
}
/*
*添加文件夹，lists_name是要添加的列表，path是添加的文件夹的路径，
*cycle如果为真就是循环递归子目录，new_lists就是是否以目录名新建列表
*然后调用add_file（上面那个）进行添加入列表中。
*/
void add_folder(gchar lists_name[STRINGS_LENGTH],gchar path[STRINGS_LENGTH],gboolean cycle,gboolean new_lists) {
    print_programming("add_folder\n");
    if (!find_lists_from_name(lists_name))strcpy(lists_name,add_new_lists(lists_name));

    DIR *dir=opendir(path);
    struct dirent *ptr=readdir(dir);
    while (ptr) {
        gchar path2[2000];
        strcpy(path2,path);
        strcat(path2,"/");
        strcat(path2,ptr->d_name);
        DIR * dir2 = opendir(path2);
        if (dir2) {
            closedir(dir2);
            gchar lists_name2[300];
            strcpy(lists_name2,lists_name);
            if (cycle) {
                ///"遇到目录，循环子目录"
                if (path2[strlen(path2)-1]!='.') {
                    if (new_lists) {
                        ///"遇到目录，根据设置，将目录名作为新建列表"
                        gchar name_for_check[310];
                        gint n=2;
                        strcpy(name_for_check,ptr->d_name);
                        while (find_lists_from_name(name_for_check))sprintf(name_for_check,"%s_%d",ptr->d_name,n++);
                        strcpy(lists_name2,name_for_check);
                    } else {
                        ///"遇到目录，根据设置，将目录名不作为新建列表"
                        if (focus_lists[0])
                            strcpy(lists_name2,focus_lists);
                        else {
                            if (main_lists.name[0])
                                strcpy(lists_name2,main_lists.name);
                            else {
                                gchar dir_name[200];
                                if (strlen(path)!=1) {
                                    gint n=strlen(path);
                                    gint s=0;
                                    while (path[n]!='/')n--;
                                    n++;
                                    while (path[n])dir_name[s++] = path[n++];
                                    dir_name[s] = 0;
                                } else
                                    strcpy(dir_name,"/");
                                strcpy(lists_name2,dir_name);
                            }
                        }
                    }
                    add_folder(lists_name2,path2,cycle,new_lists);
                }
                ptr=readdir(dir);
                continue ;
            }
        } else {
            if (path2[strlen(path2)-1]!='.') {
                if (!strstr(ptr->d_name,".part"))
                    if (was_music_file(ptr->d_name)) {
                        GSList *files=(GSList *)malloc(sizeof(GSList));
                        files->data=path2;
                        files->next = NULL;
                        add_file(files, lists_name);
                    }
            }
        }
        ptr=readdir(dir);
    }
    closedir(dir);
    return ;
}
/*
*检测是否是iceplayer支持的格式，是，返回TRUE
*/
gboolean was_music_file(gchar *d_name) {
    print_programming(("was_music_file\n"));
    if (strstr(d_name,".")&&( strstr(d_name,".MP3")||strstr(d_name,".MID")||strstr(d_name,".DTS")||strstr(d_name,".WMA")||strstr(d_name,".OGG")||strstr(d_name,".WAV")||strstr(d_name,".APE")||strstr(d_name,".FLAC")||strstr(d_name,".ACC")||strstr(d_name,".OGV")||strstr(d_name,".WMV")||strstr(d_name,".DAT")||strstr(d_name,".MKV")||strstr(d_name,".MPG")||strstr(d_name,".MP4")||strstr(d_name,".RM")||strstr(d_name,".RMVB")||strstr(d_name,".AVI")||strstr(d_name,".FLV")||strstr(d_name,".CUE")||strstr(d_name,".M4A")||strstr(d_name,".M3U")||strstr(d_name,".ASX")||strstr(d_name,".RAM")||strstr(d_name,".AAC")||strstr(d_name,".mp3")||strstr(d_name,".mid")||strstr(d_name,".dts")||strstr(d_name,".wma")||strstr(d_name,".ogg")||strstr(d_name,".wav")||strstr(d_name,".ape")||strstr(d_name,".flac")||strstr(d_name,".acc")||strstr(d_name,".ogv")||strstr(d_name,".wmv")||strstr(d_name,".dat")||strstr(d_name,".mkv")||strstr(d_name,".mpg")||strstr(d_name,".mp4")||strstr(d_name,".rm")||strstr(d_name,".rmvb")||strstr(d_name,".avi")||strstr(d_name,".flv")||strstr(d_name,".cue")||strstr(d_name,".m4a")||strstr(d_name,".m3u")||strstr(d_name,".asx")||strstr(d_name,".ram")||strstr(d_name,".aac")))
        return TRUE;
    else
        return FALSE;
}
/*
*添加歌曲的对话框,type决定是添加文件夹还是文件
*/
void add_dialog(GtkWidget *widget,gint type) {
    print_programming("add_dialog\n");
    GtkWidget *filechooserdialog;
    GtkWidget *dialog_vbox;
    GtkWidget *hbox1;
    GtkWidget *dialog_action_area;
    GtkWidget *button1;
    GtkWidget *button2;
    GtkWidget *checkbutton=NULL;
    GtkWidget *checkbutton2=NULL;
    if (type == ADD_FILES) {
        filechooserdialog = gtk_file_chooser_dialog_new("", NULL,GTK_FILE_CHOOSER_ACTION_OPEN, NULL,NULL);
        g_object_set(filechooserdialog,"select-multiple", TRUE,NULL);
    } else
        filechooserdialog = gtk_file_chooser_dialog_new("", NULL,GTK_FILE_CHOOSER_ACTION_SELECT_FOLDER, NULL,NULL);
    gtk_container_set_border_width(GTK_CONTAINER(filechooserdialog), 5);
    gtk_window_set_position(GTK_WINDOW(filechooserdialog), GTK_WIN_POS_CENTER);
    gtk_window_set_type_hint(GTK_WINDOW(filechooserdialog), GDK_WINDOW_TYPE_HINT_DIALOG);
    GdkPixbuf *IcePlayer_icon_pixbuf=gdk_pixbuf_new_from_file(ICON, NULL);
    gtk_window_set_icon(GTK_WINDOW(filechooserdialog),(GdkPixbuf *)IcePlayer_icon_pixbuf);
    gtk_window_set_keep_above     (GTK_WINDOW (filechooserdialog),TRUE);
    dialog_vbox = GTK_DIALOG(filechooserdialog)->vbox;
    gtk_widget_show(dialog_vbox);
    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox1);
    gtk_box_pack_start(GTK_BOX(dialog_vbox), hbox1, FALSE, FALSE, 0);
    if (type == ADD_FOLDER) {
        checkbutton = gtk_check_button_new_with_mnemonic (_("包含所有子目录"));
        gtk_widget_show(checkbutton);
        gtk_toggle_button_set_active((GtkToggleButton *)checkbutton,FALSE);
        gtk_box_pack_start(GTK_BOX(hbox1), checkbutton, FALSE, FALSE, 0);

        checkbutton2 = gtk_check_button_new_with_mnemonic (_("新建播放列表(按目录名)"));
        gtk_widget_show(checkbutton2);
        gtk_toggle_button_set_active((GtkToggleButton *)checkbutton2,FALSE);
        gtk_box_pack_start(GTK_BOX(hbox1), checkbutton2, FALSE, FALSE, 0);
    }
    dialog_action_area = GTK_DIALOG(filechooserdialog)->action_area;
    gtk_widget_show(dialog_action_area);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area), GTK_BUTTONBOX_END);
    button1 = gtk_button_new_from_stock("gtk-cancel");
    gtk_widget_show(button1);
    gtk_dialog_add_action_widget(GTK_DIALOG(filechooserdialog), button1, GTK_RESPONSE_CANCEL);
    GTK_WIDGET_SET_FLAGS(button1, GTK_CAN_DEFAULT);
    button2 = gtk_button_new_from_stock("gtk-open");
    gtk_widget_show(button2);
    gtk_dialog_add_action_widget(GTK_DIALOG(filechooserdialog), button2, GTK_RESPONSE_OK);
    GTK_WIDGET_SET_FLAGS(button2, GTK_CAN_DEFAULT);
    gtk_widget_grab_default(button2);
    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, _("媒体文件"));
    gtk_file_filter_add_pattern(filter,"*.[Mm][Pp][3]");
    gtk_file_filter_add_pattern(filter,"*.[Dd][Tt][Ss]");
    gtk_file_filter_add_pattern(filter,"*.[Ww][Mm][Aa]");
    gtk_file_filter_add_pattern(filter,"*.[Oo][Gg][Gg]");
    gtk_file_filter_add_pattern(filter,"*.[Ww][Aa][Vv]");
    gtk_file_filter_add_pattern(filter,"*.[Aa][Pp][Ee]");
    gtk_file_filter_add_pattern(filter,"*.[Ff][Ll][Aa][Cc]");
    gtk_file_filter_add_pattern(filter,"*.[AA][Cc][Cc]");
    gtk_file_filter_add_pattern(filter,"*.[Oo][Gg][Vv]");
    gtk_file_filter_add_pattern(filter,"*.[Ww][Mm][Vv]");
    gtk_file_filter_add_pattern(filter,"*.[Dd][Aa][Tt]");
    gtk_file_filter_add_pattern(filter,"*.[Mm][Kk][Vv]");
    gtk_file_filter_add_pattern(filter,"*.[Mm][Pp][Gg]");
    gtk_file_filter_add_pattern(filter,"*.[Mm][Pp][4]");
    gtk_file_filter_add_pattern(filter,"*.[Rr][Mm]");
    gtk_file_filter_add_pattern(filter,"*.[Rr][Mm][Vv][Bb]");
    gtk_file_filter_add_pattern(filter,"*.[Aa][Vv][Ii]");
    gtk_file_filter_add_pattern(filter,"*.[Ff][Ll][Vv]");
    gtk_file_filter_add_pattern(filter,"*.[Cc][Uu][Ee]");
    gtk_file_filter_add_pattern(filter,"*.[Mm][3][Uu]");
    gtk_file_filter_add_pattern(filter,"*.[Mm][4][Aa]");
    gtk_file_filter_add_pattern(filter,"*.[Rr][Aa][Mm]");
    gtk_file_filter_add_pattern(filter,"*.[Aa][Ss][Xx]");
    gtk_file_filter_add_pattern(filter,"*.[Aa][Aa][Cc]");
    gtk_file_filter_add_pattern(filter,"*.[Mm][Ii][Dd]");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooserdialog),filter);

    static gchar * path_used;
    /*下次打开对话框时便回到上一文件夹*/
    if (path_used)gtk_file_chooser_set_current_folder((GtkFileChooser *) filechooserdialog,path_used);

    gtk_window_set_focus ((GtkWindow *)filechooserdialog,button2);
    gboolean cycle = FALSE;
    gboolean new_lists = FALSE;
    gchar lists_name[300];
    //gboolean has_playing= FALSE;
    in_add_dialog = TRUE;
    gint aw = gtk_dialog_run((GtkDialog *) filechooserdialog);

    path_used = gtk_file_chooser_get_current_folder((GtkFileChooser *) filechooserdialog);

    if (aw==-5) {
        gchar *path;
        path = gtk_file_chooser_get_filename((GtkFileChooser *) filechooserdialog);

        if (type == ADD_FOLDER) {
            if (gtk_toggle_button_get_active((GtkToggleButton *) checkbutton))
                cycle = TRUE;
            if (gtk_toggle_button_get_active((GtkToggleButton *) checkbutton2))
                new_lists = TRUE;
            if (new_lists) {
                ///*add_new_lists(a) 新建播放列表,名为a或a+1...
                gchar *dir_name=(gchar *)g_malloc(500*sizeof(gchar *));
                if (strlen(path)!=1) {
                    gint n=strlen(path);
                    gint s=0;
                    while (path[n]!='/')n--;
                    n++;
                    while (path[n])dir_name[s++] = path[n++];
                    dir_name[s] = 0;
                } else
                    strcpy(dir_name,"/");
                strcpy(lists_name,add_new_lists(dir_name));
                strcpy(focus_lists,lists_name);
                if(dir_name)g_free_n(dir_name);
            } else {
                if (focus_lists[0])
                    strcpy(lists_name,focus_lists);
                else {
                    if (main_lists.name[0])
                        strcpy(lists_name,main_lists.name);
                    else {
                        gchar dir_name[200];
                        if (strlen(path)!=1) {
                            gint n=strlen(path);
                            gint s=0;
                            while (path[n]!='/')n--;
                            n++;
                            while (path[n])dir_name[s++] = path[n++];
                            dir_name[s] = 0;
                        } else
                            strcpy(dir_name,"/");
                        strcpy(lists_name,dir_name);
                    }
                }
            }
            add_folder(lists_name,path,cycle,new_lists);
        } else {
            GSList *files = gtk_file_chooser_get_filenames((GtkFileChooser *) filechooserdialog);
            add_file(files,focus_lists);
        }
        if(path)g_free_n(path);
    }
    gtk_widget_destroy(filechooserdialog);
    in_add_dialog = FALSE;

    show_lists(NULL);
    show_songs(NULL);

    change_picture_on_press_and_release_event(NULL,CONTROL_OPEN);

    return ;
}
/*
*点击添加歌曲的按钮时弹出的菜单，
*/
void add_menu() {
    print_programming(("add_menu\n"));
    GtkWidget * menu = gtk_menu_new();
    GtkWidget *menu_Open_file = gtk_image_menu_item_new_with_label(_("添加文件"));
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM ( menu_Open_file),gtk_image_new_from_stock ("gtk-open",GTK_ICON_SIZE_MENU));

    gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_Open_file);
    GtkWidget *menu_Open_folder = gtk_image_menu_item_new_with_label(_("添加文件夹"));
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM ( menu_Open_folder),gtk_image_new_from_stock ("gtk-open",GTK_ICON_SIZE_MENU));

    gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_Open_folder);
    g_signal_connect(menu_Open_file ,"activate",G_CALLBACK(add_dialog),(gpointer)ADD_FILES);
    g_signal_connect(menu_Open_folder ,"activate",G_CALLBACK(add_dialog),(gpointer)ADD_FOLDER);
    gtk_widget_show_all(menu);
    popup_menu(GTK_MENU(menu),2,2);
}
/*
主窗口左上角那个菜单，其实就是托盘的菜单
*/
void menu() {
    print_programming(("menu\n"));
    tray_menu();
}
//更新 focus_lists
void update_focus_lists() {
    print_programming("update_focus_lists\n");
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *value=NULL;
    GList *list_select = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_lists_treeview))),&model);
    if (list_select) {
        if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath *) list_select->data)) {
            gtk_tree_model_get(model, &iter, 0, &value, -1);
            if (strcmp(focus_lists,value)) {
                show_lists(value);
                strcpy(focus_lists,value);
                show_songs(NULL);
            }
            if (value)g_free_n(value);
        }
    }
}
//更新lists的focus_song，就是焦点所在的列表的默认选上的那个歌曲，从而达到切换列表就能返回到之前点选上的那个歌曲的效果
void update_lists_focus_song() {
    print_programming("update_lists_focus_song\n");
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *value=NULL;
    GList *list_select = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_songs_treeview))),&model);
    if (list_select)
        if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath *) list_select->data)) {
            gtk_tree_model_get(model, &iter, 0, &value, -1);
            if (strcmp(find_lists_from_name(focus_lists)->focus_song,gtk_tree_path_to_string((GtkTreePath *) list_select->data)))
                strcpy(find_lists_from_name(focus_lists)->focus_song,gtk_tree_path_to_string((GtkTreePath *) list_select->data));
            if (value)g_free_n(value);
        }
}
//通过检查已保存的playing_song_tree_path更新focus_song
void update_focus_song_from_playing_song() {
    print_programming("update_focus_song_from_playing_song\n");
    iceplayer_songs *songs = find_lists_from_name(playing_list)->songs;
    if (!songs) {
        focus_song[0]=0;
        return ;
    }
    gint num = atoi(playing_song);
    while (num!=0&&songs&&songs->next&&songs->uri[0]) {
        songs = songs->next;
        num -- ;
    }
    if (!songs->uri[0]) {
        playing_song[0]='0';
        playing_song[1]=0;
        strcpy(songs->uri,find_lists_from_name(playing_list)->songs->uri);
    }
    show_songs(songs->uri);
    strcpy(focus_song,songs->uri);

    /*GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *value=NULL;
    model =  gtk_tree_view_get_model(GTK_TREE_VIEW(iceplayer.list_songs_treeview));
    if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath *) gtk_tree_path_new_from_string(playing_song))) {
        gtk_tree_model_get(model, &iter, 0, &value, -1);
        show_songs(value);
        strcpy(focus_song,value);
       // iceplayer_lists *lists = find_lists_from_name(focus_lists);
    }*/
}
//通过检查列表的选择项更新focus_song
void update_focus_song_from_selection() {
    print_programming("update_focus_song_from_selection\n");
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *value=NULL;
    GList *list_select = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_songs_treeview))),&model);
    if (list_select)
        if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath *) list_select->data)) {
            gtk_tree_model_get(model, &iter, 0, &value, -1);
            show_songs(value);
            strcpy(focus_song,value);
            if (value)g_free_n(value);
            iceplayer_lists *lists = find_lists_from_name(focus_lists);
            strcpy(lists->focus_song,gtk_tree_path_to_string((GtkTreePath *) list_select->data));

            strcpy(playing_song,gtk_tree_path_to_string((GtkTreePath *) list_select->data));
            strcpy(playing_list,focus_lists);
        }
    if (focus_song[0])
        if (core.pipeline) {
            core.state = GST_STATE_NULL;
            gst_element_set_state(GST_ELEMENT(core.pipeline), core.state);
        }

}
//返回被点选上的歌曲
gchar *get_selection_song() {
    print_programming("get_selection_song\n");
    if (!main_lists.name[0])return NULL;
    GtkTreeModel *model;
    GtkTreeIter iter;
    static gchar *value=NULL; //为了消警告所以是static
    GList *list_select = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_songs_treeview))),&model);
    if (list_select) {
        if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath *) list_select->data)) {
            gtk_tree_model_get(model, &iter, 0, &value, -1);
        }
    }
    return value;
}
//返回正在播放的歌曲
gchar *get_playing_song() {
    print_programming("get_playing_song\n");
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return NULL;
    }
    if (playing_song[0]==0) {
        print_error("没有在播放音乐！\n");
        return NULL;
    }
    if (focus_song[0]==0) {
        print_error("没有在播放音乐！\n");
        return NULL;
    }
    return focus_song;
}
//显示主窗口函数
gboolean show() {
    print_programming("show\n");
    showing=TRUE;
    //在一些窗口管理器里不能反馈窗口状态改变的信号
    if (in_some_wm_that_cant_rec_window_state_event) {
        gtk_widget_hide(iceplayer.window);
        gtk_widget_show(iceplayer.window);
        gtk_window_move(GTK_WINDOW(iceplayer.window),configure.x,configure.y);
        gtk_window_deiconify (GTK_WINDOW(iceplayer.window) );

        gboolean taskbar() {
            gtk_window_set_skip_taskbar_hint(GTK_WINDOW(iceplayer.window),FALSE);
            gtk_window_set_skip_taskbar_hint(GTK_WINDOW(iceplayer.window),TRUE);//任务栏
            return FALSE;
        }
        if (!configure.task_bar) g_timeout_add(10,taskbar,NULL);
    } else {
        //正常情况下：
        if ( windows_state != WINDOWS_STATE_SHOW&&windows_state != WINDOWS_STATE_SHOW_ALL_WORK_PLACES) {
            gtk_window_deiconify (GTK_WINDOW(iceplayer.window) );
        } else {
            gtk_window_deiconify (GTK_WINDOW(iceplayer.window) );

            gtk_widget_hide(iceplayer.window);
            gtk_widget_show(iceplayer.window);
            gtk_window_move(GTK_WINDOW(iceplayer.window),configure.x,configure.y);
            gboolean taskbar() {
                gtk_window_set_skip_taskbar_hint(GTK_WINDOW(iceplayer.window),FALSE);
                gtk_window_set_skip_taskbar_hint(GTK_WINDOW(iceplayer.window),TRUE);//任务栏
                return FALSE;
            }
            if (!configure.task_bar)g_timeout_add(10,taskbar,NULL);
        }
    }
//检测现在的窗口管理器是否能反馈窗口状态改变的信号
    inline gboolean check_show() {
        if (windows_state!=WINDOWS_STATE_SHOW||windows_state!=WINDOWS_STATE_SHOW_ALL_WORK_PLACES) {
            if (!configure.show_page)windows_state = WINDOWS_STATE_SHOW;
            else windows_state = WINDOWS_STATE_SHOW_ALL_WORK_PLACES;
            in_some_wm_that_cant_rec_window_state_event=TRUE;
        }
        return FALSE;
    }
    g_timeout_add(50,check_show,NULL);
    if (configure.widget_lrc_show_type==WIDGET_LRC_SHOW_TYPE_INDEPENDENT) {
        gboolean t=FALSE;
        gint type;
        if (configure.x<configure.widget_lrc_Independent_width+configure.widget_lrc_Independent_x+5&&configure.x>configure.widget_lrc_Independent_width+configure.widget_lrc_Independent_x-5) {
            t=TRUE;
            type=0;
        }
        if (configure.x+configure.width<configure.widget_lrc_Independent_x+5&&configure.x+configure.width>configure.widget_lrc_Independent_x-5) {
            t=TRUE;
            type=1;
        }
        if (configure.y<configure.widget_lrc_Independent_y + configure.widget_lrc_Independent_height+5&&configure.y>configure.widget_lrc_Independent_y + configure.widget_lrc_Independent_height-5) {
            t=TRUE;
            type=2;
        }
        if (configure.y+configure.height>configure.widget_lrc_Independent_y-5&&configure.y+configure.height<configure.widget_lrc_Independent_y+5) {
            t=TRUE;
            type=3;
        }
        if (t==TRUE) {
            gtk_widget_hide(widget_lrc.window);
            gtk_widget_show_all (widget_lrc.window);
            gtk_window_move(GTK_WINDOW(widget_lrc.window),
			    configure.widget_lrc_Independent_x,
			    configure.widget_lrc_Independent_y);
            gtk_window_deiconify (GTK_WINDOW(widget_lrc.window) );
        }
    }
    return FALSE;
}
//隐藏主窗口函数
gboolean hide(gpointer data) {
    print_programming("hide\n");
    showing=FALSE;
    gtk_window_iconify (GTK_WINDOW(iceplayer.window) );
    inline gboolean check_min() {
        if (windows_state!=WINDOWS_STATE_HIDE||windows_state!=WINDOWS_STATE_HIDE_ALL_WORK_PLACES) {
            if (!configure.show_page) windows_state = WINDOWS_STATE_HIDE;
            else windows_state = WINDOWS_STATE_HIDE_ALL_WORK_PLACES;
            in_some_wm_that_cant_rec_window_state_event=TRUE;
        }
        return FALSE;
    }
    inline gboolean taskbar() {
        gtk_window_set_skip_taskbar_hint(GTK_WINDOW(iceplayer.window),FALSE);
        gtk_window_set_skip_taskbar_hint(GTK_WINDOW(iceplayer.window),TRUE);
        return FALSE;
    }
    if (!configure.task_bar)g_timeout_add(10,taskbar,NULL);
    g_timeout_add(50,check_min,NULL);

    if (configure.widget_lrc_show_type==WIDGET_LRC_SHOW_TYPE_INDEPENDENT) {
        gboolean t=FALSE;
        gint type;
        if (configure.x<configure.widget_lrc_Independent_width+configure.widget_lrc_Independent_x+5&&configure.x>configure.widget_lrc_Independent_width+configure.widget_lrc_Independent_x-5) {
            t=TRUE;
            type=0;
        }
        if (configure.x+configure.width<configure.widget_lrc_Independent_x+5&&configure.x+configure.width>configure.widget_lrc_Independent_x-5) {
            t=TRUE;
            type=1;
        }
        if (configure.y<configure.widget_lrc_Independent_y + configure.widget_lrc_Independent_height+5&&configure.y>configure.widget_lrc_Independent_y + configure.widget_lrc_Independent_height-5) {
            t=TRUE;
            type=2;
        }
        if (configure.y+configure.height>configure.widget_lrc_Independent_y-5&&configure.y+configure.height<configure.widget_lrc_Independent_y+5) {
            t=TRUE;
            type=3;
        }
        if (t==TRUE) {
            min_Independent();
        }
    }

    return FALSE;
};

//反馈主窗口状态改变的函数，在一些窗口管理器中无法挂上？
gboolean window_state_change() {
    print_programming("window_state_change\n");
    GtkWidget *window = GTK_WIDGET(iceplayer.window);
    in_some_wm_that_cant_rec_window_state_event = FALSE;
    switch (gdk_window_get_state(GDK_WINDOW(window->window))) {
    case WINDOWS_STATE_SHOW_ALL_WORK_PLACES:
    case WINDOWS_STATE_SHOW:
        windows_state = WINDOWS_STATE_SHOW;
        break ;
    case WINDOWS_STATE_HIDE_ALL_WORK_PLACES:
    case WINDOWS_STATE_HIDE:
        windows_state = WINDOWS_STATE_HIDE;
        break;
    case WINDOWS_STATE_ICONIFIED:
        windows_state = WINDOWS_STATE_ICONIFIED;
        break;
    default:
        break;
    }
    return FALSE;
}
//退出
gboolean quit () {
    print_programming("quit\n");
    //记录主窗口的位置
    if (configure.mini)gtk_window_get_position(GTK_WINDOW(iceplayer.mini_window),&configure.mini_x,&configure.mini_y);
    //记录正在播放的歌曲的播放位置
    configure.last_pos_value=(gdouble) ((((gdouble) iceplayer.progress_left_eventbox->allocation.width+(gdouble) (iceplayer.progress_node_eventbox->allocation.width)) / (gdouble) iceplayer.progress_hbox->allocation.width) * (gdouble) core.length)/1000000000;

    print_debug("退出")
    gtk_main_quit();
    //写入配置文件
    configure_wirte();
    //删除一些临时文件
    system("rm -rf /tmp/iceplayer*");
    //写入列表
    save_lists();
    //退出
    exit(0);
    return TRUE;
}
/*
*点击退出的XX时候的反馈函数
*/
gboolean single_quit(GtkWidget *widget,GdkEvent *event,gpointer user_data) {
    if (event->type != GDK_BUTTON_RELEASE)
        quit();
    return FALSE;
}
/*
*调整大小的函数，type决定是哪个方向 比如RESIEZ_LEFT
*/
gboolean resize (GtkWidget *widget,GdkEventButton *event,gint type) {
    print_programming(("resize\n"));
    if (type == RESIZE_NONE)
        return FALSE;
    gtk_window_begin_resize_drag(GTK_WINDOW(iceplayer.window),type,event->button,event->x_root,event->y_root,event->time);
    return FALSE;
}
/*
*改变鼠标放置在一些控件上面的指针图像，type决定是怎么样的鼠标指针 比如RESIZE_BUTTOM就是GDK_SB_V_DOUBLE_ARROW
*/
gboolean change_cursor (GtkWidget *widget,GdkEventCrossing *event,gint type) {
    print_programming(("change_cursor\n"));
    if (type == RESIZE_NONE) {
        gdk_window_set_cursor(GTK_WIDGET(widget)->window, NULL);
        return FALSE;
    }
    GdkCursor* cursor = NULL;
    switch (type) {
    case RESIZE_BUTTOM:
        cursor = gdk_cursor_new(GDK_SB_V_DOUBLE_ARROW);
        break;
    case RESIZE_TOP:
        cursor = gdk_cursor_new(GDK_SB_V_DOUBLE_ARROW);
        break;
    case RESIZE_RIGHT:
        cursor = gdk_cursor_new(GDK_SB_H_DOUBLE_ARROW);
        break;
    case RESIZE_LEFT:
        cursor = gdk_cursor_new(GDK_SB_H_DOUBLE_ARROW);
        break;
    case RESIZE_TOP_LEFT:
        cursor = gdk_cursor_new(GDK_TOP_LEFT_CORNER);
        break;
    case RESIZE_TOP_RIGHT:
        cursor = gdk_cursor_new(GDK_TOP_RIGHT_CORNER);
        break;
    case RESIZE_BUTTOM_LEFT:
        cursor = gdk_cursor_new(12);
        break;
    case RESIZE_BUTTOM_RIGHT:
        cursor = gdk_cursor_new(14);
        break;
    }
    gdk_window_set_cursor(GTK_WIDGET(widget)->window, cursor);
    gdk_cursor_destroy(cursor);
    return FALSE;
}
/*
*反馈一些键盘按键事件，从而达到快捷键的效果，比sc更加直观
*/
gboolean short_cuts (GtkWidget *widget,GdkEventKey *event,gint type ) {
    print_programming(("short_cuts\n"));
    if (event->type == GDK_KEY_PRESS) {
        switch (event->keyval) {
        case GDK_Return: {
            list_menu_play_selection();
            break;
        }
        case GDK_Delete: {
            if (gtk_notebook_get_current_page(GTK_NOTEBOOK(iceplayer.notebook))==SWITCH_LIST)
                list_menu_deletet_selection(NULL,FALSE);
            break;
        }
        case GDK_Escape: {
            play();
            if (!main_lists.name[0]) return FALSE;
            if (core.state == GST_STATE_PLAYING) {
                if (core.pipeline) change_picture_on_press_and_release_event(NULL,CONTROL_PLAY);
                else change_picture_on_press_and_release_event(NULL,CONTROL_PAUSE);
            } else if (core.pipeline) change_picture_on_press_and_release_event(NULL,CONTROL_PAUSE);

            return TRUE;
        }
        default: {
            switch (type) {
            case SHORT_CUTS_LISTS:
                break;
            case SHORT_CUTS_SONGS:
                break;
            case SHORT_CUTS_ICEPLAYER:
                break;
            }
        }
        break;
        }
    }
    return FALSE;
}
//移动主窗口
gboolean move (GtkWidget *widget,GdkEventButton *event,gpointer user_data) {
    print_programming(("move\n"));
    if (event->button==3) {
        main_window_menu();
        return FALSE;
    }
    //如果检测发现歌词独立窗口与主窗口贴上的，就一起移动
    inline gboolean move_widget_lrc_window(gint type) {
        static gint width,height;
        gtk_window_get_position(GTK_WINDOW(iceplayer.window),&width,&height);
        switch (type) {
        case 0://y 坐标一致，窗口歌词在主窗口的左边
            gtk_window_move(GTK_WINDOW(widget_lrc.window),width-configure.widget_lrc_Independent_width,height);
            configure.widget_lrc_Independent_x=configure.x-configure.widget_lrc_Independent_width;
            configure.widget_lrc_Independent_y=configure.y;

            if ((((configure.widget_lrc_Independent_height-configure.height)>0)?(configure.widget_lrc_Independent_height-configure.height):(configure.height-configure.widget_lrc_Independent_height))<10)//窗口高度差距在10以下
                gtk_window_resize(GTK_WINDOW(widget_lrc.window),configure.widget_lrc_Independent_width,configure.height);

            break;
        case 1://y 坐标一致，窗口歌词在主窗口的右边
            gtk_window_move(GTK_WINDOW(widget_lrc.window),width+configure.width,height);
            configure.widget_lrc_Independent_x=configure.x+configure.width;
            configure.widget_lrc_Independent_y=configure.y;

            if ((((configure.widget_lrc_Independent_height-configure.height)>0)?(configure.widget_lrc_Independent_height-configure.height):(configure.height-configure.widget_lrc_Independent_height))<10)//窗口高度差距在10以下
                gtk_window_resize(GTK_WINDOW(widget_lrc.window),configure.widget_lrc_Independent_width,configure.height);

            break;
        case 2://x 坐标一致，窗口歌词在主窗口的下面
            gtk_window_move(GTK_WINDOW(widget_lrc.window),width,height- configure.widget_lrc_Independent_height);
            configure.widget_lrc_Independent_x=configure.x;
            configure.widget_lrc_Independent_y=configure.y- configure.widget_lrc_Independent_height;

            if ((((configure.widget_lrc_Independent_width-configure.width)>0)?(configure.widget_lrc_Independent_width-configure.width):(configure.width-configure.widget_lrc_Independent_width))<10)//窗口宽度度差距在10以下
                gtk_window_resize(GTK_WINDOW(widget_lrc.window),configure.width,configure.widget_lrc_Independent_height);

            break;
        case 3://x 坐标一致，窗口歌词在主窗口的上面
            gtk_window_move(GTK_WINDOW(widget_lrc.window),width,height+configure.height);
            configure.widget_lrc_Independent_x=configure.x;
            configure.widget_lrc_Independent_y=configure.y+configure.widget_lrc_Independent_height;

            if ((((configure.widget_lrc_Independent_width-configure.width)>0)?(configure.widget_lrc_Independent_width-configure.width):(configure.width-configure.widget_lrc_Independent_width))<10)//窗口宽度度差距在10以下
                gtk_window_resize(GTK_WINDOW(widget_lrc.window),configure.width,configure.widget_lrc_Independent_height);

            break;
        }
        gint x,y;
        GdkModifierType mask;
        gdk_window_get_pointer (iceplayer.window->window,&x, &y,&mask);

        if(mask==GDK_BUTTON1_MASK)
            return TRUE;
        else
            return FALSE;
    }
    if (event->type == GDK_BUTTON_PRESS) {
        if (event->state==GDK_CONTROL_MASK)
            gtk_window_begin_resize_drag(GTK_WINDOW(iceplayer.window),GDK_WINDOW_EDGE_SOUTH_EAST,1,event->x_root,event->y_root,event->time);
        else {
            if (configure.widget_lrc_show_type==WIDGET_LRC_SHOW_TYPE_INDEPENDENT) {
                gboolean t=FALSE;
                gint type;
                if (configure.x<configure.widget_lrc_Independent_width+configure.widget_lrc_Independent_x+5&&configure.x>configure.widget_lrc_Independent_width+configure.widget_lrc_Independent_x-5) {
                    t=TRUE;
                    type=0;
                }
                if (configure.x+configure.width<configure.widget_lrc_Independent_x+5&&configure.x+configure.width>configure.widget_lrc_Independent_x-5) {
                    t=TRUE;
                    type=1;
                }
                if (configure.y<configure.widget_lrc_Independent_y + configure.widget_lrc_Independent_height+5&&configure.y>configure.widget_lrc_Independent_y + configure.widget_lrc_Independent_height-5) {
                    t=TRUE;
                    type=2;
                }
                if (configure.y+configure.height>configure.widget_lrc_Independent_y-5&&configure.y+configure.height<configure.widget_lrc_Independent_y+5) {
                    t=TRUE;
                    type=3;
                }
                if (t==TRUE) {
                    g_timeout_add(100,G_SOURCE_FUNC(move_widget_lrc_window),(gpointer *)type);
                }
            }
            gtk_window_begin_move_drag(GTK_WINDOW(iceplayer.window),1,event->x_root,event->y_root,event->time);
        }
    } else {
        if (event->type==GDK_2BUTTON_PRESS) {
            if (GTK_WINDOW(iceplayer.window)->maximize_initially) {
                gtk_window_unmaximize(GTK_WINDOW(iceplayer.window) );
            } else {
                gtk_window_maximize (GTK_WINDOW(iceplayer.window) );
            }
        }
    }
    return FALSE;
}
//窗口左上角的菜单按钮
gboolean single_menu (GtkWidget *widget,GdkEventButton *event,gpointer user_data) {
    print_programming(("single_menu\n"));
    menu();
    change_picture_on_press_and_release_event(event,CONTROL_MENU);
    return TRUE;
}
//改变按钮的图标样式，type，你懂的
gboolean change_picture (GtkWidget *widget,GdkEventCrossing *event,gint type) {
    print_programming(("change_picture\n"));
    if (type==CONTROL_PLAY||type==CONTROL_PAUSE) {
        if (core.state == GST_STATE_PLAYING)
            type=CONTROL_PLAY;
        else
            type=CONTROL_PAUSE;
    }
    if (type==MINI_PLAY||type==MINI_PAUSE) {
        if (core.state == GST_STATE_PLAYING)
            type=MINI_PLAY;
        else
            type=MINI_PAUSE;
    }
    gint single_type =0;
    if ((gint)event == 0)
        single_type = 3;//press
    else if ((gint)event == 1)
        single_type = 1;//release
    else {
        if (event->type == GDK_ENTER_NOTIFY)
            single_type = 1;//enter
        if (event->type == GDK_LEAVE_NOTIFY)
            single_type = 2;//leave
    }
    if (single_type == 0) {
        print_debug("change_picture函数收到既不是进入的信号，也不是出去的信号，将返回");
        return FALSE;
    }
    GtkWidget **eventbox = NULL;
    GtkWidget **image = NULL;
    gchar path[50];
    switch (type) {
    case CONTROL_MENU: {
        eventbox = &iceplayer.menu_eventbox ;
        image = &iceplayer.menu_image ;
        strcpy(path,"menu");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case CONTROL_MINI: {
        eventbox = &iceplayer.mini_eventbox ;
        image = &iceplayer.mini_image ;
        strcpy(path,"mini");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case CONTROL_MIN: {
        eventbox = &iceplayer.min_eventbox ;
        image = &iceplayer.min_image ;
        strcpy(path,"min");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case CONTROL_MAX: {
        eventbox = &iceplayer.max_eventbox ;
        image = &iceplayer.max_image ;
        strcpy(path,"max");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case CONTROL_CLOSE: {
        eventbox = &iceplayer.close_eventbox ;
        image = &iceplayer.close_image ;
        strcpy(path,"close");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case CONTROL_PLAY: {
        eventbox = &iceplayer.play_eventbox ;
        image = &iceplayer.play_image ;
        strcpy(path,"play");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case CONTROL_PAUSE: {
        eventbox = &iceplayer.play_eventbox ;
        image = &iceplayer.play_image ;
        strcpy(path,"pause");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case CONTROL_PREV: {
        eventbox = &iceplayer.prev_eventbox ;
        image = &iceplayer.prev_image ;
        strcpy(path,"prev");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case CONTROL_NEXT: {
        eventbox = &iceplayer.next_eventbox ;
        image = &iceplayer.next_image ;
        strcpy(path,"next");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case CONTROL_STOP: {
        eventbox = &iceplayer.stop_eventbox ;
        image = &iceplayer.stop_image ;
        strcpy(path,"stop");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case CONTROL_OPEN: {
        eventbox = &iceplayer.open_eventbox ;
        image = &iceplayer.open_image ;
        strcpy(path,"open");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case CONTROL_PROGRESS: {
        eventbox = &iceplayer.progress_node_eventbox ;
        image = &iceplayer.progress_node_image ;
        strcpy(path,"progress.node");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case CONTROL_VOLUME: {
        eventbox = &iceplayer.volume_eventbox ;
        image = &iceplayer.volume_image ;
        strcpy(path,"volume");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case SWITCH_LIST: {
        eventbox = &iceplayer.switch_list_eventbox ;
        image = &iceplayer.switch_list_image ;
        strcpy(path,"button.list");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case SWITCH_LRC: {
        eventbox = &iceplayer.switch_lrc_eventbox ;
        image = &iceplayer.switch_lrc_image ;
        strcpy(path,"button.lrc");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case SWITCH_MUSICBOX: {
        eventbox = &iceplayer.switch_music_box_eventbox ;
        image = &iceplayer.switch_music_box_image ;
        strcpy(path,"button.musicbox");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case SWITCH_CONFIG: {
        eventbox = &iceplayer.switch_configure_eventbox ;
        image = &iceplayer.switch_configure_image ;
        strcpy(path,"button.config");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case SWITCH_HELP: {
        eventbox = &iceplayer.switch_help_eventbox ;
        image = &iceplayer.switch_help_image ;
        strcpy(path,"button.help");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case WIDGET_LRC_CONTROL_MAX: {
        eventbox = &widget_lrc.max_eventbox ;
        image = &widget_lrc.max_image ;
        strcpy(path,"max");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case WIDGET_LRC_CONTROL_CLOSE: {
        eventbox = &widget_lrc.close_eventbox ;
        image = &widget_lrc.close_image ;
        strcpy(path,"close");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case WIDGET_LRC_CONTROL_MIN: {
        eventbox = &widget_lrc.min_eventbox ;
        image = &widget_lrc.min_image ;
        strcpy(path,"min");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case MINI_CLOSE: {
        eventbox = &iceplayer.mini_close_eventbox ;
        image = &iceplayer.mini_close_image ;
        strcpy(path,"mini.close");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case MINI_MINI: {
        eventbox = &iceplayer.mini_mini_eventbox ;
        image = &iceplayer.mini_mini_image ;
        strcpy(path,"mini.mini");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case MINI_PLAY: {
        eventbox = &iceplayer.mini_play_eventbox ;
        image = &iceplayer.mini_play_image ;
        strcpy(path,"mini.play");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case MINI_PAUSE: {
        eventbox = &iceplayer.mini_play_eventbox ;
        image = &iceplayer.mini_play_image ;
        strcpy(path,"mini.pause");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case MINI_PREV: {
        eventbox = &iceplayer.mini_prev_eventbox;
        image = &iceplayer.mini_prev_image ;
        strcpy(path,"mini.prev");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case MINI_NEXT: {
        eventbox = &iceplayer.mini_next_eventbox ;
        image = &iceplayer.mini_next_image ;
        strcpy(path,"mini.next");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case MINI_STOP: {
        eventbox = &iceplayer.mini_stop_eventbox ;
        image = &iceplayer.mini_stop_image ;
        strcpy(path,"mini.stop");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case MINI_SONGS: {
        eventbox = &iceplayer.mini_songs_eventbox ;
        image = &iceplayer.mini_songs_image ;
        strcpy(path,"mini.songs");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    case MINI_LISTS: {
        eventbox = &iceplayer.mini_lists_eventbox ;
        image = &iceplayer.mini_lists_image ;
        strcpy(path,"mini.lists");
        if (single_type == 1)
            strcat(path,".on");
        break;
    }
    }
    gchar *dir = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(dir,get_skin_dir());
    gtk_container_remove_new(GTK_CONTAINER(*eventbox),*image);
    *image = gtk_image_new_from_file_new(dir,path);
    g_free_n(dir);
    gtk_widget_show(*image);
    gtk_container_add(GTK_CONTAINER(*eventbox),*image);
    return FALSE;
}
///control:
//mini按钮
gboolean mini (GtkWidget *widget,GdkEventButton *event,gpointer user_data) {
    print_programming(("mini\n"));
    if (event==NULL||event->type == GDK_BUTTON_RELEASE) {
        if (!configure.mini) {
            //if(windows_state==WINDOWS_STATE_SHOW||windows_state==WINDOWS_STATE_SHOW_ALL_WORK_PLACES)hide();
            gtk_widget_show(iceplayer.mini_window);
            gtk_window_move(GTK_WINDOW(iceplayer.mini_window),configure.mini_x,configure.mini_y);
        } else {

            if (windows_state==WINDOWS_STATE_HIDE||windows_state==WINDOWS_STATE_HIDE_ALL_WORK_PLACES)show();

            gtk_window_get_position(GTK_WINDOW(iceplayer.mini_window),&configure.mini_x,&configure.mini_y);
            gtk_widget_hide(iceplayer.mini_window);
        }
        configure.mini=!configure.mini;
    }
    return TRUE;
}
//始终在最前
gboolean above() {
    print_programming(("above\n"));
    gchar *path = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    configure.above=!configure.above;

    strcpy(path,"above");
    if (configure.above)
        strcat(path,".on");

    gchar *dir = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(dir,get_skin_dir());
    gtk_container_remove_new(GTK_CONTAINER(iceplayer.above_eventbox),iceplayer.above_image);
    iceplayer.above_image = gtk_image_new_from_file_new(dir,path);
    g_free_n(path);
    g_free_n(dir);
    gtk_widget_show(iceplayer.above_image);
    gtk_container_add(GTK_CONTAINER(iceplayer.above_eventbox),iceplayer.above_image);

    gtk_window_set_keep_above(GTK_WINDOW(iceplayer.window),configure.above);
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.always_above_checkbutton),configure.above);

    gtk_widget_queue_draw(iceplayer.window);
    return TRUE;
}
//最小化
gboolean min (GtkWidget *widget,GdkEventButton *event,gpointer user_data) {
    print_programming(("min\n"));
    if (event->type == GDK_BUTTON_RELEASE) {
        hide(NULL);

        if (configure.widget_lrc_show_type==WIDGET_LRC_SHOW_TYPE_INDEPENDENT) {
            gboolean t=FALSE;
            gint type;
            if (configure.x<configure.widget_lrc_Independent_width+configure.widget_lrc_Independent_x+5&&configure.x>configure.widget_lrc_Independent_width+configure.widget_lrc_Independent_x-5) {
                t=TRUE;
                type=0;
            }
            if (configure.x+configure.width<configure.widget_lrc_Independent_x+5&&configure.x+configure.width>configure.widget_lrc_Independent_x-5) {
                t=TRUE;
                type=1;
            }
            if (configure.y<configure.widget_lrc_Independent_y + configure.widget_lrc_Independent_height+5&&configure.y>configure.widget_lrc_Independent_y + configure.widget_lrc_Independent_height-5) {
                t=TRUE;
                type=2;
            }
            if (configure.y+configure.height>configure.widget_lrc_Independent_y-5&&configure.y+configure.height<configure.widget_lrc_Independent_y+5) {
                t=TRUE;
                type=3;
            }
            if (t==TRUE) {
                min_Independent();
            }
        }
    }
    change_picture_on_press_and_release_event(event,CONTROL_MIN);
    //gtk_widget_hide(iceplayer.window);

    return TRUE;
}
//最大化
gboolean max (GtkWidget *widget,GdkEventButton *event,gpointer user_data) {
    print_programming(("max\n"));
    if (event->type == GDK_BUTTON_RELEASE) {
        if (GTK_WINDOW(iceplayer.window)->maximize_initially) {
            gtk_window_unmaximize(GTK_WINDOW(iceplayer.window) );
        } else {
            gtk_window_maximize (GTK_WINDOW(iceplayer.window) );
        }
    }
    change_picture_on_press_and_release_event(event,CONTROL_MAX);
    return TRUE;
}
//上一首 转prev()
gboolean single_prev (GtkWidget *widget,GdkEventButton *event,gpointer user_data) {
    print_programming(("single_prev\n"));
    if (event->type == GDK_BUTTON_RELEASE) {
        if (!main_lists.name[0]) {
            add();
            return TRUE;
        }
        if (playing_song[0]) {
            prev();
            change_picture_on_press_and_release_event(event,CONTROL_NEXT);
        }
    }
    change_picture_on_press_and_release_event(event,CONTROL_PREV);
    return TRUE;
}
//播放，转play()
gboolean single_play (GtkWidget *widget,GdkEventButton *event,gpointer user_data) {
    print_programming(("single_play\n"));
    if (event != NULL)
        if (event->type != GDK_BUTTON_RELEASE)
            return FALSE;

    if (!main_lists.name[0]) {
        play();
        return TRUE;
    }
    play();
    if (core.state == GST_STATE_PLAYING) {
        if (core.pipeline) {
            change_picture_on_press_and_release_event(event,CONTROL_PLAY);
            change_picture_on_press_and_release_event(event,MINI_PLAY);
        } else {
            change_picture_on_press_and_release_event(event,CONTROL_PAUSE);
            change_picture_on_press_and_release_event(event,MINI_PAUSE);
        }
    } else if (core.pipeline) {
        change_picture_on_press_and_release_event(event,CONTROL_PAUSE);
        change_picture_on_press_and_release_event(event,MINI_PAUSE);
    }

    return TRUE;
}
//下一首，转next()
gboolean single_next (GtkWidget *widget,GdkEventButton *event,gpointer user_data) {
    print_programming(("single_next\n"));
    if (event->type == GDK_BUTTON_RELEASE) {
        if (!main_lists.name[0]) {
            add_menu();
            return TRUE;
        }
        if (playing_song[0]) {
            next();
            change_picture_on_press_and_release_event(event,CONTROL_NEXT);
        }
    }
    return TRUE;
}
//停止，转stop()
gboolean single_stop (GtkWidget *widget,GdkEventButton *event,gpointer user_data) {
    print_programming(("single_stop\n"));
    if (event->type == GDK_BUTTON_RELEASE) {
        if (core.pipeline) {
            stop();
            change_picture_on_press_and_release_event(event,CONTROL_PLAY);
            change_picture_on_press_and_release_event(event,CONTROL_STOP);
            change_picture_on_press_and_release_event(event,MINI_PLAY);
            change_picture_on_press_and_release_event(event,MINI_STOP);
        }
    }
    return TRUE;
}
//添加文件，转add()
gboolean single_add (GtkWidget *widget,GdkEventButton *event,gint type) {
    print_programming(("single_add\n"));
    if (type == ADD_OPEN_BUTTON) {
        add();
    }
    change_picture_on_press_and_release_event(event,CONTROL_OPEN);
    return TRUE;
}
//进度条的跳到指定位置播放
gboolean jump_to_pos(gpointer data) {
    print_programming(("jump_to_pos\n"));
    g_object_set(core.vol, "volume",0.01, NULL);
    gst_element_seek(core.pipeline,1.0,GST_FORMAT_TIME,GST_SEEK_FLAG_FLUSH,GST_SEEK_TYPE_SET,configure.last_pos_value* GST_SECOND,GST_SEEK_TYPE_NONE,GST_CLOCK_TIME_NONE);

    gboolean ba_vol() {
        g_object_set(core.vol, "volume",configure.volume, NULL);
        return FALSE;
    }
    g_timeout_add(1000,ba_vol,NULL);

    return FALSE;
}
//进度的解析
gboolean progress (GtkWidget *widget,GdkEventButton *event,gint type) {
    if (!core.pipeline)return FALSE;
    switch (type) {
    case PROGRESS_LEFT: {
        if (!strncmp(focus_song,"<cue>",5)) {
            //cue
            gint min_pre,sec_pre,mir_pre,min_end,sec_end,mir_end;
            gchar p[STRINGS_LENGTH];
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
            gdouble m=((gdouble) ((gdouble) (event->x-(iceplayer.progress_node_eventbox->allocation.width)/2) / (gdouble) iceplayer.progress_hbox->allocation.width)*((min_end-min_pre)*60+sec_end-sec_pre)+min_pre*60+sec_pre);
            g_object_set(core.vol, "volume",0.01, NULL);
            gst_element_seek(core.pipeline,1.0,GST_FORMAT_TIME,GST_SEEK_FLAG_FLUSH,GST_SEEK_TYPE_SET,m* GST_SECOND,GST_SEEK_TYPE_NONE,GST_CLOCK_TIME_NONE);
            gboolean ba_vol() {
                g_object_set(core.vol, "volume",configure.volume, NULL);
                return FALSE;
            }
            g_timeout_add(200,ba_vol,NULL);
        } else {
            //普通歌曲
            g_object_set(core.vol, "volume",0.01, NULL);
            gst_element_seek(core.pipeline,1.0,GST_FORMAT_TIME,GST_SEEK_FLAG_FLUSH,GST_SEEK_TYPE_SET,(gdouble) ((((gdouble) (event->x-(iceplayer.progress_node_eventbox->allocation.width)/2) / (gdouble) iceplayer.progress_hbox->allocation.width)) * (gdouble) core.length)/1000000000 * GST_SECOND,GST_SEEK_TYPE_NONE,GST_CLOCK_TIME_NONE);
            gboolean ba_vol() {
                g_object_set(core.vol, "volume",configure.volume, NULL);
                return FALSE;
            }
            g_timeout_add(200,ba_vol,NULL);
        }
        break;
    }
    case PROGRESS_RIGHT: {
        if (!strncmp(focus_song,"<cue>",5)) {
            //cue
            gint min_pre,sec_pre,mir_pre,min_end,sec_end,mir_end;
            gchar p[STRINGS_LENGTH];
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
            gdouble m = (gdouble) ((gdouble) ((event->x+(gdouble) iceplayer.progress_left_eventbox->allocation.width+(gdouble) (iceplayer.progress_node_eventbox->allocation.width)) / (gdouble) (iceplayer.progress_hbox->allocation.width))*((min_end-min_pre)*60+sec_end-sec_pre)+min_pre*60+sec_pre);
            g_object_set(core.vol, "volume",0.01, NULL);
            gst_element_seek(core.pipeline,1.0,GST_FORMAT_TIME,GST_SEEK_FLAG_FLUSH,GST_SEEK_TYPE_SET,m* GST_SECOND,GST_SEEK_TYPE_NONE,GST_CLOCK_TIME_NONE);
            gboolean ba_vol() {
                g_object_set(core.vol, "volume",configure.volume, NULL);
                return FALSE;
            }
            g_timeout_add(200,ba_vol,NULL);

        } else {
            //普通歌曲
            g_object_set(core.vol, "volume",0.01, NULL);
            gst_element_seek(core.pipeline,1.0,GST_FORMAT_TIME,GST_SEEK_FLAG_FLUSH,GST_SEEK_TYPE_SET,(gdouble) ((((gdouble) event->x+(gdouble) iceplayer.progress_left_eventbox->allocation.width+(gdouble) (iceplayer.progress_node_eventbox->allocation.width)) / (gdouble) iceplayer.progress_hbox->allocation.width) * (gdouble) core.length)/1000000000 * GST_SECOND,GST_SEEK_TYPE_NONE,GST_CLOCK_TIME_NONE);
            gboolean ba_vol() {
                g_object_set(core.vol, "volume",configure.volume, NULL);
                return FALSE;
            }
            g_timeout_add(200,ba_vol,NULL);
        }
        break;
    }
    case PROGRESS_NODE: {
        break;
    }

    }
    change_picture_on_press_and_release_event(event,CONTROL_PROGRESS);
    return TRUE;
}
//鼠标滚轮事件的进度跳转
gboolean progress_scroll(GtkWidget *widget,GdkEventScroll *event,gpointer user_data) {
    gdouble m=0;
    if(event->direction==GDK_SCROLL_UP) {
         m=((gdouble) core.pos)/1000000000 - 10;
         if(m<0.0)m=0;
    }

    if(event->direction==GDK_SCROLL_DOWN) {
         m=(gdouble) core.pos/1000000000  + 10;
    }
    g_object_set(core.vol, "volume",0.01, NULL);
    gst_element_seek(core.pipeline,1.0,GST_FORMAT_TIME,GST_SEEK_FLAG_FLUSH,GST_SEEK_TYPE_SET,m* GST_SECOND,GST_SEEK_TYPE_NONE,GST_CLOCK_TIME_NONE);
    g_object_set(core.vol, "volume",configure.volume, NULL);
    return TRUE;
}
//音量
gboolean change_volume(GtkWidget *vscale) {
    print_programming(("change_volume\n"));
    if (core.pipeline)set_volume( gtk_range_get_value(GTK_RANGE(vscale)));
    return FALSE;
}
gboolean volume (GtkWidget *widget,GdkEventButton *event,gpointer user_data) {
    print_programming(("volume\n"));
    if (event->type == GDK_BUTTON_RELEASE) {
        static GtkWidget *iceplayer_volume;
        if (iceplayer_volume&&GTK_IS_WIDGET(iceplayer_volume))
            gtk_widget_show_all(iceplayer_volume);
        else {
            GtkWidget *vscale;

            iceplayer_volume = gtk_window_new (GTK_WINDOW_TOPLEVEL);
            gtk_window_set_title (GTK_WINDOW (iceplayer_volume), "iceplayer_volume");
            gtk_window_set_position (GTK_WINDOW (iceplayer_volume), GTK_WIN_POS_MOUSE);
            gtk_window_set_keep_above (GTK_WINDOW (iceplayer_volume), TRUE);
            gtk_window_set_resizable (GTK_WINDOW (iceplayer_volume), FALSE);
            gtk_window_set_decorated (GTK_WINDOW (iceplayer_volume), FALSE);

            vscale = gtk_vscale_new (GTK_ADJUSTMENT (gtk_adjustment_new (1, 0, 1, 0, 0, 0)));
            gtk_widget_show (vscale);
            gtk_container_add (GTK_CONTAINER (iceplayer_volume), vscale);
            gtk_widget_set_size_request (vscale, 18, 100);
            gtk_scale_set_draw_value (GTK_SCALE (vscale), FALSE);
            gtk_scale_set_value_pos (GTK_SCALE (vscale), GTK_POS_BOTTOM);
            gtk_range_set_inverted (GTK_RANGE (vscale), TRUE);
            gtk_range_set_value(GTK_RANGE(vscale),configure.volume);

            g_signal_connect ((gpointer) iceplayer_volume, "focus-out-event",G_CALLBACK (gtk_widget_hide),NULL);
            g_signal_connect ((gpointer) vscale, "change_value",G_CALLBACK (change_volume),NULL);

            gtk_widget_show_all(iceplayer_volume);
        }
    }
    change_picture_on_press_and_release_event(event,CONTROL_VOLUME);
    return TRUE;
}
//鼠标滚轮事件的音量跳转
gboolean volume_scroll(GtkWidget *widget,GdkEventScroll *event,gpointer user_data) {
    if(event->direction==GDK_SCROLL_UP) {
configure.volume=configure.volume+0.1;
if(configure.volume>=1.0)configure.volume=1;
    }

    if(event->direction==GDK_SCROLL_DOWN) {
configure.volume=configure.volume-0.1;
if(configure.volume<=0.0)configure.volume=0;
    }
        g_object_set(core.vol, "volume",configure.volume, NULL);
    return TRUE;
}

///list:
//反馈多列表栏改变焦点的信号，更新当前显示的列表
gboolean lists_treview_change_cursor (GtkTreeView *treeview,gpointer user_data) {
    print_programming(("lists_treview_change_cursor\n"));
    update_focus_lists();
    return FALSE;
}
//多列表栏右键事件
gboolean lists_treeview_click (GtkWidget *widget,GdkEventButton *event,gpointer user_data) {
    print_programming(("lists_treeview_click\n"));
    if (event->button==3&&event->type==GDK_BUTTON_PRESS) {
        show_lists_menu();
    }
    return FALSE;
}
//多列表栏文件夹拖放（未完成）
gboolean lists_treeview_drag_data_received (GtkWidget *widget,GdkDragContext *drag_context,gint x,gint y,GtkSelectionData *data,guint info,guint time,gpointer user_data) {
    print_programming(("lists_treeview_drag_data_received\n"));
    return FALSE;
}
gboolean lists_treeview_drag_end (GtkWidget *widget,GdkDragContext *drag_context,gpointer user_data) {
    print_programming(("lists_treeview_drag_end\n"));
    return FALSE;
}
//歌曲栏的更新点选上的歌曲
gboolean songs_treeview_change_cursor (GtkTreeView *treeview,gpointer user_data) {

    print_programming(("songs_treeview_change_cursor\n"));
    update_lists_focus_song();

    return FALSE;
}
gboolean songs_treeview_click (GtkWidget *widget,GdkEventButton *event,gpointer user_data) {

    print_programming(("songs_treeview_click\n"));
    if (event->type==GDK_2BUTTON_PRESS) { //双击
        update_focus_song_from_selection();
        ///这么复杂只是想实现：正在播反A列表，A列表高亮，点击B列表，播放B列表内歌曲，然后实现<取消掉原A的高亮，B高亮>的效果。
        gchar *a = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        strcpy(a,focus_lists);
        strcpy(focus_lists,playing_list);
        strcpy(playing_list,a);
        show_lists(focus_lists);
        strcpy(playing_list,a);
        strcpy(focus_lists,a);
        show_lists(NULL);
        g_free_n(a);

        ///

        gchar * dir = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        strcpy(dir,get_skin_dir());

        gtk_container_remove_new(GTK_CONTAINER(iceplayer.play_eventbox),iceplayer.play_image);
        iceplayer.play_image = gtk_image_new_from_file_new(dir,"play");
        g_free_n(dir);
        gtk_widget_show(iceplayer.play_image);
        gtk_container_add(GTK_CONTAINER(iceplayer.play_eventbox),iceplayer.play_image);

        play_uri();
    }
    if (event->button==3&&event->type==GDK_BUTTON_PRESS) {
        show_list_menu();
        GtkTreeModel *model;
        //GtkTreeIter iter;
        GList *list_select = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_songs_treeview))),&model);
        if (list_select)
            if (list_select->next)
                return TRUE;
    }

    return FALSE;
}
///switch:
void change_widget_lrc_ui(GtkWidget *widget,gint type) {
    print_programming(("change_widget_lrc_ui\n"));
    if(iceplayer_widget_lrc_image==NULL )
        iceplayer_widget_lrc_image = cairo_image_surface_create_from_png_new(iceplayer_widget_lrc_image,get_iceplayer_widget_lrc_back_pic());
    switch (configure.widget_lrc_show_type) {
    case WIDGET_LRC_SHOW_TYPE_INDEPENDENT: {
        gtk_window_get_position(GTK_WINDOW(widget_lrc.window),&configure.widget_lrc_Independent_x,&configure.widget_lrc_Independent_y);
        gtk_window_get_size(GTK_WINDOW(widget_lrc.window),&configure.widget_lrc_Independent_width,&configure.widget_lrc_Independent_height);
        gtk_widget_hide(widget_lrc.window);

        break;
    }
    case WIDGET_LRC_SHOW_TYPE_HPANED: {
        break ;
    }
    case WIDGET_LRC_SHOW_TYPE_INTEGRATION: {
        if (widget)gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),SWITCH_LIST);
        break ;
    }
    }
    if (GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned)&&GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned->parent))
        gtk_container_remove_new(GTK_CONTAINER(iceplayer.hpaned),iceplayer.back_eventbox_widget_lrc_hpaned);

    switch (type) {
    case WIDGET_LRC_SHOW_TYPE_INDEPENDENT: {
        if (widget&&configure.show_page==SWITCH_LRC)
            gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),SWITCH_LIST);
        gtk_widget_show_all(widget_lrc.window);
        gtk_window_move(GTK_WINDOW(widget_lrc.window),configure.widget_lrc_Independent_x,configure.widget_lrc_Independent_y);
        gtk_window_resize(GTK_WINDOW(widget_lrc.window),configure.widget_lrc_Independent_width,configure.widget_lrc_Independent_height);
        break;
    }
    case WIDGET_LRC_SHOW_TYPE_INTEGRATION: {
        if (widget) {
            gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),SWITCH_LRC);
            configure.show_page=SWITCH_LRC;
        }
        gtk_widget_set_size_request (iceplayer.widget_lrc_label,0, 0);
        gtk_widget_set_usize(iceplayer.widget_lrc_label,configure.width-4, configure.height-iceplayer.main_fixed->allocation.height-iceplayer.switch_hbox->allocation.height-24);
        break;
    }
    case WIDGET_LRC_SHOW_TYPE_HPANED: {
        iceplayer.widget_lrc_label_hpaned = gtk_label_new("");
        gtk_widget_show(iceplayer.widget_lrc_label_hpaned);
        gtk_label_set_justify(GTK_LABEL(iceplayer.widget_lrc_label_hpaned),GTK_JUSTIFY_CENTER);
        gtk_misc_set_alignment (GTK_MISC (iceplayer.widget_lrc_label_hpaned), 0.5, 0.5);
        gtk_widget_set_size_request (iceplayer.widget_lrc_label_hpaned,0, 0);

        iceplayer.back_eventbox_widget_lrc_hpaned = gtk_event_box_new ();
        gtk_widget_show(iceplayer.back_eventbox_widget_lrc_hpaned);
        gtk_event_box_set_visible_window (GTK_EVENT_BOX (iceplayer.back_eventbox_widget_lrc_hpaned), FALSE);
        gtk_container_add (GTK_CONTAINER (iceplayer.back_eventbox_widget_lrc_hpaned), iceplayer.widget_lrc_label_hpaned);

        gtk_paned_pack2 (GTK_PANED (iceplayer.hpaned), iceplayer.back_eventbox_widget_lrc_hpaned, TRUE, TRUE);
        gtk_paned_set_position(GTK_PANED(iceplayer.hpaned),configure.hpaned_pos );
        g_signal_connect ((gpointer) iceplayer.back_eventbox_widget_lrc_hpaned,      "button_press_event",G_CALLBACK (move),NULL);
        break;
    }
    }

    configure.widget_lrc_show_type= type;

    PangoFontDescription *pfd;
    pfd = pango_font_description_from_string(configure.widget_lrc_font);
    if (GTK_IS_WIDGET(iceplayer.widget_lrc_label))gtk_widget_modify_font (iceplayer.widget_lrc_label, pfd);
    if (GTK_IS_WIDGET(widget_lrc.label)) gtk_widget_modify_font (widget_lrc.label, pfd);
    if (GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned))gtk_widget_modify_font (iceplayer.widget_lrc_label_hpaned, pfd);
    pango_font_description_free (pfd);


    widget_lrc_refresh(TRUE);
}
//下面的切换栏
gboolean switch_button (GtkWidget *widget,GdkEventButton *event,gint type) {
    print_programming(("switch_button\n"));
    if (event->type==GDK_BUTTON_PRESS) {
        if (event->button==1) {
            if (type==SWITCH_LRC) {

                lrc_line_number_to_show = -1;
                switch (configure.widget_lrc_show_type) {
                case WIDGET_LRC_SHOW_TYPE_INDEPENDENT: {
                    change_widget_lrc_ui(NULL,WIDGET_LRC_SHOW_TYPE_INDEPENDENT);
                    if (configure.show_page==SWITCH_LRC)
                        configure.show_page = SWITCH_LIST;
                    gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),configure.show_page);
                    return TRUE;
                }
                case WIDGET_LRC_SHOW_TYPE_HPANED: {
                    change_widget_lrc_ui(NULL,WIDGET_LRC_SHOW_TYPE_HPANED);
                    type =  SWITCH_LIST;
                    configure.show_page=SWITCH_LIST;
                    gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),type);
                    type=SWITCH_LRC;
                    return TRUE;
                }
                case WIDGET_LRC_SHOW_TYPE_INTEGRATION: {
                    configure.show_page = SWITCH_LRC;
                    change_widget_lrc_ui(NULL,WIDGET_LRC_SHOW_TYPE_INTEGRATION);
                    gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),SWITCH_LRC);
                    return TRUE;
                }
                }
            } else {
                gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),type);
                configure.show_page = type;
                change_picture_on_press_and_release_event(event,type);
            }
        }
        if (event->button==3) {
            if (type==SWITCH_LRC) {
                GtkWidget * menu=gtk_menu_new();
                GtkWidget *menu_INDEPENDENT =gtk_image_menu_item_new_with_label(_("独立窗口"));
                GtkWidget *menu_INTEGRATION =gtk_image_menu_item_new_with_label(_("独占主窗体"));
                GtkWidget *menu_HPANED =gtk_image_menu_item_new_with_label(_("集成入主窗体"));
                gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_INDEPENDENT);
                gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_HPANED);
                gtk_menu_shell_append(GTK_MENU_SHELL(menu),menu_INTEGRATION);

                g_signal_connect((gpointer) menu_INDEPENDENT ,"activate" ,G_CALLBACK(change_widget_lrc_ui),(gpointer *)WIDGET_LRC_SHOW_TYPE_INDEPENDENT);
                g_signal_connect((gpointer) menu_HPANED ,"activate" ,G_CALLBACK(change_widget_lrc_ui),(gpointer *)WIDGET_LRC_SHOW_TYPE_HPANED);
                g_signal_connect((gpointer) menu_INTEGRATION ,"activate" ,G_CALLBACK(change_widget_lrc_ui),(gpointer *)WIDGET_LRC_SHOW_TYPE_INTEGRATION);

                gtk_widget_show_all(menu);
                popup_menu(menu,2,2);

                return TRUE;
            }
            gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),type);
            configure.show_page = type;
            change_picture_on_press_and_release_event(event,type);
            return TRUE;
        }

    }
    return TRUE;
}
//在主窗口空白处右键的菜单
gboolean main_window_menu() {
    print_programming(("main_window_menu\n"));

    GtkWidget *list_menu = gtk_menu_new();
    GtkWidget *image = NULL;
    //GtkWidget *jump_to_playing_song_item;
    GtkWidget *equalizer_item = NULL;
    GtkWidget *mini_item = NULL;
    GtkWidget *separator_4 = NULL;
    GtkWidget *lrc_item = NULL;
    GtkWidget *lrc_item_menu = NULL;
    GtkWidget *switch_to_lrc_iem = NULL;
    GtkWidget *show_desktop_lrc_item = NULL;
    //GtkWidget *search_lrc_with_custom_word_item;
    GtkWidget *separator_5 = NULL;
    GtkWidget *lock_desktop_lrc_item = NULL;
    GtkWidget *edit_lrc_item = NULL;
    //GtkWidget *sort_songs;
    //GtkWidget *sort_songs_menu;
    //GtkWidget *sort_with_file_name_item;
    //GtkWidget *un_sort_with_file_name_item;
    //GtkWidget *sort_with_id3_editor_item;
    //GtkWidget *sort_with_id3_song_name_item;
    //GtkWidget *sort_with_radom_item;
    GtkWidget *separator_6 = NULL;
    GtkWidget *play_mode_item = NULL;
    GtkWidget *play_mode_item_menu = NULL;
    GtkWidget *play_mode_normal_item = NULL;
    GtkWidget *play_mode_unnormal_item = NULL;
    GtkWidget *play_mode_radom_item = NULL;
    GtkWidget *play_mode_one_item = NULL;
    GtkWidget *separator_7 = NULL;
    GtkWidget *separator_8 = NULL;
    GtkWidget *play_mode_other_lists_item = NULL;
    //GtkWidget *lists_add_files;
    //GtkWidget *lists_add_folder;
    GtkWidget *show_lrc_item = NULL;
    GtkWidget *menu_research_lrc = NULL;
    GtkWidget *menu_research = NULL;
    GtkWidget *menu_INDEPENDENT = NULL;
    GtkWidget *menu_HPANED = NULL;
    GtkWidget *menu_INTEGRATION = NULL;

    equalizer_item = gtk_image_menu_item_new_with_label (_("均衡器"));
    gtk_widget_show (equalizer_item);
    gtk_container_add (GTK_CONTAINER (list_menu), equalizer_item);


    mini_item = gtk_image_menu_item_new_with_label (_("迷你播放器"));
    gtk_widget_show (mini_item);
    gtk_container_add (GTK_CONTAINER (list_menu), mini_item);

    image = gtk_image_new_from_stock ("gtk-properties", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (equalizer_item), image);

    lrc_item = gtk_image_menu_item_new_with_label (_("歌词"));
    gtk_widget_show (lrc_item);
    gtk_container_add (GTK_CONTAINER (list_menu), lrc_item);

    image = gtk_image_new_from_stock ("gtk-select-all", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lrc_item), image);

    lrc_item_menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (lrc_item), lrc_item_menu);

    switch_to_lrc_iem = gtk_image_menu_item_new_with_label (_("切换到歌词设置页面"));
    gtk_widget_show (switch_to_lrc_iem);
    gtk_container_add (GTK_CONTAINER (lrc_item_menu), switch_to_lrc_iem);

    image = gtk_image_new_from_stock ("gtk-jump-to", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (switch_to_lrc_iem), image);

    if (!configure.lrc) show_lrc_item = gtk_image_menu_item_new_with_label (_("使用歌词"));
    else  show_lrc_item = gtk_image_menu_item_new_with_label (_("关闭歌词"));
    gtk_widget_show (show_lrc_item);
    gtk_container_add (GTK_CONTAINER (lrc_item_menu), show_lrc_item);

    if (!configure.lrc)image = gtk_image_new_from_stock (GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
    else image = gtk_image_new_from_stock (GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (show_lrc_item), image);

    inline void research_lrc() {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_search_lrc_checkbutton),!configure.search_lrc);
        update_configure();
    }
    if (configure.search_lrc)menu_research_lrc = gtk_image_menu_item_new_with_label(_("关闭搜索歌词功能"));
    else menu_research_lrc = gtk_image_menu_item_new_with_label(_("使用搜索歌词功能"));
    gtk_widget_show (menu_research_lrc);
    gtk_container_add (GTK_CONTAINER (lrc_item_menu),menu_research_lrc);

    if (!configure.search_lrc)image = gtk_image_new_from_stock (GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
    else image = gtk_image_new_from_stock (GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_research_lrc), image);
    if (configure.search_lrc) {
        menu_research = gtk_image_menu_item_new_with_label(_("重新搜索歌词"));
        gtk_widget_show (menu_research);
        gtk_container_add (GTK_CONTAINER (lrc_item_menu),menu_research);

        image = gtk_image_new_from_stock (GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU);
        gtk_widget_show (image);
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_research), image);
    }
    g_signal_connect ((gpointer) menu_research_lrc, "activate",G_CALLBACK (research_lrc),NULL);
    if (configure.search_lrc)g_signal_connect ((gpointer) menu_research, "activate",G_CALLBACK (search_lrc_pre),NULL);

    separator_8 = gtk_separator_menu_item_new ();
    gtk_widget_show (separator_8);
    gtk_container_add (GTK_CONTAINER (lrc_item_menu), separator_8);
    gtk_widget_set_sensitive (separator_8, FALSE);

    menu_INDEPENDENT=gtk_image_menu_item_new_with_label(_("窗口歌词 | 独立窗口"));
    gtk_widget_show (menu_INDEPENDENT);
    image = gtk_image_new_from_stock ("gtk-indent", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_INDEPENDENT), image);

    menu_INTEGRATION=gtk_image_menu_item_new_with_label(_("窗口歌词 | 独占主窗体"));
    gtk_widget_show (menu_INTEGRATION);
    image = gtk_image_new_from_stock ("gtk-indent", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_INTEGRATION), image);

    menu_HPANED =gtk_image_menu_item_new_with_label(_("窗口歌词 | 集成入主窗体"));
    gtk_widget_show (menu_HPANED);
    image = gtk_image_new_from_stock ("gtk-indent", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_HPANED), image);

    gtk_container_add(GTK_CONTAINER(lrc_item_menu),menu_INDEPENDENT);
    gtk_container_add(GTK_CONTAINER(lrc_item_menu),menu_HPANED);
    gtk_container_add(GTK_CONTAINER(lrc_item_menu),menu_INTEGRATION);

    g_signal_connect((gpointer) menu_INDEPENDENT ,"activate" ,G_CALLBACK(change_widget_lrc_ui),(gpointer *)WIDGET_LRC_SHOW_TYPE_INDEPENDENT);
    g_signal_connect((gpointer) menu_HPANED ,"activate" ,G_CALLBACK(change_widget_lrc_ui),(gpointer *)WIDGET_LRC_SHOW_TYPE_HPANED);
    g_signal_connect((gpointer) menu_INTEGRATION ,"activate" ,G_CALLBACK(change_widget_lrc_ui),(gpointer *)WIDGET_LRC_SHOW_TYPE_INTEGRATION);

    if (configure.lrc) {

        separator_5 = gtk_separator_menu_item_new ();
        gtk_widget_show (separator_5);
        gtk_container_add (GTK_CONTAINER (lrc_item_menu), separator_5);
        gtk_widget_set_sensitive (separator_5, FALSE);

        if (!configure.desktop_lrc) show_desktop_lrc_item = gtk_image_menu_item_new_with_label (_("显示桌面歌词"));
        else  show_desktop_lrc_item = gtk_image_menu_item_new_with_label (_("关闭桌面歌词"));
        gtk_widget_show (show_desktop_lrc_item);
        gtk_container_add (GTK_CONTAINER (lrc_item_menu), show_desktop_lrc_item);

        if (!configure.desktop_lrc)image = gtk_image_new_from_stock (GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
        else image = gtk_image_new_from_stock (GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
        gtk_widget_show (image);
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (show_desktop_lrc_item), image);

        if (configure.desktop_lrc) {
            if (!configure.desktop_lrc_lock) lock_desktop_lrc_item = gtk_image_menu_item_new_with_label (_("锁定桌面歌词"));
            else  lock_desktop_lrc_item = gtk_image_menu_item_new_with_label (_("解锁桌面歌词"));
            gtk_widget_show (lock_desktop_lrc_item);
            gtk_container_add (GTK_CONTAINER (lrc_item_menu), lock_desktop_lrc_item);

            image = gtk_image_new_from_stock ("gtk-connect", GTK_ICON_SIZE_MENU);
            gtk_widget_show (image);
            gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lock_desktop_lrc_item), image);
        }

        edit_lrc_item = gtk_image_menu_item_new_with_label (_("编辑歌词..."));
        gtk_widget_show (edit_lrc_item);
        //gtk_container_add (GTK_CONTAINER (lrc_item_menu), edit_lrc_item);

        image = gtk_image_new_from_stock ("gtk-edit", GTK_ICON_SIZE_MENU);
        gtk_widget_show (image);
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (edit_lrc_item), image);

    }

    separator_6 = gtk_separator_menu_item_new ();
    gtk_widget_show (separator_6);
    //gtk_container_add (GTK_CONTAINER (list_menu), separator_6);
    gtk_widget_set_sensitive (separator_6, FALSE);

    play_mode_item = gtk_image_menu_item_new_with_label (_("模式"));
    gtk_widget_show (play_mode_item);
    gtk_container_add (GTK_CONTAINER (list_menu), play_mode_item);

    image = gtk_image_new_from_stock ("gtk-go-forward", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_mode_item), image);

    play_mode_item_menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (play_mode_item), play_mode_item_menu);

    play_mode_normal_item = gtk_image_menu_item_new_with_label (_("顺序播放"));
    gtk_widget_show (play_mode_normal_item);
    gtk_container_add (GTK_CONTAINER (play_mode_item_menu), play_mode_normal_item);

    image = gtk_image_new_from_stock ("gtk-goto-bottom", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_mode_normal_item), image);

    play_mode_unnormal_item = gtk_image_menu_item_new_with_label (_("逆序播放"));
    gtk_widget_show (play_mode_unnormal_item);
    gtk_container_add (GTK_CONTAINER (play_mode_item_menu), play_mode_unnormal_item);

    image = gtk_image_new_from_stock ("gtk-goto-top", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_mode_unnormal_item), image);

    play_mode_radom_item = gtk_image_menu_item_new_with_label (_("随机播放"));
    gtk_widget_show (play_mode_radom_item);
    gtk_container_add (GTK_CONTAINER (play_mode_item_menu), play_mode_radom_item);

    image = gtk_image_new_from_stock ("gtk-indent", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_mode_radom_item), image);

    play_mode_one_item = gtk_image_menu_item_new_with_label (_("单曲播放"));
    gtk_widget_show (play_mode_one_item);
    gtk_container_add (GTK_CONTAINER (play_mode_item_menu), play_mode_one_item);

    image = gtk_image_new_from_stock ("gtk-refresh", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_mode_one_item), image);

    separator_7 = gtk_separator_menu_item_new ();
    gtk_widget_show (separator_7);
    gtk_container_add (GTK_CONTAINER (play_mode_item_menu), separator_7);
    gtk_widget_set_sensitive (separator_7, FALSE);

    play_mode_other_lists_item = gtk_image_menu_item_new_with_label (_("跨列表?"));
    gtk_widget_show (play_mode_other_lists_item);
    gtk_container_add (GTK_CONTAINER (play_mode_item_menu), play_mode_other_lists_item);

    switch (configure.play_mode) {
    case PLAY_MODE_NORMAL:
        gtk_label_set_markup ((GtkLabel *)GTK_BIN (play_mode_normal_item)->child, g_markup_printf_escaped ("<span weight=\"bold\">%s</span>",gtk_label_get_text (GTK_LABEL (GTK_BIN (play_mode_normal_item)->child))));
        break;
    case PLAY_MODE_UNORMAL:
        gtk_label_set_markup ((GtkLabel *)GTK_BIN (play_mode_unnormal_item)->child, g_markup_printf_escaped ("<span weight=\"bold\">%s</span>",gtk_label_get_text (GTK_LABEL (GTK_BIN (play_mode_unnormal_item)->child))));
        break;
    case PLAY_MODE_RADOM:
        gtk_label_set_markup ((GtkLabel *)GTK_BIN (play_mode_radom_item)->child, g_markup_printf_escaped ("<span weight=\"bold\">%s</span>",gtk_label_get_text (GTK_LABEL (GTK_BIN (play_mode_radom_item)->child))));
        break;
    case PLAY_MODE_ONE:
        gtk_label_set_markup ((GtkLabel *)GTK_BIN (play_mode_one_item)->child, g_markup_printf_escaped ("<span weight=\"bold\">%s</span>",gtk_label_get_text (GTK_LABEL (GTK_BIN (play_mode_one_item)->child))));
        break;
    }

    if (!configure.auto_jump_other_lists)image = gtk_image_new_from_stock ("gtk-no", GTK_ICON_SIZE_MENU);
    else image = gtk_image_new_from_stock ("gtk-yes", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_mode_other_lists_item), image);

    GtkWidget *menu_configure =gtk_image_menu_item_new_with_label(_("配置.."));

    image = gtk_image_new_from_stock (GTK_STOCK_PROPERTIES, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_configure), image);

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

    GtkWidget *menu_about =gtk_image_menu_item_new_with_label(_("关于"));
    image = gtk_image_new_from_stock ("gtk-about", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_about), image);

    separator_4 = gtk_separator_menu_item_new ();
    gtk_widget_show (separator_4);
    gtk_container_add (GTK_CONTAINER (list_menu), separator_4);
    gtk_widget_set_sensitive (separator_4, FALSE);

    gtk_menu_shell_append(GTK_MENU_SHELL(list_menu),menu_configure);
    gtk_menu_shell_append(GTK_MENU_SHELL(list_menu),menu_update);
    gtk_menu_shell_append(GTK_MENU_SHELL(list_menu),menu_web);
    gtk_menu_shell_append(GTK_MENU_SHELL(list_menu),menu_help);
    gtk_menu_shell_append(GTK_MENU_SHELL(list_menu),menu_about);

    inline void jump_configure() {
        show();
        gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),SWITCH_CONFIG);
        gtk_notebook_set_current_page(GTK_NOTEBOOK(configure_widget.notebook),CONFIG_SW_NORMAL);
        configure.show_page=SWITCH_CONFIG;
    }

    // g_signal_connect ((gpointer) jump_to_playing_song_item, "activate",G_CALLBACK (list_menu_jump_to_playing_song),NULL);
    g_signal_connect ((gpointer) mini_item, "activate",G_CALLBACK (mini),NULL);
    g_signal_connect ((gpointer) equalizer_item, "activate",G_CALLBACK (create_equalizer),NULL);
    g_signal_connect ((gpointer) switch_to_lrc_iem, "activate",G_CALLBACK (list_menu_switch_to_lrc),NULL);
    inline void close_lrc() {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_lrc_checkbutton),!configure.lrc);
        update_configure();
        lrc();
    }
    g_signal_connect ((gpointer) show_lrc_item, "activate",G_CALLBACK (close_lrc),NULL);
    if (configure.lrc) {
        g_signal_connect ((gpointer) show_desktop_lrc_item, "activate",G_CALLBACK (list_menu_show_desktop_lrc),NULL);
        if (configure.desktop_lrc) g_signal_connect ((gpointer) lock_desktop_lrc_item, "activate",G_CALLBACK (list_menu_lock_desktop_lrc),NULL);
    }

    g_signal_connect ((gpointer) play_mode_normal_item, "activate",G_CALLBACK (list_menu_play_mode),(gpointer *)PLAY_MODE_NORMAL);
    g_signal_connect ((gpointer) play_mode_unnormal_item, "activate",G_CALLBACK (list_menu_play_mode),(gpointer *)PLAY_MODE_UNORMAL);
    g_signal_connect ((gpointer) play_mode_radom_item, "activate",G_CALLBACK (list_menu_play_mode),(gpointer *)PLAY_MODE_RADOM);
    g_signal_connect ((gpointer) play_mode_one_item, "activate",G_CALLBACK (list_menu_play_mode),(gpointer *)PLAY_MODE_ONE);
    g_signal_connect ((gpointer) play_mode_other_lists_item, "activate",G_CALLBACK (list_menu_change_play_mode_other_lists),NULL);
    g_signal_connect((gpointer) menu_configure ,"activate" ,G_CALLBACK(jump_configure),NULL);
    inline void go_to_iceplayer_forum() { //注意要使用多线程检测更新
        gtk_show_uri(NULL,"http://www.iceplayer.org", 0,NULL);
    }
    g_signal_connect((gpointer) menu_web ,"activate" ,G_CALLBACK(go_to_iceplayer_forum),NULL);
    inline void jump_help() {
        show();
        gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),SWITCH_HELP);
        configure.show_page=SWITCH_HELP;
    }
    g_signal_connect((gpointer) menu_help ,"activate" ,G_CALLBACK(jump_help),NULL);
    g_signal_connect((gpointer) menu_about ,"activate" ,G_CALLBACK(show_about_dialog),NULL);

    gtk_widget_show_all(list_menu);
    popup_menu(list_menu,2,2);

    return FALSE;

}
///list_menu:
/*


下面都是在歌曲栏和多列表栏右键的菜单的显示和实现


*/
//跳到正在播放歌曲
gboolean  list_menu_jump_to_playing_song(gpointer data) {
    print_programming(("list_menu_jump_to_playing_song\n"));
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return FALSE;
    }

    if (playing_song[0]==0) {
        print_error("没有在播放音乐！\n");
        return FALSE;
    }
    gint a=0;
    iceplayer_lists *lists = &main_lists;
    if (!lists->next&&lists->name[0])
        strcpy(playing_list,lists->name);
    while (strcmp(lists->name,playing_list)) {
        a++;
        lists=lists->next;
        if (lists==NULL) {
            print_error("找不到对应的列表！\n");
            return FALSE;
        }
    }
    if (strcmp(focus_lists,playing_list)) {
        show_lists(playing_list);
        strcpy(focus_lists,playing_list);
        show_songs(NULL);
    }
    GtkTreePath *  path1 = gtk_tree_path_new_from_indices(a,-1);
    GtkTreePath *  path2 = gtk_tree_path_new_from_string(playing_song);
    gtk_tree_view_set_cursor((GtkTreeView *) iceplayer.list_lists_treeview,path1, gtk_tree_view_get_column((GtkTreeView *) iceplayer.list_lists_treeview,0), FALSE);
    gtk_tree_view_set_cursor((GtkTreeView *) iceplayer.list_songs_treeview,path2, gtk_tree_view_get_column((GtkTreeView *) iceplayer.list_songs_treeview,0), FALSE);
    gtk_tree_path_free(path1);
    gtk_tree_path_free(path2);
    g_print("Debug:跳到%s项\n",playing_song);

    return FALSE;

}
void list_menu_play_selection() {
    print_programming(("list_menu_play_selection\n"));

    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    update_focus_song_from_selection();
    if (!focus_song[0]) {
        print_error("没有选中项!\n");
        return ;
    }

    show_songs(focus_song);
    ///这么复杂只是想实现：正在播反A列表，A列表高亮，点击B列表，播放B列表内歌曲，然后实现<取消掉原A的高亮，B高亮>的效果。
    gchar *a= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(a,focus_lists);
    strcpy(focus_lists,playing_list);
    strcpy(playing_list,a);
    show_lists(focus_lists);
    strcpy(playing_list,a);
    strcpy(focus_lists,a);
    g_free_n(a);
    show_lists(NULL);
    ///

    gchar *dir= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(dir,get_skin_dir());

    gtk_container_remove_new(GTK_CONTAINER(iceplayer.play_eventbox),iceplayer.play_image);
    iceplayer.play_image = gtk_image_new_from_file_new(dir,"play");
    g_free_n(dir);
    gtk_widget_show(iceplayer.play_image);
    gtk_container_add(GTK_CONTAINER(iceplayer.play_eventbox),iceplayer.play_image);

    play_uri();

}
void list_menu_rename() {
    print_programming(("list_menu_rename\n"));
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    //update_focus_song_from_selection();
    if (!focus_song[0]) {
        print_error("没有选中项!\n");
        return ;
    }
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *value=NULL;
    GList *list_select = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_songs_treeview))),&model);
    if (list_select){
        if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath *) list_select->data)) {
            gtk_tree_model_get(model, &iter, 0, &value, -1);
        } else {
            print_error("未知错误，在重命名中无法读取原名");
            dialog("未知错误，在重命名中无法读取原名");
            return ;
        }
    }
    if (strncmp(value,"<cue>",5)==0) {
        if (value)g_free_n(value);
        dialog(_("暂不支持修改cue文件，请手动修改"));
        return ;
    }
    gchar *entry_text= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    gchar *entry_org= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    gchar *entry_title= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

    strcpy(entry_title,_("如果存在歌词，将同时重命名歌词"));
    strcpy( entry_text,g_basename(value));
    strcpy( entry_org,g_basename(value));
    while (1) {
        GtkWidget *input_dialog;
        GtkWidget *dialog_vbox;
        GtkWidget *entry;
        GtkWidget *dialog_action_area;
        GtkWidget *cancelbutton;
        GtkWidget *okbutton;

        input_dialog = gtk_dialog_new ();
        gtk_window_set_title (GTK_WINDOW (input_dialog), entry_title);
        gtk_window_set_position (GTK_WINDOW (input_dialog), GTK_WIN_POS_CENTER_ON_PARENT);
        //gtk_window_set_resizable (GTK_WINDOW (input_dialog), FALSE);
        gtk_window_set_type_hint (GTK_WINDOW (input_dialog), GDK_WINDOW_TYPE_HINT_DIALOG);
        gtk_window_resize(GTK_WINDOW(input_dialog),500,50);

        dialog_vbox = GTK_DIALOG (input_dialog)->vbox;
        gtk_widget_show (dialog_vbox);

        entry = gtk_entry_new ();
        gtk_widget_show (entry);
        gtk_box_pack_start (GTK_BOX (dialog_vbox), entry, FALSE, FALSE, 0);
        gtk_entry_set_text(GTK_ENTRY(entry),entry_text);

        dialog_action_area = GTK_DIALOG (input_dialog)->action_area;
        gtk_widget_show (dialog_action_area);
        gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area), GTK_BUTTONBOX_END);

        cancelbutton = gtk_button_new_from_stock ("gtk-cancel");
        gtk_widget_show (cancelbutton);
        gtk_dialog_add_action_widget (GTK_DIALOG (input_dialog), cancelbutton, GTK_RESPONSE_CANCEL);
        GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

        okbutton = gtk_button_new_from_stock ("gtk-ok");
        gtk_widget_show (okbutton);
        gtk_dialog_add_action_widget (GTK_DIALOG (input_dialog), okbutton, GTK_RESPONSE_OK);
        GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);

        GtkAccelGroup *gag = gtk_accel_group_new();
        gtk_widget_add_accelerator(okbutton,"clicked",gag,GDK_Return,0,GTK_ACCEL_VISIBLE);
        gtk_window_add_accel_group(GTK_WINDOW(input_dialog),gag);

        gint a = gtk_dialog_run(GTK_DIALOG(input_dialog));

        if (a==-5&&gtk_entry_get_text(GTK_ENTRY(entry))!=NULL) {
            if (strcmp(gtk_entry_get_text(GTK_ENTRY(entry)),"")&&strcmp(gtk_entry_get_text(GTK_ENTRY(entry)),entry_org)) {
                strcpy(entry_text,gtk_entry_get_text(GTK_ENTRY(entry)));
                strcpy(entry_title,rename_songs(value,(gchar *)gtk_entry_get_text(GTK_ENTRY(entry))));
                if (strcmp(_("成功"),entry_title)==0) {
                    gtk_widget_destroy(input_dialog);
                    break ;
                } else {
                    gtk_widget_destroy(input_dialog);
                    continue ;
                }
            }
        }
        gtk_widget_destroy(input_dialog);
        break;
    }
    if (value)g_free_n(value);
    g_free_n(entry_text);
    g_free_n(entry_org);
    g_free_n(entry_title);
}
void list_menu_input_lrc() {
    print_programming(("list_menu_rename\n"));
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    //update_focus_song_from_selection();
    if (!focus_song[0]) {
        print_error("没有选中项!\n");
        return ;
    }
    GtkTreeModel *model;
    GtkTreeIter iter;
    gchar *value=NULL;
    GList *list_select = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_songs_treeview))),&model);
    if (list_select)
    {
        if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath *) list_select->data)) {
            gtk_tree_model_get(model, &iter, 0, &value, -1);
        } else {
            print_error("未知错误，在重命名中无法读取原名");
            dialog("未知错误，在重命名中无法读取原名");
            return ;
        }
    }

    GtkWidget *filechooserdialog;
    GtkWidget *dialog_vbox;
    GtkWidget *hbox1;
    GtkWidget *dialog_action_area;
    GtkWidget *button1;
    GtkWidget *button2;

    filechooserdialog = gtk_file_chooser_dialog_new("", NULL,GTK_FILE_CHOOSER_ACTION_OPEN, NULL,NULL);
    gtk_container_set_border_width(GTK_CONTAINER(filechooserdialog), 5);
    gtk_window_set_position(GTK_WINDOW(filechooserdialog), GTK_WIN_POS_CENTER);
    gtk_window_set_type_hint(GTK_WINDOW(filechooserdialog), GDK_WINDOW_TYPE_HINT_DIALOG);
    GdkPixbuf *IcePlayer_icon_pixbuf=gdk_pixbuf_new_from_file(ICON, NULL);
    gtk_window_set_icon(GTK_WINDOW(filechooserdialog),(GdkPixbuf *)IcePlayer_icon_pixbuf);
    gtk_window_set_keep_above     (GTK_WINDOW (filechooserdialog),TRUE);
    dialog_vbox = GTK_DIALOG(filechooserdialog)->vbox;
    gtk_widget_show(dialog_vbox);
    hbox1 = gtk_hbox_new(FALSE, 0);
    gtk_widget_show(hbox1);
    gtk_box_pack_start(GTK_BOX(dialog_vbox), hbox1, FALSE, FALSE, 0);

    dialog_action_area = GTK_DIALOG(filechooserdialog)->action_area;
    gtk_widget_show(dialog_action_area);
    gtk_button_box_set_layout(GTK_BUTTON_BOX(dialog_action_area), GTK_BUTTONBOX_END);
    button1 = gtk_button_new_from_stock("gtk-cancel");
    gtk_widget_show(button1);
    gtk_dialog_add_action_widget(GTK_DIALOG(filechooserdialog), button1, GTK_RESPONSE_CANCEL);
    GTK_WIDGET_SET_FLAGS(button1, GTK_CAN_DEFAULT);
    button2 = gtk_button_new_from_stock("gtk-open");
    gtk_widget_show(button2);
    gtk_dialog_add_action_widget(GTK_DIALOG(filechooserdialog), button2, GTK_RESPONSE_OK);
    GTK_WIDGET_SET_FLAGS(button2, GTK_CAN_DEFAULT);
    gtk_widget_grab_default(button2);
    GtkFileFilter* filter = gtk_file_filter_new();
    gtk_file_filter_set_name(filter, _("歌词文件"));
    gtk_file_filter_add_pattern(filter,"*.[Ll][Rr][Cc]");
    gtk_file_chooser_add_filter(GTK_FILE_CHOOSER(filechooserdialog),filter);

    static gchar * path_used;
    /*下次打开对话框时便回到上一文件夹*/
    if (path_used)gtk_file_chooser_set_current_folder((GtkFileChooser *) filechooserdialog,path_used);

    gtk_window_set_focus ((GtkWindow *)filechooserdialog,button2);

    gint aw = gtk_dialog_run((GtkDialog *) filechooserdialog);

    path_used = gtk_file_chooser_get_current_folder((GtkFileChooser *) filechooserdialog);

    if (aw==-5) {
        gchar *path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        strcpy(path,"cp ");
        strcat(path,"\"");
        strcat(path , gtk_file_chooser_get_filename((GtkFileChooser *) filechooserdialog));
        strcat(path,"\" \"");
        // strcat(path ,value);

        gchar *path2= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        if (configure.download_lrc_path[0]=='/') {
            if (configure.download_lrc_path[strlen(configure.download_lrc_path)-1]!='/')
                strcat(configure.download_lrc_path,"/");
            strcpy(path2,configure.download_lrc_path);
        } else {
            if (!strncmp(focus_song,"<cue>",5)) {
                //cue:
                gchar title[STRINGS_LENGTH];
                gchar performer[STRINGS_LENGTH];
                gchar cue_uri[STRINGS_LENGTH];

                strcpy(performer,strstr(focus_song,"PERFORMER:"));
                sscanf(performer,"PERFORMER:%[^>]%*s",performer);
                strcpy(title,strstr(focus_song,"TITLE:"));
                sscanf(title,"TITLE:%[^>]%*s",title);
                strcpy(cue_uri,strstr(focus_song,"cue_uri:"));
                sscanf(cue_uri,"cue_uri:%[^>]",cue_uri);

                strcpy(path2,g_path_get_dirname(cue_uri));
                strcat(path2,"/");
            } else {
                strcpy(path2,g_path_get_dirname(focus_song));
                strcat(path2,"/");
            }

            gchar *playing_song_lrc_path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

            if (!strncmp(focus_song,"<cue>",5)) {
                //cue:
                gchar title[STRINGS_LENGTH];
                gchar performer[STRINGS_LENGTH];
                //gchar cue_uri[STRINGS_LENGTH];

                strcpy(performer,strstr(focus_song,"PERFORMER:"));
                sscanf(performer,"PERFORMER:%[^>]",performer);
                gint n=0;
                while (performer[n]!='>')n++;
                performer[n]=0;

                strcpy(title,strstr(focus_song,"TITLE:"));
                sscanf(title,"TITLE:%[^>]",title);
                n=0;
                while (title[n]!='>')n++;
                title[n]=0;
                sprintf(playing_song_lrc_path,"%s - %s.lrc",performer,title);
            } else {
                gchar *path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
                if (get_playing_song()==NULL)
                    strcpy(path,find_lists_from_name(focus_lists)->songs->uri);
                else
                    strcpy(path,get_playing_song());
                print_programming("get_lrc_file_path_at_custom_path\n");
                static gchar playing_song_path[STRINGS_LENGTH];

                strcpy(playing_song_path,g_path_get_basename(path));
                g_free_n(path);
                if (playing_song_path==NULL) {
                    print_error("没有在播放音乐，无法播放窗口歌词\n");
                    g_free_n(playing_song_lrc_path);
                    return ;
                }
                strcpy(playing_song_lrc_path,playing_song_path_without_some_word(playing_song_path));
                gint n=strlen(playing_song_lrc_path);
                while (1) {
                    if (playing_song_lrc_path[n]=='.')
                        break;
                    n--;
                    if (n<=0) {
                        n=strlen(playing_song_lrc_path);
                        break;
                    }
                }
                playing_song_lrc_path[n+1]=0;
                strcat(playing_song_lrc_path,"lrc");

            }
            strcat(path,path2);
            strcat( path,playing_song_lrc_path);
            strcat(path2,playing_song_lrc_path);
            //gint a=0,b=0;

            strcat(path,"\"");
            print_debug(path);
            system(path);
            g_free_n(path);
            lrc_file_to_lrc_text(path2);
            g_free_n(path2);
            g_free_n(playing_song_lrc_path);
        }
        gtk_widget_destroy(filechooserdialog);
    }
}
void list_menu_open_dir_of_song() {
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    gchar *path = get_selection_song();
    if (path==NULL) {
        print_error("没有选中项!\n");
        return;
    }
    gchar q[2000];
    FILE *file_man = fopen("/usr/bin/nautilus","r");
    if (file_man) {
        if (file_man) {
            fclose(file_man);
            file_man=0;
        }
        strcpy(q,"nautilus \"");
    } else {
        file_man = fopen("/usr/bin/dolphin","r");
        if (file_man) {
            if (file_man) {
                fclose(file_man);
                file_man=0;
            }
            strcpy(q,"dolphin \"");
        } else {
            file_man = fopen("/usr/bin/thunar","r");
            if (file_man) {
                if (file_man) {
                    fclose(file_man);
                    file_man=0;
                }
                strcpy(q,"thunar \"");
            } else {
                GtkWidget *dialog_real = gtk_message_dialog_new_with_markup(NULL,GTK_DIALOG_DESTROY_WITH_PARENT    ,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,"iceplayer \n你没有nautilus就算了，没dolphin我原谅你，居然最后连thunar都没有。。。你用的是ubuntu吗？");
                gtk_dialog_run((GtkDialog *)dialog_real);
                gtk_widget_destroy(dialog_real);
                return ;
            }
        }
    }
    if (strstr(path,"<cue>")!=NULL) {
        //cue:
        strcpy(path,strstr(path,"<cue_uri:"));
        sscanf(path,"<cue_uri:%s",path);
        gint n=0;
        while (path[n]!='>')n++;
        path[n]=0;
    }
    strcat (q,path);
    gint a=strlen(q);
    while (q[a]!='/') {
        a--;
    }
    q[a] = 0;
    strcat (q,"\"");
    if (system(q));
    return;
}
void list_menu_man_tools (GtkMenuItem *menuitem,gpointer user_data) {
}
void list_menu_open_with_audacious () {
    print_programming(("list_menu_open_with_audacious\n"));
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    gchar *path = get_selection_song();
    if (path==NULL) {
        print_error("没有选中项!\n");
        return;
    }
    if (strstr(path,"<cue>")!=NULL) {
        //cue:
        gchar *te= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        strcpy(path,strstr(path,"<cue_uri:"));
        sscanf(path,"<cue_uri:%s",path);
        gint n=0;
        while (path[n]!='>')n++;
        path[n]=0;

        sprintf(te,"iceplayer \n文件《 %s》属于CUE文件，不保证所选外部程序可以打开！",path);
        GtkWidget *dialog_real = gtk_message_dialog_new_with_markup(NULL,GTK_DIALOG_DESTROY_WITH_PARENT    ,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,te);
        gtk_dialog_run((GtkDialog *)dialog_real);
        gtk_widget_destroy(dialog_real);
        g_free_n(te);
    }

    gchar q[2000];
    FILE *file_man = fopen("/usr/bin/audacious","r");
    if (file_man) {
        if (file_man) {
            fclose(file_man);
            file_man=0;
        }

    } else {
        GtkWidget *dialog_real = gtk_message_dialog_new_with_markup(NULL,GTK_DIALOG_DESTROY_WITH_PARENT    ,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,_("iceplayer \n不存在Audacious (/usr/bin/audacious)？"));
        gtk_dialog_run((GtkDialog *)dialog_real);
        gtk_widget_destroy(dialog_real);
        return ;
    }

    pthread_t id;
    strcpy(q,"audacious --enqueue-to-temp \"");
    strcat (q,path);
    strcat (q,"\"");
    pthread_create(&id,NULL,(void *) system,q);
    return;
}
void list_menu_open_with_amarok_item (GtkMenuItem *menuitem,gpointer user_data) {
    print_programming(("list_menu_open_with_amarok_item\n"));
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    gchar *path = get_selection_song();
    if (path==NULL) {
        print_error("没有选中项!\n");
        return;
    }
    if (strstr(path,"<cue>")!=NULL) {
        //cue:
        gchar *te = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        strcpy(path,strstr(path,"<cue_uri:"));
        sscanf(path,"<cue_uri:%s",path);
        gint n=0;
        while (path[n]!='>')n++;
        path[n]=0;

        sprintf(te,"iceplayer \n文件《 %s》属于CUE文件，不保证所选外部程序可以打开！",path);
        GtkWidget *dialog_real = gtk_message_dialog_new_with_markup(NULL,GTK_DIALOG_DESTROY_WITH_PARENT    ,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,te);
        gtk_dialog_run((GtkDialog *)dialog_real);
        gtk_widget_destroy(dialog_real);
        g_free_n(te);
    }
    gchar q[2000];
    FILE *file_man = fopen("/usr/bin/amarok","r");
    if (file_man) {
        if (file_man) {
            fclose(file_man);
            file_man=0;
        }

    } else {
        GtkWidget *dialog_real = gtk_message_dialog_new_with_markup(NULL,GTK_DIALOG_DESTROY_WITH_PARENT    ,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,"iceplayer \n不存在amarok (/usr/bin/amarok)？");
        gtk_dialog_run((GtkDialog *)dialog_real);
        gtk_widget_destroy(dialog_real);
        return ;
    }

    pthread_t id;
    strcpy(q,"amarok -p --append \"");
    strcat (q,path);
    strcat (q,"\"");
    pthread_create(&id,NULL,(void *) system,q);
    return;
}
void list_menu_open_with_totem (GtkMenuItem *menuitem,gpointer user_data) {
    print_programming(("list_menu_open_with_totem\n"));
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    gchar *path = get_selection_song();
    if (path==NULL) {
        print_error("没有选中项!\n");
        return;
    }
    if (strstr(path,"<cue>")!=NULL) {
        //cue:
        gchar *te= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));;
        strcpy(path,strstr(path,"<cue_uri:"));
        sscanf(path,"<cue_uri:%s",path);
        gint n=0;
        while (path[n]!='>')n++;
        path[n]=0;

        sprintf(te,"iceplayer \n文件《 %s》属于CUE文件，不保证所选外部程序可以打开！",path);
        GtkWidget *dialog_real = gtk_message_dialog_new_with_markup(NULL,GTK_DIALOG_DESTROY_WITH_PARENT    ,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,te);
        gtk_dialog_run((GtkDialog *)dialog_real);
        gtk_widget_destroy(dialog_real);
        g_free_n(te);
    }
    gchar q[2000];
    FILE *file_man = fopen("/usr/bin/totem","r");
    if (file_man) {
        if (file_man) {
            fclose(file_man);
            file_man=0;
        }

    } else {
        GtkWidget *dialog_real = gtk_message_dialog_new_with_markup(NULL,GTK_DIALOG_DESTROY_WITH_PARENT    ,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,"iceplayer \n不存在totem (/usr/bin/totem)？");
        gtk_dialog_run((GtkDialog *)dialog_real);
        gtk_widget_destroy(dialog_real);
        return ;
    }

    pthread_t id;
    strcpy(q,"totem --play --replace --enqueue \"");
    strcat (q,path);
    strcat (q,"\"");
    pthread_create(&id,NULL,(void *) system,q);
    return;
}
void list_menu_deletet_selection (GtkMenuItem *menuitem,gboolean delete_to_trash) {
    print_programming(("list_menu_deletet_selection\n"));
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    GtkTreeModel *model;
    //GtkTreeIter iter;
    GList *list_select = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_songs_treeview))),&model);
    if (!list_select) {
        print_error("没有选中项!\n");
        return ;
    }
    iceplayer_songs *songs = find_lists_from_name(focus_lists)->songs;
    gint  indices=atoi(gtk_tree_path_to_string((GtkTreePath *) list_select->data));
    gint  indices_tmp;
    gboolean inc_top = FALSE;
    while (list_select) {
        if (!strcmp( focus_lists,playing_list))
            if (!strcmp(gtk_tree_path_to_string((GtkTreePath *) list_select->data),playing_song)) {
                stop();
                playing_list[0]=0;
                playing_song[0]=0;
            }
        if (atoi(gtk_tree_path_to_string((GtkTreePath *) list_select->data))==0||inc_top) {
            inc_top = TRUE;
            if (songs->next) {
                if (delete_to_trash) {
                    gchar *a= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
                    strcpy(a,songs->uri);
                    g_file_trash(g_file_new_for_path(a),NULL,NULL);
                    g_free_n(a);
                }
                find_lists_from_name(focus_lists)->songs=songs->next;
                songs = find_lists_from_name(focus_lists)->songs;
            } else songs->uri[0]=0;
        } else {
            indices_tmp = atoi(gtk_tree_path_to_string((GtkTreePath *) list_select->data))-indices;
            indices = indices_tmp+indices;
            if (indices_tmp)
                while ((indices_tmp-1)&&songs) {
                    songs=songs->next;
                    indices_tmp--;
                }
            else {
                indices_tmp = indices;
                while ((indices_tmp-1)&&songs) {
                    songs=songs->next;
                    indices_tmp--;
                }
            }

            if (songs->next) {
                if (delete_to_trash) {

                    gchar *a= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
                    strcpy(a,songs->next->uri);
                    g_file_trash(g_file_new_for_path(a),NULL,NULL);
                    g_free_n(a);
                }
                songs->next = songs->next->next;
            }
        }
        list_select = list_select ->next;
    }
    show_songs(NULL);
}
void list_menu_delete_focus_lists (GtkMenuItem *menuitem,gpointer user_data) {
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    if (focus_lists[0]) {
        find_lists_from_name(focus_lists)->songs=NULL;
        if (!strcmp(focus_lists,playing_list)) {
            stop();
            focus_song[0]=0;
            playing_song[0]=0;
        }
        show_songs(NULL);
    } else {
        print_error("没有播放项!\n");
        dialog(_("请新建或点选左边列表!"));
    }
}
void list_menu_deletet_same_item(GtkMenuItem *menuitem,gpointer user_data) {
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    if (focus_lists[0]) {
        iceplayer_songs *songs = find_lists_from_name(focus_lists)->songs;

        while (songs) {
            iceplayer_songs *songs_for_compare = find_lists_from_name(focus_lists)->songs;
            gint n=0;
            while (songs_for_compare) {
                if (strcmp(songs_for_compare->uri,songs->uri)==0) {
                    n++;
                    if (n>=2) {
                        if (songs->next) {
                            strcpy(songs->uri,songs->next->uri);
                            songs->next=songs->next->next;
                        } else {
                            songs->uri[0]=0;
                            songs->next = NULL;
                        }
                    }
                }
                songs_for_compare=songs_for_compare->next;
            }
            songs=songs->next;
        }
        show_songs(NULL);
    } else {
        print_error("没有播放项!\n");
        dialog(_("请新建或点选左边列表!"));
    }
}
void list_menu_delete_no_exist_item (GtkMenuItem *menuitem,gpointer user_data) {
    print_programming(("list_menu_delete_no_exist_item\n"));
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    if (!focus_lists[0])
        strcpy(focus_lists,main_lists.name);
    iceplayer_songs *songs = find_lists_from_name(focus_lists)->songs;

    while (songs&&songs->uri[0]) {
        gchar *path = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        if (strstr(songs->uri,"<cue>")!=NULL) {
            //cue:
            strcpy(path,strstr(songs->uri,"<cue_uri:"));
            sscanf(path,"<cue_uri:%[^>]",path);
        } else {
            strcpy(path,songs->uri);
        }
        FILE *file=fopen(path,"r");
        if (file) {
            fclose(file);
            songs= songs->next;
        } else {
            if (songs->next!=NULL) {
                if (songs->next->uri[0]) {
                    strcpy(songs->uri,songs->next->uri);
                    songs->next=songs->next->next;
                } else {
                    songs->uri[0]=0;
                    songs->next=NULL;
                }
            } else {
                songs->uri[0]=0;
                songs->next=NULL;
            }
        }
        g_free_n(path);
    }
    show_songs(NULL);
}
void list_menu_real_delete (GtkMenuItem *menuitem,gpointer user_data) {
    print_programming(("list_menu_real_delete\n"));
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    if (!focus_lists[0])
        strcpy(focus_lists,main_lists.name);
    iceplayer_songs *songs = find_lists_from_name(focus_lists)->songs;

    if (strstr(songs->uri,"<cue>")!=NULL) {
        //cue:
        gchar * path = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        gchar *te = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        strcpy(path,strstr(songs->uri,"<cue_uri:"));
        sscanf(path,"<cue_uri:%s",path);
        gint n=0;
        while (path[n]!='>')n++;
        path[n]=0;

        sprintf(te,"iceplayer \n文件《 %s》属于CUE文件，如果删除将删除整个cue的所有歌曲和连带的ape或者flac等，为了安全，请自己手动删除！",path);
        GtkWidget *dialog_real = gtk_message_dialog_new_with_markup(NULL,GTK_DIALOG_DESTROY_WITH_PARENT    ,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,te);
        gtk_dialog_run((GtkDialog *)dialog_real);
        gtk_widget_destroy(dialog_real);
        g_free_n(path);
        g_free_n(te);
    } else {
        list_menu_deletet_selection(NULL,TRUE);
    }
}
void list_menu_id3 (GtkMenuItem *menuitem,gpointer user_data) {
    id3_dialog();
}
void list_menu_switch_to_lrc (GtkMenuItem *menuitem,gpointer user_data) {
    show();
    gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),SWITCH_CONFIG);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(configure_widget.notebook),CONFIG_SW_LRC);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(configure_widget.lrc_notebook),0);
}
void list_menu_show_desktop_lrc(GtkMenuItem *menuitem,gpointer user_data) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_desktop_lrc_checkbutton),!configure.desktop_lrc);
    update_configure();
}
void list_menu_search_lrc (GtkMenuItem *menuitem,gpointer user_data) {
    if (configure.search_lrc) {
        search_lrc_pre();
    } else {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_search_lrc_checkbutton),!configure.search_lrc);
        update_configure();
    }
}
void list_menu_lock_desktop_lrc(GtkMenuItem *menuitem,gpointer user_data) {
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.desktop_lrc_lock_checkbutton),!configure.desktop_lrc_lock);
    update_configure();
}
void list_menu_edit_lrc (GtkMenuItem *menuitem,gpointer user_data) {
}
void list_menu_sort_songs (GtkMenuItem *menuitem,gpointer user_data) {
}
void list_menu_sort_with_file_name (GtkMenuItem *menuitem,gpointer user_data) {

    print_programming(("list_menu_sort_with_file_name\n"));
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    iceplayer_lists *lists= find_lists_from_name(focus_lists);
    iceplayer_songs *songs = lists->songs;

    if (!songs) {
        print_error("列表对应歌曲为空值！\n");
        return ;
    }

    gint m=0;
    gchar linger[STRINGS_LENGTH*3][STRINGS_LENGTH];
    gchar playing[STRINGS_LENGTH];
    gchar p[50];
    while (songs) {
        if (!songs->uri[0])break;
        strcpy(linger[m],songs->uri);
        m++;
        if (m>STRINGS_LENGTH*3) {
            dialog(_("错误，过多的歌曲，由于容易崩溃或者占用大量内存，暂不支持。"));
            return ;
        }
        sprintf(p,"%d",m-1);
        if (strcmp(p,playing_song)==0) {
            strcpy(playing,songs->uri);
        }
        songs = songs->next;
    }
    if (m<=0)
        return;

    gint b=0,n=0;
///*冒泡排序*/
    for (n=0; n<m-1; n++)
        for (b=0; b<m-n-1; b++)
            if (strcmp(songs_uri_get_show_name(linger[b]),songs_uri_get_show_name(linger[b+1]))>0) {
                gchar *text = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
                strcpy(text,linger[b]);
                strcpy(linger[b],linger[b+1]);
                strcpy(linger[b+1],text);
                if (strcmp(playing,text)==0) {
                    sprintf(playing_song,"%d",m-n-1);
                }
                g_free_n(text);
            }
    n=0;
    songs = lists->songs;
    while (linger[n][0]) {
        strcpy(songs->uri,linger[n++]);
        songs=songs->next;
    }
    show_songs(NULL);
    if (strcmp(focus_lists,playing_list)==0)list_menu_jump_to_playing_song(NULL);
}
void list_menu_un_sort_with_file_name (GtkMenuItem *menuitem,gpointer user_data) {

    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    iceplayer_lists *lists= find_lists_from_name(focus_lists);
    iceplayer_songs *songs = lists->songs;

    if (!songs) {
        print_error("列表对应歌曲为空值！\n");
        return ;
    }

    gint m=0;
    gchar linger[STRINGS_LENGTH*3][STRINGS_LENGTH];
    gchar playing[STRINGS_LENGTH];
    gchar p[50];
    while (songs) {
        if (!songs->uri[0])break;
        strcpy(linger[m],songs->uri);
        m++;
        if (m>STRINGS_LENGTH*3) {
            dialog(_("错误，过多的歌曲，由于容易崩溃或者占用大量内存，暂不支持。"));
            return ;
        }
        sprintf(p,"%d",m-1);
        if (strcmp(p,playing_song)==0) {
            strcpy(playing,songs->uri);
        }
        songs = songs->next;
    }
    if (m<=0)
        return;

    gint b=0,n=0;
///*冒泡排序*/
    for (n=0; n<m-1; n++)
        for (b=0; b<m-n-1; b++)
            if (strcmp(songs_uri_get_show_name(linger[b]),songs_uri_get_show_name(linger[b+1]))<0) {
                gchar *text = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
                strcpy(text,linger[b]);
                strcpy(linger[b],linger[b+1]);
                strcpy(linger[b+1],text);
                if (strcmp(playing,text)==0) {
                    sprintf(playing_song,"%d",m-n-1);
                }
                g_free_n(text);
            }
    n=0;
    songs = lists->songs;
    while (linger[n][0]) {
        strcpy(songs->uri,linger[n++]);
        songs=songs->next;
    }
    show_songs(NULL);
    list_menu_jump_to_playing_song(NULL);
}
void list_menu_sort_with_id3_editor (GtkMenuItem *menuitem,gpointer user_data) {
}
void list_menu_sort_with_id3_song_name (GtkMenuItem *menuitem,gpointer user_data) {
}
void list_menu_sort_with_radom(GtkMenuItem *menuitem,gpointer user_data) {

    print_programming(("list_menu_sort_with_radom\n"));
    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    iceplayer_lists *lists= find_lists_from_name(focus_lists);
    iceplayer_songs *songs = lists->songs;

    if (!songs) {
        print_error("列表对应歌曲为空值！\n");
        return ;
    }

    gint m=0;
    gchar playing[STRINGS_LENGTH];
    gchar p[50];
    gchar linger[STRINGS_LENGTH*2][STRINGS_LENGTH];

    while (songs) {
        if (!songs->uri[0])break;
        strcpy(linger[m],songs->uri);
        m++;
        if (m>STRINGS_LENGTH*2) {
            dialog(_("错误，过多的歌曲，由于容易崩溃或者占用大量内存，暂不支持。"));
            return ;
        }
        sprintf(p,"%d",m-1);
        if (strcmp(p,playing_song)==0) {
            strcpy(playing,songs->uri);
        }
        songs = songs->next;
    }
    if (m<=0)
        return;

    gint b=0,n=0;
///*冒泡排序*/
    srand((unsigned)time(NULL)+1);
    for (n=0; n<m-1; n++)
        for (b=0; b<m-n-1; b++)
            if (rand()%2) {
                gchar *text = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
                strcpy(text,linger[b]);
                strcpy(linger[b],linger[b+1]);
                strcpy(linger[b+1],text);
                g_free_n(text);
            }
    srand((unsigned)time(NULL)+1);
    for (n=0; n<m-1; n++)
        for (b=0; b<m-n-1; b++)
            if (rand()%2) {
                gchar *text = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
                strcpy(text,linger[b]);
                strcpy(linger[b],linger[b+1]);
                strcpy(linger[b+1],text);
                g_free_n(text);
            }
    srand((unsigned)time(NULL)+1);
    for (n=0; n<m-1; n++)
        for (b=0; b<m-n-1; b++)
            if (rand()%2) {
                gchar *text = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
                strcpy(text,linger[b]);
                strcpy(linger[b],linger[b+1]);
                strcpy(linger[b+1],text);
                g_free_n(text);
            }
    srand((unsigned)time(NULL)+1);
    for (n=0; n<m-1; n++)
        for (b=0; b<m-n-1; b++)
            if (rand()%2) {
                gchar *text = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
                strcpy(text,linger[b]);
                strcpy(linger[b],linger[b+1]);
                strcpy(linger[b+1],text);
                g_free_n(text);
            }
    n=0;
    songs = lists->songs;
    while (linger[n][0]) {
        strcpy(songs->uri,linger[n++]);
        if (strcmp(playing,songs->uri)==0) {
            sprintf(playing_song,"%d",n-1);
        }
        songs=songs->next;
    }
    show_songs(NULL);
    list_menu_jump_to_playing_song(NULL);
}
void list_menu_play_mode(GtkMenuItem *menuitem,gint type) {
    configure.play_mode = type;
}
void list_menu_change_play_mode_other_lists (GtkMenuItem *menuitem,gpointer user_data) {
    configure.auto_jump_other_lists = !configure.auto_jump_other_lists;
}
void list_menu_search_local_song (GtkMenuItem *menuitem,gpointer user_data) {
}
void list_menu_search_music_box(GtkMenuItem *menuitem,gpointer user_data) {

}
void show_list_menu () {
    print_programming("show_list_menu\n");
    GtkWidget *list_menu = NULL;
    GtkWidget *image = NULL;
    GtkWidget *jump_to_playing_song_item = NULL;
    GtkWidget *separator_1 = NULL;
    GtkWidget *play_selection_item = NULL;
    GtkWidget *deal_with_playing_song_item = NULL;
    GtkWidget *deal_with_playing_song_item_menu = NULL;
    GtkWidget *open_dir_of_song_item = NULL;
    GtkWidget *man_tools_item = NULL;
    GtkWidget *separator_2 = NULL;
    GtkWidget *rename_item = NULL;
    GtkWidget *input_lrc_item = NULL;
    GtkWidget *open_with_audacious_item = NULL;
    GtkWidget *open_with_amarok_item = NULL;
    GtkWidget *open_with_totem_item = NULL;
    GtkWidget *delete_item = NULL;
    GtkWidget *delete_item_menu = NULL;
    GtkWidget *deletet_selection_item = NULL;
    GtkWidget *deletet_same_item = NULL;
    GtkWidget *delete_focus_lists = NULL;
    GtkWidget *separator_3 = NULL;
    GtkWidget *delete_no_exist_item = NULL;
    GtkWidget *real_delete_item = NULL;
    GtkWidget *id3_item = NULL;
    GtkWidget *equalizer_item = NULL;
    GtkWidget *separator_4 = NULL;
    GtkWidget *lrc_item = NULL;
    GtkWidget *lrc_item_menu = NULL;
    GtkWidget *switch_to_lrc_iem = NULL;
    GtkWidget *show_desktop_lrc_item = NULL;
    //GtkWidget *search_lrc_item = NULL;
    //GtkWidget *search_lrc_with_custom_word_item = NULL;
    GtkWidget *separator_5 = NULL;
    GtkWidget *lock_desktop_lrc_item = NULL;
    GtkWidget *edit_lrc_item = NULL;
    GtkWidget *sort_songs = NULL;
    GtkWidget *sort_songs_menu = NULL;
    GtkWidget *sort_with_file_name_item = NULL;
    GtkWidget *un_sort_with_file_name_item = NULL;
    GtkWidget *sort_with_id3_editor_item = NULL;
    GtkWidget *sort_with_id3_song_name_item = NULL;
    GtkWidget *sort_with_radom_item = NULL;
    //GtkWidget *separator_6 = NULL;
    GtkWidget *play_mode_item = NULL;
    GtkWidget *play_mode_item_menu = NULL;
    GtkWidget *play_mode_normal_item = NULL;
    GtkWidget *play_mode_unnormal_item = NULL;
    GtkWidget *play_mode_radom_item = NULL;
    GtkWidget *play_mode_one_item = NULL;
    GtkWidget *separator_7 = NULL;
    GtkWidget *play_mode_other_lists_item = NULL;
    GtkWidget *search_item = NULL;
    GtkWidget *search_item_menu = NULL;
    GtkWidget *search_local_song_item = NULL;
	GtkWidget *search_music_box_item = NULL;
    GtkWidget *lists_add_files = NULL;
    GtkWidget *lists_add_folder = NULL;
    GtkWidget *show_lrc_item = NULL;

    list_menu = gtk_menu_new ();

    jump_to_playing_song_item = gtk_image_menu_item_new_with_label (_("跳到正在播放"));
    gtk_widget_show (jump_to_playing_song_item);
    gtk_container_add (GTK_CONTAINER (list_menu), jump_to_playing_song_item);

    image = gtk_image_new_from_stock ("gtk-jump-to", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (jump_to_playing_song_item), image);

    lists_add_files = gtk_image_menu_item_new_with_label (_("添加文件到此"));
    gtk_widget_show (lists_add_files);
    gtk_menu_append (GTK_MENU_SHELL (list_menu), lists_add_files);

    image = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lists_add_files), image);

    lists_add_folder = gtk_image_menu_item_new_with_label (_("添加文件夹到此"));
    gtk_widget_show (lists_add_folder);
    gtk_menu_append (GTK_MENU_SHELL (list_menu), lists_add_folder);

    image = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lists_add_folder), image);
    g_signal_connect ((gpointer) lists_add_files, "activate",G_CALLBACK(add_dialog),(gpointer)ADD_FILES);
    g_signal_connect ((gpointer) lists_add_folder, "activate",G_CALLBACK(add_dialog),(gpointer)ADD_FOLDER);


    separator_1 = gtk_separator_menu_item_new ();
    gtk_widget_show (separator_1);
    gtk_container_add (GTK_CONTAINER (list_menu), separator_1);
    gtk_widget_set_sensitive (separator_1, FALSE);

    play_selection_item = gtk_image_menu_item_new_with_label (_("播放选中项"));
    gtk_widget_show (play_selection_item);
    gtk_container_add (GTK_CONTAINER (list_menu), play_selection_item);

    image = gtk_image_new_from_stock ("gtk-media-play", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_selection_item), image);

    deal_with_playing_song_item = gtk_image_menu_item_new_with_label (_("处理选中项"));
    gtk_widget_show (deal_with_playing_song_item);
    gtk_container_add (GTK_CONTAINER (list_menu), deal_with_playing_song_item);

    image = gtk_image_new_from_stock ("gtk-execute", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (deal_with_playing_song_item), image);

    deal_with_playing_song_item_menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (deal_with_playing_song_item), deal_with_playing_song_item_menu);

    open_dir_of_song_item = gtk_image_menu_item_new_with_label (_("打开关联目录..."));
    gtk_widget_show (open_dir_of_song_item);
    gtk_container_add (GTK_CONTAINER (deal_with_playing_song_item_menu), open_dir_of_song_item);

    image = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (open_dir_of_song_item), image);

    man_tools_item = gtk_image_menu_item_new_with_label (_("管理外部工具..."));
    gtk_widget_show (man_tools_item);
    //gtk_container_add (GTK_CONTAINER (deal_with_playing_song_item_menu), man_tools_item);

    image = gtk_image_new_from_stock ("gtk-execute", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (man_tools_item), image);

    separator_2 = gtk_separator_menu_item_new ();
    gtk_widget_show (separator_2);
    gtk_container_add (GTK_CONTAINER (deal_with_playing_song_item_menu), separator_2);
    gtk_widget_set_sensitive (separator_2, FALSE);
//
    rename_item= gtk_image_menu_item_new_with_label (_("重命名此歌曲（多选则为所选的第一项）"));
    gtk_widget_show (rename_item);
    gtk_container_add (GTK_CONTAINER (deal_with_playing_song_item_menu), rename_item);

    image = gtk_image_new_from_icon_name("gtk-execute", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (rename_item), image);
//
    input_lrc_item= gtk_image_menu_item_new_with_label (_("导入外部歌词"));
    gtk_widget_show (input_lrc_item);
    gtk_container_add (GTK_CONTAINER (deal_with_playing_song_item_menu), input_lrc_item);

    image = gtk_image_new_from_icon_name("gtk-execute", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (input_lrc_item), image);

    open_with_audacious_item = gtk_image_menu_item_new_with_label ("Audacious");
    gtk_widget_show (open_with_audacious_item);
    gtk_container_add (GTK_CONTAINER (deal_with_playing_song_item_menu), open_with_audacious_item);

    image = gtk_image_new_from_icon_name("audacious", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (open_with_audacious_item), image);

    open_with_amarok_item = gtk_image_menu_item_new_with_label ("Amarok");
    gtk_widget_show (open_with_amarok_item);
    gtk_container_add (GTK_CONTAINER (deal_with_playing_song_item_menu), open_with_amarok_item);

    image = gtk_image_new_from_icon_name("amarok", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (open_with_amarok_item), image);

    open_with_totem_item = gtk_image_menu_item_new_with_label ("Totem");
    gtk_widget_show (open_with_totem_item);
    gtk_container_add (GTK_CONTAINER (deal_with_playing_song_item_menu), open_with_totem_item);

    image = gtk_image_new_from_icon_name("totem", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (open_with_totem_item), image);

    delete_item = gtk_image_menu_item_new_with_label (_("删除"));
    gtk_widget_show (delete_item);
    gtk_container_add (GTK_CONTAINER (list_menu), delete_item);

    image = gtk_image_new_from_stock ("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (delete_item), image);

    delete_item_menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (delete_item), delete_item_menu);

    deletet_selection_item = gtk_image_menu_item_new_with_label (_("删除选中项"));
    gtk_widget_show (deletet_selection_item);
    gtk_container_add (GTK_CONTAINER (delete_item_menu), deletet_selection_item);

    image = gtk_image_new_from_stock ("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (deletet_selection_item), image);

    delete_focus_lists = gtk_image_menu_item_new_with_label (_("清空整个列表"));
    gtk_widget_show (delete_focus_lists);
    gtk_container_add (GTK_CONTAINER (delete_item_menu), delete_focus_lists);

    image = gtk_image_new_from_stock ("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (delete_focus_lists), image);

    separator_3 = gtk_separator_menu_item_new ();
    gtk_widget_show (separator_3);
    gtk_container_add (GTK_CONTAINER (delete_item_menu), separator_3);
    gtk_widget_set_sensitive (separator_3, FALSE);


    deletet_same_item = gtk_image_menu_item_new_with_label (_("删除重复项"));
    gtk_widget_show (deletet_same_item);
    gtk_container_add (GTK_CONTAINER (delete_item_menu), deletet_same_item);

    image = gtk_image_new_from_stock ("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (deletet_same_item), image);

    delete_no_exist_item = gtk_image_menu_item_new_with_label (_("删除已经不存在项"));
    gtk_widget_show (delete_no_exist_item);
    gtk_container_add (GTK_CONTAINER (delete_item_menu), delete_no_exist_item);

    image = gtk_image_new_from_stock ("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (delete_no_exist_item), image);

    real_delete_item = gtk_image_menu_item_new_with_label (_("移到回收站"));
    gtk_widget_show (real_delete_item);
    gtk_container_add (GTK_CONTAINER (delete_item_menu), real_delete_item);

    image = gtk_image_new_from_stock ("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (real_delete_item), image);

    id3_item = gtk_image_menu_item_new_with_label (_("查看标签信息"));
    gtk_widget_show (id3_item);
    gtk_container_add (GTK_CONTAINER (list_menu), id3_item);

    image = gtk_image_new_from_stock ("gtk-properties", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (id3_item), image);

    equalizer_item = gtk_image_menu_item_new_with_label (_("均衡器"));
    gtk_widget_show (equalizer_item);
    gtk_container_add (GTK_CONTAINER (list_menu), equalizer_item);

    image = gtk_image_new_from_stock ("gtk-properties", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (equalizer_item), image);

    separator_4 = gtk_separator_menu_item_new ();
    gtk_widget_show (separator_4);
    gtk_container_add (GTK_CONTAINER (list_menu), separator_4);
    gtk_widget_set_sensitive (separator_4, FALSE);

    lrc_item = gtk_image_menu_item_new_with_label (_("歌词"));
    gtk_widget_show (lrc_item);
    gtk_container_add (GTK_CONTAINER (list_menu), lrc_item);

    image = gtk_image_new_from_stock ("gtk-select-all", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lrc_item), image);

    lrc_item_menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (lrc_item), lrc_item_menu);

    switch_to_lrc_iem = gtk_image_menu_item_new_with_label (_("切换到歌词设置页面"));
    gtk_widget_show (switch_to_lrc_iem);
    gtk_container_add (GTK_CONTAINER (lrc_item_menu), switch_to_lrc_iem);

    image = gtk_image_new_from_stock ("gtk-jump-to", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (switch_to_lrc_iem), image);

    if (!configure.lrc) show_lrc_item = gtk_image_menu_item_new_with_label (_("使用歌词"));
    else  show_lrc_item = gtk_image_menu_item_new_with_label (_("关闭歌词"));
    gtk_widget_show (show_lrc_item);
    gtk_container_add (GTK_CONTAINER (lrc_item_menu), show_lrc_item);

    if (!configure.lrc)image = gtk_image_new_from_stock (GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
    else image = gtk_image_new_from_stock (GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (show_lrc_item), image);


    GtkWidget *menu_research_lrc = NULL;
    GtkWidget *menu_research = NULL;
    inline void research_lrc() {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_search_lrc_checkbutton),!configure.search_lrc);
        update_configure();
    }
    if (configure.search_lrc)menu_research_lrc = gtk_image_menu_item_new_with_label(_("关闭搜索歌词功能"));
    else menu_research_lrc = gtk_image_menu_item_new_with_label(_("使用搜索歌词功能"));
    gtk_widget_show (menu_research_lrc);
    gtk_container_add (GTK_CONTAINER (lrc_item_menu),menu_research_lrc);

    if (!configure.search_lrc)image = gtk_image_new_from_stock (GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
    else image = gtk_image_new_from_stock (GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_research_lrc), image);
    if (configure.search_lrc) {
        menu_research = gtk_image_menu_item_new_with_label(_("重新搜索歌词"));
        gtk_widget_show (menu_research);
        gtk_container_add (GTK_CONTAINER (lrc_item_menu),menu_research);

        image = gtk_image_new_from_stock (GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU);
        gtk_widget_show (image);
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_research), image);
    }

    GtkWidget *separator_8 = gtk_separator_menu_item_new ();
    gtk_widget_show (separator_8);
    gtk_container_add (GTK_CONTAINER (lrc_item_menu), separator_8);
    gtk_widget_set_sensitive (separator_8, FALSE);

    GtkWidget *menu_INDEPENDENT =gtk_image_menu_item_new_with_label(_("窗口歌词 | 独立窗口"));
    gtk_widget_show (menu_INDEPENDENT);
    image = gtk_image_new_from_stock ("gtk-indent", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_INDEPENDENT), image);

    GtkWidget * menu_INTEGRATION=gtk_image_menu_item_new_with_label(_("窗口歌词 | 独占主窗体"));
    gtk_widget_show (menu_INTEGRATION);
    image = gtk_image_new_from_stock ("gtk-indent", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_INTEGRATION), image);

    GtkWidget *menu_HPANED =gtk_image_menu_item_new_with_label(_("窗口歌词 | 集成入主窗体"));
    gtk_widget_show (menu_HPANED);
    image = gtk_image_new_from_stock ("gtk-indent", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (menu_HPANED), image);

    gtk_container_add(GTK_CONTAINER(lrc_item_menu),menu_INDEPENDENT);
    gtk_container_add(GTK_CONTAINER(lrc_item_menu),menu_HPANED);
    gtk_container_add(GTK_CONTAINER(lrc_item_menu),menu_INTEGRATION);

    if (configure.lrc) {

        separator_5 = gtk_separator_menu_item_new ();
        gtk_widget_show (separator_5);
        gtk_container_add (GTK_CONTAINER (lrc_item_menu), separator_5);
        gtk_widget_set_sensitive (separator_5, FALSE);

        if (!configure.desktop_lrc) show_desktop_lrc_item = gtk_image_menu_item_new_with_label (_("显示桌面歌词"));
        else  show_desktop_lrc_item = gtk_image_menu_item_new_with_label (_("关闭桌面歌词"));
        gtk_widget_show (show_desktop_lrc_item);
        gtk_container_add (GTK_CONTAINER (lrc_item_menu), show_desktop_lrc_item);

        if (!configure.desktop_lrc)image = gtk_image_new_from_stock (GTK_STOCK_YES, GTK_ICON_SIZE_MENU);
        else image = gtk_image_new_from_stock (GTK_STOCK_NO, GTK_ICON_SIZE_MENU);
        gtk_widget_show (image);
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (show_desktop_lrc_item), image);

        if (configure.desktop_lrc) {
            if (!configure.desktop_lrc_lock) lock_desktop_lrc_item = gtk_image_menu_item_new_with_label (_("锁定桌面歌词"));
            else  lock_desktop_lrc_item = gtk_image_menu_item_new_with_label (_("解锁桌面歌词"));
            gtk_widget_show (lock_desktop_lrc_item);
            gtk_container_add (GTK_CONTAINER (lrc_item_menu), lock_desktop_lrc_item);

            image = gtk_image_new_from_stock ("gtk-connect", GTK_ICON_SIZE_MENU);
            gtk_widget_show (image);
            gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lock_desktop_lrc_item), image);
        }

        edit_lrc_item = gtk_image_menu_item_new_with_label (_("编辑歌词..."));
        gtk_widget_show (edit_lrc_item);
        //gtk_container_add (GTK_CONTAINER (lrc_item_menu), edit_lrc_item);

        image = gtk_image_new_from_stock ("gtk-edit", GTK_ICON_SIZE_MENU);
        gtk_widget_show (image);
        gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (edit_lrc_item), image);

    }
    sort_songs = gtk_image_menu_item_new_with_label (_("排序"));
    gtk_widget_show (sort_songs);
    gtk_container_add (GTK_CONTAINER (list_menu), sort_songs);

    image = gtk_image_new_from_stock ("gtk-sort-descending", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (sort_songs), image);

    sort_songs_menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (sort_songs), sort_songs_menu);

    sort_with_file_name_item = gtk_image_menu_item_new_with_label (_("文件名正序"));
    gtk_widget_show (sort_with_file_name_item);
    gtk_container_add (GTK_CONTAINER (sort_songs_menu), sort_with_file_name_item);

    image = gtk_image_new_from_stock ("gtk-sort-ascending", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (sort_with_file_name_item), image);

    un_sort_with_file_name_item = gtk_image_menu_item_new_with_label (_("文件名反序"));
    gtk_widget_show (un_sort_with_file_name_item);
    gtk_container_add (GTK_CONTAINER (sort_songs_menu), un_sort_with_file_name_item);

    image = gtk_image_new_from_stock ("gtk-sort-ascending", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (un_sort_with_file_name_item), image);

    sort_with_id3_editor_item = gtk_image_menu_item_new_with_label (_("标签-歌手名排序"));
    //gtk_widget_show (sort_with_id3_editor_item);
    // gtk_container_add (GTK_CONTAINER (sort_songs_menu), sort_with_id3_editor_item);

    image = gtk_image_new_from_stock ("gtk-sort-descending", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (sort_with_id3_editor_item), image);

    sort_with_id3_song_name_item = gtk_image_menu_item_new_with_label (_("标签-歌曲名排序"));
    //gtk_widget_show (sort_with_id3_song_name_item);
    // gtk_container_add (GTK_CONTAINER (sort_songs_menu), sort_with_id3_song_name_item);

    image = gtk_image_new_from_stock ("gtk-sort-descending", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (sort_with_id3_song_name_item), image);

    sort_with_radom_item = gtk_image_menu_item_new_with_label (_("随机乱序"));
    gtk_widget_show (sort_with_radom_item);
    gtk_container_add (GTK_CONTAINER (sort_songs_menu), sort_with_radom_item);

    image = gtk_image_new_from_stock ("gtk-indent", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (sort_with_radom_item), image);
    /*
        separator_6 = gtk_separator_menu_item_new ();
        gtk_widget_show (separator_6);
        gtk_container_add (GTK_CONTAINER (sort_songs_menu), separator_6);
        gtk_widget_set_sensitive (separator_6, FALSE);
    */
    play_mode_item = gtk_image_menu_item_new_with_label (_("模式"));
    gtk_widget_show (play_mode_item);
    gtk_container_add (GTK_CONTAINER (list_menu), play_mode_item);

    image = gtk_image_new_from_stock ("gtk-go-forward", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_mode_item), image);

    play_mode_item_menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (play_mode_item), play_mode_item_menu);

    play_mode_normal_item = gtk_image_menu_item_new_with_label (_("顺序播放"));
    gtk_widget_show (play_mode_normal_item);
    gtk_container_add (GTK_CONTAINER (play_mode_item_menu), play_mode_normal_item);

    image = gtk_image_new_from_stock ("gtk-goto-bottom", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_mode_normal_item), image);

    play_mode_unnormal_item = gtk_image_menu_item_new_with_label (_("逆序播放"));
    gtk_widget_show (play_mode_unnormal_item);
    gtk_container_add (GTK_CONTAINER (play_mode_item_menu), play_mode_unnormal_item);

    image = gtk_image_new_from_stock ("gtk-goto-top", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_mode_unnormal_item), image);

    play_mode_radom_item = gtk_image_menu_item_new_with_label (_("随机播放"));
    gtk_widget_show (play_mode_radom_item);
    gtk_container_add (GTK_CONTAINER (play_mode_item_menu), play_mode_radom_item);

    image = gtk_image_new_from_stock ("gtk-indent", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_mode_radom_item), image);

    play_mode_one_item = gtk_image_menu_item_new_with_label (_("单曲播放"));
    gtk_widget_show (play_mode_one_item);
    gtk_container_add (GTK_CONTAINER (play_mode_item_menu), play_mode_one_item);

    image = gtk_image_new_from_stock ("gtk-refresh", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_mode_one_item), image);

    separator_7 = gtk_separator_menu_item_new ();
    gtk_widget_show (separator_7);
    gtk_container_add (GTK_CONTAINER (play_mode_item_menu), separator_7);
    gtk_widget_set_sensitive (separator_7, FALSE);

    play_mode_other_lists_item = gtk_image_menu_item_new_with_label (_("跨列表?"));
    gtk_widget_show (play_mode_other_lists_item);
    gtk_container_add (GTK_CONTAINER (play_mode_item_menu), play_mode_other_lists_item);

    switch (configure.play_mode) {
    case PLAY_MODE_NORMAL:
        gtk_label_set_markup ((GtkLabel *)GTK_BIN (play_mode_normal_item)->child, g_markup_printf_escaped ("<span weight=\"bold\">%s</span>",gtk_label_get_text (GTK_LABEL (GTK_BIN (play_mode_normal_item)->child))));
        break;
    case PLAY_MODE_UNORMAL:
        gtk_label_set_markup ((GtkLabel *)GTK_BIN (play_mode_unnormal_item)->child, g_markup_printf_escaped ("<span weight=\"bold\">%s</span>",gtk_label_get_text (GTK_LABEL (GTK_BIN (play_mode_unnormal_item)->child))));
        break;
    case PLAY_MODE_RADOM:
        gtk_label_set_markup ((GtkLabel *)GTK_BIN (play_mode_radom_item)->child, g_markup_printf_escaped ("<span weight=\"bold\">%s</span>",gtk_label_get_text (GTK_LABEL (GTK_BIN (play_mode_radom_item)->child))));
        break;
    case PLAY_MODE_ONE:
        gtk_label_set_markup ((GtkLabel *)GTK_BIN (play_mode_one_item)->child, g_markup_printf_escaped ("<span weight=\"bold\">%s</span>",gtk_label_get_text (GTK_LABEL (GTK_BIN (play_mode_one_item)->child))));
        break;
    }

    if (!configure.auto_jump_other_lists)image = gtk_image_new_from_stock ("gtk-no", GTK_ICON_SIZE_MENU);
    else image = gtk_image_new_from_stock ("gtk-yes", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (play_mode_other_lists_item), image);

    search_item = gtk_image_menu_item_new_with_label (_("搜索"));
    gtk_widget_show (search_item);
    //gtk_container_add (GTK_CONTAINER (list_menu), search_item);

    image = gtk_image_new_from_stock ("gtk-find", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (search_item), image);

    search_item_menu = gtk_menu_new ();
    gtk_menu_item_set_submenu (GTK_MENU_ITEM (search_item), search_item_menu);

    search_local_song_item = gtk_image_menu_item_new_with_label (_("搜索本地歌曲"));
    gtk_widget_show (search_local_song_item);
    gtk_container_add (GTK_CONTAINER (search_item_menu), search_local_song_item);

    image = gtk_image_new_from_stock ("gtk-find", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (search_local_song_item), image);

    search_music_box_item = gtk_image_menu_item_new_with_label (_("搜索网络音乐"));
    gtk_widget_show (search_music_box_item);
    gtk_container_add (GTK_CONTAINER (search_item_menu), search_music_box_item);

    image = gtk_image_new_from_stock ("gtk-find", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (search_music_box_item), image);

    g_signal_connect ((gpointer) jump_to_playing_song_item, "activate",G_CALLBACK (list_menu_jump_to_playing_song),NULL);
    g_signal_connect ((gpointer) play_selection_item, "activate",G_CALLBACK (list_menu_play_selection),NULL);
    g_signal_connect ((gpointer) open_dir_of_song_item, "activate",G_CALLBACK (list_menu_open_dir_of_song),NULL);
    g_signal_connect ((gpointer) man_tools_item, "activate",G_CALLBACK (list_menu_man_tools),NULL);
    g_signal_connect ((gpointer) rename_item, "activate",G_CALLBACK (list_menu_rename),NULL);
    g_signal_connect ((gpointer) input_lrc_item, "activate",G_CALLBACK (list_menu_input_lrc),NULL);
    g_signal_connect ((gpointer) open_with_audacious_item, "activate",G_CALLBACK (list_menu_open_with_audacious),NULL);
    g_signal_connect ((gpointer) open_with_amarok_item, "activate",G_CALLBACK (list_menu_open_with_amarok_item),NULL);
    g_signal_connect ((gpointer) open_with_totem_item, "activate",G_CALLBACK (list_menu_open_with_totem),NULL);
    g_signal_connect ((gpointer) deletet_selection_item, "activate",G_CALLBACK (list_menu_deletet_selection),NULL);
    g_signal_connect ((gpointer) delete_focus_lists, "activate",G_CALLBACK (list_menu_delete_focus_lists),NULL);
    g_signal_connect ((gpointer) delete_no_exist_item, "activate",G_CALLBACK (list_menu_delete_no_exist_item),NULL);
    g_signal_connect ((gpointer) deletet_same_item, "activate",G_CALLBACK (list_menu_deletet_same_item),NULL);
    g_signal_connect ((gpointer) real_delete_item, "activate",G_CALLBACK (list_menu_real_delete),NULL);
    g_signal_connect ((gpointer) id3_item, "activate",G_CALLBACK (list_menu_id3),NULL);
    g_signal_connect ((gpointer) equalizer_item, "activate",G_CALLBACK (create_equalizer),NULL);
    g_signal_connect ((gpointer) switch_to_lrc_iem, "activate",G_CALLBACK (list_menu_switch_to_lrc),NULL);
    inline void close_lrc() {
        gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_lrc_checkbutton),!configure.lrc);
        update_configure();
        lrc();
    }
    g_signal_connect ((gpointer) show_lrc_item, "activate",G_CALLBACK (close_lrc),NULL);
    if (configure.lrc) {
        g_signal_connect ((gpointer) show_desktop_lrc_item, "activate",G_CALLBACK (list_menu_show_desktop_lrc),NULL);
        if (configure.desktop_lrc) g_signal_connect ((gpointer) lock_desktop_lrc_item, "activate",G_CALLBACK (list_menu_lock_desktop_lrc),NULL);
    }
    g_signal_connect((gpointer) menu_INDEPENDENT ,"activate" ,G_CALLBACK(change_widget_lrc_ui),(gpointer *)WIDGET_LRC_SHOW_TYPE_INDEPENDENT);
    g_signal_connect((gpointer) menu_HPANED ,"activate" ,G_CALLBACK(change_widget_lrc_ui),(gpointer *)WIDGET_LRC_SHOW_TYPE_HPANED);
    g_signal_connect((gpointer) menu_INTEGRATION ,"activate" ,G_CALLBACK(change_widget_lrc_ui),(gpointer *)WIDGET_LRC_SHOW_TYPE_INTEGRATION);
    g_signal_connect ((gpointer) menu_research_lrc, "activate",G_CALLBACK (research_lrc),NULL);
    if (configure.search_lrc)g_signal_connect ((gpointer) menu_research, "activate",G_CALLBACK (search_lrc_pre),NULL);

    g_signal_connect ((gpointer) sort_songs, "activate",G_CALLBACK (list_menu_sort_songs),NULL);
    g_signal_connect ((gpointer) sort_with_file_name_item, "activate",G_CALLBACK (list_menu_sort_with_file_name),NULL);
    g_signal_connect ((gpointer) un_sort_with_file_name_item, "activate",G_CALLBACK (list_menu_un_sort_with_file_name),NULL);
    g_signal_connect ((gpointer) sort_with_id3_editor_item, "activate",G_CALLBACK (list_menu_sort_with_id3_editor),NULL);
    g_signal_connect ((gpointer) sort_with_id3_song_name_item, "activate",G_CALLBACK (list_menu_sort_with_id3_song_name),NULL);
    g_signal_connect ((gpointer) sort_with_radom_item, "activate",G_CALLBACK (list_menu_sort_with_radom),NULL);
    g_signal_connect ((gpointer) play_mode_normal_item, "activate",G_CALLBACK (list_menu_play_mode),(gpointer *)PLAY_MODE_NORMAL);
    g_signal_connect ((gpointer) play_mode_unnormal_item, "activate",G_CALLBACK (list_menu_play_mode),(gpointer *)PLAY_MODE_UNORMAL);
    g_signal_connect ((gpointer) play_mode_radom_item, "activate",G_CALLBACK (list_menu_play_mode),(gpointer *)PLAY_MODE_RADOM);
    g_signal_connect ((gpointer) play_mode_one_item, "activate",G_CALLBACK (list_menu_play_mode),(gpointer *)PLAY_MODE_ONE);
    g_signal_connect ((gpointer) play_mode_other_lists_item, "activate",G_CALLBACK (list_menu_change_play_mode_other_lists),NULL);
    g_signal_connect ((gpointer) search_local_song_item, "activate",G_CALLBACK (list_menu_search_local_song),NULL);
    g_signal_connect ((gpointer) search_music_box_item, "activate",G_CALLBACK (list_menu_search_music_box),NULL);

    gtk_widget_show_all (list_menu);
    popup_menu(list_menu,2,2);

}
void show_lists_menu() {
    print_programming("show_lists_menu\n");
    GtkWidget *lists_menu = gtk_menu_new ();
    GtkWidget *lists_delete;
    GtkWidget *lists_rename;
    GtkWidget *lists_new;
    GtkWidget *lists_delete_all;
    GtkWidget *image ;
    GtkWidget *lists_add_files;
    GtkWidget * lists_add_folder;

    lists_add_files = gtk_image_menu_item_new_with_label (_("添加文件到选中列表"));
    gtk_widget_show (lists_add_files);
    gtk_menu_append (GTK_MENU_SHELL (lists_menu), lists_add_files);

    image = gtk_image_new_from_stock ("gtk-add", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lists_add_files), image);

    lists_add_folder = gtk_image_menu_item_new_with_label (_("添加文件夹到选中列表"));
    gtk_widget_show (lists_add_folder);
    gtk_menu_append (GTK_MENU_SHELL (lists_menu), lists_add_folder);

    image = gtk_image_new_from_stock ("gtk-open", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lists_add_folder), image);

    lists_new = gtk_image_menu_item_new_with_label (_("新建"));
    gtk_widget_show (lists_new);
    gtk_menu_append (GTK_MENU_SHELL (lists_menu), lists_new);

    image = gtk_image_new_from_stock ("gtk-new", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lists_new), image);

    lists_rename = gtk_image_menu_item_new_with_label (_("重命名"));
    gtk_widget_show (lists_rename);
    gtk_menu_append (GTK_MENU_SHELL (lists_menu), lists_rename);

    image = gtk_image_new_from_stock (GTK_STOCK_PROPERTIES, GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lists_rename), image);

    lists_delete = gtk_image_menu_item_new_with_label (_("删除"));
    gtk_widget_show (lists_delete);
    gtk_menu_append (GTK_MENU_SHELL (lists_menu), lists_delete);

    image = gtk_image_new_from_stock ("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lists_delete), image);

    lists_delete_all = gtk_image_menu_item_new_with_label (_("删除所有"));
    gtk_widget_show (lists_delete_all);
    gtk_menu_append (GTK_MENU_SHELL (lists_menu), lists_delete_all);

    image = gtk_image_new_from_stock ("gtk-delete", GTK_ICON_SIZE_MENU);
    gtk_widget_show (image);
    gtk_image_menu_item_set_image (GTK_IMAGE_MENU_ITEM (lists_delete_all), image);

    inline void rename_l() {
        GtkTreeModel *model;
        GtkTreeIter iter;
        gchar *value=NULL;
        GList *list_select = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_lists_treeview))),&model);
        if (list_select)
        {
            if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath *) list_select->data)) {
                gtk_tree_model_get(model, &iter, 0, &value, -1);
            } else {
                print_error("未知错误，重命名列表名时无法读取原名");
                dialog("未知错误，重命名列表名时无法读取原名");
                return;
            }
        }

        GtkWidget *input_dialog;
        GtkWidget *dialog_vbox;
        GtkWidget *entry;
        GtkWidget *dialog_action_area;
        GtkWidget *cancelbutton;
        GtkWidget *okbutton;

        input_dialog = gtk_dialog_new ();
        gtk_window_set_title (GTK_WINDOW (input_dialog), _("输入"));
        gtk_window_set_position (GTK_WINDOW (input_dialog), GTK_WIN_POS_CENTER_ON_PARENT);
        gtk_window_resize(GTK_WINDOW(input_dialog), 500, 50);
        gtk_window_set_type_hint (GTK_WINDOW (input_dialog), GDK_WINDOW_TYPE_HINT_DIALOG);

        dialog_vbox = GTK_DIALOG (input_dialog)->vbox;
        gtk_widget_show (dialog_vbox);

        entry = gtk_entry_new ();
        gtk_widget_show (entry);
        gtk_entry_set_text(GTK_ENTRY(entry),value);
        gtk_box_pack_start (GTK_BOX (dialog_vbox), entry, FALSE, FALSE, 0);


        dialog_action_area = GTK_DIALOG (input_dialog)->action_area;
        gtk_widget_show (dialog_action_area);
        gtk_button_box_set_layout (GTK_BUTTON_BOX (dialog_action_area), GTK_BUTTONBOX_END);

        cancelbutton = gtk_button_new_from_stock ("gtk-cancel");
        gtk_widget_show (cancelbutton);
        gtk_dialog_add_action_widget (GTK_DIALOG (input_dialog), cancelbutton, GTK_RESPONSE_CANCEL);
        GTK_WIDGET_SET_FLAGS (cancelbutton, GTK_CAN_DEFAULT);

        okbutton = gtk_button_new_from_stock ("gtk-ok");
        gtk_widget_show (okbutton);
        gtk_dialog_add_action_widget (GTK_DIALOG (input_dialog), okbutton, GTK_RESPONSE_OK);
        GTK_WIDGET_SET_FLAGS (okbutton, GTK_CAN_DEFAULT);

        GtkAccelGroup *gag = gtk_accel_group_new();
        gtk_widget_add_accelerator(okbutton,"clicked",gag,GDK_Return,0,GTK_ACCEL_VISIBLE);
        gtk_window_add_accel_group(GTK_WINDOW(input_dialog),gag);

        gint a = gtk_dialog_run(GTK_DIALOG(input_dialog));
        if (a==-5&&gtk_entry_get_text(GTK_ENTRY(entry))!=NULL) {
            if (strcmp(gtk_entry_get_text(GTK_ENTRY(entry)),""))rename_lists(value,(gchar *)gtk_entry_get_text(GTK_ENTRY(entry)));
        }
        gtk_widget_destroy(input_dialog);
        if (value)g_free_n(value);
    }
    inline void new_l() {
        gchar *a = add_new_lists(_("新建列表"));
        show_lists(a);
        strcpy(focus_lists,a);

        show_songs(NULL);
        show_lists(NULL);
        gint index = 0;
        iceplayer_lists *lists = &main_lists;
        while (lists->next!=NULL) {
            lists=lists->next;
            index++;
        }
        gchar in[50];
        sprintf(in,"%d",index);
        gtk_tree_view_set_cursor((GtkTreeView *) iceplayer.list_lists_treeview,gtk_tree_path_new_from_string(in), gtk_tree_view_get_column((GtkTreeView *) iceplayer.list_lists_treeview,0), FALSE);
        rename_l();
    }
    inline void delete_l() {
        GtkTreeModel *model;
        GtkTreeIter iter;
        gchar *value=NULL;
        GList *list_select = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_lists_treeview))),&model);
        if (list_select)
            if (gtk_tree_model_get_iter(model, &iter, (GtkTreePath *) list_select->data)) {
                gtk_tree_model_get(model, &iter, 0, &value, -1);
                delete_lists(find_lists_from_name(value));
                if (value)g_free_n(value);
            }
    }
    inline void delete_all_l() {
        stop();
        focus_lists[0]=0;
        playing_song[0]=0;
        playing_list[0]=0;
        main_lists.focus_song[0]=0;
        main_lists.name[0]=0;
        main_lists.songs=NULL;
        main_lists.next=NULL;
        gtk_list_store_clear( GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(iceplayer.list_lists_treeview))));
        gtk_list_store_clear( GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(iceplayer.list_songs_treeview))));
    }
    g_signal_connect ((gpointer) lists_add_files, "activate",G_CALLBACK(add_dialog),(gpointer)ADD_FILES);
    g_signal_connect ((gpointer) lists_add_folder, "activate",G_CALLBACK(add_dialog),(gpointer)ADD_FOLDER);
    g_signal_connect ((gpointer) lists_new, "activate",G_CALLBACK (new_l),NULL);
    g_signal_connect ((gpointer) lists_rename, "activate",G_CALLBACK (rename_l),NULL);
    g_signal_connect ((gpointer) lists_delete, "activate",G_CALLBACK (delete_l),NULL);
    g_signal_connect ((gpointer) lists_delete_all, "activate",G_CALLBACK (delete_all_l),NULL);

    gtk_widget_show_all(lists_menu);
    popup_menu(lists_menu,2,2);
}





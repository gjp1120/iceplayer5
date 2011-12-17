#include <gtk/gtk.h>

#include "core.h"
#include "widget_lrc.h"
#include "single.h"
#include "desktop_lrc.h"
#include "list.h"
#include "ui.h"
#include "configure.h"
#include "download.h"
#include "lrc.h"

gchar searching_lrc[STRINGS_LENGTH];

gchar lrc_text[256][256];
gint widget_lrc_word_height= 20480 / PANGO_SCALE;
gint desktop_lrc_word_height = 30720 / PANGO_SCALE;
gboolean downloader_has_press;
gint download_choose_number;
iceplayer_lrc_search_data lrc_search_data;
iceplayer_lrc_search *main_lrc_search;
gint last_timer=0;
gboolean wait=FALSE;
void on_downloader_button_search_clicked  (GtkButton *button,GtkWidget *iceplayer_lrc_downloader_entry) {
    wait=TRUE;
    last_timer=-1;
    strcpy(lrc_search_data.keyword,gtk_entry_get_text((GtkEntry *)iceplayer_lrc_downloader_entry));
    gint sd=strlen(lrc_search_data.keyword)-1;
    while (lrc_search_data.keyword[sd] ==' ') {
        lrc_search_data.keyword[sd]=0;
        sd--;
    }
    if (GTK_IS_WIDGET(iceplayer_lrc_downloader_entry->parent->parent->parent))gtk_widget_destroy(iceplayer_lrc_downloader_entry->parent->parent->parent);
    g_thread_create((void *)search_lrc,NULL,FALSE,NULL);
}
gboolean on_downloader_entry_button_press() {
    downloader_has_press = FALSE;
    last_timer=0;
    return FALSE;
}
void on_downloader_list_selection_changed (GtkList *list,GtkWidget *iceplayer_lrc_downloader_list) {
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(iceplayer_lrc_downloader_list));
    GList *glist = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer_lrc_downloader_list))),&model);
    if (glist)download_choose_number = atoi(gtk_tree_path_to_string(glist->data));
    last_timer=0;
}
void on_downloader_button_black_list_clicked (GtkButton *button,GtkWidget *iceplayer_lrc_downloader) {
    last_timer=-84;
    GtkWidget *dialog = gtk_message_dialog_new(NULL,GTK_DIALOG_DESTROY_WITH_PARENT ,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK_CANCEL,_("黑名单功能是新建一个空白的lrc文件，\n使得下次播放到这个文件时就不会因为没有lrc文件而去搜索它的歌词。\n注意，重新搜索歌词仍然可以正常工作，不过这一功能可能会影响到其他有歌词下载功能的播放器的正常工作，\n确认要新建一个空白的歌词文件来关联这首歌曲吗？"));
    gint res = gtk_dialog_run((GtkDialog *)dialog);
    gtk_widget_destroy(dialog);
    if (res==-5) {
        g_print("Debug:建立歌词黑名单");
        last_timer=-1;
        FILE *file = fopen(lrc_search_data.lrc_to_save,"w");
        if (file) {
            fputs("[ti:iceplayer black list file]\n[ar:jasy.ice@163.com]\n[al:iceplayer]\n[by:slax]\n[offset:0]\n[00:00.00]iceplayer 音乐播放器(black list lrc)",file);
            fclose(file);
            on_downloader_button_close_clicked(button,iceplayer_lrc_downloader);
        } else {
            print_error("新建黑名单失败，文件无法创建，请确认你有权限建立并修改文件！\n");
            dialog(_("新建黑名单失败，文件无法创建，请确认你有权限建立并修改文件！"));
        }
    }
    last_timer=0;
}
void on_downloader_button_close_clicked (GtkButton *button,GtkWidget *iceplayer_lrc_downloader) {
    if (GTK_IS_WIDGET(iceplayer_lrc_downloader))gtk_widget_destroy(iceplayer_lrc_downloader);
    lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_CLOSE;
    last_timer=-1;
    configure.search_lrc = FALSE;
    lrc();
    configure.search_lrc = TRUE;
}
void on_downloader_button_lrc123_enter (GtkButton *button,GtkWidget *iceplayer_lrc_downloader) {
    if (GTK_IS_WIDGET(iceplayer_lrc_downloader))gtk_widget_destroy(iceplayer_lrc_downloader);
    last_timer=-1;
    gtk_show_uri(NULL,"http://www.lrc123.com", 0,NULL);
}
gboolean download_lrc(gchar *hostname) {
    inline gboolean lrc2() {
        configure.search_lrc = FALSE;
        lrc();
        configure.search_lrc = TRUE;
        return FALSE;
    }

    if (system(hostname));
    g_free_n(hostname);
    lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_DOWNLOADED;
    g_timeout_add(100,lrc2,NULL);
    return FALSE;
}
//用于给设定时限为0并且寻找到歌词的情况下，直接指定下载第1位的歌词.
void download_lrc_by_num(gint num){
        gint count = num;
        if (!main_lrc_search->uri[0])
            return;
        while (count--) {
            main_lrc_search= main_lrc_search->next;
            if (main_lrc_search->next==NULL)break;
        }
        g_print("uri为:%s ",main_lrc_search->uri);
        g_print("save为:%s \n",lrc_search_data.lrc_to_save);
        gchar *hostname= (gchar *)g_malloc(STRINGS_LENGTH*5*sizeof(gchar *));
        strcpy(hostname,"wget www.lrc123.com");
        strcat(hostname,main_lrc_search->uri);
        strcat(hostname," --output-document=\"");
        strcat(hostname,lrc_search_data.lrc_to_save);
        strcat(hostname,"\"");

        g_timeout_add(100,G_SOURCE_FUNC(download_lrc),hostname);
}
gboolean on_downloader_button_download_clicked(GtkWidget *data,GtkWidget *widget) {
    g_print("Debug:下载歌词，选择项目为:%d,",download_choose_number);
    last_timer=-1;
    lrc_search_data.search_lrc_state =  SEARCH_LRC_STATE_DOWNLOADING;
    if (GTK_IS_WIDGET(widget))gtk_widget_destroy(widget);
    if (main_lrc_search->uri[0]) {
        download_lrc_by_num(download_choose_number);
    } else {
        inline gboolean lrc2() {
            configure.search_lrc = FALSE;
            lrc();
            configure.search_lrc = TRUE;
            return FALSE;
        }
        lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_DOWNLOADED;
        g_timeout_add(10,lrc2,NULL);
    }
    return FALSE;
}
gboolean check_down(GtkWidget *widget) {
    gint n;
    if (configure.search_lrc_time[0]=='0'||configure.search_lrc_time[0]==0) {
        if (!main_lrc_search->uri[0]) {
            n=15;
        } else {

            if (wait)
                n=15;
            else
                n=0;
        }
    } else {
        sscanf(configure.search_lrc_time,"%d",&n);
    }

    if (!GTK_IS_WIDGET(widget)) {
        last_timer=0;
        return FALSE;
    }
    if (last_timer>=n+1) {
        last_timer=0;
        if (GTK_IS_WIDGET(widget))on_downloader_button_download_clicked(NULL,widget->parent->parent->parent->parent);
        else {
            last_timer=0;
            return FALSE;
        }
        return FALSE;
    }

    gchar *name= (gchar *)g_malloc(200*sizeof(gchar *));
    g_snprintf(name,200,_("下载(%ds)\nCtrl+Enter↙"),n-last_timer);
    if (GTK_IS_WIDGET(widget)) {
        gtk_button_set_label(GTK_BUTTON(widget),name);
        g_free_n(name);
    } else {
        last_timer=0;
        g_free_n(name);
        return FALSE;
    }

    last_timer++;

    return TRUE;
}
gboolean on_downloader_list_key_press (GtkWidget *iceplayer_lrc_downloader_list,GdkEventKey *event) {
    downloader_has_press = FALSE;
    if (event->type == GDK_KEY_PRESS) {
        if (event->keyval==GDK_Return) {
            on_downloader_button_download_clicked(NULL,iceplayer_lrc_downloader_list->parent->parent->parent->parent);
        }
    }
    if (event->type==GDK_2BUTTON_PRESS) {
        on_downloader_button_download_clicked(NULL,iceplayer_lrc_downloader_list->parent->parent->parent->parent);
    }
    return FALSE;
}
gboolean on_downloader_entry_key_press (GtkWidget *iceplayer_lrc_downloader_entry,GdkEventKey *event,GtkWidget *iceplayer_lrc_downloader) {
    downloader_has_press = FALSE;
    if (event->type == GDK_KEY_PRESS) {
        if (event->keyval==GDK_Return) {
            if (event->state==GDK_CONTROL_MASK)
                on_downloader_button_download_clicked(NULL,iceplayer_lrc_downloader);
            else
                on_downloader_button_search_clicked(NULL,iceplayer_lrc_downloader_entry);
        }
    }
    return FALSE;
}
gboolean choose_lrc_to_download(iceplayer_lrc_search *lrc_search) {
    GtkWidget *iceplayer_lrc_downloader;
    GtkWidget *iceplayer_lrc_downloader_table;
    GtkWidget *iceplayer_lrc_downloader_button_search;
    GtkWidget *iceplayer_lrc_downloader_scrolledwindow;
    GtkTreeSelection *iceplayer_lrc_downloader_selection;
    GtkWidget *iceplayer_lrc_downloader_vbox;
    GtkWidget *iceplayer_lrc_downloader_button_download;
    GtkWidget *iceplayer_lrc_downloader_button_close;
    GtkWidget *iceplayer_lrc_downloader_button_black_list;
    GtkWidget *iceplayer_lrc_downloader_button_lrc123;
    GtkWidget *iceplayer_lrc_downloader_entry;
    GtkWidget *iceplayer_lrc_downloader_list;

    iceplayer_lrc_downloader = gtk_dialog_new();
    gtk_widget_set_size_request (iceplayer_lrc_downloader, 400, 250);
    gtk_container_set_border_width (GTK_CONTAINER (iceplayer_lrc_downloader), 5);
    gtk_window_set_title (GTK_WINDOW (iceplayer_lrc_downloader), _("iceplayer 歌词下载"));
    gtk_window_set_keep_above     (GTK_WINDOW (iceplayer_lrc_downloader),TRUE);
    gtk_window_set_accept_focus(GTK_WINDOW(iceplayer_lrc_downloader),FALSE);
    gtk_window_set_accept_focus(GTK_WINDOW(iceplayer_lrc_downloader),TRUE);
    GdkScreen* screen;
    gint width, height;
    screen = gdk_screen_get_default();
    width = gdk_screen_get_width(screen);
    height = gdk_screen_get_height(screen);
    gtk_window_move(GTK_WINDOW (iceplayer_lrc_downloader),width-400,height-250);


    iceplayer_lrc_downloader_table = gtk_table_new (2, 2, FALSE);
    gtk_widget_show (iceplayer_lrc_downloader_table);
    gtk_box_pack_start (GTK_BOX (((GtkDialog *)iceplayer_lrc_downloader)->vbox), iceplayer_lrc_downloader_table, TRUE, TRUE, 0);

    iceplayer_lrc_downloader_entry = gtk_entry_new();
    gtk_entry_set_text ((GtkEntry *)iceplayer_lrc_downloader_entry,lrc_search_data.keyword);
    gtk_widget_show (iceplayer_lrc_downloader_entry);

    gtk_table_attach (GTK_TABLE (iceplayer_lrc_downloader_table), iceplayer_lrc_downloader_entry, 0, 1, 0, 1,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
    gtk_entry_set_invisible_char (GTK_ENTRY (iceplayer_lrc_downloader_entry), 9679);
    gtk_entry_set_activates_default (GTK_ENTRY (iceplayer_lrc_downloader_entry), TRUE);

    iceplayer_lrc_downloader_button_search = gtk_button_new_with_mnemonic (_("搜索  Enter↙"));
    gtk_widget_show (iceplayer_lrc_downloader_button_search);
    gtk_table_attach (GTK_TABLE (iceplayer_lrc_downloader_table), iceplayer_lrc_downloader_button_search, 1, 2, 0, 1,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);

    iceplayer_lrc_downloader_scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
    gtk_widget_show (iceplayer_lrc_downloader_scrolledwindow);
    gtk_table_attach (GTK_TABLE (iceplayer_lrc_downloader_table), iceplayer_lrc_downloader_scrolledwindow, 0, 1, 1, 2,(GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), 0, 0);
    gtk_container_set_border_width (GTK_CONTAINER (iceplayer_lrc_downloader_scrolledwindow), 1);
    gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (iceplayer_lrc_downloader_scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_shadow_type (GTK_SCROLLED_WINDOW (iceplayer_lrc_downloader_scrolledwindow), GTK_SHADOW_IN);

    iceplayer_lrc_downloader_list = gtk_tree_view_new();
    gtk_widget_show(iceplayer_lrc_downloader_list);
    gtk_container_add(GTK_CONTAINER(iceplayer_lrc_downloader_scrolledwindow),iceplayer_lrc_downloader_list);
    gtk_tree_view_set_reorderable(GTK_TREE_VIEW(iceplayer_lrc_downloader_list), FALSE);
    gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(iceplayer_lrc_downloader_list), FALSE);

    GtkCellRenderer *renderer;
    GtkListStore *store;
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(iceplayer_lrc_downloader_list), 0, " ", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(iceplayer_lrc_downloader_list), 0, " ", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(iceplayer_lrc_downloader_list), 0, " ", renderer, "text", 2, NULL);
    store = gtk_list_store_new(3, G_TYPE_STRING, G_TYPE_STRING,G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(iceplayer_lrc_downloader_list),GTK_TREE_MODEL(store));
    g_object_unref(store);

    iceplayer_lrc_downloader_selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer_lrc_downloader_list));
    gtk_tree_selection_set_mode(iceplayer_lrc_downloader_selection,GTK_SELECTION_SINGLE);

    iceplayer_lrc_downloader_vbox = gtk_vbox_new (FALSE, 0);
    gtk_widget_show (iceplayer_lrc_downloader_vbox);
    gtk_table_attach (GTK_TABLE (iceplayer_lrc_downloader_table), iceplayer_lrc_downloader_vbox, 1, 2, 1, 2,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (GTK_FILL), 0, 0);

    gchar *n= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

    if (configure.search_lrc_time[0]=='0'||configure.search_lrc_time[0]==0)
        sprintf(n,_("下载(%ss)\nCtrl+Enter↙"),"15");
    else
        sprintf(n,_("下载(%ss)\nCtrl+Enter↙"),configure.search_lrc_time);

    iceplayer_lrc_downloader_button_download = gtk_button_new_with_mnemonic (n);
    g_free_n(n);
    gtk_widget_show (iceplayer_lrc_downloader_button_download);
    gtk_box_pack_start (GTK_BOX (iceplayer_lrc_downloader_vbox), iceplayer_lrc_downloader_button_download, FALSE, FALSE, 0);
    gtk_widget_set_size_request(iceplayer_lrc_downloader_button_download,-1,80);

    iceplayer_lrc_downloader_button_black_list = gtk_button_new_with_mnemonic (_("黑名单.."));
    gtk_widget_show (iceplayer_lrc_downloader_button_black_list);
    gtk_box_pack_start (GTK_BOX (iceplayer_lrc_downloader_vbox), iceplayer_lrc_downloader_button_black_list, FALSE, FALSE, 0);

    iceplayer_lrc_downloader_button_close = gtk_button_new_with_mnemonic (_("关闭"));
    gtk_widget_show (iceplayer_lrc_downloader_button_close);
    gtk_box_pack_start (GTK_BOX (iceplayer_lrc_downloader_vbox), iceplayer_lrc_downloader_button_close, FALSE, FALSE, 0);

    iceplayer_lrc_downloader_button_lrc123 = gtk_button_new_with_mnemonic (_("lrc123.com"));
    gtk_widget_show (iceplayer_lrc_downloader_button_lrc123);
    gtk_box_pack_end (GTK_BOX (iceplayer_lrc_downloader_vbox), iceplayer_lrc_downloader_button_lrc123, FALSE, FALSE, 0);
    gtk_button_set_relief (GTK_BUTTON (iceplayer_lrc_downloader_button_lrc123), GTK_RELIEF_NONE);

    g_signal_connect ((gpointer) iceplayer_lrc_downloader_button_search,"clicked",G_CALLBACK (on_downloader_button_search_clicked),iceplayer_lrc_downloader_entry);
    g_signal_connect ((gpointer) iceplayer_lrc_downloader_button_download,"clicked",G_CALLBACK (on_downloader_button_download_clicked),iceplayer_lrc_downloader);
    g_signal_connect ((gpointer) iceplayer_lrc_downloader_button_black_list,"clicked",G_CALLBACK (on_downloader_button_black_list_clicked),iceplayer_lrc_downloader);
    g_signal_connect ((gpointer) iceplayer_lrc_downloader_button_close,"clicked",G_CALLBACK (on_downloader_button_close_clicked),iceplayer_lrc_downloader);
    g_signal_connect ((gpointer) iceplayer_lrc_downloader_button_lrc123,"clicked",G_CALLBACK (on_downloader_button_lrc123_enter),iceplayer_lrc_downloader);
    g_signal_connect ((gpointer) iceplayer_lrc_downloader_list,"cursor_changed",G_CALLBACK (on_downloader_list_selection_changed),iceplayer_lrc_downloader_list);
    g_signal_connect ((gpointer) iceplayer_lrc_downloader_list,"key_press_event",G_CALLBACK (on_downloader_list_key_press),iceplayer_lrc_downloader);
    g_signal_connect ((gpointer) iceplayer_lrc_downloader_entry,"key_press_event",G_CALLBACK (on_downloader_entry_key_press),iceplayer_lrc_downloader);
    g_signal_connect ((gpointer) iceplayer_lrc_downloader_entry,"button_press_event" ,G_CALLBACK(on_downloader_entry_button_press),NULL);
    g_signal_connect ((gpointer) iceplayer_lrc_downloader_list,"button_press_event" ,G_CALLBACK(on_downloader_list_key_press),NULL);
    g_signal_connect ((gpointer) iceplayer_lrc_downloader_list,"enter_notify_event" ,G_CALLBACK(on_downloader_list_key_press),NULL);


    GtkTreeIter iter;
    store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(iceplayer_lrc_downloader_list)));
    gtk_list_store_clear(store);
    iceplayer_lrc_search *lrc_search_tmp = lrc_search;
    while (lrc_search_tmp->next) {

        g_print("  歌手:");
        g_print(lrc_search_tmp->editor);
        g_print("\n");
        g_print("  歌曲:");
        g_print(lrc_search_tmp->title);
        g_print("\n");
        g_print("  专辑:");
        g_print(lrc_search_tmp->alubm);
        g_print("\n");
        g_print("  下载:");
        g_print(lrc_search_tmp->uri);
        g_print("\n");
        g_print("——————\n");

        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter,0,lrc_search_tmp->alubm,1,lrc_search_tmp->editor,2, lrc_search_tmp->title,-1);
        lrc_search_tmp = lrc_search_tmp->next;
    }

    gtk_tree_view_set_cursor((GtkTreeView *) iceplayer_lrc_downloader_list,gtk_tree_path_new_first(),gtk_tree_view_get_column((GtkTreeView *) iceplayer_lrc_downloader_list,0),FALSE);

    if (configure.search_lrc_time[0]=='0'||configure.search_lrc_time[0]==0) {
        if (main_lrc_search->uri[0]==0||wait){
            if(main_lrc_search->uri[0]==0){
                print_notify("iceplayer","歌词搜索无结果");
                 if(configure.edit_search_word) {//无结果并设定了在无结果的时候显示对话框
                    g_timeout_add(1000,G_SOURCE_FUNC(check_down),iceplayer_lrc_downloader_button_download);
                    gtk_dialog_run(GTK_DIALOG(iceplayer_lrc_downloader));
                }
            }else{//有结果并设定了时间为0
                if(wait){//如果是手动按快捷键的话
                    g_timeout_add(1000,G_SOURCE_FUNC(check_down),iceplayer_lrc_downloader_button_download);
                    gtk_dialog_run(GTK_DIALOG(iceplayer_lrc_downloader));
                }
            }
        }else{
        download_lrc_by_num(1);
        }
    } else {
        g_timeout_add(1000,G_SOURCE_FUNC(check_down),iceplayer_lrc_downloader_button_download);
        gtk_dialog_run(GTK_DIALOG(iceplayer_lrc_downloader));
    }
    return FALSE;
}
gchar *playing_song_path_without_some_word(gchar playing_song_path[STRINGS_LENGTH]) {
    gchar *path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(path,"");
    gint a=0,b=0;
    while (playing_song_path[b]) {
        if (playing_song_path[b]=='`') {//||playing_song_path[b]=='\''){
            path[a++]='\\';
            path[a++]=playing_song_path[b++];
        } else {
            path[a++]=playing_song_path[b++];
        }
    }
    path[a]=0;
    strcpy(playing_song_path,path);
    g_free_n(path);
    return playing_song_path;
}
gboolean back_to_lrc() {
    gchar *path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(path,get_lrc_path(focus_song));
    if (strcmp(path,"NULL")) {
        strcpy(searching_lrc,"");

        if (lrc_file_to_lrc_text(path)==FALSE) {
            lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_FAILED;
        }
        widget_lrc_init();
    } else {
        lrc_search_data.search_lrc_state =SEARCH_LRC_STATE_NONE;
    }
    g_free_n(path);
    return FALSE;
}
gpointer search_lrc(gpointer) {
    gchar *searching_lrc_pre= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(searching_lrc_pre,searching_lrc);

    if (configure.download_lrc_path[0]=='/') {
        if (configure.download_lrc_path[strlen(configure.download_lrc_path)-1]!='/')
            strcat(configure.download_lrc_path,"/");
        strcpy(lrc_search_data.lrc_to_save,configure.download_lrc_path);
    } else {
        if (!strncmp(focus_song,"<cue>",5)) {
            //cue:
            gchar *title=(gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
            gchar *performer=(gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
            gchar *cue_uri=(gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

            strcpy(performer,strstr(focus_song,"PERFORMER:"));
            sscanf(performer,"PERFORMER:%[^>]%*s",performer);
            strcpy(title,strstr(focus_song,"TITLE:"));
            sscanf(title,"TITLE:%[^>]%*s",title);
            strcpy(cue_uri,strstr(focus_song,"cue_uri:"));
            sscanf(cue_uri,"cue_uri:%[^>]",cue_uri);

            strcpy(lrc_search_data.lrc_to_save,g_path_get_dirname(cue_uri));
            strcat(lrc_search_data.lrc_to_save,"/");

            g_free_n(title);
            g_free_n(performer);
            g_free_n(cue_uri);
        } else {
            strcpy(lrc_search_data.lrc_to_save,g_path_get_dirname(focus_song));
            strcat(lrc_search_data.lrc_to_save,"/");
        }
    }
    gchar *playing_song_lrc_path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

    if (!strncmp(focus_song,"<cue>",5)) {
        //cue:
        gchar *title=(gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        gchar *performer=(gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        gchar *cue_uri=(gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

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

        g_free_n(title);
        g_free_n(performer);
        g_free_n(cue_uri);

    } else {
        gchar *path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        if (get_playing_song()==NULL)
            strcpy(path,find_lists_from_name(focus_lists)->songs->uri);
        else
            strcpy(path,get_playing_song());
        print_programming("get_lrc_file_path_at_custom_path\n");
        gchar *playing_song_path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

        strcpy(playing_song_path,g_path_get_basename(path));
        if (playing_song_path==NULL) {
            print_error("没有在播放音乐，无法播放窗口歌词\n");
            g_free_n(searching_lrc_pre);
            g_free_n(playing_song_lrc_path);
            return ;
        }
        strcpy(playing_song_lrc_path,playing_song_path_without_some_word(playing_song_path));
        g_free_n(playing_song_path);
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
    strcat(lrc_search_data.lrc_to_save,playing_song_lrc_path);
    g_free_n(playing_song_lrc_path);

    gchar  *hostname = "www.lrc123.com";
    gchar  *savepath = "/tmp/iceplayer.lrc.search.log";
    unlink(savepath);
    gchar *error= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

    gchar *filename= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    gint n=0,m=0;
    strcpy(filename,"/?keyword=");
    m=strlen(filename);
    while (lrc_search_data.keyword[n]) {
        if (lrc_search_data.keyword[n]==' ') {
            filename[m]=0;
            strcat(filename,"%20");
            m=strlen(filename);
            n++;
            while (lrc_search_data.keyword[n]==' ')n++;
        }
        filename[m++]=lrc_search_data.keyword[n++];
    }
    filename[m]=0;
    strcat(filename,"&field=all");
    //if(!configure.edit_search_word)

    lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_SEARCH;
    if (download(hostname,filename,savepath,error)) {
        g_free_n(filename);
        g_free_n(error);

        if (strcmp(searching_lrc_pre,searching_lrc)!=0) {
            g_free_n(searching_lrc_pre);
            g_free_n(playing_song_lrc_path);

            return ;
        }
        FILE *file=fopen("/tmp/iceplayer.lrc.search.log","r");
        gint result=0;
        //gint count=0;
        main_lrc_search= (iceplayer_lrc_search *)malloc(sizeof(iceplayer_lrc_search));
        main_lrc_search->alubm[0]=0;
        main_lrc_search->title[0]=0;
        main_lrc_search->editor[0]=0;
        main_lrc_search->uri[0]=0;
        main_lrc_search->next=0;
        iceplayer_lrc_search *lrc_search=main_lrc_search;
        lrc_search->next=NULL;
        if (file) {
            gchar *text= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
            while (fgets(text,STRINGS_LENGTH,file)) {

                if (strstr(text,"<div class=\"ft_r\">共")) {
                    strcpy(text,strstr(text,"共"));
                    sscanf(text,"共%d个结果",&result);
                    g_print("结果：%d\n",result);
                    if (result==0) {
                        lrc_search_data.search_lrc_state =  SEARCH_LRC_STATE_NOT_FOUND;
                        break;
                    } else {
                        lrc_search_data.search_lrc_state =  SEARCH_LRC_STATE_SEARCHED;
                    }
                    continue ;
                }
                if (strstr(text,"歌手:")) {
                    if (strstr(text,"<"))strcpy(text,strstr(text,"<"));
                    gint n=0,m=0;
                    while (text[n]) {
                        if (text[n]=='\n') {
                            text[n]=0;
                        }
                        if (text[n]==13) {
                            text[n]=0;
                        }
                        if (text[n]=='<') {
                            while (text[n]!='>'&&text[n])
                                n++;
                            if (!text[n])break;
                            n++;
                        } else
                            lrc_search->editor[m++]=text[n++];
                    }
                    lrc_search->editor[m]=0;
                    continue ;
                }
                if (strstr(text,"专辑:")) {
                    if (strstr(text,"<"))strcpy(text,strstr(text,"<"));
                    gint n=0,m=0;
                    while (text[n]) {
                        if (text[n]=='\n') {
                            text[n]=0;
                        }
                        if (text[n]==13) {
                            text[n]=0;
                        }
                        if (text[n]=='<') {
                            while (text[n]!='>'&&text[n])
                                n++;
                            if (!text[n])break;
                            n++;
                            continue ;
                        }
                        lrc_search->alubm[m++]=text[n++];
                    }
                    lrc_search->alubm[m]=0;
                    continue ;
                }
                if (strstr(text,"歌曲:")) {
                    if (strstr(text,"<"))strcpy(text,strstr(text,"<"));
                    gint n=0,m=0;
                    while (text[n]) {
                        if (text[n]=='\n') {
                            text[n]=0;
                        }
                        if (text[n]==13) {
                            text[n]=0;
                        }
                        if (text[n]=='<') {
                            while (text[n]!='>'&&text[n])
                                n++;
                            if (!text[n])break;
                            n++;
                            continue ;
                        }
                        lrc_search->title[m++]=text[n++];
                    }
                    lrc_search->title[m]=0;
                    continue ;
                }
                if (strstr(text,"下载:")) {
                    fgets(text,STRINGS_LENGTH,file);
                    strcpy(text,strstr(text,"<a href="));
                    sscanf(text,"<a href=\"%[^\"]\">",lrc_search->uri);

                    lrc_search->next=(iceplayer_lrc_search *)malloc(sizeof(iceplayer_lrc_search));
                    lrc_search->next->next=NULL;
                    lrc_search=lrc_search->next;
                    continue ;
                }
            }
            g_free_n(text);
            print_debug("连接选择对话框线程");
            g_timeout_add(50,G_SOURCE_FUNC(choose_lrc_to_download),main_lrc_search);
            fclose(file);
        } else {
            print_error("下载歌词逻辑错误。\n");
        }
    } else {
        print_error("搜索歌词失败。\n");

        g_timeout_add(3000,back_to_lrc,NULL);
    }
    g_free_n(searching_lrc_pre);
}
void lrc_init() {
    print_programming("lrc_init\n");
    widget_lrc_Independent_init();
    desktop_lrc_init();
}
gboolean lrc_refresh() {
    print_programming("lrc_refresh\n");
    if (configure.lrc) {

        if (configure.desktop_lrc)gtk_widget_queue_draw(desktop_lrc.window);
        widget_lrc_refresh(FALSE);

    } else {
        static gint cou=1;
        cou++;
        if (cou==20) {
            cou=1;

            if (GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned))gtk_label_set_text(GTK_LABEL(iceplayer.widget_lrc_label_hpaned),_("歌词功能关闭"));
            if (GTK_IS_WIDGET(iceplayer.widget_lrc_label))gtk_label_set_text(GTK_LABEL(iceplayer.widget_lrc_label),_("歌词功能关闭"));
            if (GTK_IS_WIDGET(widget_lrc.label))gtk_label_set_text(GTK_LABEL(widget_lrc.label),_("歌词功能关闭"));
        }
    }
    return TRUE;
}
gchar *get_lrc_path(gchar song_uri[STRINGS_LENGTH]) {
    gchar *path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(path,"");
    FILE *file = fopen(get_lrc_file_path_at_custom_path(song_uri,1),"r");
    if (file) {
        fclose(file);
        strcpy(path,get_lrc_file_path_at_custom_path(song_uri,1));
    } else {
        file = fopen(get_lrc_file_path_at_custom_path(song_uri,2),"r");
        if (file) {
            fclose(file);
            strcpy(path,get_lrc_file_path_at_custom_path(song_uri,2));
        } else {
            file = fopen(get_lrc_file_path_at_custom_path(song_uri,3),"r");
            if (file) {
                fclose(file);
                strcpy(path,get_lrc_file_path_at_custom_path(song_uri,3));
            } else {
                file = fopen(get_lrc_file_path_at_custom_path(song_uri,4),"r");
                if (file) {
                    fclose(file);
                    strcpy(path,get_lrc_file_path_at_custom_path(song_uri,4));
                } else {
                    strcpy(path, get_lrc_file_path_at_song_path(song_uri));
                }
            }
        }
    }
    file = fopen(path,"r");
    if (file) {
        fclose(file);
        gboolean ref(gpointer *er) {
            g_free_n(er);
            return FALSE;
        }
        g_timeout_add(10,G_SOURCE_FUNC(ref),NULL);
        g_print("Debug: 匹配歌词->%s\n",path);
        return path;
    } else {
        g_free_n(path);
        return "NULL";
    }
}
void search_lrc_pre() {
    lrc_search_data.search_lrc_state=SEARCH_LRC_STATE_SEARCH;
    gint n=0;
    while (lrc_text[n][0]) {
        strcpy(lrc_text[n++],"");
        if (n>=256)
            break;
    }

    if (configure.search_lrc) {
        if (strncmp(focus_song,"<cue>",5)) {
            if (configure.search_filename_or_id3) {
                strcpy(lrc_search_data.keyword,g_basename(focus_song));
                gint n=strlen(lrc_search_data.keyword);
                while (lrc_search_data.keyword[n]!='.'||n==0)
                    n--;
                if (n!=0)
                    lrc_search_data.keyword[n]=0;
            } else {
                //  ID3_tag *id3 = get_tag(focus_song);
                //strcpy(lrc_search_data.keyword,id3->song);
                // strcat(lrc_search_data.keyword," ");
                //strcat(lrc_search_data.keyword,id3->artist);
            }
        } else {
            gchar *title= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
            gchar *performer= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

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
            strcpy(lrc_search_data.keyword,title);
            strcat(lrc_search_data.keyword," ");
            strcat(lrc_search_data.keyword,performer);

            g_free_n(title);
            g_free_n(performer);
        }
        gchar * name= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        gint sa=0;
        gint sd=0;
        while (1) {
            if (lrc_search_data.keyword[sa]==-17&&lrc_search_data.keyword[sa+1]==-68&&lrc_search_data.keyword[sa+2]==-115) {
                name[sd] = ' ';
                sa=sa+3;
                sd++;
                if (sa>=strlen(lrc_search_data.keyword))break;
                continue ;
            }
            if (lrc_search_data.keyword[sa]=='-'||lrc_search_data.keyword[sa]=='_'||lrc_search_data.keyword[sa]=='{'||lrc_search_data.keyword[sa]=='}'||lrc_search_data.keyword[sa]=='('||lrc_search_data.keyword[sa]==')'||lrc_search_data.keyword[sa]=='['||lrc_search_data.keyword[sa]==']'||lrc_search_data.keyword[sa]=='\\'||lrc_search_data.keyword[sa]=='/'||lrc_search_data.keyword[sa]=='`') {
                name[sd] = ' ';
            } else {
                name[sd] = lrc_search_data.keyword[sa];
            }
            sa++;
            sd++;
            if (sa>=strlen(lrc_search_data.keyword))break;
        }
        name[sd]=0;
        sd --;
        while (name[sd] ==' ') {
            name[sd]=0;
            sd--;
        }
        strcpy(lrc_search_data.keyword,name);
        strcpy(searching_lrc,name);
        g_free_n(name);
        print_debug(lrc_search_data.keyword);
        g_thread_create((void *)search_lrc,NULL,FALSE,NULL);
    }
}
gboolean in_search_black_list() {
    print_programming("in_search_black_list\n");
    print_debug("是否匹配设定的黑名单？");
    gchar  *black_list_path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    gchar  *black_list_text= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(black_list_path,g_get_home_dir());
    strcat(black_list_path,"/.iceplayer/black_list.conf");//-");

    FILE *file=fopen(black_list_path,"r");
    gboolean in=FALSE;
    if(file){
    while(fgets(black_list_text,1000,file)) {
        if(black_list_text[strlen(black_list_text)-1]=='\n')
            black_list_text[strlen(black_list_text)-1]=0;
        if(strstr(focus_song,black_list_text)!=NULL) {
            in=TRUE;
            break;
        }
    }
    fclose(file);
    if(in) {
        print_debug("匹配，将屏蔽此歌曲，并notify通知");
        print_notify("此歌曲已被屏蔽搜索下载歌词",focus_song);
        return TRUE;
    } else {
        print_debug("不匹配，继续……");
        return FALSE;
    }
    }
    g_free_n(black_list_text);
    g_free_n(black_list_path);
    return TRUE;
}
gboolean lrc() {
    print_programming("lrc\n");
    if(!configure.lrc)return FALSE;
    gchar *path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(path,get_lrc_path(focus_song));
    if (strcmp(path,"NULL")) {
        strcpy(searching_lrc,"");

        if (lrc_file_to_lrc_text(path)==FALSE) {
            lrc_search_data.search_lrc_state = SEARCH_LRC_STATE_FAILED;
        }
        widget_lrc_init();
    } else {
        lrc_text[0][0]=0;

        wait=FALSE;
        print_error("不匹配任何本地歌词\n");
        if (configure.search_lrc&&!in_search_black_list())search_lrc_pre();
    }
    g_free_n(path);
    /*
      if(GTK_IS_WIDGET(iceplayer.widget_lrc_label))gtk_label_set_text(iceplayer.widget_lrc_label,"");
      if(GTK_IS_WIDGET(iceplayer.widget_lrc_label_hpaned))gtk_label_set_text(iceplayer.widget_lrc_label_hpaned,"");
      if(GTK_IS_WIDGET(widget_lrc.label))gtk_label_set_text(widget_lrc.label,"");*/
    return FALSE;
}
gchar *get_lrc_file_path_at_song_path(gchar song_uri[STRINGS_LENGTH]) {
    gchar *song_lrc_path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    if (!strncmp(focus_song,"<cue>",5)) {
        //cue:

        gchar *title= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        gchar *performer= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        gchar *cue_uri= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

        strcpy(performer,strstr(song_uri,"PERFORMER:"));
        sscanf(performer,"PERFORMER:%[^>]%*s",performer);
        strcpy(title,strstr(song_uri,"TITLE:"));
        sscanf(title,"TITLE:%[^>]%*s",title);
        strcpy(cue_uri,strstr(song_uri,"cue_uri:"));
        sscanf(cue_uri,"cue_uri:%[^>]",cue_uri);

        sprintf(song_lrc_path,"%s/%s - %s.lrc",g_path_get_dirname(cue_uri),performer,title);

        g_free_n(title);
        g_free_n(performer);
        g_free_n(cue_uri);
    } else {
        strcpy(song_lrc_path,song_uri);

        gint n=strlen(song_lrc_path);
        while (1) {
            if (song_lrc_path[n]=='.')
                break;
            n--;
            if (n<=0) {
                n=strlen(song_lrc_path);
                break;
            }
        }
        song_lrc_path[n+1]=0;
        strcat(song_lrc_path,"lrc");
    }

    gboolean ref(gchar *a) {
        g_free_n(a);
        return FALSE;
    }
    g_timeout_add(10,G_SOURCE_FUNC(ref),song_lrc_path);
    return song_lrc_path;
}
gchar *get_lrc_file_path_at_custom_path(gchar song_uri[STRINGS_LENGTH],gint type) {
    gchar *song_lrc_path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    switch (type) {
    case 1: {
        if (gtk_entry_get_text(GTK_ENTRY(configure_widget.local_lrc_path_1_entry))!=NULL)
	  strcpy(song_lrc_path, gtk_entry_get_text(GTK_ENTRY(configure_widget.local_lrc_path_1_entry )));
        break;
    }
    case 2: {
      if (gtk_entry_get_text(GTK_ENTRY(configure_widget.local_lrc_path_2_entry))!=NULL)
	strcpy(song_lrc_path,gtk_entry_get_text(GTK_ENTRY(configure_widget.local_lrc_path_2_entry)));
        break;
    }
    case 3: {
      if (gtk_entry_get_text(GTK_ENTRY(configure_widget.local_lrc_path_3_entry))!=NULL)
	strcpy(song_lrc_path,gtk_entry_get_text(GTK_ENTRY(configure_widget.local_lrc_path_3_entry)));
        break;
    }
    case 4: {
      if (gtk_entry_get_text(GTK_ENTRY(configure_widget.lrc_download_path_entry))!=NULL)
	strcpy(song_lrc_path,gtk_entry_get_text(GTK_ENTRY(configure_widget.lrc_download_path_entry)));
      break;
    }
    }

    if (song_lrc_path[strlen(song_lrc_path)-1]!='/')
        strcat(song_lrc_path,"/");


    if (!strncmp(song_uri,"<cue>",5)) {
        //cue:
        gchar *title= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        gchar *performer=(gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        gchar *cue_uri=(gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

        strcpy(performer,strstr(song_uri,"PERFORMER:"));
        sscanf(performer,"PERFORMER:%[^>]",performer);
        gint n=0;
        while (performer[n]!='>')n++;
        performer[n]=0;

        strcpy(title,strstr(song_uri,"TITLE:"));
        sscanf(title,"TITLE:%[^>]",title);
        n=0;
        while (title[n]!='>')n++;
        title[n]=0;
        sprintf(song_lrc_path,"%s%s - %s.lrc",song_lrc_path,performer,title);
        gboolean ref(gchar *a) {
            g_free_n(a);
            return FALSE;
        }
        g_timeout_add(10,G_SOURCE_FUNC(ref),song_lrc_path);
        g_free_n(title);
        g_free_n(performer);
        g_free_n(cue_uri);
        return song_lrc_path;
    } else {
        gchar *path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        strcpy(path,g_basename(song_uri));
        gint n=strlen(path);
        while (1) {
            if (path[n]=='.')
                break;
            n--;
            if (n<=0) {
                n=strlen(path);
                break;
            }
        }
        path[n+1]=0;
        strcat(path,"lrc");
        strcat(song_lrc_path,path);
        g_free_n(path);
        gboolean ref(gchar *a) {
            g_free_n(a);
            return FALSE;
        }
        g_timeout_add(10,G_SOURCE_FUNC(ref),song_lrc_path);
        return song_lrc_path;
    }
}
gboolean lrc_file_to_lrc_text(gchar lrc_path[STRINGS_LENGTH]) {
    print_programming("lrc_file_to_lrc_text\n");
    gint n=0;
    while (lrc_text[n][0]) {
        strcpy(lrc_text[n++],"");
        if (n>=256)
            break;
    }

    FILE* lrc_file = fopen(lrc_path,"r");
    if (lrc_file) {
        gchar *text= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        if (!fgets(text,STRINGS_LENGTH,lrc_file)) {
            if (lrc_file) {
                fclose(lrc_file);
                lrc_file = 0;
            }

            print_error("歌词文件内容为空\n");
            dialog(_("歌词文件内容为空，请重新下载"));
            g_free_n(text);
            return FALSE;
        }
        rewind(lrc_file);
        gint line_number = 0;
        while (fgets(text,STRINGS_LENGTH,lrc_file)) {
            if (text[0]==-17) {
                gchar *text2= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
                gint a=0;
                gint b=0;
                while (text[a]!='[') {
                    a++;
                    if (text[a]==0)
                        break;
                }
                if (text[a]=='[') {
                    while (text[a]) {
                        text2[b]=text[a];
                        a++;
                        b++;
                    }
                    text2[b]=0;
                    strcpy(text,text2);
                } else {
                    g_free_n(text2);
                    continue;
                }
                g_free_n(text2);
            }

            if (!g_utf8_validate(text,-1,NULL)) {
                gsize r,w;
                gchar *str = g_convert(text,-1,"UTF-8","GB18030",&r,&w,NULL);
                if (str)  strcpy(text,str);
                g_free_n(str);
            }

            gint n=0;
            while (text[n]) {
                if (text[n] == '\n') text[n] = ' ';
                n++;
            }
            if (text[0]=='[') {

                ///[00:00.00]
                if (text[0]=='['&&text[3] ==':'&&text[6]=='.'&&text[9]==']') {
                    gint n = strlen(text);
                    while (text[n-9]!='[') {
                        if (text[n]==13)text[n]='\0';
                        n--;
                        if (n==0)break;
                    }

                    gchar *word= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));//歌词文字
                    n++;

                    gint s = 0;
                    while (text[n]) word[s++]=text[n++];
                    word[s]='\0';

                    gint k=0;
                    while (text[k*10]=='['&&text[k*10+9]==']') {
                        gint n=k*10;
                        gint s = 0;
                        while (n!=k*10+10)
                            lrc_text[line_number][s++] = text[n++];
                        lrc_text[line_number][s]='\0';
                        strcat(lrc_text[line_number],word);
                        line_number++;
                        k++;
                    }
                    g_free_n(word);
                }
                ///[00:00.0]
                if (text[0]=='['&&text[3] ==':'&&text[6]=='.'&&text[8]==']') {
                    gint n = strlen(text);
                    while (text[n-8]!='[') {
                        if (text[n]==13)text[n]='\0';
                        n--;
                        if (n==0)break;
                    }

                    gchar *word= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));//歌词文字
                    n++;

                    gint s = 0;
                    while (text[n]) word[s++]=text[n++];
                    word[s]='\0';

                    gint k=0;
                    while (text[k*9]=='['&&text[k*9+8]==']') {
                        gint n=k*9;
                        gint s = 0;
                        while (n!=k*9+9)
                            lrc_text[line_number][s++] = text[n++];
                        lrc_text[line_number][s]='\0';
                        strcat(lrc_text[line_number],word);
                        line_number++;
                        k++;
                    }
                    g_free_n(word);
                }
                ///[00:00]

                if (text[0]=='['&&text[3] ==':'&&text[6]==']') {

                    gint n = strlen(text);
                    while (text[n-6]!='[') {
                        if (text[n]==13)text[n]='\0';
                        n--;
                        if (n==0)break;
                    }

                    gchar *word= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));//歌词文字
                    n++;

                    gint s = 0;
                    while (text[n]) word[s++]=text[n++];
                    word[s]='\0';

                    gint k=0;
                    while (text[k*6]=='['&&text[k*6+6]==']') {
                        gint n=k*6;
                        gint s = 0;
                        while (n!=k*6+6)
                            lrc_text[line_number][s++] = text[n++];
                        lrc_text[line_number][s]='\0';
                        strcat(lrc_text[line_number],".00]");
                        strcat(lrc_text[line_number],word);


                        line_number++;
                        k++;
                    }
                    g_free_n(word);
                }
            }
        }
        if (lrc_file) {
            fclose(lrc_file);
            lrc_file=0;
        }
        gint b=0,m=0;
        m=line_number-1;


        if (lrc_text[0][0]) ///*冒泡排序*/
            for (n=0; n<m-1; n++)
                for (b=0; b<m-n-1; b++)
                    if (strcmp(lrc_text[b],lrc_text[b+1])>0) {
                        gchar *text= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
                        strcpy(text,lrc_text[b]);
                        strcpy(lrc_text[b],lrc_text[b+1]);
                        strcpy(lrc_text[b+1],text);
                        g_free_n(text);
                    }
        g_free_n(text);
        return TRUE;
    } else {
        print_error("打开歌词文件失败？%s\n",lrc_path);
        lrc_text[0][0]=0;
        return FALSE;
    }
}
gint get_lrc_number() {
    print_programming("get_lrc_number\n");
    if (!core.pipeline||!lrc_text[0][0]) {
        print_programming("out get_lrc_number\n");
        return 0;
    }
    gint line_number=0;
    //get play time
    gint min,sec,mir;
    min = (core.pos )/60000000000;
    sec = ((core.pos/10000000) % 6000) / 100;
    mir = (core.pos/10000000) % 100;
    if (!strncmp(focus_song,"<cue>",5)) {
        gint min_pre,sec_pre,mir_pre;
        gchar *p= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
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
        min = sec>sec_pre?min-min_pre:min-min_pre-1;
        sec = sec>sec_pre?sec-sec_pre:sec-sec_pre+60;
        if (mir<mir_pre)sec--;
        mir = mir>mir_pre?mir-mir_pre:mir-mir_pre+100;
    }
    gchar *A= (gchar *)g_malloc(50*sizeof(gchar *));
    g_snprintf(A,60,"[%02d:%02d.%02d]",min,sec,mir);
    gint n=0;
    while (1) {
        n++;
        if (n==200) {
            g_free_n(A);
            return 0;
        }
        if (!strncmp(A,(const char *)lrc_text[0],9)<0) {
            line_number =0;
            g_free_n(A);
            return 0;
        }
        if (line_number>1&&!lrc_text[line_number-1][0]) {
            g_free_n(A);
            return 0;
        }
        gint a = strncmp(A,lrc_text[line_number-1],9);
        if (!lrc_text[line_number][0]) {
            g_free_n(A);
            return 0;
        }
        //gint b = strncmp(A,lrc_text[line_number],9);

        gint c;
        if (lrc_text[line_number+1][0]) {
            c = strncmp(A,lrc_text[line_number+1],9);
        } else {
            g_free_n(A);
            return 0;
        }
        if (a>0&&c<=0) {
            g_free_n(A);
            return line_number ;
        }
        if (a<0&&c<=0) {
            line_number --;
            continue;
        }
        if (a>0&&c>=0) {
            line_number ++;
            continue;
        }

        if (!lrc_text[line_number][0]) {
            g_free_n(A);
            return line_number -1;
        }
        if (line_number<=0) {
            g_free_n(A);
            return 0;
        }

        g_free_n(A);
        return line_number;
    }
    g_free_n(A);
    return 0;
}
gchar *lrc_text_get_text(gchar *lrc_text_with_time) {

    print_programming("lrc_text_get_text\n");
    gchar *tmp= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    gint m=0;
    strcpy(tmp,lrc_text_with_time);
    gint n=0;
    while (tmp[n]!=']')
        n++;
    n++;
    while (tmp[m+n]) {
        tmp[m]=tmp[m+n];
        m++;
    }
    tmp[m]=0;
    print_programming("out lrc_text_get_text\n");
    gboolean ref(gchar *a) {
        g_free_n(a);
        return FALSE;
    }
    g_timeout_add(10,G_SOURCE_FUNC(ref),tmp);
    return tmp;
}


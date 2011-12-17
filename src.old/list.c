/*
*
*
*
*
*    多列表栏和歌曲栏的列表操作
*    其ui的实现参考ui.c的ui_init()与single的有lists和songs为名的函数
*
*
*
*
*/

#include "core.h"
#include "single.h"
#include "list.h"
#include "ui.h"
#include "lrc.h"

///一些变量:
//in 8.04 reuest: typedef struct _GFile GFile;

iceplayer_lists main_lists;//根列表
iceplayer_list_menu list_menu;
gchar focus_lists[STRINGS_LENGTH];//所有列表中名字与focus_lists相同的列表将高亮显示
gchar focus_song[STRINGS_LENGTH];//所有列表中与focus_song相同的歌曲将高亮显示
gchar playing_song[30];//正在播放的项目的 GtkTreePath 的 string，比如"23"就是第二十三首歌
gchar playing_list[STRINGS_LENGTH];//正在播放的项目所在的列表名

///init:
//初始化多列表栏的属性，就一个text栏
void init_lists() {
    print_programming("init_lists\n");
    GtkCellRenderer *renderer;
    GtkListStore *store;
    renderer = gtk_cell_renderer_text_new();

    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(iceplayer.list_lists_treeview), 0, " ", renderer, "text",0,"weight",1, NULL);

    store = gtk_list_store_new(3, G_TYPE_STRING,G_TYPE_INT,G_TYPE_STRING);
    gtk_tree_view_set_model(GTK_TREE_VIEW(iceplayer.list_lists_treeview),GTK_TREE_MODEL(store));
    g_object_unref(store);

}

//初始化歌曲的属性，一个正在播放的指示栏，一个歌曲的显示名，一个歌曲的实际地址，如果是cue文件就还有其歌曲信息
void init_songs() {
    print_programming("init_songs\n");
    GtkCellRenderer *renderer;
    GtkListStore *store;
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(iceplayer.list_songs_treeview), 0, " ", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(iceplayer.list_songs_treeview), 0, " ", renderer, "text", 1,"weight", 3,  NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(iceplayer.list_songs_treeview), 0, " ", renderer, "text", 2, NULL);
    store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING,G_TYPE_STRING,G_TYPE_INT);
    gtk_tree_view_set_model(GTK_TREE_VIEW(iceplayer.list_songs_treeview),GTK_TREE_MODEL(store));
    g_object_unref(store);
    gtk_tree_view_column_set_visible(gtk_tree_view_get_column((GtkTreeView *)iceplayer.list_songs_treeview,2),FALSE);
    gtk_tree_view_set_search_column(GTK_TREE_VIEW(iceplayer.list_songs_treeview),1);
}

//读取多播放列表
gboolean list_init_lists() {
    print_programming("list_init_lists\n");
    print_debug("初始化播放列表");

    main_lists.focus_song[0]=0;
    main_lists.name[0]=0;
    main_lists.songs= (iceplayer_songs *)malloc(sizeof(iceplayer_songs));
    main_lists.songs->uri[0]=0;
    main_lists.songs->next=NULL;
    main_lists.next=NULL;

    init_lists();
    init_songs();

    read_lists();
    print_debug("初始化播放列表success!");
    return FALSE;
}

//返回列表文件，在 ~/.iceplayer/lists.conf
gchar * get_lists_file_path () {
    static gchar lists_file_path[120];
    if (!lists_file_path[0]) {
        strcpy(lists_file_path,g_get_home_dir());
        strcat(lists_file_path,"/.iceplayer/lists.conf");
    }
    return lists_file_path;
}

//返回歌曲文件，在~/.iceplayer/songs/name.songs
gchar * get_songs_file_path (gchar name[STRINGS_LENGTH]) {
    static gchar songs_file_path[120];
    strcpy(songs_file_path,g_get_home_dir());
    strcat(songs_file_path,"/.iceplayer/songs/");
    strcat(songs_file_path,name);
    strcat(songs_file_path,".songs");
    return songs_file_path;
}

///lists:
//新建列表，如果name已经存在，则新建name+n
gchar *add_new_lists(gchar name[STRINGS_LENGTH]) {
    print_programming("add_new_lists\n");
    static gchar name_for_check[310];
    gint n=2;
    strcpy(name_for_check,name);
    while (find_lists_from_name(name_for_check)) sprintf(name_for_check,"%s_%d",name,n++);

    iceplayer_lists *lists_tmp = &main_lists;
    while (lists_tmp->next!=NULL)lists_tmp = lists_tmp->next;

    if (lists_tmp->name[0]) {
        iceplayer_lists *lists = (iceplayer_lists *)malloc(sizeof(iceplayer_lists));
        strcpy(lists->name,name_for_check);
        lists->songs = (iceplayer_songs *)malloc(sizeof(iceplayer_songs));
        lists->songs= (iceplayer_songs *)malloc(sizeof(iceplayer_songs));
        lists->songs->uri[0]=0;
        lists->songs->next=NULL;
        lists->focus_song[0]=0;
        lists->next = lists_tmp->next;
        lists_tmp->next = lists;
    } else {
        strcpy(focus_lists,main_lists.name);
        lists_tmp->focus_song[0]=0;
        lists_tmp->next=NULL;
        lists_tmp->songs = (iceplayer_songs *)malloc(sizeof(iceplayer_songs));
        lists_tmp->songs->uri[0]=0;
        lists_tmp->songs->next=NULL;
        strcpy(lists_tmp->name,name_for_check);
    }
    find_lists_from_name(name_for_check);
    return name_for_check;
}

//从name返回列表
iceplayer_lists *find_lists_from_name(gchar name[STRINGS_LENGTH]) {
    print_programming("find_lists_from_name\n");
    iceplayer_lists *lists_tmp = &main_lists;
    if (!name[0])return lists_tmp;
    if (!lists_tmp->next)
        if (!strcmp(lists_tmp->name,name))return lists_tmp;
    while (lists_tmp!=NULL) {
        if (!strcmp(lists_tmp->name,name))return lists_tmp;
        lists_tmp = lists_tmp->next;
    }
    return NULL;
}
gboolean show_lists_foreach(GtkTreeModel *model,GtkTreePath *path,GtkTreeIter *iter,gchar new_list[STRINGS_LENGTH]) {
    print_programming("show_lists_foreach\n");
    gchar *name;
    gtk_tree_model_get(model, iter, 0, &name,-1);
///顺序不能改
    if (!strcmp(name,playing_list)) {
        gtk_list_store_set(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(iceplayer.list_lists_treeview))), iter,0,name,1,PANGO_WEIGHT_BOLD,-1);
    } else if (!strcmp(name,focus_lists)) {
        gtk_list_store_set(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(iceplayer.list_lists_treeview))), iter,0,name,1,PANGO_WEIGHT_NORMAL, -1);
    } else if (!strcmp(name,new_list))
        gtk_list_store_set(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(iceplayer.list_lists_treeview))), iter,0,name,1,PANGO_WEIGHT_NORMAL, -1);
    g_free_n(name);
    return FALSE;
}
void show_lists(gchar new_list[STRINGS_LENGTH]) {
    print_programming("show_lists\n");
    if (new_list==NULL) {
        iceplayer_lists *lists_tmp = &main_lists;
        ///
        GtkTreeIter iter;
        GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(iceplayer.list_lists_treeview)));
        gtk_list_store_clear(store);
        if (!lists_tmp->name[0])return;
        while (lists_tmp) {
            gtk_list_store_append(store, &iter);
            if (!strcmp(lists_tmp->name,playing_list)) {
                gtk_list_store_set(store, &iter,0,lists_tmp->name,1,PANGO_WEIGHT_ULTRAHEAVY , 2,"black",-1);
            } else {
                gtk_list_store_set(store, &iter,0,lists_tmp->name,1,PANGO_WEIGHT_NORMAL , 2,"black",-1);
            }
            lists_tmp = lists_tmp->next;
        }
    } else {
        gtk_tree_model_foreach(gtk_tree_view_get_model( (GtkTreeView *)iceplayer.list_lists_treeview), (GtkTreeModelForeachFunc) show_lists_foreach,new_list);
    }
}
iceplayer_lists *find_lists_from_next(iceplayer_lists *lists) {
    print_programming("find_lists_from_next\n");
    iceplayer_lists *lists_tmp = &main_lists;
    if (!strcmp(lists_tmp->name,lists->name))return NULL;
    while (lists_tmp) {
        if (!lists_tmp->next)return NULL;
        if (!strcmp(lists_tmp->next->name,lists->name))return lists_tmp;
        lists_tmp = lists_tmp->next;
    }
    return NULL;
}
iceplayer_lists *find_lists_from_random() {
    print_programming("find_lists_from_random\n");
    iceplayer_lists *lists = &main_lists;
    gint index=1;
    while (lists->next) {
        lists=lists->next;
        index++;
    }
    srand((unsigned)time(NULL)+1);
    index=rand()%index;
    lists = &main_lists;
    while (index>0) {
        if (lists->next) lists=lists->next;
        index--;
    }
    return lists;
}

void free_link(iceplayer_songs *head) {
    iceplayer_songs *current;
    current=head;
    while(current!=NULL) {
        head = current->next;
        g_free_n(current);
        current = NULL;
        current = head;
    }
    head=NULL;
}

void delete_lists(iceplayer_lists *lists) {
    print_programming("iceplayer_lists\n");

    //unlink(get_songs_file_path(lists->name));
    if (!strcmp(lists->name,main_lists.name)) { //只能假设是main_lists了
        if (lists->next!=NULL) { //删除main_lists
            if (!strcmp(playing_list,lists->name)) {
                stop();
                playing_song[0]=' ';
                playing_list[0]=' ';
            }
            strcpy( focus_lists,lists->next->name);
            strcpy(lists->focus_song,lists->next->focus_song);
            strcpy(lists->name,lists->next->name);
            iceplayer_songs *a = lists->songs;
            iceplayer_songs *b = (iceplayer_songs *)lists->next ;
            lists->songs = lists->next->songs;
            lists->next = lists->next->next;
            free_link(a);
            free_link(b);
        } else { //只有main_lists一个
            stop();
            focus_lists[0]=0;
            main_lists.focus_song[0]=0;
            main_lists.name[0]=0;
            free_link(main_lists.songs);
            main_lists.songs= (iceplayer_songs *)malloc(sizeof(iceplayer_songs));
            main_lists.songs->uri[0]=0;
            main_lists.songs->next=NULL;
            main_lists.next=NULL;
            focus_lists[0]=' ';
            playing_song[0]=' ';
            playing_list[0]=' ';
        }
    } else {
        if (!strcmp(playing_list,lists->name)) {
            stop();
            playing_song[0]=' ';
            playing_list[0]=' ';
        }
        if (lists->next) {
            strcpy( focus_lists,lists->next->name);
            strcpy(lists->focus_song,lists->next->focus_song);
            strcpy(lists->name,lists->next->name);
            iceplayer_songs *a=lists->songs;
            lists->songs = lists->next->songs;
            lists->next = lists->next->next;
            free_link(a);
        } else {
            focus_lists[0]=0;
            iceplayer_lists *lists_tmp = &main_lists;
            while (strcmp(lists_tmp->next->name,lists->name)) {
                lists_tmp=lists_tmp->next;
            }
            iceplayer_songs *a=(iceplayer_songs *)lists_tmp->next;
            lists_tmp->next=NULL;
            free_link(a);
        }
    }

    show_lists(NULL);
    show_songs(NULL);
}
gboolean  rename_lists(gchar old_name[STRINGS_LENGTH],gchar new_name[STRINGS_LENGTH]) {
    print_programming("rename_lists\n");
    iceplayer_lists *lists= &main_lists;
    while (strcmp(old_name,lists->name)) {
        lists=lists->next;
        if (lists==NULL)return FALSE;
    }

    static gchar name_for_check[310];
    gint n=2;
    strcpy(name_for_check,new_name);
    while (find_lists_from_name(name_for_check)) sprintf(name_for_check,"%s_%d",new_name,n++);

    if (!strcmp(focus_lists,old_name))
        strcpy(focus_lists,name_for_check);
    if (!strcmp(playing_list,old_name))
        strcpy(playing_list,name_for_check);
    strcpy(lists->name,name_for_check);
    show_lists(NULL);
    show_songs(NULL);
    return TRUE;
}
///songs:
gchar *songs_uri_get_show_name(gchar *uri) {
    if (strstr(uri,"<cue>")!=NULL) {
        //cue:
        static gchar show_name[STRINGS_LENGTH];
        gchar track[STRINGS_LENGTH];
        gchar title[STRINGS_LENGTH];
        gchar performer[STRINGS_LENGTH];
        strcpy(track,strstr(uri,"TRACK:"));
        sscanf(track,"TRACK:%[^>]",track);
        gint n=0;
        while (track[n]!='>')n++;
        track[n]=0;

        strcpy(performer,strstr(uri,"PERFORMER:"));
        sscanf(performer,"PERFORMER:%[^>]",performer);
        n=0;
        while (performer[n]!='>')n++;
        performer[n]=0;

        strcpy(title,strstr(uri,"TITLE:"));
        sscanf(title,"TITLE:%[^>]",title);
        n=0;
        while (title[n]!='>')n++;
        title[n]=0;

        sprintf(show_name,"%s %s-%s",track,performer,title);
        return show_name;
    }
    return (gchar *)g_basename(uri);
}
gboolean show_songs_foreach(GtkTreeModel *model,GtkTreePath *path,GtkTreeIter *iter,gchar new_song[STRINGS_LENGTH]) {
    print_programming("show_songs_foreach\n");

    gchar *value1=NULL,*value2=NULL;
    gtk_tree_model_get(model, iter, 0, &value1, 1,&value2,-1);

    if (!strcmp(value1,focus_song)) {
        gtk_list_store_set(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(iceplayer.list_songs_treeview))), iter,0,value1,1,value2,2,"  ", 3,PANGO_WEIGHT_NORMAL,-1);
    }
    if (!strcmp(value1,new_song))
        gtk_list_store_set(GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(iceplayer.list_songs_treeview))), iter,0,value1,1,value2,2,">",3,PANGO_WEIGHT_BOLD, -1);

    g_free_n(value1);
    g_free_n(value2);
    return FALSE;
}
void show_songs(gchar new_song[STRINGS_LENGTH]) {
    print_programming("show_songs\n");

    if (new_song == NULL) {
        iceplayer_lists *lists_tmp = find_lists_from_name(focus_lists);
        if (lists_tmp==NULL) {
            lists_tmp=&main_lists;
        }
        GtkTreeIter iter;
        GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(iceplayer.list_songs_treeview)));
        gtk_list_store_clear(store);

        iceplayer_songs *songs_tmp = lists_tmp->songs;

        if (lists_tmp->focus_song[0]==0)
            strcpy(lists_tmp->focus_song,gtk_tree_path_to_string(gtk_tree_path_new_first()));

        while (songs_tmp&&songs_tmp->uri&&songs_tmp->uri[0]) {
            gtk_list_store_append(store, &iter);

            if (strcmp(songs_tmp->uri,focus_song))
                gtk_list_store_set(store, &iter,0,songs_tmp->uri,1, songs_uri_get_show_name(songs_tmp->uri) ,2,"  ",3,PANGO_WEIGHT_NORMAL, -1);
            else
                gtk_list_store_set(store, &iter,0,songs_tmp->uri,1, songs_uri_get_show_name(songs_tmp->uri),2,">",3,PANGO_WEIGHT_BOLD, -1);

            songs_tmp = songs_tmp->next;
        }

        gtk_tree_view_set_cursor((GtkTreeView *) iceplayer.list_songs_treeview,gtk_tree_path_new_from_string(lists_tmp->focus_song), gtk_tree_view_get_column((GtkTreeView *) iceplayer.list_songs_treeview,0), FALSE);
    } else {
        gtk_tree_model_foreach(gtk_tree_view_get_model( (GtkTreeView *)iceplayer.list_songs_treeview), (GtkTreeModelForeachFunc) show_songs_foreach,new_song);
    }
}
gchar *  rename_songs(gchar old_uri[STRINGS_LENGTH],gchar new_name[STRINGS_LENGTH]) {
    print_programming("rename_lists\n");
    iceplayer_lists *lists= find_lists_from_name(focus_lists);
    iceplayer_songs *songs=lists->songs;
    while (strcmp(old_uri,songs->uri)) {
        songs=songs->next;
        if (songs==NULL) {
            print_error(_("找不到选中的歌曲?"));
            return _("找不到选中的歌曲?");
        }
    }
    gchar *new_lrc=(gchar *)g_malloc(STRINGS_LENGTH);
    gchar *new_dir=(gchar *)g_malloc(STRINGS_LENGTH);
    gchar *dir=(gchar *)g_malloc(STRINGS_LENGTH);
    gchar *dir_without_some_word=(gchar *)g_malloc(STRINGS_LENGTH);
    gchar *lrc=(gchar *)g_malloc(STRINGS_LENGTH);
    gchar *lrc_without_some_word=(gchar *)g_malloc(STRINGS_LENGTH);

    gchar *old_lrc=get_lrc_path(old_uri);
//dir
    strcpy(new_dir,old_uri);
    new_dir[strlen(old_uri)-strlen(g_basename(old_uri))]=0;
    strcat(new_dir,new_name);
    strcpy(songs->uri,new_dir);

    strcpy(dir,"mv \"");
    strcat(dir,old_uri);
    strcat(dir,"\" \"");
    strcat(dir,new_dir);
    strcat(dir,"\"");
    gint k=0,l=0;
    while (dir[k]) {
        if (dir[k]=='\'')
            dir_without_some_word[l++]='\\';
        else if (dir[k]=='\\')
            dir_without_some_word[l++]='\\';

        dir_without_some_word[l++]=dir[k++];
    }
    dir_without_some_word[l]=0;
    FILE *file=fopen(new_dir,"r");
    if (file) {
        fclose(file);
        return _("已存在目标文件");
    }
    system(dir_without_some_word);

//lrc

    if (old_lrc) {
        strcpy(new_lrc,old_lrc);
        new_lrc[strlen(old_lrc)-strlen(g_basename(old_lrc))]=0;
        gint i=strlen(new_name);
        while (new_name[i]!='.') {
            i--;
            if (i==0) {
                print_error("重命名歌词文件时无法定位后缀名");
                return  _("重命名歌词文件时无法定位后缀名?");
            }
        }
        strncat(new_lrc,new_name,i);
        strcat(new_lrc,".lrc");

        strcpy(lrc,"mv \"");
        strcat(lrc,old_lrc);
        strcat(lrc,"\" \"");
        strcat(lrc,new_lrc);
        strcat(lrc,"\"");


        gint k=0,l=0;
        while (lrc[k]) {
            if (lrc[k]=='\'')
                lrc_without_some_word[l++]='\\';
            else if (lrc[k]=='\\')
                lrc_without_some_word[l++]='\\';

            lrc_without_some_word[l++]=lrc[k++];
        }
        lrc_without_some_word[l]=0;
        system(lrc_without_some_word);

    }/*
    gchar *new_lrc=(gchar *)g_malloc(STRINGS_LENGTH);
    gchar *new_dir=(gchar *)g_malloc(STRINGS_LENGTH);
    gchar *dir=(gchar *)g_malloc(STRINGS_LENGTH);
    gchar *dir_without_some_word=(gchar *)g_malloc(STRINGS_LENGTH);
    gchar *old_lrc=get_lrc_path(old_uri);
    gchar *lrc=(gchar *)g_malloc(STRINGS_LENGTH);
    gchar *lrc_without_some_word=(gchar *)g_malloc(STRINGS_LENGTH);
    */
    g_free_n(new_lrc);
    g_free_n(new_dir);
    g_free_n(dir);
    g_free_n(dir_without_some_word);
    g_free_n(lrc);
    g_free_n(lrc_without_some_word);

    show_lists(NULL);
    show_songs(NULL);
    return  _("成功");
}
///
void set_drag() {
    //文件拖放
    //GtkTargetEntry dest_targets[ ] = { {(char*) "text/uri-list", 0, 0} };
    GtkTargetList* list = gtk_drag_dest_get_target_list( GTK_WIDGET (iceplayer.list_songs_treeview));
    gtk_target_list_add_uri_targets(list,0);
    gtk_drag_dest_set_target_list( GTK_WIDGET (iceplayer.list_songs_treeview),list);
    //gtk_drag_dest_set   ( GTK_WIDGET (treeview), 0,  dest_targets, G_N_ELEMENTS (dest_targets),GDK_ACTION_COPY );
}
void unset_drag() {
    /*   gtk_drag_source_unset(treeview);*/
    /*  gtk_drag_dest_unset(treeview);*/
}
gchar *concat_uris (gchar **uris, gsize *length) {
    print_programming("concat_uris\n");
    GString *string=NULL;
    gsize len=0;
    guint i=0;

    for (i = 0; uris[i]; ++i)
        len += strlen (uris[i]) + 1;

    if (length)
        *length = len;

    string = g_string_sized_new (len + 1);
    for (i = 0; uris[i]; ++i) {
        g_string_append (string, uris[i]);
        g_string_append_c (string, ' ');
    }

    return g_string_free (string, FALSE);
}
void transform_uris_to_quoted_fuse_paths (gchar **uris) {
    print_programming("transform_uris_to_quoted_fuse_path\n");
    guint i;

    if (!uris)
        return;
    for (i = 0; uris[i]; ++i) {

        GFile *file=NULL;

        gchar *path=NULL;

        file = g_file_new_for_uri (uris[i]);

        if ((path = g_file_get_path (file))) {
            gchar *quoted;

            quoted = g_shell_quote (path);
            g_free_n (uris[i]);
            g_free_n (path);

            uris[i] = quoted;
        }

        g_object_unref (file);
    }
}
void drag_uris_to_songs(gchar uris[STRINGS_LENGTH*3]) {
    print_programming("drag_uris_to_songs\n");
    gint n=0,m=0;
    static gchar so[STRINGS_LENGTH];

    while (uris[n]) {
        if (uris[n]=='\''&&uris[n+1]&&uris[n+1]=='/') {
            m=0;
            n++;
            while (1) {
                so[m++]=uris[n++];
                if (uris[n]=='\''&&uris[n+1]&&uris[n+1]==' '&&uris[n+2]==0)
                    // 结束
                {
                    so[m]=0;
                    if (was_music_file(so)) {
                        GSList *files=(GSList *)malloc(sizeof(GSList));
                        files->data=so;
                        files->next=NULL;
                        add_file(files, focus_lists);
                    }
                    return ;
                } else if (uris[n]=='\''&&uris[n+1]&&uris[n+1]=='\\'&&uris[n+2]&&uris[n+2]=='\'')
                    //中间有'
                {
                    n=n+3;
                } else if (uris[n]=='\''&&uris[n+1]&&uris[n+1]==' '&&uris[n+2]&&uris[n+2]=='\'')
                    //换下一首
                {
                    n=n+1;
                    so[m]=0;
                    if (was_music_file(so)) {
                        GSList *files=(GSList *)malloc(sizeof(GSList));
                        files->data=so;
                        files->next=NULL;
                        add_file(files, focus_lists);
                    }
                    break;
                }
            }
        }
        n++;//没用的,只是以防万一.
    }
    return ;
}
void drag_add_to_songs(gchar uris[STRINGS_LENGTH*3],GtkTreePath *path) {
    print_programming("drag_add_to_songs\n");
    iceplayer_songs *songs=find_lists_from_name(focus_lists)->songs;
    gint indi;
    if (path)
        sscanf(gtk_tree_path_to_string(path),"%d",&indi);
    else
        indi=1;
    iceplayer_songs *songs_t;
    if (indi!=0) {
        while (--indi) {
            songs=songs->next;
            if (songs->next==NULL||songs->uri[0]=='\0')
                break;
        }
        songs_t=songs->next;
        songs->next=NULL;
        drag_uris_to_songs(uris);

        iceplayer_songs *m_songs=find_lists_from_name(focus_lists)->songs;
        while (m_songs->next&&m_songs->uri[0]) {
            m_songs=m_songs->next;
        }

        m_songs->next=songs_t;


    }
    show_songs(NULL);
}
void on_treeview_drag_data_received (GtkWidget *widget, GdkDragContext *context,gint x, gint y, GtkSelectionData *selection_data, guint info, guint timestamp) {
    print_programming("on_treeview_drag_data_received\n");
    if (gtk_targets_include_uri (&selection_data->target, 1)) {
        gchar **uris;
        gsize len;

        uris = gtk_selection_data_get_uris (selection_data);
        if (!uris)
            return;
        transform_uris_to_quoted_fuse_paths(uris);

        g_print("收到拖放文件！\n");

        GtkTreeViewDropPosition pos;

        GtkTreePath *path = NULL;
        gtk_tree_view_get_dest_row_at_pos(GTK_TREE_VIEW(widget), x,y, &path, &pos);
//
        drag_add_to_songs(concat_uris(uris,&len),path);
    }
// else
    //{
    //g_print("拖放失败!不是文件而是文件夹或文字图片等?\n");
    // }
}
gboolean update_songs_order_foreach(GtkTreeModel *model,GtkTreePath *path,GtkTreeIter *iter) {
    static iceplayer_songs *songs;
    gchar *value0;
    gint wie;
    gtk_tree_model_get(model, iter, 0, &value0,3,&wie,-1);
    static gint play_s;
    if (strcmp(gtk_tree_path_to_string(path),"0")==0) {
        songs = find_lists_from_name(focus_lists)->songs;
        strcpy(songs->uri,value0);
        songs->next=NULL;
        play_s=0;
    } else {
        songs->next=(iceplayer_songs *)malloc(sizeof(iceplayer_songs));
        songs->next->next=NULL;
        strcpy(songs->next->uri,value0);
        songs=songs->next;
        play_s++;
    }
    if (wie!=PANGO_WEIGHT_NORMAL)
        sprintf(playing_song,"%d",play_s);
    g_free_n(value0);
    return FALSE;
}
void update_songs_order() {
    gtk_tree_model_foreach(gtk_tree_view_get_model( (GtkTreeView *)iceplayer.list_songs_treeview), (GtkTreeModelForeachFunc) update_songs_order_foreach,NULL);
}
gboolean read_lists() {
    print_programming("read_lists\n");
    GKeyFile *keyfile;
    gsize length;
    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile,get_lists_file_path(),0,NULL)) {
        print_error("无法加载列表文件，请检查列表文件是否完整！\n");
        g_key_file_free(keyfile);
        return FALSE;
    } else {
///
        gchar text[STRINGS_LENGTH];

        gchar **main_lists_name = g_key_file_get_string_list (keyfile,"☞〄列表〄☜","♪",&length,NULL);

        iceplayer_lists *lists=&main_lists;
        while (*main_lists_name) {
            strcpy(lists->name,*main_lists_name);
            FILE *file = fopen(get_songs_file_path(*main_lists_name),"r");
            main_lists_name++;
            if (!file) continue;
            strcpy(text,"");
            fgets(text,STRINGS_LENGTH,file);
            if (text[0]==0||text[0]=='\n') { //首项为空
                lists->songs=NULL;
            } else {
                iceplayer_songs *songs=(iceplayer_songs *)malloc(sizeof(iceplayer_songs));
                songs->next=NULL;
                lists->songs = songs;
                lists->next = NULL;
                lists->focus_song[0]=0;

                text[strlen(text)-1]=0;
                strcpy(songs->uri,text);
                songs->next=NULL;
                strcpy(text,"");
                fgets(text,STRINGS_LENGTH,file);
                while (text[0]&&text[0]!='\n') {
                    songs->next=(iceplayer_songs *)malloc(sizeof(iceplayer_songs));
                    text[strlen(text)-1]=0;
                    strcpy(songs->next->uri,text);
                    songs->next->next=NULL;
                    songs=songs->next;
                    strcpy(text,"");
                    fgets(text,STRINGS_LENGTH,file);
                }
            }
            fclose(file);
            if (*main_lists_name)lists->next=(iceplayer_lists *)malloc(sizeof(iceplayer_lists));
            else {
                lists->next=NULL;
                break;
            }
            lists=lists->next;
        }
///
        g_key_file_free(keyfile);
        g_free_n(*main_lists_name);
    }
    return TRUE;
}

gboolean save_lists() {
    print_programming("save_lists\n");
    GKeyFile *keyfile;
    gchar *file_buf = NULL;
    gsize length;
    keyfile = g_key_file_new();
    if (!g_key_file_load_from_file(keyfile,get_lists_file_path(),0,NULL)) {
        print_error("无法加载列表文件，请检查列表文件是否完整！\n");
        g_key_file_free(keyfile);
        return FALSE;
    } else {
///
        gchar ** main_lists_name = (gchar **) g_malloc0 (sizeof (gchar *) * STRINGS_LENGTH*50);
        iceplayer_lists *lists = &main_lists;
        if (main_lists.name[0]) {
            gint n=0;
            while (lists&&lists->name[0]) {
                *(main_lists_name+n++) = lists->name;
                lists=lists->next;
            }

            g_key_file_set_string_list (keyfile,"☞〄列表〄☜","♪", (const gchar **) main_lists_name, STRINGS_LENGTH*50);
            file_buf = g_key_file_to_data(keyfile, &length, NULL);
            g_file_set_contents(get_lists_file_path(),file_buf, -1, NULL);

///
            lists= &main_lists;
            while (lists&&lists->name[0]) {
                FILE *file = fopen(get_songs_file_path(lists->name),"w");
                iceplayer_songs *songs= lists->songs;
                while (songs&&songs->uri[0]) {
                    fputs(songs->uri,file);
                    fputs("\n",file);
                    songs=songs->next;
                }
                fputs("\n",file);
                fclose(file);
                lists=lists->next;
            }

        } else {
            *main_lists_name=NULL;
            g_key_file_set_string_list (keyfile,"☞〄列表〄☜","♪", (const gchar **) main_lists_name, STRINGS_LENGTH*50);
            file_buf = g_key_file_to_data(keyfile, &length, NULL);
            g_file_set_contents(get_lists_file_path(),file_buf, -1, NULL);

            system("rm -rf ~/.iceplayer/songs/*.songs");
        }


        g_key_file_free(keyfile);
        if(file_buf)g_free_n(file_buf);
        g_free_n(main_lists_name);
    }
    return TRUE;

}

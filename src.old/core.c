#include "core.h"
#include "single.h"
#include "list.h"
#include "ui.h"
#include "configure.h"
#include "lrc.h"
#include "common.h"

iceplayer_core core;

void switch_songs() {
    print_programming("switch_songs\n");
    if (core.pipeline) {
        core.state = GST_STATE_NULL ;
        gst_element_set_state(core.pipeline, core.state);
    }
    switch (configure.play_mode) {
    case PLAY_MODE_NORMAL: {
        next();
        break;
    }
    case PLAY_MODE_UNORMAL: {
        prev();
        break;
    }
    case PLAY_MODE_RADOM: {
        gint index=0;
        srand((unsigned)time(NULL)+1);

        iceplayer_lists *lists;
        iceplayer_songs *songs;
        if (configure.auto_jump_other_lists) {
            lists = find_lists_from_random();
            if (lists==NULL)lists=&main_lists;
            if (strcmp(lists->name,focus_lists)) {
                strcpy(playing_list,lists->name);
                show_lists(lists->name);
                strcpy(focus_lists,lists->name);
                show_songs(NULL);
            }
        }
        lists =  find_lists_from_name(focus_lists);
        if (!lists)
            lists=&main_lists;
        songs =lists->songs;
        index=0;
        while (songs) {
            index++;
            if (songs->uri[0]) {
                songs = songs->next;
            } else {
                break;
            }
        }
        srand((unsigned)time(NULL)+1);
        index=rand()%index;
        sprintf(playing_song,"%d",index);
        update_focus_song_from_playing_song();
        play_uri();
        break;
    }
    case PLAY_MODE_ONE: {
        play_uri();
        break;
    }
    }
}
gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer user_data) {
    print_programming("bus_call\n");
    switch (GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_ERROR:
        list_menu_jump_to_playing_song(NULL);
        if (configure.failed_stop) {
            stop();
            break ;
        }
    case GST_MESSAGE_EOS: {
        switch_songs();
        break;
    }
    default:
        break;
    }
    return TRUE;
}
void play_uri() {

    print_programming("play_uri\n");
    if (core.pipeline) gst_element_set_state(GST_ELEMENT(core.pipeline), GST_STATE_NULL);
    gchar *uri = (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    gchar *path= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    strcpy(uri,"file://");
    if (strncmp(focus_song,"<cue>",5)) {
        FILE *file = fopen(focus_song,"r");
        if (file) {
            fclose(file);
        } else {
            gchar *te= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
            sprintf(te,_("iceplayer \n文件\"%s\"无法读取，请重试或检查文件所在目录是否可读？相关分区是否已经加载？"),focus_song);
            print_error("%s\n",te);
            GtkWidget *dialog_real = gtk_message_dialog_new_with_markup(NULL,GTK_DIALOG_DESTROY_WITH_PARENT    ,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,te);
            gtk_dialog_run((GtkDialog *)dialog_real);
            gtk_widget_destroy(dialog_real);
            g_free_n(te);
            return ;
        }
        strcpy(path,focus_song);
        print_debug(path);

        gtk_label_set_text(GTK_LABEL(iceplayer.name_label), g_basename(path));
        print_notify(_("播放"), (gchar *)g_basename(path));
    } else {
        ///  "<cue><FILE:%s><TRACK:%s><TITLE:%s><PERFORMER:%s><INDEX_A:%s><INDEX_B:%s><cue_uri:%s>",
        gchar *cue_uri= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        gchar *title= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        gchar *performer= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        gchar *file= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));

        strcpy(cue_uri,"");
        strcpy(cue_uri,strstr(focus_song,"cue_uri:"));
        sscanf(cue_uri,"cue_uri:%[^>]%*s",cue_uri);

        strcpy(file,"");
        strcpy(file,strstr(focus_song,"<FILE:"));
        sscanf(file,"<FILE:%[^>]%*s",file);

        sprintf(path,"%s/%s",g_path_get_dirname(cue_uri),file);

        strcpy(title,"");
        strcpy(title,strstr(focus_song,"<TITLE:"));
        sscanf(title,"<TITLE:%[^>]",title);

        strcpy(performer,"");
        strcpy(performer,strstr(focus_song,"<PERFORMER:"));
        sscanf(performer,"<PERFORMER:%[^>]",performer);
        gtk_label_set_text(GTK_LABEL(iceplayer.name_label),strcat(strcat(title,"-"),performer));
        FILE *mus_file = fopen(path,"r");
        if (mus_file) {
            fclose(mus_file);
        } else {
            gchar * path2= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
            strcpy(path2,path);
            strcpy(path,cue_uri);
            gint n=strlen(path);
            while (path[n]!='.'||n==0)n--;
            path[n]=0;
            gint m=strlen(file);
            while (file[m]!='.'||m==0)m--;
            while (file[m]) {
                path[n++]=file[m++];
            }
            path[n]=0;
            mus_file = fopen(path,"r");
            if (mus_file) {
                fclose(mus_file);
                g_free_n(path2);
            } else {
                gchar *te= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
                if (strcmp(path,path2)) {
                    sprintf(te,"iceplayer \ncue的音乐文件 \"%s\"和\"%s\"都无法读取，请重试或检查文件所在目录是否可读？相关分区是否已经加载？cue内文件名和于cue同名的音乐文件是否存在？",path,path2);
                } else {
                    sprintf(te,"iceplayer \ncue的音乐文件 \"%s\"无法读取，请重试或检查文件所在目录是否可读？相关分区是否已经加载？cue内文件名和于cue同名的音乐文件是否存在？",path);
                }
                g_free_n(path2);
                print_error("%s\n",te);
                GtkWidget *dialog_real = gtk_message_dialog_new_with_markup(NULL,GTK_DIALOG_DESTROY_WITH_PARENT    ,GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK,te);
                gtk_dialog_run((GtkDialog *)dialog_real);
                gtk_widget_destroy(dialog_real);
                g_free_n(te);

                g_free_n(cue_uri);
                g_free_n(title);
                g_free_n(performer);
                g_free_n(file);

                return ;
            }
            print_debug(focus_song);
        }
        g_free_n(cue_uri);
        g_free_n(title);
        g_free_n(performer);
        g_free_n(file);
    }
    strcat(uri,path);

    print_debug("歌词");
    lrc();


    if (configure.auto_jump_to_play) {
        print_debug("跳到正在播放歌曲");
        list_menu_jump_to_playing_song(NULL);
    }

    GstBus *bus;
    GstElement *seff = NULL,*audio_sink = NULL,*video_fakesink = NULL,*audio_equalizer = NULL, *audio_convert = NULL,*spectrum_plugin = NULL;
    GstPad *pad;
    GstCaps *caps;
    core.pipeline=gst_element_factory_make("playbin","play");
    audio_sink = gst_element_factory_make("autoaudiosink", "audio_sink");
    video_fakesink = gst_element_factory_make("fakesink", "video_sink");
    audio_equalizer = gst_element_factory_make("equalizer-10bands","audio_equalizer");
    audio_convert = gst_element_factory_make("audioconvert", "eqauconv");
    core.vol = gst_element_factory_make("volume", "volume_plugin");
    spectrum_plugin = gst_element_factory_make("spectrum", "spectrum_plugin");

    g_object_set(G_OBJECT(video_fakesink), "sync", TRUE, NULL);
    g_object_set(G_OBJECT(spectrum_plugin), "bands", 30, "threshold",-80, "message", TRUE, "message-magnitude", TRUE, NULL);
    seff = gst_bin_new("audio-bin");

    gst_bin_add_many(GST_BIN(seff), audio_equalizer, audio_convert,spectrum_plugin, core.vol, audio_sink, NULL);
    caps = gst_caps_new_simple("audio/x-raw-int", "rate", G_TYPE_INT,44100, NULL);

    if (!gst_element_link_many(audio_equalizer, audio_convert, NULL) ||!gst_element_link_filtered(audio_convert, spectrum_plugin, caps) || !gst_element_link_many(spectrum_plugin, core.vol, audio_sink, NULL)) {
        print_error("连接gstreamer元素失败！\n");
    }
    gst_caps_unref(caps);
    pad = gst_element_get_static_pad(audio_equalizer, "sink");
    gst_element_add_pad(seff, gst_ghost_pad_new(NULL, pad));
    g_object_set(G_OBJECT(core.pipeline), "audio-sink", seff, NULL);
    bus=gst_pipeline_get_bus(GST_PIPELINE(core.pipeline));

    gst_bus_add_watch(bus, (GstBusFunc)bus_call, NULL);
    gst_object_unref(bus);
    gst_element_set_state(core.pipeline, GST_STATE_NULL);
    gst_element_set_state(seff, GST_STATE_READY);
    core.equalizer = audio_equalizer;

    g_object_set(G_OBJECT(core.pipeline), "uri",uri, NULL);
    g_object_set(G_OBJECT(core.pipeline), "video-sink",  video_fakesink, NULL);
    g_object_set(core.vol, "volume",0.01, NULL);
    bus = gst_pipeline_get_bus(GST_PIPELINE(core.pipeline));
    gst_object_unref(bus);
    core.state = GST_STATE_PLAYING;
    gst_element_set_state(core.pipeline, GST_STATE_PLAYING);
    core_set_eq_effect(configure.eq);

    inline gboolean change_cue_pos() {
        gchar *p= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
        strcpy(p,strstr(focus_song,"<INDEX_A"));
        gint min_pre,sec_pre,mir_pre;
        sscanf(p,"<INDEX_A:%d:%d:%d>%*s",&min_pre,&sec_pre,&mir_pre);
        gint64 nanosec =(min_pre*60+sec_pre+mir_pre/100)* GST_SECOND;
        g_object_set(core.vol, "volume",0.01, NULL);
        gst_element_seek(core.pipeline,1.0,GST_FORMAT_TIME,GST_SEEK_FLAG_FLUSH,GST_SEEK_TYPE_SET,nanosec,GST_SEEK_TYPE_NONE,GST_CLOCK_TIME_NONE);
        gboolean ba_vol() {
            g_object_set(core.vol, "volume",configure.volume, NULL);
            return FALSE;
        }
        g_timeout_add(200,ba_vol,NULL);
        g_free_n(p);
        return FALSE;
    }
    if (!strncmp(focus_song,"<cue>",5)) {
        print_debug("调整cue至歌曲准确位置");
        g_timeout_add(10,change_cue_pos,NULL);
    }
    gboolean sovl() {
        set_volume(configure.volume);
        configure_wirte();
        save_lists();
        return FALSE;
    }
    g_timeout_add(10,sovl,NULL);

    g_free_n(uri);
    g_free_n(path);

}
gboolean prev () {
    print_programming("prev\n");
    if (lrc_search_data.search_lrc_state == SEARCH_LRC_STATE_NET_SOCKET ||lrc_search_data.search_lrc_state == SEARCH_LRC_STATE_NET_DNS||lrc_search_data.search_lrc_state == SEARCH_LRC_STATE_NET_CONNECT||lrc_search_data.search_lrc_state ==SEARCH_LRC_STATE_SEARCHED)
        return FALSE;
    if (!main_lists.name[0]) {
        add_menu();
        return FALSE;
    }
    if (!find_lists_from_name(focus_lists)->songs) {
        add_menu();
        return FALSE;
    }
    if (!find_lists_from_name(focus_lists)->songs->uri[0]) {
        add_menu();
        return FALSE;
    }
    if (configure.play_mode==PLAY_MODE_RADOM)
        switch_songs();
    else if (playing_song[0]) {
        GtkTreePath *path;
        path = gtk_tree_path_new_from_string(playing_song);
        gtk_tree_path_prev(path);
        strcpy(playing_song,gtk_tree_path_to_string(path));
        update_focus_song_from_playing_song();
        play_uri();
    }
    return TRUE;
}
gboolean play () {
    print_programming("play\n");
    if (!main_lists.name[0]) {
        add_menu();
        return FALSE;
    }
    if (!find_lists_from_name(focus_lists)->songs) {
        add_menu();
        return FALSE;
    }
    if (!find_lists_from_name(focus_lists)->songs->uri[0]) {
        add_menu();
        return FALSE;
    }
    if (core.state == GST_STATE_PLAYING) {
        if (core.pipeline) {
            core.state = GST_STATE_PAUSED;
            gst_element_set_state(GST_ELEMENT(core.pipeline), core.state);
            change_picture_on_press_and_release_event(NULL,CONTROL_PLAY);
            change_picture_on_press_and_release_event(NULL,MINI_PLAY);
        } else {
            core.state = GST_STATE_NULL;
        }
    } else {
        if (core.pipeline) {
            core.state = GST_STATE_PLAYING;
            gst_element_set_state(GST_ELEMENT(core.pipeline), core.state);
            change_picture_on_press_and_release_event(NULL,CONTROL_PAUSE);
            change_picture_on_press_and_release_event(NULL,MINI_PAUSE);
        } else {
            if (!focus_song[0]) {
                update_focus_song_from_selection();
                show_songs(NULL);
            }
            if (!focus_lists[0])
                strcpy(focus_lists,main_lists.name);
            if (!focus_song[0]) {
                strcpy(focus_song,((iceplayer_songs *)(find_lists_from_name(focus_lists)->songs))->uri);
                strcpy(playing_song,focus_song);
                show_songs(NULL);
            }
            if (!focus_song[0]) {
                print_error("没有可供播放的项!\n");
                return FALSE;
            }
            play_uri();
        }
    }
    return TRUE;
}
gboolean next () {
    print_programming("next\n");
    if (lrc_search_data.search_lrc_state == SEARCH_LRC_STATE_NET_SOCKET ||lrc_search_data.search_lrc_state == SEARCH_LRC_STATE_NET_DNS||lrc_search_data.search_lrc_state == SEARCH_LRC_STATE_NET_CONNECT||lrc_search_data.search_lrc_state ==SEARCH_LRC_STATE_SEARCHED)
        return FALSE;
    if (!main_lists.name[0]) {
        add_menu();
        return FALSE;
    }
    if (!find_lists_from_name(focus_lists)->songs) {
        add_menu();
        return FALSE;
    }
    if (!find_lists_from_name(focus_lists)->songs->uri[0]) {
        add_menu();
        return FALSE;
    }

    if (configure.play_mode==PLAY_MODE_RADOM)
        switch_songs();
    else if (playing_song[0]) {
        GtkTreePath *path=NULL;
        path = gtk_tree_path_new_from_string(playing_song);
        gtk_tree_path_next(path);
        //lists是正在播放的列表
        iceplayer_lists *lists = find_lists_from_name(focus_lists);
        if (lists==NULL) {
            lists=&main_lists;
        }
//index是歌曲总数
        gint index = 0;
        iceplayer_songs *songs=lists->songs;

        while (songs&&songs->next&&songs->uri[0]) {
            songs=songs->next;
            index++;
        }
        if (index==0&&!songs) {
            print_error("没有歌曲?\n");
            if(path)gtk_tree_path_free(path);
            return FALSE;
        }

        gchar *value=NULL;
        value = gtk_tree_path_to_string(path);
        //如果总数大于或者等于正在播放的位置，就直接下一首，否则重头来

        if (index>=atoi(value))
            strcpy(playing_song,value);
        else {
            if (configure.auto_jump_other_lists) {
                iceplayer_lists *lists = find_lists_from_name(focus_lists);
                if (lists->next) {
                    if (lists->next->name[0])
                        lists=lists->next;
                    strcpy(focus_lists,lists->name);
                    show_songs(NULL);
                    strcpy(playing_list,lists->name);
                    playing_song[0]='0';
                    playing_song[1]=0;
                    list_menu_jump_to_playing_song(NULL);
                } else {
                    playing_song[0]='0';
                    playing_song[1]=0;
                }
            } else {
                playing_song[0]='0';
                playing_song[1]=0;
            }
        }
        g_free_n(value);
        if(path)gtk_tree_path_free(path);
        update_focus_song_from_playing_song();
        play_uri();
    } else {
        print_error("没有在播放歌曲！\n");
        stop();
    }
    return TRUE;
}
gboolean stop () {
    print_programming("stop\n");
    if (core.pipeline) {
        core.state = GST_STATE_NULL;
        gst_element_set_state(GST_ELEMENT(core.pipeline), core.state);
        core.pipeline = NULL;
        change_picture_on_press_and_release_event(NULL,CONTROL_PAUSE);
        change_picture_on_press_and_release_event(NULL,MINI_PAUSE);
        playing_list[0]=0;
        playing_song[0]=0;
        focus_song[0]=0;
        lrc_text[0][0]=0;
    }
    return TRUE;
}
gboolean add () {
    print_programming("add\n");
    add_menu();
    return TRUE;
}
gboolean set_volume(gdouble value) {
    print_programming("set_volume\n");
    if (!core.pipeline) return FALSE;
    if (!core.vol) return FALSE;
    g_object_set(core.vol, "volume",value, NULL);
    configure.volume = value;
    return TRUE;
}

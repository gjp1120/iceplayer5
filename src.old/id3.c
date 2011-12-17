#include <gtk/gtk.h>
#include "id3.h"
#include "list.h"
#include "ui.h"

gint id3_indices;
iceplayer_id3 id3;

/*
void id3_set_sensitive(gboolean sensitive)
{
  print_programming("id3_set_sensitive\n");
  if (id3.id3_editor)
    {
      gtk_widget_set_sensitive((GtkWidget *)id3.entry_artist,sensitive);
      gtk_widget_set_sensitive((GtkWidget *)id3.entry_album,sensitive);
      gtk_widget_set_sensitive((GtkWidget *)id3.entry_title,sensitive);
      gtk_widget_set_sensitive((GtkWidget *)id3.entry_comment,sensitive);
      gtk_widget_set_sensitive((GtkWidget *)id3.entry_track,sensitive);
      gtk_widget_set_sensitive((GtkWidget *)id3.entry_year,sensitive);
    }
}
void id3_set_entry_text(ID3_tag *tag,gchar path[STRINGS_LENGTH])
{
  print_programming("id3_set_entry_text\n");
  gtk_entry_set_text((GtkEntry *)id3.entry_artist,tag->editor);
  gtk_entry_set_text((GtkEntry *)id3.entry_album,tag->album);
  gtk_entry_set_text((GtkEntry *)id3.entry_title,tag->name);

  gtk_entry_set_text((GtkEntry *)id3.entry_comment,tag->comment);
  gtk_entry_set_text((GtkEntry *)id3.entry_track,tag->track);
  gtk_entry_set_text((GtkEntry *)id3.entry_year,tag->year);

  gchar folder[STRINGS_LENGTH*3];
  gchar name[STRINGS_LENGTH];
  gchar suffix[STRINGS_LENGTH];
  strcpy(folder,path);
  gint a=strlen(path);
  while (folder[a]!='/'&&a!=0)
    a--;
  if (a!=0)
    {
      folder[a]=0;
      gint b=0;
      a++;
      while( (path[a]!='.'&&path[a]!=0)||(path[a+4]!=0&&path[a+5]!=0))
        name[b++]=path[a++];
      name[b]=0;
      if (path[a])
        {
          gint b=0;
          while (path[a]!=0)
            suffix[b++]=path[a++];
          suffix[b]=0;
          gtk_entry_set_text((GtkEntry *)id3.entry_suffix,suffix);
        }

      gtk_entry_set_text((GtkEntry *)id3.entry_folder,folder);
      gtk_entry_set_text((GtkEntry *)id3.entry_name,name);
    }
}
ID3_tag *get_tag(gchar path[STRINGS_LENGTH])
{
  ID3_tag *tag = (ID3_tag*)malloc(sizeof(ID3_tag));
  strcpy(tag->name,"");
  strcpy(tag->editor,"");
  strcpy(tag->album,"");
  strcpy(tag->year,"");
  strcpy(tag->comment,"");
  strcpy(tag->track,"");
  strcpy(tag->genre,"");

    gchar uri[STRINGS_LENGTH];
    strcpy(uri,"file://");


  return tag;
}


*/

gchar **tag_get_id3v1(FILE *file) {
    gchar *b;
    gchar *temp;
    static gchar a[130], *tag[3];
    gsize r,w;
    gint i,n;
    tag[0] = NULL;
    tag[1] = NULL;
    tag[2] = NULL;
    if(fseek(file, -128, 2)==-1) {
        return NULL;
    }
    if(!fread(a, 128, 1, file)) {
        return NULL;
    }
    b=a;
    if(*b=='T' && *(b+1)=='A' && *(b+2)=='G') {
        b+=3;
        for(n=0; n<3; n++) {
            if(n==0)
                tag[2] = g_strndup(b, 30);
            else if(n==1)
                tag[0] = g_strndup(b, 30);
            else if(n==2)
                tag[1] = g_strndup(b, 30);
            b+=30;
            for(i=1; !(*(b-i)>=33 && *(b-i)<=122); i++)
                if(*(b-i)==0)
                    break;
                else if(*(b-i)==32)
                    *(b-i)=0;
        }
        for(i=0; i<3; i++) {
            if(!g_utf8_validate(tag[i],-1,NULL)) {
                temp = g_convert(tag[i], -1, "UTF-8", "GBK", &r, &w, NULL);
                if(tag[i]!=NULL) g_free_n(tag[i]);
                tag[i] = temp;
            } else {
                g_free_n(tag[i]);
                tag[i] = NULL;
            }
        }
        return tag;
    }
    return NULL;
}

//*****************************************************************************************************
//*
//*	FUNCTION:	tag_get_id3v2
//*	DISCRIPTION:	get ID3v2 tag, serve for fuction get_tag
//*
//*****************************************************************************************************
static char **tag_get_id3v2(FILE *file) {
    gchar *a, *c, b[5], *d;
    static gchar *tag[3];
    gint j, header_size, version;
    glong i;
    gint size[4], tag_type;
    gsize r, w;
    tag[0] = NULL;
    tag[1] = NULL;
    tag[2] = NULL;
    a = g_malloc(10);
    int output;
    output=fread(a, 10, 1, file);
    /* Judge if it is an ID3v2 tag. */
    if(*a=='I' && *(a+1)=='D' && *(a+2)=='3') {
        version = *(a+3);
        size[0] = *(a+6);
        if(size[0]<0) size[0]+=256;
        size[1] = *(a+7);
        if(size[1]<0) size[1]+=256;
        size[2] = *(a+8);
        if(size[2]<0) size[2]+=256;
        size[3] = *(a+9);
        if(size[3]<0) size[3]+=256;
        i = (size[0]&0x7F)*0x200000 + (size[1]&0x7F)*0x4000 +  (size[2]&0x7F)*0x80 + (size[3]&0x7F);
        header_size = i;
        g_free_n(a);
        a = g_malloc(i);
        if(!fread(a,i,1,file)) {
            g_free_n(a);
            return NULL;
        };
        c = a;

        /* If it is an ID3v2.4 tag. */
        if(version==4) {
            g_free_n(a);
            return NULL;
        }
        /* If it is an ID3v2.3 tag. */
        else if(version==3) {
            /* Read each tag in the loop. */
            for(; c!=a+header_size;) {
                if(tag[0]!=NULL && tag[1]!=NULL && tag[2]!=NULL) {
                    g_free_n(a);
                    return tag;
                }
                c[4]='\0';
                strcpy(b,c);
                b[4] = 0;
                tag_type = 0;
                size[0] = *(c+4);
                if(size[0]<0) size[0]+=256;
                size[1] = *(c+5);
                if(size[1]<0) size[1]+=256;
                size[2]=*(c+6);
                if(size[2]<0) size[2]+=256;
                size[3]=*(c+7);
                if(size[3]<0) size[3]+=256;
                i = size[0]*0x1000000+size[1]*0x10000+size[2]*0x100+size[3];
                if(i<0) i+=256;
                if((c+i)>(a+header_size)) break;
                if(i==0) break;
                tag_type = 0;
                if(!strcmp(b, "TPE1")) tag_type = 1;
                if(!strcmp(b, "TALB")) tag_type = 2;
                if(!strcmp(b, "TIT2")) tag_type = 3;
                if(!tag_type) {
                    c+=i+10;
                    continue;
                }
                c+=10;
                d = g_malloc(i+1);
                if(d==NULL) {
                    g_free_n(a);
                    return NULL;
                }
                for(j=0; i!=0; i--) {
                    if(*c!=0) {
                        d[j] = *c;
                        j++;
                    }
                    c++;
                }
                d[j]=0;
                if(g_utf8_validate(d,-1,NULL))
                    tag[tag_type-1] = NULL;
                else {
                    tag[tag_type-1]=g_convert(d, -1, "UTF-8", "GBK", &r, &w, NULL);
                }
                g_free_n(d);
            }
            if(tag[0]==NULL && tag[1]==NULL && tag[2]==NULL) {
                g_free_n(a);
                return NULL;
            }
        }
        /* If it is an ID3v2.2 tag. */
        else if(version==2) {
            /* Read each tag in the loop. */
            for(; c<a+header_size;) {
                if(tag[0]!=NULL && tag[1]!=NULL && tag[2]!=NULL) {
                    g_free_n(a);
                    return tag;
                }
                c[3]='\0';
                strcpy(b,c);
                b[3] = 0;
                size[0] = *(c+3);
                if(size[0]<0) size[0]+=256;
                size[1] = *(c+4);
                if(size[1]<0) size[1]+=256;
                size[2] = *(c+5);
                if(size[2]<0) size[2]+=256;
                i = size[0]*0x10000 + size[1]*0x100 + size[2];
                if(i<0) i+=256;
                if((c+i)>(a+header_size)) break;
                if(i==0) break;
                tag_type=0;
                if(!strcmp(b,"TP1")) tag_type = 1;
                if(!strcmp(b,"TAL")) tag_type = 2;
                if(!strcmp(b,"TT2")) tag_type = 3;
                if(!tag_type) {
                    c+=i+6;
                    continue;
                }
                c+=6;
                d = g_malloc(i);
                if(d==NULL) {
                    g_free_n(a);
                    return NULL;
                }
                for(j=0; i!=0; i--) {
                    if(*c==0) {
                        c++;
                        continue;
                    }
                    d[j] = *c;
                    j++;
                    c++;
                }
                d[j] = 0;
                if(g_utf8_validate(d,-1,NULL)) {
                    tag[tag_type-1] = NULL;
                } else {
                    tag[tag_type-1] = g_convert(d, -1, "UTF-8",  "GBK", &r, &w, NULL);
                }
                g_free_n(d);
            }
            if(tag[0]==NULL && tag[1]==NULL && tag[2]==NULL) {
                g_free_n(a);
                return NULL;
            }
        }
    }
    g_free_n(a);
    return NULL;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		md_get_mp3_tag
//*	DISCRIPTION:	get ID3 tag
//*
//*****************************************************************************************************
gchar **md_get_mp3_tag(gchar *filename) {
    FILE *file;
    static gchar **tag;

    if(filename==NULL || *filename==0) {
        //g_printf("Get tag: Invaild filename.\n");
        return NULL;
    }
    file=fopen(filename, "rb");
    if(file==NULL) {
        //g_printf("Get tag: Can't open file %s\n", filename);
        return NULL;
    }
    /* Get ID3v2 tag, if it returns NULL, then try to get ID3v1 tag. */
    tag=tag_get_id3v2(file);
    if(tag==NULL) {
        rewind(file);
        tag=tag_get_id3v1(file);
    }
    fclose(file);
    return tag;
}

GstElement *pipeline;
gboolean eos=FALSE;
gchar gui_treeview_info_title[STRINGS_LENGTH];
gchar gui_treeview_info_artist[STRINGS_LENGTH];
gchar gui_treeview_info_album[STRINGS_LENGTH];
gchar gui_treeview_info_genre[STRINGS_LENGTH];
gint gui_treeview_info_track=0;
gchar gui_treeview_info_codec[STRINGS_LENGTH];
gchar gui_treeview_info_start[STRINGS_LENGTH];
gchar gui_treeview_info_duration[STRINGS_LENGTH];
gchar gui_treeview_info_bitrate[STRINGS_LENGTH];
gchar gui_treeview_info_uri[STRINGS_LENGTH];
GdkPixbuf *gui_treeview_info_pixbuf=NULL;
gchar location[STRINGS_LENGTH];
///  "<cue><FILE:%s><TRACK:%s><TITLE:%s><PERFORMER:%s><INDEX_A:%s><INDEX_B:%s><cue_uri:%s>",
gchar *cue_get_song(gchar uri[STRINGS_LENGTH]) {
    static gchar title[STRINGS_LENGTH];
    gint n;
    strcpy(title,strstr(uri,"TITLE:"));
    sscanf(title,"TITLE:%[^>]",title);
    n=0;
    while (title[n]!='>')n++;
    title[n]=0;
    return title;
}
gchar *cue_get_artist(gchar uri[STRINGS_LENGTH]) {
    static gchar performer[STRINGS_LENGTH];
    strcpy(performer,strstr(uri,"PERFORMER:"));
    sscanf(performer,"PERFORMER:%[^>]",performer);
    gint n=0;
    while (performer[n]!='>')n++;
    performer[n]=0;
    return performer;
}
gchar *cue_get_album(gchar uri[STRINGS_LENGTH]) {
    static gchar cue_uri[STRINGS_LENGTH];
    strcpy(cue_uri,strstr(uri,"cue_uri:"));
    sscanf(cue_uri,"cue_uri:%[^>]",cue_uri);
    gint n=0;
    while (cue_uri[n]!='>')n++;
    cue_uri[n]=0;

    FILE *file = fopen(cue_uri,"r");
    if(file) {
        static gchar album[STRINGS_LENGTH];
        while(fgets(album,STRINGS_LENGTH, file)) {
            if(strncmp(album,"TITLE",5)==0) {
                strcpy(album,strstr(album,"TITLE \""));
                sscanf(album,"TITLE \"%[^\"]",album);
                fclose(file);
                return album;
            }
        }
        fclose(file);
        return "木有专辑信息";
    } else {
        return "打开cue失败";
    }
}
gchar *cue_get_track(gchar uri[STRINGS_LENGTH]) {
    static gchar track[STRINGS_LENGTH];
    strcpy(track,strstr(uri,"TRACK:"));
    sscanf(track,"TRACK: %s",track);
    gint n=0;
    while (track[n]!='>')n++;
    track[n]=0;
    return track;
}
static void gui_treeview_callback_callback_build_pipeline(GstElement *decodebin,GstPad *pad, gboolean last, gpointer data) {
    static GstCaps *caps;
    static GstPad *sink_pad;

    caps = gst_pad_get_caps(pad);
    if(gst_caps_is_empty(caps) || gst_caps_is_any(caps)) {

    } else {
        sink_pad = gst_element_get_static_pad(GST_ELEMENT(data), "sink");
        gst_pad_link(pad, sink_pad);
        gst_object_unref(sink_pad);
    }
    gst_caps_unref (caps);
}

static void gui_treeview_callback_add_id3_pad(GstElement *demux, GstPad *pad,GstElement *tagger) {
    GstCaps *caps;
    GstPad *conn_pad = NULL;
    caps = gst_pad_get_caps (pad);
    conn_pad = gst_element_get_compatible_pad(tagger, pad, NULL);
    gst_pad_link(pad, conn_pad);
    gst_object_unref(conn_pad);
}
static void gui_treeview_callback_add_ogg_pad(GstElement *demux, GstPad *pad,GstElement *tagger) {
    GstCaps *caps;
    GstPad *conn_pad = NULL;
    caps = gst_pad_get_caps (pad);
    conn_pad = gst_element_get_compatible_pad(tagger, pad, NULL);
    gst_pad_link(pad, conn_pad);
    gst_object_unref(conn_pad);
}
static void gui_treeview_callback_move_file() {
    if(pipeline!=NULL) {
        gst_element_set_state(pipeline, GST_STATE_NULL);
        gst_object_unref(pipeline);
        pipeline = NULL;
        //gint output;
        unlink(location);
        gchar *cmd;
        cmd=g_strconcat("mv /tmp/iceplayer_id3.audio \"",location,"\"",NULL);
        system(cmd);
    }
}
static int tag_setter_handler(GstBus *bus, GstMessage *msg, gpointer data) {
    switch(GST_MESSAGE_TYPE(msg)) {
    case GST_MESSAGE_EOS:
        gui_treeview_callback_move_file();
        break;
    default:
        break;
    }
    return GST_BUS_ASYNC;
}

static gboolean gui_treeview_callback_callback_get_metadata(GstBus *bus, GstMessage *message, gpointer data) {
    gchar *tag_genre=NULL;
    guint tag_track=0;
    //gchar *tag_comment=NULL;
    guint tag_bitrate=0;

    switch(GST_MESSAGE_TYPE(message)) {
    case GST_MESSAGE_EOS: {
        //g_print("gui_treeview_callback_callback_get_metadata1\n");
        eos = TRUE;
        return TRUE;
    }
    case GST_MESSAGE_ERROR: {
        //g_print("gui_treeview_callback_callback_get_metadata2\n");
        eos = TRUE;
        return TRUE;
    }
    case GST_MESSAGE_TAG: {
        //g_print("gui_treeview_callback_callback_get_metadata3\n");
        GstTagList *tags;
        gst_message_parse_tag(message,&tags);
        //获取比特率
        if(gst_tag_list_get_uint(tags,GST_TAG_BITRATE,&tag_bitrate)) {
            //if(gui_treeview_info_bitrate!=NULL)
            //	//g_free_n(gui_treeview_info_bitrate);
            g_snprintf(gui_treeview_info_bitrate,256,"%02d kbps",tag_bitrate/1000);

        }
        //获取风格
        if(gst_tag_list_get_string(tags,GST_TAG_GENRE,&tag_genre)) {
            if(g_utf8_validate(tag_genre,-1,NULL)) {
                //	if(gui_treeview_info_genre!=NULL)
                //	//g_free_n(gui_treeview_info_genre);
                strcpy(gui_treeview_info_genre,g_utf8_strncpy(gui_treeview_info_genre,tag_genre,255));
                gui_treeview_info_genre[255]='\0';
            }
            //g_free_n(tag_genre);
        }
        //获取曲目号
        if(gst_tag_list_get_uint(tags,GST_TAG_TRACK_NUMBER,&tag_track)) {
            gui_treeview_info_track = (gint)tag_track;
        }
        gst_tag_list_free(tags);
        return TRUE;
        break;
    }
    default:
        break;
    }
    return FALSE;
}

static void gui_treeview_callback_callback_event_loop(GstElement *element, gboolean block) {
    GstBus *bus;
    GstMessage *message;
    gboolean done = FALSE;
    bus = gst_element_get_bus(element);
    g_return_if_fail(bus!=NULL);
    while(!done && !eos) {
        if(block)
            message = gst_bus_timed_pop(bus, GST_CLOCK_TIME_NONE);
        else
            message = gst_bus_timed_pop(bus, 0);
        if(message==NULL) {
            gst_object_unref(bus);
            return;
        }
        done = gui_treeview_callback_callback_get_metadata(bus,message,NULL);
        gst_message_unref(message);
    }
    gst_object_unref(bus);
}

//*****************************************************************************************************
//*
//*	FUNCTION:		md_callback_handler
//*	DISCRIPTION:	the bus handler for getting metadata
//*
//*****************************************************************************************************
static gboolean md_callback_get_metadata(GstBus *bus, GstMessage *message, gpointer data)

{
    //g_print("md_callback_get_metadata\n");
    ID3_tag *md=data;

    gchar *tag_codec=NULL;
    gchar *tag_title=NULL;
    gchar *tag_artist=NULL;
    gchar *tag_album=NULL;

    if(md==NULL) return TRUE;
    if(md->uri==NULL) return TRUE;

    switch(GST_MESSAGE_TYPE(message)) {
    case GST_MESSAGE_TAG: {
        //g_print("md_callback_get_metadata1\n");
        GstTagList *tags;
        gst_message_parse_tag(message,&tags);
        //获取文件类型
        if(gst_tag_list_get_string(tags,GST_TAG_AUDIO_CODEC,&tag_codec)) {
            //g_print("md_callback_get_metadata2\n");
            if(tag_codec!=NULL)
                strcpy(md->codec,tag_codec);
        }
        //获取歌曲名
        if(gst_tag_list_get_string(tags,GST_TAG_TITLE,&tag_title)) {
            //g_print("md_callback_get_metadata3\n");
            if(g_utf8_validate(tag_title,-1,NULL)) {
                //if(md->song!=NULL)
                ////g_free_n((gchar *)md->song);
                strcpy(md->song , g_malloc(128));
                strcpy(md->song , g_utf8_strncpy((gchar *)md->song,tag_title,128));
            }
            //g_free_n(tag_title);
        }
        //获取艺术家
        if(gst_tag_list_get_string(tags,GST_TAG_ARTIST,&tag_artist)) {
            //g_print("md_callback_get_metadata4\n");
            if(g_utf8_validate(tag_artist,-1,NULL)) {
                //if(md->artist!=NULL) //g_free_n((gchar *)md->artist);
                strcpy(md->artist , g_malloc(128));
                strcpy(md->artist , g_utf8_strncpy((gchar *)md->artist,tag_artist,128));
            }
            //g_free_n(tag_artist);
        }
        //获取专辑名
        if(gst_tag_list_get_string(tags,GST_TAG_ALBUM,&tag_album)) {
            //g_print("md_callback_get_metadata5\n");
            if(g_utf8_validate(tag_album,-1,NULL)) {
                //if(md->album!=NULL) //g_free_n((gchar *)md->album);
                strcpy(md->album , g_malloc(128));
                strcpy(md->album , g_utf8_strncpy((gchar *)md->album,tag_album,128));
            }
            //g_free_n(tag_album);
        }
        //g_print("md_callback_get_metadata6\n");
        gst_tag_list_free(tags);
        return TRUE;
        break;
    }
    case GST_MESSAGE_ELEMENT:
        break;
    default:
        break;
    }
    return FALSE;
}

//*****************************************************************************************************
//*
//*	FUNCTION:		md_get_meta
//*	DISCRIPTION:	get meta of paticular music by uri
//*
//*****************************************************************************************************
static void md_callback_event_loop(ID3_tag *md, GstElement *element, gboolean block) {
    //g_print("md_callback_event_loop\n");
    GstBus *bus;
    GstMessage *message;
    gboolean done = FALSE;
    bus = gst_element_get_bus(element);
    g_return_if_fail(bus!=NULL);

    message = gst_bus_timed_pop(bus, GST_CLOCK_TIME_NONE);

    if(message==NULL) {
        gst_object_unref(bus);
        return;
    }
    done = md_callback_get_metadata(bus,message,md);
    gst_message_unref(message);
    gst_object_unref(bus);
}
static void md_callback_build_pipeline(GstElement *decodebin,GstPad *pad, gboolean last, gpointer data) {
    static GstCaps *caps;
    static GstPad *sink_pad;

    caps = gst_pad_get_caps(pad);
    if(!gst_caps_is_empty(caps) || gst_caps_is_any(caps)) {
        sink_pad = gst_element_get_static_pad(GST_ELEMENT(data), "sink");
        gst_pad_link(pad, sink_pad);
        gst_object_unref(sink_pad);
        //gst_element_set_state(pipeline,GST_STATE_PAUSED);	//加上这一句程序会假死，原因不详
    }
    gst_caps_unref (caps);
}
ID3_tag*	md_get_metadata(gchar *uri) {

    pipeline=NULL;

    ID3_tag *md =(ID3_tag *)g_malloc(sizeof(ID3_tag));
//g_print("0\n");
    strcpy(md->song,"unknown");
    strcpy(md->artist,"unknown");
    strcpy(md->album,"unknown");
    strcpy(md->codec,"unknown");
    strcpy(md->uri,uri);
//g_print("1\n");
    //GstFormat fmt=GST_FORMAT_TIME;
    GstElement *urisrc;
    GstElement *decodebin;
    GstElement *fakesink;
    GstBus *bus=NULL;
    gint changeTimeout = 0;
    GstStateChangeReturn ret;
    GstMessage *msg;
//g_print("2\n");
    pipeline=gst_pipeline_new("pipeline");
    urisrc=gst_element_make_from_uri(GST_URI_SRC,md->uri,"urisrc");
    decodebin=gst_element_factory_make("decodebin","decodebin");
    fakesink=gst_element_factory_make("fakesink","fakesink");

    gst_bin_add_many(GST_BIN(pipeline),urisrc,decodebin,fakesink,NULL);
    gst_element_link(urisrc,decodebin);

    g_signal_connect_object(G_OBJECT(decodebin),"new-decoded-pad",G_CALLBACK(md_callback_build_pipeline),fakesink,0);
//g_print("3\n");
    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_element_set_state(pipeline,GST_STATE_NULL);
    gst_element_set_state(pipeline,GST_STATE_READY);
    ret = gst_element_set_state(pipeline,GST_STATE_PAUSED);
    //g_print("31\n");
    if(!ret) {
        //g_print("32\n");
        if(pipeline!=NULL)
            gst_object_unref(GST_OBJECT(pipeline));
        pipeline=NULL;
        return FALSE;
    }
    //g_print("33\n");
    while(ret==GST_STATE_CHANGE_ASYNC && changeTimeout < 5) {
        msg = gst_bus_timed_pop(bus, 1 * GST_SECOND);
        if(msg!=NULL) {
            //g_print("34\n");
            md_callback_get_metadata(bus, msg, md);
            gst_message_unref(msg);
            changeTimeout = 0;
        } else
            changeTimeout++;
        ret = gst_element_get_state(pipeline, NULL, NULL, 0);
    }
    //g_print("4\n");
    gst_object_unref(bus);
    md_callback_event_loop(md, pipeline, FALSE);
    if(ret!=GST_STATE_CHANGE_SUCCESS) {
        gst_element_set_state(pipeline,GST_STATE_NULL);
        return FALSE;
    }

    ret = gst_element_set_state(pipeline,GST_STATE_NULL);
    gst_object_unref(pipeline);

    gchar *str;
    gchar *c;
    //g_print("5\n");
    if(md->codec==NULL||g_strcmp0(md->codec,"")==0||g_strcmp0(md->codec,"unknown")==0) {
        str=g_strconcat(md->uri,NULL);
        c=str+strlen(str);
        while(*c!='.')
            c--;
        if(g_strcmp0(c,".mp3")==0||g_strcmp0(c,".MP3")==0) {
            //g_free_n(md->codec);
            strcpy(md->codec,"MPEG 1 Audio, Layer 3 (MP3)");
        }
        g_free_n(str);
    }
//g_print("6\n");
    //for mp3, use custom function to read tags
    if(g_strcmp0(md->codec,"MPEG 1 Audio, Layer 3 (MP3)")==0);
    {
        gchar *location=g_filename_from_uri(md->uri,NULL,NULL);
        gchar **tag=NULL;
        tag=md_get_mp3_tag(location);
        //g_free_n(location);

        if(tag!=NULL) {
            //title
            if(tag[2]!=NULL) {
                if(md->song!=NULL)
                    //g_free_n((gchar *)md->song);
                    strcpy(md->song , g_malloc(128));
                strcpy(md->song , g_utf8_strncpy((gchar *)md->song,tag[2],128));
                g_free_n(tag[2]);
            }

            //artist
            if(tag[0]!=NULL) {
                if(md->artist!=NULL)
                    //g_free_n((gchar *)md->artist);
                    strcpy(md->artist , g_malloc(128));
                strcpy(md->artist , g_utf8_strncpy((gchar *)md->artist,tag[0],128));
                g_free_n(tag[0]);
            }

            //album
            if(tag[1]!=NULL) {
                if(md->album!=NULL)
                    //g_free_n((gchar *)md->album);
                    strcpy(md->album, g_malloc(128));
                strcpy(md->album, g_utf8_strncpy((gchar *)md->album,tag[1],128));
                g_free_n(tag[1]);
            }
        }
    }
//g_print("7\n");
    if(md->song==NULL||g_strcmp0(md->song,"")==0||g_strcmp0(md->song,"unknown")==0) {
        str=(gchar *)malloc(sizeof(gchar)*512) ;
        strcpy(str,md->uri);
        //g_print("__%s\n",c);
        c=str+strlen(str);
        //g_print("__%s\n",c);
        while(*c!='.')
            c--;
        *c='\0';
        //g_print("__%s\n",c);
        while(*c!='/')
            c--;
        c++;
        //g_print("__%s\n",c);
        strcpy(md->song,c);
        g_free_n(str);
    }
    //g_print("8\n");
    pipeline=NULL;
    return md;
}
void id3_prev(GtkWidget *button,GtkWidget *dialog) {

    GtkTreeModel *model;

    GtkTreePath *path     = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_songs_treeview))),&model)->data;
    gtk_tree_path_prev(path);
    gtk_tree_view_set_cursor((GtkTreeView *) iceplayer.list_songs_treeview,path, gtk_tree_view_get_column((GtkTreeView *) iceplayer.list_songs_treeview,0), FALSE);
    if(GTK_IS_WIDGET(dialog))gtk_widget_destroy (dialog);
    id3_dialog();
}
void id3_next(GtkWidget *button,GtkWidget *dialog) {

    GtkTreeModel *model;
    GtkTreePath *path     = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_songs_treeview))),&model)->data;
    gtk_tree_path_next(path);
    gtk_tree_view_set_cursor((GtkTreeView *) iceplayer.list_songs_treeview,path, gtk_tree_view_get_column((GtkTreeView *) iceplayer.list_songs_treeview,0), FALSE);
    if(GTK_IS_WIDGET(dialog))gtk_widget_destroy (dialog);
    id3_dialog();
}
void id3_dialog() {
    ID3_tag *tag = (ID3_tag*)malloc(sizeof(ID3_tag)) ;
    pipeline=NULL;
    eos=FALSE;

    if (!main_lists.name[0]) {
        print_error("歌曲为空值！\n");
        return ;
    }
    GtkTreeModel *model;
    GList *list_select = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_songs_treeview))),&model);
    if (!list_select) {
        print_error("没有选中项!\n");
        return ;
    }
    iceplayer_songs *id3_songs = find_lists_from_name(focus_lists)->songs;
    id3_indices=atoi(gtk_tree_path_to_string((GtkTreePath *) list_select->data));
    gint indices = id3_indices;
    while (indices) {
        id3_songs = id3_songs->next;
        indices--;
    }
    gchar path[STRINGS_LENGTH];
    strcpy(path,id3_songs->uri);
    if(strstr(path,"<cue>")!=NULL) {
        //cue:
        strcpy(tag->song,cue_get_song(id3_songs->uri));
        strcpy(tag->artist,cue_get_artist(id3_songs->uri));
        strcpy(tag->album,cue_get_album(id3_songs->uri));
        strcpy(tag->track,cue_get_track(id3_songs->uri));
        strcpy(tag->codec,"cue");
        strcpy(path,strstr(path,"<cue_uri:"));
        sscanf(path,"<cue_uri:%[^>]",path);
        strcpy(tag->uri,"file://");
        strcat(tag->uri,path);
    } else {
        strcpy(tag->uri,"file://");
        strcat(tag->uri,path);
        tag = md_get_metadata(tag->uri);
    }

    //g_print("获取选中音乐metapad\n");

//g_print("初始化\n");
//g_print("%s\n", tag->song);
//g_print("%s\n", tag->artist);
//g_print("%s\n", tag->album);
//g_print("%s\n", tag->codec);
//g_print("%s\n",tag->uri );

    strcpy(gui_treeview_info_title,tag->song);
    strcpy(gui_treeview_info_artist,tag->artist);
    strcpy(gui_treeview_info_album,tag->album);
    strcpy(gui_treeview_info_genre,"unknown");
    gui_treeview_info_track=0;
    strcpy(gui_treeview_info_codec,tag->codec);
    strcpy(gui_treeview_info_bitrate,"unknown");
    strcpy(gui_treeview_info_uri,tag->uri);

    GstElement *urisrc;
    GstElement *decodebin;
    GstElement *fakesink;
    GstBus *bus=NULL;
    gint changeTimeout = 0;
    GstStateChangeReturn ret;
    GstMessage *msg;


    if(strstr(id3_songs->uri,"<cue>")==NULL) {
        //g_print("链接管道获取信息\n");

        pipeline=gst_pipeline_new("pipeline");
        urisrc=gst_element_make_from_uri(GST_URI_SRC,gui_treeview_info_uri,"urisrc");
        decodebin=gst_element_factory_make("decodebin","decodebin");
        fakesink=gst_element_factory_make("fakesink","fakesink");
//g_print("链接管道获取信息1\n");
        gst_bin_add_many(GST_BIN(pipeline),urisrc,decodebin,fakesink,NULL);
        gst_element_link(urisrc,decodebin);

        g_signal_connect_object(G_OBJECT(decodebin),"new-decoded-pad",G_CALLBACK(gui_treeview_callback_callback_build_pipeline),fakesink,0);

        bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
        gst_element_set_state(pipeline, GST_STATE_NULL);
        ret = gst_element_set_state(pipeline,GST_STATE_PAUSED);
        if(!ret) {
            if(pipeline!=NULL)
                gst_object_unref(GST_OBJECT(pipeline));
            pipeline=NULL;
            return;
        }
        //g_print("链接管道获取信息2\n");
        while(ret==GST_STATE_CHANGE_ASYNC && !eos && changeTimeout < 5) {
            //g_print("21\n");
            msg = gst_bus_timed_pop(bus, 1 * GST_SECOND);
            if(msg!=NULL) {
                //g_print("链接管道获取信息22\n");
                gui_treeview_callback_callback_get_metadata(bus, msg, NULL);
                gst_message_unref(msg);
                changeTimeout = 0;
            } else {
                //g_print("链接管道获取信息23\n");
                changeTimeout++;
            }
            ret = gst_element_get_state(pipeline, NULL, NULL, 0);
        }
        //g_print("链接管道获取信息3\n");
        gst_object_unref(bus);
        gui_treeview_callback_callback_event_loop(pipeline,FALSE);
        if(ret!=GST_STATE_CHANGE_SUCCESS) {
            gst_element_set_state(pipeline,GST_STATE_NULL);
            return;
        }

        pipeline=NULL;
        eos=FALSE;
    }
    //g_print("建立对话框\n");////
    enum {	INFO_TITLE,
            INFO_ARTIST,
            INFO_ALBUM,
            INFO_GENRE,
            INFO_TRACK,
            INFO_NUM
         };
    GtkWidget *dialog;
    //GtkWidget *dialog1;
    GtkWidget *frame;
    GtkWidget *hbox;
    GtkWidget *table;
    GtkWidget *label;
    GtkWidget *entry[INFO_NUM];
    //GtkWidget *image;

    gchar *track=(gchar*)g_malloc(64);
    if(strstr(id3_songs->uri,"<cue>")!=NULL) {
        strcpy(track,tag->track);
    } else {
        g_snprintf(track,63,"%02d",gui_treeview_info_track);
    }
    dialog=gtk_dialog_new_with_buttons(	_("歌曲信息（切换上下首不自动保存）"),
                                        GTK_WINDOW(iceplayer.window),
                                        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                                        GTK_STOCK_OK,
                                        GTK_RESPONSE_ACCEPT,
                                        GTK_STOCK_CANCEL,
                                        GTK_RESPONSE_REJECT,
                                        NULL);
    //gtk_container_set_border_width(GTK_CONTAINER(dialog),8);
    //gtk_window_set_default_size(GTK_WINDOW(dialog),400,350);

    //gint changed=0;

    frame=gtk_frame_new(_("Basic Information"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),frame,TRUE,TRUE,0);
    hbox=gtk_hbox_new(FALSE,0);
    gtk_container_add(GTK_CONTAINER(frame),hbox);
    table=gtk_table_new(5,2,FALSE);
    gtk_container_border_width(GTK_CONTAINER(table),5);
    gtk_box_pack_start(GTK_BOX(hbox),table,TRUE,TRUE,0);
    label=gtk_label_new(_("Codec:"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
    label=gtk_label_new(_("Bitrate:"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
    label=gtk_label_new(_("Uri:"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 4, 5, GTK_FILL, GTK_FILL, 5, 5);
    label=gtk_label_new(gui_treeview_info_codec);
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_END);
    gtk_label_set_max_width_chars(GTK_LABEL(label),25);
    gtk_table_attach(GTK_TABLE(table), label, 1, 2, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
    label=gtk_label_new(gui_treeview_info_bitrate);
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_END);
    gtk_label_set_max_width_chars(GTK_LABEL(label),25);
    gtk_table_attach(GTK_TABLE(table), label, 1, 2, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
    label=gtk_label_new(gui_treeview_info_uri);
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_label_set_ellipsize(GTK_LABEL(label),PANGO_ELLIPSIZE_MIDDLE);
    gtk_label_set_max_width_chars(GTK_LABEL(label),25);
    gtk_table_attach(GTK_TABLE(table), label, 1, 2, 4, 5, GTK_FILL, GTK_FILL, 5, 5);
    //image=gtk_image_new_from_pixbuf(gui_treeview_info_pixbuf);
    //gtk_table_attach(GTK_TABLE(table), image, 2, 3, 0, 5, GTK_FILL, GTK_FILL, 5, 5);

    frame=gtk_frame_new(_("Advance Information"));
    gtk_box_pack_start(GTK_BOX(GTK_DIALOG(dialog)->vbox),frame,TRUE,TRUE,0);
    table=gtk_table_new(5,2,FALSE);
    //gtk_container_border_width(GTK_CONTAINER(table),0);
    gtk_container_add(GTK_CONTAINER(frame),table);
    label=gtk_label_new(_("Title:"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 0, 1, GTK_FILL, GTK_FILL, 5, 5);
    label=gtk_label_new(_("Artist:"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 1, 2, GTK_FILL, GTK_FILL, 5, 5);
    label=gtk_label_new(_("Album:"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 2, 3, GTK_FILL, GTK_FILL, 5, 5);
    label=gtk_label_new(_("genre:"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 3, 4, GTK_FILL, GTK_FILL, 5, 5);
    label=gtk_label_new(_("track:"));
    gtk_misc_set_alignment(GTK_MISC(label), 0, 0);
    gtk_table_attach(GTK_TABLE(table), label, 0, 1, 4, 5, GTK_FILL, GTK_FILL, 5, 5);
    entry[INFO_TITLE]=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry[INFO_TITLE]),gui_treeview_info_title);
    gtk_table_attach(GTK_TABLE(table), entry[INFO_TITLE], 1, 2, 0, 1, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);
    entry[INFO_ARTIST]=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry[INFO_ARTIST]),gui_treeview_info_artist);
    gtk_table_attach(GTK_TABLE(table), entry[INFO_ARTIST], 1, 2, 1, 2, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);
    entry[INFO_ALBUM]=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry[INFO_ALBUM]),gui_treeview_info_album);
    gtk_table_attach(GTK_TABLE(table), entry[INFO_ALBUM], 1, 2, 2, 3, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);
    entry[INFO_GENRE]=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry[INFO_GENRE]),gui_treeview_info_genre);
    gtk_table_attach(GTK_TABLE(table), entry[INFO_GENRE], 1, 2, 3, 4, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);
    entry[INFO_TRACK]=gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(entry[INFO_TRACK]),track);
    gtk_table_attach(GTK_TABLE(table), entry[INFO_TRACK], 1, 2, 4, 5, GTK_FILL | GTK_EXPAND, GTK_FILL | GTK_EXPAND, 0, 0);


    id3.button_prev = gtk_button_new_from_stock ("gtk-media-previous");
    gtk_widget_show (id3.button_prev);
    gtk_container_add (GTK_CONTAINER (((GtkDialog *)dialog)->action_area), id3.button_prev);
    GTK_WIDGET_SET_FLAGS (id3.button_prev, GTK_CAN_DEFAULT);
    gtk_button_set_relief (GTK_BUTTON (id3.button_prev), GTK_RELIEF_NONE);

    id3.button_next = gtk_button_new_from_stock ("gtk-media-next");
    gtk_widget_show (id3.button_next);
    gtk_container_add (GTK_CONTAINER (((GtkDialog *)dialog)->action_area), id3.button_next);
    GTK_WIDGET_SET_FLAGS (id3.button_next, GTK_CAN_DEFAULT);
    gtk_button_set_relief (GTK_BUTTON (id3.button_next), GTK_RELIEF_NONE);

    g_signal_connect( id3.button_prev, "clicked",G_CALLBACK(id3_prev), dialog);
    g_signal_connect( id3.button_next, "clicked",G_CALLBACK(id3_next), dialog);

    if(strstr(id3_songs->uri,"<cue>")!=NULL) {
        gtk_widget_set_sensitive(entry[INFO_TITLE],FALSE);
        gtk_widget_set_sensitive(entry[INFO_ARTIST],FALSE);
        gtk_widget_set_sensitive(entry[INFO_ALBUM],FALSE);
        gtk_widget_set_sensitive(entry[INFO_GENRE],FALSE);
        gtk_widget_set_sensitive(entry[INFO_TRACK],FALSE);
    }
    gui_treeview_info_codec[0]=' ';
    gui_treeview_info_start[0]=' ';
    gui_treeview_info_duration[0]=' ';
    gui_treeview_info_bitrate[0]=' ';
    gui_treeview_info_uri[0]=' ';
    gui_treeview_info_title[0]=' ';
    gui_treeview_info_artist[0]=' ';
    gui_treeview_info_album[0]=' ';
    gui_treeview_info_genre[0]=' ';
    track=NULL;

    gtk_widget_show_all(dialog);
    gint result = gtk_dialog_run (GTK_DIALOG (dialog));
    switch (result) {
    case GTK_RESPONSE_ACCEPT:
        if(strstr(id3_songs->uri,"<cue>")==NULL) {
            return;
        }
        break;
    case GTK_RESPONSE_REJECT:
        if(GTK_IS_WIDGET(dialog))gtk_widget_destroy (dialog);
        return;
        break;
    default:
        if(GTK_IS_WIDGET(dialog))gtk_widget_destroy (dialog);
        return;
        break;
    }

    strcpy(tag->song,g_strconcat(gtk_entry_get_text(GTK_ENTRY(entry[INFO_TITLE])),NULL));
    strcpy(tag->artist,g_strconcat(gtk_entry_get_text(GTK_ENTRY(entry[INFO_ARTIST])),NULL));
    strcpy(tag->album,g_strconcat(gtk_entry_get_text(GTK_ENTRY(entry[INFO_ALBUM])),NULL));

    strcpy(location,g_filename_from_uri(tag->uri,NULL,NULL));
    pipeline=NULL;
    GstElement *filesrc=NULL;
    GstElement *demux=NULL;
    GstElement *mux=NULL;
    GstElement *parse=NULL;
    GstElement *filesink=NULL;
    GstElement *tagger=NULL;
    bus = NULL;
    const gchar *codec=NULL;

    pipeline=gst_pipeline_new("pipeline");
    filesrc=gst_element_factory_make("filesrc","filesrc");
    filesink=gst_element_factory_make("filesink","filesink");
    if(!pipeline||!filesrc||!filesink) {
        if(GTK_IS_WIDGET(dialog))gtk_widget_destroy (dialog);
        return;
    }
    //MP3
    if(strstr(tag->codec,"MP3")!=NULL) {
        demux=gst_element_factory_make("id3demux","demux");
        tagger=gst_element_factory_make("id3v2mux","tagger");
        codec="LAME";
    }
    //OGG
    else if(strstr(tag->codec,"Vorbis")!=NULL) {
        tagger = gst_element_factory_make("vorbistag", "tagger");
        demux=gst_element_factory_make("oggdemux","demux");
        mux=gst_element_factory_make("oggmux","mux");
        parse = gst_element_factory_make("vorbisparse", "parse");
        codec = "Vorbis";
    }
    //FLAC
    else if(strstr(tag->codec,"FLAC")!=NULL) {
        tagger = gst_element_factory_make("flactag", "tagger");
        codec="FLAC";
    }
    //APE
    else if(strstr(tag->codec,"Monkey's Audio")!=NULL) {
        demux=gst_element_factory_make("apedemux","demux");
        tagger=gst_element_factory_make("apev2mux","tagger");
        codec="LAME";
    } else {
        if(GTK_IS_WIDGET(dialog))gtk_widget_destroy (dialog);
        return;
    }

    g_object_set(G_OBJECT(filesrc), "location", location, NULL);
    g_object_set(G_OBJECT(filesink), "location", "/tmp/iceplayer_id3.audio", NULL);

    sscanf(gtk_entry_get_text(GTK_ENTRY(entry[INFO_TRACK])),"%d", &gui_treeview_info_track);
    gst_tag_setter_add_tags(GST_TAG_SETTER(tagger),
                            GST_TAG_MERGE_REPLACE_ALL,
                            GST_TAG_TITLE, gtk_entry_get_text(GTK_ENTRY(entry[INFO_TITLE])),
                            GST_TAG_ARTIST, gtk_entry_get_text(GTK_ENTRY(entry[INFO_ARTIST])),
                            GST_TAG_ALBUM, gtk_entry_get_text(GTK_ENTRY(entry[INFO_ALBUM])),
                            GST_TAG_GENRE, gtk_entry_get_text(GTK_ENTRY(entry[INFO_GENRE])),
                            GST_TAG_TRACK_NUMBER, gui_treeview_info_track,
                            GST_TAG_ENCODER, "iceplayer",
                            GST_TAG_ENCODER_VERSION, 0,
                            GST_TAG_CODEC,codec,
                            NULL);
    if(GTK_IS_WIDGET(dialog))gtk_widget_destroy (dialog);

    //MP3
    if(strstr(tag->codec,"MP3")!=NULL) {
        gst_bin_add_many(GST_BIN(pipeline), filesrc, demux,tagger,filesink, NULL);
        g_signal_connect(demux, "pad-added",G_CALLBACK(gui_treeview_callback_add_id3_pad), tagger);
        gst_element_link(filesrc, demux);
        gst_element_link(tagger, filesink);
    }
    //OGG
    else if(strstr(tag->codec,"Vorbis")!=NULL) {
        gst_bin_add_many(GST_BIN(pipeline), filesrc, demux, tagger, parse, mux, filesink, NULL);
        g_signal_connect(demux, "pad-added",G_CALLBACK(gui_treeview_callback_add_ogg_pad), tagger);
        gst_element_link(filesrc, demux);
        gst_element_link_many(tagger, parse, mux, filesink,NULL);
    }
    //FLAC
    else if(strstr(tag->codec,"FLAC")!=NULL) {
        gst_bin_add_many(GST_BIN(pipeline), filesrc, tagger,filesink, NULL);
        gst_element_link_many(filesrc, tagger, filesink, NULL);
    }
    //APE
    else if(strstr(tag->codec,"Monkey's Audio")!=NULL) {
        gst_bin_add_many(GST_BIN(pipeline), filesrc, demux,tagger,filesink, NULL);
        g_signal_connect(demux, "pad-added",G_CALLBACK(gui_treeview_callback_add_id3_pad), tagger);
        gst_element_link(filesrc, demux);
        gst_element_link(tagger, filesink);
    } else {
        gst_bin_add_many(GST_BIN(pipeline), filesrc, tagger,filesink, NULL);
        gst_element_link_many(filesrc, tagger, filesink, NULL);
    }
    bus = gst_pipeline_get_bus(GST_PIPELINE(pipeline));
    gst_bus_add_watch(bus, (GstBusFunc)tag_setter_handler, NULL);
    gst_object_unref(bus);
    gst_element_set_state(pipeline, GST_STATE_NULL);
    gst_element_set_state(pipeline, GST_STATE_READY);

    if(gst_element_set_state(pipeline, GST_STATE_PLAYING)==GST_STATE_CHANGE_FAILURE) {
        gst_object_unref(pipeline);
    }
    return;
}



/*

void id3_prev  ()
{
  print_programming("id3_prev\n");
  id3_save();
  gint indices;
  if (id3_indices!=0)
    {
      indices = --id3_indices;
    }
  else
    indices = 0;

  iceplayer_songs *id3_songs = find_lists_from_name(focus_lists)->songs;
  while (indices)
    {
      id3_songs = id3_songs->next;
      indices--;
    }
  gchar path[STRINGS_LENGTH*3];
  strcpy(path,id3_songs->uri);
  if(strstr(path,"<cue>")!=NULL)
    {
      //cue:
      strcpy(path,strstr(path,"<cue_uri:"));
      sscanf(path,"<cue_uri:%[^>]",path);
    }
  id3_set_entry_text(get_tag(path),path);
}
void id3_next  ()
{
  print_programming("id3_next\n");
  id3_save();
  iceplayer_songs *id3_songs = find_lists_from_name(focus_lists)->songs;
  gint cou=0;
  while (id3_songs)
    {
      cou++;
      id3_songs = id3_songs->next;
    }
  if (id3_indices<cou)
    ++id3_indices;

  gint indices = id3_indices;

  id3_songs = find_lists_from_name(focus_lists)->songs;
  while (indices&&id3_songs->next)
    {
      id3_songs = id3_songs->next;
      indices--;
    }
  gchar path[STRINGS_LENGTH*3];
  strcpy(path,id3_songs->uri);
  if(strstr(path,"<cue>")!=NULL)
    {
      //cue:
      strcpy(path,strstr(path,"<cue_uri:"));
      sscanf(path,"<cue_uri:%[^>]",path);
    }
  id3_set_entry_text(get_tag(path),path);
}
void id3_clean_word(gchar name[STRINGS_LENGTH])
{
  print_programming("id3_clean_word\n");
  gint n=0;
  gint m=0;
  gchar name2[STRINGS_LENGTH];
  while(name[n])
    {
      if(name[n] =='\''||name[n] =='\\'||name[n] =='"'||name[n] =='&'||name[n] =='('||name[n] ==')')
        name2[m++] = '\\';
      if(name[n] ==' ')
        name[n] = '_';
      name2[m++]=name[n++];
    }
  name2[m]=0;
  strcpy(name,name2);
}
void id3_save  ()
{
  print_programming("id3_save\n");
  gint indices = id3_indices;
  iceplayer_songs *id3_songs = find_lists_from_name(focus_lists)->songs;
  while (indices&&id3_songs->next)
    {
      id3_songs = id3_songs->next;
      indices--;
    }
  if (!id3_songs)
    {
      print_error("id3_songs为空值！无法得到欲改动的文件路径！无法保存标签！\n");
      return;
    }
  gchar path[STRINGS_LENGTH*3];
  strcpy(path,id3_songs->uri);
  if (path[strlen(path)-4]=='.'&&path[strlen(path)-3]=='m'&&path[strlen(path)-2]=='p'&&path[strlen(path)-1]=='3')
    {

      ID3_tag *tag = (ID3_tag*)malloc(sizeof(ID3_tag));

void strcpy_n(gchar *a,gchar *b){
if(*a!=NULL&&*b!=NULL)
strcpy(a,b);
}
      strcpy_n(tag->name,gtk_entry_get_text((GtkEntry *)id3.entry_title) );
      strcpy_n(tag->editor, gtk_entry_get_text((GtkEntry *)id3.entry_artist));
      strcpy_n(tag->album,gtk_entry_get_text((GtkEntry *)id3.entry_album) );
      strcpy_n(tag->year,gtk_entry_get_text((GtkEntry *)id3.entry_year) );
      strcpy_n(tag->comment,gtk_entry_get_text((GtkEntry *)id3.entry_comment) );
      strcpy_n(tag->track,gtk_entry_get_text((GtkEntry *)id3.entry_track) );

      id3_clean_word(tag->name);
      id3_clean_word(tag->editor);
      id3_clean_word(tag->album);
      id3_clean_word(tag->year);
      id3_clean_word(tag->comment);
      id3_clean_word(tag->track);

      gchar su[STRINGS_LENGTH*3];
      strcpy(su,"ln -f -s \"");
      strcat(su,path);
      strcat(su,"\" /tmp/iceplayer.tmp.mp3");
      system(su);

      strcpy(su,"@datadir@/iceplayer/id3 -tag -edit -quiet ");

      strcat(su," -song=");
      if(tag->name[0])strcat(su,tag->name);
      else strcat(su,"\r");

      strcat(su," -artist=");
      if(tag->editor[0])strcat(su,tag->editor);
      else strcat(su,"\r");

      if(tag->album[0])
        {
          strcat(su," -album=");
          strcat(su,tag->album);
        }
      else strcat(su," -noalbum ");
      if(tag->year[0])
        {
          strcat(su," -year=");
          strcat(su,tag->year);
        }
      else strcat(su," -noyear ");
      if(tag->comment[0])
        {
          strcat(su," -comment=");
          strcat(su,tag->comment);
        }
      else strcat(su," -nocomment ");
      if(tag->track[0])
        {
          strcat(su," -track=");
          strcat(su,tag->track);
        }
      else strcat(su," -notrack ");
      strcat(su," -nogenre ");
      strcat(su, " /tmp/iceplayer.tmp.mp3");

      system(su);

      unlink("/tmp/iceplayer.tmp.mp3");
    }
  else
    {
      print_programming("不是mp3文件，拒绝保存！");
    }
}
void id3_close ()
{
  print_programming("id3_close\n");
  gtk_widget_destroy(id3.id3_editor);
}
void id3_dialog2()
{
  print_programming("id3_dialog\n");
  id3.id3_editor = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (id3.id3_editor), "Id3 Editor (切换上下首和关闭会自动保存修改)");
  gtk_window_set_position (GTK_WINDOW (id3.id3_editor), GTK_WIN_POS_CENTER);
  gtk_window_set_resizable (GTK_WINDOW (id3.id3_editor), FALSE);
  gtk_window_set_keep_above(GTK_WINDOW(id3.id3_editor),TRUE);

  id3.vbox = gtk_vbox_new (FALSE, 0);
  gtk_widget_show (id3.vbox);
  gtk_container_add (GTK_CONTAINER (id3.id3_editor), id3.vbox);
  gtk_container_set_border_width (GTK_CONTAINER (id3.vbox), 9);

  id3.hbox = gtk_hbox_new (FALSE, 0);
  gtk_widget_show (id3.hbox);
  gtk_box_pack_start (GTK_BOX (id3.vbox), id3.hbox, FALSE, FALSE, 0);

  id3.label_path = gtk_label_new ("文件");
  gtk_widget_show (id3.label_path);
  gtk_box_pack_start (GTK_BOX (id3.hbox), id3.label_path, FALSE, FALSE, 0);

  id3.entry_folder = gtk_entry_new ();
  gtk_widget_show (id3.entry_folder);
  gtk_box_pack_start (GTK_BOX (id3.hbox), id3.entry_folder, TRUE, TRUE, 0);
  gtk_widget_set_size_request (id3.entry_folder, 200, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (id3.entry_folder), 9679);

  id3.entry_name = gtk_entry_new ();
  gtk_widget_show (id3.entry_name);
  gtk_box_pack_start (GTK_BOX (id3.hbox), id3.entry_name, FALSE, FALSE, 0);
  gtk_widget_set_size_request (id3.entry_name, 150, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (id3.entry_name), 9679);

  id3.entry_suffix = gtk_entry_new ();
  gtk_widget_show (id3.entry_suffix);
  gtk_box_pack_start (GTK_BOX (id3.hbox), id3.entry_suffix, FALSE, FALSE, 0);
  gtk_widget_set_size_request (id3.entry_suffix, 60, -1);
  gtk_entry_set_invisible_char (GTK_ENTRY (id3.entry_suffix), 9679);

  id3.table = gtk_table_new (7, 2, FALSE);
  gtk_widget_show (id3.table);
  gtk_box_pack_start (GTK_BOX (id3.vbox), id3.table, FALSE, FALSE, 0);

  id3.label_title = gtk_label_new ("标题");
  gtk_widget_show (id3.label_title);
  gtk_table_attach (GTK_TABLE (id3.table), id3.label_title, 0, 1, 0, 1, (GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (id3.label_title), 0, 0.5);

  id3.label_atrist = gtk_label_new ("作者");
  gtk_widget_show (id3.label_atrist);
  gtk_table_attach (GTK_TABLE (id3.table), id3.label_atrist, 0, 1, 1, 2, (GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (id3.label_atrist), 0, 0.5);

  id3.label_album = gtk_label_new ("专辑");
  gtk_widget_show (id3.label_album);
  gtk_table_attach (GTK_TABLE (id3.table), id3.label_album, 0, 1, 2, 3, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (id3.label_album), 0, 0.5);

  id3.label_year = gtk_label_new ("年份");
  gtk_widget_show (id3.label_year);
  gtk_table_attach (GTK_TABLE (id3.table), id3.label_year, 0, 1, 3, 4, (GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (id3.label_year), 0, 0.5);

  id3.label_genre = gtk_label_new ("风格");
  //gtk_widget_show (id3.label_genre);
  //gtk_table_attach (GTK_TABLE (id3.table), id3.label_genre, 0, 1, 4, 5,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (id3.label_genre), 0, 0.5);

  id3.label_track = gtk_label_new ("轨道");
  gtk_widget_show (id3.label_track);
  gtk_table_attach (GTK_TABLE (id3.table), id3.label_track, 0, 1, 5, 6,(GtkAttachOptions) (GTK_FILL),(GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (id3.label_track), 0, 0.5);

  id3.label_comment = gtk_label_new ("注释");
  gtk_widget_show (id3.label_comment);
  gtk_table_attach (GTK_TABLE (id3.table), id3.label_comment, 0, 1, 6, 7,(GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_misc_set_alignment (GTK_MISC (id3.label_comment), 0, 0.5);

  id3.entry_comment = gtk_entry_new ();
  gtk_widget_show (id3.entry_comment);
  gtk_table_attach (GTK_TABLE (id3.table), id3.entry_comment, 1, 2, 6, 7, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (id3.entry_comment), 9679);

  id3.entry_track = gtk_entry_new ();
  gtk_widget_show (id3.entry_track);
  gtk_table_attach (GTK_TABLE (id3.table), id3.entry_track, 1, 2, 5, 6, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (id3.entry_track), 9679);

  id3.entry_year = gtk_entry_new ();
  gtk_widget_show (id3.entry_year);
  gtk_table_attach (GTK_TABLE (id3.table), id3.entry_year, 1, 2, 3, 4,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),  (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (id3.entry_year), 9679);

  id3.entry_artist = gtk_entry_new ();
  gtk_widget_show (id3.entry_artist);
  gtk_table_attach (GTK_TABLE (id3.table), id3.entry_artist, 1, 2, 1, 2, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL), (GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (id3.entry_artist), 9679);

  id3.entry_album = gtk_entry_new ();
  gtk_widget_show (id3.entry_album);
  gtk_table_attach (GTK_TABLE (id3.table), id3.entry_album, 1, 2, 2, 3, (GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (id3.entry_album), 9679);


  id3.entry_title = gtk_entry_new ();
  gtk_widget_show (id3.entry_title);
  gtk_table_attach (GTK_TABLE (id3.table), id3.entry_title, 1, 2, 0, 1,(GtkAttachOptions) (GTK_EXPAND | GTK_FILL),(GtkAttachOptions) (0), 0, 0);
  gtk_entry_set_invisible_char (GTK_ENTRY (id3.entry_title), 9679);

  id3.combobox_genre = gtk_combo_box_new_text ();
  //gtk_widget_show (id3.combobox_genre);
  //gtk_table_attach (GTK_TABLE (id3.table), id3.combobox_genre, 1, 2, 4, 5, (GtkAttachOptions) (GTK_FILL), (GtkAttachOptions) (GTK_FILL), 0, 0);

  id3.hbuttonbox = gtk_hbutton_box_new ();
  gtk_widget_show (id3.hbuttonbox);
  gtk_box_pack_start (GTK_BOX (id3.vbox), id3.hbuttonbox, FALSE, FALSE, 0);
  gtk_button_box_set_layout (GTK_BUTTON_BOX (id3.hbuttonbox), GTK_BUTTONBOX_EDGE);

  id3.button_prev = gtk_button_new_from_stock ("gtk-media-previous");
  gtk_widget_show (id3.button_prev);
  gtk_container_add (GTK_CONTAINER (id3.hbuttonbox), id3.button_prev);
  GTK_WIDGET_SET_FLAGS (id3.button_prev, GTK_CAN_DEFAULT);
  gtk_button_set_relief (GTK_BUTTON (id3.button_prev), GTK_RELIEF_NONE);

  id3.button_next = gtk_button_new_from_stock ("gtk-media-next");
  gtk_widget_show (id3.button_next);
  gtk_container_add (GTK_CONTAINER (id3.hbuttonbox), id3.button_next);
  GTK_WIDGET_SET_FLAGS (id3.button_next, GTK_CAN_DEFAULT);
  gtk_button_set_relief (GTK_BUTTON (id3.button_next), GTK_RELIEF_NONE);

  id3.button_save = gtk_button_new_from_stock ("gtk-close");
  gtk_widget_show (id3.button_save);
  gtk_container_add (GTK_CONTAINER (id3.hbuttonbox), id3.button_save);
  GTK_WIDGET_SET_FLAGS (id3.button_save, GTK_CAN_DEFAULT);
  gtk_button_set_relief (GTK_BUTTON (id3.button_save), GTK_RELIEF_NONE);

  g_signal_connect ((gpointer) id3.button_prev, "clicked",G_CALLBACK (id3_prev), NULL);
  g_signal_connect ((gpointer) id3.button_next, "clicked",G_CALLBACK (id3_next),NULL);
  g_signal_connect ((gpointer) id3.button_save, "clicked",G_CALLBACK (id3_close),NULL);

  gtk_widget_grab_focus (id3.button_next);

  if (!main_lists.name[0])
    {
      print_error("歌曲为空值！\n");
      return ;
    }
  GtkTreeModel *model;
  GtkTreeIter iter;
  GList *list_select = gtk_tree_selection_get_selected_rows(GTK_TREE_SELECTION(gtk_tree_view_get_selection(GTK_TREE_VIEW(iceplayer.list_songs_treeview))),&model);
  if (!list_select)
    {
      print_error("没有选中项!\n");
      return ;
    }
  iceplayer_songs *id3_songs = find_lists_from_name(focus_lists)->songs;
  id3_indices=atoi(gtk_tree_path_to_string((GtkTreePath *) list_select->data));
  gint indices = id3_indices;
  while (indices)
    {
      id3_songs = id3_songs->next;
      indices--;
    }
  gchar path[STRINGS_LENGTH];
  strcpy(path,id3_songs->uri);
  if(strstr(path,"<cue>")!=NULL)
    {
      //cue:
      strcpy(path,strstr(path,"<cue_uri:"));
      sscanf(path,"<cue_uri:%[^>]",path);
    }
  ID3_tag *tag = get_tag(path) ;

  gchar folder[STRINGS_LENGTH*3];
  gchar name[STRINGS_LENGTH];
  gchar suffix[STRINGS_LENGTH];
  strcpy(folder,path);
  gint a=strlen(path);
  while (folder[a]!='/'&&a!=0)
    a--;
  if (a!=0)
    {
      folder[a]=0;
      gint b=0;
      a++;
      while( (path[a]!='.'&&path[a]!=0)||(path[a+4]!=0&&path[a+5]!=0))
        name[b++]=path[a++];
      name[b]=0;
      if (path[a])
        {
          gint b=0;
          while (path[a]!=0)
            suffix[b++]=path[a++];
          suffix[b]=0;
          gtk_entry_set_text((GtkEntry *)id3.entry_suffix,suffix);
        }

      gtk_entry_set_text((GtkEntry *)id3.entry_folder,folder);
      gtk_entry_set_text((GtkEntry *)id3.entry_name,name);
    }

  gtk_entry_set_editable((GtkEntry *)id3.entry_folder,FALSE);
  gtk_entry_set_editable((GtkEntry *)id3.entry_name,FALSE);
  gtk_entry_set_editable((GtkEntry *)id3.entry_suffix,FALSE);

  if (tag)
    {
      gtk_entry_set_text((GtkEntry *)id3.entry_artist,tag->editor);
      gtk_entry_set_text((GtkEntry *)id3.entry_album,tag->album);
      gtk_entry_set_text((GtkEntry *)id3.entry_title,tag->name);

      gtk_entry_set_text((GtkEntry *)id3.entry_comment,tag->comment);
      gtk_entry_set_text((GtkEntry *)id3.entry_track,tag->track);
      gtk_entry_set_text((GtkEntry *)id3.entry_year,tag->year);
    }

  gtk_widget_show_all(id3.id3_editor);
}
*/

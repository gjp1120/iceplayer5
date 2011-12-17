#include <gtk/gtk.h>

#ifndef __ID3_H
#define __ID3_H

#define TAGLEN_TAG 3
#define TAGLEN_SONG 30
#define TAGLEN_ARTIST 30
#define TAGLEN_ALBUM 30
#define TAGLEN_YEAR 4
#define TAGLEN_COMMENT 30
#define TAGLEN_GENRE 1

typedef struct ID3_struct{
  gchar tag[TAGLEN_TAG+1];
  gchar song[TAGLEN_SONG+1];
  gchar artist[TAGLEN_ARTIST+1];
  gchar album[TAGLEN_ALBUM+1];
  gchar year[TAGLEN_YEAR+1];
  gchar comment[TAGLEN_COMMENT+1];
  gchar genre[100];
  gchar track[5];

	gchar		codec[100];		//歌曲格式
	//gchar		*start;		//歌曲起始时间（对CUE分割的文件有用,默认为"00:00:00"）
	//gchar		*duration;	//歌曲总时长（不一定就是文件的总时长）
	gchar	uri[1000];		//歌曲位置
} ID3_tag;

typedef struct iceplayer_id3{
    GtkWidget *button_prev;
    GtkWidget *button_next;
}iceplayer_id3;

void id3_dialog() ;

#endif //__ID3_H

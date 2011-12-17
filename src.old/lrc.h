#include "configure.h"

#ifndef __LRC_H
#define __LRC_H

#ifdef __cplusplus
extern "C" {
#endif

  typedef struct iceplayer_lrc_search
  {
    struct iceplayer_lrc_search* next;
    gchar editor[STRINGS_LENGTH];
    gchar title[STRINGS_LENGTH];
    gchar alubm[STRINGS_LENGTH];
    gchar uri[STRINGS_LENGTH];
  } iceplayer_lrc_search;

  typedef struct iceplayer_lrc_search_data {
    gchar keyword[STRINGS_LENGTH];
    gchar lrc_to_save[STRINGS_LENGTH];
    gchar lrc_to_del[STRINGS_LENGTH];
    gint search_lrc_state;
  } iceplayer_lrc_search_data;

  extern gboolean wait;
  extern iceplayer_lrc_search_data lrc_search_data;
  extern gchar lrc_text[256][256];
  extern gint widget_lrc_word_height;
  extern gint desktop_lrc_word_height;
  extern gchar *searching_lrc;
  
  void search_lrc_pre();
  void on_downloader_button_search_clicked(GtkButton *button,
										   GtkWidget *iceplayer_lrc_downloader_entry);
  gboolean on_downloader_entry_button_press();
  void on_downloader_list_selection_changed(GtkList *list,
											GtkWidget *iceplayer_lrc_downloader_list);
  void on_downloader_button_close_clicked(GtkButton *button,
										  GtkWidget *iceplayer_lrc_downloader);
  void on_downloader_button_lrc123_enter(GtkButton *button,
										 GtkWidget *iceplayer_lrc_downloader);
  gboolean on_downloader_button_download_clicked(GtkWidget *data,
												 GtkWidget *widget);
  gboolean on_downloader_list_key_press(GtkWidget *iceplayer_lrc_downloader_list,GdkEventKey *event);
  gboolean on_downloader_entry_key_press(GtkWidget *iceplayer_lrc_downloader_entry,GdkEventKey *event,GtkWidget *iceplayer_lrc_downloader);
  gboolean choose_lrc_to_download(iceplayer_lrc_search *lrc_search);
  gpointer search_lrc(gpointer);
  const gchar *get_lrc_path(const gchar *song_uri);
  void lrc_init() ;
  gboolean lrc_refresh();
  gboolean lrc();
  const gchar *get_lrc_file_path_at_song_path(const gchar *song_uri);
  const gchar *get_lrc_file_path_at_custom_path(const gchar *song_uri,
												gint type);
  gint get_lrc_number();
  gchar *lrc_text_get_text(gchar *lrc_text_with_time);
  gboolean lrc_file_to_lrc_text(gchar lrc_path[STRINGS_LENGTH]);
  gchar *playing_song_path_without_some_word(gchar playing_song_path[STRINGS_LENGTH]);

#ifdef __cplusplus
};
#endif

#endif //__LRC_H

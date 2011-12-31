/*
 * GUI 
 *
 * gui_listview.c
 * This file is part of <iceplayer>
 *
 * Copyright (C) 2010 - gjp1120, license: GPL v3
 *
 * <iceplayer> is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * <iceplayer> is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with <iceplayer>; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */
#include "common.h"
#include "gui_listview.h"

/**
 * SECTION: gui_listview
 * @Short_description: 播放器用户图形界面的实现，列表部分
 * @Title: 列表界面
 * @Include: gui_listview.h
 *
 * 负责构建播放器的图形界面中的播放列表表和歌曲列表
 */

static iceplayer_GuiData_t *icep_gui;
static iceplayer_ListStoreData_t icep_liststore;
static __thread GError *error = NULL;

/**
 * static InitListStoreSongs():
 *
 * 这个函数用于初始化一个测试用的播放列表，以后也许没用？
 * 起码目前，这个函数仅应由GUI_InitListViewSongs()调用
 *
 * Returns: TRUE代表成功
 */

static gboolean InitListStoreSongs(void)
{
  print_programming("GUI::ListStoreSongs::init()");

  GtkTreeIter iter;

  icep_liststore.liststore_songs = gtk_list_store_new(NUM_SONGLIST,
													  G_TYPE_UINT,
													  G_TYPE_STRING,
													  G_TYPE_STRING,
													  G_TYPE_STRING,
													  G_TYPE_UINT);

  gtk_list_store_append(icep_liststore.liststore_songs, &iter);
  gtk_list_store_set(icep_liststore.liststore_songs, &iter,
					 SONG_TRACK_NUM, 1,
					 SONG_TITLE_STR, "Test",
					 SONG_ALBUM_STR, "None",
					 SONG_ARTIST_STR, "None",
					 SONG_LENGTH_INT, 100,
					 -1);
  return TRUE;
}

/**
 * GUI_InitListViewSongs():
 *
 * 这个函数用于初始化歌曲列表的TreeView;
 *
 * Returns: TRUE代表成功
 */

gboolean GUI_InitListViewSongs(GtkWidget *treeview)
{
  print_programming("GUI::ListViewSongs::init()");

  GtkCellRenderer *render;

  InitListStoreSongs();

  render = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
											  -1, _("Track"), render,
											  "text", SONG_TRACK_NUM,
											  NULL);
  render = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
											  -1, _("Title"), render,
											  "text", SONG_TITLE_STR,
											  NULL);
  render = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
											  -1, _("Artist"), render,
											  "text", SONG_ARTIST_STR,
											  NULL);
  render = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
											  -1, _("Album"), render,
											  "text", SONG_ALBUM_STR,
											  NULL);
  render = gtk_cell_renderer_text_new();
  gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(treeview),
											  -1, _("Time"), render,
											  "text", SONG_LENGTH_INT,
											  NULL);

  gtk_tree_view_set_model(GTK_TREE_VIEW(treeview),
						  GTK_TREE_MODEL(icep_liststore.liststore_songs));

  return TRUE;
}

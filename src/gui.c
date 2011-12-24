/*
 * GUI 
 *
 * gui.c
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
#include "gui.h"
#include "common.h"
#include "about.h"

/**
 * SECTION: gui
 * @Short_description: 播放器用户图形界面的实现
 * @Title: 图形界面
 * @Include: gui.h
 *
 * 负责构建播放器的图形界面
 */

static const gchar *module_name = "GUI";
static iceplayer_GuiData_t iceplayer_gui;

/*
 *GtkActionEntry[] iceplayer_mainwindow_menubar:
 *主窗体的MenuBar内容，快捷键以及回调函数
 */

static GtkActionEntry iceplayer_mainwindow_menubar[] = 
  {
	{"MusicMenu", NULL, N_("_Music")},
	{"New_Playlist", NULL, N_("New Playlist"), "<control>N",
	 N_("Create a new playlist"), NULL},
	{"Add_Music", GTK_STOCK_ADD, N_("Import _File..."), NULL,
	 N_("Choose file to be added to the Playlist"), NULL},
	{"Add_Folder", GTK_STOCK_ADD, N_("_Import Folder..."), NULL,
	 N_("Choose folder to be added to the Playlist"), NULL},
	{"AutoScan", NULL, N_("_Scan Removable Media"), NULL,
	 N_("Scan for new Removable Media"), NULL},
	{"Save_Playlist", NULL, N_("_Save to File..."), "<control><alt>S",
	 N_("Save a playlist to a file"), NULL},
	{"Quit", NULL, N_("_Quit"), "<control>q",
	 N_("Quit the iceplayer"), G_CALLBACK(gtk_main_quit)},

	{"EditMenu", NULL, N_("_Edit")},
	{"CutItem", NULL, N_("Cu_t"), "<control>X", N_("Cut selection"), NULL},
	{"CopyItem", NULL, N_("_Copy"), "<control>C", N_("Copy selection"), NULL},
	{"PasteItem", NULL, N_("_Paste"), "<control>V", N_("Paste selection"), NULL},
	{"SelectALL", NULL, N_("Select All"), "<control>A",
	 N_("Select all songs"), NULL},
	{"DeSelectALL", NULL, N_("Deselect All"), NULL,
	 N_("Deselect all songs"), NULL},
	{"Add_to_Playlist", NULL, N_("_Add to Playlist")},
	{"Remove_from_Playlist", NULL, N_("Remove from Playlist"), NULL,
	 N_("Remove each selected song from the playlist"), NULL},
	{"Delete_from_Disk", NULL, N_("Delete from Disk"), NULL,
	 N_("Delete each selected song from the disk"), NULL},

	{"ViewMenu", NULL, N_("_View")},
	{"Mini_Mode", NULL, N_("M_ini Mode"), NULL, 
	 N_("Make the main window smaller"), NULL},
	{"Fullscreen_Mode", NULL, N_("_Fullscreen Mode"), NULL,
	 N_("Make the main window fullscreen"), NULL},

	{"ControlMenu", NULL, N_("_Control")},
	{"Play", NULL, N_("_Play"), NULL, N_("Start playback"), NULL},

	{"SettingsMenu", NULL, N_("Settings")},

	{"HelpMenu", NULL, N_("_Help")},
	{"About", NULL, N_("_About"), "F1",
	 N_("Show program info"), G_CALLBACK(iceplayer_gui_show_about_dialog)}
  };

/*
 *const gchar *iceplayer_gui_menu_xml:
 *菜单结构
 */

static const gchar *iceplayer_gui_menu_xml = 
  "<ui>"
  "  <menubar name='MenuBar'>"
  "	 <menu action='MusicMenu'>"
  "	   <menuitem action='New_Playlist'/>"
  "	   <separator/>"
  "	   <menuitem action='Add_Music'/>"
  "	   <menuitem action='Add_Folder'/>"
  "	   <menuitem action='AutoScan'/>"
  "	   <separator/>"
  "	   <menuitem action='Save_Playlist'/>"
  "	   <separator/>"
  "	   <menuitem action='Quit'/>"
  "	 </menu>"
  "	 <menu action='EditMenu'>"
  "	   <menuitem action='CutItem'/>"
  "	   <menuitem action='CopyItem'/>"
  "	   <menuitem action='PasteItem'/>"
  "	   <separator/>"
  "	   <menuitem action='SelectALL'/>"
  "	   <menuitem action='DeSelectALL'/>"
  "	   <separator/>"
  "	   <menu action='Add_to_Playlist'/>"
  "	   <separator/>"
  "	   <menuitem action='Remove_from_Playlist'/>"
  "	   <menuitem action='Delete_from_Disk'/>"
  "	 </menu>"
  "	 <menu action='ViewMenu'>"
  "	   <menuitem action='Mini_Mode'/>"
  "	   <menuitem action='Fullscreen_Mode'/>"
  "	 </menu>"
  "	 <menu action='ControlMenu'>"
  "	   <menuitem action='Play'/>"
  "	   <separator/>"
  "	 </menu>"
  "	 <menu action='SettingsMenu'>"
  "	 </menu>"
  "	 <menu action='HelpMenu'>"
  "	   <menuitem action='About'/>"
  "	 </menu>"
  "  </menubar>"
  "</ui>";


/**
 * static GUI_InitMainWindowLayout():
 *
 * 主窗体内容整理
 */

static void GUI_InitMainWindowLayout()
{
  print_programming("GUI::MainWindow::layout_init()");

  gtk_window_set_default_size(GTK_WINDOW(iceplayer_gui.window_main), 500, 300);

  GtkWidget *vbox_main = gtk_vbox_new(FALSE, 0);
  GtkWidget *menubar = \
	gtk_ui_manager_get_widget(iceplayer_gui.uimgr_main, "/MenuBar");

  gtk_box_pack_start(GTK_BOX(vbox_main), menubar, FALSE, FALSE, 0);

  gtk_paned_add1(GTK_PANED(iceplayer_gui.hpaned_main),
				 iceplayer_gui.treeview_lists);
  gtk_paned_add2(GTK_PANED(iceplayer_gui.hpaned_main),
				 iceplayer_gui.treeview_songs);
  gtk_paned_set_position(GTK_PANED(iceplayer_gui.hpaned_main), 100);

  gtk_box_pack_start(GTK_BOX(vbox_main), iceplayer_gui.hpaned_main,
					 TRUE, TRUE, 0);
  gtk_box_pack_end(GTK_BOX(vbox_main), iceplayer_gui.statusbar_main,
				   FALSE, FALSE, 0);
  gtk_container_add(GTK_CONTAINER(iceplayer_gui.window_main), vbox_main);
}

/**
 * static GUI_CreateMainWindow():
 *
 * 建立主窗体
 */

static void GUI_CreateMainWindow()
{
  print_programming("GUI::MainWindow::init()");

  GError *error;

  iceplayer_gui.window_main = gtk_window_new(GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title(GTK_WINDOW(iceplayer_gui.window_main),
					   "iceplayer Dev 5.0");

  //从这里开始，全是初始化UIManager的代码
  iceplayer_gui.actiongroup_main = gtk_action_group_new("Actions");
  gtk_action_group_set_translation_domain(iceplayer_gui.actiongroup_main,
										  PROG_NAME);
  gtk_action_group_add_actions(iceplayer_gui.actiongroup_main,
							   iceplayer_mainwindow_menubar,
							   G_N_ELEMENTS(iceplayer_mainwindow_menubar),
							   NULL);
  iceplayer_gui.uimgr_main = gtk_ui_manager_new();
  gtk_ui_manager_insert_action_group(iceplayer_gui.uimgr_main,
									 iceplayer_gui.actiongroup_main, 0);
  if(!gtk_ui_manager_add_ui_from_string(iceplayer_gui.uimgr_main,
										iceplayer_gui_menu_xml, -1,
										&error))
	{
	  print_err(error->message);
	  g_error_free(error);
	  gtk_main_quit();
	}

  gtk_window_add_accel_group(GTK_WINDOW(iceplayer_gui.window_main),
			         gtk_ui_manager_get_accel_group(iceplayer_gui.uimgr_main));
  //到此为止，UIManager完成

  iceplayer_gui.hpaned_main = gtk_hpaned_new();
  iceplayer_gui.treeview_lists = gtk_tree_view_new();
  iceplayer_gui.treeview_songs = gtk_tree_view_new();
  iceplayer_gui.statusbar_main = gtk_statusbar_new();

  GUI_InitMainWindowLayout();
}

/**
 * GUI_fini():
 *
 * 完成最后的清理工作
 *
 * Returns: FALSE表示成功
 */

static gboolean GUI_fini(gpointer data)
{
  print_programming("GUI::quit()");
  return FALSE;
}

/**
 * GUI_init():
 *
 * 初始化图形用户界面(主窗口)，注意：这个函数只应被调用一次
 *
 * Returns: 初始化是否成功
 */

gboolean GUI_init(void)
{
  print_programming("GUI::init()");

  static gboolean gui_is_inited = FALSE;
  GError *error;

  if(gui_is_inited) return FALSE;

  gtk_quit_add(0, GUI_fini, NULL);

  if(!gtk_window_set_default_icon_from_file(ICEPLAYER_ICON, &error))
	{
	  print_err(error->message);
	  g_error_free(error);
	}

  GUI_CreateMainWindow();

  gtk_widget_show_all(iceplayer_gui.window_main);

  return TRUE;
}

/**
 * GUI_get_data_struct():
 *
 * 仅供内部函数使用，这就是说：不要在插件中调用这个函数，一旦调用，后果自负^_^
 *
 * Returns: 到iceplayer_GuiData_t的指针
 */

iceplayer_GuiData_t *GUI_GetData_struct()
{
  return &iceplayer_gui;
}

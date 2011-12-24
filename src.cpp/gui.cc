// -*- c++ -*-
#include "common.h"
#include "gui.h"

using namespace Iceplayer;
using namespace Iceplayer::UI;
using namespace Gtk;

Main_Window::Main_Window()
{
  print_programming("UI::Main_Window::Main_Window()");

  try
	{
	  set_default_icon_from_file(ICEPLAYER_ICON);
	}
  catch(Glib::FileError &e)
	{
	  Common::show_err(e.what() + _("\nMain icon load failed!"));
	}

  this->set_title("iceplayer 5 Dev");
  this->set_size_request(500, 300);

  vbox_main = new VBox();

  mgr = UIManager::create();
  mw_actiongroup = ActionGroup::create("Actions");

  vbox_main->pack_start(*CreateMenuBar(), PACK_SHRINK);
  vbox_main->pack_start(*CreateButtonBar(), PACK_SHRINK);
  vbox_main->pack_start(*CreateMainHPaned());

  statusbar = new Statusbar();
  vbox_main->pack_end(*statusbar, PACK_SHRINK);

  this->add(*vbox_main);
  this->add_accel_group(mgr->get_accel_group());
  this->show_all();
}

Main_Window::~Main_Window()
{
  print_programming("UI::Main_Window::~Main_Window()");

  delete vbox_main;
}

Gtk::MenuBar *Main_Window::CreateMenuBar()
{
  print_programming("UI::Main_Window::CreateMenuBar()");

  try
	{
	  mgr->add_ui_from_file("./src/mw_menubar.xml");
	}
  catch(Glib::FileError &e)
	{
	  Common::show_err(e.what() + 
					   _("\nUI init failed! iceplayer will exit..........."));
	  exit(-1);
	}

  ///Music:
  mw_actiongroup->add(Action::create("MusicMenu", _("_Music")));
  mw_actiongroup->add(Action::create("New_Playlist", _("New Playlist"),
									 _("Create a new playlist")),
					  AccelKey(""), sigc::ptr_fun(&Common::Nonefun));
  mw_actiongroup->add(Action::create("Add_Music", Stock::ADD,
									 _("Import _File..."),
									 _("Choose file to be added "
									   "to the Playlist")));
  mw_actiongroup->add(Action::create("Add_Folder", Stock::ADD,
									 _("_Import Folder..."),
									 _("Choose folder to be added "
									   "to the Playlist")));
  mw_actiongroup->add(Action::create("AutoScan", _("_Scan Removable Media"),
									 _("Scan for new Removable Media")));
  mw_actiongroup->add(Action::create("Save_Playlist", _("_Save to File..."),
									 _("Save a playlist to a file")));
  mw_actiongroup->add(Action::create("Quit", _("_Quit"),
									 _("Quit the iceplayer")),
					  AccelKey("<control>q"),
					  sigc::ptr_fun(&Main::quit));

  ///Edit:
  mw_actiongroup->add(Action::create("EditMenu", _("_Edit")));
  mw_actiongroup->add(Action::create("CutItem", _("Cu_t"), _("Cut selection")));
  mw_actiongroup->add(Action::create("CopyItem", _("_Copy"),
									 _("Copy selection")));
  mw_actiongroup->add(Action::create("PasteItem", _("_Paste"),
									 _("Paste selection")));
  //sep
  mw_actiongroup->add(Action::create("SelectALL", _("Select All"),
									 _("Select all songs")));
  mw_actiongroup->add(Action::create("DeSelectALL", _("Deselect All"),
									 _("Deselect all songs")));
  mw_actiongroup->add(Action::create("Add_to_Playlist", _("_Add to Playlist")));
  mw_actiongroup->add(Action::create("Remove_from_Playlist",
									 _("Remove from Playlist"),
									 _("Remove each selected song"
									   " from the playlist")));
  mw_actiongroup->add(Action::create("Delete_from_Disk", _("Delete from Disk"),
									 _("Delete each selected song"
									   " from the disk")));
  ///View:
  mw_actiongroup->add(Action::create("ViewMenu", _("_View")));
  mw_actiongroup->add(Action::create("Mini_Mode", _("M_ini Mode"),
									 _("Make the main window smaller")));
  mw_actiongroup->add(Action::create("Fullscreen_Mode", _("_Fullscreen Mode"),
									 _("Make the main window fullscreen")));
  ///Control:
  mw_actiongroup->add(Action::create("ControlMenu", _("_Control")));
  mw_actiongroup->add(Action::create("Play", _("_Play"), _("Start playback")));
  ///Settings:
  mw_actiongroup->add(Action::create("SettingsMenu", _("Settings")));

  ///Help:
  mw_actiongroup->add(Action::create("HelpMenu", _("_Help")));
  mw_actiongroup->add(Action::create("About", _("_About"),
									 _("Show program info")),
					  AccelKey("F1"),
					  sigc::mem_fun(*this, &Main_Window::show_aboutdialog));

  mgr->insert_action_group(mw_actiongroup);

  menubar = dynamic_cast<MenuBar *>( mgr->get_widget("/MenuBar") );
  return menubar;
}

Gtk::Toolbar *Main_Window::CreateButtonBar()
{
  print_programming("UI::Main_Window::CreateButtonBar()");

  buttonbar = new Toolbar();

  ToolButton *b_play = new ToolButton(Stock::MEDIA_PLAY);
  buttonbar->append(*b_play);
  ToolButton *b_pause = new ToolButton(Stock::MEDIA_PAUSE);
  buttonbar->append(*b_pause);
  ToolButton *b_previous = new ToolButton(Stock::MEDIA_PREVIOUS);
  buttonbar->append(*b_previous);
  ToolButton *b_next = new ToolButton(Stock::MEDIA_NEXT);
  buttonbar->append(*b_next);

  return buttonbar;
}

Gtk::HPaned *Main_Window::CreateMainHPaned()
{
  print_programming("UI::Main_Window::CreateMainHPaned()");

  hpaned_main = new HPaned();

  ScrolledWindow *sw_list_left = new ScrolledWindow();
  sw_list_left->set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC);
  treeview_list_left = new TreeView();
  sw_list_left->add(*treeview_list_left);
  hpaned_main->pack1(*sw_list_left, true, true);

  ScrolledWindow *sw_list = new ScrolledWindow();
  sw_list->set_policy(POLICY_AUTOMATIC, POLICY_AUTOMATIC);
  treeview_list = new TreeView();
  sw_list->add(*treeview_list);
  hpaned_main->pack2(*sw_list, true, true);

  hpaned_main->set_position(70);

  return hpaned_main;
}

void Main_Window::show_aboutdialog()
{
  print_programming("UI::Main_Window::show_aboutdialog()");

  const gchar *artist[] = {"gjp1120 gjp1120@gmail.com",
						   "HuangJiaCheng jasy.ice@163.com",
						   NULL};
  const gchar *authors[] = {"HuangJiaCheng jasy.ice@163.com",
							"gjp1120 gjp1120@gmail.com",
							"Ekd123 zhtx10@gmail.com",
							"supercatexpert",
							NULL};

  AboutDialog *aw = new AboutDialog();
  aw->set_name(GETTEXT_PACKAGE);
  aw->set_version(Common::version);
  aw->set_license("GPLv3");
  aw->set_artists(artist);
  aw->set_authors(authors);
  aw->set_comments("A Media Player Base On Gstreamer & Gtk From Canton China !"
				   " 2010 GPL");
  aw->set_website("http://www.iceplayer.org");
  aw->set_website_label("www.iceplayer.org  jasy.ice@163.com");

  aw->run();
  delete aw;
}

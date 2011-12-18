#include "common.hxx"
#include "ui.hxx"

using namespace Iceplayer::UI;
using namespace Gtk;

Main_Window::Main_Window()
{
  try
	{
	  set_default_icon_from_file("@datadir@/iceplayer/iceplayer.png");
	}
  catch(Glib::FileError)
	{
	  print_err("can't open icon");
	}

  set_title("iceplayer 5 Dev");

  vbox_main = new VBox();
  menubar = new MenuBar();

  menubar->append(*CreateFileMenu(filemenu = new MenuItem(_("_File"), true)));
  menubar->append(*CreateEditMenu(editmenu = new MenuItem(_("_Edit"), true)));
  menubar->append(*CreateHelpMenu(helpmenu = new MenuItem(_("_Help"), true)));

  vbox_main->pack_start(*menubar, PACK_EXPAND_PADDING);

  vbox_main->pack_start(*CreateMainHPaned());

  statusbar = new Statusbar();
  vbox_main->pack_end(*statusbar, PACK_EXPAND_PADDING);

  add(*vbox_main);

  show_all();
}

Main_Window::~Main_Window()
{
  delete menubar;
  delete vbox_main;
}

Gtk::MenuItem *Main_Window::CreateFileMenu(Gtk::MenuItem *_filemenu)
{
  filemenu_submenu = new Menu();

  MenuItem *mt_new = new MenuItem(_("_New"), true);
  MenuItem *mt_open = new MenuItem(_("_Open"), true);
  MenuItem *mt_close = new MenuItem(_("_Close"), true);

  filemenu_submenu->append(*mt_new);
  filemenu_submenu->append(*mt_open);
  filemenu_submenu->append(*mt_close);

  SeparatorMenuItem *sep1 = new SeparatorMenuItem();
  filemenu_submenu->append(*sep1);

  MenuItem *mt_quit = new MenuItem(_("_Quit"), true);
  mt_quit->signal_activate().connect(sigc::ptr_fun(&Gtk::Main::quit));
  filemenu_submenu->append(*mt_quit);

  _filemenu->set_submenu(*filemenu_submenu);

  return _filemenu;
}

Gtk::MenuItem *Main_Window::CreateEditMenu(Gtk::MenuItem *_editmenu)
{
  editmenu_submenu = new Menu();

  MenuItem *mt_add = new MenuItem(_("_Add"), true);
  MenuItem *mt_del = new MenuItem(_("_Del"), true);

  editmenu_submenu->append(*mt_add);
  editmenu_submenu->append(*mt_del);

  _editmenu->set_submenu(*editmenu_submenu);

  return _editmenu;
}

Gtk::MenuItem *Main_Window::CreateHelpMenu(Gtk::MenuItem *_helpmenu)
{
  helpmenu_submenu = new Menu();

  MenuItem *mt_about = new MenuItem(_("_About"), true);
  mt_about->signal_activate().connect(sigc::mem_fun(*this, &Main_Window::show_aboutdialog));
  helpmenu_submenu->append(*mt_about);

  _helpmenu->set_submenu(*helpmenu_submenu);

  return _helpmenu;
}

Gtk::HPaned *Main_Window::CreateMainHPaned()
{
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
  aw->set_version(version);
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

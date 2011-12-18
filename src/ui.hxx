#include <gtkmm.h>

#include "common.hxx"

#ifndef __UI_HXX
#define __UI_HXX

namespace Iceplayer
{
  namespace UI
  {
	class Main_Window : public Gtk::Window
	{
	public:
	  Main_Window();
	  ~Main_Window();
	  
	private:
	  Gtk::VBox *vbox_main;

	  Gtk::MenuBar *menubar;
	  Gtk::MenuItem *filemenu;
	  Gtk::Menu *filemenu_submenu;
	  Gtk::MenuItem *editmenu;
	  Gtk::Menu *editmenu_submenu;
	  Gtk::MenuItem *helpmenu;
	  Gtk::Menu *helpmenu_submenu;

	  Gtk::HPaned *hpaned_main;
	  Gtk::TreeView *treeview_list_left;
	  Gtk::TreeView *treeview_list;

	  Gtk::Statusbar *statusbar;

	private:
	  Gtk::MenuItem *CreateFileMenu(Gtk::MenuItem *);
	  Gtk::MenuItem *CreateEditMenu(Gtk::MenuItem *);
	  Gtk::MenuItem *CreateHelpMenu(Gtk::MenuItem *);

	  Gtk::HPaned *CreateMainHPaned();

	  void show_aboutdialog();

	protected:
	  inline void connect(gpointer obj, const gchar *detailed_signal,
						  GCallback c_handler, gpointer data)
	  {
		g_signal_connect(gobj(), detailed_signal, c_handler, data);
	  }
	};
  };
};

#endif //__UI_HXX

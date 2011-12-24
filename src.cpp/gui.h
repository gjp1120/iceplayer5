// -*- c++ -*-
#include <gtkmm.h>

#include "common.h"

#ifndef __GUI_H
#define __GUI_H

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

	  Glib::RefPtr<Gtk::UIManager> mgr;
	  Glib::RefPtr<Gtk::ActionGroup> mw_actiongroup;

	  Gtk::MenuBar *menubar;

	  Gtk::Toolbar *buttonbar;

	  Gtk::HPaned *hpaned_main;
	  Gtk::TreeView *treeview_list_left;
	  Gtk::TreeView *treeview_list;

	  Gtk::Statusbar *statusbar;

	private:
	  void CreateActionGroup_MW();
	  Gtk::MenuBar *CreateMenuBar();
	  Gtk::Toolbar *CreateButtonBar();
	  Gtk::HPaned *CreateMainHPaned();
	  void show_aboutdialog();
	};
  };
};

#endif //__GUI_H

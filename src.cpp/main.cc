// -*- c++ -*-
#include "common.h"
#include "gui.h"
#include "core.h"
#include "notify.h"

using namespace Iceplayer;

int main(int argc, char **argv)
{
  print_programming("main()");

  Gtk::Main *app = new Gtk::Main(argc, argv);

  bindtextdomain (GETTEXT_PACKAGE, "@localedir@");
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  try
	{
	  UI::Main_Window *win = new UI::Main_Window();
	  Notify *notify = Notify::create();
	  
	  notify->show(_(GETTEXT_PACKAGE), _("Welcome!"));
	  Gtk::Main::run(*win);
	}
  catch(Glib::Exception &e)
	{
	  print_err(e.what().c_str());
	}
}

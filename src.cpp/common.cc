// -*- c++ -*-
#include "common.h"

const Glib::ustring Iceplayer::Common::version = "5.0dev1";

void Iceplayer::Common::show_err(const Glib::ustring &message)
{
  print_programming("Common::show_err()");

  Gtk::MessageDialog *mw = new Gtk::MessageDialog(message, false,
												  Gtk::MESSAGE_ERROR);
  gint res = mw->run();
  delete mw;
}

void Iceplayer::Common::Nonefun()
{
  print_debug("Common::Nonefun()");

}

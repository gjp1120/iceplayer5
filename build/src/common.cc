// -*- c++ -*-
#include "common.hxx"

const Glib::ustring Iceplayer::version = "5.0dev1";

void Iceplayer::print_err(const Glib::ustring &message)
{
  Gtk::MessageDialog *mw = new Gtk::MessageDialog(message, false,
												  Gtk::MESSAGE_ERROR);
  gint res = mw->run();
  delete mw;
}

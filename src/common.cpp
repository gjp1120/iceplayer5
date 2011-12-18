#include "common.hxx"

std::string version = "5.0dev1";

void print_err(const gchar *message)
{
  Gtk::MessageDialog *mw = new Gtk::MessageDialog(message, false,
												  Gtk::MESSAGE_ERROR);
  gint res = mw->run();
  delete mw;
}

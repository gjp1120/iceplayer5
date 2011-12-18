#include <gtkmm.h>
#include "common.hxx"

#define COMMENTS \
  "A Media Player Base On Gstreamer & Gtk From Canton China ! 2010 GPL"

void show_about_dialog() 
{
  const gchar *authors[] = {"HuangJiaCheng jasy.ice@163.com",
							"Ekd123 zhtx10@gmail.com",
							"supercatexpert",
							"gjp1120 gjp1120@gmail.com",
							NULL };

  Gtk::AboutDialog *about_dialog = new Gtk::AboutDialog();

  about_dialog->set_version(version);
  about_dialog->set_name("iceplayer");
  about_dialog->set_comments(COMMENTS);
  about_dialog->set_website("http://www.iceplayer.org");
  about_dialog->set_license("GPLv3");
  about_dialog->set_website_label("www.iceplayer.org  jasy.ice@163.com");
  about_dialog->set_authors(authors);
  about_dialog->run();

  delete about_dialog;
}

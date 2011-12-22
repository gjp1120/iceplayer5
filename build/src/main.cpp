#include "common.hxx"
#include "ui.hxx"

int main(int argc, char **argv)
{
  Gtk::Main *app = new Gtk::Main(argc, argv);

  bindtextdomain (GETTEXT_PACKAGE, "/usr/share/locale");
  bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
  textdomain (GETTEXT_PACKAGE);

  try
	{
	  Iceplayer::UI::Main_Window *win = new Iceplayer::UI::Main_Window();

	  Gtk::Main::run(*win);
	}
  catch(Glib::Exception &e)
	{
	  std::cerr << e.what() << std::endl;
	}
}

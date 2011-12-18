#include <gtkmm.h>
#include <libintl.h>
#include <glib/gi18n.h>
#include <iostream>

#ifndef __COMMON_HXX
#define __COMMON_HXX

#define GETTEXT_PACKAGE "iceplayer"
#define LOCALEDIR "mo"

extern std::string version;

namespace Iceplayer
{
};

void print_err(const gchar *message);

#endif //__COMMON_HXX

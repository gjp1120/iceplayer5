// -*- c++ -*-
#include <gtkmm.h>
#include <libintl.h>
#include <glib/gi18n.h>
#include <iostream>

#ifndef __COMMON_HXX
#define __COMMON_HXX

#define GETTEXT_PACKAGE "iceplayer"
#define LOCALEDIR "mo"

namespace Iceplayer
{
  extern const Glib::ustring version;

  void print_err(const Glib::ustring &message);
};

#endif //__COMMON_HXX

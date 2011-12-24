// -*- c++ -*-
#include <gtkmm.h>
#include <libintl.h>
#include <glib/gi18n.h>
#include <iostream>

#ifndef __COMMON_H
#define __COMMON_H

#define TTY_FONT_BLUE "\033[34m"
#define TTY_FONT_YELLOW "\033[33m"
#define TTY_FONT_RED "\033[31m"
#define TTY_FONT_BLACK "\033[30m"
#define TTY_FONT_GREEN "\033[32m"

#define GETTEXT_PACKAGE "iceplayer"
#define LOCALEDIR "mo"

#define ICEPLAYER_ICON "@datadir@/iceplayer.png"

#define print_debug(x)													\
  g_print(TTY_FONT_YELLOW "Thread&0x%x "								\
		  TTY_FONT_BLUE"Debug:"											\
		  TTY_FONT_BLACK " %s\n", g_thread_self(), x)
#define print_programming(x) \
  g_print(TTY_FONT_YELLOW "Thread&0x%x"									\
		  TTY_FONT_GREEN " Programming:"								\
		  TTY_FONT_BLACK " %s\n", g_thread_self(), x)
#define print_err(x) \
  g_print(TTY_FONT_YELLOW "Thread&0x%x "								\
		  TTY_FONT_RED "*ERROR*"										\
		  TTY_FONT_BLACK ": %s\n",	g_thread_self(), x);

namespace Iceplayer
{
  namespace Common
  {
	extern const Glib::ustring version;

	void show_err(const Glib::ustring &);
	void Nonefun();
  };
};

#endif //__COMMON_H

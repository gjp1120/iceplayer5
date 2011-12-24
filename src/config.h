/*
 * Config Module
 *
 * config.h
 * This file is part of <iceplayer>
 *
 * Copyright (C) 2010 - gjp1120, license: GPL v3
 *
 * <iceplayer> is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * <iceplayer> is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with <iceplayer>; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include "common.h"

#ifndef __CONFIG_H
#define __CONFIG_H

G_BEGIN_DECLS
;;

gboolean Config_init();

gint Config_getInt(const gchar *);
const gchar *Config_getStr(const gchar *);
gboolean Config_getBool(const gchar *);

gboolean Config_setInt(const gchar *, gint32);
gboolean Config_setStr(const gchar *, const gchar *);
gboolean Config_setBool(const gchar *, gboolean);

G_END_DECLS

#endif //__CONFIG_H

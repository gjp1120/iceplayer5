/*
 * Config
 *
 * config.c
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
#include "config.h"
#include <dconf.h>

/**
 * SECTION: config
 * @Short_description: 配置模块，隔离配置实现
 * @Title: 配置模块
 * @Include: config.h
 *
 * 配置模块
 * 请所有的配置信息都使用这里的函数保存和读取
 */

static const gchar *module_name = "Config";
static DConfClient *client;

static void watchfunc(DConfClient *_client, const gchar *path,
					  const gchar * const * item,
					  gint n_items,
					  const gchar *tag,
					  gpointer data)
{
  print_programming("Config::__watchfunc()");
}

gboolean Config_init()
{
  print_programming("Config::init()");

  client = dconf_client_new(NULL, watchfunc, NULL, NULL);
  if(!client)
	{
	  print_err("Can't init Dconf");
	  return FALSE;
	}

  return TRUE;
}

gint Config_getInt(const gchar *key)
{
  print_programming("Config::getInt()");

  gchar *full_key = g_malloc(STRINGS_LENGTH * sizeof(gchar));

  if(key[1] == '/')
	strcpy(full_key, "/apps/iceplayer");
  else
	strcpy(full_key, "/apps/iceplayer/");

  strcat(full_key, key);

  GVariant *val = dconf_client_read(client, full_key);
  gint32 ret = g_variant_get_int32(val);
  g_variant_unref(val);

  g_free(full_key);
  return ret;
}

const gchar *Config_getStr(const gchar *key)
{
  print_programming("Config::getStr()");

  gchar *full_key = g_malloc(STRINGS_LENGTH * sizeof(gchar));

  if(key[1] == '/')
	strcpy(full_key, "/apps/iceplayer");
  else
	strcpy(full_key, "/apps/iceplayer/");

  strcat(full_key, key);

  GVariant *val = dconf_client_read(client, full_key);
  const gchar *ret = g_variant_get_string(val, NULL);
  g_variant_unref(val);

  print_debug(ret);

  g_free(full_key);
  return ret;
}

gboolean Config_getBool(const gchar *key)
{
  print_programming("Config::getBool()");

  gchar *full_key = g_malloc(STRINGS_LENGTH * sizeof(gchar));

  if(key[1] == '/')
	strcpy(full_key, "/apps/iceplayer");
  else
	strcpy(full_key, "/apps/iceplayer/");

  strcat(full_key, key);
  GVariant *val = dconf_client_read(client, full_key);
  gboolean ret = g_variant_get_boolean(val);
  g_variant_unref(val);

  g_free(full_key);
  return ret;
}

gboolean Config_setBool(const gchar *key, gboolean _val)
{
  print_programming("Config::setBool()");

  GError *err = NULL;
  gboolean ret = TRUE;
  gchar *full_key = g_malloc(STRINGS_LENGTH * sizeof(gchar));

  if(key[1] == '/')
	strcpy(full_key, "/apps/iceplayer");
  else
	strcpy(full_key, "/apps/iceplayer/");

  strcat(full_key, key);

  GVariant *val = g_variant_new_boolean(_val);
  if(!dconf_client_write(client, full_key, val, NULL, NULL, &err))
	{
	  print_err(err->message);
	  ret = FALSE;
	}

  g_free(full_key);
  g_variant_unref(val);
  return ret;
}

gboolean Config_setInt(const gchar *key, gint32 _val)
{
  print_programming("Config::setInt()");

  GError *err = NULL;
  gboolean ret = TRUE;
  gchar *full_key = g_malloc(STRINGS_LENGTH * sizeof(gchar));

  if(key[1] == '/')
	strcpy(full_key, "/apps/iceplayer");
  else
	strcpy(full_key, "/apps/iceplayer/");

  strcat(full_key, key);

  GVariant *val = g_variant_new_int32(_val);
  if(!dconf_client_write(client, full_key, val, NULL, NULL, &err))
	{
	  print_err(err->message);
	  ret = FALSE;
	}

  g_free(full_key);
  g_variant_unref(val);
  return ret;
}

gboolean Config_setStr(const gchar *key, const gchar *_val)
{
  print_programming("Config::setStr()");

  GError *err = NULL;
  gboolean ret = TRUE;
  gchar *full_key = g_malloc(STRINGS_LENGTH * sizeof(gchar));

  if(key[1] == '/')
	strcpy(full_key, "/apps/iceplayer");
  else
	strcpy(full_key, "/apps/iceplayer/");

  strcat(full_key, key);

  GVariant *val = g_variant_new_string(_val);
  if(!dconf_client_write(client, full_key, val, NULL, NULL, &err))
	{
	  print_err(err->message);
	  ret = FALSE;
	}

  g_free(full_key);
  g_variant_unref(val);
  return ret;
}

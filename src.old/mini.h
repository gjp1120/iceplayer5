#include <gtk/gtk.h>

#ifndef __MINI_H
#define __MINI_H

#ifdef __cplusplus
extern "C" {
#endif

gboolean on_mini_expose();
void init_mini();
gchar *get_mini_back();
void updata_mini_image();

#ifdef __cplusplus
};
#endif

#endif //__MINI_H

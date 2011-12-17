#include <gtk/gtk.h>

#ifndef __UPDATE_H
#define __UPDATE_H

void update_iceplayer_check_update();//注意要使用多线程检测更新
void auto_Update(gboolean *quite) ;

#endif //__UPDATE_H

#include "core.h"
#include "common.h"
#include <dlfcn.h>
#include <gtk/gtk.h>

void pl_init()
{
  print_programming("pl_init call up\n");
  
  void *updater = dlopen("@datadir@/iceplayer/pls/libiceplayer_updater.so", RTLD_NOW);
  if(updater)
	{
	  void (*update_func)() = dlsym(updater, "auto_Update");
	  if(update_func)
		g_thread_create_full((GThreadFunc)update_func, NULL,0,FALSE,TRUE,G_THREAD_PRIORITY_LOW,NULL);
	}
  else
	{
	  print_notify("err!", "err:can't load updater");
	}

}


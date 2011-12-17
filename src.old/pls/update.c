#include <gtk/gtk.h>
#include "update.h"
#include "../common.h"
/*
void iceplayer_check_update() { //检查更新
    print_programming("iceplayer_check_update\n");
    if (configure.auto_update)update_iceplayer_check_update();//注意要使用多线程检测更新
}
*/

gboolean dialog2(gchar *word) {
    dialog(word);
    return FALSE;
}

void auto_Update(gboolean *quite) {
    usleep(1000);
    print_debug("自动检查更新。。。");
    if (system("w3m -no-cookie http://jasy.ice.blog.163.com/blog/static/8843073020103851923530/ > /tmp/iceplayer.update"));
    FILE * a = fopen("/tmp/iceplayer.update","r");
    gchar b[5000];
    gchar d[100];
    gchar c[6000];
    strcpy(c,"\n");
    if (!a) {
        if (a) {
            fclose(a);
            a=0;
        }
        unlink("/tmp/iceplayer.update");
        return ;
    } else {
        if (!fgets(b,2000,a)) {
            print_error("无法连接网络以检查更新\n");
            if(quite != NULL)
                dialog2(_("无法连接网络以检查更新"));
            if (a) {
                fclose(a);
                a=0;
            }
            unlink("/tmp/iceplayer.update");
            return ;
        }
    }
    while (fgets(b,2000,a)) {
        if (strstr(b,"最新更新版：")) {
            if (fgets(b,2000,a));
            if(b[strlen(b)-1]=='\n')
                b[strlen(b)-1]='\0';
            if (strcmp(b, verison) < 0) {
                static gchar n[STRINGS_LENGTH*5];
                sprintf(n,_("Debug:检查结果：iceplayer 当前版本为 %s 记录更新为 %s \nDebug:恭喜你，你使用的是友情产品。 ^_^ ～～。 \n"),verison,b);
                //print_notify("检查更新：",n);
                if(quite != NULL)
                    g_timeout_add(10,G_SOURCE_FUNC(dialog2),n);
                g_print(n);
            }
            if (strcmp(b,verison)==0) {
                static gchar n[STRINGS_LENGTH*5];
                sprintf(n,_("检查结果：iceplayer 当前版本为%s 已为最新版本。"),verison);
                if(quite != NULL)
                    g_timeout_add(10,G_SOURCE_FUNC(dialog2),n);
                //print_notify("检查更新：",n);
                g_print(n);

            }
            if (strcmp(b,verison)>0) {
                strcpy(d,b);
                while (fgets(b,2000,a)) {
                    if (strstr(b,"End")) {
                        break;
                    }
                    strcat(c,"\t");
                    strcat(c,b);
                }
                static gchar n[STRINGS_LENGTH*10];
                sprintf(n,_("Debug:检查结果：iceplayer 当前版本为%s 最新版本为%s将进行更新。\n最新特性:\n%s"),verison,d,c);
                //if(quite != NULL)
                g_timeout_add(10,G_SOURCE_FUNC(dialog2),n);
                //print_notify("检查更新：",n);

                g_print(n);
                gtk_show_uri(NULL,"http://www.iceplayer.org", 0,NULL);
            }
            break;
        }
    }
    if (a) {
        fclose(a);
        a=0;
    }
    unlink("/tmp/iceplayer.update");
}

void update_iceplayer_check_update() { //注意要使用多线程检测更新
    print_programming(("update_iceplayer_check_update\n"));
    g_thread_create_full((GThreadFunc)auto_Update, NULL,0,FALSE,TRUE,G_THREAD_PRIORITY_LOW,NULL);
}

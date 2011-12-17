#include "core.h"
#include "keybinding.h"
#include "widget_lrc.h"
#include "single.h"
#include "desktop_lrc.h"
#include "id3.h"
#include "list.h"
#include "ui.h"
#include "configure.h"
#include "instance.h"
#include "common.h"
#include "instance.h"
#include "lrc.h"
#include "mini.h"
#include "equalizer.h"
#include "pl.h"
#include <libnotify/notify.h>

#include <fstream>

//进度条和歌词的刷新循环
gboolean refresh(gpointer)
{
  if (core.state != GST_STATE_PLAYING)
	{
	  static gint cou=1;
	  
	  switch(cou)
		{
		case 3:
		  progress_refresh();
		  break;
		case 5:
		  progress_refresh();
		  break;
		case 7:
		  progress_refresh();
		  break;
		case 9:
		  cou = 1;
		  progress_refresh();
		  lrc_refresh();
		  break;
		}
	  return TRUE;
    }

  progress_refresh();
  lrc_refresh();
  return TRUE;
}

//初始化环境(建立配置目录，初始化配置文件)
void environment_init()
{
  std::string *make_dir = new std::string("mkdir --parents ");

  *make_dir += g_get_home_dir();
  *make_dir += "/.iceplayer/skin";
  system(make_dir->c_str());

  *make_dir = "mkdir --parents ";
  *make_dir += g_get_home_dir();
  *make_dir += "/.iceplayer/songs/";
  system(make_dir->c_str());

  delete make_dir;

  FILE *file = fopen(get_configure_path(),"r");

  if (!file) {
	file = fopen(get_configure_path(),"w");
	fputs("\n[常规]\n[播放]\n[皮肤]\n[歌词]"
		  "\n[窗口歌词]\n[桌面歌词]\n[音乐盒]"
		  "\n[快捷键]\n[迷你模式]\n[信息]\n",file);
  }
  fclose(file);

  file = fopen(get_lists_file_path(),"r");
  if (!file) {
	file = fopen(get_lists_file_path(),"w");
	fputs("\n[☞〄列表〄☜]\n♪=\n",file);
  }
  fclose(file);

  strcpy(focus_lists,"");
  strcpy(playing_list,"");
  strcpy(playing_song,"0");
  strcpy(focus_song,"");
}

//一个特定系统的(针对Ubuntu Natty:11.04)
void if_natty() {
  FILE *file = fopen("/etc/issue","r");
  if(file) {
	gchar *rel = new gchar[STRINGS_LENGTH];
	fgets(rel,1000,file);
	if(strstr(rel,"11.04")) {
	  system("gsettings set com.canonical.Unity.Panel "
			 "systray-whitelist \"['all']\"");
	}
	fclose(file);
	delete []rel;
  }
}

gboolean auto_play(void *) 
{
  print_debug(_("开启即播放"));
  play();
  if (core.pipeline)change_picture_on_press_and_release_event(NULL,
															  CONTROL_PLAY);
  if (!configure.begin_play)play();
  return FALSE;
}

void init_configure_to_iceplayer_after_list() {
    print_debug(_("初始化 some configures"));

    if (!configure.mini) gtk_widget_hide(iceplayer.mini_window);
    else gtk_window_move(GTK_WINDOW(iceplayer.mini_window),
						 configure.mini_x,configure.mini_y);

    if (find_lists_from_name(focus_lists)) {
        strcpy(playing_list,focus_lists);
        show_lists(NULL);
        show_songs(NULL);
        update_focus_song_from_playing_song();
        if (main_lists.songs && main_lists.songs->uri[0] &&
			(configure.begin_play||(!configure.begin_play&&configure.last_pos))
			&& focus_song[0] && playing_song[0])
		  {
            gchar *path = new gchar[STRINGS_LENGTH];
            strcpy(path,focus_song);
            if (!strncmp(path,"<cue>",5)) {
			  //cue:
			  gchar *path2 = new gchar[STRINGS_LENGTH];
			  strcpy(path2,strstr(path,"<cue_uri:"));
			  sscanf(path2,"<cue_uri:%[^>]",path);
			  delete []path2;
            }

            FILE *file = fopen(path,"r");
            if (file) {
                fclose(file);
                g_timeout_add(50,auto_play,NULL);
            } else {
                gchar *te = new gchar[STRINGS_LENGTH];

                sprintf(te,
						_("iceplayer  文件\"%s\"无法读取，"
						  "请重试或检查文件所在目录是否可读？"
						  "相关分区是否已经加载？"),
						path);
                GtkWidget *dialog_real = gtk_message_dialog_new_with_markup(NULL, GTK_DIALOG_DESTROY_WITH_PARENT, GTK_MESSAGE_QUESTION,GTK_BUTTONS_OK, te);
                gtk_dialog_run((GtkDialog *)dialog_real);
                gtk_widget_destroy(dialog_real);
                delete []te;
            }

            delete []path;
            if (configure.last_pos)
			  g_timeout_add(1500,
							jump_to_pos, NULL);
        }
        g_timeout_add(500,
					  list_menu_jump_to_playing_song,NULL);
    }
    if (configure.begin_min)
	  g_timeout_add(50,hide,NULL);
}

//初始化函数
void iceplayer_init(int argc, char *argv[]) {
    print_programming("iceplayer_init\n");
    print_debug("init one-instance");
    instance_init(argv[1]);//单实例

    print_debug("init gtk & gst");

	//这其实是gettext的初始化
    bindtextdomain (GETTEXT_PACKAGE, "@localedir@");
    bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
    textdomain (GETTEXT_PACKAGE);

	//init gtk & gst
    gtk_init (&argc, &argv);
    gst_init (&argc, &argv);

    print_debug("init_env");
    environment_init();
    print_debug(_("初始化配置"));
    if_natty();                    //针对ubuntu natty
    configure_init_variable();     //读取配置中的变量
    print_debug(_("初始化皮肤参数"));
    ui_init_skin();             //读取所选取的皮肤所对应的所有的控件的图片/位置变量
    print_debug(_("初始化界面"));
    ui_init();                   //建立窗体+控件
    print_debug(_("初始化notify"));
    notify_init("iceplayer");    //libnotify初始化，在此之后即可使用桌面通知
    print_notify("iceplayer", _("欢迎使用"));
    print_debug(_("初始化歌词"));
    lrc_init();
    print_debug(_("应用配置"));
    configure_init();
    print_debug(_("读取多播放列表"));
    list_init_lists();
    print_debug(_("初始化迷你模式"));
    init_mini();
    print_debug(_("应用播放类配置"));
    init_configure_to_iceplayer_after_list();
    print_debug(_("初始化全局快捷键"));
    keybinding_init();
    print_debug(_("初始化均衡器"));
    eq_init();
	print_debug(_("初始化插件"));
	pl_init();
    print_debug(_("显示主窗体"));
    gtk_widget_show_all(iceplayer.window);
    change_widget_lrc_ui(NULL,
						 configure.widget_lrc_show_type);
    gtk_notebook_set_current_page(GTK_NOTEBOOK(iceplayer.notebook),
								  configure.show_page);
    g_timeout_add(80,refresh,NULL);
    print_debug(_("初始化进度条"));
    progress_refresh_init();
    print_debug(_("初始化完成"));
}

int main (int argc, char *argv[]) {
  g_print("Welcome! Version=:%s\n",version.c_str());

  iceplayer_init(argc, argv);

  gtk_main();

  return 0;
}

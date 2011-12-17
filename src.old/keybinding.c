#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>
#include <gst/gst.h>
#include <dirent.h>
#include <glib.h>
#include <iconv.h>
#include <gdk/gdk.h>
#include <gdk/gdkwindow.h>
#include <gdk/gdkx.h>
#include <X11/Xlib.h>
#include <glib/gtypes.h>
#include <gtk/gtkaccelgroup.h>
#include <pthread.h>
#include <sys/un.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <libgen.h>

#include <gtk/gtk.h>
#include "core.h"
#include "keybinding.h"
#include "single.h"
#include "list.h"
#include "configure.h"
#include "lrc.h"
#include "common.h"

/**//*   全局快捷键 感谢gnome-do&tomboy&OSD-lyrics*/

gboolean showing = TRUE;

typedef enum {
    EGG_VIRTUAL_SHIFT_MASK    = 1 << 0,
    EGG_VIRTUAL_LOCK_MASK	    = 1 << 1,
    EGG_VIRTUAL_CONTROL_MASK  = 1 << 2,
    EGG_VIRTUAL_ALT_MASK      = 1 << 3,
    EGG_VIRTUAL_MOD2_MASK	    = 1 << 4,
    EGG_VIRTUAL_MOD3_MASK	    = 1 << 5,
    EGG_VIRTUAL_MOD4_MASK	    = 1 << 6,
    EGG_VIRTUAL_MOD5_MASK	    = 1 << 7,
#if 0
    GDK_BUTTON1_MASK  = 1 << 8,
    GDK_BUTTON2_MASK  = 1 << 9,
    GDK_BUTTON3_MASK  = 1 << 10,
    GDK_BUTTON4_MASK  = 1 << 11,
    GDK_BUTTON5_MASK  = 1 << 12,
#endif
    EGG_VIRTUAL_META_MASK = 1 << 24,
    EGG_VIRTUAL_SUPER_MASK = 1 << 25,
    EGG_VIRTUAL_HYPER_MASK = 1 << 26,
    EGG_VIRTUAL_MODE_SWITCH_MASK = 1 << 27,
    EGG_VIRTUAL_NUM_LOCK_MASK = 1 << 28,
    EGG_VIRTUAL_SCROLL_LOCK_MASK = 1 << 29,
    EGG_VIRTUAL_RELEASE_MASK  = 1 << 30,
    EGG_VIRTUAL_MODIFIER_MASK = 0x7f0000ff
} EggVirtualModifierType;
static gboolean egg_accelerator_parse_virtual (const gchar *accelerator,guint *accelerator_key, EggVirtualModifierType *accelerator_mods);
static void egg_keymap_resolve_virtual_modifiers (GdkKeymap *keymap, EggVirtualModifierType  virtual_mods,GdkModifierType *concrete_mods);
typedef void (* OlBindkeyHandler) (char *keystring, gpointer user_data);
static void keybinder_bind (const char *keystring, OlBindkeyHandler  handler,  gpointer user_data);
void keybinder_unbind (const char *keystring, OlBindkeyHandler  handler);
gboolean keybinder_is_modifier (guint keycode);
guint32 ol_keybinder_get_current_event_time (void);
typedef struct _Binding {
    OlBindkeyHandler  handler;
    gpointer              user_data;
    char                 *keystring;
    uint                  keycode;
    uint                  modifiers;
} Binding;
static GSList *bindings = NULL;
static guint num_lock_mask, caps_lock_mask, scroll_lock_mask;
static void grab_ungrab_with_ignorable_modifiers (GdkWindow *rootwin,Binding *binding,gboolean   grab) {
    guint mod_masks [] = { 0, num_lock_mask,caps_lock_mask,scroll_lock_mask,num_lock_mask | caps_lock_mask,num_lock_mask | scroll_lock_mask,caps_lock_mask | scroll_lock_mask,num_lock_mask  | caps_lock_mask | scroll_lock_mask,};
    int i;
    for (i = 0; i < G_N_ELEMENTS (mod_masks); i++) {
        if (grab) {
            XGrabKey (GDK_WINDOW_XDISPLAY (rootwin),binding->keycode,binding->modifiers | mod_masks [i],GDK_WINDOW_XWINDOW (rootwin),False,GrabModeAsync,GrabModeAsync);
        } else {
            XUngrabKey (GDK_WINDOW_XDISPLAY (rootwin),binding->keycode,binding->modifiers | mod_masks [i],GDK_WINDOW_XWINDOW (rootwin));
        }
    }
}
static gboolean do_grab_key (Binding *binding) {
    GdkKeymap *keymap = gdk_keymap_get_default ();
    GdkWindow *rootwin = gdk_get_default_root_window ();
    EggVirtualModifierType virtual_mods = 0;
    guint keysym = 0;
    if (keymap == NULL || rootwin == NULL)
        return FALSE;
    if (!egg_accelerator_parse_virtual (binding->keystring, &keysym,&virtual_mods))
        return FALSE;
    binding->keycode = XKeysymToKeycode (GDK_WINDOW_XDISPLAY (rootwin),keysym);
    if (binding->keycode == 0)
        return FALSE;
    egg_keymap_resolve_virtual_modifiers (keymap,virtual_mods,&binding->modifiers);
    gdk_error_trap_push ();
    grab_ungrab_with_ignorable_modifiers (rootwin,binding,TRUE /* grab */);
    gdk_flush ();
    if (gdk_error_trap_pop ()) {
        g_print ("绑定全局快捷键 '%s' 失败，被人捷足先登了？\n", binding->keystring);

        return FALSE;
    }
    return TRUE;
}
void keybinder_bind (const char *keystring, OlBindkeyHandler  handler, gpointer user_data) {
    Binding *binding;
    gboolean success;
    binding = g_new0 (Binding, 1);
    binding->keystring = g_strdup (keystring);
    binding->handler = handler;
    binding->user_data = user_data;
    success = do_grab_key (binding);
    if (success) {
        bindings = g_slist_prepend (bindings, binding);
    } else {
        g_free_n (binding->keystring);
        g_free_n (binding);
    }
}
static gboolean do_ungrab_key (Binding *binding) {
    GdkWindow *rootwin = gdk_get_default_root_window ();
// g_print ("Removing grab for '%s'\n", binding->keystring);
    grab_ungrab_with_ignorable_modifiers (rootwin, binding,FALSE /* ungrab */);

    return TRUE;
}
void  keybinder_unbind (const char *keystring, OlBindkeyHandler  handler) {
    GSList *iter;

    for (iter = bindings; iter != NULL; iter = iter->next) {
        Binding *binding = (Binding *) iter->data;

        if (strcmp (keystring, binding->keystring) != 0 ||
                handler != binding->handler)
            continue;

        do_ungrab_key (binding);

        bindings = g_slist_remove (bindings, binding);

        g_free_n (binding->keystring);
        g_free_n (binding);
        break;
    }
}
int a_to_b(int a) {
    switch(a) {
    case 'q':
    case 'Q':
        return 24;
    case 'w':
    case 'W':
        return 25;
    case 'e':
    case 'E':
        return 26;
    case 'r':
    case 'R':
        return 27;
    case 't':
    case 'T':
        return 28;
    case 'y':
    case 'Y':
        return 29;
    case 'u':
    case 'U':
        return 30;
    case 'i':
    case 'I':
        return 31;
    case 'O':
    case 'o':
        return 32;
    case 'p':
    case 'P':
        return 33;
    case 'a':
    case 'A':
        return 38;
    case 's':
    case 'S':
        return 39;
    case 'd':
    case 'D':
        return 40;
    case 'F':
    case 'f':
        return 41;
    case 'g':
    case 'G':
        return 42;
    case 'H':
    case 'h':
        return 43;
    case 'j':
    case 'J':
        return 44;
    case 'K':
    case 'k':
        return 45;
    case 'l':
    case 'L':
        return 46;
    case 'Z':
    case 'z':
        return 52;
    case 'x':
    case 'X':
        return 53;
    case 'c':
    case 'C':
        return 54;
    case 'V':
    case 'v':
        return 55;
    case 'B':
    case 'b':
        return 56;
    case 'n':
    case 'N':
        return 57;
    case 'M':
    case 'm':
        return 58;
    }
    return 0;
}
static GdkFilterReturn filter_func (GdkXEvent *gdk_xevent, GdkEvent *event, gpointer data) {
    GdkFilterReturn return_val = GDK_FILTER_CONTINUE;
    XEvent *xevent = (XEvent *) gdk_xevent;
    gint _next = a_to_b(configure.quick_next[(strlen(configure.quick_next)>1?strlen(configure.quick_next):1)-1]);
    gint _play = a_to_b(configure.quick_play_and_pause[(strlen(configure.quick_play_and_pause)>1?strlen(configure.quick_play_and_pause):1)-1]);
    gint _prev = a_to_b(configure.quick_previous[(strlen(configure.quick_previous)>1?strlen(configure.quick_previous):1)-1]);
    gint _show = a_to_b(configure.quick_show_and_hide[(strlen(configure.quick_show_and_hide)>1?strlen(configure.quick_show_and_hide):1)-1]);
    gint _stop = a_to_b(configure.quick_stop[(strlen(configure.quick_stop)>1?strlen(configure.quick_stop):1)-1]);
    gint _quit = a_to_b(configure.quick_quit[(strlen(configure.quick_quit)>1?strlen(configure.quick_quit):1)-1]);
    gint _research_lrc = a_to_b(configure.quick_research_lrc[(strlen(configure.quick_research_lrc)>1?strlen(configure.quick_research_lrc):1)-1]);

    switch (xevent->type) {
    case KeyRelease: {
        if(xevent->xkey.keycode == _next) {
            next();
            break;
        }
        if(xevent->xkey.keycode == _play) {
            if (!main_lists.name[0]) {
                play();
                return return_val;
            }
            play();
            if (core.state == GST_STATE_PLAYING) {
                if (core.pipeline)
                    change_picture_on_press_and_release_event(NULL,CONTROL_PLAY);
                else
                    change_picture_on_press_and_release_event(NULL,CONTROL_PAUSE);
            } else if (core.pipeline)
                change_picture_on_press_and_release_event(NULL,CONTROL_PAUSE);
            break;
        }
        if(xevent->xkey.keycode == _prev) {
            prev();
            break;
        }
        if(xevent->xkey.keycode == _stop) {
            stop();
            break;
        }
        if(xevent->xkey.keycode == _quit) {
            quit();
            break;
        }
        if(xevent->xkey.keycode == _show) {
            //if( windows_state != WINDOWS_STATE_SHOW&&windows_state != WINDOWS_STATE_SHOW_ALL_WORK_PLACES){
            if(!showing) {
                show();
            } else hide(NULL);
            break;
        }
        if(xevent->xkey.keycode == _research_lrc) {
            if (!configure.search_lrc) {
                gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(configure_widget.use_search_lrc_checkbutton),!configure.search_lrc);
                update_configure();
            }
            wait=TRUE;
            search_lrc_pre();
            break;
        }
    }
    }
    return return_val;
}
static void a() {}
void keybinding_init () {
    GdkWindow *rootwin = gdk_get_default_root_window ();
    gdk_window_add_filter (rootwin, filter_func,NULL);
    keybinder_bind (configure.quick_next, a, NULL);
    keybinder_bind ( configure.quick_play_and_pause, a, NULL);
    keybinder_bind (configure.quick_previous, a, NULL);
    keybinder_bind (configure.quick_quit, a, NULL);
    keybinder_bind (configure.quick_show_and_hide, a, NULL);
    keybinder_bind (configure.quick_stop, a, NULL);
    keybinder_bind (configure.quick_research_lrc, a, NULL);
}
enum {
    EGG_MODMAP_ENTRY_SHIFT   = 0,
    EGG_MODMAP_ENTRY_LOCK    = 1,
    EGG_MODMAP_ENTRY_CONTROL = 2,
    EGG_MODMAP_ENTRY_MOD1    = 3,
    EGG_MODMAP_ENTRY_MOD2    = 4,
    EGG_MODMAP_ENTRY_MOD3    = 5,
    EGG_MODMAP_ENTRY_MOD4    = 6,
    EGG_MODMAP_ENTRY_MOD5    = 7,
    EGG_MODMAP_ENTRY_LAST    = 8
};
#define MODMAP_ENTRY_TO_MODIFIER(x) (1 << (x))
typedef struct {
    EggVirtualModifierType mapping[EGG_MODMAP_ENTRY_LAST];
} EggModmap;
static const EggModmap* egg_keymap_get_modmap (GdkKeymap *keymap);
static inline gboolean is_alt (const gchar *string) {
    return ((string[0] == '<') &&
            (string[1] == 'a' || string[1] == 'A') &&
            (string[2] == 'l' || string[2] == 'L') &&
            (string[3] == 't' || string[3] == 'T') &&
            (string[4] == '>'));
}
static inline gboolean is_ctl (const gchar *string) {
    return ((string[0] == '<') &&
            (string[1] == 'c' || string[1] == 'C') &&
            (string[2] == 't' || string[2] == 'T') &&
            (string[3] == 'l' || string[3] == 'L') &&
            (string[4] == '>'));
}
static inline gboolean is_modx (const gchar *string) {
    return ((string[0] == '<') &&
            (string[1] == 'm' || string[1] == 'M') &&
            (string[2] == 'o' || string[2] == 'O') &&
            (string[3] == 'd' || string[3] == 'D') &&
            (string[4] >= '1' && string[4] <= '5') &&
            (string[5] == '>'));
}
static inline gboolean is_ctrl (const gchar *string) {
    return ((string[0] == '<') &&
            (string[1] == 'c' || string[1] == 'C') &&
            (string[2] == 't' || string[2] == 'T') &&
            (string[3] == 'r' || string[3] == 'R') &&
            (string[4] == 'l' || string[4] == 'L') &&
            (string[5] == '>'));
}
static inline gboolean is_shft (const gchar *string) {
    return ((string[0] == '<') &&
            (string[1] == 's' || string[1] == 'S') &&
            (string[2] == 'h' || string[2] == 'H') &&
            (string[3] == 'f' || string[3] == 'F') &&
            (string[4] == 't' || string[4] == 'T') &&
            (string[5] == '>'));
}
static inline gboolean is_shift (const gchar *string) {
    return ((string[0] == '<') &&
            (string[1] == 's' || string[1] == 'S') &&
            (string[2] == 'h' || string[2] == 'H') &&
            (string[3] == 'i' || string[3] == 'I') &&
            (string[4] == 'f' || string[4] == 'F') &&
            (string[5] == 't' || string[5] == 'T') &&
            (string[6] == '>'));
}
static inline gboolean is_control (const gchar *string) {
    return ((string[0] == '<') &&
            (string[1] == 'c' || string[1] == 'C') &&
            (string[2] == 'o' || string[2] == 'O') &&
            (string[3] == 'n' || string[3] == 'N') &&
            (string[4] == 't' || string[4] == 'T') &&
            (string[5] == 'r' || string[5] == 'R') &&
            (string[6] == 'o' || string[6] == 'O') &&
            (string[7] == 'l' || string[7] == 'L') &&
            (string[8] == '>'));
}
static inline gboolean is_release (const gchar *string) {
    return ((string[0] == '<') &&
            (string[1] == 'r' || string[1] == 'R') &&
            (string[2] == 'e' || string[2] == 'E') &&
            (string[3] == 'l' || string[3] == 'L') &&
            (string[4] == 'e' || string[4] == 'E') &&
            (string[5] == 'a' || string[5] == 'A') &&
            (string[6] == 's' || string[6] == 'S') &&
            (string[7] == 'e' || string[7] == 'E') &&
            (string[8] == '>'));
}
static inline gboolean is_meta (const gchar *string) {
    return ((string[0] == '<') &&
            (string[1] == 'm' || string[1] == 'M') &&
            (string[2] == 'e' || string[2] == 'E') &&
            (string[3] == 't' || string[3] == 'T') &&
            (string[4] == 'a' || string[4] == 'A') &&
            (string[5] == '>'));
}
static inline gboolean is_super (const gchar *string) {
    return ((string[0] == '<') &&
            (string[1] == 's' || string[1] == 'S') &&
            (string[2] == 'u' || string[2] == 'U') &&
            (string[3] == 'p' || string[3] == 'P') &&
            (string[4] == 'e' || string[4] == 'E') &&
            (string[5] == 'r' || string[5] == 'R') &&
            (string[6] == '>'));
}
static inline gboolean is_hyper (const gchar *string) {
    return ((string[0] == '<') &&
            (string[1] == 'h' || string[1] == 'H') &&
            (string[2] == 'y' || string[2] == 'Y') &&
            (string[3] == 'p' || string[3] == 'P') &&
            (string[4] == 'e' || string[4] == 'E') &&
            (string[5] == 'r' || string[5] == 'R') &&
            (string[6] == '>'));
}
static gboolean egg_accelerator_parse_virtual (const gchar *accelerator,guint *accelerator_key,EggVirtualModifierType *accelerator_mods) {
    guint keyval;
    GdkModifierType mods;
    gint len;
    gboolean bad_keyval;
    if (accelerator_key)
        *accelerator_key = 0;
    if (accelerator_mods)
        *accelerator_mods = 0;
    g_return_val_if_fail (accelerator != NULL, FALSE);
    bad_keyval = FALSE;
    keyval = 0;
    mods = 0;
    len = strlen (accelerator);
    while (len) {
        if (*accelerator == '<') {
            if (len >= 9 && is_release (accelerator)) {
                accelerator += 9;
                len -= 9;
                mods |= EGG_VIRTUAL_RELEASE_MASK;
            } else if (len >= 9 && is_control (accelerator)) {
                accelerator += 9;
                len -= 9;
                mods |= EGG_VIRTUAL_CONTROL_MASK;
            } else if (len >= 7 && is_shift (accelerator)) {
                accelerator += 7;
                len -= 7;
                mods |= EGG_VIRTUAL_SHIFT_MASK;
            } else if (len >= 6 && is_shft (accelerator)) {
                accelerator += 6;
                len -= 6;
                mods |= EGG_VIRTUAL_SHIFT_MASK;
            } else if (len >= 6 && is_ctrl (accelerator)) {
                accelerator += 6;
                len -= 6;
                mods |= EGG_VIRTUAL_CONTROL_MASK;
            } else if (len >= 6 && is_modx (accelerator)) {
                static const guint mod_vals[] = {
                    EGG_VIRTUAL_ALT_MASK, EGG_VIRTUAL_MOD2_MASK, EGG_VIRTUAL_MOD3_MASK,
                    EGG_VIRTUAL_MOD4_MASK, EGG_VIRTUAL_MOD5_MASK
                };
                len -= 6;
                accelerator += 4;
                mods |= mod_vals[*accelerator - '1'];
                accelerator += 2;
            } else if (len >= 5 && is_ctl (accelerator)) {
                accelerator += 5;
                len -= 5;
                mods |= EGG_VIRTUAL_CONTROL_MASK;
            } else if (len >= 5 && is_alt (accelerator)) {
                accelerator += 5;
                len -= 5;
                mods |= EGG_VIRTUAL_ALT_MASK;
            } else if (len >= 6 && is_meta (accelerator)) {
                len -= 6;
                mods |= EGG_VIRTUAL_META_MASK;
            } else if (len >= 7 && is_hyper (accelerator)) {
                accelerator += 7;
                len -= 7;
                mods |= EGG_VIRTUAL_HYPER_MASK;
            } else if (len >= 7 && is_super (accelerator)) {
                accelerator += 7;
                len -= 7;
                mods |= EGG_VIRTUAL_SUPER_MASK;
            } else {
                gchar last_ch;
                last_ch = *accelerator;
                while (last_ch && last_ch != '>') {
                    last_ch = *accelerator;
                    accelerator += 1;
                    len -= 1;
                }
            }
        } else {
            keyval = gdk_keyval_from_name (accelerator);
            if (keyval == 0)
                bad_keyval = TRUE;
            accelerator += len;
            len -= len;
        }
    }
    if (accelerator_key)
        *accelerator_key = gdk_keyval_to_lower (keyval);
    if (accelerator_mods)
        *accelerator_mods = mods;
    return !bad_keyval;
}
gchar* egg_virtual_accelerator_name (guint  accelerator_key,EggVirtualModifierType accelerator_mods) {
    static const gchar text_release[] = "<Release>";
    static const gchar text_shift[] = "<Shift>";
    static const gchar text_control[] = "<Control>";
    static const gchar text_mod1[] = "<Alt>";
    static const gchar text_mod2[] = "<Mod2>";
    static const gchar text_mod3[] = "<Mod3>";
    static const gchar text_mod4[] = "<Mod4>";
    static const gchar text_mod5[] = "<Mod5>";
    static const gchar text_meta[] = "<Meta>";
    static const gchar text_super[] = "<Super>";
    static const gchar text_hyper[] = "<Hyper>";
    guint l;
    gchar *keyval_name;
    gchar *accelerator;
    accelerator_mods &= EGG_VIRTUAL_MODIFIER_MASK;
    keyval_name = gdk_keyval_name (gdk_keyval_to_lower (accelerator_key));
    if (!keyval_name)
        keyval_name = "";
    l = 0;
    if (accelerator_mods & EGG_VIRTUAL_RELEASE_MASK)
        l += sizeof (text_release) - 1;
    if (accelerator_mods & EGG_VIRTUAL_SHIFT_MASK)
        l += sizeof (text_shift) - 1;
    if (accelerator_mods & EGG_VIRTUAL_CONTROL_MASK)
        l += sizeof (text_control) - 1;
    if (accelerator_mods & EGG_VIRTUAL_ALT_MASK)
        l += sizeof (text_mod1) - 1;
    if (accelerator_mods & EGG_VIRTUAL_MOD2_MASK)
        l += sizeof (text_mod2) - 1;
    if (accelerator_mods & EGG_VIRTUAL_MOD3_MASK)
        l += sizeof (text_mod3) - 1;
    if (accelerator_mods & EGG_VIRTUAL_MOD4_MASK)
        l += sizeof (text_mod4) - 1;
    if (accelerator_mods & EGG_VIRTUAL_MOD5_MASK)
        l += sizeof (text_mod5) - 1;
    if (accelerator_mods & EGG_VIRTUAL_META_MASK)
        l += sizeof (text_meta) - 1;
    if (accelerator_mods & EGG_VIRTUAL_HYPER_MASK)
        l += sizeof (text_hyper) - 1;
    if (accelerator_mods & EGG_VIRTUAL_SUPER_MASK)
        l += sizeof (text_super) - 1;
    l += strlen (keyval_name);
    accelerator = g_new (gchar, l + 1);
    l = 0;
    accelerator[l] = 0;
    if (accelerator_mods & EGG_VIRTUAL_RELEASE_MASK) {
        strcpy (accelerator + l, text_release);
        l += sizeof (text_release) - 1;
    }
    if (accelerator_mods & EGG_VIRTUAL_SHIFT_MASK) {
        strcpy (accelerator + l, text_shift);
        l += sizeof (text_shift) - 1;
    }
    if (accelerator_mods & EGG_VIRTUAL_CONTROL_MASK) {
        strcpy (accelerator + l, text_control);
        l += sizeof (text_control) - 1;
    }
    if (accelerator_mods & EGG_VIRTUAL_ALT_MASK) {
        strcpy (accelerator + l, text_mod1);
        l += sizeof (text_mod1) - 1;
    }
    if (accelerator_mods & EGG_VIRTUAL_MOD2_MASK) {
        strcpy (accelerator + l, text_mod2);
        l += sizeof (text_mod2) - 1;
    }
    if (accelerator_mods & EGG_VIRTUAL_MOD3_MASK) {
        strcpy (accelerator + l, text_mod3);
        l += sizeof (text_mod3) - 1;
    }
    if (accelerator_mods & EGG_VIRTUAL_MOD4_MASK) {
        strcpy (accelerator + l, text_mod4);
        l += sizeof (text_mod4) - 1;
    }
    if (accelerator_mods & EGG_VIRTUAL_MOD5_MASK) {
        strcpy (accelerator + l, text_mod5);
        l += sizeof (text_mod5) - 1;
    }
    if (accelerator_mods & EGG_VIRTUAL_META_MASK) {
        strcpy (accelerator + l, text_meta);
        l += sizeof (text_meta) - 1;
    }
    if (accelerator_mods & EGG_VIRTUAL_HYPER_MASK) {
        strcpy (accelerator + l, text_hyper);
        l += sizeof (text_hyper) - 1;
    }
    if (accelerator_mods & EGG_VIRTUAL_SUPER_MASK) {
        strcpy (accelerator + l, text_super);
        l += sizeof (text_super) - 1;
    }
    strcpy (accelerator + l, keyval_name);
    return accelerator;
}
static void egg_keymap_resolve_virtual_modifiers (GdkKeymap *keymap,EggVirtualModifierType  virtual_mods,GdkModifierType *concrete_mods) {
    GdkModifierType concrete;
    int i;
    const EggModmap *modmap;
    g_return_if_fail (GDK_IS_KEYMAP (keymap));
    g_return_if_fail (concrete_mods != NULL);
    modmap = egg_keymap_get_modmap (keymap);
    /* Not so sure about this algorithm. */
    concrete = 0;
    i = 0;
    while (i < EGG_MODMAP_ENTRY_LAST) {
        if (modmap->mapping[i] & virtual_mods)
            concrete |= (1 << i);
        ++i;
    }
    *concrete_mods = concrete;
}
void egg_keymap_virtualize_modifiers (GdkKeymap *keymap,GdkModifierType concrete_mods,EggVirtualModifierType *virtual_mods) {
    GdkModifierType virtual;
    int i;
    const EggModmap *modmap;
    g_return_if_fail (GDK_IS_KEYMAP (keymap));
    g_return_if_fail (virtual_mods != NULL);
    modmap = egg_keymap_get_modmap (keymap);
    /* Not so sure about this algorithm. */
    virtual = 0;
    i = 0;
    while (i < EGG_MODMAP_ENTRY_LAST) {
        if ((1 << i) & concrete_mods) {
            EggVirtualModifierType cleaned;
            cleaned = modmap->mapping[i] & ~(EGG_VIRTUAL_MOD2_MASK |
                                             EGG_VIRTUAL_MOD3_MASK |
                                             EGG_VIRTUAL_MOD4_MASK |
                                             EGG_VIRTUAL_MOD5_MASK);
            if (cleaned != 0) {
                virtual |= cleaned;
            } else {
                /* Rather than dropping mod2->mod5 if not bound,
                 * go ahead and use the concrete names
                 */
                virtual |= modmap->mapping[i];
            }
        }
        ++i;
    }
    *virtual_mods = virtual;
}
static void reload_modmap (GdkKeymap *keymap,EggModmap *modmap) {
    XModifierKeymap *xmodmap;
    int map_size;
    int i;
    /* FIXME multihead */
    xmodmap = XGetModifierMapping (gdk_x11_get_default_xdisplay ());
    memset (modmap->mapping, 0, sizeof (modmap->mapping));
    /* there are 8 modifiers, and the first 3 are shift, shift lock,
     * and control
     */
    map_size = 8 * xmodmap->max_keypermod;
    i = 3 * xmodmap->max_keypermod;
    while (i < map_size) {
        /* get the key code at this point in the map,
         * see if its keysym is one we're interested in
         */
        int keycode = xmodmap->modifiermap[i];
        GdkKeymapKey *keys;
        guint *keyvals;
        int n_entries;
        int j;
        EggVirtualModifierType mask;
        keys = NULL;
        keyvals = NULL;
        n_entries = 0;
        gdk_keymap_get_entries_for_keycode (keymap,
                                            keycode,
                                            &keys, &keyvals, &n_entries);
        mask = 0;
        j = 0;
        while (j < n_entries) {
            if (keyvals[j] == GDK_Num_Lock)
                mask |= EGG_VIRTUAL_NUM_LOCK_MASK;
            else if (keyvals[j] == GDK_Scroll_Lock)
                mask |= EGG_VIRTUAL_SCROLL_LOCK_MASK;
            else if (keyvals[j] == GDK_Meta_L ||
                     keyvals[j] == GDK_Meta_R)
                mask |= EGG_VIRTUAL_META_MASK;
            else if (keyvals[j] == GDK_Hyper_L ||
                     keyvals[j] == GDK_Hyper_R)
                mask |= EGG_VIRTUAL_HYPER_MASK;
            else if (keyvals[j] == GDK_Super_L ||
                     keyvals[j] == GDK_Super_R)
                mask |= EGG_VIRTUAL_SUPER_MASK;
            else if (keyvals[j] == GDK_Mode_switch)
                mask |= EGG_VIRTUAL_MODE_SWITCH_MASK;
            ++j;
        }
        /* Mod1Mask is 1 << 3 for example, i.e. the
         * fourth modifier, i / keyspermod is the modifier
         * index
         */
        modmap->mapping[i/xmodmap->max_keypermod] |= mask;
        g_free_n (keyvals);
        g_free_n (keys);
        ++i;
    }
    /* Add in the not-really-virtual fixed entries */
    modmap->mapping[EGG_MODMAP_ENTRY_SHIFT] |= EGG_VIRTUAL_SHIFT_MASK;
    modmap->mapping[EGG_MODMAP_ENTRY_CONTROL] |= EGG_VIRTUAL_CONTROL_MASK;
    modmap->mapping[EGG_MODMAP_ENTRY_LOCK] |= EGG_VIRTUAL_LOCK_MASK;
    modmap->mapping[EGG_MODMAP_ENTRY_MOD1] |= EGG_VIRTUAL_ALT_MASK;
    modmap->mapping[EGG_MODMAP_ENTRY_MOD2] |= EGG_VIRTUAL_MOD2_MASK;
    modmap->mapping[EGG_MODMAP_ENTRY_MOD3] |= EGG_VIRTUAL_MOD3_MASK;
    modmap->mapping[EGG_MODMAP_ENTRY_MOD4] |= EGG_VIRTUAL_MOD4_MASK;
    modmap->mapping[EGG_MODMAP_ENTRY_MOD5] |= EGG_VIRTUAL_MOD5_MASK;
    XFreeModifiermap (xmodmap);
}
static const EggModmap* egg_keymap_get_modmap (GdkKeymap *keymap) {
    EggModmap *modmap;
    /* This is all a hack, much simpler when we can just
     * modify GDK directly.
     */
    modmap = g_object_get_data (G_OBJECT (keymap),
                                "egg-modmap");
    if (modmap == NULL) {
        modmap = g_new0 (EggModmap, 1);
        /* FIXME modify keymap change events with an event filter
         * and force a reload if we get one
         */
        reload_modmap (keymap, modmap);
        g_object_set_data_full (G_OBJECT (keymap),
                                "egg-modmap",
                                modmap,
                                g_free_n);
    }
    g_assert (modmap != NULL);
    return modmap;
}

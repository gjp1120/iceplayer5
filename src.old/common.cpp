#include "common.h"
#include "configure.h"
#include "ui.h"
#include <libnotify/notify.h>

std::string version = "4.05-20110520";

gchar verison[STRINGS_LENGTH]="4.05-20110520";
NotifyNotification *notify;

static gboolean g_free_char(gpointer a)
{
  print_programming("g_free_char\n");
  g_free_n(a);
  return FALSE;
}

static gboolean g_free_str(gpointer a)
{
  print_programming("g_free_str\n");
  delete reinterpret_cast<std::string *>(a);
  return FALSE;
}

GtkWidget *gtk_image_new_from_file_new(const char *dir,const char *picture)
{
    print_programming("gtk_image_new_from_file_new\n");
    if (picture == NULL)
        return NULL;
	std::string *str = new std::string(dir);

	*str += picture;
	*str += ".png";
    g_timeout_add(10, g_free_str, str);
    return gtk_image_new_from_file(str->c_str());
}

GdkPixbuf *gdk_pixbuf_new_from_file_new(const char *dir,const char *picture)
{
    print_programming(("gdk_pixbuf_new_from_file_new\n"));
    if (picture == NULL)
        return NULL;
	std::string *str = new std::string(dir);

	*str += picture;
	*str += ".png";
    g_timeout_add(10, g_free_str, str);
	return gdk_pixbuf_new_from_file(str->c_str(), NULL);
}

void print_error (const char *format, ...) {
    gchar * buf= (gchar *)g_malloc(STRINGS_LENGTH*sizeof(gchar *));
    va_list msg;

    va_start(msg, format);
    vsprintf(buf, format, msg);
    va_end(msg);

    g_print("Error:%s", buf);
    g_free_n(buf);
}

void gtk_fixed_move_new(GtkWidget *fixed,GtkWidget *widget,gint x,gint y) {
    print_programming(("gtk_fixed_move_new\n"));
    if (widget->parent)
      gtk_fixed_move(GTK_FIXED(fixed), widget,
		       x >= 0 ? x : configure.width - 4 + x,
		       y >= 0 ? y : skin.main_height + y);
    else
      gtk_fixed_put(GTK_FIXED(fixed),widget,
		    x >= 0 ? x : configure.width - 4 + x,
		    y >= 0 ? y : skin.main_height + y);
}

void gtk_container_remove_new (GtkContainer *container ,GtkWidget *widget) {
    print_programming(("gtk_container_remove_new\n"));
    if (GTK_IS_WIDGET(container) &&
	GTK_IS_WIDGET(widget) &&
	widget -> parent == GTK_WIDGET (container))
        gtk_container_remove(container,widget);
}

gchar *g_key_file_get_string_new(GKeyFile *key_file,
								 const gchar *group_name,
								 const gchar *key,
								 GError **error)
{
    print_programming(("g_key_file_get_string_new\n"));
    static gchar strings[STRINGS_LENGTH];
    gchar *st = NULL;
    st = g_key_file_get_string(key_file,group_name,key,error);
    if(st!= NULL) {
      strcpy(strings,st);
    } else {
      //fixme:这样做好像没有价值
      error = NULL;
      strings[0]=' ';
      strings[1]='\0';
    }
    return strings;
}

void print_notify(const gchar *title,const gchar *text) {
  print_programming(("print_notify\n"));
  if (configure.notify) {
	if (notify==NULL) {
	  notify = notify_notification_new(title, text, ICON
//patch by Ekd123 20110404
#ifdef NOTIFY_CHECK_VERSION
#if NOTIFY_CHECK_VERSION(0,7,0)
									   );
#else
	                                   , NULL);
#endif
#else
	                                   , NULL);
#endif
//patch end
} else {
  notify_notification_update(notify, title, text, ICON);
 }
notify_notification_show(notify, NULL);
}
}

GdkPixbuf *gdk_pixbuf_set_alpha(GdkPixbuf *src, gint alpha) {
    print_programming(("gdk_pixbuf_set_alpha\n"));

    GdkPixbuf *dest=NULL;
    gint w, h;

    g_return_val_if_fail(src != NULL, NULL);
    g_return_val_if_fail(alpha >=0 && alpha <= 255, NULL);

    w = gdk_pixbuf_get_width(src);
    h = gdk_pixbuf_get_height(src);

    dest = gdk_pixbuf_new(GDK_COLORSPACE_RGB, TRUE, 8, w, h);
    gdk_pixbuf_fill(dest, 0x00000000);

    gdk_pixbuf_composite(src, dest, 0, 0, w, h, 0, 0, 1, 1, GDK_INTERP_BILINEAR,
						 alpha);

    g_object_unref(src);
    return dest;
}

gboolean g_free_char_cairo_surface_t(gpointer h)
{
    print_programming("g_free_char_cairo_surface_t\n");
    cairo_surface_destroy(static_cast<cairo_surface_t *>(h));
    return FALSE;
}

cairo_surface_t *cairo_image_surface_create_from_png_new(cairo_surface_t *s,
														 gchar *uri)
{
    print_programming("cairo_image_surface_create_from_png_new\n");
    if(s != NULL)
      g_timeout_add(25, g_free_char_cairo_surface_t, s);

    return cairo_image_surface_create_from_png(uri);
}

void g_free_n(gpointer er) {
    print_programming("g_free_n\n");
    if(er)g_free(er);
}

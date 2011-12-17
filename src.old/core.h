#include <gst/gst.h>

#ifndef __CORE_H
#define __CORE_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct iceplayer_core{
    GstElement *pipeline;
    GstElement *vol;
    GstState state;
    GstFormat fmt;
    gint64 pos;
    gint64 length;
    GstElement * equalizer;
}iceplayer_core;

extern iceplayer_core core;

gboolean bus_call(GstBus *bus, GstMessage *msg, gpointer user_data);
void play_uri();
gboolean prev ();
gboolean play ();
gboolean next ();
gboolean stop ();
gboolean add ();
gboolean set_volume(gdouble value);
void core_set_eq_effect(gdouble *fq);
void switch_songs();

#ifdef __cplusplus
};
#endif

#endif //__CORE_H

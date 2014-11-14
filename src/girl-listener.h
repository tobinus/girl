/* $Id: girl-station.h,v 1.3 2002/10/04 20:20:30 oka Exp $ */

#ifndef GIRL_LISTENER_H
#define GIRL_LISTENER_H

#include "girl.h"

typedef struct _GirlListenerInfo GirlListenerInfo;
typedef struct _GirlLocationInfo GirlLocationInfo;

struct _GirlListenerInfo {
	gchar *id;
	gchar *location;
	gchar *mail;
	gchar *name;
	gchar *pass;
	gchar *uri;
	GirlLocationInfo *locationinfo;
	GirlListenerInfo *next;
};

struct _GirlLocationInfo {
	gchar *name;
	gchar *link;
	gchar *glat;
	gchar *glon;
	gchar *grad;
	gchar *vote;
	gchar *rack;
};

GirlListenerInfo *girl_listener_load_from_file(GirlListenerInfo * head,
					       char *filename);
GirlListenerInfo *girl_listener_load_from_http(GirlListenerInfo * head,
					       gpointer data);

#endif				/* GIRL_LISTENER_H */

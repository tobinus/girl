/* $Id$
 *
 * GNOME Internet Radio Locator
 *
 * Copyright (C) 2014  Ole Aamot Software
 *
 * Author: Ole Aamot <oka@oka.no>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef GIRL_STATION_H
#define GIRL_STATION_H

#include "girl.h"

typedef struct _GirlStationInfo GirlStationInfo;
typedef struct _GirlStreamInfo GirlStreamInfo;

typedef enum {
	GIRL_CHANNELS_MONO = 0x0001,
	GIRL_CHANNELS_STEREO = 0x0002,
	GIRL_CHANNELS_5_1 = 0x0005,
	GIRL_CHANNELS_NONE = 0xffff
} GirlChannels;

typedef enum {
	GIRL_STREAM_SHOUTCAST = 0x0001,
	GIRL_STREAM_OGG = 0x0002
} GirlStreamType;

struct _GirlStationInfo {
	GtkWidget *widget;
	gchar *id;
	gchar *name;
	gchar *frequency;
	gchar *location;
	gchar *description;
	gchar *bitrate;
	gchar *samplerate;
	gchar *uri;
	/*   gchar *category; */
	GirlStreamInfo *stream;
	GirlStationInfo *next;
	GirlStationInfo *prev;
};

struct _GirlStreamInfo {
	gchar *id;
	gchar *name;
	GirlStreamInfo *next;
	gchar *mimetype;
	glong bitrate;
	glong samplerate;
	GirlChannels channels;
	gchar *uri;
};

void show_error(gchar * msg);
void girl_stream_player(GtkWidget * widget, gpointer data);
void girl_station_free(GirlStationInfo * info);
GirlStationInfo *girl_station_load_from_file(GirlStationInfo * head,
					     char *filename);
GirlStationInfo *girl_station_load_from_http(GirlStationInfo * head,
					     gpointer data);
void girl_launch_helper(char *url, GirlStreamType type);

#endif				/* GIRL_STATION_H */

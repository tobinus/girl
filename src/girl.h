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

#ifndef GIRL_H
#define GIRL_H

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

typedef enum {
	GIRL_STREAM_PLAYER = 0x0001,
	GIRL_STREAM_RECORD = 0x0002
} GirlHelperType;

#include <gnome.h>
#include <libgnomevfs/gnome-vfs.h>
#include "girl-listener.h"
#include "girl-program.h"
#include "girl-runners.h"
#include "girl-station.h"

#ifdef DEBUG
#define MSG(x...) g_message(x)
#else
#define MSG(x...)
#endif

void show_error(gchar * msg);
void appbar_send_msg(const char *a, ...);

/* GnomeVFSFileSize get_size(GnomeVFSURI *uri); */

/* GnomeVFSURI *xfer_stations(gchar *src, gchar *dest); */

gchar *copy_to_mem(GnomeVFSURI * uri, GnomeVFSFileSize len);

/* Fetcher for the stations */

void girl_helper_run(char *url, char *name, GirlStreamType type, GirlHelperType girl);
void on_listen_button_clicked(GtkWidget * button, gpointer user_data);
void on_record_button_clicked(GtkWidget * button, gpointer user_data);
void on_next_click(GtkWidget *, gpointer user_data);
void on_previous_click(GtkWidget *, gpointer user_data);
void on_listeners_selector_button_clicked(GtkWidget *, gpointer user_data);
void on_listeners_selector_changed(GtkWidget * a, gpointer user_data);
void on_programs_selector_button_clicked(GtkWidget *, gpointer user_data);
void on_programs_selector_changed(GtkWidget * a, gpointer user_data);
void on_stations_selector_button_clicked(GtkWidget *, gpointer user_data);
void on_stations_selector_changed(GtkWidget * a, gpointer user_data);
void quit_app(GtkWidget *, gpointer user_data);
void about_app(GtkWidget *, gpointer user_data);
void about_listener(GtkWidget *, gpointer user_data);
void about_program(GtkWidget *, gpointer user_data);
void about_station(GtkWidget *, gpointer user_data);

struct _GirlData {
	GtkImage *pixmap;
	GnomeAppBar *appbar;
	GirlListenerInfo *selected_listener;
	gchar *selected_listener_uri;
	gchar *selected_listener_name;
	gchar *selected_listener_location;
	gchar *selected_listener_release;
	gchar *selected_listener_description;
	GirlProgramInfo *selected_program;
	gchar *selected_program_uri;
	gchar *selected_program_name;
	gchar *selected_program_location;
	gchar *selected_program_release;
	gchar *selected_program_description;
	GirlStationInfo *selected_station;
	gchar *selected_station_uri;
	gchar *selected_station_name;
	gchar *selected_station_location;
	gchar *selected_station_release;
	gchar *selected_station_description;
	gint selected_bitrate;
	GirlChannels selected_channels;
	gint selected_samplerate;
	GdkPixbuf *icon;
};

typedef struct _GirlData GirlData;

extern GirlData *girl;
extern GList *girl_stations;
extern GList *girl_programs;
extern GList *girl_listeners;

#endif /* GIRL_H */

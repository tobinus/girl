/* $Id: girl.c,v 1.4 2002/10/04 20:20:30 oka Exp $
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

#include <config.h>

#include <gnome.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-application-registry.h>

#include <stdlib.h>
#include <string.h>

#include "girl.h"
#include "girl-gui.h"
#include "girl-listener.h"
#include "girl-station.h"

#ifdef DEBUG
#define MSG(x...) g_message(x)
#else
#define MSG(x...)
#endif

GirlData *girl;

GList *girl_listeners;
GList *girl_programs;
GList *girl_stations;

GtkWidget *girl_app;
GtkWidget *listeners_selector = NULL;
GtkWidget *programs_selector = NULL;
GtkWidget *stations_selector = NULL;

int main(int argc, char *argv[])
{
	gchar *iconname;

#ifdef GETTEXT_PACKAGE
	bindtextdomain(GETTEXT_PACKAGE, GNOMELOCALEDIR);
	bind_textdomain_codeset(GETTEXT_PACKAGE, "UTF-8");
	textdomain(GETTEXT_PACKAGE);
#endif

	if (gnome_vfs_init() == FALSE) {
		g_warning("Gaaaaaaaah! Gnome-VFS init failed mate !\n");
		exit(1);
	}

	gnome_program_init(PACKAGE, VERSION,
			   LIBGNOMEUI_MODULE,
			   argc, argv,
			   GNOME_PARAM_APP_DATADIR, DATADIR, NULL);

	girl_app = create_girl_app();

	gtk_widget_show(girl_app);

	/* Set up the listeners list */
	listeners_selector = create_listeners_selector(girl->selected_listener_uri,"listeners.xml");

	g_object_add_weak_pointer(G_OBJECT(listeners_selector),
				  (void **) &(listeners_selector));

	/* Set up the programs list */
	programs_selector = create_programs_selector(girl->selected_program_uri,"programs.xml");

	/* gtk_widget_show(programs_selector); */

        g_object_add_weak_pointer(G_OBJECT(programs_selector),
				  (void **) &(programs_selector));

	/* Set up the stations list */
	stations_selector = create_stations_selector(girl->selected_station_uri,
						     "stations.xml");

	/* gtk_widget_show(stations_selector); */

	g_object_add_weak_pointer(G_OBJECT(stations_selector),
				  (void **) &(stations_selector));

	/* Icons */
	iconname = gnome_program_locate_file(NULL,
					     GNOME_FILE_DOMAIN_APP_PIXMAP,
					     "girl/icons/hicolor/48x48/apps/girl.png", TRUE, NULL);
	if (iconname != NULL) {
		girl->icon = gdk_pixbuf_new_from_file(iconname, NULL);
		g_free(iconname);
	} else {
		girl->icon = NULL;
	}

	if (girl->icon != NULL)
		gtk_window_set_icon(GTK_WINDOW(girl_app), girl->icon);

	gtk_main();
	return 0;
}

void
toggle_in_area (double x1, double y1, double x2, double y2)
{
	double nx1,nx2,ny1,ny2;
	
}

void appbar_send_msg(const char *a, ...)
{
	gchar foo[2048];
	va_list ap;
	va_start(ap, a);

	vsnprintf(foo, 2000, a, ap);

	gnome_appbar_pop(girl->appbar);
	gnome_appbar_push(girl->appbar, foo);

	MSG("%s", foo);

	/* lets update the statusbar widget */
	while (gtk_events_pending())
		gtk_main_iteration();

	va_end(ap);
}

static void cause_movement(int way)
{

}

void on_previous_click(GtkWidget * a, gpointer user_data)
{
	GList *l = g_list_previous(girl_stations);
	if (l != NULL) {
		GirlStationInfo *station = l->data;
		if (station->prev != NULL) {
			girl_stations->data = station->prev;
		} else {
			girl_stations->data = station;
		}

		cause_movement(-1);
		if (station != NULL) {
			printf("Previous Station ID: %s\n", station->id);
			printf("Previous Station Name: %s\n", station->name);
			printf("Previous Station URI: %s\n", station->stream->uri);
			printf("Previous Station Location: %s\n", station->location);

			girl->selected_station_uri = station->stream->uri;

			MSG("on_station_select_changed: %s\n",
			    girl->selected_station_uri);

			girl->selected_station_name = station->name;

			MSG("on_station_select_changed: %s\n",
			    girl->selected_station_name);

			girl->selected_station_location = station->location;

			MSG("on_station_select_changed: %s\n",
			    girl->selected_station_location);

			girl->selected_station_release = station->release;

			MSG("on_station_select_changed: %s\n",
			    girl->selected_station_release);

			girl->selected_station_description = station->description;

			MSG("on_station_select_changed: %s\n",
			    girl->selected_station_description);

			appbar_send_msg(_("Selected %s in %s: %s"),
					girl->selected_station_name,
					girl->selected_station_location,
					girl->selected_station_uri,
					girl->selected_station_release);
			

			girl_helper_run(girl->selected_station_uri,
					girl->selected_station_name,
					GIRL_STREAM_SHOUTCAST,
					GIRL_STREAM_PLAYER);
		}
	} else {
		printf("Prev Station: At the beginning of Stations list!\n");
	}
}

void on_next_click(GtkWidget * a, gpointer user_data)
{
	GList *l = g_list_first(girl_stations);
	if (l != NULL) {
		GirlStationInfo *station = l->data;
		if (station->next != NULL) {
			girl_stations->data = station->next;
		} else {
			printf("Next Station: At the end of Stations list!\n");
		}
		cause_movement(1);
		if (station != NULL) {
			printf("Next Station ID: %s\n", station->id);
			printf("Next Station Name: %s\n", station->name);
			printf("Next Station URI: %s\n", station->stream->uri);
			printf("Next Station Location: %s\n", station->location);

			girl->selected_station_uri = station->stream->uri;

			MSG("on_station_select_changed: %s\n",
			    girl->selected_station_uri);

			girl->selected_station_name = station->name;

			MSG("on_station_select_changed: %s\n",
			    girl->selected_station_name);

			girl->selected_station_location = station->location;

			MSG("on_station_select_changed: %s\n",
			    girl->selected_station_location);

			girl->selected_station_release = station->release;

			MSG("on_station_select_changed: %s\n",
			    girl->selected_station_release);

			girl->selected_station_description = station->description;

			MSG("on_station_select_changed: %s\n",
			    girl->selected_station_description);

			appbar_send_msg(_("Selected %s in %s: %s"),
					girl->selected_station_name,
					girl->selected_station_location,
					girl->selected_station_uri,
					girl->selected_station_release);
			
			girl_helper_run(station->stream->uri,
					station->name,
					GIRL_STREAM_SHOUTCAST,
					GIRL_STREAM_PLAYER);
		}
	}
}

void on_listeners_selector_button_clicked(GtkWidget * a,
					  gpointer user_data)
{
	gtk_widget_show(listeners_selector);
	gdk_window_raise(listeners_selector->window);
}

void on_listeners_selector_changed(GtkWidget * a, gpointer user_data)
{
	if (girl->selected_listener_uri != NULL)
		g_free(girl->selected_listener_uri);

	girl->selected_listener_uri =
	    g_strdup(g_object_get_data(G_OBJECT(a), "listener_uri"));
	MSG("on_listener_select_changed: %s\n",
	    girl->selected_listener_uri);

	girl->selected_listener_name =
	    g_strdup(g_object_get_data(G_OBJECT(a), "listener_name"));
	MSG("on_listener_select_changed: %s\n",
	    girl->selected_listener_name);

	girl->selected_listener_location =
	    g_strdup(g_object_get_data(G_OBJECT(a), "listener_location"));
	MSG("on_listener_select_changed: %s\n",
	    girl->selected_listener_location);

	girl->selected_listener_release =
	    g_strdup(g_object_get_data(G_OBJECT(a), "listener_release"));
	MSG("on_listener_select_changed: %s\n",
	    girl->selected_listener_release);

	girl->selected_listener_description =
	    g_strdup(g_object_get_data(G_OBJECT(a), "listener_description"));
	MSG("on_listener_select_changed: %s\n",
	    girl->selected_listener_description);

	appbar_send_msg(_("Selected %s in %s: %s (%f)"),
			girl->selected_listener_name,
			girl->selected_listener_location,
			girl->selected_listener_uri,
		        girl->selected_listener_release);

	girl_helper_run(girl->selected_listener_uri,
			girl->selected_listener_name,
			GIRL_STREAM_SHOUTCAST,
			GIRL_STREAM_PLAYER);

}

void on_programs_selector_button_clicked(GtkWidget * a, gpointer user_data)
{
	gtk_widget_show(programs_selector);
	gdk_window_raise(programs_selector->window);
}

void on_programs_selector_changed(GtkWidget * a, gpointer user_data)
{
	if (girl->selected_program_uri != NULL)
		g_free(girl->selected_program_uri);

	girl->selected_program_uri =
	    g_strdup(g_object_get_data(G_OBJECT(a), "program_uri"));
	MSG("on_program_select_changed: %s\n", girl->selected_program_uri);

	girl->selected_program_name =
	    g_strdup(g_object_get_data(G_OBJECT(a), "program_name"));
	MSG("on_program_select_changed: %s\n",
	    girl->selected_program_name);

	girl->selected_program_location =
	    g_strdup(g_object_get_data(G_OBJECT(a), "program_location"));
	MSG("on_program_select_changed: %s\n",
	    girl->selected_program_location);

	girl->selected_program_release =
	    g_strdup(g_object_get_data(G_OBJECT(a), "program_release"));
	MSG("on_program_select_changed: %s\n",
	    girl->selected_program_release);

	girl->selected_program_description =
	    g_strdup(g_object_get_data(G_OBJECT(a), "program_description"));
	MSG("on_program_select_changed: %s\n",
	    girl->selected_program_description);

	appbar_send_msg(_("Selected %s in %s: %s"),
			girl->selected_program_name,
			girl->selected_program_location,
			girl->selected_program_uri,
			girl->selected_program_release);

	girl_helper_run(girl->selected_program_uri,
			girl->selected_program_name,
			GIRL_STREAM_SHOUTCAST,
			GIRL_STREAM_PLAYER);
}

void on_stations_selector_button_clicked(GtkWidget * a, gpointer user_data)
{
	gtk_widget_show(stations_selector);
	gdk_window_raise(stations_selector->window);
}

void on_stations_selector_changed(GtkWidget * a, gpointer user_data)
{
	if (girl->selected_station_uri != NULL)
		g_free(girl->selected_station_uri);

	girl->selected_station_uri =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_uri"));
	MSG("on_station_select_changed: %s\n", girl->selected_station_uri);

	girl->selected_station_name =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_name"));
	MSG("on_station_select_changed: %s\n",
	    girl->selected_station_name);

	girl->selected_station_location =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_location"));
	MSG("on_station_select_changed: %s\n",
	    girl->selected_station_location);

	girl->selected_station_release =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_release"));
	MSG("on_station_select_changed: %s\n",
	    girl->selected_station_release);

	girl->selected_station_description =
	    g_strdup(g_object_get_data(G_OBJECT(a), "station_description"));
	MSG("on_station_select_changed: %s\n",
	    girl->selected_station_description);

	appbar_send_msg(_("Selected %s in %s: %s"),
			girl->selected_station_name,
			girl->selected_station_location,
			girl->selected_station_uri,
			girl->selected_station_release);

	girl_helper_run(girl->selected_station_uri,
			girl->selected_station_name,
			GIRL_STREAM_SHOUTCAST,
		        GIRL_STREAM_PLAYER);
}

void quit_app(GtkWidget * a, gpointer user_data)
{
	gnome_config_push_prefix("/girl/General/");
	gnome_config_set_string("selected_listener_uri",
				girl->selected_listener_uri);
	gnome_config_set_string("selected_listener_name",
				girl->selected_listener_name);
	gnome_config_set_string("selected_listener_location",
				girl->selected_listener_location);
	gnome_config_set_string("selected_listener_release",
				girl->selected_listener_release);
	gnome_config_set_string("selected_station_uri",
				girl->selected_station_uri);
	gnome_config_set_string("selected_station_name",
				girl->selected_station_name);
	gnome_config_set_string("selected_station_location",
				girl->selected_station_location);
	gnome_config_set_string("selected_station_release",
				girl->selected_station_release);
	gnome_config_set_string("selected_station_description",
				girl->selected_station_description);
	gnome_config_sync();
	gnome_config_pop_prefix();

	gtk_widget_destroy(girl_app);
	if (GTK_IS_WIDGET(stations_selector)) {
		gtk_widget_destroy(stations_selector);
	}

	gtk_main_quit();
}

void about_app(GtkWidget * a, gpointer user_data)
{
	static GtkWidget *about = NULL;
	const gchar *authors[] = {
		("Ole Aamot <ole@src.gnome.org>"),
		NULL,
	};
	const gchar *translator_credits = _("translator_credits");

	if (about) {
		gdk_window_raise(about->window);
		return;
	}

	about = gnome_about_new(_("GNOME Internet Radio Locator"), VERSION, _("Copyright 2014 Ole Aamot Software\nCopyright 2002 Free Software Foundation"), _("Internet radio station and listener locator"), (const gchar **) authors, NULL,	/* documenters */
				strcmp(translator_credits,
				       "translator_credits") !=
				0 ? translator_credits : NULL, girl->icon);

	g_signal_connect(G_OBJECT(about), "destroy",
			 G_CALLBACK(gtk_widget_destroy), NULL);
	g_signal_connect(G_OBJECT(about), "delete-event",
			 G_CALLBACK(gtk_widget_destroy), NULL);
	g_object_add_weak_pointer(G_OBJECT(about), (void **) &(about));
	gtk_widget_show(about);
}

void about_listener(GtkWidget * a, gpointer user_data)
{
	static GtkWidget *about_listener = NULL;
	const gchar *translator_credits = _("translator_credits");
	const gchar *authors[] = {
		girl->selected_listener_name,
		NULL,
	};

	if (about_listener) {
		gdk_window_raise(about_listener->window);
		return;
	}

	about_listener = gnome_about_new(girl->selected_listener_name,
					girl->selected_listener_release,
					girl->selected_listener_location,
					girl->selected_listener_description,
					authors,
					NULL,
					NULL,
					girl->icon);
	g_signal_connect(G_OBJECT(about_listener), "destroy",
			 G_CALLBACK(gtk_widget_destroy), NULL);
	g_signal_connect(G_OBJECT(about_listener), "delete-event",
			 G_CALLBACK(gtk_widget_destroy), NULL);
	g_object_add_weak_pointer(G_OBJECT(about_listener), (void **) &(about_listener));
	gtk_widget_show(about_listener);
}

void about_program(GtkWidget * a, gpointer user_data)
{
	static GtkWidget *about_program = NULL;
	const gchar *translator_credits = _("translator_credits");
	const gchar *authors[] = {
		girl->selected_program_name,
		NULL,
	};

	if (about_program) {
		gdk_window_raise(about_program->window);
		return;
	}

	if (girl->selected_program_name != NULL) {
		about_program = gnome_about_new(girl->selected_program_name,
						girl->selected_program_release,
						girl->selected_program_location,
						girl->selected_program_description,
						authors,
						NULL,
						NULL,
						girl->icon);
		g_signal_connect(G_OBJECT(about_program), "destroy",
				 G_CALLBACK(gtk_widget_destroy), NULL);
		g_signal_connect(G_OBJECT(about_program), "delete-event",
				 G_CALLBACK(gtk_widget_destroy), NULL);
		g_object_add_weak_pointer(G_OBJECT(about_program), (void **) &(about_program));
		gtk_widget_show(about_program);
	}

}

void about_station(GtkWidget * a, gpointer user_data)
{
	static GtkWidget *about_station = NULL;
	const gchar *translator_credits = _("translator_credits");
	const gchar *authors[] = {
		girl->selected_station_name,
		NULL,
	};

	if (about_station) {
		gdk_window_raise(about_station->window);
		return;
	}

	about_station = gnome_about_new(girl->selected_station_name,
					girl->selected_station_release,
					girl->selected_station_location,
					girl->selected_station_description,
					authors,
					NULL,
					NULL,
					girl->icon);

	g_signal_connect(G_OBJECT(about_station), "destroy",
			 G_CALLBACK(gtk_widget_destroy), NULL);
	g_signal_connect(G_OBJECT(about_station), "delete-event",
			 G_CALLBACK(gtk_widget_destroy), NULL);
	g_object_add_weak_pointer(G_OBJECT(about_station), (void **) &(about_station));
	gtk_widget_show(about_station);
}

void on_listen_button_clicked(GtkWidget *a, gpointer user_data)
{


	/* girl_stream_player(a, user_data); */

	appbar_send_msg(_("Listening to %s in %s: %s "),
			girl->selected_station_name,
			girl->selected_station_location,
			girl->selected_station_uri,
			girl->selected_station_release);

	girl_helper_run(girl->selected_station_uri,
			girl->selected_station_name,
			GIRL_STREAM_SHOUTCAST,
			GIRL_STREAM_PLAYER);

}

void on_record_button_clicked(GtkWidget *a, gpointer user_data)
{
	GtkWidget *dialog;

	if (girl->selected_station_name != NULL) {
		appbar_send_msg(_("Recording from %s in %s: %s "),
				girl->selected_station_name,
				girl->selected_station_location,
				girl->selected_station_uri,
				girl->selected_station_release);
		girl_helper_run(girl->selected_station_uri,
				girl->selected_station_name,
				GIRL_STREAM_SHOUTCAST,
				GIRL_STREAM_RECORD);
	} else {
		dialog = gtk_message_dialog_new(GTK_WINDOW(girl_app),
						GTK_DIALOG_MODAL,
						GTK_MESSAGE_ERROR,
						GTK_BUTTONS_CLOSE, "Could not record!", NULL);
		gtk_dialog_run(GTK_DIALOG(dialog));
		gtk_widget_destroy(dialog);
	}
}

GnomeVFSFileSize get_size(GnomeVFSURI * uri)
{
	GnomeVFSFileInfoOptions info_options;
	GnomeVFSFileInfo *info;
	GnomeVFSFileSize len;
	GnomeVFSResult result;

	info_options = GNOME_VFS_FILE_INFO_DEFAULT;

	info = gnome_vfs_file_info_new();
	result = gnome_vfs_get_file_info_uri(uri, info, info_options);

	if (result != GNOME_VFS_OK)
		return 0;

	len = (unsigned long long) info->size;
	gnome_vfs_file_info_unref(info);

	return len;
}

static gint
girl_archive_progress_callback(GnomeVFSXferProgressInfo * info, gpointer data)
{

	while (gtk_events_pending())
		gtk_main_iteration();

	appbar_send_msg(_("Archiving %s"), (gchar *) data);
	MSG("%s", gnome_vfs_result_to_string(info->status));
	return TRUE;
}

GnomeVFSURI *girl_archive_new(gchar *src, gchar *dest)
{
	GnomeVFSURI *src_uri, *dest_uri;
	GnomeVFSResult result;
	GnomeVFSXferOptions xfer_options;

	xfer_options = GNOME_VFS_XFER_DEFAULT;

	src_uri = gnome_vfs_uri_new(src);
	dest_uri = gnome_vfs_uri_new(dest);

	result = gnome_vfs_xfer_uri(src_uri,
				    dest_uri,
				    xfer_options,
				    GNOME_VFS_XFER_ERROR_MODE_QUERY,
				    GNOME_VFS_XFER_OVERWRITE_MODE_REPLACE,
				    girl_archive_progress_callback, src);

	gnome_vfs_uri_unref(src_uri);

	if (result != GNOME_VFS_OK)
		return NULL;

	return dest_uri;
}

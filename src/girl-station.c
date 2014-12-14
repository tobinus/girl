/* $id$
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
#include <stdio.h>
#include <string.h>
#include <gtk/gtk.h>
#include <libxml/xmlmemory.h>
#include <libxml/parser.h>
#include <libgnome/gnome-exec.h>
#include <libgnomevfs/gnome-vfs.h>
#include <libgnomevfs/gnome-vfs-application-registry.h>

#include "girl.h"
#include "girl-station.h"

extern GirlData *girl;
extern GList *girl_stations;
extern GList *girl_listeners;
extern GList *girl_streams;

extern GtkWidget *girl_app;

void show_error(gchar * msg)
{
#ifdef GIRL_CLI

	printf("Error!!!\n");
#else
	GtkWidget *dialog;

	dialog = gtk_message_dialog_new(GTK_WINDOW(girl_app),
					GTK_DIALOG_MODAL,
					GTK_MESSAGE_ERROR,
					GTK_BUTTONS_CLOSE, msg, NULL);
	gtk_dialog_run(GTK_DIALOG(dialog));
	gtk_widget_destroy(dialog);
#endif				/* GIRL_CLI */
}

static void
cb_child_watch( GPid  pid,
		gint  status)
{
	/* Remove timeout callback */
	g_source_remove(girl->timeout_id );

	/* Close pid */
	g_spawn_close_pid( pid );
}

static gboolean
cb_out_watch( GIOChannel   *channel,
	      GIOCondition  cond)
{
	gchar *string;
	gsize  size;

	if( cond == G_IO_HUP )
	{
		g_io_channel_unref( channel );
		return( FALSE );
	}

	g_io_channel_read_line( channel, &string, &size, NULL, NULL );

	/* gnome_appbar_pop(girl->appbar); */
	/* gnome_appbar_push(girl->appbar, string); */

	g_free( string );

	return( TRUE );
}

static gboolean
cb_err_watch( GIOChannel   *channel,
	      GIOCondition  cond)
{
	gchar *string;
	gsize  size;

	if( cond == G_IO_HUP )
	{
		g_io_channel_unref( channel );
		return( FALSE );
	}

	g_io_channel_read_line( channel, &string, &size, NULL, NULL );
	/* gnome_appbar_pop(girl->appbar); */
	/* gnome_appbar_push(girl->appbar, string); */
	g_free( string );

	return( TRUE );
}

static gboolean
cb_timeout( )
{
	/* Bounce progress bar */
	gtk_progress_bar_pulse( girl->progress );

	return( TRUE );
}

void girl_helper_run(char *url, char *name, GirlStreamType type, GirlHelperType helper)
{
	GError *err = NULL;
	GTimeVal mtime;

	const char *mime_info;
	/* GnomeVFSMimeApplication *app; */
	char *app, *command, *msg, *archive;
	char **argv = NULL;
	gint argc;
	gint status;

	GPid        pid;
	gint        out, error;
	GIOChannel *out_ch, *err_ch;
	gboolean    ret;

	g_return_if_fail(url != NULL);
	MSG("%s", url);

	mime_info = gnome_vfs_get_mime_type(url);

	g_get_current_time(&mtime);

	/* app = gnome_vfs_mime_get_default_application (mime_info); */

	if (helper == GIRL_STREAM_PLAYER) {
		app = g_strdup(GIRL_HELPER_PLAYER);
	}

	if (helper == GIRL_STREAM_RECORD) {
		app = g_strdup(GIRL_HELPER_RECORD);
	}

	if (app != NULL) {
		command = g_strconcat(app, " ", url, NULL);
		g_print("Helper application is %s\n", command);
		if (type == GIRL_STREAM_SHOUTCAST) {
			if (helper == GIRL_STREAM_PLAYER) {
				command = g_strconcat(app, " ", url, NULL);
				/* argv[0] = g_strdup(app); */
				/* argv[1] = g_strdup(url); */
			}
			if (helper == GIRL_STREAM_RECORD) {
				/* archive = g_strconcat("file://", g_get_home_dir(), "/.girl/", name, NULL); */
				/* girl_archive_new(url, archive); */
				/* printf("Archiving program at %s\n", archive); */
				command = g_strconcat(app, " ", url, NULL);
				/* " -d ", g_get_home_dir(), "/.girl -D \"", name, "\" -s -a -u girl/", VERSION, NULL); */
				/* command = g_strconcat(command, " -d ", g_get_home_dir(), "/.girl/", name, " -D %S%A%T -t 10 -u girl/", VERSION, NULL); */
			}
			g_print("Helper command is %s\n", command);
		}
		/* gnome_vfs_mime_application_free (app); */
	} else {
		if (helper == GIRL_STREAM_PLAYER) {
			msg = g_strdup_printf(_("An error happened trying to play "
						"%s\nEither the file doesn't exist, or you "
						"don't have a player for it."),
					      url);
		}
		if (helper == GIRL_STREAM_RECORD) {
			msg = g_strdup_printf(_("An error happened trying to record "
						"%s\nEither the file doesn't exist, or you "
						"don't have a recorder for it."),
					      url);
		}
		if (msg != NULL) {
			show_error(msg);
			g_free(msg);
		}
		return;
	}

	if (helper == GIRL_STREAM_PLAYER) {
		if (!g_spawn_command_line_async(command, &err)) {
			msg = g_strdup_printf(_("Failed to open URL: '%s'\n"
						"Details: %s"), url, err->message);
			show_error(msg);
			g_error_free(err);
			g_free(msg);
		} else {
			g_print("Launching %s player\n", command);
		}
	}

	if (helper == GIRL_STREAM_RECORD) {

/* #if 0 */
		/* gchar *argv[] = { command, NULL }; */
		/* Spawn child process */

		g_shell_parse_argv(command,
				   &argc,
				   &argv,
				   NULL);
		ret = g_spawn_async_with_pipes (".",
						argv,
						NULL,
						G_SPAWN_SEARCH_PATH|G_SPAWN_STDOUT_TO_DEV_NULL|G_SPAWN_STDERR_TO_DEV_NULL|G_SPAWN_DO_NOT_REAP_CHILD,
						NULL,
						NULL,
						&pid,
						NULL,
						NULL,
						NULL,
						&err);
		if( ! ret )
		{
			msg = g_strdup_printf(_("Failed to run %s (%i)\n"), command, pid);
			show_error(msg);
			g_free(msg);
			return;
		}
		/* Add watch function to catch termination of the process. This function
		 * will clean any remnants of process. */
		g_child_watch_add( pid, (GChildWatchFunc)cb_child_watch, girl);

		/* Install timeout fnction that will move the progress bar */
		girl->timeout_id = g_timeout_add(100,(GSourceFunc)cb_timeout,girl);
/* #endif */
/* #if 0 */
		ret = g_spawn_async_with_pipes( NULL, /* command */ argv, NULL,
						/* G_SPAWN_DO_NOT_REAP_CHILD */ G_SPAWN_DEFAULT, NULL,
						NULL, &pid, NULL, &out, &error, NULL );
		if( ! ret )
		{
			msg = g_strdup_printf(_("Failed to run %s (%i)\n"), command, pid);
			show_error(msg);
			g_free(msg);
			return;
		}
		/* Add watch function to catch termination of the process. This function
		 * will clean any remnants of process. */
		g_child_watch_add( pid, (GChildWatchFunc)cb_child_watch, girl );
		/* Create channels that will be used to read girl from pipes. */
#ifdef G_OS_WIN32
		out_ch = g_io_channel_win32_new_fd( out );
		err_ch = g_io_channel_win32_new_fd( err );
#else
		out_ch = g_io_channel_unix_new( out );
		err_ch = g_io_channel_unix_new( err );
#endif
		/* Add watches to channels */
		g_io_add_watch( out_ch, G_IO_IN | G_IO_HUP, (GIOFunc)cb_out_watch, girl );
		g_io_add_watch( err_ch, G_IO_IN | G_IO_HUP, (GIOFunc)cb_err_watch, girl );
		/* Install timeout fnction that will move the progress bar */
		girl->timeout_id = g_timeout_add( 100, (GSourceFunc)cb_timeout, girl );
/* #endif */
#if 0
		if (!g_spawn_command_line_sync(command, stdout, stderr, status, &err)) {
			msg = g_strdup_printf(_("Failed to open URL: '%s'\n"
						"Status code: %i\n"
						"Details: %s"), url, status, err->message);
			show_error(msg);
			g_error_free(err);
			g_free(msg);
		} else {
			g_print("Launching %s\n", command);
		}
		/* Add watch function to catch termination of the process. This function
		 * will clean any remnants of process. */
		g_child_watch_add( pid, (GChildWatchFunc)cb_child_watch, girl );
#endif
	}
}

void girl_stream_player(GtkWidget * widget, gpointer data)
{
	girl_helper_run(girl->selected_station_uri,
			girl->selected_station_name,
			GIRL_STREAM_SHOUTCAST,
		        GIRL_STREAM_PLAYER);
}

void girl_stream_record(GtkWidget * widget, gpointer data)
{
	girl_helper_run(girl->selected_station_uri,
			girl->selected_station_name,
			GIRL_STREAM_SHOUTCAST,
		        GIRL_STREAM_RECORD);
}

void girl_stream_select(GirlStreamsInfo * info)
{
}

static void
girl_station_parser(GirlStationInfo * station, xmlDocPtr doc,
		    xmlNodePtr cur)
{
	xmlNodePtr sub;
	char *chans;

	g_return_if_fail(station != NULL);
	g_return_if_fail(doc != NULL);
	g_return_if_fail(cur != NULL);

	station->id = xmlGetProp(cur, "id");
	MSG("station->id = %s\n", station->id);
	station->name = xmlGetProp(cur, "name");
	MSG("station->name = %s\n", station->name);
	station->rank = xmlGetProp(cur, "rank");
	MSG("station->rank = %s\n", station->rank);
	station->type = xmlGetProp(cur, "type");
	MSG("station->type = %s\n", station->type);
	station->release = xmlGetProp(cur, "release");
	MSG("station->release = %s\n", station->release);

	sub = cur->xmlChildrenNode;

	while (sub != NULL) {

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "frequency"))) {
			station->frequency =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			MSG("station->frequency = %s\n",
			    station->frequency);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "location"))) {
			station->location =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			MSG("station->location = %s\n", station->location);

		}

		if ((!xmlStrcmp
		     (sub->name, (const xmlChar *) "description"))) {
			station->description =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			MSG("station->description = %s\n", station->description);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "uri"))) {
			station->uri =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			MSG("station->uri = %s\n", station->uri);
			/* fprintf(stdout, "%s (%s)\n%s\n\n", station->name, station->location, station->uri); */
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "stream"))) {

			GirlStreamInfo *stream = g_new0(GirlStreamInfo, 1);
			station->stream = stream;

			station->stream->mimetype =
			    xmlGetProp(sub, "mime");
			MSG("station->stream->mimetype = %s\n",
			    station->stream->mimetype);
			if (xmlGetProp(sub, "bitrate") != NULL) {
				station->stream->bitrate =
				    atol(xmlGetProp(sub, "bitrate"));
				MSG("station->stream->bitrate = %li\n",
				    station->stream->bitrate);
			}

			if (xmlGetProp(sub, "samplerate") != NULL) {
				station->stream->samplerate =
				    atol(xmlGetProp(sub, "samplerate"));
			}

			MSG("station->stream->samplerate = %li\n",
			    station->stream->samplerate);
			station->stream->uri = xmlGetProp(sub, "uri");
			MSG("station->stream->uri = %s\n",
			    station->stream->uri);

			/* fprintf(stdout, "%s (%s)\n%s\n\n, ", station->name, station->location, station->stream->uri); */
			
			chans = xmlGetProp(sub, "channels");

			if (chans != NULL) {
				if (strcmp(chans, "stereo") == 0) {
					station->stream->channels =
					    GIRL_CHANNELS_STEREO;
					MSG("station->stream->channels = %d\n", station->stream->channels);
				} else if (strcmp(chans, "mono") == 0) {
					station->stream->channels =
					    GIRL_CHANNELS_MONO;
					MSG("station->stream->channels = %d\n", station->stream->channels);
				} else if (strcmp(chans, "5:1") == 0) {
					station->stream->channels =
					    GIRL_CHANNELS_5_1;
					MSG("station->stream->channels = %d\n", station->stream->channels);
				}
				g_free(chans);
			}

		}
		/* if... "stream" */
		sub = sub->next;
	}

	return;
}

GirlStationInfo *girl_station_load_from_yp(char *uri)
{
	GirlStationInfo *gstation;
	return gstation;
}

GirlStationInfo *girl_station_load_from_http(GirlStationInfo * head,
					     gpointer data)
{
	GirlStationInfo *gstation;
	gstation = girl_station_load_from_file (head, "http://girl.src.oka.no/stations.xml");
	return gstation;
}

GirlStationInfo *girl_station_load_from_file(GirlStationInfo * head,
					     char *filename)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	GirlStationInfo *curr = NULL;
	char *version;
	GirlStationInfo *mem_station;

	g_return_val_if_fail(filename != NULL, NULL);

	doc = xmlReadFile(filename, NULL, 0);

	if (doc == NULL) {
		perror("xmlParseFile");
		xmlFreeDoc(doc);
		return NULL;
	}

	cur = xmlDocGetRootElement(doc);

	if (cur == NULL) {
		fprintf(stderr, "Empty document\n");
		xmlFreeDoc(doc);
		return NULL;
	}

	if (xmlStrcmp(cur->name, (const xmlChar *) "girl")) {
		fprintf(stderr,
			"Document of wrong type, root node != girl\n");
		xmlFreeDoc(doc);
		return NULL;
	}

	version = xmlGetProp(cur, "version");

	MSG("Valid Girl %s XML document... Parsing stations...\n",
	    version);

	free(version);

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {

		if ((!xmlStrcmp(cur->name, (const xmlChar *) "station"))) {

			MSG("Found a new station.\n");

			curr = g_new0(GirlStationInfo, 1);
			mem_station = g_new0(GirlStationInfo, 1);

			girl_station_parser(curr, doc, cur);

			curr->next = head;

			head = curr;

			mem_station = head;

			girl_stations = g_list_append(girl_stations, (GirlStationInfo *)mem_station);

			MSG("Done with parsing the station.\n");

		}
		cur = cur->next;
	}

	MSG("Finished parsing XML document.\n");

	xmlFreeDoc(doc);

	return curr;
}

/* $id$
 *
 * GNOME Internet Radio Locator
 *
 * Copyright (C) 2014, 2015  Ole Aamot Software
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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <config.h>
#include <stdio.h>
#include <string.h>
#include <dirent.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/resource.h>
#include <glib.h>
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

	/* FIXME? g_source_remove(girl->timeout_id ); */

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

	/* const char *mime_info = NULL; */
	/* GnomeVFSMimeApplication *app; */
	char *app = NULL, *command = NULL, *msg = NULL;
        /* char *archive; */
	char **argv = NULL;
	gint argc;
	/* gint status; */

	GPid        pid;
	gint        out, error;
	GIOChannel *out_ch, *err_ch;
	gboolean    ret;

	if (signal(SIGCHLD, SIG_IGN) == SIG_ERR) {
		perror(0);
		exit(1);
	}
	
	g_return_if_fail(url != NULL);
	GIRL_DEBUG_MSG("%s", url);

	/* mime_info = gnome_vfs_get_mime_type(url); */

	g_get_current_time(&mtime);

	/* app = (gchar *)gnome_vfs_mime_get_default_application (mime_info); */
	
	if (helper == GIRL_STREAM_PLAYER) {
		app = g_strdup(GIRL_HELPER_PLAYER);
	}

	if (helper == GIRL_STREAM_RECORD) {
		app = g_strdup(GIRL_HELPER_RECORD);
	}

	if (g_strcmp0(app,"no")!=0) {
		command = g_strconcat(app, " ", url, NULL);
		GIRL_DEBUG_MSG("Helper application is %s\n", command);
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
		        GIRL_DEBUG_MSG("Helper command is %s\n", command);
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


#if 0
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
		
		girl->player_pid = pid;
		girl->player_status = GIRL_PLAYER_TRUE;
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
		/* Create channels that will be used to read girl from pipes. *
			
#ifdef G_OS_WIN32
		out_ch = g_io_channel_win32_new_fd( out );
		err_ch = g_io_channel_win32_new_fd( error );
#else
		out_ch = g_io_channel_unix_new( out );
		err_ch = g_io_channel_unix_new( error );
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
			GIRL_DEBUG_MSG("Launching %s\n", command);
		}
		/* Add watch function to catch termination of the process. This function
		 * will clean any remnants of process. */
		g_child_watch_add( pid, (GChildWatchFunc)cb_child_watch, girl );
#endif
#endif

		/* Original async player code */
		
		if (!g_spawn_command_line_async(command, &err)) {
			msg = g_strdup_printf(_("Failed to open URL: '%s'\n"
						"Details: %s"), url, err->message);
			show_error(msg);
			g_error_free(err);
			g_free(msg);
		} else {
			girl->player_status = GIRL_PLAYER_TRUE;
			GIRL_DEBUG_MSG("Launching %s player\n", command);
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

		girl->record_pid = pid;
		girl->record_status = GIRL_RECORD_TRUE;

		girl_archive_new(pid, "archive.mp3");
				
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
		err_ch = g_io_channel_win32_new_fd( error );
#else
		out_ch = g_io_channel_unix_new( out );
		err_ch = g_io_channel_unix_new( error );
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
			GIRL_DEBUG_MSG("Launching %s\n", command);
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

	station->id = (gchar *)xmlGetProp(cur, (const xmlChar *)"id");
	GIRL_DEBUG_MSG("station->id = %s\n", station->id);
	station->name = (gchar *)xmlGetProp(cur, (const xmlChar *)"name");
	GIRL_DEBUG_MSG("station->name = %s\n", station->name);
	station->rank = (gchar *)xmlGetProp(cur, (const xmlChar *)"rank");
	GIRL_DEBUG_MSG("station->rank = %s\n", station->rank);
	station->type = (gchar *)xmlGetProp(cur, (const xmlChar *)"type");
	GIRL_DEBUG_MSG("station->type = %s\n", station->type);
	station->band = (gchar *)xmlGetProp(cur, (const xmlChar *)"band");
	GIRL_DEBUG_MSG("station->band = %s\n", station->band);

	sub = cur->xmlChildrenNode;

	while (sub != NULL) {

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "frequency"))) {
			station->frequency = (gchar *)
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("station->frequency = %s\n",
			    station->frequency);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "location"))) {
			station->location = (gchar *)
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("station->location = %s\n", station->location);

		}

		if ((!xmlStrcmp
		     (sub->name, (const xmlChar *) "description"))) {
			station->description = (gchar *)
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("station->description = %s\n", station->description);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "uri"))) {
			station->uri = (gchar *)
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			GIRL_DEBUG_MSG("station->uri = %s\n", station->uri);
			/* fprintf(stdout, "%s (%s)\n%s\n\n", station->name, station->location, station->uri); */
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "stream"))) {

			GirlStreamInfo *stream = g_new0(GirlStreamInfo, 1);
			station->stream = stream;

			station->stream->mimetype = (gchar *)
				xmlGetProp(sub, (const xmlChar *)"mime");
			GIRL_DEBUG_MSG("station->stream->mimetype = %s\n",
			    station->stream->mimetype);
			if (xmlGetProp(sub, (const xmlChar *)"bitrate") != NULL) {
				station->stream->bitrate =
					(glong)atol((char *)xmlGetProp(sub, (const xmlChar *)"bitrate"));
				GIRL_DEBUG_MSG("station->stream->bitrate = %li\n",
				    station->stream->bitrate);
			}

			if (xmlGetProp(sub, (const xmlChar *)"samplerate") != NULL) {
				station->stream->samplerate = (glong) 
					atol((char *)xmlGetProp(sub, (const xmlChar *)"samplerate"));
			}

			GIRL_DEBUG_MSG("station->stream->samplerate = %li\n",
			    station->stream->samplerate);
			station->stream->uri = (gchar *)xmlGetProp(sub, (const xmlChar *)"uri");
			GIRL_DEBUG_MSG("station->stream->uri = %s\n",
			    station->stream->uri);

			/* fprintf(stdout, "%s (%s)\n%s\n\n", station->name, station->location, station->stream->uri); */
			
			chans = (gchar *)xmlGetProp(sub, (const xmlChar *)"stations");

			if (chans != NULL) {
				if (strcmp(chans, "stereo") == 0) {
					station->stream->channels =
					    GIRL_CHANNELS_STEREO;
					GIRL_DEBUG_MSG("station->stream->channels = %d\n", station->stream->channels);
				} else if (strcmp(chans, "mono") == 0) {
					station->stream->channels =
					    GIRL_CHANNELS_MONO;
					GIRL_DEBUG_MSG("station->stream->channels = %d\n", station->stream->channels);
				} else if (strcmp(chans, "5:1") == 0) {
					station->stream->channels =
					    GIRL_CHANNELS_5_1;
					GIRL_DEBUG_MSG("station->stream->channels = %d\n", station->stream->channels);
				}
				g_free(chans);
			}

		}
		/* if... "stream" */
		sub = sub->next;
	}

	return;
}

GirlStationInfo *girl_station_load_from_http(GirlStationInfo * head,
					     gpointer data)
{
	GirlStationInfo *gstation;
	gstation = girl_station_load_from_file (head, "http://girl.src.oka.no/girl.xml");
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

	version = (gchar *)xmlGetProp(cur, (const xmlChar *)"version");

	GIRL_DEBUG_MSG("Valid Girl %s XML document... Parsing stations...\n",
	    version);

	free(version);

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {

		if ((!xmlStrcmp(cur->name, (const xmlChar *) "station"))) {

			GIRL_DEBUG_MSG("Found a new station.\n");

			curr = g_new0(GirlStationInfo, 1);
			mem_station = g_new0(GirlStationInfo, 1);

			girl_station_parser(curr, doc, cur);

			curr->next = head;

			head = curr;

			mem_station = head;

			girl_stations = g_list_append(girl_stations, (GirlStationInfo *)mem_station);

			GIRL_DEBUG_MSG("Done with parsing the station.\n");

		}
		cur = cur->next;
	}

	GIRL_DEBUG_MSG("Finished parsing XML document.\n");

	xmlFreeDoc(doc);

	return curr;
}

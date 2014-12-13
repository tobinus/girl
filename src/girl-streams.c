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
#include "girl-streams.h"

extern GirlData *girl;
extern GList *girl_stations;
extern GList *girl_streams;
extern GList *girl_listeners;
extern GList *girl_streams;

extern GtkWidget *girl_app;

GirlStreamsInfo *girl_streams_new (GirlStreamsInfo * head,
				   gchar *mime,
				   gchar *uri,
				   gchar *codec,
				   gchar *samplerate,
				   gchar *channels,
	                           gchar *bitrate) {
}

static void
girl_streams_parser(GirlStreamsInfo *streams, xmlDocPtr doc,
		    xmlNodePtr cur)
{
	xmlNodePtr sub;
	char *chans;

	g_return_if_fail(streams != NULL);
	g_return_if_fail(doc != NULL);
	g_return_if_fail(cur != NULL);

	streams->mime = xmlGetProp(cur, "mime");
	MSG("streams->mime = %s\n", streams->mime);
	streams->uri = xmlGetProp(cur, "uri");
	MSG("streams->uri = %s\n", streams->uri);
	streams->samplerate = xmlGetProp(cur, "samplerate");
	MSG("streams->samplerate = %s\n", streams->samplerate);
	streams->codec = xmlGetProp(cur, "codec");
	MSG("streams->codec = %s\n", streams->codec);
	streams->bitrate = xmlGetProp(cur, "bitrate");
	MSG("streams->bitrate = %s\n", streams->bitrate);
	streams->channels = xmlGetProp(cur, "channels");
	MSG("streams->channels = %s\n", streams->channels);

#if 0
	sub = cur->xmlChildrenNode;

	while (sub != NULL) {

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "frequency"))) {
			streams->frequency =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			MSG("streams->frequency = %s\n",
			    streams->frequency);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "location"))) {
			streams->location =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			MSG("streams->location = %s\n", streams->location);
			/* fprintf(stdout, "%s (%s), ", streams->name, streams->location); */
		}

		if ((!xmlStrcmp
		     (sub->name, (const xmlChar *) "description"))) {
			streams->description =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			MSG("streams->description = %s\n", streams->description);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "uri"))) {
			streams->uri =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			MSG("streams->uri = %s\n", streams->uri);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "encoder"))) {

			GirlEncoderInfo *encoder = g_new0(GirlEncoderInfo, 1);
			streams->encoder = encoder;

			streams->encoder->mimetype =
			    xmlGetProp(sub, "mime");
			MSG("streams->encoder->mimetype = %s\n",
			    streams->encoder->mimetype);
			if (xmlGetProp(sub, "bitrate") != NULL) {
				streams->encoder->bitrate =
				    atol(xmlGetProp(sub, "bitrate"));
				MSG("streams->encoder->bitrate = %li\n",
				    streams->encoder->bitrate);
			}

			if (xmlGetProp(sub, "samplerate") != NULL) {
				streams->encoder->samplerate =
				    atol(xmlGetProp(sub, "samplerate"));
			}

			MSG("streams->encoder->samplerate = %li\n",
			    streams->encoder->samplerate);
			streams->encoder->uri = xmlGetProp(sub, "uri");
			MSG("streams->encoder->uri = %s\n",
			    streams->encoder->uri);

			chans = xmlGetProp(sub, "channels");

			if (chans != NULL) {
				if (strcmp(chans, "stereo") == 0) {
					streams->encoder->channels =
					    GIRL_CHANNELS_STEREO;
					MSG("streams->encoder->channels = %d\n", streams->encoder->channels);
				} else if (strcmp(chans, "mono") == 0) {
					streams->encoder->channels =
					    GIRL_CHANNELS_MONO;
					MSG("streams->encoder->channels = %d\n", streams->encoder->channels);
				} else if (strcmp(chans, "5:1") == 0) {
					streams->encoder->channels =
					    GIRL_CHANNELS_5_1;
					MSG("streams->encoder->channels = %d\n", streams->encoder->channels);
				}
				g_free(chans);
			}

		}
		/* if... "encoder" */
		sub = sub->next;
	}
#endif

	return;
}

GirlStreamsInfo *girl_streams_load_from_http(GirlStreamsInfo * head,
					     gpointer data)
{
	GirlStreamsInfo *gstreams;
	gstreams = girl_streams_load_from_file (head, "http://girl.src.oka.no/streams.xml");
	return gstreams;
}

GirlStreamsInfo *girl_streams_load_from_file(GirlStreamsInfo * head,
					     char *filename)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	GirlStreamsInfo *curr = NULL;
	char *version;
	GirlStreamsInfo *mem_streams;

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

	MSG("Valid Girl %s XML document... Parsing streams...\n",
	    version);

	free(version);

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {

		if ((!xmlStrcmp(cur->name, (const xmlChar *) "streams"))) {

			MSG("Found a new streams.\n");

			curr = g_new0(GirlStreamsInfo, 1);
			mem_streams = g_new0(GirlStreamsInfo, 1);

			girl_streams_parser(curr, doc, cur);

			curr->next = head;

			head = curr;

			mem_streams = head;

			girl_streams = g_list_append(girl_streams, (GirlStreamsInfo *)mem_streams);

			MSG("Done with parsing the streams.\n");

		}
		cur = cur->next;
	}

	MSG("Finished parsing XML document.\n");

	xmlFreeDoc(doc);

	return curr;
}

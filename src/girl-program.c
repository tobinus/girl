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
#include "girl-program.h"

extern GirlData *girl;
extern GList *girl_stations;
extern GList *girl_programs;
extern GList *girl_listeners;

extern GtkWidget *girl_app;

static void
girl_program_parser(GirlProgramInfo * program, xmlDocPtr doc,
		    xmlNodePtr cur)
{
	xmlNodePtr sub;
	char *chans;

	g_return_if_fail(program != NULL);
	g_return_if_fail(doc != NULL);
	g_return_if_fail(cur != NULL);

	program->id = xmlGetProp(cur, "id");
	MSG("program->id = %s\n", program->id);
	program->name = xmlGetProp(cur, "name");
	MSG("program->name = %s\n", program->name);
	program->rank = xmlGetProp(cur, "rank");
	MSG("program->rank = %s\n", program->rank);
	program->type = xmlGetProp(cur, "type");
	MSG("program->type = %s\n", program->type);
	program->release = xmlGetProp(cur, "release");
	MSG("program->release = %s\n", program->release);

	sub = cur->xmlChildrenNode;

	while (sub != NULL) {

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "frequency"))) {
			program->frequency =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			MSG("program->frequency = %s\n",
			    program->frequency);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "location"))) {
			program->location =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			MSG("program->location = %s\n", program->location);
			/* fprintf(stdout, "%s (%s), ", program->name, program->location); */
		}

		if ((!xmlStrcmp
		     (sub->name, (const xmlChar *) "description"))) {
			program->description =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			MSG("program->description = %s\n", program->description);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "uri"))) {
			program->uri =
			    xmlNodeListGetString(doc, sub->xmlChildrenNode,
						 1);
			MSG("program->uri = %s\n", program->uri);
		}

		if ((!xmlStrcmp(sub->name, (const xmlChar *) "archive"))) {

			GirlArchiveInfo *archive = g_new0(GirlArchiveInfo, 1);
			program->archive = archive;

			program->archive->mimetype =
			    xmlGetProp(sub, "mime");
			MSG("program->archive->mimetype = %s\n",
			    program->archive->mimetype);
			if (xmlGetProp(sub, "bitrate") != NULL) {
				program->archive->bitrate =
				    atol(xmlGetProp(sub, "bitrate"));
				MSG("program->archive->bitrate = %li\n",
				    program->archive->bitrate);
			}

			if (xmlGetProp(sub, "samplerate") != NULL) {
				program->archive->samplerate =
				    atol(xmlGetProp(sub, "samplerate"));
			}

			MSG("program->archive->samplerate = %li\n",
			    program->archive->samplerate);
			program->archive->uri = xmlGetProp(sub, "uri");
			MSG("program->archive->uri = %s\n",
			    program->archive->uri);

			chans = xmlGetProp(sub, "channels");

			if (chans != NULL) {
				if (strcmp(chans, "stereo") == 0) {
					program->archive->channels =
					    GIRL_CHANNELS_STEREO;
					MSG("program->archive->channels = %d\n", program->archive->channels);
				} else if (strcmp(chans, "mono") == 0) {
					program->archive->channels =
					    GIRL_CHANNELS_MONO;
					MSG("program->archive->channels = %d\n", program->archive->channels);
				} else if (strcmp(chans, "5:1") == 0) {
					program->archive->channels =
					    GIRL_CHANNELS_5_1;
					MSG("program->archive->channels = %d\n", program->archive->channels);
				}
				g_free(chans);
			}

		}
		/* if... "archive" */
		sub = sub->next;
	}

	return;
}

GirlProgramInfo *girl_program_load_from_http(GirlProgramInfo * head,
					     gpointer data)
{
	GirlProgramInfo *gprogram;
	gprogram = girl_program_load_from_file (head, "http://girl.src.oka.no/programs.xml");
	return gprogram;
}

GirlProgramInfo *girl_program_load_from_file(GirlProgramInfo * head,
					     char *filename)
{
	xmlDocPtr doc = NULL;
	xmlNodePtr cur = NULL;
	GirlProgramInfo *curr = NULL;
	char *version;
	GirlProgramInfo *mem_program;

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

	MSG("Valid Girl %s XML document... Parsing programs...\n",
	    version);

	free(version);

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {

		if ((!xmlStrcmp(cur->name, (const xmlChar *) "program"))) {

			MSG("Found a new program.\n");

			curr = g_new0(GirlProgramInfo, 1);
			mem_program = g_new0(GirlProgramInfo, 1);

			girl_program_parser(curr, doc, cur);

			curr->next = head;

			head = curr;

			mem_program = head;

			girl_programs = g_list_append(girl_programs, (GirlProgramInfo *)mem_program);

			MSG("Done with parsing the program.\n");

		}
		cur = cur->next;
	}

	MSG("Finished parsing XML document.\n");

	xmlFreeDoc(doc);

	return curr;
}

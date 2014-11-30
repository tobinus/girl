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

#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/stat.h>

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#if 0
#define G_DISABLE_DEPRECATED
#define GDK_DISABLE_DEPRECATED
#define GDK_PIXBUF_DISABLE_DEPRECATED
#define GTK_DISABLE_DEPRECATED
#define GNOME_DISABLE_DEPRECATED
#endif

#include <libgnome/libgnome.h>
/* #include <libgnome/gnome-desktop-item.h> */
#include <gtk/gtk.h>

#include "girl.h"
#include "girl-gui.h"
#include "girl-station.h"

#ifdef DEBUG
#define MSG(x...) g_message(x)
#else
#define MSG(x...)
#endif

extern GtkWidget *girl_app;

GnomeUIInfo toolbar[] = {
	GNOMEUIINFO_ITEM_STOCK(("Listen"), ("Listen to selected station"),
			       on_listen_button_clicked,
			       GTK_STOCK_REFRESH),
	GNOMEUIINFO_ITEM_STOCK(("Listeners"), ("Listeners"),
			       on_listeners_selector_button_clicked,
			       GTK_STOCK_FIND),
	GNOMEUIINFO_ITEM_STOCK(("Stations"), ("Stations"),
			       on_stations_selector_button_clicked,
			       GTK_STOCK_PREFERENCES),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(("Previous"),
			       ("Go back to the previous station"),
			       on_previous_click, GTK_STOCK_GO_BACK),
	GNOMEUIINFO_ITEM_STOCK(("Next"), ("Proceed to the next station"),
			       on_next_click, GTK_STOCK_GO_FORWARD),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(("About Listener"),
			       ("About the GNOME Internet Radio Locator"),
			       about_listener, GNOME_STOCK_ABOUT),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(("About Station"),
			       ("About the GNOME Internet Radio Locator"),
			       about_station, GNOME_STOCK_ABOUT),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(("About Program"),
			       ("About the GNOME Internet Radio Locator"),
			       about_app, GNOME_STOCK_ABOUT),
	GNOMEUIINFO_SEPARATOR,
	GNOMEUIINFO_ITEM_STOCK(("Exit"),
			       ("Quit the GNOME Internet Radio Locator"),
			       quit_app, GTK_STOCK_QUIT),
	GNOMEUIINFO_END,

};

/* 
   Reading list of stations from

   $PREFIX/share/girl/stations.xml 
   $HOME/.girl/stations.xml

*/

GtkWidget *create_listeners_selector(char *selected_listener_uri,
				     char *filename)
{
	GirlListenerInfo *listenerinfo, *locallistener;

	GtkWidget *listeners_selector;
	GtkWidget *align, *menu, *drop_down, *item;

	gchar *listener_uri, *listener_name, *listener_location, *listener_release, *listener_description;
	gchar *label, *world_listener_xml_uri,
	    *local_listener_xml_file;

	int i = 0, selection = -1;

	/* The Listeners dialog */
	listeners_selector = gtk_dialog_new_with_buttons(("Select a listener"), GTK_WINDOW(girl_app), 0,	/* flags */
							 GTK_STOCK_CLOSE,
							 GTK_RESPONSE_ACCEPT,
							 NULL);
	gtk_container_set_border_width
	    (GTK_CONTAINER(GTK_DIALOG(listeners_selector)->vbox), 6);

	align = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER
			  (GTK_DIALOG(listeners_selector)->vbox), align);
	gtk_container_set_border_width(GTK_CONTAINER(align), 6);
	gtk_widget_show(align);

	menu = gtk_menu_new();
	gtk_widget_show(menu);

	/* creating the menu items */

	world_listener_xml_uri = gnome_program_locate_file(NULL,
							   GNOME_FILE_DOMAIN_APP_DATADIR,
							   "girl/listeners.xml",
							   FALSE,
							   NULL);

	/* world_listener_xml_uri = g_strdup("http://girl.src.oka.no/listeners.xml"); */

	MSG("world_listener_xml_uri = %s\n",
	    world_listener_xml_uri);

	if (world_listener_xml_uri == NULL) {
		g_warning(("Failed to open %s.  Please install it.\n"),
			  world_listener_xml_uri);
	}

	local_listener_xml_file =
	    g_strconcat(g_get_home_dir(), "/.girl/listeners.xml", NULL);
	locallistener =
	    girl_listener_load_from_http(NULL, local_listener_xml_file);

	if (locallistener == NULL) {
		g_warning(("Failed to open %s\n"),
			  local_listener_xml_file);
	}

/*   g_free (local_listener_xml_file); */

	listenerinfo =
	    girl_listener_load_from_file(locallistener,
					 world_listener_xml_uri);

	/* girl_listeners = NULL; */

	while (listenerinfo != NULL) {

		label =
		    g_strconcat(listenerinfo->name, " (",
				listenerinfo->location, ")", NULL);
		listener_uri = g_strdup(listenerinfo->uri);
		listener_name = g_strdup(listenerinfo->name);
		listener_location = g_strdup(listenerinfo->location);
		listener_description = g_strdup(listenerinfo->description);

		/* girl_listeners = g_list_append(girl_listeners,(GirlListenerInfo *)listenerinfo); */

		if (label != NULL) {
			item = gtk_menu_item_new_with_label(label);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
			g_signal_connect(G_OBJECT(item), "activate",
					 G_CALLBACK
					 (on_listeners_selector_changed),
					 NULL);
			g_object_set_data(G_OBJECT(item), "listener_uri",
					  (gpointer) listener_uri);
			g_object_set_data(G_OBJECT(item), "listener_name",
					  (gpointer) listener_name);
			g_object_set_data(G_OBJECT(item),
					  "listener_location",
					  (gpointer) listener_location);
			g_object_set_data(G_OBJECT(item),
					  "listener_release",
					  (gpointer) listener_release);
			g_object_set_data(G_OBJECT(item),
					  "listener_description",
					  (gpointer) listener_description);
			gtk_widget_show(item);
			g_free(label);

			/* selection */

			printf("selected_listener_uri: %s\n",
			       selected_listener_uri);

			printf("listener_uri: %s\n", listener_uri);

			if (selected_listener_uri != NULL
			    && listener_uri != NULL
			    && !strcmp(selected_listener_uri,
				       listener_uri))
				selection = i;
		} else {
			g_free(listener_uri);
			g_free(listener_name);
			g_free(listener_location);
			g_free(listener_description);
		}
		i++;
		listenerinfo = listenerinfo->next;
	}

	drop_down = gtk_option_menu_new();
	gtk_widget_show(drop_down);
	gtk_option_menu_set_menu(drop_down, menu);
	gtk_container_add(GTK_CONTAINER(align), drop_down);

	if (selection != -1)
		gtk_option_menu_set_history(drop_down, selection);

	g_signal_connect(G_OBJECT(listeners_selector), "response",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) listeners_selector);
	g_signal_connect(G_OBJECT(listeners_selector), "delete-event",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) listeners_selector);

	return listeners_selector;
}

GtkWidget *create_stations_selector(char *selected_station_uri,
				    char *filename)
{
	GirlStationInfo *stationinfo, *localstation;
	GtkWidget *stations_selector;
	GtkWidget *align, *menu, *drop_down, *item;

	gchar *station_uri, *station_name, *station_location, *station_release, *station_description;
	gchar *label, *world_station_xml_filename, *local_station_xml_file;

	int i = 0, selection = -1;

	/* The Stations dialog */
	stations_selector = gtk_dialog_new_with_buttons(("Select a station"), GTK_WINDOW(girl_app), 0,	/* flags */
							GTK_STOCK_CLOSE,
							GTK_RESPONSE_ACCEPT,
							NULL);
	gtk_container_set_border_width
	    (GTK_CONTAINER(GTK_DIALOG(stations_selector)->vbox), 6);

	align = gtk_alignment_new(0.5, 0.5, 0, 0);
	gtk_container_add(GTK_CONTAINER
			  (GTK_DIALOG(stations_selector)->vbox), align);
	gtk_container_set_border_width(GTK_CONTAINER(align), 6);
	gtk_widget_show(align);

	menu = gtk_menu_new();
	gtk_widget_show(menu);

	/* creating the menu items */

	world_station_xml_filename = gnome_program_locate_file(NULL,
							       GNOME_FILE_DOMAIN_APP_DATADIR,
							       "girl/stations.xml",
							       FALSE,
							       NULL);
	/* world_station_xml_filename = g_strdup("http://girl.src.oka.no/stations.xml"); */

	MSG("world_station_xml_filename = %s\n",
	    world_station_xml_filename);

	if (world_station_xml_filename == NULL) {
		g_warning(("Failed to open %s.  Please install it.\n"),
			  world_station_xml_filename);
	}

	local_station_xml_file =
	    g_strconcat(g_get_home_dir(), "/.girl/stations.xml", NULL);
	localstation =
	    girl_station_load_from_file(NULL, local_station_xml_file);

	if (localstation == NULL) {
		g_warning(("Failed to open %s\n"), local_station_xml_file);
	}

/*   g_free (local_station_xml_file); */

	stationinfo =
	    girl_station_load_from_file(localstation,
					world_station_xml_filename);

	girl_stations = NULL;

	while (stationinfo != NULL) {

		label =
		    g_strconcat(stationinfo->name, " (",
				stationinfo->location, ")", NULL);
		station_uri = g_strdup(stationinfo->stream->uri);
		station_name = g_strdup(stationinfo->name);
		station_location = g_strdup(stationinfo->location);
		station_release = g_strdup(stationinfo->release);
		station_description = g_strdup(stationinfo->description);
		
		girl_stations = g_list_append(girl_stations,(GirlStationInfo *)stationinfo);

		if (label != NULL) {
			item = gtk_menu_item_new_with_label(label);
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), item);
			g_signal_connect(G_OBJECT(item), "activate",
					 G_CALLBACK
					 (on_stations_selector_changed),
					 NULL);
			g_object_set_data(G_OBJECT(item), "station_uri",
					  (gpointer) station_uri);
			g_object_set_data(G_OBJECT(item), "station_name",
					  (gpointer) station_name);
			g_object_set_data(G_OBJECT(item),
					  "station_location",
					  (gpointer) station_location);
			g_object_set_data(G_OBJECT(item),
					  "station_release",
					  (gpointer) station_release);
			g_object_set_data(G_OBJECT(item),
					  "station_description",
					  (gpointer) station_description);
			gtk_widget_show(item);
			g_free(label);

			/* selection */
			if (selected_station_uri != NULL &&
			    !strcmp(selected_station_uri, station_uri))
				selection = i;
		} else {
			g_free(station_uri);
			g_free(station_name);
			g_free(station_location);
			g_free(station_release);
			g_free(station_description);
		}
		i++;
		stationinfo = stationinfo->next;
	}

	drop_down = gtk_option_menu_new();
	gtk_widget_show(drop_down);
	gtk_option_menu_set_menu(drop_down, menu);
	gtk_container_add(GTK_CONTAINER(align), drop_down);

	if (selection != -1)
		gtk_option_menu_set_history(drop_down, selection);

	g_signal_connect(G_OBJECT(stations_selector), "response",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) stations_selector);
	g_signal_connect(G_OBJECT(stations_selector), "delete-event",
			 G_CALLBACK(gtk_widget_hide),
			 (gpointer) stations_selector);

	return stations_selector;
}

#if 0
static void station_print_selection(GtkWidget * list, gpointer func_data)
{
	GList *dlist;
	/*      dlist = GTK_LIST(list)->selection; */

	if (!dlist) {
		g_print("Selection cleared!\n");
		return;
	}

	g_print("The selection is a ");

	while (dlist) {

		GtkObject *list_item;
		gchar *item_data_string;

		list_item = dlist->data;
		item_data_string =
		    g_object_get_data(G_OBJECT(list_item),
				      "station_list_item_data");
		g_print("%s ", item_data_string);

		dlist = dlist->next;
	}
	g_print("\n");
}
#endif

GtkWidget *create_girl_app()
{
	GtkWidget *girl_app;
	GtkWidget *vbox1;

	GtkWidget *girl_pixmap;

	GtkWidget *appbar;
	
	GirlData *girl_data = g_new0(GirlData, 1);
	char *pmf;

	girl_app = gnome_app_new("girl", ("girl"));
	gtk_window_set_title(GTK_WINDOW(girl_app),
			     ("GNOME Internet Radio Locator"));

	vbox1 = gtk_vbox_new(FALSE, 0);
	gtk_widget_show(vbox1);
	gnome_app_set_contents(GNOME_APP(girl_app), vbox1);

	pmf = gnome_program_locate_file(NULL,
					GNOME_FILE_DOMAIN_APP_DATADIR,
					"girl/pixmaps/girl-map.png", FALSE, NULL);

	girl_pixmap = gtk_image_new_from_file(pmf);
	g_free(pmf);

	if (girl_pixmap == NULL)
		g_error(("Couldn't create pixmap"));

	gtk_widget_show(girl_pixmap);
	gtk_box_pack_start(GTK_BOX(vbox1), girl_pixmap, TRUE, TRUE, 0);

	appbar = gnome_appbar_new(FALSE, TRUE, FALSE);
	gnome_app_set_statusbar(GNOME_APP(girl_app), GTK_WIDGET(appbar));
	gnome_app_create_toolbar(GNOME_APP(girl_app), toolbar);

	girl_data->pixmap = GTK_IMAGE(girl_pixmap);
	girl_data->appbar = GNOME_APPBAR(appbar);

	gnome_appbar_push(girl_data->appbar,
			  ("Point on the map and click \"Listen\" to listen to the station."));

	/*    g_signal_connect(G_OBJECT(calendar), */
	/*                     "day_selected_double_click", */
	/*                     G_CALLBACK (on_listen_button_clicked), girl_data); */

	g_signal_connect(G_OBJECT(girl_app), "destroy",
			 G_CALLBACK(quit_app), girl_data);

	girl = girl_data;

#if GIRL_CFG
	gnome_config_push_prefix("/girl/General/");

	girl->selected_listener_uri =
	    gnome_config_get_string("selected_listener_uri=");
	girl->selected_listener_name =
	    gnome_config_get_string("selected_listener_name=");
	girl->selected_listener_location =
	    gnome_config_get_string("selected_listener_location=");
	girl->selected_listener_release =
	    gnome_config_get_string("selected_listener_release=");
	girl->selected_listener_description =
	    gnome_config_get_string("selected_listener_description=");

	printf("girl->selected_listener_uri: %s\n",
	       girl->selected_listener_uri);
	printf("girl->selected_listener_name: %s\n",
	       girl->selected_listener_name);
	printf("girl->selected_listener_location: %s\n",
	       girl->selected_listener_location);
	printf("girl->selected_listener_release: %s\n",
	       girl->selected_listener_release);
	printf("girl->selected_listener_description: %s\n",
	       girl->selected_listener_description);

	girl->selected_station_uri =
	    gnome_config_get_string("selected_station_uri=");
	girl->selected_station_name =
	    gnome_config_get_string("selected_station_name=");
	girl->selected_station_location =
	    gnome_config_get_string("selected_station_location=");
	girl->selected_station_release =
	    gnome_config_get_string("selected_station_release=");
	girl->selected_station_description =
	    gnome_config_get_string("selected_station_description=");

	printf("girl->selected_station_uri: %s\n",
	       girl->selected_station_uri);
	printf("girl->selected_station_name: %s\n",
	       girl->selected_station_name);
	printf("girl->selected_station_location: %s\n",
	       girl->selected_station_location);
	printf("girl->selected_station_release: %s\n",
	       girl->selected_station_release);
	printf("girl->selected_station_description: %s\n",
	       girl->selected_station_description);

	gnome_config_pop_prefix();
#endif

	return girl_app;
}

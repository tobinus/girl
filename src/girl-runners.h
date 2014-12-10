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

#ifndef GIRL_RUNNERS_H
#define GIRL_RUNNERS_H

#include "girl.h"

typedef struct _GirlRunnersInfo GirlRunnersInfo;

struct _GirlRunnersInfo {
	GtkWidget *widget;
	GMutex mutex;
	gchar *id;
	gchar *name;
	gchar *command;
};

GirlRunnersInfo *girl_runners_new (GirlRunnersInfo * head,
				   gchar *id,
				   gchar *name,
				   gchar *date,
				   gchar *time,
				   gchar *file);

void girl_runners_free(GirlRunnersInfo * info);

void girl_runners_mutex_init(GirlRunnersInfo *info, GMutex runner);
void girl_runners_mutex_lock(GirlRunnersInfo *info, GMutex runner);
void girl_runners_mutex_unlock(GirlRunnersInfo *info, GMutex mutex);
void girl_runners_mutex_exit(GirlRunnersInfo *info, GMutex mutex);

#endif /* GIRL_RUNNERS_H */

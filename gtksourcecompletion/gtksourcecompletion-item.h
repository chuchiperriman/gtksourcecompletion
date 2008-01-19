/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-item.h
 *
 *  Copyright (C) 2007 - Chuchiperriman <chuchiperriman@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#ifndef GTK_SOURCE_COMPLETION_ITEM_H
#define GTK_SOURCE_COMPLETION_ITEM_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef struct _GtkSourceCompletionItem GtkSourceCompletionItem;

#include "gtksourcecompletion-provider.h"

/* GtkSourceCompletionItem functions */
GtkSourceCompletionItem*
gtk_source_completion_item_new(int id,
																const gchar *name,
																const GdkPixbuf *icon,
																int priority,
																GtkSourceCompletionProvider *provider,
																gpointer user_data);

void
gtk_source_completion_item_free(GtkSourceCompletionItem *item);

const gchar*
gtk_source_completion_item_get_name(GtkSourceCompletionItem *item);

const GdkPixbuf*
gtk_source_completion_item_get_icon(GtkSourceCompletionItem *item);

gchar*
gtk_source_completion_item_get_user_data(GtkSourceCompletionItem *item);

GtkSourceCompletionProvider*
gtk_source_completion_item_get_provider(GtkSourceCompletionItem *item);

G_END_DECLS


#endif

/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-item.c
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

#include "gtksourcecompletion-item.h"

struct _GtkSourceCompletionItem
{
	int id;
	gchar *name;
	const GdkPixbuf *icon;
	int priority;
	GtkSourceCompletionProvider *provider;
	gpointer user_data;
	const gchar *page_name;
};

void
gtk_source_completion_item_free(GtkSourceCompletionItem *item)
{
	g_debug("Free GtkSourceCompletionItem");
	g_free(item->name);
	g_free(item);
}

GtkSourceCompletionItem*
gtk_source_completion_item_new(int id,
				const gchar *name,
				const GdkPixbuf *icon,
				int priority,
				GtkSourceCompletionProvider *provider,
				gpointer user_data)
{
	g_debug("Created GtkSourceCompletionItem");
	GtkSourceCompletionItem* item = g_malloc0(sizeof(GtkSourceCompletionItem));
	item->id = id;
	item->name = g_strdup(name);
	item->icon = icon;
	item->priority = priority;
	item->user_data = user_data;
	item->provider = provider;
	item->page_name = DEFAULT_PAGE;
	return item;
}

GtkSourceCompletionItem*
gtk_source_completion_item_new_full(int id,
				    const gchar *name,
				    const GdkPixbuf *icon,
				    int priority,
				    GtkSourceCompletionProvider *provider,
				    const gchar *page_name,
				    gpointer user_data)
{
	GtkSourceCompletionItem *item; 
	item = gtk_source_completion_item_new(id,name,icon,priority,provider,user_data);
	item->page_name = page_name;
	
	return item;
}

const gchar*
gtk_source_completion_item_get_name(GtkSourceCompletionItem *item)
{
	return item->name;
}

gpointer
gtk_source_completion_item_get_user_data(GtkSourceCompletionItem *item)
{
	return item->user_data;
}

const GdkPixbuf*
gtk_source_completion_item_get_icon(GtkSourceCompletionItem *item)
{
	return item->icon;
}

GtkSourceCompletionProvider*
gtk_source_completion_item_get_provider(GtkSourceCompletionItem *item)
{
	return item->provider;
}

const gchar*
gtk_source_completion_item_get_page_name(GtkSourceCompletionItem *item)
{
	return item->page_name;
}

int
gtk_source_completion_item_get_id(GtkSourceCompletionItem *item)
{
	return item->id;
}

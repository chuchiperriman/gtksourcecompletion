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

/**
 * gtk_source_completion_item_new:
 * @id: An id for identify this item
 * @name: Item name that will be shown in the completion popup
 * @icon: Item icon that will be shown in the completion popup
 * @priority: The item priority. Items with high priority will be
 * 				shown first in the completion popup
 * @provider: The provider that creates the item
 * @user_data: User data used by the providers
 *
 * Returns The new GtkSourceCompletionItem
 */
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

/**
 * gtk_source_completion_item_get_name:
 * @item: The GtkSourceCompletionItem
 *
 * Returns The item name
 */
const gchar*
gtk_source_completion_item_get_name(GtkSourceCompletionItem *item)
{
	return item->name;
}

/**
 * gtk_source_completion_item_get_user_data:
 * @item: The GtkSourceCompletionItem
 *
 * Returns the user data of this item
 */
gchar*
gtk_source_completion_item_get_user_data(GtkSourceCompletionItem *item)
{
	return item->user_data;
}

/**
 * gtk_source_completion_item_get_icon:
 * @item: The GtkSourceCompletionItem
 *
 * Returns the icon of this item
 */
const GdkPixbuf*
gtk_source_completion_item_get_icon(GtkSourceCompletionItem *item)
{
	return item->icon;
}

/**
 * gtk_source_completion_item_get_provider:
 * @item: The GtkSourceCompletionItem
 *
 * Returns the #GtkSourceCompletionProvider that did create the Item.
 */
GtkSourceCompletionProvider*
gtk_source_completion_item_get_provider(GtkSourceCompletionItem *item)
{
	return item->provider;
}

/**
 * gtk_source_completion_item_get_page_name:
 * @item: The GtkSourceCompletionItem
 *
 * Returns the page name where the item will be placed.
 */
const gchar*
gtk_source_completion_item_get_page_name(GtkSourceCompletionItem *item)
{
	return item->page_name;
}

/**
 * gtk_source_completion_item_get_id:
 * @item: The GtkSourceCompletionItem
 *
 * Returns current item id
 *
 */
int
gtk_source_completion_item_get_id(GtkSourceCompletionItem *item)
{
	return item->id;
}

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

#define DEFAULT_PAGE "Default"

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
																gpointer user_data);

/**
 * gtk_source_completion_item_new_full:
 * @id: An id for identify this item
 * @name: Item name that will be shown in the completion popup
 * @icon: Item icon that will be shown in the completion popup
 * @priority: The item priority. Items with high priority will be
 * 				shown first in the completion popup
 * @provider: The provider that creates the item
 * @page_name: The page name of this item. If NULL, the item will be shown 
 * in the default page.
 * @user_data: User data used by the providers
 *
 * Returns The new GtkSourceCompletionItem
 */
GtkSourceCompletionItem*
gtk_source_completion_item_new_full(int id,
							const gchar *name,
							const GdkPixbuf *icon,
							int priority,
							GtkSourceCompletionProvider *provider,
							const gchar *page_name,
							gpointer user_data);

/**
 * gtk_source_completion_item_free:
 * @item: The GtkSourceCompletionItem
 *
 * Frees the completion item.
 *
 */
void
gtk_source_completion_item_free(GtkSourceCompletionItem *item);

/**
 * gtk_source_completion_item_get_id:
 * @item: The GtkSourceCompletionItem
 *
 * Returns current item id
 *
 */
int
gtk_source_completion_item_get_id(GtkSourceCompletionItem *item);

/**
 * gtk_source_completion_item_get_name:
 * @item: The GtkSourceCompletionItem
 *
 * Returns The item name
 */
const gchar*
gtk_source_completion_item_get_name(GtkSourceCompletionItem *item);

/**
 * gtk_source_completion_item_get_icon:
 * @item: The GtkSourceCompletionItem
 *
 * Returns the icon of this item
 */
const GdkPixbuf*
gtk_source_completion_item_get_icon(GtkSourceCompletionItem *item);

/**
 * gtk_source_completion_item_get_user_data:
 * @item: The GtkSourceCompletionItem
 *
 * Returns the user data of this item
 */
gpointer
gtk_source_completion_item_get_user_data(GtkSourceCompletionItem *item);

/**
 * gtk_source_completion_item_get_provider:
 * @item: The GtkSourceCompletionItem
 *
 * Returns the #GtkSourceCompletionProvider that did create the Item.
 */
GtkSourceCompletionProvider*
gtk_source_completion_item_get_provider(GtkSourceCompletionItem *item);

/**
 * gtk_source_completion_item_get_page_name:
 * @item: The GtkSourceCompletionItem
 *
 * Returns the page name where the item will be placed.
 */
const gchar*
gtk_source_completion_item_get_page_name(GtkSourceCompletionItem *item);

G_END_DECLS


#endif

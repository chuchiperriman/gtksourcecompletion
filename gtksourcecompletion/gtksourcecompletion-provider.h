/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-provider.h
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
 
#ifndef __GTK_SOURCE_COMPLETION_PROVIDER_H__
#define __GTK_SOURCE_COMPLETION_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS


#define GTK_SOURCE_COMPLETION_TYPE_PROVIDER (gtk_source_completion_provider_get_type ())
#define GTK_SOURCE_COMPLETION_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_SOURCE_COMPLETION_TYPE_PROVIDER, GtkSourceCompletionProvider))
#define GTK_SOURCE_COMPLETION_IS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_SOURCE_COMPLETION_TYPE_PROVIDER))
#define GTK_SOURCE_COMPLETION_PROVIDER_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GTK_SOURCE_COMPLETION_TYPE_PROVIDER, GtkSourceCompletionProviderIface))

typedef struct _GtkSourceCompletionProvider GtkSourceCompletionProvider;
typedef struct _GtkSourceCompletionProviderIface GtkSourceCompletionProviderIface;

#include "gtksourcecompletion.h"
#include "gtksourcecompletion-trigger.h"
#include "gtksourcecompletion-item.h"

struct _GtkSourceCompletionProviderIface {
	
	GTypeInterface parent;

	const gchar* (*get_name) (GtkSourceCompletionProvider *self);

	GList* (*get_data) (GtkSourceCompletionProvider* self,
	 										GtkSourceCompletion* completion, 
	 										GtkSourceCompletionTrigger *trigger);
	void (*data_selected) (GtkSourceCompletionProvider* self,
											GtkSourceCompletion* completion, 
											GtkSourceCompletionItem* data);
											
	void (*end_completion) (GtkSourceCompletionProvider* self,
	 										GtkSourceCompletion* completion);
	 										
	gchar* (*get_item_info_markup) (GtkSourceCompletionProvider *self, 
							GtkSourceCompletionItem *item);

};

/**
 * gtk_source_completion_provider_get_name:
 * @self: the #GtkSourceCompletionProvider
 *
 * The provider name. By example: "Document word completion provider"
 *
 * Returns: The provider's name
 * 
 **/
const gchar*
gtk_source_completion_provider_get_name(GtkSourceCompletionProvider* self);

/**
 * gtk_source_completion_provider_get_data:
 * @self: the #GtkSourceCompletionProvider
 * @completion: The #GtkSourceCompletion.
 * @trigger: The #GtkSourceCompletionTrigger that raise the event
 *
 * The completion call this function when an event is raised.
 * This function may return a list of #GtkSourceCompletionItem to be shown
 * in the popup to the user.
 *
 * Returns: a list of #GtkSourceCompletionData or NULL if there are no items
 * 
 **/
GList* 
gtk_source_completion_provider_get_data (GtkSourceCompletionProvider* self, 
					GtkSourceCompletion* completion, 
					GtkSourceCompletionTrigger *trigger);

/**
 * gtk_source_completion_provider_data_selected:
 * @self: the #GtkSourceCompletionProvider
 * @view: The #GtkSourceCompletion.
 * @item: The data selected by the user.
 *
 * The completion call this function when the user select an item of this provider.
 * The provider may insert the text in the view of do something.
 *
 * 
 **/				
void 
gtk_source_completion_provider_data_selected (GtkSourceCompletionProvider* self, 
					GtkSourceCompletion *completion, 
					GtkSourceCompletionItem* item);

/**
 * gtk_source_completion_provider_end_completion:
 * @self: the #GtkSourceCompletionProvider
 * @view: The #GtkSourceCompletion.
 *
 * The completion call this function when it is goint to hide the popup
 * 
 **/					
void 
gtk_source_completion_provider_end_completion (GtkSourceCompletionProvider* self, 
					GtkSourceCompletion* completion);

/**
 * gtk_source_completion_provider_get_item_info_markup:
 * @self: the #GtkSourceCompletionProvider
 * @item: The selected item.
 *
 * The completion call this function when the user wants to view the 
 * aditional info of the current item.
 *
 * Returns: The pango markup to be shown in the info window.
 * 
 **/
gchar*
gtk_source_completion_provider_get_item_info_markup(GtkSourceCompletionProvider *self,
				GtkSourceCompletionItem *item);

GType 
gtk_source_completion_provider_get_type ();

G_END_DECLS

#endif

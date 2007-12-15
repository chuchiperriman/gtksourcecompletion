/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-provider.c
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

#include "gtksourcecompletion-provider.h"


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
gtk_source_completion_provider_get_name(GtkSourceCompletionProvider *self)
{
	return GTK_SOURCE_COMPLETION_PROVIDER_GET_INTERFACE (self)->get_name (self);
}
/**
 * gtk_source_completion_provider_get_data:
 * @self: the #GtkSourceCompletionProvider
 * @view: The #GtkTextView.
 * @event_name: The event name raised.
 * @event_data: Some data of the event. Every event have your own data.
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
					GtkTextView* view, 
					const gchar* event_name, 
					gpointer event_data)
{
	return GTK_SOURCE_COMPLETION_PROVIDER_GET_INTERFACE (self)->get_data (self, view, event_name, event_data);
}

/**
 * gtk_source_completion_provider_data_selected:
 * @self: the #GtkSourceCompletionProvider
 * @view: The #GtkTextView.
 * @item: The data selected by the user.
 *
 * The completion call this function when the user select an item of this provider.
 * The provider may insert the text in the view of do something.
 *
 * 
 **/
void 
gtk_source_completion_provider_data_selected (GtkSourceCompletionProvider* self, 
					GtkTextView* view, 
					GtkSourceCompletionItem* item)
{
	GTK_SOURCE_COMPLETION_PROVIDER_GET_INTERFACE (self)->data_selected (self, view, item);
}

/**
 * gtk_source_completion_provider_end_completion:
 * @self: the #GtkSourceCompletionProvider
 * @view: The #GtkTextView.
 *
 * The completion call this function when it is goint to hide the popup
 * 
 **/
void 
gtk_source_completion_provider_end_completion (GtkSourceCompletionProvider* self, 
					GtkTextView* view)
{
	GTK_SOURCE_COMPLETION_PROVIDER_GET_INTERFACE (self)->end_completion(self, view);
}

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
				GtkSourceCompletionItem *item)
{
	return GTK_SOURCE_COMPLETION_PROVIDER_GET_INTERFACE (self)->get_item_info_markup (self, item);
}

static void 
gtk_source_completion_provider_base_init (GtkSourceCompletionProviderIface * iface)
{
	static gboolean initialized = FALSE;
	if (!initialized) {
		initialized = TRUE;
	}
}


GType 
gtk_source_completion_provider_get_type ()
{
	static GType gtk_source_completion_provider_type_id = 0;
	if (!gtk_source_completion_provider_type_id) {
		static const GTypeInfo g_define_type_info = { 
							sizeof (GtkSourceCompletionProviderIface), 
							(GBaseInitFunc) gtk_source_completion_provider_base_init, 
							(GBaseFinalizeFunc) NULL, 
							(GClassInitFunc) NULL, 
							(GClassFinalizeFunc) NULL, 
							NULL, 
							0, 
							0, 
							(GInstanceInitFunc) NULL };
							
		gtk_source_completion_provider_type_id = 
				g_type_register_static (G_TYPE_INTERFACE, 
							"GtkSourceCompletionProvider", 
							&g_define_type_info, 
							0);
	}
	return gtk_source_completion_provider_type_id;
}





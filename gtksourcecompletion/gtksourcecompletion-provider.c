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

const gchar*
gtk_source_completion_provider_get_name(GtkSourceCompletionProvider *self)
{
	return GTK_SOURCE_COMPLETION_PROVIDER_GET_INTERFACE (self)->get_name (self);
}

GList* 
gtk_source_completion_provider_get_data (GtkSourceCompletionProvider* self,
					GtkSourceCompletion* completion, 
					GtkSourceCompletionTrigger *trigger)
{
	return GTK_SOURCE_COMPLETION_PROVIDER_GET_INTERFACE (self)->get_data (self, completion, trigger);
}

void 
gtk_source_completion_provider_data_selected (GtkSourceCompletionProvider* self, 
					GtkSourceCompletion* completion, 
					GtkSourceCompletionItem* item)
{
	GTK_SOURCE_COMPLETION_PROVIDER_GET_INTERFACE (self)->data_selected (self, completion, item);
}

void 
gtk_source_completion_provider_end_completion (GtkSourceCompletionProvider* self, 
					GtkSourceCompletion* completion)
{
	GTK_SOURCE_COMPLETION_PROVIDER_GET_INTERFACE (self)->end_completion(self, completion);
}

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





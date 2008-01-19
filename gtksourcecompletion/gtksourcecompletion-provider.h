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

const gchar*
gtk_source_completion_provider_get_name(GtkSourceCompletionProvider* self);

GList* 
gtk_source_completion_provider_get_data (GtkSourceCompletionProvider* self, 
					GtkSourceCompletion* completion, 
					GtkSourceCompletionTrigger *trigger);
					
void 
gtk_source_completion_provider_data_selected (GtkSourceCompletionProvider* self, 
					GtkSourceCompletion *completion, 
					GtkSourceCompletionItem* item);
					
void 
gtk_source_completion_provider_end_completion (GtkSourceCompletionProvider* self, 
					GtkSourceCompletion* completion);

gchar*
gtk_source_completion_provider_get_item_info_markup(GtkSourceCompletionProvider *self,
				GtkSourceCompletionItem *item);

GType 
gtk_source_completion_provider_get_type ();

G_END_DECLS

#endif

/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion.h
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

#ifndef _GTK_SOURCE_COMPLETION_H_
#define _GTK_SOURCE_COMPLETION_H_

#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

G_BEGIN_DECLS

#define GTK_TYPE_SOURCE_COMPLETION             (gtk_source_completion_get_type ())
#define GTK_SOURCE_COMPLETION(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_SOURCE_COMPLETION, GtkSourceCompletion))
#define GTK_SOURCE_COMPLETION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_SOURCE_COMPLETION, GtkSourceCompletionClass))
#define GTK_IS_SOURCE_COMPLETION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_SOURCE_COMPLETION))
#define GTK_IS_SOURCE_COMPLETION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_SOURCE_COMPLETION))
#define GTK_SOURCE_COMPLETION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_SOURCE_COMPLETION, GtkSourceCompletionClass))

typedef struct _GtkSourceCompletionPrivate GtkSourceCompletionPrivate;

typedef struct _GtkSourceCompletionClass GtkSourceCompletionClass;
typedef struct _GtkSourceCompletion GtkSourceCompletion;
typedef struct _GtkSourceCompletionItem GtkSourceCompletionItem;


#include "gtksourcecompletion-provider.h"
#include "gtksourcecompletion-trigger.h"

struct _GtkSourceCompletionClass
{
	GObjectClass parent_class;

	/* Signals */
	void(* populate_completion) (GtkSourceCompletion* completion);
};

struct _GtkSourceCompletion
{
	GObject parent_instance;
	GtkSourceCompletionPrivate *priv;
};

GType 
gtk_source_completion_get_type (void) G_GNUC_CONST;

GtkSourceCompletion* 
gtk_source_completion_new (GtkTextView *view);

void 
gtk_source_completion_trigger_event(GtkSourceCompletion *completion, 
					const gchar *event_name, 
					gpointer event_data);

void 
gtk_source_completion_register_provider(GtkSourceCompletion *completion, 
					GtkSourceCompletionProvider *provider);

void
gtk_source_completion_unregister_provider(GtkSourceCompletion *completion,
					GtkSourceCompletionProvider *provider);

GtkTextView* 
gtk_source_completion_get_view(GtkSourceCompletion *completion);

gboolean
gtk_source_completion_is_visible(GtkSourceCompletion *completion);

GtkSourceCompletion*
gtk_source_completion_get_from_view(
								GtkTextView *view);

GtkSourceCompletionProvider*
gtk_source_completion_get_provider(GtkSourceCompletion *completion,
								const gchar* provider_name);

void
gtk_source_completion_register_trigger(GtkSourceCompletion *completion,
								GtkSourceCompletionTrigger *trigger);
								
void
gtk_source_completion_unregister_trigger(GtkSourceCompletion *completion,
								GtkSourceCompletionTrigger *trigger);
								
GtkSourceCompletionTrigger*
gtk_source_completion_get_trigger(GtkSourceCompletion *completion,
								const gchar* trigger_name);

void
gtk_source_completion_activate(GtkSourceCompletion *completion);

void
gtk_source_completion_deactivate(GtkSourceCompletion *completion);

void
gtk_source_completion_finish_completion(GtkSourceCompletion *completion);

/* GtkSourceCompletionItem functions */
GtkSourceCompletionItem*
gtk_source_completion_item_new(int id,
																const gchar *name,
																const GdkPixbuf *icon,
																int priority,
																gpointer user_data);

gchar*
gtk_source_completion_item_get_name(GtkSourceCompletionItem *item);

gchar*
gtk_source_completion_item_get_user_data(GtkSourceCompletionItem *item);


G_END_DECLS

#endif /* _GTK_SOURCE_COMPLETION_H_ */

/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-trigger.h
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
 
#ifndef __GTK_SOURCE_COMPLETION_TRIGGER_H__
#define __GTK_SOURCE_COMPLETION_TRIGGER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS


#define GTK_TYPE_SOURCE_COMPLETION_TRIGGER (gtk_source_completion_trigger_get_type ())
#define GTK_SOURCE_COMPLETION_TRIGGER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_SOURCE_COMPLETION_TRIGGER, GtkSourceCompletionTrigger))
#define GTK_IS_SOURCE_COMPLETION_TRIGGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_SOURCE_COMPLETION_TRIGGER))
#define GTK_SOURCE_COMPLETION_TRIGGER_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GTK_TYPE_SOURCE_COMPLETION_TRIGGER, GtkSourceCompletionTriggerIface))

typedef struct _GtkSourceCompletionTrigger GtkSourceCompletionTrigger;
typedef struct _GtkSourceCompletionTriggerIface GtkSourceCompletionTriggerIface;

#include "gtksourcecompletion.h"

struct _GtkSourceCompletionTriggerIface {
	GTypeInterface parent;
	const gchar* (*get_name) (GtkSourceCompletionTrigger *self);
	gboolean (*activate) (GtkSourceCompletionTrigger* self);
	gboolean (*deactivate) (GtkSourceCompletionTrigger* self);
};

/**
 * gtk_source_completion_trigger_get_name:
 * @self: the #GtkSourceCompletionTrigger
 *
 * The trigger name. By example: "C autocompletion trigger"
 *
 * Returns: The trigger's name
 * 
 **/
const gchar*
gtk_source_completion_trigger_get_name(GtkSourceCompletionTrigger* self);

/**
 * gtk_source_completion_trigger_activate:
 * @self: the #GtkSourceCompletionTrigger
 *
 * Activate the completion trigger.
 *
 * Returns: TRUE if activation is OK, FALSE if not.
 * 
 **/
gboolean
gtk_source_completion_trigger_activate (GtkSourceCompletionTrigger* self);

/**
 * gtk_source_completion_trigger_deactivate:
 * @self: the #GtkSourceCompletionTrigger
 *
 * Deactive the completion trigger
 *
 * Returns: TRUE if activation is OK, FALSE if not.
 * 
 **/				
gboolean
gtk_source_completion_trigger_deactivate (GtkSourceCompletionTrigger* self);

GType 
gtk_source_completion_trigger_get_type ();


G_END_DECLS

#endif

/*
 * gtksourcecompletiontrigger.h
 * This file is part of gtksourcecompletion
 *
 * Copyright (C) 2007 -2009 Jesús Barbero Rodríguez <chuchiperriman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA.
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

struct _GtkSourceCompletionTriggerIface {
	GTypeInterface parent;
	
	const gchar* (*get_name)   (GtkSourceCompletionTrigger *self);
	gboolean     (*activate)   (GtkSourceCompletionTrigger* self);
	gboolean     (*deactivate) (GtkSourceCompletionTrigger* self);
};

GType		 gtk_source_completion_trigger_get_type		(void);

const gchar	*gtk_source_completion_trigger_get_name		(GtkSourceCompletionTrigger *self);

gboolean	 gtk_source_completion_trigger_activate		(GtkSourceCompletionTrigger *self);
				
gboolean	 gtk_source_completion_trigger_deactivate	(GtkSourceCompletionTrigger *self);

G_END_DECLS

#endif

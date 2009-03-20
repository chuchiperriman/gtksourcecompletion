/*
 * gtksourcecompletiontrigger-customkey.h
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
 
#ifndef GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY_H__
#define GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY_H__

#include <glib.h>
#include <glib-object.h>
#include "gtksourcecompletiontrigger.h"
#include "gtksourcecompletion.h"

G_BEGIN_DECLS

#define GTK_TYPE_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY (gtk_source_completion_trigger_customkey_get_type ())
#define GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY, GtkSourceCompletionTriggerCustomkey))
#define GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY, GtkSourceCompletionTriggerCustomkeyClass))
#define GTK_IS_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY))
#define GTK_IS_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY))
#define GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY, GtkSourceCompletionTriggerCustomkeyClass))

typedef struct _GtkSourceCompletionTriggerCustomkey GtkSourceCompletionTriggerCustomkey;
typedef struct _GtkSourceCompletionTriggerCustomkeyClass GtkSourceCompletionTriggerCustomkeyClass;
typedef struct _GtkSourceCompletionTriggerCustomkeyPrivate GtkSourceCompletionTriggerCustomkeyPrivate;

struct _GtkSourceCompletionTriggerCustomkey
{
	GObject parent;
	
	GtkSourceCompletionTriggerCustomkeyPrivate *priv;
};

struct _GtkSourceCompletionTriggerCustomkeyClass
{
	GObjectClass parent;
};

GType		 gtk_source_completion_trigger_customkey_get_type	(void) G_GNUC_CONST;

GtkSourceCompletionTriggerCustomkey
		*gtk_source_completion_trigger_customkey_new		(GtkSourceCompletion *completion,
									 const gchar *trigger_name,
									 const gchar *keys);

void		 gtk_source_completion_trigger_customkey_set_keys	(GtkSourceCompletionTriggerCustomkey *self,
									 const gchar *keys);

G_END_DECLS

#endif

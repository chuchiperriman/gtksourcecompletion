/*
 * ##(FILENAME) - ##(DESCRIPTION)
 *
 * Copyright (C) ##(DATE_YEAR) - ##(AUTHOR_FULLNAME)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __##(PLUGIN_ID.upper)_TRIGGER_H__
#define __##(PLUGIN_ID.upper)_TRIGGER_H__

#include <glib.h>
#include <glib-object.h>
#include "gtksourcecompletion-trigger.h"
#include "gtksourcecompletion.h"

G_BEGIN_DECLS

#define TYPE_GSC_##(PLUGIN_ID.upper)_TRIGGER (gsc_##(PLUGIN_ID.lower)_trigger_get_type ())
#define GSC_##(PLUGIN_ID.upper)_TRIGGER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_GSC_##(PLUGIN_ID.upper)_TRIGGER, Gsc##(PLUGIN_ID.camel)Trigger))
#define GSC_##(PLUGIN_ID.upper)_TRIGGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GSC_##(PLUGIN_ID.upper)_TRIGGER, Gsc##(PLUGIN_ID.camel)TriggerClass))
#define IS_GSC_##(PLUGIN_ID.upper)_TRIGGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_GSC_##(PLUGIN_ID.upper)_TRIGGER))
#define IS_GSC_##(PLUGIN_ID.upper)_TRIGGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_GSC_##(PLUGIN_ID.upper)_TRIGGER))
#define GSC_##(PLUGIN_ID.upper)_TRIGGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GSC_##(PLUGIN_ID.upper)_TRIGGER, Gsc##(PLUGIN_ID.camel)TriggerClass))

#define GSC_##(PLUGIN_ID.upper)_TRIGGER_NAME "Gsc##(PLUGIN_ID.camel)Trigger"

typedef struct _Gsc##(PLUGIN_ID.camel)Trigger Gsc##(PLUGIN_ID.camel)Trigger;
typedef struct _Gsc##(PLUGIN_ID.camel)TriggerClass Gsc##(PLUGIN_ID.camel)TriggerClass;
typedef struct _Gsc##(PLUGIN_ID.camel)TriggerPrivate Gsc##(PLUGIN_ID.camel)TriggerPrivate;

struct _Gsc##(PLUGIN_ID.camel)Trigger {
	GObject parent;
	Gsc##(PLUGIN_ID.camel)TriggerPrivate *priv;	
};

struct _Gsc##(PLUGIN_ID.camel)TriggerClass {
	GObjectClass parent;
};

Gsc##(PLUGIN_ID.camel)Trigger* 
gsc_##(PLUGIN_ID.lower)_trigger_new(GtkSourceCompletion *completion);

GType gsc_##(PLUGIN_ID.lower)_trigger_get_type ();

G_END_DECLS

#endif

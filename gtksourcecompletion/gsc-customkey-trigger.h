/*
 * gsc-customkey-trigger.h - Type here a short description of your trigger
 *
 * Copyright (C) 2008 - perriman
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

#ifndef __CUSTOMKEY_TRIGGER_H__
#define __CUSTOMKEY_TRIGGER_H__

#include <glib.h>
#include <glib-object.h>
#include "gtksourcecompletion-trigger.h"
#include "gtksourcecompletion.h"

G_BEGIN_DECLS

#define TYPE_GSC_CUSTOMKEY_TRIGGER (gsc_customkey_trigger_get_type ())
#define GSC_CUSTOMKEY_TRIGGER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_GSC_CUSTOMKEY_TRIGGER, GscCustomkeyTrigger))
#define GSC_CUSTOMKEY_TRIGGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GSC_CUSTOMKEY_TRIGGER, GscCustomkeyTriggerClass))
#define IS_GSC_CUSTOMKEY_TRIGGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_GSC_CUSTOMKEY_TRIGGER))
#define IS_GSC_CUSTOMKEY_TRIGGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_GSC_CUSTOMKEY_TRIGGER))
#define GSC_CUSTOMKEY_TRIGGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GSC_CUSTOMKEY_TRIGGER, GscCustomkeyTriggerClass))

typedef struct _GscCustomkeyTrigger GscCustomkeyTrigger;
typedef struct _GscCustomkeyTriggerClass GscCustomkeyTriggerClass;
typedef struct _GscCustomkeyTriggerPrivate GscCustomkeyTriggerPrivate;

struct _GscCustomkeyTrigger {
	GObject parent;
	GscCustomkeyTriggerPrivate *priv;	
};

struct _GscCustomkeyTriggerClass {
	GObjectClass parent;
};

/**
 * gsc_customkey_trigger_new:
 * @completion: The #GtkSourceCompletion
 * @trigger_name: The trigger name wich will be user the we trigger the event.
 * @keys: The string representation of the keys that we will
 * use to activate the event. You can get this 
 * string with #gtk_accelerator_name
 *
 * This is a generic trigger. You tell the name and the key and the trigger
 * will be triggered when the user press this key (or keys)
 *
 * Returns The new #GscCustomkeyTrigger
 *
 */
GscCustomkeyTrigger* 
gsc_customkey_trigger_new(GtkSourceCompletion *completion,
			  const gchar* trigger_name, 
			  const gchar* keys);

GType 
gsc_customkey_trigger_get_type ();

G_END_DECLS

#endif

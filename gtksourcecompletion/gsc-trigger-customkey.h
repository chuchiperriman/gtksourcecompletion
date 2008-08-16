/*
 * gsc-trigger-customkey.h - Type here a short description of your trigger
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

#ifndef GSC_TRIGGER_CUSTOMKEY_H__
#define GSC_TRIGGER_CUSTOMKEY_H__

#include <glib.h>
#include <glib-object.h>
#include "gsc-trigger.h"
#include "gsc-manager.h"

G_BEGIN_DECLS

#define GSC_TYPE_TRIGGER_CUSTOMKEY (gsc_trigger_customkey_get_type ())
#define GSC_TRIGGER_CUSTOMKEY(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_TRIGGER_CUSTOMKEY, GscTriggerCustomkey))
#define GSC_TRIGGER_CUSTOMKEY_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_TRIGGER_CUSTOMKEY, GscTriggerCustomkeyClass))
#define GSC_IS_TRIGGER_CUSTOMKEY(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_TRIGGER_CUSTOMKEY))
#define GSC_IS_TRIGGER_CUSTOMKEY_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_TRIGGER_CUSTOMKEY))
#define GSC_TRIGGER_CUSTOMKEY_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_TRIGGER_CUSTOMKEY, GscTriggerCustomkeyClass))

typedef struct _GscTriggerCustomkey GscTriggerCustomkey;
typedef struct _GscTriggerCustomkeyClass GscTriggerCustomkeyClass;
typedef struct _GscTriggerCustomkeyPrivate GscTriggerCustomkeyPrivate;

struct _GscTriggerCustomkey {
	GObject parent;
	GscTriggerCustomkeyPrivate *priv;	
};

struct _GscTriggerCustomkeyClass {
	GObjectClass parent;
};

/**
 * gsc_trigger_customkey_new:
 * @completion: The #GscManager
 * @trigger_name: The trigger name wich will be user the we trigger the event.
 * @keys: The string representation of the keys that we will
 * use to activate the event. You can get this 
 * string with #gtk_accelerator_name
 *
 * This is a generic trigger. You tell the name and the key and the trigger
 * will be triggered when the user press this key (or keys)
 *
 * Returns The new #GscTriggerCustomkey
 *
 */
GscTriggerCustomkey* 
gsc_trigger_customkey_new(GscManager *completion,
			  const gchar* trigger_name, 
			  const gchar* keys);

/**
 * gsc_trigger_customkey_set_keys:
 * @self: The #GscTriggerCustomkey 
 * @keys: The string representation of the keys that we will
 * use to activate the user request event. You can get this 
 * string with #gtk_accelerator_name
 *
 * Assign the keys that we will use to activate the user request event
 */
void
gsc_trigger_customkey_set_keys(GscTriggerCustomkey * self, 
				 const gchar* keys);

/**
 * gsc_trigger_customkey_set_opts:
 * @self: The #GscTriggerCustomkey
 * @options: The trigger options
 *
 * Use this function if you want to tell the completion how must
 * show the popup (position, filter, etc.) then this trigger occurs
 * 
 **/
void
gsc_trigger_customkey_set_opts(GscTriggerCustomkey *self,
				GscManagerEventOptions *options);

GType 
gsc_trigger_customkey_get_type ();

G_END_DECLS

#endif

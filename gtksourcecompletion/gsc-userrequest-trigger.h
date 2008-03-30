/* Copyright (C) 2007 - Jesús Barbero <chuchiperriman@gmail.com>
 *  *
 *  This file is part of gtksourcecompletion.
 *
 *  gtksourcecompletion is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  gtksourcecompletion is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __USERREQUEST_TRIGGER_H__
#define __USERREQUEST_TRIGGER_H__

#include <glib.h>
#include <glib-object.h>
#include "gtksourcecompletion-trigger.h"
#include "gtksourcecompletion.h"

G_BEGIN_DECLS

#define TYPE_GSC_USERREQUEST_TRIGGER (gsc_userrequest_trigger_get_type ())
#define GSC_USERREQUEST_TRIGGER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_GSC_USERREQUEST_TRIGGER, GscUserRequestTrigger))
#define GSC_USERREQUEST_TRIGGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GSC_USERREQUEST_TRIGGER, GscUserRequestTriggerClass))
#define IS_GSC_USERREQUEST_TRIGGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_GSC_USERREQUEST_TRIGGER))
#define IS_GSC_USERREQUEST_TRIGGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_GSC_USERREQUEST_TRIGGER))
#define GSC_USERREQUEST_TRIGGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GSC_USERREQUEST_TRIGGER, GscUserRequestTriggerClass))

#define GSC_USERREQUEST_TRIGGER_NAME "GscUserRequestTrigger"

typedef struct _GscUserRequestTrigger GscUserRequestTrigger;
typedef struct _GscUserRequestTriggerClass GscUserRequestTriggerClass;
typedef struct _GscUserRequestTriggerPrivate GscUserRequestTriggerPrivate;

struct _GscUserRequestTrigger {
	GObject parent;
	GscUserRequestTriggerPrivate *priv;	
};

struct _GscUserRequestTriggerClass {
	GObjectClass parent;
};

/**
 * gsc_userrequest_trigger_new:
 *
 * Returns The new #GscUserRequestTrigger
 *
 */
GscUserRequestTrigger* 
gsc_userrequest_trigger_new();

/**
 * gsc_userrequest_trigger_set_keys:
 * @self: The #GscUserRequestTrigger 
 * @keys: The string representation of the keys that we will
 * use to activate the user request event. You can get this 
 * string with #gtk_accelerator_name
 *
 * Assign the keys that we will use to activate the user request event
 */
void
gsc_userrequest_trigger_set_keys(GscUserRequestTrigger * self, const gchar* keys);

GType gsc_userrequest_trigger_get_type ();

G_END_DECLS

#endif

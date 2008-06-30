/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-trigger.h
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
 
#ifndef __GSC_TRIGGER_H__
#define __GSC_TRIGGER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS


#define GSC_TYPE_TRIGGER (gsc_trigger_get_type ())
#define GSC_TRIGGER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_TRIGGER, GscTrigger))
#define GSC_IS_TRIGGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_TRIGGER))
#define GSC_TRIGGER_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GSC_TYPE_TRIGGER, GscTriggerIface))

typedef struct _GscTrigger GscTrigger;
typedef struct _GscTriggerIface GscTriggerIface;

struct _GscTriggerIface {
	GTypeInterface parent;
	const gchar* (*get_name) (GscTrigger *self);
	gboolean (*activate) (GscTrigger* self);
	gboolean (*deactivate) (GscTrigger* self);
};

/**
 * gsc_trigger_get_name:
 * @self: the #GscTrigger
 *
 * The trigger name. By example: "C autocompletion trigger"
 *
 * Returns: The trigger's name
 * 
 **/
const gchar*
gsc_trigger_get_name(GscTrigger* self);

/**
 * gsc_trigger_activate:
 * @self: the #GscTrigger
 *
 * Activate the completion trigger.
 *
 * Returns: TRUE if activation is OK, FALSE if not.
 * 
 **/
gboolean
gsc_trigger_activate (GscTrigger* self);

/**
 * gsc_trigger_deactivate:
 * @self: the #GscTrigger
 *
 * Deactive the completion trigger
 *
 * Returns: TRUE if activation is OK, FALSE if not.
 * 
 **/				
gboolean
gsc_trigger_deactivate (GscTrigger* self);

GType 
gsc_trigger_get_type ();


G_END_DECLS

#endif

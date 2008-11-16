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

#ifndef __TRIGGER_##(PLUGIN_ID.upper)_H__
#define __TRIGGER_##(PLUGIN_ID.upper)_H__

#include <glib.h>
#include <glib-object.h>
#include <gtksourcecompletion/gsc-trigger.h>
#include <gtksourcecompletion/gsc-manager.h>

G_BEGIN_DECLS

#define GSC_TYPE_TRIGGER_##(PLUGIN_ID.upper) (gsc_trigger_##(PLUGIN_ID.lower)_get_type ())
#define GSC_TRIGGER_##(PLUGIN_ID.upper)(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_TRIGGER_##(PLUGIN_ID.upper), GscTrigger##(PLUGIN_ID.camel)))
#define GSC_TRIGGER_##(PLUGIN_ID.upper)_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_TRIGGER_##(PLUGIN_ID.upper), GscTrigger##(PLUGIN_ID.camel)Class))
#define GSC_IS_TRIGGER_##(PLUGIN_ID.upper)(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_TRIGGER_##(PLUGIN_ID.upper)))
#define GSC_IS_TRIGGER_##(PLUGIN_ID.upper)_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_TRIGGER_##(PLUGIN_ID.upper)))
#define GSC_TRIGGER_##(PLUGIN_ID.upper)_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_TRIGGER_##(PLUGIN_ID.upper), GscTrigger##(PLUGIN_ID.camel)Class))

#define GSC_TRIGGER_##(PLUGIN_ID.upper)_NAME "GscTrigger##(PLUGIN_ID.camel)"

typedef struct _GscTrigger##(PLUGIN_ID.camel) GscTrigger##(PLUGIN_ID.camel);
typedef struct _GscTrigger##(PLUGIN_ID.camel)Class GscTrigger##(PLUGIN_ID.camel)Class;
typedef struct _GscTrigger##(PLUGIN_ID.camel)Private GscTrigger##(PLUGIN_ID.camel)Private;

struct _GscTrigger##(PLUGIN_ID.camel) {
	GObject parent;
	GscTrigger##(PLUGIN_ID.camel)Private *priv;	
};

struct _GscTrigger##(PLUGIN_ID.camel)Class {
	GObjectClass parent;
};

GscTrigger##(PLUGIN_ID.camel)* 
gsc_trigger_##(PLUGIN_ID.lower)_new(GscManager *completion);

GType gsc_trigger_##(PLUGIN_ID.lower)_get_type ();

G_END_DECLS

#endif

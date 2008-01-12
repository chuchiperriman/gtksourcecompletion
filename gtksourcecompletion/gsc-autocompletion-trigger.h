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
#ifndef __AUTOCOMPLETION_TRIGGER_H__
#define __AUTOCOMPLETION_TRIGGER_H__

#include <glib.h>
#include <glib-object.h>
#include "gtksourcecompletion-trigger.h"
#include "gtksourcecompletion.h"

G_BEGIN_DECLS

#define TYPE_GSC_AUTOCOMPLETION_TRIGGER (gsc_autocompletion_trigger_get_type ())
#define GSC_AUTOCOMPLETION_TRIGGER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_GSC_AUTOCOMPLETION_TRIGGER, GscAutocompletionTrigger))
#define GSC_AUTOCOMPLETION_TRIGGER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GSC_AUTOCOMPLETION_TRIGGER, GscAutocompletionTriggerClass))
#define IS_GSC_AUTOCOMPLETION_TRIGGER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_GSC_AUTOCOMPLETION_TRIGGER))
#define IS_GSC_AUTOCOMPLETION_TRIGGER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_GSC_AUTOCOMPLETION_TRIGGER))
#define GSC_AUTOCOMPLETION_TRIGGER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GSC_AUTOCOMPLETION_TRIGGER, GscAutocompletionTriggerClass))

#define GSC_AUTOCOMPLETION_TRIGGER_NAME "GscAutocompletionTrigger"

typedef struct _GscAutocompletionTrigger GscAutocompletionTrigger;
typedef struct _GscAutocompletionTriggerClass GscAutocompletionTriggerClass;
typedef struct _GscAutocompletionTriggerPrivate GscAutocompletionTriggerPrivate;

struct _GscAutocompletionTrigger {
	GObject parent;
	GscAutocompletionTriggerPrivate *priv;	
};

struct _GscAutocompletionTriggerClass {
	GObjectClass parent;
};

GscAutocompletionTrigger* 
gsc_autocompletion_trigger_new();

void
gsc_autocompletion_trigger_set_delay(GscAutocompletionTrigger* trigger, guint delay);

GType gsc_autocompletion_trigger_get_type ();

G_END_DECLS

#endif

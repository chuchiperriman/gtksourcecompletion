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
#ifndef __GSC_TRIGGER_AUTOWORDS_H__
#define __GSC_TRIGGER_AUTOWORDS_H__

#include <glib.h>
#include <glib-object.h>
#include "gsc-trigger.h"
#include "gsc-manager.h"

G_BEGIN_DECLS

#define GSC_TYPE_TRIGGER_AUTOWORDS (gsc_trigger_autowords_get_type ())
#define GSC_TRIGGER_AUTOWORDS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_TRIGGER_AUTOWORDS, GscTriggerAutowords))
#define GSC_TRIGGER_AUTOWORDS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_TRIGGER_AUTOWORDS, GscTriggerAutowordsClass))
#define GSC_IS_TRIGGER_AUTOWORDS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_TRIGGER_AUTOWORDS))
#define GSC_IS_TRIGGER_AUTOWORDS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_TRIGGER_AUTOWORDS))
#define GSC_TRIGGER_AUTOWORDS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_TRIGGER_AUTOWORDS, GscTriggerAutowordsClass))

/*
 * FIXME: Is this define really needed here?
 */
#define GSC_TRIGGER_AUTOWORDS_NAME "GscTriggerAutowords"

typedef struct _GscTriggerAutowords GscTriggerAutowords;
typedef struct _GscTriggerAutowordsClass GscTriggerAutowordsClass;
typedef struct _GscTriggerAutowordsPrivate GscTriggerAutowordsPrivate;

struct _GscTriggerAutowords
{
	GObject parent;
	
	GscTriggerAutowordsPrivate *priv;
};

struct _GscTriggerAutowordsClass
{
	GObjectClass parent;
};

GType			 gsc_trigger_autowords_get_type		(void);

GscTriggerAutowords	*gsc_trigger_autowords_new		(GscManager *completion);

void			 gsc_trigger_autowords_set_delay	(GscTriggerAutowords* trigger,
								 guint delay);

G_END_DECLS

#endif

/*
 * gsc-trigger-dotexample.h - Type here a short description of your trigger
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

#ifndef __TRIGGER_DOTEXAMPLE_H__
#define __TRIGGER_DOTEXAMPLE_H__

#include <glib.h>
#include <glib-object.h>
#include <gtksourcecompletion/gsc-trigger.h>
#include <gtksourcecompletion/gsc-manager.h>

G_BEGIN_DECLS

#define GSC_TYPE_TRIGGER_DOTEXAMPLE (gsc_trigger_dotexample_get_type ())
#define GSC_TRIGGER_DOTEXAMPLE(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_TRIGGER_DOTEXAMPLE, GscTriggerDotexample))
#define GSC_TRIGGER_DOTEXAMPLE_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_TRIGGER_DOTEXAMPLE, GscTriggerDotexampleClass))
#define GSC_IS_TRIGGER_DOTEXAMPLE(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_TRIGGER_DOTEXAMPLE))
#define GSC_IS_TRIGGER_DOTEXAMPLE_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_TRIGGER_DOTEXAMPLE))
#define GSC_TRIGGER_DOTEXAMPLE_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_TRIGGER_DOTEXAMPLE, GscTriggerDotexampleClass))

#define GSC_TRIGGER_DOTEXAMPLE_NAME "GscTriggerDotexample"

typedef struct _GscTriggerDotexample GscTriggerDotexample;
typedef struct _GscTriggerDotexampleClass GscTriggerDotexampleClass;
typedef struct _GscTriggerDotexamplePrivate GscTriggerDotexamplePrivate;

struct _GscTriggerDotexample {
	GObject parent;
	GscTriggerDotexamplePrivate *priv;	
};

struct _GscTriggerDotexampleClass {
	GObjectClass parent;
};

GscTriggerDotexample* 
gsc_trigger_dotexample_new(GscManager *completion);

GType gsc_trigger_dotexample_get_type ();

G_END_DECLS

#endif

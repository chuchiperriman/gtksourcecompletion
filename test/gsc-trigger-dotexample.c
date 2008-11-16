 /* gsc-trigger-dotexample.c - Type here a short description of your trigger
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

#include <glib/gprintf.h>
#include <string.h>
#include <ctype.h>
#include "gsc-trigger-dotexample.h"

struct _GscTriggerDotexamplePrivate {
	GscManager *completion;
};

#define GSC_TRIGGER_DOTEXAMPLE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GSC_TYPE_TRIGGER_DOTEXAMPLE, GscTriggerDotexamplePrivate))

enum  {
	GSC_TRIGGER_DOTEXAMPLE_DUMMY_PROPERTY,
};

static const gchar* gsc_trigger_dotexample_real_get_name(GscTrigger* base);
static gboolean gsc_trigger_dotexample_real_activate (GscTrigger* base);
static gboolean gsc_trigger_dotexample_real_deactivate (GscTrigger* base);

static gpointer gsc_trigger_dotexample_parent_class = NULL;
static GscTriggerIface* gsc_trigger_dotexample_parent_iface = NULL;


static const gchar* gsc_trigger_dotexample_real_get_name(GscTrigger *self)
{
	return GSC_TRIGGER_DOTEXAMPLE_NAME;
}

static gboolean
gsc_trigger_dotexample_real_activate (GscTrigger* base)
{
	g_debug("Activating Dotexample trigger");
	GscTriggerDotexample *self = GSC_TRIGGER_DOTEXAMPLE(base);

	return TRUE;
}

static gboolean
gsc_trigger_dotexample_real_deactivate (GscTrigger* base)
{
	g_debug("Deactivating Dotexample trigger");
	GscTriggerDotexample *self = GSC_TRIGGER_DOTEXAMPLE(base);
	return FALSE;
}

static void gsc_trigger_dotexample_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec)
{
}


static void gsc_trigger_dotexample_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec)
{
}

static void gsc_trigger_dotexample_init (GscTriggerDotexample * self)
{
	self->priv = g_new0(GscTriggerDotexamplePrivate, 1);
	g_debug("Init Dotexample trigger");
}

static void gsc_trigger_dotexample_finalize(GObject *object)
{
	g_debug("Finish Dotexample trigger");
	GscTriggerDotexample *self;
	self = GSC_TRIGGER_DOTEXAMPLE(object);
	G_OBJECT_CLASS(gsc_trigger_dotexample_parent_class)->finalize(object);
}

static void gsc_trigger_dotexample_class_init (GscTriggerDotexampleClass * klass)
{
	gsc_trigger_dotexample_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->get_property = gsc_trigger_dotexample_get_property;
	G_OBJECT_CLASS (klass)->set_property = gsc_trigger_dotexample_set_property;
	G_OBJECT_CLASS (klass)->finalize = gsc_trigger_dotexample_finalize;
}

static void gsc_trigger_dotexample_interface_init (GscTriggerIface * iface)
{
	gsc_trigger_dotexample_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_name = gsc_trigger_dotexample_real_get_name;
	iface->activate = gsc_trigger_dotexample_real_activate;
	iface->deactivate = gsc_trigger_dotexample_real_deactivate;
}

GType gsc_trigger_dotexample_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (GscTriggerDotexampleClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gsc_trigger_dotexample_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GscTriggerDotexample), 0, (GInstanceInitFunc) gsc_trigger_dotexample_init };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "GscTriggerDotexample", &g_define_type_info, 0);
		static const GInterfaceInfo gsc_trigger_dotexample_info = { (GInterfaceInitFunc) gsc_trigger_dotexample_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		g_type_add_interface_static (g_define_type_id, GSC_TYPE_TRIGGER, &gsc_trigger_dotexample_info);
	}
	return g_define_type_id;
}

/**
 * gsc_trigger_dotexample_new:
 *
 * Returns The new #GscTriggerDotexample
 *
 */
GscTriggerDotexample*
gsc_trigger_dotexample_new(GscManager *completion)
{
	GscTriggerDotexample *self = GSC_TRIGGER_DOTEXAMPLE (g_object_new (GSC_TYPE_TRIGGER_DOTEXAMPLE, NULL));
	self->priv->completion = completion;
	return self;
}


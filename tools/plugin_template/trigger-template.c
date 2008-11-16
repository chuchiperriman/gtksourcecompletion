 /* ##(FILENAME) - ##(DESCRIPTION)
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

#include <glib/gprintf.h>
#include <string.h>
#include <ctype.h>
#include "gsc-trigger-##(PLUGIN_ID.lower).h"

struct _GscTrigger##(PLUGIN_ID.camel)Private {
	GscManager *completion;
};

#define GSC_TRIGGER_##(PLUGIN_ID.upper)_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GSC_TYPE_TRIGGER_##(PLUGIN_ID.upper), GscTrigger##(PLUGIN_ID.camel)Private))

enum  {
	GSC_TRIGGER_##(PLUGIN_ID.upper)_DUMMY_PROPERTY,
};

static const gchar* gsc_trigger_##(PLUGIN_ID.lower)_real_get_name(GscTrigger* base);
static gboolean gsc_trigger_##(PLUGIN_ID.lower)_real_activate (GscTrigger* base);
static gboolean gsc_trigger_##(PLUGIN_ID.lower)_real_deactivate (GscTrigger* base);

static gpointer gsc_trigger_##(PLUGIN_ID.lower)_parent_class = NULL;
static GscTriggerIface* gsc_trigger_##(PLUGIN_ID.lower)_parent_iface = NULL;


static const gchar* gsc_trigger_##(PLUGIN_ID.lower)_real_get_name(GscTrigger *self)
{
	return GSC_TRIGGER_##(PLUGIN_ID.upper)_NAME;
}

static gboolean
gsc_trigger_##(PLUGIN_ID.lower)_real_activate (GscTrigger* base)
{
	g_debug("Activating ##(PLUGIN_ID.camel) trigger");
	GscTrigger##(PLUGIN_ID.camel) *self = GSC_TRIGGER_##(PLUGIN_ID.upper)(base);

	return TRUE;
}

static gboolean
gsc_trigger_##(PLUGIN_ID.lower)_real_deactivate (GscTrigger* base)
{
	g_debug("Deactivating ##(PLUGIN_ID.camel) trigger");
	GscTrigger##(PLUGIN_ID.camel) *self = GSC_TRIGGER_##(PLUGIN_ID.upper)(base);
	return FALSE;
}

static void gsc_trigger_##(PLUGIN_ID.lower)_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec)
{
}


static void gsc_trigger_##(PLUGIN_ID.lower)_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec)
{
}

static void gsc_trigger_##(PLUGIN_ID.lower)_init (GscTrigger##(PLUGIN_ID.camel) * self)
{
	self->priv = g_new0(GscTrigger##(PLUGIN_ID.camel)Private, 1);
	g_debug("Init ##(PLUGIN_ID.camel) trigger");
}

static void gsc_trigger_##(PLUGIN_ID.lower)_finalize(GObject *object)
{
	g_debug("Finish ##(PLUGIN_ID.camel) trigger");
	GscTrigger##(PLUGIN_ID.camel) *self;
	self = GSC_TRIGGER_##(PLUGIN_ID.upper)(object);
	G_OBJECT_CLASS(gsc_trigger_##(PLUGIN_ID.lower)_parent_class)->finalize(object);
}

static void gsc_trigger_##(PLUGIN_ID.lower)_class_init (GscTrigger##(PLUGIN_ID.camel)Class * klass)
{
	gsc_trigger_##(PLUGIN_ID.lower)_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->get_property = gsc_trigger_##(PLUGIN_ID.lower)_get_property;
	G_OBJECT_CLASS (klass)->set_property = gsc_trigger_##(PLUGIN_ID.lower)_set_property;
	G_OBJECT_CLASS (klass)->finalize = gsc_trigger_##(PLUGIN_ID.lower)_finalize;
}

static void gsc_trigger_##(PLUGIN_ID.lower)_interface_init (GscTriggerIface * iface)
{
	gsc_trigger_##(PLUGIN_ID.lower)_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_name = gsc_trigger_##(PLUGIN_ID.lower)_real_get_name;
	iface->activate = gsc_trigger_##(PLUGIN_ID.lower)_real_activate;
	iface->deactivate = gsc_trigger_##(PLUGIN_ID.lower)_real_deactivate;
}

GType gsc_trigger_##(PLUGIN_ID.lower)_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (GscTrigger##(PLUGIN_ID.camel)Class), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gsc_trigger_##(PLUGIN_ID.lower)_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GscTrigger##(PLUGIN_ID.camel)), 0, (GInstanceInitFunc) gsc_trigger_##(PLUGIN_ID.lower)_init };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "GscTrigger##(PLUGIN_ID.camel)", &g_define_type_info, 0);
		static const GInterfaceInfo gsc_trigger_##(PLUGIN_ID.lower)_info = { (GInterfaceInitFunc) gsc_trigger_##(PLUGIN_ID.lower)_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		g_type_add_interface_static (g_define_type_id, GSC_TYPE_TRIGGER, &gsc_trigger_##(PLUGIN_ID.lower)_info);
	}
	return g_define_type_id;
}

/**
 * gsc_trigger_##(PLUGIN_ID.lower)_new:
 *
 * Returns The new #GscTrigger##(PLUGIN_ID.camel)
 *
 */
GscTrigger##(PLUGIN_ID.camel)*
gsc_trigger_##(PLUGIN_ID.lower)_new(GscManager *completion)
{
	GscTrigger##(PLUGIN_ID.camel) *self = GSC_TRIGGER_##(PLUGIN_ID.upper) (g_object_new (GSC_TYPE_TRIGGER_##(PLUGIN_ID.upper), NULL));
	self->priv->completion = completion;
	return self;
}


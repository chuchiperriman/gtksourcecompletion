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
#include "gtksourcecompletion-utils.h"
#include "gsc-##(PLUGIN_ID.lower)-trigger.h"

struct _Gsc##(PLUGIN_ID.camel)TriggerPrivate {
	GtkSourceCompletion *completion;
};

#define GSC_##(PLUGIN_ID.upper)_TRIGGER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_GSC_##(PLUGIN_ID.upper)_TRIGGER, Gsc##(PLUGIN_ID.camel)TriggerPrivate))

enum  {
	GSC_##(PLUGIN_ID.upper)_TRIGGER_DUMMY_PROPERTY,
};

static const gchar* gsc_##(PLUGIN_ID.lower)_trigger_real_get_name(GtkSourceCompletionTrigger* base);
static gboolean gsc_##(PLUGIN_ID.lower)_trigger_real_activate (GtkSourceCompletionTrigger* base);
static gboolean gsc_##(PLUGIN_ID.lower)_trigger_real_deactivate (GtkSourceCompletionTrigger* base);

static gpointer gsc_##(PLUGIN_ID.lower)_trigger_parent_class = NULL;
static GtkSourceCompletionTriggerIface* gsc_##(PLUGIN_ID.lower)_trigger_parent_iface = NULL;


static const gchar* gsc_##(PLUGIN_ID.lower)_trigger_real_get_name(GtkSourceCompletionTrigger *self)
{
	return GSC_##(PLUGIN_ID.upper)_TRIGGER_NAME;
}

static gboolean
gsc_##(PLUGIN_ID.lower)_trigger_real_activate (GtkSourceCompletionTrigger* base)
{
	g_debug("Activating ##(PLUGIN_ID.camel) trigger");
	Gsc##(PLUGIN_ID.camel)Trigger *self = GSC_##(PLUGIN_ID.upper)_TRIGGER(base);

	return TRUE;
}

static gboolean
gsc_##(PLUGIN_ID.lower)_trigger_real_deactivate (GtkSourceCompletionTrigger* base)
{
	g_debug("Deactivating ##(PLUGIN_ID.camel) trigger");
	Gsc##(PLUGIN_ID.camel)Trigger *self = GSC_##(PLUGIN_ID.upper)_TRIGGER(base);
	return FALSE;
}

static void gsc_##(PLUGIN_ID.lower)_trigger_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec)
{
}


static void gsc_##(PLUGIN_ID.lower)_trigger_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec)
{
}

static void gsc_##(PLUGIN_ID.lower)_trigger_init (Gsc##(PLUGIN_ID.camel)Trigger * self)
{
	self->priv = g_new0(Gsc##(PLUGIN_ID.camel)TriggerPrivate, 1);
	g_debug("Init ##(PLUGIN_ID.camel) trigger");
}

static void gsc_##(PLUGIN_ID.lower)_trigger_finalize(GObject *object)
{
	g_debug("Finish ##(PLUGIN_ID.camel) trigger");
	Gsc##(PLUGIN_ID.camel)Trigger *self;
	self = GSC_##(PLUGIN_ID.upper)_TRIGGER(object);
	G_OBJECT_CLASS(gsc_##(PLUGIN_ID.lower)_trigger_parent_class)->finalize(object);
}

static void gsc_##(PLUGIN_ID.lower)_trigger_class_init (Gsc##(PLUGIN_ID.camel)TriggerClass * klass)
{
	gsc_##(PLUGIN_ID.lower)_trigger_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->get_property = gsc_##(PLUGIN_ID.lower)_trigger_get_property;
	G_OBJECT_CLASS (klass)->set_property = gsc_##(PLUGIN_ID.lower)_trigger_set_property;
	G_OBJECT_CLASS (klass)->finalize = gsc_##(PLUGIN_ID.lower)_trigger_finalize;
}

static void gsc_##(PLUGIN_ID.lower)_trigger_interface_init (GtkSourceCompletionTriggerIface * iface)
{
	gsc_##(PLUGIN_ID.lower)_trigger_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_name = gsc_##(PLUGIN_ID.lower)_trigger_real_get_name;
	iface->activate = gsc_##(PLUGIN_ID.lower)_trigger_real_activate;
	iface->deactivate = gsc_##(PLUGIN_ID.lower)_trigger_real_deactivate;
}

GType gsc_##(PLUGIN_ID.lower)_trigger_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (Gsc##(PLUGIN_ID.camel)TriggerClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gsc_##(PLUGIN_ID.lower)_trigger_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (Gsc##(PLUGIN_ID.camel)Trigger), 0, (GInstanceInitFunc) gsc_##(PLUGIN_ID.lower)_trigger_init };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "Gsc##(PLUGIN_ID.camel)Trigger", &g_define_type_info, 0);
		static const GInterfaceInfo gsc_##(PLUGIN_ID.lower)_trigger_info = { (GInterfaceInitFunc) gsc_##(PLUGIN_ID.lower)_trigger_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		g_type_add_interface_static (g_define_type_id, GTK_SOURCE_COMPLETION_TYPE_TRIGGER, &gsc_##(PLUGIN_ID.lower)_trigger_info);
	}
	return g_define_type_id;
}

/**
 * gsc_##(PLUGIN_ID.lower)_trigger_new:
 *
 * Returns The new #Gsc##(PLUGIN_ID.camel)Trigger
 *
 */
Gsc##(PLUGIN_ID.camel)Trigger*
gsc_##(PLUGIN_ID.lower)_trigger_new(GtkSourceCompletion *completion)
{
	Gsc##(PLUGIN_ID.camel)Trigger *self = GSC_##(PLUGIN_ID.upper)_TRIGGER (g_object_new (TYPE_GSC_##(PLUGIN_ID.upper)_TRIGGER, NULL));
	self->priv->completion = completion;
	return self;
}


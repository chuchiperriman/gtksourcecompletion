 /* ##(FILENAME) - ##(DESCRIPTION)
 *
 * Copyright (C) ##(DATE_YEAR) - ##(AUTHOR_FULLNAME)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */
 
#include <glib/gprintf.h>
#include <string.h>
#include "gtc-##(PLUGIN_ID.lower)-provider.h"

#define ICON_FILE ICON_DIR"/locals.png"

struct _Gsc##(PLUGIN_ID.camel)ProviderPrivate {
	
};

#define GSC_##(PLUGIN_ID.upper)_PROVIDER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_GSC_##(PLUGIN_ID.upper)_PROVIDER, Gsc##(PLUGIN_ID.camel)ProviderPrivate))
enum  {
	GSC_##(PLUGIN_ID.upper)_PROVIDER_DUMMY_PROPERTY,
};
static const gchar* gsc_##(PLUGIN_ID.lower)_provider_real_get_name (GtkSourceCompletionProvider* self);
static GList* gsc_##(PLUGIN_ID.lower)_provider_real_get_data (GtkSourceCompletionProvider* base, GtkTextView* completion, const gchar* event_name, gpointer event_data);
static void gsc_##(PLUGIN_ID.lower)_provider_real_end_completion (GtkSourceCompletionProvider* base, GtkTextView* view);
static void gsc_##(PLUGIN_ID.lower)_provider_real_data_selected (GtkSourceCompletionProvider* base, GtkTextView* completion, GtkSourceCompletionItem* data);
static gchar* gsc_##(PLUGIN_ID.lower)_provider_real_get_item_info_markup (GtkSourceCompletionProvider *self, GtkSourceCompletionItem *item);
static gpointer gsc_##(PLUGIN_ID.lower)_provider_parent_class = NULL;
static GtkSourceCompletionProviderIface* gsc_##(PLUGIN_ID.lower)_provider_gtk_source_completion_provider_parent_iface = NULL;


static const gchar* gsc_##(PLUGIN_ID.lower)_provider_real_get_name (GtkSourceCompletionProvider* self)
{
	return GSC_##(PLUGIN_ID.upper)_PROVIDER_NAME;
}

static GList* gsc_##(PLUGIN_ID.lower)_provider_real_get_data (GtkSourceCompletionProvider* base, GtkTextView* completion, const gchar* event_name, gpointer event_data)
{
	return NULL;
}

static void gsc_##(PLUGIN_ID.lower)_provider_real_end_completion (GtkSourceCompletionProvider* base, GtkTextView* completion)
{

}

static void gsc_##(PLUGIN_ID.lower)_provider_real_data_selected (GtkSourceCompletionProvider* base, GtkTextView* text_view, GtkSourceCompletionItem* data)
{
	
}

static gchar*
gsc_##(PLUGIN_ID.lower)_provider_real_get_item_info_markup(GtkSourceCompletionProvider *self,
				GtkSourceCompletionItem *item)
{
	return NULL;
}

static void gsc_##(PLUGIN_ID.lower)_provider_real_data_free (GtkSourceCompletionProvider* self, GtkSourceCompletionItem* data)
{
}

static void gsc_##(PLUGIN_ID.lower)_provider_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec)
{
}


static void gsc_##(PLUGIN_ID.lower)_provider_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec)
{
}

static void gsc_##(PLUGIN_ID.lower)_provider_finalize(GObject *object)
{
	Gsc##(PLUGIN_ID.camel)Provider *self;
	
	self = GSC_##(PLUGIN_ID.upper)_PROVIDER(object);
	
	G_OBJECT_CLASS(gsc_##(PLUGIN_ID.lower)_provider_parent_class)->finalize(object);
}


static void gsc_##(PLUGIN_ID.lower)_provider_class_init (Gsc##(PLUGIN_ID.camel)ProviderClass * klass)
{
	gsc_##(PLUGIN_ID.lower)_provider_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->get_property = gsc_##(PLUGIN_ID.lower)_provider_get_property;
	G_OBJECT_CLASS (klass)->set_property = gsc_##(PLUGIN_ID.lower)_provider_set_property;
	G_OBJECT_CLASS (klass)->finalize = gsc_##(PLUGIN_ID.lower)_provider_finalize;
}


static void gsc_##(PLUGIN_ID.lower)_provider_gtk_source_completion_provider_interface_init (GtkSourceCompletionProviderIface * iface)
{
	gsc_##(PLUGIN_ID.lower)_provider_gtk_source_completion_provider_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_data = gsc_##(PLUGIN_ID.lower)_provider_real_get_data;
	iface->data_selected = gsc_##(PLUGIN_ID.lower)_provider_real_data_selected;
	iface->get_name = gsc_##(PLUGIN_ID.lower)_provider_real_get_name;
	iface->end_completion = gsc_##(PLUGIN_ID.lower)_provider_real_end_completion;
	iface->get_item_info_markup = gsc_##(PLUGIN_ID.lower)_provider_real_get_item_info_markup;
}


static void gsc_##(PLUGIN_ID.lower)_provider_init (Gsc##(PLUGIN_ID.camel)Provider * self)
{
	self->priv = g_new0(Gsc##(PLUGIN_ID.camel)ProviderPrivate, 1);
}

GType gsc_##(PLUGIN_ID.lower)_provider_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (Gsc##(PLUGIN_ID.camel)ProviderClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gsc_##(PLUGIN_ID.lower)_provider_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (Gsc##(PLUGIN_ID.camel)Provider), 0, (GInstanceInitFunc) gsc_##(PLUGIN_ID.lower)_provider_init };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "Gsc##(PLUGIN_ID.camel)Provider", &g_define_type_info, 0);
		static const GInterfaceInfo gtk_source_completion_provider_info = { (GInterfaceInitFunc) gsc_##(PLUGIN_ID.lower)_provider_gtk_source_completion_provider_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		g_type_add_interface_static (g_define_type_id, GTK_SOURCE_COMPLETION_TYPE_PROVIDER, &gtk_source_completion_provider_info);
	}
	return g_define_type_id;
}


Gsc##(PLUGIN_ID.camel)Provider*
gsc_##(PLUGIN_ID.lower)_provider_new()
{
	return GSC_##(PLUGIN_ID.upper)_PROVIDER (g_object_new (TYPE_GSC_##(PLUGIN_ID.upper)_PROVIDER, NULL));
}


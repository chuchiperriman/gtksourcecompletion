/* 
 *  ##(FILENAME) - ##(DESCRIPTION)
 *
 *  Copyright (C) ##(DATE_YEAR) - ##(AUTHOR_FULLNAME)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "gsc-provider-##(PLUGIN_ID.lower).h"

struct _GscProvider##(PLUGIN_ID.camel)Private {
	
};

#define GSC_PROVIDER_##(PLUGIN_ID.upper)_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_GSC_PROVIDER_##(PLUGIN_ID.upper), GscProvider##(PLUGIN_ID.camel)Private))

static const gchar* 
gsc_provider_##(PLUGIN_ID.lower)_real_get_name (GscProvider* self);
static GList* 
gsc_provider_##(PLUGIN_ID.lower)_real_get_proposals (GscProvider* base,
						     GscTrigger *trigger);
static void 
gsc_provider_##(PLUGIN_ID.lower)_real_finish (GscProvider* base);

static gpointer 
gsc_provider_##(PLUGIN_ID.lower)_parent_class = NULL;
static GscProviderIface* 
gsc_provider_##(PLUGIN_ID.lower)_parent_iface = NULL;


static const gchar* 
gsc_provider_##(PLUGIN_ID.lower)_real_get_name (GscProvider* self)
{
	return GSC_PROVIDER_##(PLUGIN_ID.upper)_NAME;
}

static GList* 
gsc_provider_##(PLUGIN_ID.lower)_real_get_proposals (GscProvider* base,
						GscTrigger *trigger)
{
	return NULL;
}

static void 
gsc_provider_##(PLUGIN_ID.lower)_real_finish (GscProvider* base)
{

}

static void 
gsc_provider_##(PLUGIN_ID.lower)_finalize(GObject *object)
{
	GscProvider##(PLUGIN_ID.camel) *self;
	self = GSC_PROVIDER_##(PLUGIN_ID.upper)(object);
	G_OBJECT_CLASS(gsc_provider_##(PLUGIN_ID.lower)_parent_class)->finalize(object);
}


static void 
gsc_provider_##(PLUGIN_ID.lower)_class_init (GscProvider##(PLUGIN_ID.camel)Class *klass)
{
	gsc_provider_##(PLUGIN_ID.lower)_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->finalize = gsc_provider_##(PLUGIN_ID.lower)_finalize;
}

static void
gsc_provider_##(PLUGIN_ID.lower)_interface_init (GscProviderIface *iface)
{
	gsc_provider_##(PLUGIN_ID.lower)_parent_iface = g_type_interface_peek_parent (iface);
	
	iface->get_name = gsc_provider_##(PLUGIN_ID.lower)_real_get_name;
	iface->get_proposals = gsc_provider_##(PLUGIN_ID.lower)_real_get_proposals;
	iface->finish = gsc_provider_##(PLUGIN_ID.lower)_real_finish;
}


static void 
gsc_provider_##(PLUGIN_ID.lower)_init (GscProvider##(PLUGIN_ID.camel) * self)
{
	self->priv = g_new0(GscProvider##(PLUGIN_ID.camel)Private, 1);
}

GType gsc_provider_##(PLUGIN_ID.lower)_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (GscProvider##(PLUGIN_ID.camel)Class), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gsc_provider_##(PLUGIN_ID.lower)_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GscProvider##(PLUGIN_ID.camel)), 0, (GInstanceInitFunc) gsc_provider_##(PLUGIN_ID.lower)_init };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "GscProvider##(PLUGIN_ID.camel)", &g_define_type_info, 0);
		static const GInterfaceInfo gsc_provider_info = { (GInterfaceInitFunc) gsc_provider_##(PLUGIN_ID.lower)_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		g_type_add_interface_static (g_define_type_id, GSC_TYPE_PROVIDER, &gsc_provider_info);
	}
	return g_define_type_id;
}


GscProvider##(PLUGIN_ID.camel)*
gsc_provider_##(PLUGIN_ID.lower)_new()
{
	return GSC_PROVIDER_##(PLUGIN_ID.upper) (g_object_new (GSC_TYPE_PROVIDER_##(PLUGIN_ID.upper), NULL));
}


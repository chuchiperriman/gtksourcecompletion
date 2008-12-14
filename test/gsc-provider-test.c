/* 
 *  gsc-provider-test.c - Type here a short description of your plugin
 *
 *  Copyright (C) 2008 - perriman
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

#include "gsc-provider-test.h"

struct _GscProviderTestPrivate {
	
};

#define GSC_PROVIDER_TEST_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_GSC_PROVIDER_TEST, GscProviderTestPrivate))

static const gchar* 
gsc_provider_test_real_get_name (GscProvider* self);
static GList* 
gsc_provider_test_real_get_proposals (GscProvider* base,
						     GscTrigger *trigger);
static void 
gsc_provider_test_real_finish (GscProvider* base);

static gpointer 
gsc_provider_test_parent_class = NULL;
static GscProviderIface* 
gsc_provider_test_parent_iface = NULL;


static const gchar* 
gsc_provider_test_real_get_name (GscProvider* self)
{
	return GSC_PROVIDER_TEST_NAME;
}

static GList* 
gsc_provider_test_real_get_proposals (GscProvider* base,
						GscTrigger *trigger)
{
	GList *list = NULL;
	GscProposal *prop;
	
	prop = gsc_proposal_new("Proposal 1",
				"Info proposal 1",
				NULL);
	list = g_list_append (list, prop);
	prop = gsc_proposal_new("Proposal 2",
				"Info proposal 2",
				NULL);
	list = g_list_append (list, prop);
	prop = gsc_proposal_new("Proposal 3",
				"Info proposal 3",
				NULL);
	list = g_list_append (list, prop);
	
	/*Page 2*/
	prop = gsc_proposal_new("Proposal 1,2",
				"Info proposal 1,2",
				NULL);
	gsc_proposal_set_page_name(prop,"Page 2");
	list = g_list_append (list, prop);
	prop = gsc_proposal_new("Proposal 2,2",
				"Info proposal 2,2",
				NULL);
	gsc_proposal_set_page_name(prop,"Page 2");
	list = g_list_append (list, prop);
	prop = gsc_proposal_new("Proposal 3,3",
				"Info proposal 3,3",
				NULL);
	gsc_proposal_set_page_name(prop,"Page 3");
	list = g_list_append (list, prop);
	return list;
}

static void 
gsc_provider_test_real_finish (GscProvider* base)
{

}

static void 
gsc_provider_test_finalize(GObject *object)
{
	GscProviderTest *self;
	self = GSC_PROVIDER_TEST(object);
	G_OBJECT_CLASS(gsc_provider_test_parent_class)->finalize(object);
}


static void 
gsc_provider_test_class_init (GscProviderTestClass *klass)
{
	gsc_provider_test_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->finalize = gsc_provider_test_finalize;
}

static void
gsc_provider_test_interface_init (GscProviderIface *iface)
{
	gsc_provider_test_parent_iface = g_type_interface_peek_parent (iface);
	
	iface->get_name = gsc_provider_test_real_get_name;
	iface->get_proposals = gsc_provider_test_real_get_proposals;
	iface->finish = gsc_provider_test_real_finish;
}


static void 
gsc_provider_test_init (GscProviderTest * self)
{
	self->priv = g_new0(GscProviderTestPrivate, 1);
}

GType gsc_provider_test_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (GscProviderTestClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gsc_provider_test_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GscProviderTest), 0, (GInstanceInitFunc) gsc_provider_test_init };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "GscProviderTest", &g_define_type_info, 0);
		static const GInterfaceInfo gsc_provider_info = { (GInterfaceInitFunc) gsc_provider_test_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		g_type_add_interface_static (g_define_type_id, GSC_TYPE_PROVIDER, &gsc_provider_info);
	}
	return g_define_type_id;
}


GscProviderTest*
gsc_provider_test_new()
{
	return GSC_PROVIDER_TEST (g_object_new (GSC_TYPE_PROVIDER_TEST, NULL));
}


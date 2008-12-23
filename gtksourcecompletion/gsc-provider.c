/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-provider.c
 *
 *  Copyright (C) 2007 - Chuchiperriman <chuchiperriman@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include "gsc-provider.h"

/**
 * gsc_provider_get_name:
 * @self: The #GscProvider
 *
 * The provider name. By example: "Document word completion provider"
 *
 * Returns: The provider's name 
 */
const gchar*
gsc_provider_get_name (GscProvider *self)
{
	g_return_val_if_fail (GSC_IS_PROVIDER (self), NULL);
	return GSC_PROVIDER_GET_INTERFACE (self)->get_name (self);
}

/* Default implementation */
static const gchar *
gsc_provider_get_name_default (GscProvider *self)
{
	g_return_val_if_reached (NULL);
}

/**
 * gsc_provider_get_proposals:
 * @self: The #GscProvider
 * @trigger: The #GscTrigger that raise the event
 *
 * The completion call this function when an event is raised.
 * This function may return a list of #GscProposal to be shown
 * in the popup to the user.
 *
 * Returns: a list of #GscProposal or NULL if there are no proposals
 */
GList* 
gsc_provider_get_proposals (GscProvider* self,
			    GscTrigger *trigger)
{
	g_return_val_if_fail (GSC_IS_PROVIDER (self), NULL);
	return GSC_PROVIDER_GET_INTERFACE (self)->get_proposals (self, trigger);
}

/* Default implementation */
static GList *
gsc_provider_get_proposals_default (GscProvider *self,
				    GscTrigger *trigger)
{
	g_return_val_if_reached (NULL);
}

/**
 * gsc_provider_finish:
 * @self: The #GscProvider
 *
 * The completion call this function when it is goint to hide the popup (The 
 * user selects a proposal or hide the completion popup)
 */
void 
gsc_provider_finish (GscProvider* self)
{
	g_return_if_fail (GSC_IS_PROVIDER (self));
	GSC_PROVIDER_GET_INTERFACE (self)->finish (self);
}

/* Default implementation */
static void
gsc_provider_finish_default (GscProvider *self)
{
	g_return_if_reached ();
}

static void 
gsc_provider_base_init (GscProviderIface * iface)
{
	static gboolean initialized = FALSE;
	
	iface->get_name = gsc_provider_get_name_default;
	iface->get_proposals = gsc_provider_get_proposals_default;
	iface->finish = gsc_provider_finish_default;
	
	if (!initialized) {
		initialized = TRUE;
	}
}


GType 
gsc_provider_get_type ()
{
	static GType gsc_provider_type_id = 0;
	if (!gsc_provider_type_id) {
		static const GTypeInfo g_define_type_info = 
		{ 
			sizeof (GscProviderIface), 
			(GBaseInitFunc) gsc_provider_base_init, 
			(GBaseFinalizeFunc) NULL, 
			(GClassInitFunc) NULL, 
			(GClassFinalizeFunc) NULL, 
			NULL, 
			0, 
			0, 
			(GInstanceInitFunc) NULL 
		};
						
		gsc_provider_type_id = 
				g_type_register_static (G_TYPE_INTERFACE, 
							"GscProvider", 
							&g_define_type_info, 
							0);
	}
	return gsc_provider_type_id;
}



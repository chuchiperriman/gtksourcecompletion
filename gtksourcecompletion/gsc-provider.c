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

const gchar*
gsc_provider_get_name(GscProvider *self)
{
	return GSC_PROVIDER_GET_INTERFACE (self)->get_name (self);
}

GList* 
gsc_provider_get_proposals (GscProvider* self,
			    GscTrigger *trigger)
{
	return GSC_PROVIDER_GET_INTERFACE (self)->get_proposals (self, trigger);
}

void 
gsc_provider_finish (GscProvider* self)
{
	GSC_PROVIDER_GET_INTERFACE (self)->finish(self);
}

static void 
gsc_provider_base_init (GscProviderIface * iface)
{
	static gboolean initialized = FALSE;
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



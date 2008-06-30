/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-trigger.c
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

#include "gsc-trigger.h"

static void 
gsc_trigger_base_init (GscTriggerIface * iface)
{
	static gboolean initialized = FALSE;
	if (!initialized) {
		initialized = TRUE;
	}
}

GType 
gsc_trigger_get_type ()
{
	static GType gsc_trigger_type_id = 0;
	if (!gsc_trigger_type_id) {
		static const GTypeInfo g_define_type_info = 
		{ 
			sizeof (GscTriggerIface), 
			(GBaseInitFunc) gsc_trigger_base_init, 
			(GBaseFinalizeFunc) NULL, 
			(GClassInitFunc) NULL, 
			(GClassFinalizeFunc) NULL, 
			NULL, 
			0, 
			0, 
			(GInstanceInitFunc) NULL 
		};
							
		gsc_trigger_type_id = 
			g_type_register_static (G_TYPE_INTERFACE, 
						"GscTrigger", 
						&g_define_type_info, 
						0);
	}
	return gsc_trigger_type_id;
}

const gchar*
gsc_trigger_get_name(GscTrigger *self)
{
	return GSC_TRIGGER_GET_INTERFACE (self)->get_name (self);
}

gboolean
gsc_trigger_activate (GscTrigger* self)
{
	return GSC_TRIGGER_GET_INTERFACE (self)->activate (self);
}

gboolean
gsc_trigger_deactivate (GscTrigger* self)
{
	return GSC_TRIGGER_GET_INTERFACE (self)->deactivate (self);
}


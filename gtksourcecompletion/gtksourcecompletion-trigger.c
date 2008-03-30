/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-trigger.c
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

#include "gtksourcecompletion-trigger.h"

static void 
gtk_source_completion_trigger_base_init (GtkSourceCompletionTriggerIface * iface)
{
	static gboolean initialized = FALSE;
	if (!initialized) {
		initialized = TRUE;
	}
}

GType 
gtk_source_completion_trigger_get_type ()
{
	static GType gtk_source_completion_trigger_type_id = 0;
	if (!gtk_source_completion_trigger_type_id) {
		static const GTypeInfo g_define_type_info = { 
							sizeof (GtkSourceCompletionTriggerIface), 
							(GBaseInitFunc) gtk_source_completion_trigger_base_init, 
							(GBaseFinalizeFunc) NULL, 
							(GClassInitFunc) NULL, 
							(GClassFinalizeFunc) NULL, 
							NULL, 
							0, 
							0, 
							(GInstanceInitFunc) NULL };
							
		gtk_source_completion_trigger_type_id = 
				g_type_register_static (G_TYPE_INTERFACE, 
							"GtkSourceCompletionTrigger", 
							&g_define_type_info, 
							0);
	}
	return gtk_source_completion_trigger_type_id;
}

const gchar*
gtk_source_completion_trigger_get_name(GtkSourceCompletionTrigger *self)
{
	return GTK_SOURCE_COMPLETION_TRIGGER_GET_INTERFACE (self)->get_name (self);
}

gboolean
gtk_source_completion_trigger_activate (GtkSourceCompletionTrigger* self)
{
	return GTK_SOURCE_COMPLETION_TRIGGER_GET_INTERFACE (self)->activate (self);
}

gboolean
gtk_source_completion_trigger_deactivate (GtkSourceCompletionTrigger* self)
{
	return GTK_SOURCE_COMPLETION_TRIGGER_GET_INTERFACE (self)->deactivate (self);
}


/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletiontrigger.c
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
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

/**
 * SECTION:gsc-trigger
 * @title: GtkSourceCompletionTrigger
 * @short_description: Completion trigger interface
 *
 * You must implement this interface to trigger completion events with #GscCompletion
 * 
 */
 
#include "gtksourcecompletiontrigger.h"

/**
 * gtk_source_completion_trigger_get_name:
 * @self: The #GtkSourceCompletionTrigger
 *
 * The trigger name. By example: "C autocompletion trigger".
 *
 * Returns: The trigger's name
 */
const gchar*
gtk_source_completion_trigger_get_name (GtkSourceCompletionTrigger *self)
{
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_TRIGGER (self), NULL);
	return GTK_SOURCE_COMPLETION_TRIGGER_GET_INTERFACE (self)->get_name (self);
}

/* Default implementation */
static const gchar *
gtk_source_completion_trigger_get_name_default (GtkSourceCompletionTrigger *self)
{
	g_return_val_if_reached (NULL);
}

/**
 * gtk_source_completion_trigger_activate:
 * @self: The #GtkSourceCompletionTrigger
 *
 * Activate the completion trigger.
 *
 * Returns: %TRUE if the activation was OK, %FALSE if not.
 */
gboolean
gtk_source_completion_trigger_activate (GtkSourceCompletionTrigger *self)
{
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_TRIGGER (self), FALSE);
	return GTK_SOURCE_COMPLETION_TRIGGER_GET_INTERFACE (self)->activate (self);
}

/* Default implementation */
static gboolean
gtk_source_completion_trigger_activate_default (GtkSourceCompletionTrigger *self)
{
	g_return_val_if_reached (FALSE);
}

/**
 * gtk_source_completion_trigger_deactivate:
 * @self: The #GtkSourceCompletionTrigger
 *
 * Deactive the completion trigger
 *
 * Returns: TRUE if the deactivation was OK, FALSE if not.
 */
gboolean
gtk_source_completion_trigger_deactivate (GtkSourceCompletionTrigger* self)
{
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION_TRIGGER (self), FALSE);
	return GTK_SOURCE_COMPLETION_TRIGGER_GET_INTERFACE (self)->deactivate (self);
}

/* Default implementation */
static gboolean
gtk_source_completion_trigger_deactivate_default (GtkSourceCompletionTrigger *self)
{
	g_return_val_if_reached (FALSE);
}

static void 
gtk_source_completion_trigger_base_init (GtkSourceCompletionTriggerIface *iface)
{
	static gboolean initialized = FALSE;
	
	iface->get_name = gtk_source_completion_trigger_get_name_default;
	iface->activate = gtk_source_completion_trigger_activate_default;
	iface->deactivate = gtk_source_completion_trigger_deactivate_default;
	
	if (!initialized) {
		initialized = TRUE;
	}
}

GType
gtk_source_completion_trigger_get_type ()
{
	static GType gtk_source_completion_trigger_type_id = 0;
	if (!gtk_source_completion_trigger_type_id) {
		static const GTypeInfo g_define_type_info = 
		{ 
			sizeof (GtkSourceCompletionTriggerIface), 
			(GBaseInitFunc) gtk_source_completion_trigger_base_init, 
			(GBaseFinalizeFunc) NULL, 
			(GClassInitFunc) NULL, 
			(GClassFinalizeFunc) NULL, 
			NULL, 
			0, 
			0, 
			(GInstanceInitFunc) NULL 
		};
							
		gtk_source_completion_trigger_type_id = 
			g_type_register_static (G_TYPE_INTERFACE, 
						"GtkSourceCompletionTrigger", 
						&g_define_type_info, 
						0);
	}
	return gtk_source_completion_trigger_type_id;
}



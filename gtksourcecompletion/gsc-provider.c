/*
 * gscprovider.c
 * This file is part of gsc
 *
 * Copyright (C) 2007 -2009 Jesús Barbero Rodríguez <chuchiperriman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA.
 */


/**
 * SECTION:gsc_completion-provider
 * @title: GscProvider
 * @short_description: Completion provider interface
 *
 * You must implement this interface to provide proposals to #GscCompletion
 * 
 */

#include "gsc-provider.h"
#include "gsc-completion.h"
#include <gtksourceview/gtksourceview.h>

#include "gsc-utils.h"
#include "gsc-i18n.h"

/* Default implementations */
static const gchar *
gsc_completion_provider_get_name_default (GscProvider *provider)
{
	g_return_val_if_reached (NULL);
}

static GdkPixbuf *
gsc_completion_provider_get_icon_default (GscProvider *provider)
{
	return NULL;
}

static void
gsc_completion_provider_populate_completion_default (GscProvider *provider,
							    GscContext  *context)
{
}

static const gchar *
gsc_completion_provider_get_capabilities_default (GscProvider *provider)
{
	return GSC_COMPLETION_CAPABILITY_AUTOMATIC;
}

static GtkWidget *
gsc_completion_provider_get_info_widget_default (GscProvider *provider,
                                                        GscProposal *proposal)
{
	return NULL;
}

static void
gsc_completion_provider_update_info_default (GscProvider *provider,
                                                    GscProposal *proposal,
                                                    GscInfo     *info)
{
}

static gboolean
gsc_completion_provider_activate_proposal_default (GscProvider *provider,
                                                          GscProposal *proposal,
                                                          GtkTextIter                 *iter)
{
	return FALSE;
}

static void 
gsc_completion_provider_base_init (GscProviderIface *iface)
{
	static gboolean initialized = FALSE;
	
	iface->get_name = gsc_completion_provider_get_name_default;
	iface->get_icon = gsc_completion_provider_get_icon_default;

	iface->populate_completion = gsc_completion_provider_populate_completion_default;
	
	iface->get_capabilities = gsc_completion_provider_get_capabilities_default;
	
	iface->get_info_widget = gsc_completion_provider_get_info_widget_default;
	iface->update_info = gsc_completion_provider_update_info_default;
	
	iface->activate_proposal = gsc_completion_provider_activate_proposal_default;

	if (!initialized)
	{
		initialized = TRUE;
	}
}

GType 
gsc_completion_provider_get_type ()
{
	static GType gsc_completion_provider_type_id = 0;

	if (!gsc_completion_provider_type_id)
	{
		static const GTypeInfo g_define_type_info = 
		{ 
			sizeof (GscProviderIface), 
			(GBaseInitFunc) gsc_completion_provider_base_init, 
			(GBaseFinalizeFunc) NULL, 
			(GClassInitFunc) NULL, 
			(GClassFinalizeFunc) NULL, 
			NULL, 
			0, 
			0, 
			(GInstanceInitFunc) NULL 
		};
						
		gsc_completion_provider_type_id = 
				g_type_register_static (G_TYPE_INTERFACE, 
							"GscProvider", 
							&g_define_type_info, 
							0);
	}

	return gsc_completion_provider_type_id;
}

/**
 * gsc_completion_provider_get_name:
 * @provider: The #GscProvider
 *
 * Get the name of the provider. This should be a translatable name for
 * display to the user. For example: _("Document word completion provider").
 *
 * Returns: The name of the provider.
 */
const gchar *
gsc_completion_provider_get_name (GscProvider *provider)
{
	g_return_val_if_fail (GSC_IS_PROVIDER (provider), NULL);
	return GSC_PROVIDER_GET_INTERFACE (provider)->get_name (provider);
}

/**
 * gsc_completion_provider_get_icon:
 * @provider: The #GscProvider
 *
 * Get the icon of the provider.
 *
 * Returns: The icon to be used for the provider, or %NULL if the provider does
 *          not have a special icon.
 */
GdkPixbuf *
gsc_completion_provider_get_icon (GscProvider *provider)
{
	g_return_val_if_fail (GSC_IS_PROVIDER (provider), NULL);
	return GSC_PROVIDER_GET_INTERFACE (provider)->get_icon (provider);
}

/**
 * gsc_completion_provider_populate_completion:
 * @provider: The #GscProvider
 * @context: The #GscContext to add the proposals, get the
 * current criteria etc.
 *
 * TODO Document this function
 *
 */
void
gsc_completion_provider_populate_completion (GscProvider *provider,
						    GscContext *context)
{
	g_return_if_fail (GSC_IS_PROVIDER (provider));
	g_return_if_fail (GTK_IS_CONTEXT (context));

	GSC_PROVIDER_GET_INTERFACE (provider)->populate_completion (provider, 
										      context);
}

/**
 * gsc_completion_provider_get_capabilities:
 * @provider: The #GscProvider
 *
 * A list of capabilities this provider supports
 *
 * Returns: list of capabilities.
 */
const gchar *
gsc_completion_provider_get_capabilities (GscProvider *provider)
{
	g_return_val_if_fail (GSC_IS_PROVIDER (provider), NULL);
	return GSC_PROVIDER_GET_INTERFACE (provider)->get_capabilities (provider);
}

/**
 * gsc_completion_provider_get_info_widget:
 * @provider: The #GscProvider
 * @proposal: The currently selected #GscProposal
 *
 * Get a customized info widget to show extra information of a proposal.
 * This allows for customized widgets on a proposal basis, although in general
 * providers will have the same custom widget for all their proposals and
 * @proposal can be ignored. The implementation of this function is optional. 
 * If implemented, #gsc_completion_provider_update_info MUST also be
 * implemented. If not implemented, the default 
 * #gsc_completion_proposal_get_info will be used to display extra
 * information about a #GscProposal.
 *
 * Returns: a custom #GtkWidget to show extra information about @proposal.
 */
GtkWidget *
gsc_completion_provider_get_info_widget (GscProvider *provider,
                                                GscProposal *proposal)
{
	g_return_val_if_fail (GSC_IS_PROVIDER (provider), NULL);
	g_return_val_if_fail (GTK_IS_PROPOSAL (proposal), NULL);
	
	return GSC_PROVIDER_GET_INTERFACE (provider)->get_info_widget (provider, proposal);
}

/**
 * gsc_completion_provider_update_info:
 * @provider: A #GscProvider
 * @proposal: A #GscProposal
 * @info: A #GscInfo
 *
 * Update extra information shown in @info for @proposal. This should be
 * implemented if your provider sets a custom info widget for @proposal.
 * This function MUST be implemented when 
 * #gsc_completion_provider_get_info_widget is implemented.
 */
void 
gsc_completion_provider_update_info (GscProvider *provider,
                                            GscProposal *proposal,
                                            GscInfo     *info)
{
	g_return_if_fail (GSC_IS_PROVIDER (provider));
	g_return_if_fail (GTK_IS_PROPOSAL (proposal));
	g_return_if_fail (GTK_IS_INFO (info));
	
	GSC_PROVIDER_GET_INTERFACE (provider)->update_info (provider, proposal, info);
}

/**
 * gsc_completion_provider_activate_proposal:
 * @provider: A #GscProvider
 * @proposal: A #GscProposal
 * @iter: A #GtkTextIter
 *
 * Activate @proposal at @iter. When this functions returns %FALSE, the default
 * activation of @proposal will take place which replaces the word at @iter
 * with the label of @proposal.
 *
 * Returns: %TRUE to indicate that the proposal activation has been handled,
 *          %FALSE otherwise.
 */
gboolean
gsc_completion_provider_activate_proposal (GscProvider *provider,
                                                  GscProposal *proposal,
                                                  GtkTextIter                 *iter)
{
	g_return_val_if_fail (GSC_IS_PROVIDER (provider), FALSE);
	g_return_val_if_fail (GTK_IS_PROPOSAL (proposal), FALSE);
	
	return GSC_PROVIDER_GET_INTERFACE (provider)->activate_proposal (provider, 
	                                                                                   proposal,
	                                                                                   iter);
}

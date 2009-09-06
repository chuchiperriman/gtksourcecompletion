/*
 * gscproposal.c
 * This file is part of gsc
 *
 * Copyright (C) 2007 - 2009 Jesús Barbero Rodríguez <chuchiperriman@gmail.com>
 * Copyright (C) 2009 Jesse van den Kieboom <jessevdk@gnome.org>
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
 * SECTION:gscproposal
 * @title: GscProposal
 * @short_description: Completion proposal object
 *
 * The proposal interface represents a completion item in the completion window.
 * It provides information on how to display the completion item and what action
 * should be taken when the completion item is activated.
 */

#include "gsc-proposal.h"

#include "gsc-marshal.h"

/* Signals */
enum
{
	CHANGED,
	NUM_SIGNALS
};

static guint signals[NUM_SIGNALS] = {0,};

static const gchar *
gsc_completion_proposal_get_label_default (GscProposal *proposal)
{
	g_return_val_if_reached (NULL);
}

static const gchar *
gsc_completion_proposal_get_markup_default (GscProposal *proposal)
{
	return NULL;
}

static const gchar *
gsc_completion_proposal_get_text_default (GscProposal *proposal)
{
	return NULL;
}

static GdkPixbuf *
gsc_completion_proposal_get_icon_default (GscProposal *proposal)
{
	return NULL;
}

static const gchar *
gsc_completion_proposal_get_info_default (GscProposal *proposal)
{
	return NULL;
}

static guint
gsc_completion_proposal_get_hash_default (GscProposal *proposal)
{
	const gchar *label;
	
	label = gsc_completion_proposal_get_label (proposal);
	
	if (label == NULL)
		label = gsc_completion_proposal_get_markup (proposal);
	
	if (label != NULL)
		return g_str_hash (label);
	else
		g_return_val_if_reached (0);
}

static gboolean
gsc_completion_proposal_equals_default (GscProposal *proposal1,
					       GscProposal *proposal2)
{
	const gchar *label1, *label2;
	
	label1 = gsc_completion_proposal_get_markup (proposal1);
	label2 = gsc_completion_proposal_get_markup (proposal2);

	if (label1 != NULL && label2 == NULL)
	{
		return FALSE;
	}
	else if (label2 != NULL && label1 == NULL)
	{
		return FALSE;
	}
	else if (label1 == NULL && label2 == NULL)
	{
		label1 = gsc_completion_proposal_get_label (proposal1);
		label2 = gsc_completion_proposal_get_label (proposal2);
	}

	if (label1 != NULL && label2 != NULL)
	{
		/* FIXME: g_utf8_collate ??? */
		if (g_strcmp0 (label1, label2) == 0)
			return TRUE;
		else
			return FALSE;
	}
	else
	{
		g_return_val_if_reached (FALSE);
	}
}

static void 
gsc_completion_proposal_init (GscProposalIface *iface)
{
	static gboolean initialized = FALSE;
	
	iface->get_label = gsc_completion_proposal_get_label_default;
	iface->get_markup = gsc_completion_proposal_get_markup_default;
	iface->get_text = gsc_completion_proposal_get_text_default;
	
	iface->get_icon = gsc_completion_proposal_get_icon_default;
	iface->get_info = gsc_completion_proposal_get_info_default;
	
	iface->get_hash = gsc_completion_proposal_get_hash_default;
	iface->equals = gsc_completion_proposal_equals_default;
	
	if (!initialized)
	{
		/**
		 * GscProposal::changed:
		 * @proposal: The #GscProposal
		 *
		 * Emitted when the proposal has changed. The completion popup
		 * will react to this by updating the shown information.
		 *
		 */
		signals[CHANGED] = 
			g_signal_new ("changed",
			      G_TYPE_FROM_INTERFACE (iface),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GscProposalIface, changed),
			      NULL, 
			      NULL,
			      g_cclosure_marshal_VOID__VOID, 
			      G_TYPE_NONE,
			      0);

		initialized = TRUE;
	}
}

GType 
gsc_completion_proposal_get_type ()
{
	static GType gsc_completion_proposal_type_id = 0;
	
	if (!gsc_completion_proposal_type_id)
	{
		static const GTypeInfo g_define_type_info =
		{
			sizeof (GscProposalIface),
			(GBaseInitFunc) gsc_completion_proposal_init, 
			NULL,
			NULL,
			NULL,
			NULL,
			0,
			0,
			NULL
		};
		
		gsc_completion_proposal_type_id = 
			g_type_register_static (G_TYPE_INTERFACE,
						"GscProposal",
						&g_define_type_info,
						0);
	}
	
	return gsc_completion_proposal_type_id;
}

/**
 * gsc_completion_proposal_get_label:
 * @proposal: A #GscProposal
 *
 * Gets the label of @proposal. The label is shown in the list of proposals as
 * plain text. If you need any markup (such as bold or italic text), you have
 * to implement #gsc_completion_proposal_get_markup.
 *
 * Returns: The label of @proposal.
 */
const gchar *
gsc_completion_proposal_get_label (GscProposal *proposal)
{
	g_return_val_if_fail (GTK_IS_PROPOSAL (proposal), NULL);	
	return GSC_COMPLETION_PROPOSAL_GET_INTERFACE (proposal)->get_label (proposal);
}

/**
 * gsc_completion_proposal_get_markup:
 * @proposal: A #GscProposal
 *
 * Gets the label of @proposal with markup. The label is shown in the list of 
 * proposals and may contain markup. This will be used instead of
 * #gsc_completion_proposal_get_label if implemented.
 *
 * Returns: The label of @proposal with markup.
 */
const gchar *
gsc_completion_proposal_get_markup (GscProposal *proposal)
{
	g_return_val_if_fail (GTK_IS_PROPOSAL (proposal), NULL);	
	return GSC_COMPLETION_PROPOSAL_GET_INTERFACE (proposal)->get_markup (proposal);
}

/**
 * gsc_completion_proposal_get_text:
 * @proposal: A #GscProposal
 *
 * Gets the text of @proposal. The text that is inserted into
 * the text buffer when the proposal is activated by the default activation.
 * You are free to implement a custom activation handler in the provider and
 * not implement this function.
 *
 * Returns: The text of @proposal.
 */
const gchar *
gsc_completion_proposal_get_text (GscProposal *proposal)
{
	g_return_val_if_fail (GTK_IS_PROPOSAL (proposal), NULL);
	return GSC_COMPLETION_PROPOSAL_GET_INTERFACE (proposal)->get_text (proposal);
}

/**
 * gsc_completion_proposal_get_icon:
 * @proposal: A #GscProposal
 *
 * Gets the icon of @proposal.
 *
 * Returns: The icon of @proposal.
 */
GdkPixbuf *
gsc_completion_proposal_get_icon (GscProposal *proposal)
{
	g_return_val_if_fail (GTK_IS_PROPOSAL (proposal), NULL);
	return GSC_COMPLETION_PROPOSAL_GET_INTERFACE (proposal)->get_icon (proposal);
}

/**
 * gsc_completion_proposal_get_info:
 * @proposal: A #GscProposal
 *
 * Gets extra information associated to the proposal. This information will be
 * used to present the user with extra, detailed information about the
 * selected proposal.
 *
 * Returns: The extra information of @proposal or %NULL if no extra information
 *          is associated to @proposal.
 */
const gchar *
gsc_completion_proposal_get_info (GscProposal *proposal)
{
	g_return_val_if_fail (GTK_IS_PROPOSAL (proposal), NULL);
	return GSC_COMPLETION_PROPOSAL_GET_INTERFACE (proposal)->get_info (proposal);
}

guint
gsc_completion_proposal_get_hash (GscProposal *proposal)
{
	g_return_val_if_fail (GTK_IS_PROPOSAL (proposal), 0);
	return GSC_COMPLETION_PROPOSAL_GET_INTERFACE (proposal)->get_hash (proposal);
}

gboolean
gsc_completion_proposal_equals (GscProposal *proposal1,
				       GscProposal *proposal2)
{
	g_return_val_if_fail (GTK_IS_PROPOSAL (proposal1), FALSE);
	g_return_val_if_fail (GTK_IS_PROPOSAL (proposal2), FALSE);
	return GSC_COMPLETION_PROPOSAL_GET_INTERFACE (proposal1)->equals (proposal1, proposal2);
}

/**
 * gsc_completion_proposal_changed:
 * @proposal: A #GscProposal
 *
 * Emits the "changed" signal on @proposal. This should be called by
 * implementations whenever the name, icon or info of the proposal has
 * changed.
 */
void
gsc_completion_proposal_changed (GscProposal *proposal)
{
	g_return_if_fail (GTK_IS_PROPOSAL (proposal));
	g_signal_emit (proposal, signals[CHANGED], 0);
}

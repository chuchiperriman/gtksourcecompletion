/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-proposal.h
 *
 *  Copyright (C) 2008 - ChuchiPerriman <chuchiperriman@gmail.com>
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

#ifndef _GTK_SOURCE_COMPLETION_PROPOSAL_H
#define _GTK_SOURCE_COMPLETION_PROPOSAL_H

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "gtksourcecompletion.h"

G_BEGIN_DECLS

#define GTK_SOURCE_COMPLETION_TYPE_PROPOSAL             (gtk_source_completion_proposal_get_type ())
#define GTK_SOURCE_COMPLETION_PROPOSAL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_SOURCE_COMPLETION_TYPE_PROPOSAL, GtkSourceCompletionProposal))
#define GTK_SOURCE_COMPLETION_PROPOSAL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_SOURCE_COMPLETION_TYPE_PROPOSAL, GtkSourceCompletionProposalClass)
#define GTK_SOURCE_COMPLETION_IS_PROPOSAL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_SOURCE_COMPLETION_TYPE_PROPOSAL))
#define GTK_SOURCE_COMPLETION_IS_PROPOSAL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_SOURCE_COMPLETION_TYPE_PROPOSAL))
#define GTK_SOURCE_COMPLETION_PROPOSAL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_SOURCE_COMPLETION_TYPE_PROPOSAL, GtkSourceCompletionProposalClass))

#define GTK_SOURCE_COMPLETION_PROPOSAL_DEFAULT_PAGE "Default"
#define GTK_SOURCE_COMPLETION_PROPOSAL_DEFAULT_PRIORITY 10

typedef struct _GtkSourceCompletionProposalPrivate GtkSourceCompletionProposalPrivate;
typedef struct _GtkSourceCompletionProposalClass GtkSourceCompletionProposalClass;
typedef struct _GtkSourceCompletionProposal GtkSourceCompletionProposal;

typedef void (*GtkSourceCompletionProposalApply)  (GtkSourceCompletionProposal *proposal,
						   GtkSourceCompletion *completion);
typedef gchar* (*GtkSourceCompletionProposalGenInfo)  (GtkSourceCompletionProposal *proposal);

struct _GtkSourceCompletionProposalClass
{
	GObjectClass parent_class;
	
	gboolean (*apply) (GtkSourceCompletionProposal *proposal,
			   GtkSourceCompletion *completion,
			   gpointer user_data);
	gboolean (*display_info) (GtkSourceCompletionProposal *proposal,
				  GtkSourceCompletion *completion,
				  gpointer user_data);
};

struct _GtkSourceCompletionProposal
{
	GObject parent_instance;
	GtkSourceCompletionProposalPrivate *priv;
};

GType 
gtk_source_completion_proposal_get_type (void) G_GNUC_CONST;

/**
 * gtk_source_completion_proposal_new:
 * @label: Item label that will be shown in the completion popup. 
 * We copy this string
 * @info: Item info markup that will be shown when the user select to view the item info.
 * We copy this string
 * @icon: Item icon that will be shown in the completion popup
 *
 * This function creates a new proposal. By default, when the user selects the proposal, the 
 * proposal label will be inserted into the GtkTextView. You can connect to apply
 * and disply-info signals to overwrite the default functions
 *
 * Returns The new GtkSourceCompletionProposal
 */
GtkSourceCompletionProposal*
gtk_source_completion_proposal_new(const gchar *label,
				   const gchar *info,
				   const GdkPixbuf *icon);

/**
 * gtk_source_completion_proposal_get_label:
 * @proposal: The GtkSourceCompletionProposal
 *
 * Returns The proposal label
 */
const gchar*
gtk_source_completion_proposal_get_label(GtkSourceCompletionProposal *proposal);

/**
 * gtk_source_completion_proposal_get_icon:
 * @proposal: The GtkSourceCompletionProposal
 *
 * Returns the icon of this proposal
 */
const GdkPixbuf*
gtk_source_completion_proposal_get_icon(GtkSourceCompletionProposal *proposal);

/**
 * gtk_source_completion_proposal_set_page_name:
 * @proposal: The GtkSourceCompletionProposal
 * @page_name: The page name where this proposal will be shown
 *
 */
void
gtk_source_completion_proposal_set_page_name(GtkSourceCompletionProposal *self,
					     const gchar *page_name);
					     
/**
 * gtk_source_completion_proposal_get_page_name:
 * @proposal: The GtkSourceCompletionProposal
 *
 * Returns the page name where the proposal will be placed.
 */
const gchar*
gtk_source_completion_proposal_get_page_name(GtkSourceCompletionProposal *proposal);

/**
 * gtk_source_completion_proposal_get_info:
 * @proposal: The GtkSourceCompletionProposal
 *
 * Returns The proposal info markup asigned to this proposal.
 *
 */
const gchar* 
gtk_source_completion_proposal_get_info(GtkSourceCompletionProposal *proposal);

/**
 * gtk_source_completion_proposal_apply:
 * @proposal: The #GtkSourceCompletionProposal
 * @completion: The #GtkSourceCompletion 
 * 
 * The completion calls this function when the user selects the proposal. This 
 * function emits the "apply" signal. The default handler insert the proposal 
 * label into the view. You can overwrite this signal.
 *
 */
void
gtk_source_completion_proposal_apply(GtkSourceCompletionProposal *proposal,
					GtkSourceCompletion *completion);

/**
 * gtk_source_completion_proposal_display_info:
 * @proposal: The #GtkSourceCompletionProposal
 * @completion: The #GtkSourceCompletion 
 * 
 * The completion calls this function when the user want to view the proposal info.
 * This function emits the "display-info" signal. The default handler show the 
 * current info asigned to this proposal. You can overwrite this signal and set
 * the current info by hand using #gtk_source_completion_set_current_info.
 *
 */
void
gtk_source_completion_proposal_display_info(GtkSourceCompletionProposal *proposal,
					GtkSourceCompletion *completion);


G_END_DECLS

#endif

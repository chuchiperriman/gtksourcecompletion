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

#ifndef _GTKSOURCECOMPLETION_PROPOSAL_H
#define _GTKSOURCECOMPLETION_PROPOSAL_H

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>
#include "gtksourcecompletion.h"

G_BEGIN_DECLS

#define GTKSOURCECOMPLETION_TYPE_PROPOSAL             (gtksourcecompletion_proposal_get_type ())
#define GTKSOURCECOMPLETION_PROPOSAL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTKSOURCECOMPLETION_TYPE_PROPOSAL, GtkSourceCompletionProposal))
#define GTKSOURCECOMPLETION_PROPOSAL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTKSOURCECOMPLETION_TYPE_PROPOSAL, GtkSourceCompletionProposalClass)
#define GTKSOURCECOMPLETION_IS_PROPOSAL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTKSOURCECOMPLETION_TYPE_PROPOSAL))
#define GTKSOURCECOMPLETION_IS_PROPOSAL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTKSOURCECOMPLETION_TYPE_PROPOSAL))
#define GTKSOURCECOMPLETION_PROPOSAL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTKSOURCECOMPLETION_TYPE_PROPOSAL, GtkSourceCompletionProposalClass))

#define GTKSOURCECOMPLETION_PROPOSAL_DEFAULT_PAGE "Default"
#define GTKSOURCECOMPLETION_PROPOSAL_DEFAULT_PRIORITY 10

typedef struct _GtkSourceCompletionProposalPrivate GtkSourceCompletionProposalPrivate;
typedef struct _GtkSourceCompletionProposalClass GtkSourceCompletionProposalClass;
typedef struct _GtkSourceCompletionProposal GtkSourceCompletionProposal;

typedef void (*GtkSourceCompletionProposalApply)  (GtkSourceCompletionProposal *proposal,
						   GtkSourceCompletion *completion);
typedef gchar* (*GtkSourceCompletionProposalGenInfo)  (GtkSourceCompletionProposal *proposal);

struct _GtkSourceCompletionProposalClass
{
	GObjectClass parent_class;
};

struct _GtkSourceCompletionProposal
{
	GObject parent_instance;
	GtkSourceCompletionProposalPrivate *priv;
};

GType 
gtksourcecompletion_proposal_get_type (void) G_GNUC_CONST;

/* Default functions */
void 
gtk_source_completion_proposal_apply_default(GtkSourceCompletionProposal *proposal,
						  GtkSourceCompletion *completion);
gchar* 
gtk_source_completion_proposal_get_info_default(GtkSourceCompletionProposal *proposal);
/*********************/

/**
 * gtk_source_completion_proposal_new_full:
 * @id: An id for identify this proposal
 * @name: Item name that will be shown in the completion popup
 * @icon: Item icon that will be shown in the completion popup
 * @priority: The proposal priority. Items with high priority will be
 * 				shown first in the completion popup
 * @provider: The provider that creates the proposal
 * @page_name: The page name of this proposal. If NULL, the proposal will be shown 
 * in the default page.
 * @apply_func: This function will be called when the user selects the proposal
 * @inf_func: This function will be called to get the proposal info markup 
 * @user_data: User data used by the providers
 *
 * If a function is NULL then we assign the default function.
 *
 * Returns The new GtkSourceCompletionProposal
 */
GtkSourceCompletionProposal*
gtk_source_completion_proposal_new_full(int id,
				    const gchar *name,
				    const GdkPixbuf *icon,
				    int priority,
				    const gchar *page_name,
				    GtkSourceCompletionProposalApply apply_func,
				    GtkSourceCompletionProposalGenInfo info_func,
				    gpointer user_data);

/**
 * gtk_source_completion_proposal_new:
 * @id: An id for identify this proposal. This can be used by the provider but the 
 * completion does not use this value
 * @name: Item name that will be shown in the completion popup
 * @icon: Item icon that will be shown in the completion popup
 * @priority: The proposal priority. Items with high priority will be
 * 				shown first in the completion popup
 * @user_data: User data used by the providers
 *
 * This function creates a new proposal. When the user selects the proposal, the 
 * proposal name will be inserted into the GtkTextView. You can create the proposal
 * using #gtk_source_completion_proposal_new_full to set a custom function.
 *
 * The proposal has not info by default.
 *
 * Returns The new GtkSourceCompletionProposal
 */
GtkSourceCompletionProposal*
gtk_source_completion_proposal_new(int id,
				   const gchar *name,
				   const GdkPixbuf *icon,
				   int priority,
				   gpointer user_data);

/**
 * gtk_source_completion_proposal_get_id:
 * @proposal: The GtkSourceCompletionProposal
 *
 * Returns current proposal id
 *
 */
int
gtk_source_completion_proposal_get_id(GtkSourceCompletionProposal *proposal);

/**
 * gtk_source_completion_proposal_get_name:
 * @proposal: The GtkSourceCompletionProposal
 *
 * Returns The proposal name
 */
const gchar*
gtk_source_completion_proposal_get_name(GtkSourceCompletionProposal *proposal);

/**
 * gtk_source_completion_proposal_get_icon:
 * @proposal: The GtkSourceCompletionProposal
 *
 * Returns the icon of this proposal
 */
const GdkPixbuf*
gtk_source_completion_proposal_get_icon(GtkSourceCompletionProposal *proposal);

/**
 * gtk_source_completion_proposal_get_priority:
 * @proposal: The GtkSourceCompletionProposal
 *
 * Returns the proposal priority
 */
gint
gtk_source_completion_proposal_get_priority(GtkSourceCompletionProposal *proposal);

/**
 * gtk_source_completion_proposal_get_user_data:
 * @proposal: The GtkSourceCompletionProposal
 *
 * Returns the user data of this proposal
 */
gpointer
gtk_source_completion_proposal_get_user_data(GtkSourceCompletionProposal *proposal);

/**
 * gtk_source_completion_proposal_get_page_name:
 * @proposal: The GtkSourceCompletionProposal
 *
 * Returns the page name where the proposal will be placed.
 */
const gchar*
gtk_source_completion_proposal_get_page_name(GtkSourceCompletionProposal *proposal);

/**
 * gtk_source_completion_proposal_selected:
 * @proposal: The GtkSourceCompletionProposal
 * @completion: The #GtkSourceCompletion 
 * 
 * The completion calls this function when the user selects the proposal. This 
 * function will call to the #GtkSourceCompletionProposalApply setted on the creation.
 *
 */
void
gtk_source_completion_proposal_selected(GtkSourceCompletionProposal *proposal,
					GtkSourceCompletion *completion);

/**
 * gtk_source_completion_proposal_get_info_markup:
 * @proposal: The GtkSourceCompletionProposal
 *
 * The completion calls this function to get the proposal info to be shown into 
 * the info window. This info must to be a markup. You can use #g_markup_escape_text
 *
 * Returns The proposal info markup (new allocated).
 *
 */
gchar* 
gtk_source_completion_proposal_get_info_markup(GtkSourceCompletionProposal *proposal);

G_END_DECLS

#endif

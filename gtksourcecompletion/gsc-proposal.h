/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-proposal.h
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

#ifndef _GSC_PROPOSAL_H
#define _GSC_PROPOSAL_H

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GSC_TYPE_PROPOSAL             (gsc_proposal_get_type ())
#define GSC_PROPOSAL(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_PROPOSAL, GscProposal))
#define GSC_PROPOSAL_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_PROPOSAL, GscProposalClass)
#define GSC_IS_PROPOSAL(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_PROPOSAL))
#define GSC_IS_PROPOSAL_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_PROPOSAL))
#define GSC_PROPOSAL_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_PROPOSAL, GscProposalClass))

#define GSC_PROPOSAL_DEFAULT_PAGE "Default"
#define GSC_PROPOSAL_DEFAULT_PRIORITY 10

typedef struct _GscProposalPrivate GscProposalPrivate;
typedef struct _GscProposalClass GscProposalClass;
typedef struct _GscProposal GscProposal;

struct _GscProposalClass
{
	GObjectClass parent_class;
	
	gboolean (*apply) (GscProposal *proposal,
			   GtkTextView *view);
	const gchar* (*get_info) (GscProposal *proposal);
};

struct _GscProposal
{
	GObject parent_instance;
	GscProposalPrivate *priv;
};

GType 
gsc_proposal_get_type (void) G_GNUC_CONST;

/**
 * gsc_proposal_new:
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
 * Returns The new GscProposal
 */
GscProposal*
gsc_proposal_new(const gchar *label,
		const gchar *info,
		const GdkPixbuf *icon);

/**
 * gsc_proposal_get_label:
 * @proposal: The GscProposal
 *
 * Returns The proposal label
 */
const gchar*
gsc_proposal_get_label(GscProposal *proposal);

/**
 * gsc_proposal_get_icon:
 * @proposal: The GscProposal
 *
 * Returns the icon of this proposal
 */
const GdkPixbuf*
gsc_proposal_get_icon(GscProposal *proposal);

/**
 * gsc_proposal_set_page_name:
 * @proposal: The GscProposal
 * @page_name: The page name where this proposal will be shown
 *
 */
void
gsc_proposal_set_page_name(GscProposal *self,
			   const gchar *page_name);
					     
/**
 * gsc_proposal_get_page_name:
 * @proposal: The GscProposal
 *
 * Returns the page name where the proposal will be placed.
 */
const gchar*
gsc_proposal_get_page_name(GscProposal *proposal);

/**
 * gsc_proposal_get_info:
 * @proposal: The GscProposal
 *
 * Returns The proposal info markup asigned for this proposal.
 * The completion calls this function when the user want to view the proposal info.
 * You can overwrite this function if you need to change the default mechanism
 *
 */
const gchar* 
gsc_proposal_get_info(GscProposal *proposal);

/**
 * gsc_proposal_apply:
 * @proposal: The #GscProposal
 * @view: The #GtkTextView
 * 
 * The completion calls this function when the user selects the proposal. This 
 * function emits the "apply" signal. The default handler insert the proposal 
 * label into the view. You can overwrite this signal.
 *
 */
void
gsc_proposal_apply(GscProposal *proposal,
		   GtkTextView *view);

G_END_DECLS

#endif

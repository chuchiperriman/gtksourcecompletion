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

typedef struct _GscProposalPrivate GscProposalPrivate;
typedef struct _GscProposalClass GscProposalClass;
typedef struct _GscProposal GscProposal;

struct _GscProposal
{
	GObject parent_instance;
	
	GscProposalPrivate *priv;
};

struct _GscProposalClass
{
	GObjectClass parent_class;
	
	gboolean     (*apply)    (GscProposal *proposal,
				  GtkTextView *view);
	const gchar* (*get_info) (GscProposal *proposal);
};

GType			 gsc_proposal_get_type		(void) G_GNUC_CONST;

GscProposal		*gsc_proposal_new		(const gchar *label,
							 const gchar *info,
							 const GdkPixbuf *icon);

const gchar		*gsc_proposal_get_label		(GscProposal *proposal);

const GdkPixbuf		*gsc_proposal_get_icon		(GscProposal *proposal);


void			 gsc_proposal_set_page_name	(GscProposal *self,
							 const gchar *page_name);

const gchar		*gsc_proposal_get_page_name	(GscProposal *proposal);

const gchar		*gsc_proposal_get_info		(GscProposal *proposal);

void			 gsc_proposal_apply		(GscProposal *proposal,
							 GtkTextView *view);

G_END_DECLS

#endif

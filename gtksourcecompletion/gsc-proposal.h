/*
 * gscproposal.h
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

#ifndef __GSC_COMPLETION_PROPOSAL_H__
#define __GSC_COMPLETION_PROPOSAL_H__

#include <glib-object.h>
#include <gdk-pixbuf/gdk-pixbuf.h>

G_BEGIN_DECLS

#define GSC_TYPE_PROPOSAL			(gsc_proposal_get_type ())
#define GSC_COMPLETION_PROPOSAL(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_PROPOSAL, GscProposal))
#define GSC_IS_PROPOSAL(obj)			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_PROPOSAL))
#define GSC_COMPLETION_PROPOSAL_GET_INTERFACE(obj)	(G_TYPE_INSTANCE_GET_INTERFACE ((obj), GSC_TYPE_PROPOSAL, GscProposalIface))

typedef struct _GscProposal		GscProposal;
typedef struct _GscProposalIface	GscProposalIface;

struct _GscProposalIface
{
	GTypeInterface parent;
	
	/* Interface functions */
	const gchar 	*(*get_label)	(GscProposal *proposal);
	const gchar 	*(*get_markup)	(GscProposal *proposal);
	const gchar 	*(*get_text)	(GscProposal *proposal);
	
	GdkPixbuf	*(*get_icon)	(GscProposal *proposal);
	const gchar	*(*get_info)	(GscProposal *proposal);
	
	guint		 (*get_hash)	(GscProposal *proposal);
	gboolean	 (*equals)	(GscProposal *proposal1,
					 GscProposal *proposal2);
	
	/* Signals */
	void		 (*changed)	(GscProposal *proposal);
};

GType 			 gsc_proposal_get_type 	(void) G_GNUC_CONST;

const gchar		*gsc_proposal_get_label	(GscProposal *proposal);
const gchar		*gsc_proposal_get_markup	(GscProposal *proposal);
const gchar		*gsc_proposal_get_text	(GscProposal *proposal);

GdkPixbuf		*gsc_proposal_get_icon	(GscProposal *proposal);
const gchar		*gsc_proposal_get_info	(GscProposal *proposal);

guint			 gsc_proposal_get_hash	(GscProposal *proposal);
gboolean		 gsc_proposal_equals		(GscProposal *proposal1,
									 GscProposal *proposal2);

void			 gsc_proposal_changed		(GscProposal *proposal);

G_END_DECLS

#endif /* __GSC_COMPLETION_PROPOSAL_H__ */

/*
 * ##(FILENAME) - ##(DESCRIPTION)
 *
 * Copyright (C) ##(DATE_YEAR) - ##(AUTHOR_FULLNAME)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __GSC_PROPOSAL_##(PLUGIN_ID.upper)_H__
#define __GSC_PROPOSAL_##(PLUGIN_ID.upper)_H__

#include <glib.h>
#include <glib-object.h>
#include <gedit/gedit-plugin.h>
#include <gtksourcecompletion/gsc-proposal.h>

G_BEGIN_DECLS

#define GSC_TYPE_PROPOSAL_##(PLUGIN_ID.upper) (gsc_proposal_##(PLUGIN_ID.lower)_get_type ())
#define GSC_PROPOSAL_##(PLUGIN_ID.upper)(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_PROPOSAL_##(PLUGIN_ID.upper), GscProposal##(PLUGIN_ID.camel)))
#define GSC_PROPOSAL_##(PLUGIN_ID.upper)_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_PROPOSAL_##(PLUGIN_ID.upper), GscProposal##(PLUGIN_ID.camel)Class))
#define GSC_IS_PROPOSAL_##(PLUGIN_ID.upper)(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_PROPOSAL_##(PLUGIN_ID.upper)))
#define GSC_IS_PROPOSAL_##(PLUGIN_ID.upper)_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_PROPOSAL_##(PLUGIN_ID.upper)))
#define GSC_PROPOSAL_##(PLUGIN_ID.upper)_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_PROPOSAL_##(PLUGIN_ID.upper), GscProposal##(PLUGIN_ID.camel)Class))

typedef struct _GscProposal##(PLUGIN_ID.camel) GscProposal##(PLUGIN_ID.camel);
typedef struct _GscProposal##(PLUGIN_ID.camel)Class GscProposal##(PLUGIN_ID.camel)Class;
typedef struct _GscProposal##(PLUGIN_ID.camel)Private GscProposal##(PLUGIN_ID.camel)Private;

struct _GscProposal##(PLUGIN_ID.camel) {
	GscProposal parent;
	GscProposal##(PLUGIN_ID.camel)Private *priv;	
};

struct _GscProposal##(PLUGIN_ID.camel)Class {
	GscProposalClass parent;
};

GType			 gsc_proposal_##(PLUGIN_ID.lower)_get_type	();

GscProposal		*gsc_proposal_##(PLUGIN_ID.lower)_new		(const gchar *label,
									 const gchar *info,
									 GdkPixbuf *icon);

G_END_DECLS

#endif

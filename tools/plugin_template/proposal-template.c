 /* ##(FILENAME) - ##(DESCRIPTION)
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

#include "gsc-proposal-##(PLUGIN_ID.lower).h"

struct _GscProposal##(PLUGIN_ID.camel)Private
{
	gchar *dummy;
};

G_DEFINE_TYPE(GscProposal##(PLUGIN_ID.camel), gsc_proposal_##(PLUGIN_ID.lower), GSC_TYPE_PROPOSAL);

#define GSC_PROPOSAL_##(PLUGIN_ID.upper)_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GSC_TYPE_PROPOSAL_##(PLUGIN_ID.upper), GscProposal##(PLUGIN_ID.camel)Private))
 
static gboolean
gsc_proposal_##(PLUGIN_ID.lower)_apply(GscProposal* proposal, GtkTextView *view)
{
	/*Do the work when the user selects this proposal*/
	return TRUE;
}

static void
gsc_proposal_##(PLUGIN_ID.lower)_finalize (GObject *object)
{
	G_OBJECT_CLASS (gsc_proposal_##(PLUGIN_ID.lower)_parent_class)->finalize (object);
}

static void
gsc_proposal_##(PLUGIN_ID.lower)_class_init (GscProposal##(PLUGIN_ID.camel)Class *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	g_type_class_add_private (object_class, sizeof(GscProposal##(PLUGIN_ID.camel)Private));
	
	GscProposalClass *proposal_class = GSC_PROPOSAL_CLASS (klass);
	proposal_class->apply = gsc_proposal_##(PLUGIN_ID.lower)_apply;
}

static void
gsc_proposal_##(PLUGIN_ID.lower)_init (GscProposal##(PLUGIN_ID.camel) *self)
{
	self->priv = GSC_PROPOSAL_##(PLUGIN_ID.upper)_GET_PRIVATE (self);
}

GscProposal*
gsc_proposal_##(PLUGIN_ID.lower)_new (const gchar *label,
				      const gchar *info,
				      GdkPixbuf *icon)
{
	GscProposal##(PLUGIN_ID.camel) *self = GSC_PROPOSAL_##(PLUGIN_ID.upper) (g_object_new (GSC_TYPE_PROPOSAL_##(PLUGIN_ID.upper), 
								 "label", label,
								 "info", info,
								 "icon", icon,
								 NULL));
	return GSC_PROPOSAL (self);
}



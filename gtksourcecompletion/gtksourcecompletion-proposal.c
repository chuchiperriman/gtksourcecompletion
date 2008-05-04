/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-proposal.c
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
 
  
#include "gtksourcecompletion-proposal.h"
#include "gtksourcecompletion-utils.h"

struct _GtkSourceCompletionProposalPrivate
{
	int id;
	gchar *name;
	const GdkPixbuf *icon;
	int priority;
	gpointer user_data;
	const gchar *page_name;
	GtkSourceCompletionProposalApply apply_func;
	GtkSourceCompletionProposalGenInfo get_info_func;
};

static GObjectClass* parent_class = NULL;

#define GTK_SOURCE_COMPLETION_PROPOSAL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GTK_SOURCE_COMPLETION_TYPE_PROPOSAL, GtkSourceCompletionProposalPrivate))


static void
gtk_source_completion_proposal_init (GtkSourceCompletionProposal *self)
{
	self->priv = GTK_SOURCE_COMPLETION_PROPOSAL_GET_PRIVATE(self);
	
	g_debug("Created GtkSourceCompletionProposal");
	self->priv->id = -1;
	self->priv->name = NULL;
	self->priv->icon = NULL;
	self->priv->priority = GTK_SOURCE_COMPLETION_PROPOSAL_DEFAULT_PRIORITY;
	self->priv->user_data = NULL;
	self->priv->page_name = GTK_SOURCE_COMPLETION_PROPOSAL_DEFAULT_PAGE;
}

static void
gtk_source_completion_proposal_finalize (GObject *object)
{
	GtkSourceCompletionProposal *self = GTK_SOURCE_COMPLETION_PROPOSAL(object);
	g_debug("Free GtkSourceCompletionProposal");
	g_free(self->priv->name);
	
	G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_source_completion_proposal_class_init (GtkSourceCompletionProposalClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));

	object_class->finalize = gtk_source_completion_proposal_finalize;

	g_type_class_add_private (object_class, sizeof(GtkSourceCompletionProposalPrivate));
}

GType
gtk_source_completion_proposal_get_type (void)
{
	static GType our_type = 0;

	if(our_type == 0)
	{
		static const GTypeInfo our_info =
		{
			sizeof (GtkSourceCompletionProposalClass), /* class_size */
			(GBaseInitFunc) NULL, /* base_init */
			(GBaseFinalizeFunc) NULL, /* base_finalize */
			(GClassInitFunc) gtk_source_completion_proposal_class_init, /* class_init */
			(GClassFinalizeFunc) NULL, /* class_finalize */
			NULL /* class_data */,
			sizeof (GtkSourceCompletionProposal), /* instance_size */
			0, /* n_preallocs */
			(GInstanceInitFunc) gtk_source_completion_proposal_init, /* instance_init */
			NULL /* value_table */  
		};
		our_type = g_type_register_static (G_TYPE_OBJECT, "GtkSourceCompletionProposal",
		                                   &our_info, 0);
	}

	return our_type;
}

GtkSourceCompletionProposal*
gtk_source_completion_proposal_new_full(int id,
				    const gchar *name,
				    const GdkPixbuf *icon,
				    int priority,
				    const gchar *page_name,
				    GtkSourceCompletionProposalApply apply_func,
				    GtkSourceCompletionProposalGenInfo info_func,
				    gpointer user_data)
{
	GtkSourceCompletionProposal *self = 
		GTK_SOURCE_COMPLETION_PROPOSAL(g_object_new (GTK_SOURCE_COMPLETION_TYPE_PROPOSAL, NULL));
	self->priv->id = id;
	self->priv->name = g_strdup(name);
	self->priv->icon = icon;
	self->priv->priority = priority;
	self->priv->user_data = user_data;
	self->priv->page_name = page_name!=NULL ? page_name : GTK_SOURCE_COMPLETION_PROPOSAL_DEFAULT_PAGE;
	self->priv->apply_func = apply_func!=NULL ? apply_func : gtk_source_completion_proposal_apply_default;
	self->priv->get_info_func = info_func!=NULL ? info_func : gtk_source_completion_proposal_get_info_default;
		
	return self;
}

GtkSourceCompletionProposal*
gtk_source_completion_proposal_new(int id,
				   const gchar *name,
				   const GdkPixbuf *icon,
				   int priority,
				   gpointer user_data)
{
	return gtk_source_completion_proposal_new_full(id,
						       name,
						       icon,
						       priority,
						       NULL,
						       NULL,
						       NULL,
						       user_data);
}

int
gtk_source_completion_proposal_get_id(GtkSourceCompletionProposal *self)
{
	return self->priv->id;
}

const gchar*
gtk_source_completion_proposal_get_name(GtkSourceCompletionProposal *self)
{
	return self->priv->name;
}

const GdkPixbuf*
gtk_source_completion_proposal_get_icon(GtkSourceCompletionProposal *self)
{
	return self->priv->icon;
}

gpointer
gtk_source_completion_proposal_get_user_data(GtkSourceCompletionProposal *self)
{
	return self->priv->user_data;
}

const gchar*
gtk_source_completion_proposal_get_page_name(GtkSourceCompletionProposal *self)
{
	return self->priv->page_name;
}

void
gtk_source_completion_proposal_selected(GtkSourceCompletionProposal *self,
					GtkSourceCompletion *completion)
{
	self->priv->apply_func(self,completion);
}

gchar* 
gtk_source_completion_proposal_get_info_markup(GtkSourceCompletionProposal *self)
{
	return self->priv->get_info_func(self);
}

gint
gtk_source_completion_proposal_get_priority(GtkSourceCompletionProposal *self)
{
	return self->priv->priority;
}

/*Default functions*/
void 
gtk_source_completion_proposal_apply_default(GtkSourceCompletionProposal *self,
					     GtkSourceCompletion *completion)
{
	GtkTextView *view = gtk_source_completion_get_view(completion);
	gtk_source_view_replace_actual_word(view,
					    gtk_source_completion_proposal_get_name(self));
}

gchar*
gtk_source_completion_proposal_get_info_default(GtkSourceCompletionProposal *proposal)
{
	return NULL;
}


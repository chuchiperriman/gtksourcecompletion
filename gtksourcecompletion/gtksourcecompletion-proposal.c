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
#include "gtksourcecompletion-marshal.h"

/* Signals */
enum {
	APPLY,
	DISPLAY_INFO,
	LAST_SIGNAL
};

struct _GtkSourceCompletionProposalPrivate
{
	gchar *label;
	gchar *info;
	const GdkPixbuf *icon;
	const gchar *page_name;
};

static GObjectClass* parent_class = NULL;

#define GTK_SOURCE_COMPLETION_PROPOSAL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GTK_TYPE_SOURCE_COMPLETION_PROPOSAL, GtkSourceCompletionProposalPrivate))

static guint signals[LAST_SIGNAL] = { 0 };

static gboolean
gtk_source_completion_proposal_apply_default(GtkSourceCompletionProposal *self,
					     GtkSourceCompletion *completion)
{
	GtkTextView *view = gtk_source_completion_get_view(completion);
	gtk_source_view_replace_actual_word(view,
					    self->priv->label);
	return FALSE;
}

static gboolean
gtk_source_completion_proposal_display_info_default(GtkSourceCompletionProposal *self,
					     GtkSourceCompletion *completion)
{
	gtk_source_completion_set_current_info(completion,self->priv->info);
	return FALSE;
}

static void
gtk_source_completion_proposal_init (GtkSourceCompletionProposal *self)
{
	self->priv = GTK_SOURCE_COMPLETION_PROPOSAL_GET_PRIVATE(self);
	
	g_debug("Created GtkSourceCompletionProposal");
	self->priv->label = NULL;
	self->priv->info = NULL;
	self->priv->icon = NULL;
	self->priv->page_name = GTK_SOURCE_COMPLETION_PROPOSAL_DEFAULT_PAGE;
}

static void
gtk_source_completion_proposal_finalize (GObject *object)
{
	GtkSourceCompletionProposal *self = GTK_SOURCE_COMPLETION_PROPOSAL(object);
	g_debug("Free GtkSourceCompletionProposal");
	g_free(self->priv->label);
	g_free(self->priv->info);
	
	G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gtk_source_completion_proposal_class_init (GtkSourceCompletionProposalClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));

	object_class->finalize = gtk_source_completion_proposal_finalize;

	g_type_class_add_private (object_class, sizeof(GtkSourceCompletionProposalPrivate));
	
	klass->apply = gtk_source_completion_proposal_apply_default;
	klass->display_info = gtk_source_completion_proposal_display_info_default;
	
	signals [APPLY] =
		g_signal_new ("apply",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GtkSourceCompletionProposalClass, apply),
			      g_signal_accumulator_true_handled,
			      NULL,
			      gtksourcecompletion_marshal_BOOLEAN__POINTER,
			      G_TYPE_BOOLEAN,
			      1,
			      GTK_TYPE_POINTER);
	signals [DISPLAY_INFO] =
		g_signal_new ("display-info",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GtkSourceCompletionProposalClass, display_info),
			      g_signal_accumulator_true_handled,
			      NULL,
			      gtksourcecompletion_marshal_BOOLEAN__POINTER,
			      G_TYPE_BOOLEAN,
			      1,
			      GTK_TYPE_POINTER);
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
gtk_source_completion_proposal_new(const gchar *label,
				   const gchar *info,
				   const GdkPixbuf *icon)
{
	GtkSourceCompletionProposal *self = 
		GTK_SOURCE_COMPLETION_PROPOSAL(g_object_new (GTK_TYPE_SOURCE_COMPLETION_PROPOSAL, NULL));
	self->priv->label = g_strdup(label);
	self->priv->info = g_strdup(info);
	self->priv->icon = icon;
		
	return self;
}

const gchar*
gtk_source_completion_proposal_get_label(GtkSourceCompletionProposal *self)
{
	return self->priv->label;
}

const GdkPixbuf*
gtk_source_completion_proposal_get_icon(GtkSourceCompletionProposal *self)
{
	return self->priv->icon;
}

void
gtk_source_completion_proposal_set_page_name(GtkSourceCompletionProposal *self,
					     const gchar *page_name)
{
	self->priv->page_name = page_name;
}

const gchar*
gtk_source_completion_proposal_get_page_name(GtkSourceCompletionProposal *self)
{
	return self->priv->page_name;
}

const gchar* 
gtk_source_completion_proposal_get_info(GtkSourceCompletionProposal *self)
{
	return self->priv->info;
}

void
gtk_source_completion_proposal_apply(GtkSourceCompletionProposal *self,
					GtkSourceCompletion *completion)
{
	gboolean ret = TRUE;
	g_signal_emit_by_name (self, "apply",completion,&ret);
}

void
gtk_source_completion_proposal_display_info(GtkSourceCompletionProposal *self,
					    GtkSourceCompletion *completion)
{
	gboolean ret = TRUE;
	g_signal_emit_by_name (self, "display-info",completion,&ret);
}


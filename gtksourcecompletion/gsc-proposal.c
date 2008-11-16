/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-proposal.c
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
 
  
#include "gsc-proposal.h"
#include "gsc-utils.h"
#include "gsc-marshal.h"
#include "gsc-i18n.h"

/* Signals */
enum {
	APPLY,
	DISPLAY_INFO,
	LAST_SIGNAL
};

/* Properties */
enum {
	PROP_0,
	PROP_LABEL,
	PROP_INFO,
	PROP_ICON
};

struct _GscProposalPrivate
{
	gchar *label;
	gchar *info;
	const GdkPixbuf *icon;
	const gchar *page_name;
};

static GObjectClass* parent_class = NULL;

#define GSC_PROPOSAL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GSC_TYPE_PROPOSAL, GscProposalPrivate))

static guint signals[LAST_SIGNAL] = { 0 };

static gboolean
gsc_proposal_apply_default(GscProposal *self,
			   GtkTextView *view)
{
	gsc_replace_actual_word(view,
				self->priv->label);
	return FALSE;
}

static const gchar*
gsc_proposal_get_info_default(GscProposal *self)
{
	return self->priv->info;
}

static void
gsc_proposal_init (GscProposal *self)
{
	self->priv = GSC_PROPOSAL_GET_PRIVATE(self);
	
	self->priv->label = NULL;
	self->priv->info = NULL;
	self->priv->icon = NULL;
	self->priv->page_name = GSC_PROPOSAL_DEFAULT_PAGE;
}

static void
gsc_proposal_finalize (GObject *object)
{
	GscProposal *self = GSC_PROPOSAL(object);
	g_free(self->priv->label);
	g_free(self->priv->info);
	
	G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gsc_proposal_get_property (GObject    *object,
				    guint       prop_id,
				    GValue     *value,
				    GParamSpec *pspec)
{
	GscProposal *self;

	g_return_if_fail (GSC_IS_PROPOSAL (object));

	self = GSC_PROPOSAL(object);

	switch (prop_id)
	{
			
		case PROP_LABEL:
			g_value_set_string(value,self->priv->label);
			break;
		case PROP_INFO:
			g_value_set_string(value,
					   self->priv->info);
			break;
		case PROP_ICON:
			g_value_set_pointer(value,
					   (gpointer)self->priv->icon);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gsc_proposal_set_property (GObject      *object,
					    guint         prop_id,
					    const GValue *value,
					    GParamSpec   *pspec)
{
	GscProposal *self;

	g_return_if_fail (GSC_IS_PROPOSAL (object));

	self = GSC_PROPOSAL(object);

	switch (prop_id)
	{
		case PROP_LABEL:
			self->priv->label = g_value_dup_string(value);
			break;
		case PROP_INFO:
			self->priv->info = g_value_dup_string(value);
			break;
		case PROP_ICON:
			self->priv->icon = (GdkPixbuf*)g_value_get_pointer(value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gsc_proposal_class_init (GscProposalClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));

	object_class->get_property = gsc_proposal_get_property;
	object_class->set_property = gsc_proposal_set_property;
	object_class->finalize = gsc_proposal_finalize;

	g_type_class_add_private (object_class, sizeof(GscProposalPrivate));
	
	klass->apply = gsc_proposal_apply_default;
	klass->get_info = gsc_proposal_get_info_default;
	
	/* Proposal properties */
	
	/**
	 * GscProposal:label:
	 *
	 * Label to be shown for this proposal
	 */
	g_object_class_install_property (object_class,
					 PROP_LABEL,
					 g_param_spec_string ("label",
							      _("Label to be shown for this proposal"),
							      _("Label to be shown for this proposal"),
							      NULL,
							      G_PARAM_READWRITE));
	/**
	 * GscProposal:info:
	 *
	 * Info to be shown for this proposal
	 */
	g_object_class_install_property (object_class,
					 PROP_INFO,
					 g_param_spec_string ("info",
							      _("Info to be shown for this proposal"),
							      _("Info to be shown for this proposal"),
							      NULL,
							      G_PARAM_READWRITE));
	/**
	 * GscProposal:icon:
	 *
	 * Icon to be shown for this proposal
	 */
	g_object_class_install_property (object_class,
					 PROP_ICON,
					 g_param_spec_pointer ("icon",
							      _("Icon to be shown for this proposal"),
							      _("Icon to be shown for this proposal"),
							      G_PARAM_READWRITE));
	
	/* Proposal Signals */
	/**
	 * GscProposal::apply:
	 * @proposal: The proposal who emits the signal
	 * @view: The #GtkTextView where the proposal must be applied
	 *
	 * The ::apply signal is emitted when the proposal has been selected
	 * and must to be applied.
	 *
	 **/
	signals [APPLY] =
		g_signal_new ("apply",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GscProposalClass, apply),
			      g_signal_accumulator_true_handled,
			      NULL,
			      gtksourcecompletion_marshal_BOOLEAN__POINTER,
			      G_TYPE_BOOLEAN,
			      1,
			      GTK_TYPE_POINTER);
}

GType
gsc_proposal_get_type (void)
{
	static GType our_type = 0;

	if(our_type == 0)
	{
		static const GTypeInfo our_info =
		{
			sizeof (GscProposalClass), /* class_size */
			(GBaseInitFunc) NULL, /* base_init */
			(GBaseFinalizeFunc) NULL, /* base_finalize */
			(GClassInitFunc) gsc_proposal_class_init, /* class_init */
			(GClassFinalizeFunc) NULL, /* class_finalize */
			NULL /* class_data */,
			sizeof (GscProposal), /* instance_size */
			0, /* n_preallocs */
			(GInstanceInitFunc) gsc_proposal_init, /* instance_init */
			NULL /* value_table */  
		};
		our_type = g_type_register_static (G_TYPE_OBJECT, "GscProposal",
		                                   &our_info, 0);
	}

	return our_type;
}

GscProposal*
gsc_proposal_new(const gchar *label,
		 const gchar *info,
		 const GdkPixbuf *icon)
{
	GscProposal *self = 
		GSC_PROPOSAL(g_object_new (GSC_TYPE_PROPOSAL, NULL));
	self->priv->label = g_strdup(label);
	self->priv->info = g_strdup(info);
	self->priv->icon = icon;
		
	return self;
}

const gchar*
gsc_proposal_get_label(GscProposal *self)
{
	return self->priv->label;
}

const GdkPixbuf*
gsc_proposal_get_icon(GscProposal *self)
{
	return self->priv->icon;
}

void
gsc_proposal_set_page_name(GscProposal *self,
					     const gchar *page_name)
{
	self->priv->page_name = page_name;
}

const gchar*
gsc_proposal_get_page_name(GscProposal *self)
{
	return self->priv->page_name;
}

const gchar* 
gsc_proposal_get_info(GscProposal *self)
{
	return self->priv->info;
}

void
gsc_proposal_apply(GscProposal *self,
		   GtkTextView *view)
{
	gboolean ret = TRUE;
	g_signal_emit_by_name (self, "apply", view, &ret);
}


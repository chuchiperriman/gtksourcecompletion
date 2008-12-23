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

#define GSC_PROPOSAL_DEFAULT_PAGE _("Default")
#define GSC_PROPOSAL_DEFAULT_PRIORITY 10

#define GSC_PROPOSAL_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), \
					 GSC_TYPE_PROPOSAL, GscProposalPrivate))

G_DEFINE_TYPE(GscProposal, gsc_proposal, G_TYPE_OBJECT);

struct _GscProposalPrivate
{
	gchar *label;
	gchar *info;
	GdkPixbuf *icon;
	gchar *page_name;
};

/* Properties */
enum
{
	PROP_0,
	PROP_LABEL,
	PROP_INFO,
	PROP_ICON,
	PROP_PAGE_NAME
};

static gboolean
gsc_proposal_apply_default (GscProposal *self,
			    GtkTextView *view)
{
	gsc_replace_actual_word (view,
				 self->priv->label);
	return FALSE;
}

static const gchar*
gsc_proposal_get_info_default (GscProposal *self)
{
	return self->priv->info;
}

static void
gsc_proposal_init (GscProposal *self)
{
	self->priv = GSC_PROPOSAL_GET_PRIVATE (self);
	
	self->priv->label = NULL;
	self->priv->info = NULL;
	self->priv->icon = NULL;
	self->priv->page_name = g_strdup (GSC_PROPOSAL_DEFAULT_PAGE);
}

static void
gsc_proposal_finalize (GObject *object)
{
	GscProposal *self = GSC_PROPOSAL (object);
	
	g_free (self->priv->label);
	g_free (self->priv->info);
	g_free (self->priv->page_name);
	
	if (self->priv->icon != NULL)
		g_object_unref(self->priv->icon);
	
	G_OBJECT_CLASS (gsc_proposal_parent_class)->finalize (object);
}

static void
gsc_proposal_get_property (GObject    *object,
			   guint       prop_id,
			   GValue     *value,
			   GParamSpec *pspec)
{
	GscProposal *self;

	g_return_if_fail (GSC_IS_PROPOSAL (object));

	self = GSC_PROPOSAL (object);

	switch (prop_id)
	{
			
		case PROP_LABEL:
			g_value_set_string (value,self->priv->label);
			break;
		case PROP_INFO:
			g_value_set_string (value,
					    self->priv->info);
			break;
		case PROP_ICON:
			g_value_set_pointer (value,
					     (gpointer)self->priv->icon);
			break;
		case PROP_PAGE_NAME:
			g_value_set_stringr (value,
					     self->priv->page_name);
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

	self = GSC_PROPOSAL (object);

	switch (prop_id)
	{
		case PROP_LABEL:
			self->priv->label = g_value_dup_string (value);
			break;
		case PROP_INFO:
			self->priv->info = g_value_dup_string (value);
			break;
		case PROP_ICON:
			if (self->priv->icon != NULL)
				g_object_unref (self->priv->icon);

			self->priv->icon = g_object_ref ((GdkPixbuf*)g_value_get_pointer (value));
			break;
		case PROP_PAGE_NAME:
			self->priv->page_name = g_value_dup_string (value);
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

	object_class->get_property = gsc_proposal_get_property;
	object_class->set_property = gsc_proposal_set_property;
	object_class->finalize = gsc_proposal_finalize;

	g_type_class_add_private (object_class, sizeof (GscProposalPrivate));
	
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

	/**
	 * GscProposal:page-name:
	 *
	 * Page name for this proposal
	 */
	g_object_class_install_property (object_class,
					 PROP_PAGE_NAME,
					 g_param_spec_string ("page-name",
							      _("Page name for this proposal"),
							      _("Page name for this proposal"),
							      NULL,
							      G_PARAM_READWRITE));
}

/**
 * gsc_proposal_new:
 * @label: Item label that will be shown in the completion popup. 
 * @info: Item info markup that will be shown when the user select to view the item info.
 * @icon: Item icon that will be shown in the completion popup
 *
 * This function creates a new #GscProposal. By default, when the user selects 
 * the proposal, the proposal label will be inserted into the GtkTextView.
 * You can overwrite the apply and disply-info functions to overwrite the default.
 *
 * Returns: A new #GscProposal
 */
GscProposal*
gsc_proposal_new (const gchar *label,
		  const gchar *info,
		  GdkPixbuf *icon)
{
	GscProposal *self;
	
	self = GSC_PROPOSAL (g_object_new (GSC_TYPE_PROPOSAL, NULL));
	
	self->priv->label = g_strdup (label);
	self->priv->info = g_strdup (info);
	if (icon != NULL)
	{
		self->priv->icon = g_object_ref (icon);
	}
	else
	{
		self->priv->icon = NULL;
	}
		
	
	return self;
}

/**
 * gsc_proposal_get_label:
 * @proposal: a #GscProposal
 *
 * Returns: The proposal label that will be shown into the popup
 */
const gchar*
gsc_proposal_get_label (GscProposal *self)
{
	g_return_val_if_fail (GSC_IS_PROPOSAL (self), NULL);
	
	return self->priv->label;
}

/**
 * gsc_proposal_get_icon:
 * @proposal: a #GscProposal
 *
 * Gets the icon of this @proposal that will be shown into the popup.
 *
 * Returns: the icon of this @proposal that will be shown into the popup
 */
const GdkPixbuf*
gsc_proposal_get_icon (GscProposal *self)
{
	g_return_val_if_fail (GSC_IS_PROPOSAL (self), NULL);

	return self->priv->icon;
}

/**
 * gsc_proposal_set_page_name:
 * @proposal: a #GscProposal
 * @page_name: The name for the page
 *
 * Sets the name of the page where this proposal will be shown.
 * If @page_name is %NULL the default page will be used.
 */
void
gsc_proposal_set_page_name (GscProposal *self,
			    const gchar *page_name)
{
	g_return_if_fail (GSC_IS_PROPOSAL (self));

	g_free (self->priv->page_name);
	
	if (page_name == NULL)
	{
		self->priv->page_name = g_strdup (GSC_PROPOSAL_DEFAULT_PAGE);
	}
	else
	{
		self->priv->page_name = g_strdup (page_name);
	}
}

/**
 * gsc_proposal_get_page_name:
 * @proposal: a #GscProposal
 *
 * Gets the page name where the @proposal will be placed.
 *
 * Returns: the page name where the @proposal will be placed.
 */
const gchar*
gsc_proposal_get_page_name (GscProposal *self)
{
	g_return_val_if_fail (GSC_IS_PROPOSAL (self), NULL);
	
	return self->priv->page_name;
}

/**
 * gsc_proposal_get_info:
 * @proposal: a #GscProposal
 *
 * The completion calls this function when the user wants to see the proposal info.
 * You can overwrite this function if you need to change the default mechanism.
 *
 * Returns: The proposal info markup asigned for this proposal or NULL;
 */
const gchar* 
gsc_proposal_get_info (GscProposal *self)
{
	g_return_val_if_fail (GSC_IS_PROPOSAL (self), NULL);

	return GSC_PROPOSAL_GET_CLASS (self)->get_info (self);
}

/**
 * gsc_proposal_apply:
 * @proposal: a #GscProposal
 * @view: The #GtkTextView
 * 
 * The completion calls this function when the user selects the proposal. 
 * The default handler insert the proposal label into the view. 
 * You can overwrite this function.
 *
 */
void
gsc_proposal_apply (GscProposal *self,
		    GtkTextView *view)
{
	g_return_if_fail (GSC_IS_PROPOSAL (self));
	
	GSC_PROPOSAL_GET_CLASS (self)->apply (self, view);
}


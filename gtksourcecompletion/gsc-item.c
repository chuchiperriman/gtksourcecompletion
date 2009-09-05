/*
 * gscitem.c
 * This file is part of gsc
 *
 * Copyright (C) 2009 - Jesse van den Kieboom
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#include <gtksourceview/gscitem.h>

#include "gsc-utils.h"
#include "gsc-i18n.h"

#define GSC_COMPLETION_ITEM_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE((object), GSC_TYPE_ITEM, GscItemPrivate))

struct _GscItemPrivate
{
	gchar *label;
	gchar *markup;
	gchar *text;
	gchar *info;
	GdkPixbuf *icon;
};

/* Properties */
enum
{
	PROP_0,
	PROP_LABEL,
	PROP_MARKUP,
	PROP_TEXT,
	PROP_ICON,
	PROP_INFO
};

static void gsc_completion_proposal_iface_init (gpointer g_iface, gpointer iface_data);

G_DEFINE_TYPE_WITH_CODE (GscItem, 
			 gsc_completion_item, 
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GSC_TYPE_PROPOSAL,
			 			gsc_completion_proposal_iface_init))

static const gchar *
gsc_completion_proposal_get_label_impl (GscProposal *self)
{
	return GSC_COMPLETION_ITEM (self)->priv->label;
}

static const gchar *
gsc_completion_proposal_get_markup_impl (GscProposal *self)
{
	return GSC_COMPLETION_ITEM (self)->priv->markup;
}

static const gchar *
gsc_completion_proposal_get_text_impl (GscProposal *self)
{
	return GSC_COMPLETION_ITEM (self)->priv->text;
}

static GdkPixbuf *
gsc_completion_proposal_get_icon_impl (GscProposal *self)
{
	return GSC_COMPLETION_ITEM (self)->priv->icon;
}

static const gchar *
gsc_completion_proposal_get_info_impl (GscProposal *self)
{
	return GSC_COMPLETION_ITEM (self)->priv->info;
}

static void
gsc_completion_proposal_iface_init (gpointer g_iface, 
					   gpointer iface_data)
{
	GscProposalIface *iface = (GscProposalIface *)g_iface;
	
	/* Interface data getter implementations */
	iface->get_label = gsc_completion_proposal_get_label_impl;
	iface->get_markup = gsc_completion_proposal_get_markup_impl;
	iface->get_text = gsc_completion_proposal_get_text_impl;
	iface->get_icon = gsc_completion_proposal_get_icon_impl;
	iface->get_info = gsc_completion_proposal_get_info_impl;
}

static void
gsc_completion_item_finalize (GObject *object)
{
	GscItem *self = GSC_COMPLETION_ITEM(object);
	
	g_free (self->priv->label);
	g_free (self->priv->markup);
	g_free (self->priv->text);

	g_free (self->priv->info);
	
	if (self->priv->icon != NULL)
	{
		g_object_unref (self->priv->icon);
	}

	G_OBJECT_CLASS (gsc_completion_item_parent_class)->finalize (object);
}

static void
gsc_completion_item_get_property (GObject    *object,
					 guint       prop_id,
					 GValue     *value,
					 GParamSpec *pspec)
{
	GscItem *self;

	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_ITEM (object));

	self = GSC_COMPLETION_ITEM (object);

	switch (prop_id)
	{
		case PROP_LABEL:
			g_value_set_string (value, self->priv->label);
			break;
		case PROP_MARKUP:
			g_value_set_string (value, self->priv->markup);
			break;
		case PROP_TEXT:
			g_value_set_string (value, self->priv->text);
			break;
		case PROP_INFO:
			g_value_set_string (value, self->priv->info);
			break;
		case PROP_ICON:
			g_value_set_object (value, self->priv->icon);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
emit_changed (GscItem *item)
{
	gsc_completion_proposal_changed (GSC_COMPLETION_PROPOSAL (item));
}

static void
gsc_completion_item_set_property (GObject      *object,
					 guint         prop_id,
					 const GValue *value,
					 GParamSpec   *pspec)
{
	GscItem *self;

	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_ITEM (object));

	self = GSC_COMPLETION_ITEM (object);

	switch (prop_id)
	{
		case PROP_LABEL:
			g_free (self->priv->label);
			self->priv->label = g_value_dup_string (value);
			
			emit_changed (self);
			break;
		case PROP_MARKUP:
			g_free (self->priv->markup);
			self->priv->markup = g_value_dup_string (value);
			
			emit_changed (self);
			break;
		case PROP_TEXT:
			g_free (self->priv->text);
			self->priv->text = g_value_dup_string (value);
			break;
		case PROP_INFO:
			g_free (self->priv->info);
			self->priv->info = g_value_dup_string (value);
			
			emit_changed (self);
			break;
		case PROP_ICON:
			if (self->priv->icon != NULL)
			{
				g_object_unref (self->priv->icon);
			}
			
			self->priv->icon = GDK_PIXBUF (g_value_dup_object (value));
			emit_changed (self);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gsc_completion_item_class_init (GscItemClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	object_class->finalize = gsc_completion_item_finalize;
	object_class->get_property = gsc_completion_item_get_property;
	object_class->set_property = gsc_completion_item_set_property;

	/**
	 * GscItem:label:
	 *
	 * Label to be shown for this proposal.
	 */
	g_object_class_install_property (object_class,
					 PROP_LABEL,
					 g_param_spec_string ("label",
							      _("Label"),
							      _("Label to be shown for this item"),
							      NULL,
							      G_PARAM_READWRITE));

	/**
	 * GscItem:markup:
	 *
	 * Label with markup to be shown for this proposal.
	 */
	g_object_class_install_property (object_class,
					 PROP_MARKUP,
					 g_param_spec_string ("markup",
							      _("Markup"),
							      _("Markup to be shown for this item"),
							      NULL,
							      G_PARAM_READWRITE));

	/**
	 * GscItem:text:
	 *
	 * Proposal text.
	 */
	g_object_class_install_property (object_class,
					 PROP_TEXT,
					 g_param_spec_string ("text",
							      _("Text"),
							      _("Item text"),
							      NULL,
							      G_PARAM_READWRITE));

	/**
	 * GscItem:icon:
	 *
	 * Icon to be shown for this proposal.
	 */
	g_object_class_install_property (object_class,
					 PROP_ICON,
					 g_param_spec_object ("icon",
							      _("Icon"),
							      _("Icon to be shown for this item"),
							      GDK_TYPE_PIXBUF,
							      G_PARAM_READWRITE));

	/**
	 * GscItem:info:
	 *
	 * Optional extra information to be shown for this proposal.
	 */
	g_object_class_install_property (object_class,
					 PROP_INFO,
					 g_param_spec_string ("info",
							      _("Info"),
							      _("Info to be shown for this item"),
							      NULL,
							      G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof(GscItemPrivate));
}

static void
gsc_completion_item_init (GscItem *self)
{
	self->priv = GSC_COMPLETION_ITEM_GET_PRIVATE (self);
}

/** 
 * gsc_completion_item_new:
 * @label: The item label
 * @text: The item text
 * @icon: The item icon
 * @info: The item extra information
 *
 * Create a new #GscItem with label @label, icon @icon and 
 * extra information @info. Both @icon and @info can be %NULL in which case 
 * there will be no icon shown and no extra information available.
 *
 * Returns: The new #GscItem.
 *
 */
GscItem *
gsc_completion_item_new (const gchar *label,
				const gchar *text,
				GdkPixbuf   *icon,
				const gchar *info)
{
	return g_object_new (GSC_TYPE_ITEM, 
			     "label", label,
			     "text", text,
			     "icon", icon,
			     "info", info,
			     NULL);
}

/** 
 * gsc_completion_item_new_with_markup:
 * @markup: The item markup label
 * @text: The item text
 * @icon: The item icon
 * @info: The item extra information
 *
 * Create a new #GscItem with markup label @markup, icon 
 * @icon and extra information @info. Both @icon and @info can be %NULL in 
 * which case there will be no icon shown and no extra information available.
 *
 * Returns: The new #GscItem.
 *
 */
GscItem *
gsc_completion_item_new_with_markup (const gchar *markup,
                                            const gchar *text,
                                            GdkPixbuf   *icon,
                                            const gchar *info)
{
	return g_object_new (GSC_TYPE_ITEM, 
			     "markup", markup,
			     "text", text,
			     "icon", icon,
			     "info", info,
			     NULL);
}

/** 
 * gsc_completion_item_new_from_stock:
 * @label: The item label
 * @text: The item text
 * @stock: The stock icon
 * @info: The item extra information
 *
 * Creates a new #GscItem from a stock item. If @label is %NULL, 
 * the stock label will be used.
 *
 * Returns: the newly constructed #GscItem.
 *
 */
GscItem *
gsc_completion_item_new_from_stock (const gchar *label,
					   const gchar *text,
					   const gchar *stock,
					   const gchar *info)
{
	GscItem *item;
	GdkPixbuf *icon;
	GtkIconTheme *theme;
	gint width;
	gint height;
	GtkStockItem stock_item;
	
	if (stock != NULL)
	{
		theme = gtk_icon_theme_get_default ();
	
		gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &width, &height);

		icon = gtk_icon_theme_load_icon (theme, 
						 stock, 
						 width, 
						 GTK_ICON_LOOKUP_USE_BUILTIN, 
						 NULL);

		if (label == NULL && gtk_stock_lookup (stock, &stock_item))
		{
			label = stock_item.label;
		}
	}
	else
	{
		icon = NULL;
	}
	
	item = gsc_completion_item_new (label, text, icon, info);
	
	if (icon != NULL)
	{
		g_object_unref (icon);
	}
	
	return item;
}

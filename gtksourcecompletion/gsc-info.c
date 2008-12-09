/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-info.c
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
 
  
#include "gsc-info.h"
#include "gsc-utils.h"
#include "gsc-i18n.h"

struct _GscInfoPrivate
{
	GtkWidget *box;
	GtkWidget *info_scroll;
	GtkWidget *label;
	GtkWidget *bottom_bar;
	GtkWidget *info_button;
	
	GscInfoType type;
	gboolean adjust_height;
	gboolean adjust_width;
	gint max_height;
	gint max_width;
	
	GtkWidget *custom_widget;
};

/* Signals */
enum
{
	INFO_TYPE_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

/*Type definition*/
G_DEFINE_TYPE(GscInfo, gsc_info, GTK_TYPE_WINDOW);

#define GSC_INFO_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GSC_TYPE_INFO, GscInfoPrivate))
#define WINDOW_WIDTH 350
#define WINDOW_HEIGHT 200


static void
adjust_resize (GscInfo *self)
{
	/*TODO Control the "+30" for the scrollbars*/
	GtkWidget *current;
	GtkRequisition req;
	gint w, h;
	
	current = self->priv->custom_widget ? self->priv->custom_widget : self->priv->label;
	
	gtk_widget_size_request (current, &req);
	
	if (self->priv->adjust_height)
		h = req.height > self->priv->max_height 
			? self->priv->max_height 
			: req.height + 30;
	else
		h = WINDOW_HEIGHT;
	
	if (self->priv->adjust_width)
		w = req.width > self->priv->max_width 
			? self->priv->max_width
			: req.width + 30;
	else
		w = WINDOW_WIDTH;

	if (w < 1 || h < 1 ) 
		return;
		
	gtk_window_resize (GTK_WINDOW (self), w, h );
}

static void
show (GtkWidget *widget)
{
	GscInfo *self = GSC_INFO (widget);
	
	adjust_resize (self);
	/*
	TODO Set short by default or set the button depending on GscInfoType?
	*/
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->priv->info_button),
				      FALSE);
	gsc_info_set_info_type (self, GSC_INFO_TYPE_SHORT);
	
	GTK_WIDGET_CLASS (gsc_info_parent_class)->show (GTK_WIDGET (self));
}

static void
hide (GtkWidget *widget)
{
	GscInfo *self = GSC_INFO (widget);
	
	gtk_label_set_label (GTK_LABEL (self->priv->label), "");
	GTK_WIDGET_CLASS (gsc_info_parent_class)->hide (GTK_WIDGET (self));
}

static void
info_toggled_cb (GtkToggleButton *widget,
		 gpointer user_data)
{
	GscInfo *self = GSC_INFO (user_data);
	
	if (gtk_toggle_button_get_active (widget))
	{
		gsc_info_set_info_type (self,
					GSC_INFO_TYPE_EXTENDED);
	}
	else
	{
		gsc_info_set_info_type (self,
					GSC_INFO_TYPE_SHORT);
	}
}

static void
gsc_info_init (GscInfo *self)
{
	GtkWidget *info_icon;
	GtkWidget *info_button;
	
	self->priv = GSC_INFO_GET_PRIVATE (self);
	self->priv->adjust_height = FALSE;
	self->priv->adjust_width = FALSE;
	self->priv->max_height = WINDOW_HEIGHT;
	self->priv->max_width = WINDOW_WIDTH;
	self->priv->custom_widget = NULL;
	
	gtk_window_set_type_hint (GTK_WINDOW (self),
				  GDK_WINDOW_TYPE_HINT_NORMAL);
	gtk_window_set_decorated (GTK_WINDOW(self), FALSE);
	gtk_window_set_default_size (GTK_WINDOW (self),
				     WINDOW_WIDTH,WINDOW_HEIGHT);
	gtk_container_set_border_width (GTK_CONTAINER (self), 1);

	self->priv->info_scroll = gtk_scrolled_window_new (NULL, NULL);
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (self->priv->info_scroll),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);

	self->priv->label = gtk_label_new (NULL);
	    
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (self->priv->info_scroll),
					       self->priv->label);
	
	/*Bottom bar*/
	info_icon = gtk_image_new_from_stock (GTK_STOCK_INFO,
					      GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_set_tooltip_text (info_icon, _("Toggle info view"));
	
	info_button = gtk_toggle_button_new ();
	g_object_set (G_OBJECT (info_button), "can-focus", FALSE, NULL);
	
	self->priv->info_button = info_button;
	gtk_button_set_focus_on_click (GTK_BUTTON (info_button), FALSE);
	gtk_container_add (GTK_CONTAINER (info_button), info_icon);
	g_signal_connect (G_OBJECT (info_button),
			  "toggled",
			  G_CALLBACK (info_toggled_cb),
			  self);

	self->priv->bottom_bar = gtk_hbox_new (FALSE, 1);
	gtk_box_pack_start (GTK_BOX (self->priv->bottom_bar),
			    info_button,
			    FALSE,
			    FALSE,
			    0);
	
	self->priv->box = gtk_vbox_new (FALSE, 1);
	gtk_box_pack_start (GTK_BOX (self->priv->box),
			    self->priv->info_scroll,
			    TRUE,
			    TRUE,
			    0);

	gtk_box_pack_end (GTK_BOX (self->priv->box),
			  self->priv->bottom_bar,
			  FALSE,
			  FALSE,
			  0);
	
	gtk_container_add (GTK_CONTAINER (self), self->priv->box);
	
	/*
	 * FIXME: Add show instead of show_all
	 */
	gtk_widget_show_all (self->priv->box);
}

static void
gsc_info_finalize (GObject *object)
{
	/*GscInfo *self = GSC_INFO(object);*/
	
	G_OBJECT_CLASS (gsc_info_parent_class)->finalize (object);
}

static void
gsc_info_class_init (GscInfoClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);

	g_type_class_add_private (object_class, sizeof(GscInfoPrivate));

	object_class->finalize = gsc_info_finalize;
	widget_class->show = show;
	widget_class->hide = hide;
	
	/**
	 * GscInfo::info-type-changed:
	 * @popup: The #GscPopup who emits the signal
	 * @type: The new #GscInfoType
	 *
	 * When the info type change 
	 *
	 **/
	signals[INFO_TYPE_CHANGED] =
		g_signal_new ("info-type-changed",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			      0,
			      NULL, 
			      NULL,
			      g_cclosure_marshal_VOID__ENUM, 
			      G_TYPE_NONE,
			      1,
			      GTK_TYPE_INT);
}


GscInfo*
gsc_info_new (void)
{
	GscInfo *self = GSC_INFO(g_object_new (GSC_TYPE_INFO,
					       "type", GTK_WINDOW_POPUP,
					       NULL));
	return self;
}

void
gsc_info_move_to_cursor (GscInfo* self,
			 GtkTextView *view)
{
	int x,y;
	
	g_return_if_fail  (GSC_IS_INFO (self));

	adjust_resize (self);
	
	gsc_get_window_position_in_cursor (GTK_WINDOW (self),
					   view,
					   &x,
					   &y);
	gtk_window_move (GTK_WINDOW (self), x, y);
}

void
gsc_info_set_markup (GscInfo* self,
		     const gchar* markup)
{
	g_return_if_fail  (GSC_IS_INFO (self));

	gtk_label_set_markup (GTK_LABEL (self->priv->label), markup);
}

/**
 * gsc_info_set_adjust_height:
 * @self: The #GscInfo
 * @adjust: TRUE to adjust height to content, FALSE to fixed height
 * @max_height: if adjust = TRUE, set the max height. -1 to preserve the 
 * current value
 *
 * TRUE adjust height to the content. If the content is only a line, the info
 * will be small and if there are a lot of lines, the info will be large to the 
 * max_height
 *
 */
void
gsc_info_set_adjust_height (GscInfo* self,
			    gboolean adjust,
			    gint max_height)
{
	g_return_if_fail  (GSC_IS_INFO (self));

	/*TODO Control if max_height > screen height*/
	self->priv->adjust_height = adjust;
	if (max_height > 0)
		self->priv->max_height = max_height;
}

void
gsc_info_set_adjust_width (GscInfo* self,
			   gboolean adjust,
			   gint max_width)
{
	g_return_if_fail  (GSC_IS_INFO (self));

	/*TODO Control if max_width > screen width*/
	self->priv->adjust_width = adjust;
	if (max_width > 0)
		self->priv->max_width = max_width;
}

void
gsc_info_set_info_type (GscInfo* self,
		        GscInfoType type)
{
	g_return_if_fail  (GSC_IS_INFO (self));

	if (self->priv->type != type)
	{
		if (type == GSC_INFO_TYPE_SHORT)
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->priv->info_button),
						      FALSE);
		else
			gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->priv->info_button),
						      TRUE);
		
		g_signal_emit (G_OBJECT (self), signals[INFO_TYPE_CHANGED], 0, type);
		self->priv->type = type;
	}
}

void
gsc_info_set_bottom_bar_visible (GscInfo* self,
				 gboolean visible)
{
	g_return_if_fail  (GSC_IS_INFO (self));

	if (visible)
		gtk_widget_show (self->priv->bottom_bar);
	else
		gtk_widget_hide (self->priv->bottom_bar);
}

void
gsc_info_set_custom (GscInfo* self,
		     GtkWidget *custom_widget)
{
	g_return_if_fail (GSC_IS_INFO (self));
	
	if (self->priv->custom_widget == custom_widget)
		return;
		
	if (custom_widget)
	{
		g_return_if_fail (GTK_IS_WIDGET (custom_widget));
		
		if (self->priv->custom_widget)
		{
			gtk_container_remove (GTK_CONTAINER (self->priv->box),
					      self->priv->custom_widget);
			g_object_unref (self->priv->custom_widget);
			self->priv->custom_widget = NULL;
		}
		else
		{
			gtk_widget_hide (self->priv->info_scroll);
		}
		
		self->priv->custom_widget = g_object_ref (custom_widget);
		gtk_container_add (GTK_CONTAINER (self->priv->box),
				   custom_widget);
		gtk_widget_show (self->priv->custom_widget);
	}
	else
	{

		if (self->priv->custom_widget)
		{
			gtk_container_remove (GTK_CONTAINER (self->priv->box),
					      self->priv->custom_widget);
			g_object_unref (self->priv->custom_widget);
			self->priv->custom_widget = NULL;
			gtk_widget_show (self->priv->info_scroll);
		}
		
	}
}

GtkWidget*
gsc_info_get_custom (GscInfo* self)
{
	g_return_val_if_fail (GSC_IS_INFO (self), NULL);

	return self->priv->custom_widget;
}

GscInfoType
gsc_info_get_info_type (GscInfo* self)
{
	g_return_val_if_fail (GSC_IS_INFO (self), 0);

	return self->priv->type;
}



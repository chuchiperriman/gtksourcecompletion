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

struct _GscInfoPrivate
{
	GtkWidget *box;
	GtkWidget *info_scroll;
	GtkWidget *label;
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

static GtkWindowClass* parent_class = NULL;
static guint signals[LAST_SIGNAL] = { 0 };

/*Type definition*/
G_DEFINE_TYPE(GscInfo, gsc_info, GTK_TYPE_WINDOW);

#define GSC_INFO_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GSC_TYPE_INFO, GscInfoPrivate))
#define WINDOW_WIDTH 350
#define WINDOW_HEIGHT 200

static void
_show(GtkWidget *widget)
{

	/*TODO Control the "+30" for the scrollbars*/
	GscInfo *self = GSC_INFO(widget);
	GtkWidget *current = self->priv->custom_widget ? self->priv->custom_widget : self->priv->label;
	GtkRequisition req;
	gint w, h;
	gtk_widget_size_request(current, &req);
	
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
		
	gtk_window_resize(GTK_WINDOW(self),w , h );
	GTK_WIDGET_CLASS (parent_class)->show (GTK_WIDGET(self));
}

static void
_hide(GtkWidget *widget)
{
	GscInfo *self = GSC_INFO(widget);
	gtk_label_set_label(GTK_LABEL(self->priv->label),"");
	GTK_WIDGET_CLASS (parent_class)->hide (GTK_WIDGET(self));
}

static void
gsc_info_init (GscInfo *self)
{
	self->priv = GSC_INFO_GET_PRIVATE(self);
	self->priv->adjust_height = FALSE;
	self->priv->adjust_width = FALSE;
	self->priv->max_height = WINDOW_HEIGHT;
	self->priv->max_width = WINDOW_WIDTH;
	self->priv->custom_widget = NULL;
	
	gtk_window_set_type_hint (GTK_WINDOW(self), GDK_WINDOW_TYPE_HINT_NORMAL);
	gtk_window_set_decorated(GTK_WINDOW(self),FALSE);
	gtk_window_set_default_size(GTK_WINDOW(self),WINDOW_WIDTH,WINDOW_HEIGHT);
	gtk_container_set_border_width(GTK_CONTAINER(self),1);

	self->priv->info_scroll = gtk_scrolled_window_new(NULL,NULL);
	g_object_set (self->priv->info_scroll,
	    "has-default", FALSE,
	    "can-default", FALSE,
	    "has-focus", FALSE,
	    "is-focus", FALSE, 
	    NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(self->priv->info_scroll),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	self->priv->label = gtk_label_new(NULL);
	g_object_set (self->priv->label,
	    "has-default", FALSE,
	    "can-default", FALSE,
	    "has-focus", FALSE,
	    "is-focus", FALSE, 
	    NULL);
	    
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(self->priv->info_scroll),
					      self->priv->label);
	
	self->priv->box = gtk_vbox_new(FALSE,0);
	gtk_container_add(GTK_CONTAINER(self->priv->box),self->priv->info_scroll);
	gtk_container_add(GTK_CONTAINER(self),self->priv->box);
	
	gtk_widget_show_all(self->priv->box);
}

static void
gsc_info_finalize (GObject *object)
{
	/*GscInfo *self = GSC_INFO(object);*/
	
	G_OBJECT_CLASS (parent_class)->finalize (object);
}

static void
gsc_info_class_init (GscInfoClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	parent_class = GTK_WINDOW_CLASS (g_type_class_peek_parent (klass));

	object_class->finalize = gsc_info_finalize;
	widget_class->show = _show;
	widget_class->hide = _hide;
	g_type_class_add_private (object_class, sizeof(GscInfoPrivate));
	
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
gsc_info_new(void)
{
	GscInfo *self = GSC_INFO(g_object_new (GSC_TYPE_INFO,
					"type", GTK_WINDOW_POPUP,
					NULL));
	return self;
}

void
gsc_info_move_to_cursor(GscInfo* self, GtkTextView *view)
{
	int x,y;
	gsc_get_window_position_in_cursor(GTK_WINDOW(self),
					  view,
					  &x,
					  &y);
	gtk_window_move(GTK_WINDOW(self), x, y);
}

void
gsc_info_set_markup(GscInfo* self,
		    const gchar* markup)
{
	gtk_label_set_markup(GTK_LABEL(self->priv->label),markup);
}

void
gsc_info_set_adjust_height(GscInfo* self,
			   gboolean adjust,
			   gint max_height)
{
	/*TODO Control if max_height > screen height*/
	self->priv->adjust_height = adjust;
	if (max_height > 0)
		self->priv->max_height = max_height;
}

void
gsc_info_set_adjust_width(GscInfo* self,
			  gboolean adjust,
			  gint max_width)
{
	/*TODO Control if max_width > screen width*/
	self->priv->adjust_width = adjust;
	if (max_width > 0)
		self->priv->max_width = max_width;
}

void
gsc_info_set_info_type(GscInfo* self,
		       GscInfoType type)
{
	if (self->priv->type != type)
	{
		self->priv->type = type;
		g_signal_emit (G_OBJECT (self), signals[INFO_TYPE_CHANGED], 0, type);
	}
}

void
gsc_info_set_custom(GscInfo* self,
		    GtkWidget *custom_widget)
{

	g_return_if_fail(GSC_IS_INFO(self));
	if (self->priv->custom_widget == custom_widget)
		return;
		
	if (custom_widget)
	{
		g_return_if_fail(GTK_IS_WIDGET(custom_widget));
		if (self->priv->custom_widget)
		{
			gtk_container_remove (GTK_CONTAINER (self->priv->box), self->priv->custom_widget);
			g_object_unref(self->priv->custom_widget);
			self->priv->custom_widget = NULL;
		}
		else
		{
			//gtk_container_remove (GTK_CONTAINER (self->priv->box), self->priv->info_scroll);
			gtk_widget_hide(self->priv->info_scroll);
		}
		
		self->priv->custom_widget = g_object_ref (custom_widget);
		gtk_container_add (GTK_CONTAINER (self->priv->box), custom_widget);
		gtk_widget_show(self->priv->custom_widget);
	}
	else
	{

		if (self->priv->custom_widget)
		{
			gtk_container_remove (GTK_CONTAINER (self->priv->box), self->priv->custom_widget);
			g_object_unref(self->priv->custom_widget);
			self->priv->custom_widget = NULL;
			//gtk_container_add (GTK_CONTAINER (self->priv->box), self->priv->info_scroll);
			gtk_widget_show(self->priv->info_scroll);
		}
		
	}
}





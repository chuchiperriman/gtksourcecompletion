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
	GtkWidget *label;
	GscInfoType type;
	gboolean adjust_height;
	gboolean adjust_width;
	gint max_height;
	gint max_width;
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

static gboolean
_focus_out_event_cb(GtkWidget *widget,
			GdkEventFocus *event,
			gpointer user_data)
{
	gtk_widget_hide(widget);
	return FALSE;
}

static void
_show(GtkWidget *widget)
{

	/*TODO Control the "+30" for the scrollbars*/
	GscInfo *self = GSC_INFO(widget);
	const gchar* text = gtk_label_get_text(GTK_LABEL(self->priv->label));
	if (text == NULL || g_strcmp0(text,"") == 0)
		return;
	
	GtkRequisition req;
	gint w, h;
	gtk_widget_size_request(self->priv->label, &req);
	
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

	gtk_window_set_default_size(GTK_WINDOW(self),WINDOW_WIDTH,WINDOW_HEIGHT);
	gtk_window_set_decorated(GTK_WINDOW(self),FALSE);
	GtkWidget* info_scroll = gtk_scrolled_window_new(NULL,NULL);
	g_object_set (info_scroll, 
	    "has-default", FALSE,
	    "can-default", FALSE,
	    "has-focus", FALSE,
	    "is-focus", FALSE, 
	    NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(info_scroll),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	self->priv->label = gtk_label_new(NULL);
	g_object_set (self->priv->label, 
	    "has-default", FALSE,
	    "can-default", FALSE,
	    "has-focus", FALSE,
	    "is-focus", FALSE, 
	    NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(info_scroll),
					      self->priv->label);
	gtk_container_set_border_width(GTK_CONTAINER(self),1);
	gtk_container_add(GTK_CONTAINER(self),info_scroll);

	gtk_window_set_type_hint (GTK_WINDOW(self), GDK_WINDOW_TYPE_HINT_NORMAL);

	gtk_window_set_skip_pager_hint (GTK_WINDOW(self), TRUE);
	gtk_window_set_skip_taskbar_hint (GTK_WINDOW(self), TRUE);	
	gtk_window_set_focus_on_map(GTK_WINDOW(self), FALSE);
	gtk_window_set_accept_focus(GTK_WINDOW(self), FALSE);
	g_object_set (self,
	    "has-default", FALSE,
	    "can-default", FALSE,
	    "has-focus", FALSE,
	    "is-focus", FALSE, 
	    NULL);
	gtk_widget_show_all(self);
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
	/*Window type (TOPLEVEL,POPUP...)*/
	
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



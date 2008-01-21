/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsv-completion-popup.c
 *
 *  Copyright (C) 2007 - Chuchiperriman <chuchiperriman@gmail.com>
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
 
#include "gsv-completion-popup.h"
#include "gsv-completion-tree.h"
#include "gtksourcecompletion-i18n.h"
#include "gtksourcecompletion-utils.h"
#include "gtksourcecompletion-item.h"

#define COL_PIXBUF 0
#define COL_NAME 1
#define COL_DATA 2

#define WINDOW_WIDTH 350
#define WINDOW_HEIGHT 200

#define GSV_COMPLETION_POPUP_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object),\
					 GSV_TYPE_COMPLETION_POPUP,                    \
					 GsvCompletionPopupPriv))
					 

/* Signals */
enum
{
	ITEM_SELECTED,
	LAST_SIGNAL
};

static guint popup_signals[LAST_SIGNAL] = { 0 };

struct _GsvCompletionPopupPriv
{
	GtkWidget *info_window;
	GsvCompletionTree *completion_tree;
	GtkWidget *info_button;
	GtkWidget *info_label;
	GtkWidget *view;
	GtkWidget *notebook;
	gboolean destroy_has_run;
};

G_DEFINE_TYPE(GsvCompletionPopup, gsv_completion_popup, GTK_TYPE_WINDOW);

/*
 * Return TRUE if the position is over the text and FALSE if 
 * the position is under the text.
 */
static gboolean 
_get_popup_position(GsvCompletionPopup *self, gint *x, gint *y)
{
	gint w,h,xtext,ytext;
	gint sw = gdk_screen_width();
	gint sh = gdk_screen_height();

	gtk_source_view_get_cursor_pos(GTK_TEXT_VIEW(self->priv->view),x,y);
	gtk_window_get_size(GTK_WINDOW(self), &w, &h);
	if (*x+w > sw) *x = sw - w -4;
	/*If we cannot show it down, we show it up.*/
	if (*y+h > sh)
	{
		PangoLayout* layout = gtk_widget_create_pango_layout(GTK_WIDGET(self->priv->view), NULL);
		pango_layout_get_pixel_size(layout,&xtext,&ytext);
		*y = *y -ytext - h;
		g_object_unref(layout);
		return TRUE;
	}

	return FALSE;
}

static void
_set_current_completion_info(GsvCompletionPopup *self)
{
	gchar* markup = NULL;
	GtkSourceCompletionItem *item;
	if (gsv_completion_popup_get_selected_item(self,&item))
	{
		markup = gtk_source_completion_provider_get_item_info_markup(
						gtk_source_completion_item_get_provider(item),item);
		if (markup != NULL)
		{
			gtk_label_set_markup(GTK_LABEL(self->priv->info_label),markup);
			g_free(markup);
		}
		else
		{
			gtk_label_set_markup(GTK_LABEL(self->priv->info_label), _("There is no info for the current item"));
		}
	}
	
	
}

static void
_show_completion_info(GsvCompletionPopup *self)
{
	_set_current_completion_info(self);
	gint y, x, sw, sh;
	_get_popup_position(self,&x,&y);
	sw = gdk_screen_width();
	sh = gdk_screen_height();
	x += WINDOW_WIDTH;

	if (x + WINDOW_WIDTH >= sw)
	{
		x -= (WINDOW_WIDTH * 2);
	}
	gtk_window_move(GTK_WINDOW(self->priv->info_window), x, y);
	gtk_widget_show(self->priv->info_window);
}

static void
_info_toggled_cb(GtkToggleButton *widget,
				gpointer user_data)
{
	GsvCompletionPopup *self = GSV_COMPLETION_POPUP(user_data);
	if (gtk_toggle_button_get_active(widget))
	{
		_show_completion_info(self);
	}
	else
	{
		gtk_widget_hide(self->priv->info_window);
	}
}

static void
_item_selected_cb (GsvCompletionTree *tree, GtkSourceCompletionItem *item,
										gpointer user_data)
{
	g_signal_emit (G_OBJECT (user_data), popup_signals[ITEM_SELECTED], 0, item);
}

static void 
_selection_changed_cd(GsvCompletionTree *tree, GtkSourceCompletionItem *item,
					gpointer user_data)
{
	GsvCompletionPopup *self = GSV_COMPLETION_POPUP(user_data);

	if (GTK_WIDGET_VISIBLE(self->priv->info_window))
	{
		_show_completion_info(self);
	}
}

static void
gsv_completion_popup_show(GtkWidget *widget)
{
	GsvCompletionPopup *self = GSV_COMPLETION_POPUP(widget);
	gint x, y;
	_get_popup_position(self,&x,&y);
	gtk_window_move(GTK_WINDOW(self), x, y);
	
	if (!GTK_WIDGET_VISIBLE(self))
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->priv->info_button),FALSE);
		GTK_WIDGET_CLASS (gsv_completion_popup_parent_class)->show (widget);
	}
	gsv_completion_tree_select_first(self->priv->completion_tree);
}

static void
gsv_completion_popup_hide(GtkWidget *widget)
{
	GsvCompletionPopup *self = GSV_COMPLETION_POPUP(widget);
	
	GTK_WIDGET_CLASS (gsv_completion_popup_parent_class)->hide (widget);
	gtk_widget_hide(self->priv->info_window);

}

static void
gsv_completion_popup_realize (GtkWidget *widget)
{
	GsvCompletionPopup *self = GSV_COMPLETION_POPUP(widget);
	gtk_container_set_border_width(GTK_CONTAINER(self),1);
	gtk_widget_set_size_request(GTK_WIDGET(self),WINDOW_WIDTH,WINDOW_HEIGHT);
	gtk_window_set_resizable(GTK_WINDOW(self),TRUE);
	
	GTK_WIDGET_CLASS (gsv_completion_popup_parent_class)->realize (widget);
}

static void
gsv_completion_popup_finalize (GObject *object)
{
	g_debug("Finish GsvCompletionPopup");
	
	G_OBJECT_CLASS (gsv_completion_popup_parent_class)->finalize (object);
	
}

static void
gsv_completion_popup_destroy (GtkObject *object)
{
	g_debug("Destroy GsvCompletionPopup");

	GsvCompletionPopup *self = GSV_COMPLETION_POPUP(object);
	
	if (!self->priv->destroy_has_run)
	{
		gsv_completion_popup_clear(self);
		self->priv->destroy_has_run = TRUE;
	}
	
	GTK_OBJECT_CLASS (gsv_completion_popup_parent_class)->destroy (object);
	
}

static void
gsv_completion_popup_class_init (GsvCompletionPopupClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	g_type_class_add_private (klass, sizeof(GsvCompletionPopupPriv));
	
	object_class->finalize = gsv_completion_popup_finalize;
	gtkobject_class->destroy = gsv_completion_popup_destroy;
	widget_class->show = gsv_completion_popup_show;
	widget_class->hide = gsv_completion_popup_hide;
	widget_class->realize = gsv_completion_popup_realize;
	
	popup_signals[ITEM_SELECTED] =
    g_signal_new ("item-selected",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (GsvCompletionPopupClass, item_selected),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER, 
                  G_TYPE_NONE,
                  1,
                  GTK_TYPE_POINTER);
	
}

static void
gsv_completion_popup_init (GsvCompletionPopup *self)
{
	g_debug("Init GsvCompletionPopup");
	self->priv = GSV_COMPLETION_POPUP_GET_PRIVATE(self);
	self->priv->destroy_has_run = FALSE;


	self->priv->completion_tree = GSV_COMPLETION_TREE(gsv_completion_tree_new());
	/*HBox. Up the scroll and the tree and down the icon list*/
	self->priv->notebook = gtk_notebook_new();

	/* TODO Remove all labels when done*/
	GtkWidget *default_label = gtk_label_new("Default");
	gtk_notebook_append_page(GTK_NOTEBOOK(self->priv->notebook),
			GTK_WIDGET(self->priv->completion_tree),default_label);
	/*Icon list*/
	GtkWidget *info_icon = gtk_image_new_from_stock(GTK_STOCK_INFO,GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_set_tooltip_text(info_icon, _("Show Item Info"));
	GtkWidget *info_button = gtk_toggle_button_new();
	self->priv->info_button = info_button;
	gtk_container_add(GTK_CONTAINER(info_button),info_icon);
	g_signal_connect (G_OBJECT (info_button),
					"toggled",
					G_CALLBACK (_info_toggled_cb),
					self);

	GtkWidget *hbox = gtk_hbox_new(FALSE,1);
	gtk_box_pack_start(GTK_BOX(hbox),
					info_button,
					FALSE,
					FALSE,
					0);
	/*Packing all*/
	GtkWidget *vbox = gtk_vbox_new(FALSE,1);
	gtk_box_pack_start(GTK_BOX(vbox),
					self->priv->notebook,
					TRUE,
					TRUE,
					0);

	gtk_box_pack_end(GTK_BOX(vbox),
					hbox,
					FALSE,
					FALSE,
					0);

	gtk_container_add(GTK_CONTAINER(self),vbox);
	gtk_widget_show_all(vbox);

	/*Info window*/
	self->priv->info_window = gtk_window_new(GTK_WINDOW_POPUP);
	gtk_widget_set_size_request(self->priv->info_window,WINDOW_WIDTH,WINDOW_HEIGHT);
	GtkWidget* info_scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(info_scroll),
						GTK_POLICY_AUTOMATIC,
						GTK_POLICY_AUTOMATIC);
	self->priv->info_label = gtk_label_new(NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(info_scroll),self->priv->info_label);
	gtk_container_set_border_width(GTK_CONTAINER(self->priv->info_window),1);
	gtk_container_add(GTK_CONTAINER(self->priv->info_window),info_scroll);
	
	gtk_widget_show(info_scroll);
	gtk_widget_show(self->priv->info_label);
	
	/* Connect signals */
	
	g_signal_connect(self->priv->completion_tree, 
						"item-selected",
						G_CALLBACK(_item_selected_cb),
						(gpointer) self);
						
	g_signal_connect(self->priv->completion_tree, 
						"selection-changed",
						G_CALLBACK(_selection_changed_cd),
						(gpointer) self);
}

GtkWidget*
gsv_completion_popup_new (GtkTextView *view)
{
  GsvCompletionPopup *self = GSV_COMPLETION_POPUP ( g_object_new (gsv_completion_popup_get_type() , NULL));
  self->priv->view = GTK_WIDGET(view);
  GTK_WINDOW(self)->type = GTK_WINDOW_POPUP;
  return GTK_WIDGET(self);
}

void
gsv_completion_popup_add_data(GsvCompletionPopup *self,
					GtkSourceCompletionItem* data)
{
	gsv_completion_tree_add_data(self->priv->completion_tree,data);
}

void
gsv_completion_popup_clear(GsvCompletionPopup *self)
{
	gsv_completion_tree_clear(self->priv->completion_tree);
}

gboolean
gsv_completion_popup_select_first(GsvCompletionPopup *self)
{
	return gsv_completion_tree_select_first(self->priv->completion_tree);
}

gboolean 
gsv_completion_popup_select_last(GsvCompletionPopup *self)
{
	return gsv_completion_tree_select_last(self->priv->completion_tree);
}

gboolean
gsv_completion_popup_select_previous(GsvCompletionPopup *self, 
					gint rows)
{
	return gsv_completion_tree_select_previous(self->priv->completion_tree,rows);
}

gboolean
gsv_completion_popup_select_next(GsvCompletionPopup *self, 
					gint rows)
{
	return gsv_completion_tree_select_next(self->priv->completion_tree,rows);
}

/*Not free the item*/
gboolean
gsv_completion_popup_get_selected_item(GsvCompletionPopup *self,
													GtkSourceCompletionItem **item)
{
	return gsv_completion_tree_get_selected_item(self->priv->completion_tree,item);
}

gboolean
gsv_completion_popup_has_items(GsvCompletionPopup *self)
{
	return gsv_completion_tree_has_items(self->priv->completion_tree);
}

void
gsv_completion_popup_toggle_item_info(GsvCompletionPopup *self)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->priv->info_button),
									!gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self->priv->info_button)));
}

void
gsv_completion_popup_refresh(GsvCompletionPopup *self)
{
	gsv_completion_popup_show(GTK_WIDGET(self));
}



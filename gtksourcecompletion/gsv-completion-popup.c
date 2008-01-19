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
	GtkWidget *data_tree_view;
	GtkWidget *info_button;
	GtkWidget *info_label;
	GtkWidget *view;
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
_popup_tree_row_activated_cb (GtkTreeView *tree_view,
										GtkTreePath *path,
										GtkTreeViewColumn *column,
										gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkSourceCompletionItem *data;
	GsvCompletionPopup *self;
	GValue value_name = {0,};
	
	self = GSV_COMPLETION_POPUP(user_data);
	
	model = gtk_tree_view_get_model(tree_view);
	
	gtk_tree_model_get_iter(model,&iter,path);
	gtk_tree_model_get_value(model,&iter,COL_DATA,&value_name);
	data = (GtkSourceCompletionItem*)g_value_get_pointer(&value_name);
	
	g_signal_emit (G_OBJECT (self), popup_signals[ITEM_SELECTED], 0, data);
}

static void 
_selection_changed_cd(GtkTreeSelection *treeselection,
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
	gtk_tree_view_scroll_to_point(GTK_TREE_VIEW(self->priv->data_tree_view),0,0);
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
		
	/*HBox. Up the scroll and the tree and down the icon list*/
	/* Scroll and tree */
	GtkWidget* scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
						GTK_POLICY_AUTOMATIC,
						GTK_POLICY_AUTOMATIC);
	self->priv->data_tree_view = gtk_tree_view_new();
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(self->priv->data_tree_view),FALSE);
	
	gtk_container_add(GTK_CONTAINER(scroll),self->priv->data_tree_view);
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
					scroll,
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
	
	/* Create the Tree */
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkCellRenderer* renderer_pixbuf;
	GtkTreeViewColumn* column_pixbuf;
	
	renderer_pixbuf = gtk_cell_renderer_pixbuf_new();
   column_pixbuf = gtk_tree_view_column_new_with_attributes ("Pixbuf",
   		renderer_pixbuf, "pixbuf", COL_PIXBUF, NULL);
	
	column = gtk_tree_view_column_new();
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (column, renderer, FALSE);
	gtk_tree_view_column_set_attributes (column,renderer,"text",COL_NAME,NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW(self->priv->data_tree_view), column_pixbuf);
	gtk_tree_view_append_column (GTK_TREE_VIEW(self->priv->data_tree_view), column);
	
	/* Create the model */
	GtkListStore *list_store = gtk_list_store_new (4,GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_POINTER);
		
	gtk_tree_view_set_model(GTK_TREE_VIEW(self->priv->data_tree_view),GTK_TREE_MODEL(list_store));
	
	/* Connect signals */
	
	g_signal_connect(self->priv->data_tree_view, 
						"row-activated",
						G_CALLBACK(_popup_tree_row_activated_cb),
						(gpointer) self);
						
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->data_tree_view));
	g_signal_connect(selection, 
						"changed",
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

	g_assert(data != NULL);
	
	GtkTreeIter iter;
	GtkListStore *store;
	
	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->data_tree_view)));
	
	gtk_list_store_append (store,&iter);
			
	gtk_list_store_set (store, 
						&iter,
						COL_PIXBUF, gtk_source_completion_item_get_icon(data),
						COL_NAME, gtk_source_completion_item_get_name(data),
						COL_DATA, data,
						-1);
}

void
gsv_completion_popup_clear(GsvCompletionPopup *self)
{
	GtkListStore *store = store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->data_tree_view)));
	GtkTreeModel *model = GTK_TREE_MODEL(store);
	GtkTreeIter iter;
	GtkSourceCompletionItem *data;
	
	if (gtk_tree_model_get_iter_first(model,&iter))
	{
		do
		{
			GValue value_data = {0,};
			gtk_tree_model_get_value(model,&iter,COL_DATA,&value_data);
			data = (GtkSourceCompletionItem*)g_value_get_pointer(&value_data);
			gtk_source_completion_item_free(data);
		}while(gtk_tree_model_iter_next(model,&iter));
	}
	
	gtk_list_store_clear(store);
}

gboolean
gsv_completion_popup_select_first(GsvCompletionPopup *self)
{
	GtkTreeIter iter;
	GtkTreePath* path;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	if (!GTK_WIDGET_VISIBLE(self))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->data_tree_view));

	if (gtk_tree_selection_get_mode(selection) == GTK_SELECTION_NONE)
		return FALSE;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->data_tree_view));
		
	gtk_tree_model_get_iter_first(model, &iter);
	gtk_tree_selection_select_iter(selection, &iter);
	path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(self->priv->data_tree_view), path, NULL, FALSE, 0, 0);
	gtk_tree_path_free(path);
	return TRUE;
}

gboolean 
gsv_completion_popup_select_last(GsvCompletionPopup *self)
{
	GtkTreeIter iter;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	GtkTreePath* path;
	gint children;
	
	if (!GTK_WIDGET_VISIBLE(self))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->data_tree_view));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->data_tree_view));
	
	if (gtk_tree_selection_get_mode(selection) == GTK_SELECTION_NONE)
		return FALSE;
	
	children = gtk_tree_model_iter_n_children(model, NULL);
	if (children > 0)
	{
		gtk_tree_model_iter_nth_child(model, &iter, NULL, children - 1);
	
		gtk_tree_selection_select_iter(selection, &iter);
		path = gtk_tree_model_get_path(model, &iter);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(self->priv->data_tree_view), path, NULL, FALSE, 0, 0);
		gtk_tree_path_free(path);
		return TRUE;
	}
	return FALSE;
}

gboolean
gsv_completion_popup_select_previous(GsvCompletionPopup *self, 
					gint rows)
{
	GtkTreeIter iter;
	GtkTreePath* path;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	
	if (!GTK_WIDGET_VISIBLE(self))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->data_tree_view));
	
	if (gtk_tree_selection_get_mode(selection) == GTK_SELECTION_NONE)
		return FALSE;
	
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gint i;
		path = gtk_tree_model_get_path(model, &iter);
		for (i=0; i  < rows; i++)
			gtk_tree_path_prev(path);
		
		if (gtk_tree_model_get_iter(model, &iter, path))
		{
			gtk_tree_selection_select_iter(selection, &iter);
			gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(self->priv->data_tree_view), path, NULL, FALSE, 0, 0);
		}
		gtk_tree_path_free(path);
	}
	else
	{
		return gsv_completion_popup_select_first(self);
	}
	
	return TRUE;
}

gboolean
gsv_completion_popup_select_next(GsvCompletionPopup *self, 
					gint rows)
{
	GtkTreeIter iter;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	
	if (!GTK_WIDGET_VISIBLE(self))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->data_tree_view));
	if (gtk_tree_selection_get_mode(selection) == GTK_SELECTION_NONE)
		return FALSE;
	
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gint i;
		GtkTreePath* path;
		for (i = 0; i < rows; i++)
		{
			if (!gtk_tree_model_iter_next(model, &iter))
				return gsv_completion_popup_select_last(self);
		}
		gtk_tree_selection_select_iter(selection, &iter);
		path = gtk_tree_model_get_path(model, &iter);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(self->priv->data_tree_view), path, NULL, FALSE, 0, 0);
		gtk_tree_path_free(path);
	}
	else
	{
		return gsv_completion_popup_select_first(self);
	}
	return TRUE;
}

/*Not free the item*/
gboolean
gsv_completion_popup_get_selected_item(GsvCompletionPopup *self,
													GtkSourceCompletionItem **item)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GValue value_item = {0,};
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->data_tree_view));
	if (gtk_tree_selection_get_selected(selection,NULL, &iter))
	{
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->data_tree_view));
		gtk_tree_model_get_value(model,&iter,COL_DATA,&value_item);
		*item = (GtkSourceCompletionItem*)g_value_get_pointer(&value_item);
		
		return TRUE;
	}
	
	return FALSE;
	
}

gboolean
gsv_completion_popup_has_items(GsvCompletionPopup *self)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->data_tree_view));
	return gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model),&iter);
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



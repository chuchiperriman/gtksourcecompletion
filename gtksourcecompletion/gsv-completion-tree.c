/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsv-completion-tree.c
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

#include "gsv-completion-tree.h"

#define COL_PIXBUF 0
#define COL_NAME 1
#define COL_DATA 2

#define GSV_COMPLETION_TREE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object),\
					 GSV_TYPE_COMPLETION_TREE,                    \
					 GsvCompletionTreePriv))

/* Signals */
enum
{
	ITEM_SELECTED,
	SELECTION_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

struct _GsvCompletionTreePriv
{
	GtkWidget *tree_view;

	gboolean destroy_has_run;
};

static void
_tree_row_activated_cb (GtkTreeView *tree_view,
										GtkTreePath *path,
										GtkTreeViewColumn *column,
										gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkSourceCompletionItem *data;
	GsvCompletionTree *self;
	GValue value_name = {0,};
	
	self = GSV_COMPLETION_TREE(user_data);
	
	model = gtk_tree_view_get_model(tree_view);
	
	gtk_tree_model_get_iter(model,&iter,path);
	gtk_tree_model_get_value(model,&iter,COL_DATA,&value_name);
	data = (GtkSourceCompletionItem*)g_value_get_pointer(&value_name);
	
	g_signal_emit (G_OBJECT (self), signals[ITEM_SELECTED], 0, data);
}

static void 
_selection_changed_cd(GtkTreeSelection *treeselection,
					gpointer user_data)
{
	GtkSourceCompletionItem *item;
	GsvCompletionTree *self = GSV_COMPLETION_TREE(user_data);
	if (gsv_completion_tree_get_selected_item(self,&item))
	{
		g_signal_emit (G_OBJECT (self), signals[SELECTION_CHANGED], 0, item);
	}
}


G_DEFINE_TYPE(GsvCompletionTree, gsv_completion_tree, GTK_TYPE_SCROLLED_WINDOW);

static void
gsv_completion_tree_finalize (GObject *object)
{
	g_debug("Finish GsvCompletionTree");
	
	G_OBJECT_CLASS (gsv_completion_tree_parent_class)->finalize (object);
	
}

static void
gsv_completion_tree_class_init (GsvCompletionTreeClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	g_type_class_add_private (klass, sizeof(GsvCompletionTreePriv));
	
	object_class->finalize = gsv_completion_tree_finalize;
	
	signals[ITEM_SELECTED] =
    g_signal_new ("item-selected",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (GsvCompletionTreeClass, item_selected),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER, 
                  G_TYPE_NONE,
                  1,
                  GTK_TYPE_POINTER);
                  
	signals[SELECTION_CHANGED] =
    g_signal_new ("selection-changed",
                  G_TYPE_FROM_CLASS (klass),
                  G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
                  G_STRUCT_OFFSET (GsvCompletionTreeClass, item_selected),
                  NULL, NULL,
                  g_cclosure_marshal_VOID__POINTER, 
                  G_TYPE_NONE,
                  1,
                  GTK_TYPE_POINTER);
}

static void
gsv_completion_tree_init (GsvCompletionTree *self)
{
	g_debug("Init GsvCompletionTree");
	self->priv = GSV_COMPLETION_TREE_GET_PRIVATE(self);
	self->priv->destroy_has_run = FALSE;
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(self),
						GTK_POLICY_AUTOMATIC,
						GTK_POLICY_AUTOMATIC);
	
	gtk_scrolled_window_set_hadjustment(GTK_SCROLLED_WINDOW(self),NULL);
	gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(self),NULL);

	self->priv->tree_view = gtk_tree_view_new();
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(self->priv->tree_view),FALSE);
	
	gtk_container_add(GTK_CONTAINER(self),self->priv->tree_view);

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

	gtk_tree_view_append_column (GTK_TREE_VIEW(self->priv->tree_view), column_pixbuf);
	gtk_tree_view_append_column (GTK_TREE_VIEW(self->priv->tree_view), column);
	
	/* Create the model */
	GtkListStore *list_store = gtk_list_store_new (4,GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_POINTER);
		
	gtk_tree_view_set_model(GTK_TREE_VIEW(self->priv->tree_view),GTK_TREE_MODEL(list_store));
	
	/* Connect signals */
	
	g_signal_connect(self->priv->tree_view, 
						"row-activated",
						G_CALLBACK(_tree_row_activated_cb),
						(gpointer) self);
					
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->tree_view));
	g_signal_connect(selection, 
						"changed",
						G_CALLBACK(_selection_changed_cd),
						(gpointer) self);
}


/**
 * gsv_completion_tree_get_selected_item:
 * @self: The #GsvCompletionTree
 * @item: A reference of an item. This function sets the pointer to the selected item.
 *
 * @Returns TRUE if there is an item selected
 *
 */
gboolean
gsv_completion_tree_get_selected_item(GsvCompletionTree *self,
													GtkSourceCompletionItem **item)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GValue value_item = {0,};
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->tree_view));
	if (gtk_tree_selection_get_selected(selection,NULL, &iter))
	{
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->tree_view));
		gtk_tree_model_get_value(model,&iter,COL_DATA,&value_item);
		*item = (GtkSourceCompletionItem*)g_value_get_pointer(&value_item);
		
		return TRUE;
	}
	
	return FALSE;
	
}

/**
 * gsv_completion_tree_add_data:
 * @self: The #GsvCompletionTree
 * @data: the item to add to the tree
 *
 * Adds a new item into the tree
 *
 */
void
gsv_completion_tree_add_data(GsvCompletionTree *self,
					GtkSourceCompletionItem* data)
{

	g_assert(data != NULL);
	
	GtkTreeIter iter;
	GtkListStore *store;
	
	store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->tree_view)));
	
	gtk_list_store_append (store,&iter);
			
	gtk_list_store_set (store, 
						&iter,
						COL_PIXBUF, gtk_source_completion_item_get_icon(data),
						COL_NAME, gtk_source_completion_item_get_name(data),
						COL_DATA, data,
						-1);
}

/**
 * gsv_completion_tree_clear:
 * @self: the #GsvCompletionTree
 *
 * Clear the tree model and free the items 
 */
void
gsv_completion_tree_clear(GsvCompletionTree *self)
{
	GtkListStore *store = store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->tree_view)));
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

/**
 * gsv_completion_tree_has_items:
 * @self: The #GsvCompletionTree
 *
 * Returns TRUE if the tree has one or more items.
 */
gboolean
gsv_completion_tree_has_items(GsvCompletionTree *self)
{
	GtkTreeIter iter;
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->tree_view));
	return gtk_tree_model_get_iter_first(GTK_TREE_MODEL(model),&iter);
}

/**
 * gsv_completion_tree_select_first:
 * @self: The #GsvCompletionTree
 *
 * This functions selects the first item on the tree
 *
 * Returns TRUE if there is an item and it has been selected
 */
gboolean
gsv_completion_tree_select_first(GsvCompletionTree *self)
{
	GtkTreeIter iter;
	GtkTreePath* path;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	if (!GTK_WIDGET_VISIBLE(self))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->tree_view));

	if (gtk_tree_selection_get_mode(selection) == GTK_SELECTION_NONE)
		return FALSE;

	model = gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->tree_view));
		
	gtk_tree_model_get_iter_first(model, &iter);
	gtk_tree_selection_select_iter(selection, &iter);
	path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(self->priv->tree_view), path, NULL, FALSE, 0, 0);
	gtk_tree_path_free(path);
	return TRUE;
}

/**
 * gsv_completion_tree_select_last:
 * @self: The #GsvCompletionTree
 *
 * This functions selects the last item on the tree
 *
 * Returns TRUE if there is an item and it has been selected
 */
gboolean 
gsv_completion_tree_select_last(GsvCompletionTree *self)
{
	GtkTreeIter iter;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	GtkTreePath* path;
	gint children;
	
	if (!GTK_WIDGET_VISIBLE(self))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->tree_view));
	model = gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->tree_view));
	
	if (gtk_tree_selection_get_mode(selection) == GTK_SELECTION_NONE)
		return FALSE;
	
	children = gtk_tree_model_iter_n_children(model, NULL);
	if (children > 0)
	{
		gtk_tree_model_iter_nth_child(model, &iter, NULL, children - 1);
	
		gtk_tree_selection_select_iter(selection, &iter);
		path = gtk_tree_model_get_path(model, &iter);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(self->priv->tree_view), path, NULL, FALSE, 0, 0);
		gtk_tree_path_free(path);
		return TRUE;
	}
	return FALSE;
}

/**
 * gsv_completion_tree_select_previous:
 * @self: The #GsvCompletionTree
 * @rows: the number of the previous items to select
 *
 * This functions selects the rows number of items before the current.
 *
 * Returns TRUE if there is an item and it has been selected. If rows=5 but the tree
 * only have 3 items, it returns true too.
 */
gboolean
gsv_completion_tree_select_previous(GsvCompletionTree *self, 
					gint rows)
{
	GtkTreeIter iter;
	GtkTreePath* path;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	
	if (!GTK_WIDGET_VISIBLE(self))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->tree_view));
	
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
			gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(self->priv->tree_view), path, NULL, FALSE, 0, 0);
		}
		gtk_tree_path_free(path);
	}
	else
	{
		return gsv_completion_tree_select_first(self);
	}
	
	return TRUE;
}

/**
 * gsv_completion_tree_select_next:
 * @self: The #GsvCompletionTree
 * @rows: the number of the next items to select
 *
 * This functions selects the rows number of items after the current.
 *
 * Returns TRUE if there is an item and it has been selected. If rows=5 but the tree
 * only have 3 items, it returns true too.
 */
gboolean
gsv_completion_tree_select_next(GsvCompletionTree *self, 
					gint rows)
{
	GtkTreeIter iter;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	
	if (!GTK_WIDGET_VISIBLE(self))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->tree_view));
	if (gtk_tree_selection_get_mode(selection) == GTK_SELECTION_NONE)
		return FALSE;
	
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gint i;
		GtkTreePath* path;
		for (i = 0; i < rows; i++)
		{
			if (!gtk_tree_model_iter_next(model, &iter))
				return gsv_completion_tree_select_last(self);
		}
		gtk_tree_selection_select_iter(selection, &iter);
		path = gtk_tree_model_get_path(model, &iter);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(self->priv->tree_view), path, NULL, FALSE, 0, 0);
		gtk_tree_path_free(path);
	}
	else
	{
		return gsv_completion_tree_select_first(self);
	}
	return TRUE;
}

/**
 * gsv_completion_tree_new:
 *
 * Create a new GsvCompletionTree
 *
 * Returns the new #GsvCompletionTree
 *
 */
GtkWidget*
gsv_completion_tree_new()
{
	GsvCompletionTree *self = GSV_COMPLETION_TREE ( g_object_new (gsv_completion_tree_get_type() , NULL));
	return GTK_WIDGET(self);
}





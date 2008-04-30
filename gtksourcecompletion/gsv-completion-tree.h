/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsv-completion-tree.h
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
 
#ifndef GTK_SOURCE_COMPLETION_TREE_H
#define GTK_SOURCE_COMPLETION_TREE_H

#include <gtk/gtk.h>
#include "gtksourcecompletion-item.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GSV_TYPE_COMPLETION_TREE              (gsv_completion_tree_get_type())
#define GSV_COMPLETION_TREE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GSV_TYPE_COMPLETION_TREE, GsvCompletionTree))
#define GSV_COMPLETION_TREE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GSV_TYPE_COMPLETION_TREE, GsvCompletionTreeClass))
#define GSV_IS_COMPLETION_TREE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GSV_TYPE_COMPLETION_TREE))
#define GSV_IS_COMPLETION_TREE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GSV_TYPE_COMPLETION_TREE))
#define GSV_COMPLETION_TREE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GSV_TYPE_COMPLETION_TREE, GsvCompletionTreeClass))

typedef struct _GsvCompletionTreePriv GsvCompletionTreePriv;
typedef struct _GsvCompletionTree GsvCompletionTree;
typedef struct _GsvCompletionTreeClass GsvCompletionTreeClass;


struct _GsvCompletionTreeClass
{
	GtkScrolledWindowClass parent_class;
	void (* item_selected)(GsvCompletionTree *tree,
			       GtkSourceCompletionItem *item);
	void (* selection_changed)(GsvCompletionTree *tree,
				   GtkSourceCompletionItem *item);
};

struct _GsvCompletionTree
{
	GtkScrolledWindow parent;
	GsvCompletionTreePriv *priv;
};

GType 
gsv_completion_tree_get_type (void) G_GNUC_CONST;

/**
 * gsv_completion_tree_new:
 *
 * Create a new GsvCompletionTree
 *
 * Returns the new #GsvCompletionTree
 *
 */
GtkWidget*
gsv_completion_tree_new();

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
				      GtkSourceCompletionItem **item);
/**
 * gsv_completion_tree_select_first:
 * @self: The #GsvCompletionTree
 *
 * This functions selects the first item on the tree
 *
 * Returns TRUE if there is an item and it has been selected
 */
gboolean
gsv_completion_tree_select_first(GsvCompletionTree *self);

/**
 * gsv_completion_tree_select_last:
 * @self: The #GsvCompletionTree
 *
 * This functions selects the last item on the tree
 *
 * Returns TRUE if there is an item and it has been selected
 */
gboolean 
gsv_completion_tree_select_last(GsvCompletionTree *self);

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
				    gint rows);

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
				gint rows);

/**
 * gsv_completion_tree_get_selected_item:
 * @self: the #GsvCompletionTree
 * @item: Sets the ponter to the selected item.
 *
 * Sets the param item to the selected item if there is an item selected.
 *
 * Returns TRUE if there is an item selected
 */
gboolean
gsv_completion_tree_get_selected_item(GsvCompletionTree *self,
				      GtkSourceCompletionItem **item);
				      
/**
 * gsv_completion_tree_clear:
 * @self: the #GsvCompletionTree
 *
 * Clear the tree model and free the items 
 */
void
gsv_completion_tree_clear(GsvCompletionTree *self);

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
			     GtkSourceCompletionItem* data);

/**
 * gsv_completion_tree_has_items:
 * @self: The #GsvCompletionTree
 *
 * Returns TRUE if the tree has one or more items.
 */
gboolean
gsv_completion_tree_has_items(GsvCompletionTree *self);

G_END_DECLS
#endif

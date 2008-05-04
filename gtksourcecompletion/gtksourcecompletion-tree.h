/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-tree.h
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
#include "gtksourcecompletion-proposal.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTK_TYPE_SOURCE_COMPLETION_TREE              (gtk_source_completion_tree_get_type())
#define GTK_SOURCE_COMPLETION_TREE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_SOURCE_COMPLETION_TREE, GtkSourceCompletionTree))
#define GTK_SOURCE_COMPLETION_TREE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_SOURCE_COMPLETION_TREE, GtkSourceCompletionTreeClass))
#define GTK_IS_SOURCE_COMPLETION_TREE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_SOURCE_COMPLETION_TREE))
#define GTK_IS_SOURCE_COMPLETION_TREE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_SOURCE_COMPLETION_TREE))
#define GTK_SOURCE_COMPLETION_TREE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_SOURCE_COMPLETION_TREE, GtkSourceCompletionTreeClass))

typedef struct _GtkSourceCompletionTreePriv GtkSourceCompletionTreePriv;
typedef struct _GtkSourceCompletionTree GtkSourceCompletionTree;
typedef struct _GtkSourceCompletionTreeClass GtkSourceCompletionTreeClass;


struct _GtkSourceCompletionTreeClass
{
	GtkScrolledWindowClass parent_class;
	void (* proposal_selected)(GtkSourceCompletionTree *tree,
			       GtkSourceCompletionProposal *proposal);
	void (* selection_changed)(GtkSourceCompletionTree *tree,
				   GtkSourceCompletionProposal *proposal);
};

struct _GtkSourceCompletionTree
{
	GtkScrolledWindow parent;
	GtkSourceCompletionTreePriv *priv;
};

GType 
gtk_source_completion_tree_get_type (void) G_GNUC_CONST;

/**
 * gtk_source_completion_tree_new:
 *
 * Create a new GtkSourceCompletionTree
 *
 * Returns the new #GtkSourceCompletionTree
 *
 */
GtkWidget*
gtk_source_completion_tree_new();

/**
 * gtk_source_completion_tree_get_selected_proposal:
 * @self: The #GtkSourceCompletionTree
 * @proposal: A reference of a proposal. This function sets the pointer to the selected proposal.
 *
 * @Returns TRUE if there is an proposal selected
 *
 */
gboolean
gtk_source_completion_tree_get_selected_proposal(GtkSourceCompletionTree *self,
				      GtkSourceCompletionProposal **proposal);
/**
 * gtk_source_completion_tree_select_first:
 * @self: The #GtkSourceCompletionTree
 *
 * This functions selects the first proposal on the tree
 *
 * Returns TRUE if there is an proposal and it has been selected
 */
gboolean
gtk_source_completion_tree_select_first(GtkSourceCompletionTree *self);

/**
 * gtk_source_completion_tree_select_last:
 * @self: The #GtkSourceCompletionTree
 *
 * This functions selects the last proposal on the tree
 *
 * Returns TRUE if there is an proposal and it has been selected
 */
gboolean 
gtk_source_completion_tree_select_last(GtkSourceCompletionTree *self);

/**
 * gtk_source_completion_tree_select_previous:
 * @self: The #GtkSourceCompletionTree
 * @rows: the number of the previous proposals to select
 *
 * This functions selects the rows number of proposals before the current.
 *
 * Returns TRUE if there is an proposal and it has been selected. If rows=5 but the tree
 * only have 3 proposals, it returns true too.
 */
gboolean
gtk_source_completion_tree_select_previous(GtkSourceCompletionTree *self, 
				    gint rows);

/**
 * gtk_source_completion_tree_select_next:
 * @self: The #GtkSourceCompletionTree
 * @rows: the number of the next proposals to select
 *
 * This functions selects the rows number of proposals after the current.
 *
 * Returns TRUE if there is an proposal and it has been selected. If rows=5 but the tree
 * only have 3 proposals, it returns true too.
 */
gboolean
gtk_source_completion_tree_select_next(GtkSourceCompletionTree *self, 
				gint rows);

/**
 * gtk_source_completion_tree_get_selected_proposal:
 * @self: the #GtkSourceCompletionTree
 * @proposal: Sets the ponter to the selected proposal.
 *
 * Sets the param proposal to the selected proposal if there is an proposal selected.
 *
 * Returns TRUE if there is an proposal selected
 */
gboolean
gtk_source_completion_tree_get_selected_proposal(GtkSourceCompletionTree *self,
				      GtkSourceCompletionProposal **proposal);
				      
/**
 * gtk_source_completion_tree_clear:
 * @self: the #GtkSourceCompletionTree
 *
 * Clear the tree model and free the proposals 
 */
void
gtk_source_completion_tree_clear(GtkSourceCompletionTree *self);

/**
 * gtk_source_completion_tree_add_data:
 * @self: The #GtkSourceCompletionTree
 * @data: the proposal to add to the tree
 *
 * Adds a new proposal into the tree
 *
 */
void
gtk_source_completion_tree_add_data(GtkSourceCompletionTree *self,
			     GtkSourceCompletionProposal* data);

/**
 * gtk_source_completion_tree_has_proposals:
 * @self: The #GtkSourceCompletionTree
 *
 * Returns TRUE if the tree has one or more proposals.
 */
gboolean
gtk_source_completion_tree_has_proposals(GtkSourceCompletionTree *self);

G_END_DECLS
#endif

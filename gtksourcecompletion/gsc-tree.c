/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-tree.c
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

#include "gsc-tree.h"

#define COL_PIXBUF 0
#define COL_NAME 1
#define COL_DATA 2

#define GSC_TREE_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object),\
					 GSC_TYPE_TREE,                    \
					 GscTreePriv))
					 
#include <string.h>

/* Signals */
enum
{
	ITEM_SELECTED,
	SELECTION_CHANGED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

struct _GscTreePriv
{
	GtkWidget *tree_view;
	gboolean destroy_has_run;
	GtkListStore *list_store;
	GtkTreeModelFilter *model_filter;
	const gchar* current_filter;
	gboolean active_filter;
};

static void
_tree_row_activated_cb (GtkTreeView *tree_view,
			GtkTreePath *path,
			GtkTreeViewColumn *column,
			gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GscProposal *data;
	GscTree *self;
	GValue value_name = {0,};
	
	self = GSC_TREE(user_data);
	
	model = gtk_tree_view_get_model(tree_view);
	
	gtk_tree_model_get_iter(model,&iter,path);
	gtk_tree_model_get_value(model,&iter,COL_DATA,&value_name);
	data = (GscProposal*)g_value_get_pointer(&value_name);
	
	g_signal_emit (G_OBJECT (self), signals[ITEM_SELECTED], 0, data);
}

static void 
_selection_changed_cd(GtkTreeSelection *treeselection,
		      gpointer user_data)
{
	GscProposal *proposal;
	GscTree *self = GSC_TREE(user_data);
	if (gsc_tree_get_selected_proposal(self,&proposal))
	{
		g_signal_emit (G_OBJECT (self), signals[SELECTION_CHANGED], 0, proposal);
	}
}


G_DEFINE_TYPE(GscTree, gsc_tree, GTK_TYPE_SCROLLED_WINDOW);

static gboolean
_filter_by_name_func (GtkTreeModel *model,
			GtkTreeIter *iter,
			gpointer data)
{
	
	GscTree *self = GSC_TREE(data);
	if (!self->priv->active_filter)
		return TRUE;
		
	const gchar* filter = self->priv->current_filter;
	if (filter == NULL)
		return TRUE;
		
	GValue current_name_value = {0,};
	gtk_tree_model_get_value(model,
				iter,
				1,
				&current_name_value);
	const gchar* current_name = g_value_get_string(&current_name_value);
	
	if (current_name == NULL)
		return TRUE;
	
	gint len_cur = strlen (filter);
	if (strncmp(filter,current_name,len_cur)==0)
	{
		return TRUE;
	}
	return FALSE;
}

static void
gsc_tree_finalize (GObject *object)
{
	g_debug("Finish GscTree");
	GscTree *self = GSC_TREE(object);
	self->priv->current_filter = NULL;
	G_OBJECT_CLASS (gsc_tree_parent_class)->finalize (object);
}

static void
gsc_tree_class_init (GscTreeClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	g_type_class_add_private (klass, sizeof(GscTreePriv));
	
	object_class->finalize = gsc_tree_finalize;
	
	/**
	 * GscTree::proposal-selected:
	 * @gsctree: the object which received the signal.
	 * @proposal: The #GscProposal selected
	 *
	 * Emits when the user selects a proposal of this tree.
	 **/
	signals[ITEM_SELECTED] =
		g_signal_new ("proposal-selected",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GscTreeClass, proposal_selected),
			      NULL, 
			      NULL,
			      g_cclosure_marshal_VOID__POINTER, 
			      G_TYPE_NONE,
			      1,
			      GTK_TYPE_POINTER);
	
	/**
	 * GscTree::selection-changed:
	 * @gsctree: the object which received the signal.
	 * @proposal: The current #GscProposal in the tree
	 *
	 * Emits when the user change the current proposal
	 **/		      
	signals[SELECTION_CHANGED] =
		g_signal_new ("selection-changed",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GscTreeClass, proposal_selected),
			      NULL, 
			      NULL,
			      g_cclosure_marshal_VOID__POINTER, 
			      G_TYPE_NONE,
			      1,
			      GTK_TYPE_POINTER);
}

static void
gsc_tree_init (GscTree *self)
{
	g_debug("Init GscTree");
	self->priv = GSC_TREE_GET_PRIVATE(self);
	self->priv->destroy_has_run = FALSE;
	self->priv->current_filter = NULL;
	self->priv->active_filter = FALSE;
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(self),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	
	gtk_scrolled_window_set_hadjustment(GTK_SCROLLED_WINDOW(self),NULL);
	gtk_scrolled_window_set_vadjustment(GTK_SCROLLED_WINDOW(self),NULL);

	self->priv->tree_view = gtk_tree_view_new();
	GValue val = {0,};
	g_value_init(&val,G_TYPE_BOOLEAN);
	g_value_set_boolean(&val,FALSE);
	g_object_set_property(G_OBJECT(self->priv->tree_view),"can-focus",&val);
	gtk_tree_view_set_headers_visible(GTK_TREE_VIEW(self->priv->tree_view),FALSE);
	
	gtk_container_add(GTK_CONTAINER(self),self->priv->tree_view);

	/* Create the Tree */
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkCellRenderer* renderer_pixbuf;
	GtkTreeViewColumn* column_pixbuf;
	
	renderer_pixbuf = gtk_cell_renderer_pixbuf_new();
	column_pixbuf = gtk_tree_view_column_new_with_attributes ("Pixbuf",
								  renderer_pixbuf, 
								  "pixbuf", 
								  COL_PIXBUF, 
								  NULL);
	
	column = gtk_tree_view_column_new();
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (column, renderer, FALSE);
	gtk_tree_view_column_set_attributes (column,renderer,"text",COL_NAME,NULL);

	gtk_tree_view_append_column (GTK_TREE_VIEW(self->priv->tree_view), column_pixbuf);
	gtk_tree_view_append_column (GTK_TREE_VIEW(self->priv->tree_view), column);
	
	/* Create the model */
	self->priv->list_store = gtk_list_store_new (4,
						       GDK_TYPE_PIXBUF, 
						       G_TYPE_STRING, 
						       G_TYPE_POINTER, 
						       G_TYPE_POINTER);
	
	GtkTreeModel *model = gtk_tree_model_filter_new(GTK_TREE_MODEL(self->priv->list_store),
							NULL);

	self->priv->model_filter = GTK_TREE_MODEL_FILTER(model);
	
	gtk_tree_model_filter_set_visible_func(self->priv->model_filter,
						_filter_by_name_func,
						self,
						NULL);

	gtk_tree_view_set_model(GTK_TREE_VIEW(self->priv->tree_view),
				model);
	
	
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

gboolean
gsc_tree_get_selected_proposal(GscTree *self,
				      GscProposal **proposal)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GValue value_proposal = {0,};
	GtkTreeSelection *selection = gtk_tree_view_get_selection(GTK_TREE_VIEW(self->priv->tree_view));
	if (gtk_tree_selection_get_selected(selection,NULL, &iter))
	{
		model = gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->tree_view));
		gtk_tree_model_get_value(model,&iter,COL_DATA,&value_proposal);
		*proposal = (GscProposal*)g_value_get_pointer(&value_proposal);
		
		return TRUE;
	}
	
	return FALSE;
	
}

void
gsc_tree_add_data(GscTree *self,
			     GscProposal* data)
{
	g_assert(data != NULL);
	
	GtkTreeIter iter;
	GtkListStore *store = self->priv->list_store;
	
	gtk_list_store_append (store,&iter);
			
	gtk_list_store_set (store, 
			    &iter,
			    COL_PIXBUF, gsc_proposal_get_icon(data),
			    COL_NAME, gsc_proposal_get_label(data),
			    COL_DATA, data,
			    -1);
}

void
gsc_tree_clear(GscTree *self)
{
	GtkListStore *store = self->priv->list_store;
	GtkTreeModel *model = GTK_TREE_MODEL(store);
	GtkTreeIter iter;
	GscProposal *data;
	
	if (gtk_tree_model_get_iter_first(model,&iter))
	{
		do
		{
			GValue value_data = {0,};
			gtk_tree_model_get_value(model,&iter,COL_DATA,&value_data);
			data = (GscProposal*)g_value_get_pointer(&value_data);
			g_object_unref(data);
		}while(gtk_tree_model_iter_next(model,&iter));
	}
	
	gtk_list_store_clear(store);
}

gboolean
gsc_tree_select_first(GscTree *self)
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
		
	if (gtk_tree_model_get_iter_first(model, &iter))
	{
		gtk_tree_selection_select_iter(selection, &iter);
		path = gtk_tree_model_get_path(model, &iter);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(self->priv->tree_view), 
						   path, 
						   NULL, 
						   FALSE, 
						   0, 
						   0);
		gtk_tree_path_free(path);
		return TRUE;
	}
	else
	{
		return FALSE;
	}
	
}

gboolean 
gsc_tree_select_last(GscTree *self)
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
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(self->priv->tree_view), 
					     path, 
					     NULL, 
					     FALSE, 
					     0, 
					     0);
		gtk_tree_path_free(path);
		return TRUE;
	}
	return FALSE;
}

gboolean
gsc_tree_select_previous(GscTree *self, 
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
			gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(self->priv->tree_view), 
						     path, 
						     NULL, 
						     FALSE, 
						     0, 
						     0);
		}
		gtk_tree_path_free(path);
	}
	else
	{
		return gsc_tree_select_first(self);
	}
	
	return TRUE;
}

gboolean
gsc_tree_select_next(GscTree *self, 
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
				return gsc_tree_select_last(self);
		}
		gtk_tree_selection_select_iter(selection, &iter);
		path = gtk_tree_model_get_path(model, &iter);
		gtk_tree_view_scroll_to_cell(GTK_TREE_VIEW(self->priv->tree_view), 
					     path, 
					     NULL, 
					     FALSE, 
					     0, 
					     0);
		gtk_tree_path_free(path);
	}
	else
	{
		return gsc_tree_select_first(self);
	}
	return TRUE;
}

GtkWidget*
gsc_tree_new()
{
	GscTree *self = GSC_TREE (g_object_new (gsc_tree_get_type() , NULL));
	return GTK_WIDGET(self);
}

gint 
gsc_tree_get_num_proposals(GscTree *self)
{
	GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(self->priv->tree_view));
	return gtk_tree_model_iter_n_children(model, NULL);
}



void
gsc_tree_filter(GscTree *self, const gchar* filter)
{
	self->priv->active_filter = TRUE;
	self->priv->current_filter = filter;
	gtk_tree_model_filter_refilter(self->priv->model_filter);
	self->priv->active_filter = FALSE;
	self->priv->current_filter = NULL;
	//gsc_tree_select_first(self);
}



/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-completion.c
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
 *
 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */
 
/**
 * SECTION:gsc-completion
 * @title: GscCompletion
 * @short_description: Main Completion Object
 *
 * This is the main completion object. It manages all providers (#GscProvider) and 
 * triggers (#GscTrigger) for a #GtkTextView.. 
 */

#include <gdk/gdkkeysyms.h> 
#include "gsc-i18n.h"
#include "gsc-utils.h"
#include "gsc-marshal.h"
#include "gsc-completion.h"
#include "gsc-completion-priv.h"
#include <string.h>

static gboolean lib_initialized = FALSE;

#define WINDOW_WIDTH 350
#define WINDOW_HEIGHT 200

#define GSC_COMPLETION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object),\
					 GSC_TYPE_COMPLETION,                    \
					 GscCompletionPriv))

enum
{
	COLUMN_PIXBUF,
	COLUMN_NAME,
	COLUMN_DATA,
	N_COLUMNS
};

typedef struct 
{
	GscTrigger *trigger;
	GscProvider *provider;
} PTPair;

/* Signals */
enum
{
	PROPOSAL_SELECTED,
	DISPLAY_INFO,
	LAST_SIGNAL
};

enum
{
	PROP_0,
	PROP_MANAGE_KEYS,
	PROP_REMEMBER_INFO_VISIBILITY,
	PROP_SELECT_ON_SHOW,
	PROP_ACTIVE
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(GscCompletion, gsc_completion, GTK_TYPE_WINDOW);

/* **************** GtkTextView-GscCompletion Control *********** */

/*
 * We save a map with a GtkTextView and his GscCompletion. If you 
 * call twice to gsc_proposal_new, the second time it returns
 * the previous created GscCompletion, not creates a new one
 *
 * FIXME We will remove this functions when we will integrate 
 * Gsc in GtkSourceView
 */

static GHashTable *gsccompletion_map = NULL;

static GscCompletion* 
completion_control_get_completion (GtkTextView *view)
{
	if (gsccompletion_map == NULL)
		gsccompletion_map = g_hash_table_new (g_direct_hash,
						   g_direct_equal);

	return g_hash_table_lookup (gsccompletion_map, view);
}

static void 
completion_control_add_completion (GtkTextView *view,
				   GscCompletion *comp)
{
	if (gsccompletion_map == NULL)
		gsccompletion_map = g_hash_table_new (g_direct_hash,
						   g_direct_equal);
	g_hash_table_insert (gsccompletion_map, view, comp);
}

static void 
completion_control_remove_completion (GtkTextView *view)
{
	if (gsccompletion_map == NULL)
		gsccompletion_map = g_hash_table_new (g_direct_hash,
						   g_direct_equal);
	g_hash_table_remove (gsccompletion_map, view);
}
/* ********************************************************************* */

/************************** TreeView utility functions *****************/
static gboolean
filter_func (GtkTreeModel *model,
	     GtkTreeIter *iter,
	     gpointer data)
{
	GscTree *tree = (GscTree *)data;
	GscProposal *proposal = NULL;
	
	if (!tree->filter_active || !tree->filter_func)
		return TRUE;
		
	gtk_tree_model_get (model,
			    iter,
			    COLUMN_DATA,
			    &proposal,
			    -1);
	
	if (proposal == NULL)
		return TRUE;
	
	return tree->filter_func (proposal, tree->filter_data);
}

static void
row_activated_cb (GtkTreeView *tree_view,
		  GtkTreePath *path,
		  GtkTreeViewColumn *column,
		  GscCompletion *self)
{
	_gsc_completion_select_current_proposal (self);
}

static void 
selection_changed_cd (GtkTreeSelection *selection, 
		      GscCompletion *self)
{
	if (GTK_WIDGET_VISIBLE (self->priv->info_window))
	{
		_gsc_completion_update_info_pos (self);
	}
}

static GscTree *
create_treeview (GscCompletion *completion)
{
	GscTree *tree;
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkCellRenderer* renderer_pixbuf;
	GtkTreeViewColumn* column_pixbuf;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	
	tree = g_slice_new (GscTree);
	
	tree->treeview = GTK_TREE_VIEW (gtk_tree_view_new ());
	gtk_widget_show (GTK_WIDGET (tree->treeview));
	
	tree->filter_data = NULL;
	tree->filter_active = FALSE;
	tree->filter_func = NULL;

	g_object_set (tree->treeview, "can-focus", FALSE, NULL);
	gtk_tree_view_set_headers_visible (GTK_TREE_VIEW (tree->treeview),
					   FALSE);
	
	/* Create the Tree */
	renderer_pixbuf = gtk_cell_renderer_pixbuf_new ();
	column_pixbuf = gtk_tree_view_column_new_with_attributes ("Pixbuf",
								  renderer_pixbuf, 
								  "pixbuf", 
								  COLUMN_PIXBUF, 
								  NULL);
	
	column = gtk_tree_view_column_new ();
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (column, renderer, FALSE);
	gtk_tree_view_column_set_attributes (column, renderer,
					     "text", COLUMN_NAME, NULL);

	gtk_tree_view_append_column (tree->treeview, column_pixbuf);
	gtk_tree_view_append_column (tree->treeview, column);
	
	/* Create the model */
	tree->list_store = gtk_list_store_new (N_COLUMNS,
					       GDK_TYPE_PIXBUF, 
					       G_TYPE_STRING, 
					       G_TYPE_POINTER);
	
	model = gtk_tree_model_filter_new (GTK_TREE_MODEL (tree->list_store),
					   NULL);

	tree->model_filter = GTK_TREE_MODEL_FILTER (model);
	
	gtk_tree_model_filter_set_visible_func (tree->model_filter,
						filter_func,
						tree,
						NULL);

	gtk_tree_view_set_model (tree->treeview,
				 model);
	
	g_signal_connect (tree->treeview,
			  "row-activated",
			  G_CALLBACK (row_activated_cb),
			  completion);
	
	selection = gtk_tree_view_get_selection (tree->treeview);
	g_signal_connect (selection,
			  "changed",
			  G_CALLBACK (selection_changed_cd),
			  completion);
	
	return tree;
}

static gboolean
gsc_tree_get_selected_proposal (GscTree *tree,
				GscProposal **proposal)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkTreeSelection *selection;
	
	selection = gtk_tree_view_get_selection (tree->treeview);
	
	if (gtk_tree_selection_get_selected (selection, NULL, &iter))
	{
		model = gtk_tree_view_get_model (tree->treeview);
		
		gtk_tree_model_get (model, &iter,
				    COLUMN_DATA,
				    proposal, -1);
		
		return TRUE;
	}
	
	return FALSE;
}

static void
gsc_tree_add_data (GscTree *tree,
		   GscProposal *data)
{
	GtkTreeIter iter;

	g_assert (data != NULL);
	
	gtk_list_store_append (tree->list_store, &iter);
			
	gtk_list_store_set (tree->list_store, 
			    &iter,
			    COLUMN_PIXBUF, gsc_proposal_get_icon (data),
			    COLUMN_NAME, gsc_proposal_get_label (data),
			    COLUMN_DATA, data,
			    -1);
}

static void
gsc_tree_clear (GscTree *tree)
{
	GtkTreeModel *model = GTK_TREE_MODEL (tree->list_store);
	GtkTreeIter iter;
	GscProposal *data;
	
	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		do
		{
			gtk_tree_model_get (model, &iter,
					    COLUMN_DATA, &data, -1);
			g_object_unref (data);
		} while (gtk_tree_model_iter_next (model, &iter));
	}
	
	gtk_list_store_clear (tree->list_store);
}

static gboolean
gsc_tree_select_first (GscTree *tree)
{
	GtkTreeIter iter;
	GtkTreePath* path;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	
	selection = gtk_tree_view_get_selection (tree->treeview);

	if (gtk_tree_selection_get_mode (selection) == GTK_SELECTION_NONE)
		return FALSE;

	model = gtk_tree_view_get_model (tree->treeview);
		
	if (gtk_tree_model_get_iter_first (model, &iter))
	{
		gtk_tree_selection_select_iter (selection, &iter);
		path = gtk_tree_model_get_path (model, &iter);
		gtk_tree_view_scroll_to_cell (tree->treeview,
					      path, 
					      NULL, 
					      FALSE, 
					      0, 
					      0);
		gtk_tree_path_free (path);
		return TRUE;
	}
	
	return FALSE;
}

static gboolean 
gsc_tree_select_last (GscTree *tree)
{
	GtkTreeIter iter;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	GtkTreePath* path;
	gint children;
	
	if (!GTK_WIDGET_VISIBLE (tree->treeview))
		return FALSE;
	
	selection = gtk_tree_view_get_selection (tree->treeview);
	model = gtk_tree_view_get_model (tree->treeview);
	
	if (gtk_tree_selection_get_mode (selection) == GTK_SELECTION_NONE)
		return FALSE;
	
	children = gtk_tree_model_iter_n_children (model, NULL);
	if (children > 0)
	{
		gtk_tree_model_iter_nth_child (model, &iter,
					       NULL, children - 1);
	
		gtk_tree_selection_select_iter (selection, &iter);
		path = gtk_tree_model_get_path (model, &iter);
		gtk_tree_view_scroll_to_cell (tree->treeview,
					      path, 
					      NULL, 
					      FALSE, 
					      0, 
					      0);
		gtk_tree_path_free (path);
		return TRUE;
	}
	
	return FALSE;
}

static gboolean
gsc_tree_select_previous (GscTree *tree, 
			  gint rows)
{
	GtkTreeIter iter;
	GtkTreePath* path;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	
	if (!GTK_WIDGET_VISIBLE (tree->treeview))
		return FALSE;
	
	selection = gtk_tree_view_get_selection (tree->treeview);
	
	if (gtk_tree_selection_get_mode (selection) == GTK_SELECTION_NONE)
		return FALSE;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		gint i;
		
		path = gtk_tree_model_get_path (model, &iter);
		
		for (i=0; i  < rows; i++)
			gtk_tree_path_prev (path);
		
		if (gtk_tree_model_get_iter(model, &iter, path))
		{
			gtk_tree_selection_select_iter (selection, &iter);
			gtk_tree_view_scroll_to_cell (tree->treeview,
						      path, 
						      NULL, 
						      FALSE, 
						      0, 
						      0);
		}
		gtk_tree_path_free (path);
	}
	else
	{
		return gsc_tree_select_first (tree);
	}
	
	return TRUE;
}

static gboolean
gsc_tree_select_next (GscTree *tree, 
		      gint rows)
{
	GtkTreeIter iter;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	
	if (!GTK_WIDGET_VISIBLE (tree->treeview))
		return FALSE;
	
	selection = gtk_tree_view_get_selection (tree->treeview);
	if (gtk_tree_selection_get_mode (selection) == GTK_SELECTION_NONE)
		return FALSE;
	
	if (gtk_tree_selection_get_selected (selection, &model, &iter))
	{
		GtkTreePath* path;
		gint i;
		
		for (i = 0; i < rows; i++)
		{
			if (!gtk_tree_model_iter_next (model, &iter))
				return gsc_tree_select_last (tree);
		}
		gtk_tree_selection_select_iter (selection, &iter);
		path = gtk_tree_model_get_path (model, &iter);
		gtk_tree_view_scroll_to_cell (tree->treeview,
					      path, 
					      NULL, 
					      FALSE, 
					      0, 
					      0);
		gtk_tree_path_free (path);
	}
	else
	{
		return gsc_tree_select_first (tree);
	}
	
	return TRUE;
}

static gint 
gsc_tree_get_num_proposals (GscTree *tree)
{
	GtkTreeModel *model = gtk_tree_view_get_model (tree->treeview);
	
	return gtk_tree_model_iter_n_children (model, NULL);
}

static void
gsc_tree_filter_visible (GscTree *tree,
			 GscCompletionFilterFunc func,
			 gpointer user_data)
{
	tree->filter_active = TRUE;
	tree->filter_data = user_data;
	tree->filter_func = func;
	gtk_tree_model_filter_refilter (tree->model_filter);
	tree->filter_active = FALSE;
	tree->filter_data = NULL;
	tree->filter_func = NULL;
}
/************************ TreeView utility functions END *****************/

static void
free_pair (gpointer data)
{
	PTPair *ptp = (PTPair *)data;
	g_object_unref (ptp->provider);
	g_slice_free (PTPair, ptp);
}

static GscTree*
get_current_tree (GscCompletion *self)
{
	return self->priv->active_page->tree;
}

static void
end_completion (GscCompletion *self)
{
	if (GTK_WIDGET_VISIBLE (self))
	{
		gtk_widget_hide (GTK_WIDGET (self));
		/*
		* We are connected to the hide signal too. Then end_completion 
		* will be called again but the popup will not be visible
		*/
	}
	else
	{
		GList *l;
		
		for (l = self->priv->prov_trig; l != NULL; l = g_list_next (l))
		{
			PTPair *ptp = (PTPair *)l->data;
			if (ptp->trigger == self->priv->active_trigger)
				gsc_provider_finish (GSC_PROVIDER (ptp->provider));
		}
		
		self->priv->active_trigger = NULL;
	}
}

static void
gsc_completion_clear (GscCompletion *self)
{
	GList *l;
	
	g_return_if_fail (GSC_IS_COMPLETION (self));
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscCompletionPage *page = (GscCompletionPage *)l->data;
		
		gsc_tree_clear (page->tree);
	}
}

static gboolean
update_pages_visibility (GscCompletion *self)
{
	GList *l;
	gboolean first_set = FALSE;
	guint num_pages_with_data = 0;
	gint i = 0;
	GtkAdjustment *ad;
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscCompletionPage *page = (GscCompletionPage *)l->data;
		
		if (gsc_tree_get_num_proposals (page->tree) > 0)
		{
			/*Selects the first page with data*/
			if (!first_set)
			{
				gtk_notebook_set_current_page (GTK_NOTEBOOK (self->priv->notebook),
							       i);
				ad = gtk_tree_view_get_vadjustment (page->tree->treeview);
				gtk_adjustment_set_value (ad, 0);
				ad = gtk_tree_view_get_hadjustment (page->tree->treeview);
				gtk_adjustment_set_value (ad, 0);
				first_set = TRUE;
			}
			num_pages_with_data++;
		}
		i++;
	}
	if (num_pages_with_data > 1)
	{
		gtk_widget_show (self->priv->next_page_button);
		gtk_widget_show (self->priv->prev_page_button);
	}
	else
	{
		gtk_widget_hide (self->priv->next_page_button);
		gtk_widget_hide (self->priv->prev_page_button);
	}
	
	return num_pages_with_data > 0;
}

static void
gsc_completion_show_or_update (GtkWidget *widget)
{
	gboolean data;
	
	/*Only show the popup, the positions is set before this function*/
	GscCompletion *self = GSC_COMPLETION (widget);
	
	data = update_pages_visibility (self);
	
	if (data && !GTK_WIDGET_VISIBLE (self))
	{
		GTK_WIDGET_CLASS (gsc_completion_parent_class)->show (widget);
		
		if (!self->priv->remember_info_visibility)
			self->priv->info_visible = FALSE;
		
		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->priv->info_button),
					      self->priv->info_visible);
	}
}

static void
gsc_completion_page_next (GscCompletion *self)
{
	gint pages;
	gint current_page;
	gint page;
	GscCompletionPage *popup_page;
	
	g_return_if_fail (GSC_IS_COMPLETION (self));

	pages = g_list_length (self->priv->pages);
	current_page = g_list_index (self->priv->pages, self->priv->active_page);
	page = current_page;
	
	do
	{
		if (page == pages - 1)
		{
			page = 0;
		}
		else
		{
			page++;
		}
		popup_page = (GscCompletionPage *)g_list_nth_data (self->priv->pages, page);
	}
	while (gsc_tree_get_num_proposals (popup_page->tree) == 0 &&
	       page != current_page);

	if (page != current_page)
	{
		gtk_notebook_set_current_page (GTK_NOTEBOOK (self->priv->notebook),
					       page);
	
		/*
		 * After setting the page the active_page was updated
		 * so we can update the tree
		 */
		_gsc_completion_select_first (self);
	
		if (GTK_WIDGET_VISIBLE (self->priv->info_window))
		{
			_gsc_completion_update_info_pos (self);
		}
	}
}

static void
gsc_completion_page_previous (GscCompletion *self)
{
	gint pages;
	gint current_page;
	gint page;
	GscCompletionPage *popup_page;
	
	g_return_if_fail (GSC_IS_COMPLETION (self));
	
	pages = g_list_length (self->priv->pages);
	current_page = g_list_index (self->priv->pages,
				     self->priv->active_page);
	page = current_page;
	
	do
	{
		if (page == 0)
		{
			page = pages - 1;
		}
		else
		{
			page--;
		}
		popup_page = (GscCompletionPage *)g_list_nth_data (self->priv->pages,
								   page);
	}
	while (gsc_tree_get_num_proposals (popup_page->tree) == 0 &&
	       page != current_page);
	
	if (page != current_page)
	{
		gtk_notebook_set_current_page (GTK_NOTEBOOK (self->priv->notebook),
					       page);
	
		/*
		 * After setting the page the active_page was updated
		 * so we can update the tree
		 */
		_gsc_completion_select_first (self);
	
		if (GTK_WIDGET_VISIBLE (self->priv->info_window))
		{
			_gsc_completion_update_info_pos (self);
		}
	}
}

static GscCompletionPage *
gsc_completion_page_new (GscCompletion *self,
			 const gchar *tree_name)
{
	/*Creates the new trees*/
	GscCompletionPage *page;
	GtkWidget *label;
	
	page = g_slice_new (GscCompletionPage);
	
	page->name = g_strdup (tree_name);
	
	page->tree = create_treeview (self);
	self->priv->pages = g_list_append (self->priv->pages,
					   page);
	
	page->tree->scroll = GTK_SCROLLED_WINDOW (gtk_scrolled_window_new (NULL, NULL));
	gtk_widget_show (GTK_WIDGET (page->tree->scroll));
	
	gtk_scrolled_window_set_policy (page->tree->scroll,
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	
	gtk_container_add (GTK_CONTAINER (page->tree->scroll),
			   GTK_WIDGET (page->tree->treeview));
	
	label = gtk_label_new (tree_name);
	
	gtk_notebook_append_page (GTK_NOTEBOOK (self->priv->notebook),
				  GTK_WIDGET (page->tree->scroll),
				  label);
	
	return page;
}

static GscTree*
get_tree_by_name (GscCompletion *self,
		  const gchar* tree_name)
{
	GscCompletionPage *page;
	GList *l;
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		page = (GscCompletionPage *)l->data;
	
		if (strcmp (page->name, tree_name) == 0)
			return page->tree;
	}
	
	page = gsc_completion_page_new (self, tree_name);

	return page->tree;
}

static void
info_toggled_cb (GtkToggleButton *widget,
		 gpointer user_data)
{
	GscCompletion *self = GSC_COMPLETION (user_data);
	
	if (gtk_toggle_button_get_active (widget))
	{
		gtk_widget_show (self->priv->info_window);
	}
	else
	{
		gtk_widget_hide (self->priv->info_window);
	}
}

static void
next_page_cb (GtkWidget *widget,
	      gpointer user_data)
{
	GscCompletion *self = GSC_COMPLETION (user_data);
	
	gsc_completion_page_next (self);
}

static void
prev_page_cb (GtkWidget *widget,
	      gpointer user_data)
{
	GscCompletion *self = GSC_COMPLETION (user_data);
	
	gsc_completion_page_previous (self);
}

static gboolean
switch_page_cb (GtkNotebook *notebook, 
		GtkNotebookPage *n_page,
		gint page_num, 
		gpointer user_data)
{
	GscCompletion *self = GSC_COMPLETION (user_data);
	
	/* Update the active page */
	self->priv->active_page = (GscCompletionPage *)g_list_nth_data (self->priv->pages,
								   page_num);
	
	gtk_label_set_label (GTK_LABEL (self->priv->tab_label),
			     self->priv->active_page->name);

	return FALSE;
}

static void
show_info_cb (GtkWidget *widget,
	      gpointer user_data)
{
	GscCompletion *self = GSC_COMPLETION (user_data);
	
	g_return_if_fail (GTK_WIDGET_VISIBLE (GTK_WIDGET (self)));
	
	_gsc_completion_update_info_pos (self);
	self->priv->info_visible = TRUE;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->priv->info_button),
				      TRUE);
}

static void
hide_info_cb (GtkWidget *widget,
	      gpointer user_data)
{
	GscCompletion *self = GSC_COMPLETION (user_data);
	
	self->priv->info_visible = FALSE;
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->priv->info_button),
				      FALSE);
}

static gboolean
gsc_completion_display_info_default (GscCompletion *self,
				     GscProposal *proposal)
{
	if (proposal)
	{
		const gchar *info;
		
		info = gsc_proposal_get_info (proposal);
		
		if (info != NULL)
		{
			gsc_info_set_markup (GSC_INFO (self->priv->info_window), info);
		}
		else
		{
			gsc_info_set_markup (GSC_INFO (self->priv->info_window),
					     _("There is no info for the current proposal"));
		}
		return TRUE;
	}
	
	return FALSE;
}

static gboolean
gsc_completion_proposal_selected_default (GscCompletion *self,
					  GscProposal *proposal)
{
	gsc_proposal_apply (proposal, self->priv->view);
	end_completion (self);
	return FALSE;
}

static void
gsc_completion_hide (GtkWidget *widget)
{
	GscCompletion *self = GSC_COMPLETION (widget);
	gboolean info_visible = self->priv->info_visible;
	
	GTK_WIDGET_CLASS (gsc_completion_parent_class)->hide (widget);
	
	//setting to FALSE, hide the info window
	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->priv->info_button),
				      FALSE);
	
	self->priv->info_visible = info_visible;
}

static void
gsc_completion_realize (GtkWidget *widget)
{
	GscCompletion *self = GSC_COMPLETION (widget);
	
	gtk_container_set_border_width (GTK_CONTAINER (self), 1);
	gtk_widget_set_size_request (GTK_WIDGET (self),
				     WINDOW_WIDTH, WINDOW_HEIGHT);
	gtk_window_set_resizable (GTK_WINDOW (self), TRUE);
	
	GTK_WIDGET_CLASS (gsc_completion_parent_class)->realize (widget);
}

static gboolean
gsc_completion_delete_event_cb (GtkWidget *widget,
				GdkEvent  *event,
				gpointer   user_data) 
{
	/*Prevent the alt+F4 keys*/
	return TRUE;
}

static void
free_page (gpointer data,
	   gpointer user_data)
{
	GscCompletionPage *page = (GscCompletionPage *)data;
	
	g_slice_free (GscTree, page->tree);
	g_free (page->name);
	g_slice_free (GscCompletionPage, page);
}

static gboolean
gsc_completion_configure_event (GtkWidget *widget,
			   GdkEventConfigure *event)
{
	gboolean ret;
	GscCompletion *self = GSC_COMPLETION (widget);
	ret = GTK_WIDGET_CLASS (gsc_completion_parent_class)->configure_event (widget, event);
	
	if (GTK_WIDGET_VISIBLE (self->priv->info_window) )
		_gsc_completion_update_info_pos (self);
	
	return ret;
}

static void
gsc_completion_finalize (GObject *object)
{
	GscCompletion *self = GSC_COMPLETION (object);
	
	if (self->priv->active)
		gsc_completion_set_active (self, FALSE);
	
	g_list_foreach (self->priv->pages, (GFunc) free_page, NULL);
	g_list_free (self->priv->pages);
	
	if (self->priv->triggers != NULL)
	{
		g_list_foreach (self->priv->triggers, (GFunc) g_object_unref,
				NULL);
		g_list_free (self->priv->triggers);
	}
	
	if (self->priv->prov_trig != NULL)
	{
		g_list_foreach (self->priv->prov_trig, (GFunc) free_pair,
				NULL);
		g_list_free (self->priv->prov_trig);
	}
	
	completion_control_remove_completion(self->priv->view);
	
	G_OBJECT_CLASS (gsc_completion_parent_class)->finalize (object);
}

static void
gsc_completion_destroy (GtkObject *object)
{
	GscCompletion *self = GSC_COMPLETION (object);
	
	if (!self->priv->destroy_has_run)
	{
		gsc_completion_clear (self);
		self->priv->destroy_has_run = TRUE;
	}
	GTK_OBJECT_CLASS (gsc_completion_parent_class)->destroy (object);
}

static gboolean
view_key_press_event_cb (GtkWidget *view,
			 GdkEventKey *event, 
			 gpointer user_data)
{
	gboolean ret = FALSE;
	GscCompletion *self;
	
	g_return_val_if_fail (GSC_IS_COMPLETION (user_data), FALSE);
	
	self = GSC_COMPLETION (user_data);
	
	if (!GTK_WIDGET_VISIBLE (self))
		return FALSE;
	
	switch (event->keyval)
 	{
		case GDK_Escape:
		{
			gtk_widget_hide (GTK_WIDGET (self));
			ret = TRUE;
			break;
		}
 		case GDK_Down:
		{
			ret = _gsc_completion_select_next (self, 1);
			break;
		}
		case GDK_Page_Down:
		{
			ret = _gsc_completion_select_next (self, 5);
			break;
		}
		case GDK_Up:
		{
			ret = _gsc_completion_select_previous (self, 1);
			if (!ret)
				ret = _gsc_completion_select_first (self);
			break;
		}
		case GDK_Page_Up:
		{
			ret = _gsc_completion_select_previous (self, 5);
			break;
		}
		case GDK_Home:
		{
			ret = _gsc_completion_select_first (self);
			break;
		}
		case GDK_End:
		{
			ret = _gsc_completion_select_last (self);
			break;
		}
		case GDK_Return:
		case GDK_Tab:
		{
			ret = _gsc_completion_select_current_proposal (self);
			gtk_widget_hide (GTK_WIDGET (self));
			break;
		}
		case GDK_Right:
		{
			gsc_completion_page_next (self);
			ret = TRUE;
			break;
		}
		case GDK_Left:
		{
			gsc_completion_page_previous (self);
			ret = TRUE;
			break;
		}
		case GDK_i:
		{
			if ((event->state & GDK_CONTROL_MASK) != 0)
			{
				gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->priv->info_button),
					!gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->priv->info_button)));
				ret = TRUE;
			}
		}
	}
	return ret;
}

static void
set_manage_keys (GscCompletion *self)
{
	if (self->priv->manage_keys && self->priv->signals_ids[TEXT_VIEW_KP] == 0)
	{
		self->priv->signals_ids[TEXT_VIEW_KP] = 
			g_signal_connect (self->priv->view,
					  "key-press-event",
					  G_CALLBACK (view_key_press_event_cb),
					  self);
	}
	
	if (!self->priv->manage_keys && self->priv->signals_ids[TEXT_VIEW_KP] != 0)
	{
		g_signal_handler_disconnect (self->priv->view,
					     self->priv->signals_ids[TEXT_VIEW_KP]);
		self->priv->signals_ids[TEXT_VIEW_KP] = 0;
	}
}

static void
gsc_completion_get_property (GObject    *object,
			     guint       prop_id,
			     GValue     *value,
			     GParamSpec *pspec)
{
	GscCompletion *self;
	
	g_return_if_fail (GSC_IS_COMPLETION (object));
	
	self = GSC_COMPLETION (object);

	switch (prop_id)
	{
		case PROP_MANAGE_KEYS:
			g_value_set_boolean (value, self->priv->manage_keys);
			break;
		case PROP_REMEMBER_INFO_VISIBILITY:
			g_value_set_boolean (value, self->priv->remember_info_visibility);
			break;
		case PROP_SELECT_ON_SHOW:
			g_value_set_boolean (value, self->priv->select_on_show);
			break;
		case PROP_ACTIVE:
			g_value_set_boolean (value, self->priv->active);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gsc_completion_set_property (GObject      *object,
			     guint         prop_id,
			     const GValue *value,
			     GParamSpec   *pspec)
{
	GscCompletion *self;
	g_return_if_fail (GSC_IS_COMPLETION (object));
	
	self = GSC_COMPLETION (object);

	switch (prop_id)
	{
		case PROP_MANAGE_KEYS:
			self->priv->manage_keys = g_value_get_boolean (value);
			set_manage_keys (self);
			break;
		case PROP_REMEMBER_INFO_VISIBILITY:
			self->priv->remember_info_visibility = g_value_get_boolean (value);
			break;
		case PROP_SELECT_ON_SHOW:
			self->priv->select_on_show = g_value_get_boolean (value);
			break;
		case PROP_ACTIVE:
			gsc_completion_set_active (self, g_value_get_boolean (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gsc_completion_class_init (GscCompletionClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	
	g_type_class_add_private (klass, sizeof (GscCompletionPriv));
	
	object_class->get_property = gsc_completion_get_property;
	object_class->set_property = gsc_completion_set_property;
	object_class->finalize = gsc_completion_finalize;
	gtkobject_class->destroy = gsc_completion_destroy;
	widget_class->show = gsc_completion_show_or_update;
	widget_class->hide = gsc_completion_hide;
	widget_class->realize = gsc_completion_realize;
	widget_class->configure_event = gsc_completion_configure_event;
	klass->display_info = gsc_completion_display_info_default;
	klass->proposal_selected = gsc_completion_proposal_selected_default;
	
	/**
	 * GscCompletion:manage-completion-keys:
	 *
	 * %TRUE if this object must control the completion keys pressed into the
	 * #GtkTextView associated (up next proposal, down previous proposal etc.)
	 */
	g_object_class_install_property (object_class,
					 PROP_MANAGE_KEYS,
					 g_param_spec_boolean ("manage-completion-keys",
							      _("Manage Up, Down etc. keys when the completion is visible"),
							      _("Manage Up, Down etc. keys when the completion is visible"),
							      TRUE,
							      G_PARAM_READWRITE));
	/**
	 * GscCompletion:remember-info-visibility:
	 *
	 * %TRUE if the completion must remember the last info state
	 * (visible or hidden)
	 */
	g_object_class_install_property (object_class,
					 PROP_REMEMBER_INFO_VISIBILITY,
					 g_param_spec_boolean ("remember-info-visibility",
							      _("Remember the last info state (visible or hidden)"),
							      _("Remember the last info state (visible or hidden)"),
							      FALSE,
							      G_PARAM_READWRITE));
	/**
	 * GscCompletion:select-on-show:
	 *
	 * %TRUE if the completion must to mark as selected the first proposal
	 * on show
	 */
	g_object_class_install_property (object_class,
					 PROP_SELECT_ON_SHOW,
					 g_param_spec_boolean ("select-on-show",
							      _("Completion mark as selected the first proposal on show"),
							      _("Completion mark as selected the first proposal on show"),
							      FALSE,
							      G_PARAM_READWRITE));
	/**
	 * GscCompletion:active:
	 *
	 * %TRUE if the completion mechanism is active. 
	 */
	g_object_class_install_property (object_class,
					 PROP_ACTIVE,
					 g_param_spec_boolean ("active",
							      _("Set or get if the completion mechanism is active"),
							      _("Set or get if the completion mechanism is active"),
							      FALSE,
							      G_PARAM_READWRITE));
	/**
	 * GscCompletion::proposal-selected:
	 * @completion: The #GscCompletion who emits the signal
	 * @proposal: The selected #GscProposal
	 *
	 * When the user selects a proposal
	 **/
	signals[PROPOSAL_SELECTED] =
		g_signal_new ("proposal-selected",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GscCompletionClass, proposal_selected),
			      g_signal_accumulator_true_handled, 
			      NULL,
			      gtksourcecompletion_marshal_BOOLEAN__POINTER, 
			      G_TYPE_BOOLEAN,
			      1,
			      GTK_TYPE_POINTER);
	
	/**
	 * GscCompletion::display-info:
	 * @completion: The #GscCompletion who emits the signal
	 * @proposal: The #GscProposal the user wants to see the information
	 *
	 * When the user want to see the information of a proposal
	 **/	      
	signals[DISPLAY_INFO] =
		g_signal_new ("display-info",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GscCompletionClass, display_info),
			      g_signal_accumulator_true_handled, 
			      NULL,
			      gtksourcecompletion_marshal_BOOLEAN__POINTER,
			      G_TYPE_BOOLEAN,
			      1,
			      GTK_TYPE_POINTER);
}

static void
gsc_completion_init (GscCompletion *self)
{
	GtkWidget *info_icon;
	GtkWidget *info_button;
	GtkWidget *next_page_icon;
	GtkWidget *prev_page_icon;
	GtkWidget *vbox;

	if (!lib_initialized)
	{
		bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
		bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
		lib_initialized = TRUE;
	}

	self->priv = GSC_COMPLETION_GET_PRIVATE (self);
	self->priv->destroy_has_run = FALSE;
	self->priv->active_trigger = NULL;
	self->priv->active = FALSE;
	self->priv->manage_keys = TRUE;
	self->priv->remember_info_visibility = FALSE;
	self->priv->info_visible = FALSE;
	self->priv->select_on_show = FALSE;

	gtk_window_set_type_hint (GTK_WINDOW (self),
				  GDK_WINDOW_TYPE_HINT_NORMAL);
	gtk_window_set_focus_on_map (GTK_WINDOW (self), FALSE);
	gtk_widget_set_size_request (GTK_WIDGET (self),
				     WINDOW_WIDTH,WINDOW_HEIGHT);
	gtk_window_set_decorated (GTK_WINDOW (self), FALSE);
	
	/*Notebook*/
	self->priv->notebook = gtk_notebook_new ();
	gtk_widget_show (self->priv->notebook);
	g_object_set (G_OBJECT (self->priv->notebook), "can-focus",
		      FALSE, NULL);
	gtk_notebook_set_show_tabs (GTK_NOTEBOOK (self->priv->notebook), FALSE);
	gtk_notebook_set_show_border (GTK_NOTEBOOK (self->priv->notebook),
				      FALSE);
	
	/* Add default page */
	self->priv->active_page = gsc_completion_page_new (self, DEFAULT_PAGE);
	
	/*Icon list*/
	info_icon = gtk_image_new_from_stock (GTK_STOCK_INFO,
					      GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (info_icon);
	gtk_widget_set_tooltip_text (info_icon, _("Show Proposal Info"));
	
	info_button = gtk_toggle_button_new ();
	gtk_widget_show (info_button);
	g_object_set (G_OBJECT (info_button), "can-focus", FALSE, NULL);
	
	self->priv->info_button = info_button;
	gtk_button_set_focus_on_click (GTK_BUTTON (info_button), FALSE);
	gtk_container_add (GTK_CONTAINER (info_button), info_icon);
	g_signal_connect (G_OBJECT (info_button),
			  "toggled",
			  G_CALLBACK (info_toggled_cb),
			  self);

	self->priv->bottom_bar = gtk_hbox_new (FALSE, 1);
	gtk_widget_show (self->priv->bottom_bar);
	gtk_box_pack_start (GTK_BOX (self->priv->bottom_bar), info_button,
			    FALSE, FALSE, 0);

	/*Next/Previous page buttons*/
	
	next_page_icon = gtk_image_new_from_stock (GTK_STOCK_GO_FORWARD,
						   GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (next_page_icon);
	prev_page_icon = gtk_image_new_from_stock (GTK_STOCK_GO_BACK,
						   GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_show (prev_page_icon);
	
	self->priv->next_page_button = gtk_button_new ();
	gtk_widget_show (self->priv->next_page_button);
	g_object_set (G_OBJECT (self->priv->next_page_button),
		      "can-focus", FALSE,
		      "focus-on-click", FALSE,
		      NULL);
	gtk_widget_set_tooltip_text (self->priv->next_page_button,
				     _("Next page"));
	gtk_container_add (GTK_CONTAINER (self->priv->next_page_button),
			   next_page_icon);
	g_signal_connect (G_OBJECT (self->priv->next_page_button),
			  "clicked",
			  G_CALLBACK (next_page_cb),
			  self);
	
	self->priv->prev_page_button = gtk_button_new ();
	gtk_widget_show (self->priv->prev_page_button);
	g_object_set (G_OBJECT (self->priv->prev_page_button),
		      "can-focus", FALSE,
		      "focus-on-click", FALSE,
		      NULL);
	gtk_widget_set_tooltip_text (self->priv->prev_page_button,
				     _("Previous page"));
	gtk_container_add (GTK_CONTAINER (self->priv->prev_page_button),
			   prev_page_icon);
	g_signal_connect (G_OBJECT (self->priv->prev_page_button),
			  "clicked",
			  G_CALLBACK (prev_page_cb),
			  self);

	gtk_box_pack_end (GTK_BOX (self->priv->bottom_bar),
			  self->priv->next_page_button,
			  FALSE, FALSE, 1);
	gtk_box_pack_end (GTK_BOX (self->priv->bottom_bar),
			  self->priv->prev_page_button,
			  FALSE, FALSE, 1);
	/*Page label*/
	self->priv->tab_label = gtk_label_new (DEFAULT_PAGE);
	gtk_widget_show (self->priv->tab_label);
	gtk_box_pack_end (GTK_BOX (self->priv->bottom_bar),
			  self->priv->tab_label,
			  FALSE, TRUE, 10);
	
	/*Main vbox*/
	vbox = gtk_vbox_new (FALSE, 1);
	gtk_widget_show (vbox);
	gtk_box_pack_start (GTK_BOX (vbox), self->priv->notebook,
			    TRUE, TRUE, 0);

	gtk_box_pack_end (GTK_BOX (vbox), self->priv->bottom_bar,
			  FALSE, FALSE, 0);

	gtk_container_add (GTK_CONTAINER (self), vbox);

	/*Info window*/
	self->priv->info_window = GTK_WIDGET (gsc_info_new ());

	/* Connect signals */
			
	g_signal_connect (self->priv->notebook, 
			  "switch-page",
			  G_CALLBACK (switch_page_cb),
			  self);
			
	g_signal_connect (self,
			  "delete-event",
			  G_CALLBACK (gsc_completion_delete_event_cb),
			  NULL);

	g_signal_connect (self->priv->info_window,
			  "show-info",
			  G_CALLBACK (show_info_cb),
			  self);
			  
	g_signal_connect (self->priv->info_window,
			  "hide",
			  G_CALLBACK (hide_info_cb),
			  self);

	self->priv->triggers = NULL;
	self->priv->prov_trig = NULL;
}

static void
view_destroy_event_cb (GtkWidget *widget,
		       gpointer user_data)
{
	GscCompletion *self = GSC_COMPLETION (user_data);
	
	g_object_unref (self);
}

static gboolean
view_focus_out_event_cb (GtkWidget *widget,
			 GdkEventFocus *event,
			 gpointer user_data)
{
	GscCompletion *self = GSC_COMPLETION (user_data);
	
	if (GTK_WIDGET_VISIBLE (self)
	    && !GTK_WIDGET_HAS_FOCUS (self))
		end_completion (self);
	
	return FALSE;
}

static gboolean
view_button_press_event_cb (GtkWidget *widget,
			    GdkEventButton *event,
			    gpointer user_data)
{
	GscCompletion *self = GSC_COMPLETION (user_data);
	
	if (GTK_WIDGET_VISIBLE (self))
		end_completion (self);

	return FALSE;
}

static void
gsc_completion_add_data (GscCompletion *self,
			 GscProposal* data)
{
	GscTree *tree;
	
	g_return_if_fail (GSC_IS_COMPLETION (self));
	
	tree = get_tree_by_name (self, gsc_proposal_get_page_name (data));
	
	gsc_tree_add_data (tree, data);
}

void
_gsc_completion_update_info_pos (GscCompletion *self)
{
	GscProposal *proposal = NULL;
	gint y, x, sw, sh;
	
	if (gsc_tree_get_selected_proposal (get_current_tree (self), &proposal))
	{
		gboolean ret = TRUE;
		g_signal_emit (self, signals[DISPLAY_INFO], 0, proposal, &ret);
	}
	
	gtk_window_get_position (GTK_WINDOW (self), &x, &y);
	sw = gdk_screen_width ();
	sh = gdk_screen_height ();
	x += WINDOW_WIDTH;

	if (x + WINDOW_WIDTH >= sw)
	{
		x -= (WINDOW_WIDTH * 2);
	}
	gtk_window_move (GTK_WINDOW (self->priv->info_window), x, y);
	gtk_window_set_transient_for (GTK_WINDOW (self->priv->info_window),
				      gtk_window_get_transient_for (GTK_WINDOW (self)));
}

gboolean
_gsc_completion_select_first (GscCompletion *self)
{
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);
	
	return gsc_tree_select_first (get_current_tree (self));
}

gboolean 
_gsc_completion_select_last (GscCompletion *self)
{
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);

	return gsc_tree_select_last (get_current_tree (self));
}

gboolean
_gsc_completion_select_previous (GscCompletion *self, 
			   gint rows)
{
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);

	return gsc_tree_select_previous (get_current_tree (self), rows);
}

gboolean
_gsc_completion_select_next (GscCompletion *self, 
		       gint rows)
{
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);

	return gsc_tree_select_next (get_current_tree (self), rows);
}

gboolean
_gsc_completion_select_current_proposal (GscCompletion *self)
{
	gboolean selected = TRUE;
	GscProposal *prop = NULL;
	
	if (gsc_tree_get_selected_proposal (get_current_tree (self), &prop))
	{
		g_signal_emit (G_OBJECT (self), signals[PROPOSAL_SELECTED],
			       0, prop, &selected);
		selected = TRUE;
	}
	else
	{
		selected = FALSE;
	}
	
	return selected;
}

/**
 * gsc_completion_new:
 *
 * Returns The new #GscCompletion
 */
GtkWidget*
gsc_completion_new (GtkTextView *view)
{
	GscCompletion *self = GSC_COMPLETION (g_object_new (GSC_TYPE_COMPLETION,
							    "type", GTK_WINDOW_POPUP,
							    NULL));
	self->priv->view = view;
	
	completion_control_add_completion (view,self);
	
	return GTK_WIDGET (self);
}

/**
 * gsc_completion_register_trigger:
 * @self: The #GscCompletion
 * @trigger: The trigger to register
 *
 * This function register a completion trigger. If the completion is actived
 * then this method activate the trigger. This function reference the trigger
 * object
 *
 * Returns: %TRUE if the trigger has been registered
 */
gboolean
gsc_completion_register_trigger (GscCompletion *self,
				 GscTrigger *trigger)
{
	GList *l;
	
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);
	
	l = g_list_find (self->priv->triggers, trigger);
	/*Only register the trigger if it has not been registered yet*/
	if (l == NULL)
	{
	
		self->priv->triggers = g_list_append (self->priv->triggers,
						      trigger);
		g_object_ref (trigger);
		
		if (self->priv->active)
		{
			gsc_trigger_activate (trigger);
		}
		return TRUE;
	}
	
	return FALSE;
}

/**
 * gsc_completion_register_provider:
 * @self: the #GscCompletion
 * @provider: The #GscProvider.
 * @trigger_name: The trigger name what you want to register this provider
 *
 * This function register the provider into the completion and reference it. When 
 * an event is raised, completion call to the provider to get the data. When the user
 * select a proposal, it call the provider to tell it this action and the provider do
 * that it want (normally inserts some text)
 * 
 * Returns: %TRUE if it was registered or %FALSE if not (because it has been already registered,
 * or the trigger doesn't exists)
 *
 **/
gboolean
gsc_completion_register_provider (GscCompletion *self,
				  GscProvider *provider,
				  GscTrigger *trigger)
{
	PTPair *ptp;
	GList *l;
	
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);
	g_return_val_if_fail (GSC_IS_PROVIDER (provider), FALSE);
	g_return_val_if_fail (GSC_IS_TRIGGER (trigger), FALSE);
	g_return_val_if_fail (g_list_find (self->priv->triggers, trigger) != NULL,
			      FALSE);

	/*Check if the provider-trigger pair has been registered*/
	for (l = self->priv->prov_trig; l != NULL; l = g_list_next (l))
	{
		ptp = (PTPair *)l->data;
		if (ptp->trigger == trigger && ptp->provider == provider)
			return FALSE;
	}

	ptp = g_slice_new (PTPair);
	ptp->provider = provider;
	ptp->trigger = trigger;
	self->priv->prov_trig = g_list_append (self->priv->prov_trig, ptp);
	
	g_object_ref (provider);

	return TRUE;
}

/**
 * gsc_completion_unregister_trigger:
 * @self: The #GscCompletion
 * @trigger: The trigger to unregister
 *
 * This function unregister a completion trigger. If the completion is actived
 * then this method deactivate the trigger. This function unreference the trigger
 * object
 *
 * Returns: %TRUE if the trigger has been unregistered or %FALSE if not (because
 * the trigger has not been registered)
 */
gboolean
gsc_completion_unregister_trigger (GscCompletion *self,
				   GscTrigger *trigger)
{
	GList *l;
	
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);
	g_return_val_if_fail (g_list_find (self->priv->triggers, trigger) != NULL,
			      FALSE);
	
	self->priv->triggers = g_list_remove (self->priv->triggers, trigger);
	
	if (self->priv->active)
	{
		gsc_trigger_deactivate (trigger);
	}
	
	for (l = self->priv->prov_trig; l != NULL; l = g_list_next (l))
	{
		PTPair *ptp = (PTPair *)l->data;
		
		if (ptp->trigger == trigger)
		{
			free_pair (ptp);
			self->priv->prov_trig = g_list_remove_link (self->priv->prov_trig, l);
			//TODO we need start the list again
		}
	}

	g_object_unref (trigger);
	return TRUE;
}

/**
 * gsc_completion_unregister_provider:
 * @self: the #GscCompletion
 * @provider: The #GscProvider.
 * @trigger: The trigger what you want to unregister this provider
 *
 * This function unregister the provider.
 * 
 * Returns TRUE if it was unregistered or FALSE if not (because it doesn't exists,
 * or the trigger don't exists)
 * 
 **/
gboolean
gsc_completion_unregister_provider (GscCompletion *self,
				    GscProvider *provider,
				    GscTrigger *trigger)
{
	GList *l;
	gboolean ret = FALSE;
	
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);
	g_return_val_if_fail (g_list_find (self->priv->triggers, trigger) != NULL,
			      FALSE);
	
	for (l = self->priv->prov_trig; l != NULL; l = g_list_next (l))
	{
		PTPair *ptp = (PTPair *)l->data;
		
		if (ptp->provider == provider)
		{
			free_pair (ptp);
			self->priv->prov_trig = g_list_remove_link (self->priv->prov_trig, l);
			//TODO we need start the list again
			ret = TRUE;
		}
	}
		
	return ret;
}

/**
 * gsc_completion_get_active_trigger:
 * @self: The #GscCompletion
 *
 * This function return the active trigger. The active trigger is the last
 * trigger raised if the completion is active. If the completion is not visible then
 * there is no an active trigger.
 *
 * Returns: The trigger or NULL if completion is not active
 */
GscTrigger*
gsc_completion_get_active_trigger (GscCompletion *self)
{
	g_return_val_if_fail (GSC_IS_COMPLETION (self), NULL);

	return self->priv->active_trigger;
}


/**
 * gsc_completion_get_view:
 * @self: The #GscCompletion
 *
 * Returns: The view associated with this completion.
 */
GtkTextView*
gsc_completion_get_view (GscCompletion *self)
{
	g_return_val_if_fail (GSC_COMPLETION (self), NULL);
	
	return self->priv->view;
}

/**
 * gsc_completion_trigger_event:
 * @self: the #GscCompletion
 * @trigger: The trigger who trigger the event
 *
 * Calling this function, the completion call to all providers to get data and, if 
 * they return data, it shows the completion to the user. 
 *
 * Returns: %TRUE if the event has been triggered, %FALSE if not
 * 
 **/
gboolean 
gsc_completion_trigger_event (GscCompletion *self, 
			      GscTrigger *trigger)
{
	GList *l;
	GList *data_list;
	GList *final_list = NULL;
	GscProposal *last_proposal = NULL;
	gint x, y;

	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);
	g_return_val_if_fail (GSC_IS_TRIGGER (trigger), FALSE);
	g_return_val_if_fail (self->priv->active, FALSE);
	
	/*
	 * If the completion is visble and there is a trigger active, you cannot
	 * raise a different trigger until the current trigger finish o_O
	 */
	if (GTK_WIDGET_VISIBLE (self)
	    && self->priv->active_trigger != trigger)
	{
		return FALSE;
	}
	
	end_completion (self);
	gsc_completion_clear (self);
	
	for (l = self->priv->prov_trig; l != NULL; l = g_list_next (l))
	{
		PTPair *ptp = (PTPair *)l->data;
		
		if (ptp->trigger == trigger)
		{
			data_list = gsc_provider_get_proposals (ptp->provider, 
								trigger);
			if (data_list != NULL)
			{
				final_list = g_list_concat (final_list,
							    data_list);
			}
		}
	}
	
	if (final_list == NULL)
	{
		if (GTK_WIDGET_VISIBLE (self))
			end_completion (self);
		return FALSE;
	}
	
	data_list = final_list;
	/* Insert the data into the model */
	do
	{
		last_proposal = GSC_PROPOSAL (data_list->data);
		
		gsc_completion_add_data (self,
					 last_proposal);
	} while ((data_list = g_list_next (data_list)) != NULL);
	
	g_list_free (final_list);
	
	if (!GTK_WIDGET_HAS_FOCUS (self->priv->view))
		return FALSE;
	
	/*
	 *FIXME Do it supports only cursor position? We can 
	 *add a new "position-type": cursor, center_screen,
	 *center_window, custom etc.
	 */
	gsc_get_window_position_in_cursor (GTK_WINDOW (self),
					   self->priv->view,
					   &x, &y, NULL);

	gtk_window_move (GTK_WINDOW (self),
			 x, y);
	
	self->priv->active_page = (GscCompletionPage *)self->priv->pages->data;
	
	/*
	We must call to gtk_widget_show and not to gsc_completion_show_or_update
	because if we don't call to gtk_widget_show, the show signal is not emitted
	*/
	gtk_widget_show (GTK_WIDGET (self));

	gtk_widget_grab_focus (GTK_WIDGET (self->priv->view));

	self->priv->active_trigger = trigger;
	
	if (self->priv->select_on_show)
		_gsc_completion_select_first (self);

	return TRUE;
}

/**
 * gsc_completion_finish_completion:
 * @self: The #GscCompletion
 *
 * This function finish the completion if it is active (visible).
 */
void
gsc_completion_finish_completion (GscCompletion *self)
{
	g_return_if_fail (GSC_IS_COMPLETION (self));

	if (GTK_WIDGET_VISIBLE (self))
	{
		end_completion (self);
	}
}

/**
 * gsc_completion_filter_proposals:
 * @self: the #GscCompletion
 * @func: function to filter the proposals visibility
 * @user_data: user data to pass to func
 *
 * This function call to @func for all proposal of all pages. @func must
 * return %TRUE if the proposal is visible or %FALSE if the completion must to 
 * hide the proposal.
 * 
 **/
void
gsc_completion_filter_proposals (GscCompletion *self,
				 GscCompletionFilterFunc func,
				 gpointer user_data)
{
	GList *l;
	
	g_return_if_fail (GSC_IS_COMPLETION (self));
	g_return_if_fail (func);
	
	if (!GTK_WIDGET_VISIBLE (self))
		return;
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscCompletionPage *page = (GscCompletionPage *)l->data;
		if (gsc_tree_get_num_proposals (page->tree) > 0)
		{
			gsc_tree_filter_visible (page->tree,
						 (GscCompletionFilterFunc) func,
						 user_data);
		}
	}

	if (!update_pages_visibility (self))
	{
		end_completion (self);
	}
}

/**
 * gsc_completion_set_active:
 * @self: The #GscCompletion
 * @active: %TRUE if you want to activate the completion mechanism.
 *
 * This function activate/deactivate the completion mechanism. The completion
 * connects/disconnect all signals and activate/deactivate all registered triggers.
 */
void
gsc_completion_set_active (GscCompletion *self,
			   gboolean active)
{
	GList *l;
	GscTrigger *trigger;
	gint i;
	
	g_return_if_fail (GSC_IS_COMPLETION (self));
	
	if (active)
	{
		if (self->priv->active)
			return;

		self->priv->signals_ids[TEXT_VIEW_DESTROY] = 
				g_signal_connect (self->priv->view,
						  "destroy",
						  G_CALLBACK (view_destroy_event_cb),
						  self);
		self->priv->signals_ids[TEXT_VIEW_FOCUS_OUT] = 
				g_signal_connect (self->priv->view,
						  "focus-out-event",
						  G_CALLBACK (view_focus_out_event_cb),
						  self);
		self->priv->signals_ids[TEXT_VIEW_BUTTON_PRESS] = 
				g_signal_connect (self->priv->view,
						  "button-press-event",
						  G_CALLBACK (view_button_press_event_cb),
						  self);
	
		set_manage_keys (self);
	
		/* We activate the triggers */
		for (l = self->priv->triggers; l != NULL; l = g_list_next (l))
		{
			trigger =  GSC_TRIGGER (l->data);
		
			gsc_trigger_activate (trigger);
		}
	}
	else
	{
		for (i = 0; i < LAST_EXTERNAL_SIGNAL; i++)
		{
			if (g_signal_handler_is_connected (self->priv->view, 
							   self->priv->signals_ids[i]))
			{
				g_signal_handler_disconnect (self->priv->view,
							     self->priv->signals_ids[i]);
			}
			self->priv->signals_ids[i] = 0;
		}
	
		for (l = self->priv->triggers; l != NULL; l = g_list_next (l))
		{
			trigger =  GSC_TRIGGER (l->data);
		
			gsc_trigger_deactivate (trigger);
		}
	}
	self->priv->active = active;
}

/**
 * gsc_completion_get_active:
 * @self: The #GscCompletion
 *
 * Returns: %TRUE if the completion mechanism is active, %FALSE if not.
 */
gboolean
gsc_completion_get_active (GscCompletion *self)
{
	return self->priv->active;
}

/**
 * gsc_completion_get_bottom_bar:
 * @self: The #GscCompletion
 *
 * The bottom bar is the widgetwith the info button, the page name etc.
 *
 * Returns: The bottom bar widget (it is a #GtkBox by now)
 */
GtkWidget*
gsc_completion_get_bottom_bar (GscCompletion *self)
{
	return self->priv->bottom_bar;
}

/**
 * gsc_completion_get_info_widget:
 * @self: The #GscCompletion
 *
 * The info widget is the window where the completion shows the
 * proposal info or help. DO NOT USE IT (only to connect to some signal
 * or set the content in a custom widget).
 *
 * Returns: The internal GscInfo widget.
 */
GscInfo*
gsc_completion_get_info_widget (GscCompletion *self)
{
	return GSC_INFO (self->priv->info_window);
}

/**
 * gsc_completion_get_trigger:
 * @self: The #GscCompletion
 * @trigger_name: The trigger name to find
 *
 * Returns: The #GscTrigger registered with @trigger_name, %NULL if it cannot
 * be found
 */
GscTrigger*
gsc_completion_get_trigger (GscCompletion *self,
			    const gchar *trigger_name)
{

	GList *l;
	GscTrigger *trigger;
	g_return_val_if_fail (GSC_IS_COMPLETION (self), NULL);

	for (l = self->priv->triggers; l != NULL; l = g_list_next (l))
	{
		trigger =  GSC_TRIGGER (l->data);
		
		if (g_strcmp0 (gsc_trigger_get_name (trigger), trigger_name) == 0)
			return trigger;
	}
	
	return NULL;
}

/**
 * gsc_completion_get_provider:
 * @self: The #GscCompletion
 * @prov_name: The provider name to find
 *
 * Returns: The #GscProvider registered with @prov_name
 */
GscProvider*
gsc_completion_get_provider (GscCompletion *self,
			     const gchar *prov_name)
{

	GList *l;
	g_return_val_if_fail (GSC_IS_COMPLETION (self), NULL);

	for (l = self->priv->prov_trig; l != NULL; l = g_list_next (l))
	{
		PTPair *ptp = (PTPair *)l->data;
		if (g_strcmp0 (gsc_provider_get_name (ptp->provider), prov_name) == 0)
			return ptp->provider;
	}
		
	return NULL;
}

/**
 * gsc_completion_get_from_view:
 * @view: The #GtkTextView associated with a #GscCompletion
 *
 * Returns: The #GscCompletion associated with a @view or %NULL.
 */
GscCompletion*
gsc_completion_get_from_view(GtkTextView *view)
{
        return completion_control_get_completion(view);
}

/**
 * gsc_completion_get_page_pos:
 * @self: The #GscCompletion
 * @page_name: The page name to search
 *
 * Search the page position with the given name
 *
 * Returns: the page position or -1 if it is not found
 */
gint
gsc_completion_get_page_pos (GscCompletion *self,
			     const gchar *page_name)
{
	GList *l;
	gint pos = 0;
	
	g_return_val_if_fail (GSC_IS_COMPLETION (self), -1);
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscCompletionPage *page = (GscCompletionPage *)l->data;
		if (g_strcmp0 (page_name, page->name) == 0)
			return pos;
		
		pos++;
	}
	
	return -1;
}

/**
 * gsc_completion_get_n_pages:
 * @self: The #GscCompletion
 * 
 * Returns: The number of pages
 */
gint
gsc_completion_get_n_pages (GscCompletion *self)
{
	g_return_val_if_fail (GSC_IS_COMPLETION (self), -1);
	
	return g_list_length (self->priv->pages);
}


/**
 * gsc_completion_set_page_pos:
 * @self: The #GscCompletion
 * @page_name: The page name you want to set the position (stating by 1 because 
 * 0 is the default page)
 * @position: The new position of the page. If this is negative, or is larger
 * than the number of elements in the list, the new element is added on to
 * the end of the list
 * 
 * Returns: %TRUE if the position has been set.
 */
gboolean
gsc_completion_set_page_pos (GscCompletion *self,
			     const gchar *page_name,
			     gint position)
{
	GList *l;
	gboolean res = FALSE;
	GscCompletionPage *page;
	
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		page = (GscCompletionPage *)l->data;
		if (g_strcmp0 (page_name, page->name) == 0)
		{
			break;
		}
		page = NULL;
	}
	
	if (page == NULL)
	{
		page = gsc_completion_page_new (self, page_name);
	}
	
	self->priv->pages = g_list_remove (self->priv->pages, page);
	self->priv->pages = g_list_insert (self->priv->pages, page, position);
	gtk_notebook_reorder_child (GTK_NOTEBOOK (self->priv->notebook),
				    GTK_WIDGET (page->tree->scroll),
				    position);
	if (GTK_WIDGET_VISIBLE (self))
		update_pages_visibility (self);
	res = TRUE;
	
	return res;
}


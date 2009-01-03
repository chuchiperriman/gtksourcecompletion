/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-popup.c
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

#include <gdk/gdkkeysyms.h> 
#include "gsc-popup.h"
#include "gsc-i18n.h"
#include "gsc-utils.h"
#include "gsc-info.h"
#include "gsc-marshal.h"
#include <string.h>

#define WINDOW_WIDTH 350
#define WINDOW_HEIGHT 200

#define GSC_POPUP_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object),\
					 GSC_TYPE_POPUP,                    \
					 GscPopupPriv))

/* Signals */
enum
{
	ITEM_SELECTED,
	DISPLAY_INFO,
	LAST_SIGNAL
};

static guint popup_signals[LAST_SIGNAL] = { 0 };

typedef struct _GscPopupPage
{
	gchar *name;
	GtkWidget *view;
} GscPopupPage;

struct _GscPopupPriv
{
	GtkWidget *info_window;
	GtkWidget *info_button;
	GtkWidget *notebook;
	GtkWidget *tab_label;
	GtkWidget *next_page_button;
	GtkWidget *prev_page_button;
	GtkWidget *bottom_bar;
	
	GList *pages;
	GscPopupPage *active_page;

	gboolean destroy_has_run;
};

G_DEFINE_TYPE(GscPopup, gsc_popup, GTK_TYPE_WINDOW);

static GscTree*
get_current_tree (GscPopup *self)
{
	return GSC_TREE (self->priv->active_page->view);
}

static void
show_completion_info (GscPopup *self)
{
	GscProposal *proposal = NULL;
	gint y, x, sw, sh;
	
	if (gsc_popup_get_selected_proposal (self, &proposal))
	{
		gboolean ret = TRUE;
		g_signal_emit_by_name (self, "display-info", proposal, &ret);
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
	gtk_widget_show (self->priv->info_window);
}

static void
row_activated_cb (GtkTreeView *tree_view,
		  GtkTreePath *path,
		  GtkTreeViewColumn *column,
		  gpointer user_data)
{
	GtkTreeModel *model;
	GscProposal *proposal;
	
	model = gtk_tree_view_get_model (tree_view);
	
	gsc_tree_get_selected_proposal (GSC_TREE (tree_view), &proposal);

	g_signal_emit (G_OBJECT (user_data), popup_signals[ITEM_SELECTED],
		       0, proposal);
}

static void 
selection_changed_cd (GtkTreeSelection *selection, 
		      GscPopup *self)
{
	if (GTK_WIDGET_VISIBLE (self->priv->info_window))
	{
		show_completion_info (self);
	}
}

static GscPopupPage *
gsc_popup_page_new (GscPopup *self,
		    const gchar *tree_name)
{
	/*Creates the new trees*/
	GscPopupPage *page;
	GtkWidget *label;
	GtkWidget *sw;
	GtkTreeSelection *selection;
	
	page = g_slice_new (GscPopupPage);
	
	page->name = g_strdup (tree_name);
	
	page->view = gsc_tree_new ();
	gtk_widget_show (page->view);
	g_object_set (G_OBJECT (page->view), "can-focus",
		      FALSE, NULL);
	self->priv->pages = g_list_append (self->priv->pages,
					   page);
	
	sw = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_show (sw);
	
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (sw),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	
	gtk_container_add (GTK_CONTAINER (sw), page->view);
	
	label = gtk_label_new (tree_name);
	
	gtk_notebook_append_page (GTK_NOTEBOOK (self->priv->notebook),
				  sw, label);
	
	g_signal_connect (page->view,
			  "row-activated",
			  G_CALLBACK (row_activated_cb),
			  self);
	
	selection = gtk_tree_view_get_selection (GTK_TREE_VIEW (page->view));
	g_signal_connect (selection,
			  "changed",
			  G_CALLBACK (selection_changed_cd),
			  self);
	
	return page;
}

static GscTree*
get_tree_by_name (GscPopup *self,
		  const gchar* tree_name)
{
	GscPopupPage *page;
	GList *l;
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		page = (GscPopupPage *)l->data;
	
		if (strcmp (page->name, tree_name) == 0)
			return GSC_TREE (page->view);
	}
	
	page = gsc_popup_page_new (self, tree_name);

	return GSC_TREE (page->view);
}

static void
info_toggled_cb (GtkToggleButton *widget,
		 gpointer user_data)
{
	GscPopup *self = GSC_POPUP (user_data);
	
	if (gtk_toggle_button_get_active (widget))
	{
		show_completion_info (self);
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
	GscPopup *self = GSC_POPUP (user_data);
	
	gsc_popup_page_next (self);
}

static void
prev_page_cb (GtkWidget *widget,
	      gpointer user_data)
{
	GscPopup *self = GSC_POPUP (user_data);
	
	gsc_popup_page_previous (self);
}

static gboolean
switch_page_cb (GtkNotebook *notebook, 
		GtkNotebookPage *n_page,
		gint page_num, 
		gpointer user_data)
{
	GscPopup *self = GSC_POPUP (user_data);
	
	/* Update the active page */
	self->priv->active_page = (GscPopupPage *)g_list_nth_data (self->priv->pages,
								   page_num);
	
	gtk_label_set_label (GTK_LABEL (self->priv->tab_label),
			     self->priv->active_page->name);

	return FALSE;
}

static gboolean
update_pages_visibility (GscPopup *self)
{
	GList *l;
	gboolean first_set = FALSE;
	guint num_pages_with_data = 0;
	gint i = 0;
	GtkAdjustment *ad;
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscPopupPage *page = (GscPopupPage *)l->data;
		
		if (gsc_tree_get_num_proposals (GSC_TREE (page->view)) > 0)
		{
			/*Selects the first page with data*/
			if (!first_set)
			{
				gtk_notebook_set_current_page (GTK_NOTEBOOK (self->priv->notebook),
							       i);
				ad = gtk_tree_view_get_vadjustment (GTK_TREE_VIEW (page->view));
				gtk_adjustment_set_value (ad, 0);
				ad = gtk_tree_view_get_hadjustment (GTK_TREE_VIEW (page->view));
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

static gboolean
gsc_popup_display_info_default (GscPopup *self,
				GscProposal *proposal)
{
	if (proposal)
	{
		const gchar *info;
		
		info = gsc_proposal_get_info (proposal);
		gsc_popup_set_current_info (self, info);
	}
	return FALSE;
}

static void
gsc_popup_hide (GtkWidget *widget)
{
	GscPopup *self = GSC_POPUP (widget);
	
	GTK_WIDGET_CLASS (gsc_popup_parent_class)->hide (widget);
	gtk_widget_hide (self->priv->info_window);
}

static void
gsc_popup_realize (GtkWidget *widget)
{
	GscPopup *self = GSC_POPUP (widget);
	
	gtk_container_set_border_width (GTK_CONTAINER (self), 1);
	gtk_widget_set_size_request (GTK_WIDGET (self),
				     WINDOW_WIDTH, WINDOW_HEIGHT);
	gtk_window_set_resizable (GTK_WINDOW (self), TRUE);
	
	GTK_WIDGET_CLASS (gsc_popup_parent_class)->realize (widget);
}

static gboolean
gsc_popup_delete_event_cb (GtkWidget *widget,
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
	GscPopupPage *page = (GscPopupPage *)data;
	
	g_free (page->name);
	g_slice_free (GscPopupPage, page);
}

static void
gsc_popup_finalize (GObject *object)
{
	GscPopup *self = GSC_POPUP (object);
	
	g_list_foreach (self->priv->pages, (GFunc) free_page, NULL);
	g_list_free (self->priv->pages);
	
	g_debug ("popup finalize");
	
	G_OBJECT_CLASS (gsc_popup_parent_class)->finalize (object);
}

static void
gsc_popup_destroy (GtkObject *object)
{
	GscPopup *self = GSC_POPUP (object);
	
	if (!self->priv->destroy_has_run)
	{
		gsc_popup_clear (self);
		self->priv->destroy_has_run = TRUE;
	}
	GTK_OBJECT_CLASS (gsc_popup_parent_class)->destroy (object);
}

static void
gsc_popup_class_init (GscPopupClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	
	g_type_class_add_private (klass, sizeof (GscPopupPriv));
	
	object_class->finalize = gsc_popup_finalize;
	gtkobject_class->destroy = gsc_popup_destroy;
	widget_class->show = gsc_popup_show_or_update;
	widget_class->hide = gsc_popup_hide;
	widget_class->realize = gsc_popup_realize;
	klass->display_info = gsc_popup_display_info_default;
	
	/**
	 * GscPopup::proposal-selected:
	 * @popup: The #GscPopup who emits the signal
	 * @proposal: The selected #GscProposal
	 *
	 * When the user selects a proposal
	 **/
	popup_signals[ITEM_SELECTED] =
		g_signal_new ("proposal-selected",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GscPopupClass, proposal_selected),
			      NULL, 
			      NULL,
			      g_cclosure_marshal_VOID__POINTER, 
			      G_TYPE_NONE,
			      1,
			      GTK_TYPE_POINTER);
	
	/**
	 * GscPopup::display-info:
	 * @popup: The #GscPopup who emits the signal
	 * @proposal: The #GscProposal the user wants to see the information
	 *
	 * When the user want to see the information of a proposal
	 **/	      
	popup_signals[DISPLAY_INFO] =
		g_signal_new ("display-info",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_LAST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GscPopupClass, display_info),
			      g_signal_accumulator_true_handled, 
			      NULL,
			      gtksourcecompletion_marshal_BOOLEAN__POINTER,
			      G_TYPE_BOOLEAN,
			      1,
			      GTK_TYPE_POINTER);
}

static void
gsc_popup_init (GscPopup *self)
{
	GtkWidget *info_icon;
	GtkWidget *info_button;
	GtkWidget *next_page_icon;
	GtkWidget *prev_page_icon;
	GtkWidget *vbox;

	self->priv = GSC_POPUP_GET_PRIVATE (self);
	self->priv->destroy_has_run = FALSE;

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
	self->priv->active_page = gsc_popup_page_new (self, DEFAULT_PAGE);
	
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
			  G_CALLBACK (gsc_popup_delete_event_cb),
			  NULL);
}

/**
 * gsc_popup_new:
 *
 * Returns The new #GscPopup
 */
GtkWidget*
gsc_popup_new ()
{
	GscPopup *self = GSC_POPUP (g_object_new (GSC_TYPE_POPUP,
				    "type", GTK_WINDOW_POPUP,
				    NULL));
	return GTK_WIDGET (self);
}

/**
 * gsc_popup_add_data:
 * @self: The #GscPopup
 * @data: The #GscProposal to add.
 *
 * The popup frees the proposal when it will be cleaned.
 *
 */
void
gsc_popup_add_data (GscPopup *self,
		    GscProposal* data)
{
	GscTree *tree;
	
	g_return_if_fail (GSC_IS_POPUP (self));
	
	tree = get_tree_by_name (self, gsc_proposal_get_page_name (data));
	
	gsc_tree_add_data (tree, data);
}

/**
 * gsc_popup_clear:
 * @self: The #GscPopup
 *
 * Clear all proposals in all pages. It frees all completion proposals.
 *
 */
void
gsc_popup_clear (GscPopup *self)
{
	GList *l;
	
	g_return_if_fail (GSC_IS_POPUP (self));
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscPopupPage *page = (GscPopupPage *)l->data;
		
		gsc_tree_clear (GSC_TREE (page->view));
	}
}

/**
 * gsc_popup_select_first:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_first
 *
 * Returns
 */
gboolean
gsc_popup_select_first (GscPopup *self)
{
	g_return_val_if_fail (GSC_IS_POPUP (self), FALSE);
	
	return gsc_tree_select_first (get_current_tree (self));
}

/**
 * gsc_popup_select_last:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_last
 *
 * Returns
 */
gboolean 
gsc_popup_select_last (GscPopup *self)
{
	g_return_val_if_fail (GSC_IS_POPUP (self), FALSE);

	return gsc_tree_select_last (get_current_tree (self));
}

/**
 * gsc_popup_select_previous:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_previous
 *
 * Returns
 */
gboolean
gsc_popup_select_previous (GscPopup *self, 
			   gint rows)
{
	g_return_val_if_fail (GSC_IS_POPUP (self), FALSE);

	return gsc_tree_select_previous (get_current_tree (self), rows);
}

/**
 * gsc_popup_select_next:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_next
 *
 * Returns
 */
gboolean
gsc_popup_select_next (GscPopup *self, 
		       gint rows)
{
	g_return_val_if_fail (GSC_IS_POPUP (self), FALSE);

	return gsc_tree_select_next (get_current_tree (self), rows);
}

/**
 * gsc_popup_get_selected_proposal:
 * @self: The #GscPopup
 *
 * See #gsc_tree_get_selected_proposal. Not free the proposal!
 *
 * Returns
 */
gboolean
gsc_popup_get_selected_proposal (GscPopup *self,
				 GscProposal **proposal)
{
	g_return_val_if_fail (GSC_IS_POPUP (self), FALSE);

	return gsc_tree_get_selected_proposal (get_current_tree (self),
					       proposal);
}

gboolean
gsc_popup_select_current_proposal (GscPopup *self)
{
	gboolean selected = FALSE;
	GscProposal *prop = NULL;
	if (gsc_popup_get_selected_proposal (self, &prop))
	{
		g_signal_emit (G_OBJECT (self), popup_signals[ITEM_SELECTED],
			       0, prop);
		selected = TRUE;
	}
	return selected;
}

/**
 * gsc_popup_has_proposals:
 * @self: The #GscPopup
 *
 * Returns TRUE if the popup has almost one element.
 */
gboolean
gsc_popup_has_proposals (GscPopup *self)
{
	GList *l;
	
	g_return_val_if_fail (GSC_IS_POPUP (self), FALSE);
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscPopupPage *page = (GscPopupPage *)l->data;
		
		if (gsc_tree_get_num_proposals (GSC_TREE (page->view)) > 0)
			return TRUE;
	}
	return FALSE;
}

/**
 * gsc_popup_toggle_proposal_info:
 * @self: The #GscPopup
 *
 * This toggle the state of the info dialog. If the info is visible
 * then it hide the info dialog. If the dialog is hidden then it 
 * shows the info dialog.
 *
 */
void
gsc_popup_toggle_proposal_info (GscPopup *self)
{
	g_return_if_fail (GSC_IS_POPUP (self));

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->priv->info_button),
				      !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->priv->info_button)));
}

/**
 * gsc_popup_page_next:
 * @self: The #GscPopup
 *
 * Shows the next completion page. If it is the last page then 
 * shows the first one.
 *
 */
void
gsc_popup_page_next (GscPopup *self)
{
	gint pages;
	gint current_page;
	gint page;
	GscPopupPage *popup_page;
	
	g_return_if_fail (GSC_IS_POPUP (self));

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
		popup_page = (GscPopupPage *)g_list_nth_data (self->priv->pages, page);
	}
	while (gsc_tree_get_num_proposals (GSC_TREE (popup_page->view)) == 0 &&
	       page != current_page);

	if (page != current_page)
	{
		gtk_notebook_set_current_page (GTK_NOTEBOOK (self->priv->notebook),
					       page);
	
		/*
		 * After setting the page the active_page was updated
		 * so we can update the tree
		 */
		gsc_tree_select_first (get_current_tree (self));
	
		if (GTK_WIDGET_VISIBLE (self->priv->info_window))
		{
			show_completion_info (self);
		}
	}
}

/**
 * gsc_popup_page_previous:
 * @self: The #GscPopup
 *
 * Shows the previous completion page. If it is the first page then
 * shows the last one.
 *
 */
void
gsc_popup_page_previous (GscPopup *self)
{
	gint pages;
	gint current_page;
	gint page;
	GscPopupPage *popup_page;
	
	g_return_if_fail (GSC_IS_POPUP (self));
	
	pages = g_list_length (self->priv->pages);
	current_page = g_list_index (self->priv->pages, self->priv->active_page);
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
		popup_page = (GscPopupPage *)g_list_nth_data (self->priv->pages,
							      page);
	}
	while (gsc_tree_get_num_proposals (GSC_TREE (popup_page->view)) == 0 &&
	       page != current_page);
	
	if (page != current_page)
	{
		gtk_notebook_set_current_page (GTK_NOTEBOOK (self->priv->notebook),
					       page);
	
		/*
		 * After setting the page the active_page was updated
		 * so we can update the tree
		 */
		gsc_tree_select_first (get_current_tree (self));
	
		if (GTK_WIDGET_VISIBLE (self->priv->info_window))
		{
			show_completion_info (self);
		}
	}
}

/**
 * gsc_popup_set_current_info:
 * @self: The #GscPopup
 * @info: Markup with the info to be shown into the info window
 */
void
gsc_popup_set_current_info (GscPopup *self,
			    const gchar *info)
{
	g_return_if_fail (GSC_IS_POPUP (self));

	if (info != NULL)
	{
		gsc_info_set_markup (GSC_INFO (self->priv->info_window), info);
	}
	else
	{
		gsc_info_set_markup (GSC_INFO (self->priv->info_window),
				     _("There is no info for the current proposal"));
	}
}

/**
 * gsc_popup_get_num_active_pages:
 * @self: The #GscPopup
 *
 * Returns: The number of active pages (pages with proposals)
 */
gint
gsc_popup_get_num_active_pages (GscPopup *self)
{
	GList *l;
	guint num_pages_with_data = 0;
	
	g_return_val_if_fail (GSC_IS_POPUP (self), 0);
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscPopupPage *page = (GscPopupPage *)l->data;
		
		if (gsc_tree_get_num_proposals (GSC_TREE (page->view)) > 0)
		{
			num_pages_with_data++;
		}
	}
	
	return num_pages_with_data;
}

/**
 * gsc_popup_show_or_update:
 * @widget: The #GscPopup
 *
 * Show the completion popup or update if it is visible because 
 * using gtk_widget_show only works if the popup is not visible.
 *
 */
void
gsc_popup_show_or_update (GtkWidget *widget)
{
	gboolean data;
	
	/*Only show the popup, the positions is set before this function*/
	GscPopup *self = GSC_POPUP (widget);
	
	data = update_pages_visibility (self);
	
	if (data && !GTK_WIDGET_VISIBLE (self))
	{

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->priv->info_button),
					      FALSE);
		GTK_WIDGET_CLASS (gsc_popup_parent_class)->show (widget);
	}
}

void
gsc_popup_bottom_bar_set_visible (GscPopup *self,
				  gboolean visible)
{
	g_return_if_fail (GSC_IS_POPUP (self));

	if (visible)
		gtk_widget_show (self->priv->bottom_bar);
	else
		gtk_widget_hide (self->priv->bottom_bar);
}

gboolean
gsc_popup_bottom_bar_get_visible (GscPopup *self)
{
	g_return_val_if_fail (GSC_IS_POPUP (self), FALSE);

	return GTK_WIDGET_VISIBLE (self->priv->bottom_bar);
}

gboolean
gsc_popup_autoselect (GscPopup *self)
{
	GscTree *tree;
	
	g_return_val_if_fail (GSC_IS_POPUP (self), FALSE);

	update_pages_visibility (self);
	if (gsc_popup_get_num_active_pages (self) == 1)
	{
		tree = get_current_tree (self);
		if (gsc_tree_get_num_proposals (tree) == 1)
		{
			gsc_tree_select_first (tree);
			return gsc_popup_select_current_proposal (self);
		}
	}
	
	return FALSE;
}

/**
 * gsc_popup_filter_visible:
 * @self: The #GscPopup 
 * @func: function to filter the proposals visibility
 * @user_data: user data to pass to func
 *
 * Returns: TRUE if the popup has visible proposals.
 */
gboolean
gsc_popup_filter_visible (GscPopup *self,
			  GscPopupFilterVisibleFunc func,
			  gpointer user_data)
{
	GList *l;
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscPopupPage *page = (GscPopupPage *)l->data;
		
		if (gsc_tree_get_num_proposals (GSC_TREE (page->view)) > 0)
		{
			gsc_tree_filter_visible (GSC_TREE (page->view),
						 (GscTreeFilterVisibleFunc) func,
						 user_data);
		}
	}
	
	return update_pages_visibility (self);
}




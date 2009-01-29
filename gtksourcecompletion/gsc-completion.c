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

 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <gdk/gdkkeysyms.h> 
#include "gsc-i18n.h"
#include "gsc-utils.h"
#include "gsc-info.h"
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

typedef struct 
{
	GscTrigger *trigger;
	GscProvider *provider;
} PTPair;

/* Signals */
enum
{
	ITEM_SELECTED,
	DISPLAY_INFO,
	COMP_STARTED,
	COMP_FINISHED,
	LAST_SIGNAL
};

static guint signals[LAST_SIGNAL] = { 0 };

G_DEFINE_TYPE(GscCompletion, gsc_completion, GTK_TYPE_WINDOW);

/* **************** GtkTextView-GscManager Control *********** */

/*
 * We save a map with a GtkTextView and his GscManager. If you 
 * call twice to gsc_proposal_new, the second time it returns
 * the previous created GscManager, not creates a new one
 *
 * FIXME We will remove this functions when we will integrate 
 * Gsc in GtkSourceView
 */

static GHashTable *completion_map = NULL;

static GscCompletion* 
completion_control_get_completion (GtkTextView *view)
{
	if (completion_map == NULL)
		completion_map = g_hash_table_new (g_direct_hash,
						   g_direct_equal);

	return g_hash_table_lookup (completion_map, view);
}

static void 
completion_control_add_completion (GtkTextView *view,
				   GscCompletion *comp)
{
	g_hash_table_insert (completion_map, view, comp);
}

static void 
completion_control_remove_completion (GtkTextView *view)
{
	g_hash_table_remove (completion_map, view);
}
/* ********************************************************************* */

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
	return GSC_TREE (self->priv->active_page->view);
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
		/*FIXME search all provider of the active trigger
		GList *l;
		
		for (l = self->priv->providers; l != NULL; l = g_list_next (l))
		{
			gsc_provider_finish (GSC_PROVIDER (l->data));
		}
		*/
		
		self->priv->active_trigger = NULL;
		
		g_signal_emit (G_OBJECT (self), signals[COMP_FINISHED], 0);
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
		
		gsc_tree_clear (GSC_TREE (page->view));
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

static void
gsc_completion_set_current_info (GscCompletion *self,
			    const gchar *info)
{
	g_return_if_fail (GSC_IS_COMPLETION (self));

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

static void
gsc_completion_show_or_update (GtkWidget *widget)
{
	gboolean data;
	
	/*Only show the popup, the positions is set before this function*/
	GscCompletion *self = GSC_COMPLETION (widget);
	
	data = update_pages_visibility (self);
	
	if (data && !GTK_WIDGET_VISIBLE (self))
	{

		gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->priv->info_button),
					      FALSE);
		GTK_WIDGET_CLASS (gsc_completion_parent_class)->show (widget);
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
			_gsc_completion_info_show (self);
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
		popup_page = (GscCompletionPage *)g_list_nth_data (self->priv->pages,
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
			_gsc_completion_info_show (self);
		}
	}
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

	g_signal_emit (G_OBJECT (user_data), signals[ITEM_SELECTED],
		       0, proposal);
}

static void 
selection_changed_cd (GtkTreeSelection *selection, 
		      GscCompletion *self)
{
	if (GTK_WIDGET_VISIBLE (self->priv->info_window))
	{
		_gsc_completion_info_show (self);
	}
}

static GscCompletionPage *
gsc_completion_page_new (GscCompletion *self,
		    const gchar *tree_name)
{
	/*Creates the new trees*/
	GscCompletionPage *page;
	GtkWidget *label;
	GtkWidget *sw;
	GtkTreeSelection *selection;
	
	page = g_slice_new (GscCompletionPage);
	
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
get_tree_by_name (GscCompletion *self,
		  const gchar* tree_name)
{
	GscCompletionPage *page;
	GList *l;
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		page = (GscCompletionPage *)l->data;
	
		if (strcmp (page->name, tree_name) == 0)
			return GSC_TREE (page->view);
	}
	
	page = gsc_completion_page_new (self, tree_name);

	return GSC_TREE (page->view);
}

static void
info_toggled_cb (GtkToggleButton *widget,
		 gpointer user_data)
{
	GscCompletion *self = GSC_COMPLETION (user_data);
	
	if (gtk_toggle_button_get_active (widget))
	{
		_gsc_completion_info_show (self);
	}
	else
	{
		_gsc_completion_info_hide (self);
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

static gboolean
gsc_completion_display_info_default (GscCompletion *self,
				GscProposal *proposal)
{
	if (proposal)
	{
		const gchar *info;
		
		info = gsc_proposal_get_info (proposal);
		gsc_completion_set_current_info (self, info);
	}
	return FALSE;
}

static void
gsc_completion_hide (GtkWidget *widget)
{
	GscCompletion *self = GSC_COMPLETION (widget);
	
	GTK_WIDGET_CLASS (gsc_completion_parent_class)->hide (widget);
	_gsc_completion_info_hide (self);
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
		_gsc_completion_info_show (self);
	
	return ret;
}

static void
gsc_completion_finalize (GObject *object)
{
	GscCompletion *self = GSC_COMPLETION (object);
	
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
	
	g_debug ("completion finalize");
	
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

static void
gsc_completion_class_init (GscCompletionClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	
	g_type_class_add_private (klass, sizeof (GscCompletionPriv));
	
	object_class->finalize = gsc_completion_finalize;
	gtkobject_class->destroy = gsc_completion_destroy;
	widget_class->show = gsc_completion_show_or_update;
	widget_class->hide = gsc_completion_hide;
	widget_class->realize = gsc_completion_realize;
	widget_class->configure_event = gsc_completion_configure_event;
	klass->display_info = gsc_completion_display_info_default;
	
	/**
	 * GscCompletion::proposal-selected:
	 * @completion: The #GscCompletion who emits the signal
	 * @proposal: The selected #GscProposal
	 *
	 * When the user selects a proposal
	 **/
	signals[ITEM_SELECTED] =
		g_signal_new ("proposal-selected",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GscCompletionClass, proposal_selected),
			      NULL, 
			      NULL,
			      g_cclosure_marshal_VOID__POINTER, 
			      G_TYPE_NONE,
			      1,
			      GTK_TYPE_POINTER);
	
	/*FIXME Remove this signal and add toggled-info-visible signals*/
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
	/*FIXME Add the COMP_STARTED and COMP_FINISHED signals*/
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
	
	/*FIXME add active property or remove this field*/
	self->priv->active = TRUE;

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

	self->priv->triggers = NULL;
	self->priv->prov_trig = NULL;
}

void
_gsc_completion_info_show (GscCompletion *self)
{
	GscProposal *proposal = NULL;
	gint y, x, sw, sh;
	
	if (gsc_tree_get_selected_proposal (get_current_tree (self), &proposal))
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

void
_gsc_completion_info_hide (GscCompletion *self)
{
	if (GTK_WIDGET_VISIBLE (self->priv->info_window))
	{
		gtk_widget_hide (self->priv->info_window);
	}
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
	return GTK_WIDGET (self);
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

/**
 * gsc_completion_select_first:
 * @self: The #GscCompletion
 *
 * See #gsc_tree_select_first
 *
 * Returns
 */
gboolean
gsc_completion_select_first (GscCompletion *self)
{
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);
	
	return gsc_tree_select_first (get_current_tree (self));
}

/**
 * gsc_completion_select_last:
 * @self: The #GscCompletion
 *
 * See #gsc_tree_select_last
 *
 * Returns
 */
gboolean 
gsc_completion_select_last (GscCompletion *self)
{
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);

	return gsc_tree_select_last (get_current_tree (self));
}

/**
 * gsc_completion_select_previous:
 * @self: The #GscCompletion
 *
 * See #gsc_tree_select_previous
 *
 * Returns
 */
gboolean
gsc_completion_select_previous (GscCompletion *self, 
			   gint rows)
{
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);

	return gsc_tree_select_previous (get_current_tree (self), rows);
}

/**
 * gsc_completion_select_next:
 * @self: The #GscCompletion
 * @rows: Selects the next @rows row.
 *
 * See #gsc_tree_select_next
 *
 * Returns
 */
gboolean
gsc_completion_select_next (GscCompletion *self, 
		       gint rows)
{
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);

	return gsc_tree_select_next (get_current_tree (self), rows);
}

/**
 * gsc_completion_select_current_proposal:
 * @self: The #GscCompletion
 * 
 * Selects the current selected proposal if there is one selected. This function
 * emits the ITEM_SELECTED signal.
 *
 * Returns: TRUE if a proposal has been selected.
 */
gboolean
gsc_completion_select_current_proposal (GscCompletion *self)
{
	gboolean selected = FALSE;
	GscProposal *prop = NULL;
	if (gsc_tree_get_selected_proposal (get_current_tree (self), &prop))
	{
		g_signal_emit (G_OBJECT (self), signals[ITEM_SELECTED],
			       0, prop);
		selected = TRUE;
	}
	return selected;
}

/**
 * gsc_completion_has_proposals:
 * @self: The #GscCompletion
 *
 * Returns TRUE if the completion has almost one element.
 */
gboolean
gsc_completion_has_proposals (GscCompletion *self)
{
	GList *l;
	
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscCompletionPage *page = (GscCompletionPage *)l->data;
		
		if (gsc_tree_get_num_proposals (GSC_TREE (page->view)) > 0)
			return TRUE;
	}
	return FALSE;
}

/**
 * gsc_completion_toggle_proposal_info:
 * @self: The #GscCompletion
 *
 * This toggle the state of the info dialog. If the info is visible
 * then it hide the info dialog. If the dialog is hidden then it 
 * shows the info dialog.
 *
 */
void
gsc_completion_toggle_proposal_info (GscCompletion *self)
{
	g_return_if_fail (GSC_IS_COMPLETION (self));

	gtk_toggle_button_set_active (GTK_TOGGLE_BUTTON (self->priv->info_button),
				      !gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON (self->priv->info_button)));
}

/**
 * gsc_completion_get_num_active_pages:
 * @self: The #GscCompletion
 *
 * Returns: The number of active pages (pages with proposals)
 */
gint
gsc_completion_get_num_active_pages (GscCompletion *self)
{
	GList *l;
	guint num_pages_with_data = 0;
	
	g_return_val_if_fail (GSC_IS_COMPLETION (self), 0);
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscCompletionPage *page = (GscCompletionPage *)l->data;
		
		if (gsc_tree_get_num_proposals (GSC_TREE (page->view)) > 0)
		{
			num_pages_with_data++;
		}
	}
	
	return num_pages_with_data;
}

/**
 * gsc_completion_bottom_bar_set_visible:
 * @self: The #GscCompletion
 * @visible: %TRUE if you want to show the bottom bar, %FALSE if not.
 *
 */
void
gsc_completion_bottom_bar_set_visible (GscCompletion *self,
				  gboolean visible)
{
	g_return_if_fail (GSC_IS_COMPLETION (self));

	if (visible)
		gtk_widget_show (self->priv->bottom_bar);
	else
		gtk_widget_hide (self->priv->bottom_bar);
}

/**
 * gsc_completion_bottom_bar_set_visible:
 * @self: The #GscCompletion
 *
 *
 * Returns: %TRUE if the bottom bar is visible, %FALSE if not.
 *
 */
gboolean
gsc_completion_bottom_bar_get_visible (GscCompletion *self)
{
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);

	return GTK_WIDGET_VISIBLE (self->priv->bottom_bar);
}
/*FIXME add this in a future
static gboolean
gsc_completion_autoselect (GscCompletion *self)
{
	GscTree *tree;
	
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);

	update_pages_visibility (self);
	if (gsc_completion_get_num_active_pages (self) == 1)
	{
		tree = get_current_tree (self);
		if (gsc_tree_get_num_proposals (tree) == 1)
		{
			gsc_tree_select_first (tree);
			return gsc_completion_select_current_proposal (self);
		}
	}
	
	return FALSE;
}
*/

/**
 * gsc_completion_filter_visible:
 * @self: The #GscCompletion 
 * @func: function to filter the proposals visibility
 * @user_data: user data to pass to func
 *
 * This function call to @func for all proposal of all pages. @func must
 * return %TRUE if the proposal is visible or %FALSE if the completion must to 
 * hide the proposal.
 *
 * Returns: %TRUE if the completion has visible proposals.
 */
gboolean
gsc_completion_filter_visible (GscCompletion *self,
			  GscCompletionFilterFunc func,
			  gpointer user_data)
{
	GList *l;
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscCompletionPage *page = (GscCompletionPage *)l->data;
		
		if (gsc_tree_get_num_proposals (GSC_TREE (page->view)) > 0)
		{
			gsc_tree_filter_visible (GSC_TREE (page->view),
						 (GscTreeFilterVisibleFunc) func,
						 user_data);
		}
	}
	
	return update_pages_visibility (self);
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
 * or the trigger don't exists)
 *
 **/
gboolean
gsc_completion_register_provider (GscCompletion *self,
				  GscProvider *provider,
				  GscTrigger *trigger)
{
	PTPair *ptp;
	
	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);
	g_return_val_if_fail (GSC_IS_PROVIDER (provider), FALSE);
	g_return_val_if_fail (GSC_IS_TRIGGER (trigger), FALSE);
	g_return_val_if_fail (g_list_find (self->priv->triggers, trigger) != NULL,
			      FALSE);

	/*FIXME Check if the provider-trigger pair has been registered*/
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
			free_pair (ptp);
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
	g_return_val_if_fail (GSC_COMPLETION (self), NULL);

	return self->priv->active_trigger;
}

/*FIXME Doc*/
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

	g_return_val_if_fail (GSC_IS_COMPLETION (self), FALSE);
	g_return_val_if_fail (GSC_IS_TRIGGER (trigger), FALSE);
	
	/*
	 * If the completion is visble and there is a trigger active, you cannot
	 * raise a different trigger until the current trigger finish o_O
	 */
	if (GTK_WIDGET_VISIBLE (self)
	    && self->priv->active_trigger != trigger)
	{
		return FALSE;
	}
	
	/*FIXME End the current completion if there is active*/
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
	
	gint x, y;
	gboolean selected = FALSE;
	GtkWindow *win;
	
	/*FIXME 
	if (self->priv->autoselect)
	{
		selected = gsc_completion_autoselect (self);
	}*/

	if (!selected)
	{
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
		
		g_signal_emit (G_OBJECT (self), signals[COMP_STARTED], 0);
		
		gsc_completion_show_or_update (GTK_WIDGET (self));

		/*Set the focus to the View, not the completion popup*/
		win = GTK_WINDOW (gtk_widget_get_ancestor (GTK_WIDGET (self->priv->view),
				  GTK_TYPE_WINDOW));
		gtk_window_present (win);
		gtk_widget_grab_focus (GTK_WIDGET (self->priv->view));

		self->priv->active_trigger = trigger;
	}
	
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
	g_return_if_fail (GSC_COMPLETION (self));

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
	
	g_return_if_fail (GSC_COMPLETION (self));
	g_return_if_fail (func);
	
	if (!GTK_WIDGET_VISIBLE (self))
		return;
	
	for (l = self->priv->pages; l != NULL; l = g_list_next (l))
	{
		GscCompletionPage *page = (GscCompletionPage *)l->data;
		if (gsc_tree_get_num_proposals (GSC_TREE (page->view)) > 0)
		{
			gsc_tree_filter_visible (GSC_TREE (page->view),
             					 (GscTreeFilterVisibleFunc) func,
             					 user_data);
		}
	}
  
	if (!update_pages_visibility (self))
	{
		end_completion (self);
	}
}




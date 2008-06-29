/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-popup.c
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
 
#include "gtksourcecompletion-popup.h"
#include "gtksourcecompletion-tree.h"
#include "gtksourcecompletion-i18n.h"
#include "gtksourcecompletion-utils.h"
#include "gtksourcecompletion-proposal.h"

#define WINDOW_WIDTH 350
#define WINDOW_HEIGHT 200

#define GTK_SOURCE_COMPLETION_POPUP_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object),\
					 GTK_TYPE_SOURCE_COMPLETION_POPUP,                    \
					 GtkSourceCompletionPopupPriv))
					 

/* Signals */
enum
{
	ITEM_SELECTED,
	DISPLAY_INFO,
	LAST_SIGNAL
};

static guint popup_signals[LAST_SIGNAL] = { 0 };

struct _GtkSourceCompletionPopupPriv
{
	GtkWidget *info_window;
	GtkWidget *info_button;
	GtkWidget *info_label;
	GtkWidget *view;
	GtkWidget *notebook;
	GtkWidget *tab_label;
	GtkWidget *next_page_icon;
	GHashTable *trees;
	gboolean destroy_has_run;
};

G_DEFINE_TYPE(GtkSourceCompletionPopup, gtk_source_completion_popup, GTK_TYPE_WINDOW);

static GtkSourceCompletionPopupOptions default_options = {
	GTK_SOURCE_COMPLETION_POPUP_POSITION_CURSOR,
	GTK_SOURCE_COMPLETION_POPUP_FILTER_NONE
};

static void
_proposal_selected_cb (GtkSourceCompletionTree *tree, 
		   GtkSourceCompletionProposal *proposal,
		   gpointer user_data);

static void 
_selection_changed_cd(GtkSourceCompletionTree *tree, 
		      GtkSourceCompletionProposal *proposal,
		      gpointer user_data);


static GtkSourceCompletionTree*
_get_current_tree(GtkSourceCompletionPopup *self)
{
	gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->priv->notebook));
	GtkSourceCompletionTree *tree =
		GTK_SOURCE_COMPLETION_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),page));
	
	return tree;
}

static GtkSourceCompletionTree*
_get_tree_by_name(GtkSourceCompletionPopup *self, const gchar* tree_name)
{
	GtkSourceCompletionTree *tree =
		GTK_SOURCE_COMPLETION_TREE(g_hash_table_lookup(self->priv->trees,tree_name));
		
	if (tree==NULL)
	{
		/*We create the new trees*/
		GtkWidget *completion_tree = gtk_source_completion_tree_new(); 
		g_hash_table_insert(self->priv->trees,(gpointer)tree_name,completion_tree);
		GtkWidget *label = gtk_label_new(tree_name);
		gtk_notebook_append_page(GTK_NOTEBOOK(self->priv->notebook),
			GTK_WIDGET(completion_tree),label);
		tree = GTK_SOURCE_COMPLETION_TREE(completion_tree);
		gtk_widget_show_all(completion_tree);
		g_signal_connect(completion_tree, 
				"proposal-selected",
				G_CALLBACK(_proposal_selected_cb),
				(gpointer) self);
						
		g_signal_connect(completion_tree, 
				"selection-changed",
				G_CALLBACK(_selection_changed_cd),
				(gpointer) self);
	}

	return tree;
}

/*
 * Return TRUE if the position is over the text and FALSE if 
 * the position is under the text.
 */
static gboolean 
_get_popup_position_in_cursor(GtkSourceCompletionPopup *self, gint *x, gint *y)
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
		PangoLayout* layout = 
			gtk_widget_create_pango_layout(GTK_WIDGET(self->priv->view), NULL);
		pango_layout_get_pixel_size(layout,&xtext,&ytext);
		*y = *y -ytext - h;
		g_object_unref(layout);
		return TRUE;
	}

	return FALSE;
}

static void
_get_popup_position_center_screen(GtkSourceCompletionPopup *self, gint *x, gint *y)
{
	gint w,h;
	gint sw = gdk_screen_width();
	gint sh = gdk_screen_height();
	gtk_window_get_size(GTK_WINDOW(self), &w, &h);
	
	*x = (sw/2) - (w/2) - 20;
	*y = (sh/2) - (h/2);
}

static void
_show_completion_info(GtkSourceCompletionPopup *self)
{
	GtkSourceCompletionProposal *proposal;
	gtk_source_completion_popup_get_selected_proposal(self,&proposal);
	if (proposal!=NULL)
	{
		g_signal_emit_by_name (self, "display-info",proposal);
	}
	gint y, x, sw, sh;
	gtk_window_get_position(GTK_WINDOW(self),&x,&y);
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
	GtkSourceCompletionPopup *self = GTK_SOURCE_COMPLETION_POPUP(user_data);
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
_proposal_selected_cb (GtkSourceCompletionTree *tree, 
		   GtkSourceCompletionProposal *proposal,
		   gpointer user_data)
{
	g_signal_emit (G_OBJECT (user_data), popup_signals[ITEM_SELECTED], 0, proposal);
}

static void 
_selection_changed_cd(GtkSourceCompletionTree *tree, 
		      GtkSourceCompletionProposal *proposal,
		      gpointer user_data)
{
	GtkSourceCompletionPopup *self = GTK_SOURCE_COMPLETION_POPUP(user_data);

	if (GTK_WIDGET_VISIBLE(self->priv->info_window))
	{
		_show_completion_info(self);
	}
}

static gboolean
_switch_page_cb(GtkNotebook *notebook, 
		GtkNotebookPage *page,
		gint page_num, 
		gpointer user_data)
{
	GtkSourceCompletionPopup *self = GTK_SOURCE_COMPLETION_POPUP(user_data);
	GtkWidget *tree = gtk_notebook_get_nth_page(notebook,page_num);
	const gchar* label_text = gtk_notebook_get_tab_label_text(notebook,tree);
	
	gtk_label_set_label(GTK_LABEL(self->priv->tab_label),label_text);

	return FALSE;
}

static void
_update_pages_visibility(GtkSourceCompletionPopup *self)
{
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	guint i, num_pages_with_data = 0;
	gboolean first_set = FALSE;
	GtkSourceCompletionTree *tree;
	for(i=0;i<pages;i++)
	{
		tree= GTK_SOURCE_COMPLETION_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),i));
		if (gtk_source_completion_tree_has_proposals(tree))
		{
			if (!first_set)
			{
				gtk_notebook_set_current_page(GTK_NOTEBOOK(self->priv->notebook),i);
				first_set = TRUE;
			}
			num_pages_with_data++;
		}
	}
	if (num_pages_with_data > 1)
		gtk_widget_show(self->priv->next_page_icon);
	else
		gtk_widget_hide(self->priv->next_page_icon);
	
	
}

static void
gtk_source_completion_popup_show_with_opts(GtkWidget *widget, GtkSourceCompletionPopupOptions *options)
{
	GtkSourceCompletionPopup *self = GTK_SOURCE_COMPLETION_POPUP(widget);
	gint x, y;
	if (options->position_type == GTK_SOURCE_COMPLETION_POPUP_POSITION_CURSOR)
		_get_popup_position_in_cursor(self,&x,&y);
	else if (options->position_type == GTK_SOURCE_COMPLETION_POPUP_POSITION_CENTER_SCREEN)
		_get_popup_position_center_screen(self,&x,&y);
	gtk_window_move(GTK_WINDOW(self), x, y);
	
	_update_pages_visibility(self);
	
	if (!GTK_WIDGET_VISIBLE(self))
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->priv->info_button),FALSE);
		GTK_WIDGET_CLASS (gtk_source_completion_popup_parent_class)->show (widget);
	}
	gtk_source_completion_tree_select_first(_get_current_tree(self));
}

static void
gtk_source_completion_popup_show(GtkWidget *widget)
{
	gtk_source_completion_popup_show_with_opts(widget,&default_options);
}

static void
gtk_source_completion_popup_hide(GtkWidget *widget)
{
	GtkSourceCompletionPopup *self = GTK_SOURCE_COMPLETION_POPUP(widget);
	
	GTK_WIDGET_CLASS (gtk_source_completion_popup_parent_class)->hide (widget);
	gtk_widget_hide(self->priv->info_window);

}

static void
gtk_source_completion_popup_realize (GtkWidget *widget)
{
	GtkSourceCompletionPopup *self = GTK_SOURCE_COMPLETION_POPUP(widget);
	gtk_container_set_border_width(GTK_CONTAINER(self),1);
	gtk_widget_set_size_request(GTK_WIDGET(self),WINDOW_WIDTH,WINDOW_HEIGHT);
	gtk_window_set_resizable(GTK_WINDOW(self),TRUE);
	
	GTK_WIDGET_CLASS (gtk_source_completion_popup_parent_class)->realize (widget);
}

static void
gtk_source_completion_popup_finalize (GObject *object)
{
	g_debug("Finish GtkSourceCompletionPopup");
	GtkSourceCompletionPopup *self = GTK_SOURCE_COMPLETION_POPUP(object);
	
	g_hash_table_destroy(self->priv->trees);
	
	G_OBJECT_CLASS (gtk_source_completion_popup_parent_class)->finalize (object);
	
}

static void
gtk_source_completion_popup_destroy (GtkObject *object)
{
	g_debug("Destroy GtkSourceCompletionPopup");

	GtkSourceCompletionPopup *self = GTK_SOURCE_COMPLETION_POPUP(object);
	
	if (!self->priv->destroy_has_run)
	{
		gtk_source_completion_popup_clear(self);
		self->priv->destroy_has_run = TRUE;
	}
	
	GTK_OBJECT_CLASS (gtk_source_completion_popup_parent_class)->destroy (object);
	
}

static void
gtk_source_completion_popup_class_init (GtkSourceCompletionPopupClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	GtkObjectClass *gtkobject_class = GTK_OBJECT_CLASS (klass);
	GtkWidgetClass *widget_class = GTK_WIDGET_CLASS (klass);
	g_type_class_add_private (klass, sizeof(GtkSourceCompletionPopupPriv));
	
	object_class->finalize = gtk_source_completion_popup_finalize;
	gtkobject_class->destroy = gtk_source_completion_popup_destroy;
	widget_class->show = gtk_source_completion_popup_show;
	widget_class->hide = gtk_source_completion_popup_hide;
	widget_class->realize = gtk_source_completion_popup_realize;
	
	popup_signals[ITEM_SELECTED] =
		g_signal_new ("proposal-selected",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			      G_STRUCT_OFFSET (GtkSourceCompletionPopupClass, proposal_selected),
			      NULL, 
			      NULL,
			      g_cclosure_marshal_VOID__POINTER, 
			      G_TYPE_NONE,
			      1,
			      GTK_TYPE_POINTER);
			      
	popup_signals[DISPLAY_INFO] =
		g_signal_new ("display-info",
			      G_TYPE_FROM_CLASS (klass),
			      G_SIGNAL_RUN_FIRST | G_SIGNAL_ACTION,
			      0,
			      NULL, 
			      NULL,
			      g_cclosure_marshal_VOID__POINTER, 
			      G_TYPE_NONE,
			      1,
			      GTK_TYPE_POINTER);
}

static void
gtk_source_completion_popup_init (GtkSourceCompletionPopup *self)
{
	g_debug("Init GtkSourceCompletionPopup");
	gtk_widget_set_size_request(GTK_WIDGET(self),WINDOW_WIDTH,WINDOW_HEIGHT);
	self->priv = GTK_SOURCE_COMPLETION_POPUP_GET_PRIVATE(self);
	self->priv->destroy_has_run = FALSE;
	self->priv->trees = g_hash_table_new(g_str_hash,g_str_equal);

	GtkWidget *completion_tree = gtk_source_completion_tree_new();
	
	g_hash_table_insert(self->priv->trees,DEFAULT_PAGE,completion_tree);
	
	self->priv->notebook = gtk_notebook_new();
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(self->priv->notebook),FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(self->priv->notebook),FALSE);

	GtkWidget *default_label = gtk_label_new(DEFAULT_PAGE);
	gtk_notebook_append_page(GTK_NOTEBOOK(self->priv->notebook),
			GTK_WIDGET(completion_tree),default_label);
	/*Icon list*/
	GtkWidget *info_icon = 
		gtk_image_new_from_stock(GTK_STOCK_INFO,GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_set_tooltip_text(info_icon, _("Show Proposal Info"));
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

	/*Next page icon*/
	self->priv->next_page_icon = gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_box_pack_end(GTK_BOX(hbox),
			 self->priv->next_page_icon,
			 FALSE,
			 FALSE,
			 1);
	/*Page label*/
	self->priv->tab_label = gtk_label_new(DEFAULT_PAGE);
	gtk_box_pack_end(GTK_BOX(hbox),
			 self->priv->tab_label,
			 FALSE,
			 TRUE,
			 10);
	
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
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(info_scroll),
					      self->priv->info_label);
	gtk_container_set_border_width(GTK_CONTAINER(self->priv->info_window),1);
	gtk_container_add(GTK_CONTAINER(self->priv->info_window),info_scroll);
	
	gtk_widget_show(info_scroll);
	gtk_widget_show(self->priv->info_label);
	
	/* Connect signals */
	
	g_signal_connect(completion_tree, 
			"proposal-selected",
			G_CALLBACK(_proposal_selected_cb),
			(gpointer) self);
						
	g_signal_connect(completion_tree, 
			"selection-changed",
			G_CALLBACK(_selection_changed_cd),
			(gpointer) self);
			
	g_signal_connect(self->priv->notebook, 
			"switch-page",
			G_CALLBACK(_switch_page_cb),
			(gpointer) self);
}

GtkWidget*
gtk_source_completion_popup_new (GtkTextView *view)
{
	GtkSourceCompletionPopup *self = GTK_SOURCE_COMPLETION_POPUP ( g_object_new (gtk_source_completion_popup_get_type() , NULL));
	self->priv->view = GTK_WIDGET(view);
	GTK_WINDOW(self)->type = GTK_WINDOW_POPUP;
	return GTK_WIDGET(self);
}

void
gtk_source_completion_popup_add_data(GtkSourceCompletionPopup *self,
			      GtkSourceCompletionProposal* data)
{
	GtkSourceCompletionTree *tree = _get_tree_by_name(self,
						    gtk_source_completion_proposal_get_page_name(data));
	
	gtk_source_completion_tree_add_data(tree,data);
}

void
gtk_source_completion_popup_clear(GtkSourceCompletionPopup *self)
{
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	guint i;
	GtkSourceCompletionTree *tree;
	for(i=0;i<pages;i++)
	{
		tree= GTK_SOURCE_COMPLETION_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),i));
		gtk_source_completion_tree_clear(tree);
	}
}

gboolean
gtk_source_completion_popup_select_first(GtkSourceCompletionPopup *self)
{
	return gtk_source_completion_tree_select_first(_get_current_tree(self));
}

gboolean 
gtk_source_completion_popup_select_last(GtkSourceCompletionPopup *self)
{
	return gtk_source_completion_tree_select_last(_get_current_tree(self));
}

gboolean
gtk_source_completion_popup_select_previous(GtkSourceCompletionPopup *self, 
				     gint rows)
{
	return gtk_source_completion_tree_select_previous(_get_current_tree(self),rows);
}

gboolean
gtk_source_completion_popup_select_next(GtkSourceCompletionPopup *self, 
				 gint rows)
{
	return gtk_source_completion_tree_select_next(_get_current_tree(self),rows);
}

gboolean
gtk_source_completion_popup_get_selected_proposal(GtkSourceCompletionPopup *self,
					GtkSourceCompletionProposal **proposal)
{
	return gtk_source_completion_tree_get_selected_proposal(_get_current_tree(self),proposal);
}

gboolean
gtk_source_completion_popup_has_proposals(GtkSourceCompletionPopup *self)
{
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	guint i;
	GtkSourceCompletionTree *tree;
	for(i=0;i<pages;i++)
	{
		tree= GTK_SOURCE_COMPLETION_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),i));
		if (gtk_source_completion_tree_has_proposals(tree))
			return TRUE;
	}
	return FALSE;
}

void
gtk_source_completion_popup_toggle_proposal_info(GtkSourceCompletionPopup *self)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->priv->info_button),
				     !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self->priv->info_button)));
}

void
gtk_source_completion_popup_refresh(GtkSourceCompletionPopup *self)
{
	gtk_source_completion_popup_show(GTK_WIDGET(self));
}

void
gtk_source_completion_popup_refresh_with_opts(GtkSourceCompletionPopup *self,
					      GtkSourceCompletionPopupOptions *options)
{
	gtk_source_completion_popup_show_with_opts(GTK_WIDGET(self),options);
}

void
gtk_source_completion_popup_page_next(GtkSourceCompletionPopup *self)
{
	GtkSourceCompletionTree *tree;
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->priv->notebook));
	gint original_page = page;
	
	do
	{
		page++;
		if (page == pages)
			page = 0;
		
		tree = GTK_SOURCE_COMPLETION_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),page));
		if (gtk_source_completion_tree_has_proposals(tree))
		{
			gtk_notebook_set_current_page(GTK_NOTEBOOK(self->priv->notebook),page);
			gtk_source_completion_tree_select_first(tree);
			break;
		}
	
	}while(page!=original_page);
	
	if (page!=original_page)
	{
		if (GTK_WIDGET_VISIBLE(self->priv->info_window))
		{
			_show_completion_info(self);
		}
	}
}

void
gtk_source_completion_popup_page_previous(GtkSourceCompletionPopup *self)
{
	GtkSourceCompletionTree *tree;
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->priv->notebook));
	gint original_page = page;
	
	do
	{
		page--;
		if (page < 0)
			page = pages -1;
	
		tree = GTK_SOURCE_COMPLETION_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),page));	
		if (gtk_source_completion_tree_has_proposals(tree))
		{
			gtk_notebook_set_current_page(GTK_NOTEBOOK(self->priv->notebook),page);
			gtk_source_completion_tree_select_first(tree);
			break;
		}
	}while(page!=original_page);
	
	if (page!=original_page)
	{
		if (GTK_WIDGET_VISIBLE(self->priv->info_window))
		{
			_show_completion_info(self);
		}
	}
}

void
gtk_source_completion_popup_set_current_info(GtkSourceCompletionPopup *self,
					     gchar *info)
{
	if (info!=NULL)
	{
		gtk_label_set_markup(GTK_LABEL(self->priv->info_label),info);
	}
	else
	{
		gtk_label_set_markup(GTK_LABEL(self->priv->info_label), 
				     _("There is no info for the current proposal"));
	}
}

gint
gtk_source_completion_popup_get_num_active_pages(GtkSourceCompletionPopup *self)
{
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	guint i, num_pages_with_data = 0;
	GtkSourceCompletionTree *tree;
	for(i=0;i<pages;i++)
	{
		tree= GTK_SOURCE_COMPLETION_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),i));
		if (gtk_source_completion_tree_has_proposals(tree))
		{
			++num_pages_with_data;
		}
	}
	return num_pages_with_data;
}



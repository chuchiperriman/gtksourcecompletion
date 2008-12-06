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
#include "gsc-tree.h"
#include "gsc-i18n.h"
#include "gsc-utils.h"
#include "gsc-info.h"

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

/* Keys */
#define DEFAULT_INFO_KEYS "<Control>i"
#define DEFAULT_PAGE_NEXT_KEYS "Right"
#define DEFAULT_PAGE_PREV_KEYS "Left"

typedef struct 
{
	guint key;
	GdkModifierType mods;
} KeyDef;

struct _GscPopupPriv
{
	GtkWidget *info_window;
	GtkWidget *info_button;
	GtkWidget *notebook;
	GtkWidget *tab_label;
	GtkWidget *next_page_icon;
	GtkWidget *bottom_bar;
	GHashTable *trees;
	gboolean destroy_has_run;
	KeyDef keys[KEYS_LAST];
};

G_DEFINE_TYPE(GscPopup, gsc_popup, GTK_TYPE_WINDOW);

static void
gsc_popup_hide(GtkWidget *widget);

static void
_proposal_selected_cb (GscTree *tree, 
		       GscProposal *proposal,
		       gpointer user_data);

static void 
_selection_changed_cd(GscTree *tree, 
		      GscProposal *proposal,
		      gpointer user_data);

static GscTree*
_get_current_tree(GscPopup *self)
{
	gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->priv->notebook));
	GscTree *tree =
		GSC_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),page));
	
	return tree;
}

static GscTree*
_get_tree_by_name(GscPopup *self, const gchar* tree_name)
{
	GscTree *tree =
		GSC_TREE(g_hash_table_lookup(self->priv->trees,tree_name));
		
	if (tree==NULL)
	{
		/*Creates the new trees*/
		GtkWidget *completion_tree = gsc_tree_new(); 
		g_hash_table_insert(self->priv->trees,(gpointer)tree_name,completion_tree);
		GtkWidget *label = gtk_label_new(tree_name);
		gtk_notebook_append_page(GTK_NOTEBOOK(self->priv->notebook),
			GTK_WIDGET(completion_tree),label);
		tree = GSC_TREE(completion_tree);
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

static void
_show_completion_info(GscPopup *self)
{
	GscProposal *proposal = NULL;
	if (gsc_popup_get_selected_proposal(self,&proposal))
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
	gtk_window_set_transient_for(GTK_WINDOW(self->priv->info_window), gtk_window_get_transient_for (GTK_WINDOW(self)));
	gtk_widget_show(self->priv->info_window);
}

static void
_info_toggled_cb(GtkToggleButton *widget,
		 gpointer user_data)
{
	GscPopup *self = GSC_POPUP(user_data);
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
_proposal_selected_cb (GscTree *tree, 
		   GscProposal *proposal,
		   gpointer user_data)
{
	g_signal_emit (G_OBJECT (user_data), popup_signals[ITEM_SELECTED], 0, proposal);
}

static void 
_selection_changed_cd(GscTree *tree, 
		      GscProposal *proposal,
		      gpointer user_data)
{
	GscPopup *self = GSC_POPUP(user_data);

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
	GscPopup *self = GSC_POPUP(user_data);
	GtkWidget *tree = gtk_notebook_get_nth_page(notebook,page_num);
	const gchar* label_text = gtk_notebook_get_tab_label_text(notebook,tree);
	
	gtk_label_set_label(GTK_LABEL(self->priv->tab_label),label_text);

	return FALSE;
}

static void
_update_pages_visibility(GscPopup *self)
{
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	guint i, num_pages_with_data = 0;
	gboolean first_set = FALSE;
	GscTree *tree;
	for(i=0;i<pages;i++)
	{
		tree= GSC_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),i));
		if (gsc_tree_get_num_proposals(tree)>0)
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

gboolean
gsc_popup_manage_key(GscPopup *self,
		     GdkEventKey *event)
{
	gboolean catched = FALSE;
	gboolean ret = FALSE;
	switch (event->keyval)
 	{
		case GDK_Escape:
		{
			gsc_popup_hide(GTK_WIDGET(self));
			catched = TRUE;
			ret = TRUE;
			break;
		}
 		case GDK_Down:
		{
			ret = gsc_popup_select_next(self, 1);
			catched = TRUE;
			break;
		}
		case GDK_Page_Down:
		{
			ret = gsc_popup_select_next(self, 5);
			catched = TRUE;
			break;
		}
		case GDK_Up:
		{
			ret = gsc_popup_select_previous(self, 1);
			if (!ret)
				ret = gsc_popup_select_first(self);
			catched = TRUE;
			break;
		}
		case GDK_Page_Up:
		{
			ret = gsc_popup_select_previous(self, 5);
			catched = TRUE;
			break;
		}
		case GDK_Home:
		{
			ret = gsc_popup_select_first(self);
			catched = TRUE;
			break;
		}
		case GDK_End:
		{
			ret = gsc_popup_select_last(self);
			catched = TRUE;
			break;
		}
		case GDK_Return:
		case GDK_Tab:
		{
			GscProposal *prop = NULL;
			if (gsc_popup_get_selected_proposal(self,&prop))
			{
				g_signal_emit (G_OBJECT (self), popup_signals[ITEM_SELECTED], 0, prop);
				gsc_popup_hide(GTK_WIDGET(self));
			}
			else
			{
				gsc_popup_hide(GTK_WIDGET(self));
			}
			ret = TRUE;
			catched = TRUE;
			break;
		}
	}
	if (!catched)
	{
		if (gsc_compare_keys(self->priv->keys[KEYS_INFO].key,
				     self->priv->keys[KEYS_INFO].mods,
				     event))
		{
			gsc_popup_toggle_proposal_info(self);
			ret = TRUE;
		}else if (gsc_compare_keys(self->priv->keys[KEYS_PAGE_NEXT].key,
					   self->priv->keys[KEYS_PAGE_NEXT].mods,
					   event))
		{
			gsc_popup_page_next(self);
			ret = TRUE;
		}else if (gsc_compare_keys(self->priv->keys[KEYS_PAGE_PREV].key,
					   self->priv->keys[KEYS_PAGE_PREV].mods,
					   event))
		{
			gsc_popup_page_previous(self);
			ret = TRUE;
		}
	}
	return ret;
}

void
gsc_popup_show_or_update(GtkWidget *widget)
{
	/*TODO This function called only if the widget has not been shown previously*/
	
	/*Only show the popup, the positions is set before this function*/
	GscPopup *self = GSC_POPUP(widget);
	/*
	gtk_window_set_transient_for(GTK_WINDOW(self),
		parent);
	*/
	
	_update_pages_visibility(self);
	
	if (!GTK_WIDGET_VISIBLE(self))
	{
		gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->priv->info_button),FALSE);
		GTK_WIDGET_CLASS (gsc_popup_parent_class)->show (widget);
	}
	
	gsc_tree_select_first(_get_current_tree(self));
}

static void
gsc_popup_hide(GtkWidget *widget)
{
	GscPopup *self = GSC_POPUP(widget);
	
	GTK_WIDGET_CLASS (gsc_popup_parent_class)->hide (widget);
	gtk_widget_hide(self->priv->info_window);

}

static void
gsc_popup_realize (GtkWidget *widget)
{
	GscPopup *self = GSC_POPUP(widget);
	gtk_container_set_border_width(GTK_CONTAINER(self),1);
	gtk_widget_set_size_request(GTK_WIDGET(self),WINDOW_WIDTH,WINDOW_HEIGHT);
	gtk_window_set_resizable(GTK_WINDOW(self),TRUE);
	GTK_WIDGET_CLASS (gsc_popup_parent_class)->realize (widget);
}

static gboolean
gsc_popup_delete_event_cb(GtkWidget *widget,
			GdkEvent  *event,
			gpointer   user_data) 
{
	/*Prevent the alt+F4 keys*/
	return TRUE;
}

static void
gsc_popup_finalize (GObject *object)
{
	GscPopup *self = GSC_POPUP(object);
	
	g_hash_table_destroy(self->priv->trees);
	
	G_OBJECT_CLASS (gsc_popup_parent_class)->finalize (object);
	
}

static void
gsc_popup_destroy (GtkObject *object)
{
	GscPopup *self = GSC_POPUP(object);
	
	if (!self->priv->destroy_has_run)
	{
		gsc_popup_clear(self);
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
	g_type_class_add_private (klass, sizeof(GscPopupPriv));
	
	object_class->finalize = gsc_popup_finalize;
	gtkobject_class->destroy = gsc_popup_destroy;
	widget_class->show = gsc_popup_show_or_update;
	widget_class->hide = gsc_popup_hide;
	widget_class->realize = gsc_popup_realize;
	
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
gsc_popup_init (GscPopup *self)
{
	gtk_window_set_type_hint(GTK_WINDOW(self),
		GDK_WINDOW_TYPE_HINT_NORMAL);
	gtk_window_set_focus_on_map(GTK_WINDOW(self),FALSE);
	gtk_widget_set_size_request(GTK_WIDGET(self),WINDOW_WIDTH,WINDOW_HEIGHT);
	gtk_window_set_decorated(GTK_WINDOW(self),FALSE);
	
	self->priv = GSC_POPUP_GET_PRIVATE(self);
	self->priv->destroy_has_run = FALSE;
	self->priv->trees = g_hash_table_new(g_str_hash,g_str_equal);
	
	gsc_popup_set_key(self,KEYS_INFO,DEFAULT_INFO_KEYS);
	gsc_popup_set_key(self,KEYS_PAGE_NEXT,DEFAULT_PAGE_NEXT_KEYS);
	gsc_popup_set_key(self,KEYS_PAGE_PREV,DEFAULT_PAGE_PREV_KEYS);

	GtkWidget *completion_tree = gsc_tree_new();
	
	GValue val = {0,};
	g_value_init(&val,G_TYPE_BOOLEAN);
	g_value_set_boolean(&val,FALSE);
	g_object_set_property(G_OBJECT(completion_tree),"can-focus",&val);
	
	g_hash_table_insert(self->priv->trees,DEFAULT_PAGE,completion_tree);
	
	self->priv->notebook = gtk_notebook_new();
	g_object_set_property(G_OBJECT(self->priv->notebook),"can-focus",&val);
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
	g_object_set_property(G_OBJECT(info_button),"can-focus",&val);
	self->priv->info_button = info_button;
	gtk_button_set_focus_on_click(GTK_BUTTON(info_button),FALSE);
	gtk_container_add(GTK_CONTAINER(info_button),info_icon);
	g_signal_connect (G_OBJECT (info_button),
			  "toggled",
			  G_CALLBACK (_info_toggled_cb),
			  self);

	self->priv->bottom_bar = gtk_hbox_new(FALSE,1);
	gtk_box_pack_start(GTK_BOX(self->priv->bottom_bar),
			   info_button,
			   FALSE,
			   FALSE,
			   0);

	/*Next page icon*/
	self->priv->next_page_icon = gtk_image_new_from_stock(GTK_STOCK_GO_FORWARD,GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_box_pack_end(GTK_BOX(self->priv->bottom_bar),
			 self->priv->next_page_icon,
			 FALSE,
			 FALSE,
			 1);
	/*Page label*/
	self->priv->tab_label = gtk_label_new(DEFAULT_PAGE);
	gtk_box_pack_end(GTK_BOX(self->priv->bottom_bar),
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
			 self->priv->bottom_bar,
			 FALSE,
			 FALSE,
			 0);

	gtk_container_add(GTK_CONTAINER(self),vbox);
	gtk_widget_show_all(vbox);

	/*Info window*/
	self->priv->info_window = GTK_WIDGET(gsc_info_new());
	/*
	g_object_set(self->priv->info_window, 
			     "can-focus", FALSE,
			     "accept-focus", FALSE,
			      NULL);
	*/
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
			
	g_signal_connect(self,
			"delete-event",
			G_CALLBACK(gsc_popup_delete_event_cb),
			NULL);
	
}

GtkWidget*
gsc_popup_new ()
{
	GscPopup *self = GSC_POPUP ( g_object_new (gsc_popup_get_type() ,
					"type", GTK_WINDOW_POPUP,
					NULL));
	return GTK_WIDGET(self);
}

void
gsc_popup_add_data(GscPopup *self,
		   GscProposal* data)
{
	GscTree *tree = _get_tree_by_name(self,
					  gsc_proposal_get_page_name(data));
	
	gsc_tree_add_data(tree,data);
}

void
gsc_popup_clear(GscPopup *self)
{
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	guint i;
	GscTree *tree;
	for(i=0;i<pages;i++)
	{
		tree= GSC_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),i));
		gsc_tree_clear(tree);
	}
}

gboolean
gsc_popup_select_first(GscPopup *self)
{
	return gsc_tree_select_first(_get_current_tree(self));
}

gboolean 
gsc_popup_select_last(GscPopup *self)
{
	return gsc_tree_select_last(_get_current_tree(self));
}

gboolean
gsc_popup_select_previous(GscPopup *self, 
				     gint rows)
{
	return gsc_tree_select_previous(_get_current_tree(self),rows);
}

gboolean
gsc_popup_select_next(GscPopup *self, 
				 gint rows)
{
	return gsc_tree_select_next(_get_current_tree(self),rows);
}

gboolean
gsc_popup_get_selected_proposal(GscPopup *self,
				GscProposal **proposal)
{
	return gsc_tree_get_selected_proposal(_get_current_tree(self),proposal);
}

gboolean
gsc_popup_has_proposals(GscPopup *self)
{
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	guint i;
	GscTree *tree;
	for(i=0;i<pages;i++)
	{
		tree= GSC_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),i));
		if (gsc_tree_get_num_proposals(tree) > 0)
			return TRUE;
	}
	return FALSE;
}

void
gsc_popup_toggle_proposal_info(GscPopup *self)
{
	gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(self->priv->info_button),
				     !gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(self->priv->info_button)));
}

void
gsc_popup_page_next(GscPopup *self)
{
	GscTree *tree;
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->priv->notebook));
	gint original_page = page;
	
	do
	{
		page++;
		if (page == pages)
			page = 0;
		
		tree = GSC_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),page));
		if (gsc_tree_get_num_proposals(tree) > 0)
		{
			gtk_notebook_set_current_page(GTK_NOTEBOOK(self->priv->notebook),page);
			gsc_tree_select_first(tree);
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
gsc_popup_page_previous(GscPopup *self)
{
	GscTree *tree;
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	gint page = gtk_notebook_get_current_page(GTK_NOTEBOOK(self->priv->notebook));
	gint original_page = page;
	
	do
	{
		page--;
		if (page < 0)
			page = pages -1;
	
		tree = GSC_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),page));	
		if (gsc_tree_get_num_proposals(tree) > 0)
		{
			gtk_notebook_set_current_page(GTK_NOTEBOOK(self->priv->notebook),page);
			gsc_tree_select_first(tree);
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
gsc_popup_set_current_info(GscPopup *self,
					     gchar *info)
{
	if (info!=NULL)
	{
		gsc_info_set_markup(GSC_INFO(self->priv->info_window),info);
	}
	else
	{
		gsc_info_set_markup(GSC_INFO(self->priv->info_window),
				     _("There is no info for the current proposal"));
	}
}

gint
gsc_popup_get_num_active_pages(GscPopup *self)
{
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	guint i, num_pages_with_data = 0;
	GscTree *tree;
	for(i=0;i<pages;i++)
	{
		tree= GSC_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),i));
		if (gsc_tree_get_num_proposals(tree) > 0)
		{
			++num_pages_with_data;
		}
	}
	return num_pages_with_data;
}

/* TODO We will need this code to filter the popup content

static void
_filter_changed_cb (GtkEntry *entry,
		    gpointer  user_data)
{
	GscPopup *self = GSC_POPUP(user_data);
	
	gint pages = gtk_notebook_get_n_pages(GTK_NOTEBOOK(self->priv->notebook));
	guint i;
	GscTree *tree;
	for(i=0;i<pages;i++)
	{
		tree = GSC_TREE(gtk_notebook_get_nth_page(GTK_NOTEBOOK(self->priv->notebook),i));
		gsc_tree_filter(tree,
				gtk_entry_get_text(GTK_ENTRY(self->priv->filter)));
	}
	gsc_popup_select_first(self);
}
*/

void
gsc_popup_set_key(GscPopup *self, KeysType type, const gchar* keys)
{
	guint key;
	GdkModifierType mods;
	gtk_accelerator_parse(keys,&key,&mods);
	g_return_if_fail(key!=0);
	self->priv->keys[type].key = key;
	self->priv->keys[type].mods = mods;
}

gchar*
gsc_popup_get_key(GscPopup *self, KeysType type)
{
	return gtk_accelerator_name(self->priv->keys[type].key,
				    self->priv->keys[type].mods);
}

void
gsc_popup_bottom_bar_set_visible(GscPopup *self, gboolean visible)
{
	if (visible)
		gtk_widget_show(self->priv->bottom_bar);
	else
		gtk_widget_hide(self->priv->bottom_bar);
}

gboolean
gsc_popup_bottom_bar_get_visible(GscPopup *self)
{
	return GTK_WIDGET_VISIBLE(self->priv->bottom_bar);
}




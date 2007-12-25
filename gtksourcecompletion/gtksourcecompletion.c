/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion.c
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
#include <string.h>
#include "gtksourcecompletion.h"
#include "gtksourcecompletion-utils.h"

#define COL_PIXBUF 0
#define COL_NAME 1
#define COL_PROVIDER 2
#define COL_DATA 3

#define WINDOW_WIDTH 350
#define WINDOW_HEIGHT 200

/* Internal signals */
enum
{
	IS_POPUP_ROW_ACTIVATE,
	IS_GTK_TEXT_VIEW_KP,
	IS_GTK_TEXT_DESTROY,
	IS_GTK_TEXT_FOCUS_OUT,
	IS_LAST_SIGNAL
};

struct _GtkSourceCompletionPrivate
{
	GtkTextView *text_view;
	GtkTreeView *data_tree_view;
	GtkWidget *window;
	GList *providers;
	GList *triggers;
	gulong internal_signal_ids[IS_LAST_SIGNAL];
	GtkWidget *info_window;
	GtkWidget *info_label;
	GtkToggleButton *info_button;
	gboolean active;
	
};

struct _GtkSourceCompletionItem
{
	int id;
	gchar *name;
	const GdkPixbuf *icon;
	int priority;
	gpointer user_data;
};

#define GTK_SOURCE_COMPLETION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GTK_TYPE_SOURCE_COMPLETION, GtkSourceCompletionPrivate))

struct _InternalCompletionData
{
	GtkSourceCompletionProvider *provider;
	GtkSourceCompletionItem *data;
};

typedef struct _InternalCompletionData InternalCompletionData;

static GObjectClass* parent_class = NULL;

const gchar* NO_INFO = "There is no info for the current item";

static void 
show_completion_info		(GtkSourceCompletion *completion);
static void
set_current_completion_info(GtkSourceCompletion *completion);

/***************** GtkTextView-GtkSourceCompletion Control ************/

/*
 * We save a map with a GtkTextView and his GtkSourceCompletion. If you 
 * call twice to gtk_source_completion_item_new, the second time it returns
 * the previous created GtkSourceCompletion, not creates a new one
 */

static GHashTable *completion_map = NULL;

static GtkSourceCompletion* 
completion_control_get_completion(GtkTextView *view)
{
	if (completion_map==NULL)
		completion_map = g_hash_table_new(g_direct_hash,g_direct_equal);

	return g_hash_table_lookup(completion_map,view);
}

static void 
completion_control_add_completion(GtkTextView *view,GtkSourceCompletion *comp)
{
	g_hash_table_insert(completion_map,view,comp);
}

static void 
completion_control_remove_completion(GtkTextView *view)
{
	g_hash_table_remove(completion_map,view);
}

/***************** GtkSourceCompletionItem functions ******************/
static void
gtk_source_completion_item_free(GtkSourceCompletionItem *item)
{
	g_free(item->name);
	g_free(item);
}

/**
 * gtk_source_completion_item_new:
 * @id: An id for identify this item
 * @name: Item name that will be shown in the completion popup
 * @icon: Item icon that will be shown in the completion popup
 * @priority: The item priority. Items with high priority will be
 * 				shown first in the completion popup
 * @user_data: User data used by the providers
 *
 * Returns The new GtkSourceCompletionItem
 */
GtkSourceCompletionItem*
gtk_source_completion_item_new(int id,
							const gchar *name,
							const GdkPixbuf *icon,
							int priority,
							gpointer user_data)
{
	GtkSourceCompletionItem* item = g_malloc0(sizeof(GtkSourceCompletionItem));
	item->id = id;
	item->name = g_strdup(name);
	item->icon = icon;
	item->priority = priority;
	item->user_data = user_data;
	return item;
}

/**
 * gtk_source_completion_item_get_name:
 * @item: The GtkSourceCompletionItem
 *
 * Returns The item name
 */
gchar*
gtk_source_completion_item_get_name(GtkSourceCompletionItem *item)
{
	return item->name;
}

/**
 * gtk_source_completion_item_get_user_data:
 * @item: The GtkSourceCompletionItem
 *
 * Returns the user data of this item
 */
gchar*
gtk_source_completion_item_get_user_data(GtkSourceCompletionItem *item)
{
	return item->user_data;
}
/**********************************************************************/

static gint
internal_data_compare (gconstpointer v1,
					gconstpointer v2)
{
	InternalCompletionData *i1 = (InternalCompletionData*) v1;
	InternalCompletionData *i2 = (InternalCompletionData*) v2;
	
	return i2->data->priority - i1->data->priority;
	
}

static void
end_completion (GtkSourceCompletion *completion)
{
	/*
	* If a provider is registered in two events, they 
	* will be called twice.
	*/
	gtk_widget_hide(completion->priv->window);
	gtk_widget_hide(completion->priv->info_window);

	GList *providers = completion->priv->providers;
	do
	{
		GtkSourceCompletionProvider *provider =  GTK_SOURCE_COMPLETION_PROVIDER(providers->data);
		gtk_source_completion_provider_end_completion(provider,completion->priv->text_view);
			
	}while((providers = g_list_next(providers)) != NULL);
		
}

/*
 * Returns new allocated InternalCompletionData.  You must free it.
 */
static InternalCompletionData*
get_selected_item(GtkSourceCompletion *completion)
{
	InternalCompletionData *idata = NULL;
	GtkTreeIter iter;
	GtkTreeModel *model;
	GValue value_prov = {0,};
	GValue value_item = {0,};
	GtkTreeSelection *selection = gtk_tree_view_get_selection(completion->priv->data_tree_view);
	if (gtk_tree_selection_get_selected(selection,NULL, &iter))
	{
		model = gtk_tree_view_get_model(completion->priv->data_tree_view);
		gtk_tree_model_get_value(model,&iter,COL_PROVIDER,&value_prov);
		gtk_tree_model_get_value(model,&iter,COL_DATA,&value_item);
		idata = g_malloc0(sizeof(InternalCompletionData));
		idata->provider = GTK_SOURCE_COMPLETION_PROVIDER(g_value_get_pointer(&value_prov));
		idata->data = (GtkSourceCompletionItem*)g_value_get_pointer(&value_item);
	}
	
	return idata;
}

/***************** MOVING IN THE TREE ***********************/
static gboolean
popup_tree_first(GtkSourceCompletion *completion)
{
	GtkTreeIter iter;
	GtkTreePath* path;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	if (!gtk_source_completion_is_visible(completion))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(completion->priv->data_tree_view);

	if (gtk_tree_selection_get_mode(selection) == GTK_SELECTION_NONE)
		return FALSE;

	model = gtk_tree_view_get_model(completion->priv->data_tree_view);
		
	gtk_tree_model_get_iter_first(model, &iter);
	gtk_tree_selection_select_iter(selection, &iter);
	path = gtk_tree_model_get_path(model, &iter);
	gtk_tree_view_scroll_to_cell(completion->priv->data_tree_view, path, NULL, FALSE, 0, 0);
	gtk_tree_path_free(path);
	return TRUE;
}

static gboolean 
popup_tree_last(GtkSourceCompletion *completion)
{
	GtkTreeIter iter;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	GtkTreePath* path;
	gint children;
	
	if (!gtk_source_completion_is_visible(completion))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(completion->priv->data_tree_view);
	model = gtk_tree_view_get_model(completion->priv->data_tree_view);
	
	if (gtk_tree_selection_get_mode(selection) == GTK_SELECTION_NONE)
		return FALSE;
	
	children = gtk_tree_model_iter_n_children(model, NULL);
	if (children > 0)
	{
		gtk_tree_model_iter_nth_child(model, &iter, NULL, children - 1);
	
		gtk_tree_selection_select_iter(selection, &iter);
		path = gtk_tree_model_get_path(model, &iter);
		gtk_tree_view_scroll_to_cell(completion->priv->data_tree_view, path, NULL, FALSE, 0, 0);
		gtk_tree_path_free(path);
		return TRUE;
	}
	return FALSE;
}

static gboolean
popup_tree_up(GtkSourceCompletion *completion, 
					gint rows)
{
	GtkTreeIter iter;
	GtkTreePath* path;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	
	if (!gtk_source_completion_is_visible(completion))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(completion->priv->data_tree_view);
	
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
			gtk_tree_view_scroll_to_cell(completion->priv->data_tree_view, path, NULL, FALSE, 0, 0);
		}
		gtk_tree_path_free(path);
	}
	else
	{
		return popup_tree_first(completion);
	}
	
	return TRUE;
}

static gboolean
popup_tree_down(GtkSourceCompletion *completion, 
					gint rows)
{
	GtkTreeIter iter;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	
	if (!gtk_source_completion_is_visible(completion))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(completion->priv->data_tree_view);
	if (gtk_tree_selection_get_mode(selection) == GTK_SELECTION_NONE)
		return FALSE;
	
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gint i;
		GtkTreePath* path;
		for (i = 0; i < rows; i++)
		{
			if (!gtk_tree_model_iter_next(model, &iter))
				return popup_tree_last(completion);
		}
		gtk_tree_selection_select_iter(selection, &iter);
		path = gtk_tree_model_get_path(model, &iter);
		gtk_tree_view_scroll_to_cell(completion->priv->data_tree_view, path, NULL, FALSE, 0, 0);
		gtk_tree_path_free(path);
	}
	else
	{
		return popup_tree_first(completion);
	}
	return TRUE;
}

static gboolean
popup_tree_selection(GtkSourceCompletion *completion)
{
	GtkTreeIter iter;
	GtkTreeModel* model;
	GtkTreeSelection* selection;
	GtkTreePath* path;
	
	if (!gtk_source_completion_is_visible(completion))
		return FALSE;
	
	selection = gtk_tree_view_get_selection(completion->priv->data_tree_view);
	
	if (gtk_tree_selection_get_mode(selection) == GTK_SELECTION_NONE)
		return FALSE;	
		
	if (gtk_tree_selection_count_selected_rows(selection)==0)
	{
		//end_completion (completion);
		return FALSE;
	}

	
	if (gtk_tree_selection_get_selected(selection, &model, &iter))
	{
		gtk_tree_selection_select_iter(selection, &iter);
		path = gtk_tree_model_get_path(model, &iter);
		if (path!=NULL)
		{
			gtk_tree_view_row_activated(completion->priv->data_tree_view,
								path,
								gtk_tree_view_get_column(completion->priv->data_tree_view,COL_NAME));
		}
		gtk_tree_path_free(path);
		return TRUE;
	}

	return FALSE;
}

/****************************************************************************/

static gboolean
view_key_press_event_cb(GtkWidget *view,
					GdkEventKey *event, 
					gpointer user_data)
{
	/* Catch only keys of popup movement */
	gboolean ret = FALSE;
	GtkSourceCompletion *completion;
	g_assert(GTK_IS_SOURCE_COMPLETION(user_data));
	completion = GTK_SOURCE_COMPLETION(user_data);
	
	if (gtk_source_completion_is_visible(completion))
	{
		switch (event->keyval)
	 	{
			case GDK_Escape:
			case GDK_space:
			{
				end_completion (completion);
				break;
			}
	 		case GDK_Down:
			{
				ret = popup_tree_down(completion, 1);
				break;
			}
			case GDK_Page_Down:
			{
				ret = popup_tree_down(completion, 5);
				break;
			}
			case GDK_Up:
			{
				if (popup_tree_up(completion, 1))
				{
					ret = TRUE;
				}
				else
				{
					ret = popup_tree_first(completion);
				}

				break;
			}
			case GDK_Page_Up:
			{
				ret = popup_tree_up(completion, 5);
				break;
			}
			case GDK_Home:
			{
				ret = popup_tree_first(completion);
				break;
			}
			case GDK_End:
			{
				ret = popup_tree_last(completion);
				break;
			}
			case GDK_Return:
			case GDK_Tab:
			{
				ret = popup_tree_selection(completion);
				if (!ret)
				{
					end_completion(completion);
				}
				break;
			}
			/*Special keys... */
			case GDK_i:
			{
				if ((event->state & GDK_CONTROL_MASK))
				{
					/*View information of the item */
					gtk_toggle_button_set_active(completion->priv->info_button,
									!gtk_toggle_button_get_active(completion->priv->info_button));
					ret = TRUE;
				}
			}
		}

	}
	
	return ret;

}

static void
view_destroy_event_cb(GtkWidget *widget,
				gpointer user_data)
{
	GtkSourceCompletion *completion = GTK_SOURCE_COMPLETION(user_data);
	g_object_unref(completion);
}

static gboolean
view_focus_out_event_cb(GtkWidget *widget,
				GdkEventFocus *event,
				gpointer user_data)
{
	GtkSourceCompletion *completion = GTK_SOURCE_COMPLETION(user_data);
	if (gtk_source_completion_is_visible(completion))
	{
		end_completion(completion);
	}
	return FALSE;
}
static void
popup_tree_row_activated_cb (GtkTreeView *tree_view,
										GtkTreePath *path,
										GtkTreeViewColumn *column,
										gpointer user_data)
{
	GtkTreeIter iter;
	GtkTreeModel *model;
	GtkSourceCompletionProvider *provider;
	GtkSourceCompletionItem *data;
	GtkSourceCompletion *completion;
	GValue value_prov = {0,};
	GValue value_name = {0,};
	
	completion = GTK_SOURCE_COMPLETION(user_data);
	model = gtk_tree_view_get_model(tree_view);
	
	gtk_tree_model_get_iter(model,&iter,path);
	gtk_tree_model_get_value(model,&iter,COL_DATA,&value_name);
	data = (GtkSourceCompletionItem*)g_value_get_pointer(&value_name);
	gtk_tree_model_get_value(model,&iter,COL_PROVIDER,&value_prov);
	provider = GTK_SOURCE_COMPLETION_PROVIDER(g_value_get_pointer(&value_prov));
	gtk_source_completion_provider_data_selected(provider,completion->priv->text_view, data);
	end_completion (completion);
}

static gboolean
info_toggled_cb(GtkToggleButton *widget,
				gpointer user_data)
{
	GtkSourceCompletion *completion = GTK_SOURCE_COMPLETION(user_data);
	if (gtk_toggle_button_get_active(widget))
	{
		show_completion_info(completion);
	}
	else
	{
		gtk_widget_hide(completion->priv->info_window);
	}
	return FALSE;
}

static void 
selection_changed_cd(GtkTreeSelection *treeselection,
					gpointer user_data)
{
	GtkSourceCompletion *completion = GTK_SOURCE_COMPLETION(user_data);

	if (GTK_WIDGET_VISIBLE(completion->priv->info_window))
	{
		show_completion_info(completion);
	}
}

static void
view_get_screen_pos(GtkTextView *text_view, 
					gint *x,
					gint *y)
{
	GdkWindow *win;
	GtkTextMark* insert_mark;
	GtkTextBuffer* text_buffer;
	GtkTextIter start;
	GdkRectangle location;
	gint win_x, win_y;
	gint xx, yy;

	text_buffer = gtk_text_view_get_buffer(text_view);
	insert_mark = gtk_text_buffer_get_insert(text_buffer);
	gtk_text_buffer_get_iter_at_mark(text_buffer,&start,insert_mark);
	gtk_text_view_get_iter_location(text_view,
														&start,
														&location );
	gtk_text_view_buffer_to_window_coords (text_view,
                                        GTK_TEXT_WINDOW_WIDGET,
                                        location.x, location.y,
                                        &win_x, &win_y);

	win = gtk_text_view_get_window (text_view, 
                                GTK_TEXT_WINDOW_WIDGET);
	gdk_window_get_origin (win, &xx, &yy);
	
	*x = win_x + xx;
	*y = win_y + yy + location.height;
	
}

static void
clean_model_data(GtkListStore *store)
{
	GtkTreeModel *model = GTK_TREE_MODEL(store);
	GtkTreeIter iter;
	GtkSourceCompletionItem *data;
	GtkSourceCompletionProvider *prov;
	
	
	if (gtk_tree_model_get_iter_first(model,&iter))
	{
		do
		{
			GValue value_data = {0,};
			GValue value_prov = {0,};
			gtk_tree_model_get_value(model,&iter,COL_DATA,&value_data);
			data = (GtkSourceCompletionItem*)g_value_get_pointer(&value_data);
			gtk_tree_model_get_value(model,&iter,COL_PROVIDER,&value_prov);
			prov = GTK_SOURCE_COMPLETION_PROVIDER(g_value_get_pointer(&value_prov));
			gtk_source_completion_item_free(data);
		}while(gtk_tree_model_iter_next(model,&iter));
	}
	
	gtk_list_store_clear(store);
}

static void
gtcp_create_tree_model(GtkSourceCompletion *completion)
{
	GtkListStore *list_store;
	list_store = gtk_list_store_new (4,GDK_TYPE_PIXBUF, G_TYPE_STRING, G_TYPE_POINTER, G_TYPE_POINTER);
		
	gtk_tree_view_set_model(completion->priv->data_tree_view,GTK_TREE_MODEL(list_store));
}

static void
gtcp_add_data_to_tree(GtkSourceCompletion *completion, 
					GtkSourceCompletionItem* data, 
					GtkSourceCompletionProvider *provider)
{

	g_assert(data != NULL);
	g_assert(provider != NULL);
	
	GtkTreeIter iter;
	GtkListStore *store;
	
	store = GTK_LIST_STORE(gtk_tree_view_get_model(completion->priv->data_tree_view));
	
	gtk_list_store_append (store,&iter);
			
	gtk_list_store_set (store, 
						&iter,
						COL_PIXBUF, data->icon,
						COL_NAME, data->name,
						COL_PROVIDER, (gpointer)provider,
						COL_DATA, data,
						-1);
}

/* Create the tree but not the model */
static void
gtcp_load_tree(GtkSourceCompletion *completion)
{
	g_assert(completion!=NULL);
	GtkCellRenderer *renderer;
	GtkTreeViewColumn *column;
	GtkCellRenderer* renderer_pixbuf;
	GtkTreeViewColumn* column_pixbuf;
	
	/* Icon column */
	renderer_pixbuf = gtk_cell_renderer_pixbuf_new();
   column_pixbuf = gtk_tree_view_column_new_with_attributes ("Pixbuf",
   		renderer_pixbuf, "pixbuf", COL_PIXBUF, NULL);
	
	/* Name column */
	column = gtk_tree_view_column_new();
	renderer = gtk_cell_renderer_text_new ();
	gtk_tree_view_column_pack_start (column, renderer, FALSE);
	gtk_tree_view_column_set_attributes (column,renderer,"text",COL_NAME,NULL);

	gtk_tree_view_append_column (completion->priv->data_tree_view, column_pixbuf);
	gtk_tree_view_append_column (completion->priv->data_tree_view, column);
}

static void
gtcp_create_popup(GtkSourceCompletion *completion)
{
	completion->priv->window = gtk_window_new(GTK_WINDOW_POPUP);
	gtk_widget_set_size_request(completion->priv->window,WINDOW_WIDTH,WINDOW_HEIGHT);
	gtk_window_set_resizable(GTK_WINDOW(completion->priv->window),TRUE);
	gtk_container_set_border_width(GTK_CONTAINER(completion->priv->window),1);
	
	/*HBox. Up the scroll and the tree and down the icon list*/
	/* Scroll and tree */
	GtkWidget* scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scroll),
						GTK_POLICY_AUTOMATIC,
						GTK_POLICY_AUTOMATIC);
	completion->priv->data_tree_view = GTK_TREE_VIEW(gtk_tree_view_new());
	gtk_tree_view_set_headers_visible(completion->priv->data_tree_view,FALSE);
	
	gtk_container_add(GTK_CONTAINER(scroll),GTK_WIDGET(completion->priv->data_tree_view));
	/*Icon list*/
	GtkWidget *info_icon = gtk_image_new_from_stock(GTK_STOCK_INFO,GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_widget_set_tooltip_text(info_icon, "Show Item Info");
	GtkWidget *info_button = gtk_toggle_button_new();
	completion->priv->info_button = GTK_TOGGLE_BUTTON(info_button);
	gtk_container_add(GTK_CONTAINER(info_button),info_icon);
	g_signal_connect (G_OBJECT (info_button), 
					"toggled",
					G_CALLBACK (info_toggled_cb),
					completion);

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

	gtk_container_add(GTK_CONTAINER(completion->priv->window),vbox);
	gtk_widget_show_all(vbox);

	/*Info window*/
	completion->priv->info_window = gtk_window_new(GTK_WINDOW_POPUP);
	gtk_widget_set_size_request(completion->priv->info_window,WINDOW_WIDTH,WINDOW_HEIGHT);
	GtkWidget* info_scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(info_scroll),
						GTK_POLICY_AUTOMATIC,
						GTK_POLICY_AUTOMATIC);
	completion->priv->info_label = gtk_label_new(NULL);
	gtk_scrolled_window_add_with_viewport(GTK_SCROLLED_WINDOW(info_scroll),GTK_WIDGET(completion->priv->info_label));
	gtk_container_set_border_width(GTK_CONTAINER(completion->priv->info_window),1);
	gtk_container_add(GTK_CONTAINER(completion->priv->info_window),info_scroll);
	
	gtk_widget_show(info_scroll);
	gtk_widget_show(completion->priv->info_label);

}

static void
free_provider_list(gpointer list)
{
	GList *start = (GList*)list;
	GList *temp = (GList*)list;
	if (temp != NULL)
	{
		do
		{
			g_object_unref(G_OBJECT(temp->data));
			
		}while((temp = g_list_next(temp)) != NULL);
		g_list_free(start);
	}
	
}

static void
free_trigger_list(gpointer list)
{
	GList *start = (GList*)list;
	GList *temp = (GList*)list;
	if (temp != NULL)
	{
		do
		{
			g_object_unref(G_OBJECT(temp->data));
			
		}while((temp = g_list_next(temp)) != NULL);
		g_list_free(start);
	}
	
}

/*
 * Return TRUE if the position is over the text and FALSE if 
 * the position is under the text.
 */
static gboolean 
get_popup_position(GtkSourceCompletion *completion, gint *x, gint *y)
{
	gint w,h,xtext,ytext;
	gint sw = gdk_screen_width();
	gint sh = gdk_screen_height();

	view_get_screen_pos(completion->priv->text_view,x,y);
	gtk_window_get_size(GTK_WINDOW(completion->priv->window), &w, &h);
	if (*x+w > sw) *x = sw - w -4;
	/*If we cannot show it down, we show it up.*/
	if (*y+h > sh)
	{
		PangoLayout* layout = gtk_widget_create_pango_layout(GTK_WIDGET(completion->priv->text_view), NULL);
		pango_layout_get_pixel_size(layout,&xtext,&ytext);
		*y = *y -ytext - h;
		g_object_unref(layout);
		return TRUE;
	}

	return FALSE;
}

static void
set_current_completion_info(GtkSourceCompletion *completion)
{
	gchar* markup = NULL;
	InternalCompletionData *idata = get_selected_item(completion);
	if (idata!=NULL)
	{
		markup = gtk_source_completion_provider_get_item_info_markup(idata->provider,idata->data);
	}
	g_free(idata);
	
	if (markup != NULL)
	{
		gtk_label_set_markup(GTK_LABEL(completion->priv->info_label),markup);
		g_free(markup);
	}
	else
	{
		gtk_label_set_markup(GTK_LABEL(completion->priv->info_label), NO_INFO);
	}
}

static void
show_completion_info(GtkSourceCompletion *completion)
{
	set_current_completion_info(completion);
	gint y, x, sw, sh;
	get_popup_position(completion,&x,&y);
	sw = gdk_screen_width();
	sh = gdk_screen_height();
	x += WINDOW_WIDTH;

	if (x + WINDOW_WIDTH >= sw)
	{
		x -= (WINDOW_WIDTH * 2);
	}
	gtk_window_move(GTK_WINDOW(completion->priv->info_window), x, y);
	gtk_widget_show(completion->priv->info_window);
}

static void
show_completion_popup(GtkSourceCompletion *completion)
{
	gint x, y;
	get_popup_position(completion,&x,&y);
	gtk_window_move(GTK_WINDOW(completion->priv->window), x, y);
	
	gtk_toggle_button_set_active(completion->priv->info_button,FALSE);
	gtk_widget_show(completion->priv->window);
	gtk_tree_view_scroll_to_point(completion->priv->data_tree_view,0,0);
}

static void
gtk_source_completion_init (GtkSourceCompletion *completion)
{
	gint i;
	completion->priv = GTK_SOURCE_COMPLETION_GET_PRIVATE(completion);
	
	completion->priv->active = FALSE;
	completion->priv->providers = NULL;
	completion->priv->triggers = NULL;
	gtcp_create_popup(completion);
	gtcp_load_tree(completion);
	gtcp_create_tree_model(completion);
	
	for (i=0;i<IS_LAST_SIGNAL;i++)
	{
		completion->priv->internal_signal_ids[i] = 0;
	}
	
	completion->priv->internal_signal_ids[IS_POPUP_ROW_ACTIVATE] = 
			g_signal_connect(completion->priv->data_tree_view, 
						"row-activated",
						G_CALLBACK(popup_tree_row_activated_cb),
						(gpointer) completion);
						
	GtkTreeSelection *selection = gtk_tree_view_get_selection(completion->priv->data_tree_view);
	g_signal_connect(selection, 
						"changed",
						G_CALLBACK(selection_changed_cd),
						(gpointer) completion);

}

static void
gtk_source_completion_finalize (GObject *object)
{
	GtkSourceCompletion *completion = GTK_SOURCE_COMPLETION(object);
	g_debug("finish GtkSourceCompletion");
	if (completion->priv->active)
	{
		gtk_source_completion_deactivate(completion);
	}
	
	GtkListStore *store;
	
	store = GTK_LIST_STORE(gtk_tree_view_get_model(completion->priv->data_tree_view));
	clean_model_data(store);

	free_provider_list(completion->priv->providers);
	free_trigger_list(completion->priv->triggers);

	completion_control_remove_completion(completion->priv->text_view);
	
	/*TODO We needs to free the widgets*/

}

static void
gtk_source_completion_class_init (GtkSourceCompletionClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));

	object_class->finalize = gtk_source_completion_finalize;

	g_type_class_add_private (object_class, sizeof(GtkSourceCompletionPrivate));				
}

GType
gtk_source_completion_get_type (void)
{
	static GType our_type = 0;

	if(our_type == 0)
	{
		static const GTypeInfo our_info =
		{
			sizeof (GtkSourceCompletionClass), /* class_size */
			(GBaseInitFunc) NULL, /* base_init */
			(GBaseFinalizeFunc) NULL, /* base_finalize */
			(GClassInitFunc) gtk_source_completion_class_init, /* class_init */
			(GClassFinalizeFunc) NULL, /* class_finalize */
			NULL /* class_data */,
			sizeof (GtkSourceCompletion), /* instance_size */
			0, /* n_preallocs */
			(GInstanceInitFunc) gtk_source_completion_init, /* instance_init */
			NULL /* value_table */  
		};

		our_type = g_type_register_static (G_TYPE_OBJECT, "GtkSourceCompletion",
		                                   &our_info, 0);
	}

	return our_type;
}

/**
 * gtk_source_completion_new:
 * @view: a #GtkSourceView.
 *
 * Creates a new #GtkSourceCompletion asociated to a GtkSourceView
 *
 * Returns: value: A new #GtkSourceCompletion
 **/
GtkSourceCompletion*
gtk_source_completion_new (GtkTextView *view)
{

	GtkSourceCompletion *completion = completion_control_get_completion(view);
	if (completion !=NULL)
	{
		return completion;
	}

	completion = GTK_SOURCE_COMPLETION (g_object_new (GTK_TYPE_SOURCE_COMPLETION, NULL));
	completion->priv->text_view = view;

	completion_control_add_completion(view,completion);
	
	return completion;
}

/**
 * gtk_source_completion_register_provider:
 * @completion: the #GtkSourceCompletion
 * @provider: The #GtkSourceCompletionProvider.
 *
 * This function register the provider into the completion and reference it. When 
 * an event is raised, completion call to the provider to get the data. When the user
 * select an item, it call the provider to tell it this action and the provider do
 * that it want (normally inserts some text)
 * 
 **/
void
gtk_source_completion_register_provider(GtkSourceCompletion *completion,
					GtkSourceCompletionProvider *provider)
{
	completion->priv->providers = g_list_append(completion->priv->providers,provider);
	g_object_ref(provider);
}

/**
 * gtk_source_completion_unregister_provider:
 * @completion: the #GtkSourceCompletion
 * @provider: The #GtkSourceCompletionProvider.
 *
 * This function unregister the provider.
 * 
 **/
void
gtk_source_completion_unregister_provider(GtkSourceCompletion *completion,
					GtkSourceCompletionProvider *provider)
{
	g_return_if_fail(g_list_find(completion->priv->providers, provider) != NULL);	
	completion->priv->providers = g_list_remove(completion->priv->providers, provider);
	g_object_unref(provider);
}

/**
 * gtk_source_completion_get_view:
 * @completion: the #GtkSourceCompletion
 *
 * Returns: The internal #GtkTextView of this completion.
 * 
 **/
GtkTextView*
gtk_source_completion_get_view(GtkSourceCompletion *completion)
{
	return completion->priv->text_view;
}

/**
 * gtk_source_completion_raise_event:
 * @completion: the #GtkSourceCompletion
 * @event_name: The event name to raise
 * @event_data: This object will be passed to the providers to give them some special information of the event
 *
 * Calling this function, the completion call to all providers to get data and, if 
 * they return data, it shows the completion to the user. 
 * 
 **/
void
gtk_source_completion_raise_event(GtkSourceCompletion *completion, 
					const gchar *event_name, 
					gpointer event_data)
{
	GList* data_list;
	GList* original_list;
	GList* final_list = NULL;
	GList *providers_list;
	GtkSourceCompletionProvider *provider;
	GtkListStore *store;
	GtkTreeIter iter;
	InternalCompletionData *idata = NULL;
	
	store = GTK_LIST_STORE(gtk_tree_view_get_model(completion->priv->data_tree_view));
	clean_model_data(store);
	
	providers_list = completion->priv->providers;
	
	if (providers_list != NULL)
	{
		/*Getting the data...*/
		do
		{
			provider =  GTK_SOURCE_COMPLETION_PROVIDER(providers_list->data);
			data_list = gtk_source_completion_provider_get_data (
							provider, completion->priv->text_view, event_name, event_data);
			if (data_list != NULL)
			{
				original_list = data_list;
				do
				{
					idata = g_malloc0(sizeof(InternalCompletionData));
					idata->provider = provider;
					idata->data = (GtkSourceCompletionItem*)data_list->data;
					final_list = g_list_append(final_list, idata);
					//gtcp_add_data_to_tree(completion, GTK_SOURCE_COMPLETION_DATA(data_list->data), provider);
					
				}while((data_list = g_list_next(data_list)) != NULL);
				g_list_free(original_list);
			}
			
		}while((providers_list = g_list_next(providers_list)) != NULL);
		
		if (final_list!=NULL)
		{
			/*Order the data*/
			final_list = g_list_sort (final_list,internal_data_compare);
			data_list = final_list;
			/* Insert the data into the model */
			do
			{
				idata = (InternalCompletionData*)data_list->data;
				gtcp_add_data_to_tree(completion, idata->data,
									GTK_SOURCE_COMPLETION_PROVIDER(idata->provider));
				g_free(idata);
				
			}while((data_list = g_list_next(data_list)) != NULL);
			g_list_free(final_list);
			
			/* If there are not items, we don't show the popup */
			if (gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store),&iter))
			{
				show_completion_popup(completion);
			}
			else
			{
				if (gtk_source_completion_is_visible(completion))
				{
					end_completion (completion);
				}
			}
		}
		else
		{
			if (gtk_source_completion_is_visible(completion))
			{
				end_completion (completion);
			}
		}
	}
	else
	{
		if (gtk_source_completion_is_visible(completion))
		{
			end_completion (completion);
		}
	}
	
}


/**
 * gtk_source_completion_is_visible:
 * @completion: The #GtkSourceCompletion
 *
 * Returns TRUE if the completion popup is visible.
 *
 */
gboolean
gtk_source_completion_is_visible(GtkSourceCompletion *completion)
{
	return GTK_WIDGET_VISIBLE(completion->priv->window);
}

/**
 * gtk_source_completion_get_from_view:
 * @view: the GtkSourceView
 *
 * Returns NULL if the GtkTextView haven't got an associated GtkSourceCompletion
 * or the GtkSourceCompletion of the GtkTextView
 * 
 **/
GtkSourceCompletion*
gtk_source_completion_get_from_view(
																GtkTextView *view)
{
	return completion_control_get_completion(view);
}

/**
 * gtk_source_completion_has_provider:
 * @completion: The #GtkSourceCompletion
 * @provider_name: Provider's name that you are looking for.
 *
 * Returns TRUE if the completion has this provider registered or 
 * FALSE if not.
 *
 */
gboolean
gtk_source_completion_has_provider(GtkSourceCompletion *completion,
								const gchar* provider_name)
{
	GList *plist = completion->priv->providers;
	GtkSourceCompletionProvider *provider;	
	if (plist != NULL)
	{
		do
		{
			provider =  GTK_SOURCE_COMPLETION_PROVIDER(plist->data);
			if (strcmp(gtk_source_completion_provider_get_name(provider),provider_name)==0)
			{
				return TRUE;
			}
		}while((plist = g_list_next(plist)) != NULL);
	}

	return FALSE;
}

/**
 * gtk_source_completion_register_trigger:
 * @completion: The #GtkSourceCompletion
 * @trigger: The trigger to register
 *
 * This function register a completion trigger. If the completion is actived
 * then this method activate the trigger. This function reference the trigger
 * object
 */
void
gtk_source_completion_register_trigger(GtkSourceCompletion *completion,
								GtkSourceCompletionTrigger *trigger)
{
	completion->priv->triggers = g_list_append(completion->priv->triggers,trigger);
	g_object_ref(trigger);
	if (completion->priv->active)
	{
		gtk_source_completion_trigger_activate(trigger);
	}
}

/**
 * gtk_source_completion_unregister_trigger:
 * @completion: The #GtkSourceCompletion
 * @trigger: The trigger to unregister
 *
 * This function unregister a completion trigger. If the completion is actived
 * then this method deactivate the trigger. This function reference the trigger
 * object
 */								
void
gtk_source_completion_unregister_trigger(GtkSourceCompletion *completion,
								GtkSourceCompletionTrigger *trigger)
{
	g_return_if_fail(g_list_find(completion->priv->triggers, trigger) != NULL);	
	completion->priv->triggers = g_list_remove(completion->priv->triggers, trigger);
	if (completion->priv->active)
	{
		gtk_source_completion_trigger_deactivate(trigger);
	}
	g_object_unref(trigger);
}

/**
 * gtk_source_completion_get_trigger:
 * @completion: The #GtkSourceCompletion
 * @trigger_name: The trigger name to get
 *
 * This function return the trigger with this name.
 *
 * Returns The trigger or NULL if not exists
 *
 */
GtkSourceCompletionTrigger*
gtk_source_completion_get_trigger(GtkSourceCompletion *completion,
								const gchar* trigger_name)
{
	GList *plist = completion->priv->triggers;
	GtkSourceCompletionTrigger *trigger;	
	if (plist != NULL)
	{
		do
		{
			trigger =  GTK_SOURCE_COMPLETION_TRIGGER(plist->data);
			if (strcmp(gtk_source_completion_trigger_get_name(trigger),trigger_name)==0)
			{
				return trigger;
			}
		}while((plist = g_list_next(plist)) != NULL);
	}

	return FALSE;
}

/**
 * gtk_source_completion_activate:
 * @completion: The #GtkSourceCompletion
 *
 * This function activate the completion mechanism. The completion connects 
 * all signals and activate all registered triggers.
 */
void
gtk_source_completion_activate(GtkSourceCompletion *completion)
{
	g_debug("Activating GtkSourceCompletion");
	completion->priv->internal_signal_ids[IS_GTK_TEXT_VIEW_KP] = 
			g_signal_connect(completion->priv->text_view,
						"key-press-event",
						G_CALLBACK(view_key_press_event_cb),
						(gpointer) completion);
	g_debug("destroy connected"	);
	completion->priv->internal_signal_ids[IS_GTK_TEXT_DESTROY] = 
			g_signal_connect(completion->priv->text_view,
							"destroy",
							G_CALLBACK(view_destroy_event_cb),
							(gpointer)completion);
	completion->priv->internal_signal_ids[IS_GTK_TEXT_FOCUS_OUT] = 
			g_signal_connect(completion->priv->text_view,
							"focus-out-event",
							G_CALLBACK(view_focus_out_event_cb),
							(gpointer)completion);
	/* We activate the triggers*/
	GList *plist = completion->priv->triggers;
	GtkSourceCompletionTrigger *trigger;	
	if (plist != NULL)
	{
		do
		{
			trigger =  GTK_SOURCE_COMPLETION_TRIGGER(plist->data);
			gtk_source_completion_trigger_activate(trigger);

		}while((plist = g_list_next(plist)) != NULL);
	}	
	
	completion->priv->active = TRUE;
}

/**
 * gtk_source_completion_deactivate:
 * @completion: The #GtkSourceCompletion
 *
 * This function deactivate the completion mechanism. The completion disconnect
 * all signals and deactivate all registered triggers.
 */
void
gtk_source_completion_deactivate(GtkSourceCompletion *completion)
{
	g_debug("Deactivating GtkSourceCompletion");
	gint i;
	g_signal_handler_disconnect (completion->priv->data_tree_view,
				completion->priv->internal_signal_ids[IS_POPUP_ROW_ACTIVATE]);
	for (i=IS_POPUP_ROW_ACTIVATE;i<IS_LAST_SIGNAL;i++)
	{
		if (g_signal_handler_is_connected(completion->priv->text_view, completion->priv->internal_signal_ids[i]))
		{
			g_signal_handler_disconnect (completion->priv->text_view,
				completion->priv->internal_signal_ids[i]);
		}
		completion->priv->internal_signal_ids[i] = 0;
	}
	
	GList *plist = completion->priv->triggers;
	GtkSourceCompletionTrigger *trigger;	
	if (plist != NULL)
	{
		do
		{
			trigger =  GTK_SOURCE_COMPLETION_TRIGGER(plist->data);
			gtk_source_completion_trigger_deactivate(trigger);

		}while((plist = g_list_next(plist)) != NULL);
	}	
	
	completion->priv->active = FALSE;
}


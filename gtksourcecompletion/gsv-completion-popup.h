/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsv-completion-popup.h
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

#ifndef GSV_COMPLETION_POPUP_H
#define GSV_COMPLETION_POPUP_H

#include <gtk/gtk.h>
#include "gtksourcecompletion-item.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GSV_TYPE_COMPLETION_POPUP              (gsv_completion_popup_get_type())
#define GSV_COMPLETION_POPUP(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GSV_TYPE_COMPLETION_POPUP, GsvCompletionPopup))
#define GSV_COMPLETION_POPUP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GSV_TYPE_COMPLETION_POPUP, GsvCompletionPopupClass))
#define GSV_IS_COMPLETION_POPUP(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GSV_TYPE_COMPLETION_POPUP))
#define GSV_IS_COMPLETION_POPUP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GSV_TYPE_COMPLETION_POPUP))
#define GSV_COMPLETION_POPUP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GSV_TYPE_COMPLETION_POPUP, GsvCompletionPopupClass))

#define DEFAULT_PAGE "Default"

typedef struct _GsvCompletionPopupPriv GsvCompletionPopupPriv;
typedef struct _GsvCompletionPopup GsvCompletionPopup;
typedef struct _GsvCompletionPopupClass GsvCompletionPopupClass;

struct _GsvCompletionPopupClass
{
  GtkWindowClass parent_class;
  
  void	 (* item_selected)(GsvCompletionPopup *popup,
				     	 GtkSourceCompletionItem *item);

};

struct _GsvCompletionPopup
{
  GtkWindow parent;
  
  GsvCompletionPopupPriv *priv;

};

GType gsv_completion_popup_get_type (void) G_GNUC_CONST;

/**
 * gsv_completion_popup_select_first:
 * @self: The #GsvCompletionPopup
 *
 * See #gsv_completion_tree_select_first
 *
 * Returns
 */
gboolean
gsv_completion_popup_select_first(GsvCompletionPopup *self);

/**
 * gsv_completion_popup_select_last:
 * @self: The #GsvCompletionPopup
 *
 * See #gsv_completion_tree_select_last
 *
 * Returns
 */
gboolean 
gsv_completion_popup_select_last(GsvCompletionPopup *self);

/**
 * gsv_completion_popup_select_previous:
 * @self: The #GsvCompletionPopup
 *
 * See #gsv_completion_tree_select_previous
 *
 * Returns
 */
gboolean
gsv_completion_popup_select_previous(GsvCompletionPopup *self, 
					gint rows);

/**
 * gsv_completion_popup_select_next:
 * @self: The #GsvCompletionPopup
 *
 * See #gsv_completion_tree_select_next
 *
 * Returns
 */
gboolean
gsv_completion_popup_select_next(GsvCompletionPopup *self, 
					gint rows);

/**
 * gsv_completion_popup_get_selected_item:
 * @self: The #GsvCompletionPopup
 *
 * See #gsv_completion_tree_select_item. Not free the item!
 *
 * Returns
 */
gboolean
gsv_completion_popup_get_selected_item(GsvCompletionPopup *self,
					GtkSourceCompletionItem **item);

/**
 * gsv_completion_popup_new:
 * @view: The #GtkTextView where the popup gets and put the completion.
 *
 * Returns The new #GsvCompletionPopup
 */
GtkWidget*
gsv_completion_popup_new (GtkTextView *view);

/**
 * gsv_completion_popup_clear:
 * @self: The #GsvCompletionPopup
 *
 * Clear all items in all pages. It frees all completion items.
 *
 */
void
gsv_completion_popup_clear(GsvCompletionPopup *self);

/**
 * gsv_completion_popup_add_data:
 * @self: The #GsvCompletionPopup
 * @data: The #GtkSourceCompletionItem to add.
 *
 * The popup frees the item when it will be cleaned.
 *
 */
void
gsv_completion_popup_add_data(GsvCompletionPopup *self,
					GtkSourceCompletionItem* data);

/**
 * gsv_completion_popup_has_items:
 * @self: The #GsvCompletionPopup
 *
 * Returns TRUE if the popup has almost one element.
 */
gboolean
gsv_completion_popup_has_items(GsvCompletionPopup *self);

/**
 * gsv_completion_popup_toggle_item_info:
 * @self: The #GsvCompletionPopup
 *
 * This toggle the state of the info dialog. If the info is visible
 * then it hide the info dialog. If the dialog is hidden then it 
 * shows the info dialog.
 *
 */
void
gsv_completion_popup_toggle_item_info(GsvCompletionPopup *self);

/**
 * gsv_completion_popup_refresh:
 * @self: The #GsvCompletionPopup
 *
 * Really only show the completion by now.
 *
 */
void
gsv_completion_popup_refresh(GsvCompletionPopup *self);

/**
 * gsv_completion_popup_page_next:
 * @self: The #GsvCompletionPopup
 *
 * Shows the next completion page. If it is the last page then 
 * shows the first one.
 *
 */
void
gsv_completion_popup_page_next(GsvCompletionPopup *self);

/**
 * gsv_completion_popup_page_previous:
 * @self: The #GsvCompletionPopup
 *
 * Shows the previous completion page. If it is the first page then
 * shows the last one.
 *
 */
void
gsv_completion_popup_page_previous(GsvCompletionPopup *self);

G_END_DECLS

#endif 

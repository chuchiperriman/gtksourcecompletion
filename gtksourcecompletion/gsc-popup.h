/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-popup.h
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

#ifndef GSC_POPUP_H
#define GSC_POPUP_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

typedef enum{
	GSC_POPUP_POSITION_CURSOR,
	GSC_POPUP_POSITION_CENTER_SCREEN
}GscPopupPositionType;

typedef enum{
	GSC_POPUP_FILTER_NONE,
	GSC_POPUP_FILTER_TREE
}GscPopupFilterType;

typedef struct{
	GscPopupPositionType position_type;
	GscPopupFilterType filter_type;
}GscPopupOptions;

#include "gsc-proposal.h"


/*
 * Type checking and casting macros
 */
#define GSC_TYPE_POPUP              (gsc_popup_get_type())
#define GSC_POPUP(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GSC_TYPE_POPUP, GscPopup))
#define GSC_POPUP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GSC_TYPE_POPUP, GscPopupClass))
#define GSC_IS_POPUP(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GSC_TYPE_POPUP))
#define GSC_IS_POPUP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_POPUP))
#define GSC_POPUP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GSC_TYPE_POPUP, GscPopupClass))

#define DEFAULT_PAGE "Default"

typedef struct _GscPopupPriv GscPopupPriv;
typedef struct _GscPopup GscPopup;
typedef struct _GscPopupClass GscPopupClass;

struct _GscPopupClass
{
  GtkWindowClass parent_class;
  
  void	 (* proposal_selected)(GscPopup *popup,
			   GscProposal *proposal);
};

struct _GscPopup
{
  GtkWindow parent;
  
  GscPopupPriv *priv;

};

GType gsc_popup_get_type (void) G_GNUC_CONST;

/**
 * gsc_popup_select_first:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_first
 *
 * Returns
 */
gboolean
gsc_popup_select_first(GscPopup *self);

/**
 * gsc_popup_select_last:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_last
 *
 * Returns
 */
gboolean 
gsc_popup_select_last(GscPopup *self);

/**
 * gsc_popup_select_previous:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_previous
 *
 * Returns
 */
gboolean
gsc_popup_select_previous(GscPopup *self, 
				     gint rows);

/**
 * gsc_popup_select_next:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_next
 *
 * Returns
 */
gboolean
gsc_popup_select_next(GscPopup *self, 
				 gint rows);

/**
 * gsc_popup_get_selected_proposal:
 * @self: The #GscPopup
 *
 * See #gsc_tree_select_proposal. Not free the proposal!
 *
 * Returns
 */
gboolean
gsc_popup_get_selected_proposal(GscPopup *self,
					GscProposal **proposal);

/**
 * gsc_popup_new:
 * @view: The #GtkTextView where the popup gets and put the completion.
 *
 * Returns The new #GscPopup
 */
GtkWidget*
gsc_popup_new (GtkTextView *view);

/**
 * gsc_popup_clear:
 * @self: The #GscPopup
 *
 * Clear all proposals in all pages. It frees all completion proposals.
 *
 */
void
gsc_popup_clear(GscPopup *self);

/**
 * gsc_popup_add_data:
 * @self: The #GscPopup
 * @data: The #GscProposal to add.
 *
 * The popup frees the proposal when it will be cleaned.
 *
 */
void
gsc_popup_add_data(GscPopup *self,
			      GscProposal* data);

/**
 * gsc_popup_has_proposals:
 * @self: The #GscPopup
 *
 * Returns TRUE if the popup has almost one element.
 */
gboolean
gsc_popup_has_proposals(GscPopup *self);

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
gsc_popup_toggle_proposal_info(GscPopup *self);

/**
 * gsc_popup_refresh:
 * @self: The #GscPopup
 *
 * Show the completion popup with the default the options.
 *
 */
void
gsc_popup_refresh(GscPopup *self);

/**
 * gsc_popup_refresh_with_opts:
 * @self: The #GscPopup
 * @options: Options to tell the popup how it must to be shown.
 *
 * Show the completion popup with the current the options.
 */
void
gsc_popup_refresh_with_opts(GscPopup *self,
					      GscPopupOptions *options);

/**
 * gsc_popup_page_next:
 * @self: The #GscPopup
 *
 * Shows the next completion page. If it is the last page then 
 * shows the first one.
 *
 */
void
gsc_popup_page_next(GscPopup *self);

/**
 * gsc_popup_page_previous:
 * @self: The #GscPopup
 *
 * Shows the previous completion page. If it is the first page then
 * shows the last one.
 *
 */
void
gsc_popup_page_previous(GscPopup *self);

/**
 * gsc_popup_set_current_info:
 * @self: The #GscPopup
 * @info: Markup with the info to be shown into the info window
 */
void
gsc_popup_set_current_info(GscPopup *self,
					     gchar *info);

/**
 * gsc_popup_get_num_active_pags:
 * @self: The #GscPopup
 *
 * Returns The number of active pages
 */
gint
gsc_popup_get_num_active_pages(GscPopup *self);

G_END_DECLS

#endif 

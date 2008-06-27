/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-popup.h
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

#ifndef GTK_SOURCE_COMPLETION_POPUP_H
#define GTK_SOURCE_COMPLETION_POPUP_H

#include <gtk/gtk.h>
#include "gtksourcecompletion-proposal.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GTK_TYPE_SOURCE_COMPLETION_POPUP              (gtk_source_completion_popup_get_type())
#define GTK_SOURCE_COMPLETION_POPUP(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GTK_TYPE_SOURCE_COMPLETION_POPUP, GtkSourceCompletionPopup))
#define GTK_SOURCE_COMPLETION_POPUP_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GTK_TYPE_SOURCE_COMPLETION_POPUP, GtkSourceCompletionPopupClass))
#define GTK_IS_SOURCE_COMPLETION_POPUP(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GTK_TYPE_SOURCE_COMPLETION_POPUP))
#define GTK_IS_SOURCE_COMPLETION_POPUP_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_SOURCE_COMPLETION_POPUP))
#define GTK_SOURCE_COMPLETION_POPUP_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GTK_TYPE_SOURCE_COMPLETION_POPUP, GtkSourceCompletionPopupClass))

#define DEFAULT_PAGE "Default"

typedef struct _GtkSourceCompletionPopupPriv GtkSourceCompletionPopupPriv;
typedef struct _GtkSourceCompletionPopup GtkSourceCompletionPopup;
typedef struct _GtkSourceCompletionPopupClass GtkSourceCompletionPopupClass;

struct _GtkSourceCompletionPopupClass
{
  GtkWindowClass parent_class;
  
  void	 (* proposal_selected)(GtkSourceCompletionPopup *popup,
			   GtkSourceCompletionProposal *proposal);
};

struct _GtkSourceCompletionPopup
{
  GtkWindow parent;
  
  GtkSourceCompletionPopupPriv *priv;

};

GType gtk_source_completion_popup_get_type (void) G_GNUC_CONST;

/**
 * gtk_source_completion_popup_select_first:
 * @self: The #GtkSourceCompletionPopup
 *
 * See #gtk_source_completion_tree_select_first
 *
 * Returns
 */
gboolean
gtk_source_completion_popup_select_first(GtkSourceCompletionPopup *self);

/**
 * gtk_source_completion_popup_select_last:
 * @self: The #GtkSourceCompletionPopup
 *
 * See #gtk_source_completion_tree_select_last
 *
 * Returns
 */
gboolean 
gtk_source_completion_popup_select_last(GtkSourceCompletionPopup *self);

/**
 * gtk_source_completion_popup_select_previous:
 * @self: The #GtkSourceCompletionPopup
 *
 * See #gtk_source_completion_tree_select_previous
 *
 * Returns
 */
gboolean
gtk_source_completion_popup_select_previous(GtkSourceCompletionPopup *self, 
				     gint rows);

/**
 * gtk_source_completion_popup_select_next:
 * @self: The #GtkSourceCompletionPopup
 *
 * See #gtk_source_completion_tree_select_next
 *
 * Returns
 */
gboolean
gtk_source_completion_popup_select_next(GtkSourceCompletionPopup *self, 
				 gint rows);

/**
 * gtk_source_completion_popup_get_selected_proposal:
 * @self: The #GtkSourceCompletionPopup
 *
 * See #gtk_source_completion_tree_select_proposal. Not free the proposal!
 *
 * Returns
 */
gboolean
gtk_source_completion_popup_get_selected_proposal(GtkSourceCompletionPopup *self,
					GtkSourceCompletionProposal **proposal);

/**
 * gtk_source_completion_popup_new:
 * @view: The #GtkTextView where the popup gets and put the completion.
 *
 * Returns The new #GtkSourceCompletionPopup
 */
GtkWidget*
gtk_source_completion_popup_new (GtkTextView *view);

/**
 * gtk_source_completion_popup_clear:
 * @self: The #GtkSourceCompletionPopup
 *
 * Clear all proposals in all pages. It frees all completion proposals.
 *
 */
void
gtk_source_completion_popup_clear(GtkSourceCompletionPopup *self);

/**
 * gtk_source_completion_popup_add_data:
 * @self: The #GtkSourceCompletionPopup
 * @data: The #GtkSourceCompletionProposal to add.
 *
 * The popup frees the proposal when it will be cleaned.
 *
 */
void
gtk_source_completion_popup_add_data(GtkSourceCompletionPopup *self,
			      GtkSourceCompletionProposal* data);

/**
 * gtk_source_completion_popup_has_proposals:
 * @self: The #GtkSourceCompletionPopup
 *
 * Returns TRUE if the popup has almost one element.
 */
gboolean
gtk_source_completion_popup_has_proposals(GtkSourceCompletionPopup *self);

/**
 * gtk_source_completion_popup_toggle_proposal_info:
 * @self: The #GtkSourceCompletionPopup
 *
 * This toggle the state of the info dialog. If the info is visible
 * then it hide the info dialog. If the dialog is hidden then it 
 * shows the info dialog.
 *
 */
void
gtk_source_completion_popup_toggle_proposal_info(GtkSourceCompletionPopup *self);

/**
 * gtk_source_completion_popup_refresh:
 * @self: The #GtkSourceCompletionPopup
 *
 * Really only show the completion by now.
 *
 */
void
gtk_source_completion_popup_refresh(GtkSourceCompletionPopup *self);

/**
 * gtk_source_completion_popup_page_next:
 * @self: The #GtkSourceCompletionPopup
 *
 * Shows the next completion page. If it is the last page then 
 * shows the first one.
 *
 */
void
gtk_source_completion_popup_page_next(GtkSourceCompletionPopup *self);

/**
 * gtk_source_completion_popup_page_previous:
 * @self: The #GtkSourceCompletionPopup
 *
 * Shows the previous completion page. If it is the first page then
 * shows the last one.
 *
 */
void
gtk_source_completion_popup_page_previous(GtkSourceCompletionPopup *self);

/**
 * gtk_source_completion_popup_set_current_info:
 * @self: The #GtkSourceCompletionPopup
 * @info: Markup with the info to be shown into the info window
 */
void
gtk_source_completion_popup_set_current_info(GtkSourceCompletionPopup *self,
					     gchar *info);

/**
 * gtk_source_completion_popup_get_num_active_pags:
 * @self: The #GtkSourceCompletionPopup
 *
 * Returns The number of active pages
 */
gint
gtk_source_completion_popup_get_num_active_pages(GtkSourceCompletionPopup *self);

G_END_DECLS

#endif 

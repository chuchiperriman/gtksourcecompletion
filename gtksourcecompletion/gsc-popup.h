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
#include "gsc-tree.h"
#include "gsc-proposal.h"

G_BEGIN_DECLS

/*
 * FIXME: Should be called something like GscPopupKeysType
 */
typedef enum {
	KEYS_INFO,
	KEYS_PAGE_NEXT,
	KEYS_PAGE_PREV,
	KEYS_LAST
}KeysType;

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

typedef GscTreeFilterVisibleFunc GscPopupFilterVisibleFunc;

typedef struct _GscPopupPriv GscPopupPriv;
typedef struct _GscPopup GscPopup;
typedef struct _GscPopupClass GscPopupClass;

struct _GscPopup
{
	GtkWindow parent;

	GscPopupPriv *priv;
};

struct _GscPopupClass
{
	GtkWindowClass parent_class;

	void     (* proposal_selected)(GscPopup *popup, GscProposal *proposal);
	gboolean (*display_info)      (GscPopup *popup, GscProposal *proposal);
};

GType		 gsc_popup_get_type			(void) G_GNUC_CONST;

GtkWidget	*gsc_popup_new				(void);

/*FIXME Remove*/
void		 gsc_popup_add_data			(GscPopup *self,
							 GscProposal *data);

/*FIXME Remove or private*/
void		 gsc_popup_clear			(GscPopup *self);

/*FIXME Remove or private*/
gboolean	 gsc_popup_select_first			(GscPopup *self);

/*FIXME Remove or private*/
gboolean	 gsc_popup_select_last			(GscPopup *self);

/*FIXME Remove or private*/
gboolean	 gsc_popup_select_previous		(GscPopup *self, 
							 gint rows);

/*FIXME Remove or private*/
gboolean	 gsc_popup_select_next			(GscPopup *self, 
							 gint rows);

gboolean	 gsc_popup_get_selected_proposal	(GscPopup *self,
							 GscProposal **proposal);

/*FIXME Remove or private*/
gboolean	 gsc_popup_select_current_proposal	(GscPopup *self);

/*FIXME Remove or private*/
gboolean	 gsc_popup_has_proposals		(GscPopup *self);

/*FIXME Remove or private*/
void		 gsc_popup_toggle_proposal_info		(GscPopup *self);

/*FIXME Remove or private*/
void		 gsc_popup_page_next			(GscPopup *self);
/*FIXME Remove or private*/
void		 gsc_popup_page_previous		(GscPopup *self);
/*FIXME Remove (you can get the info and set the text)*/
void		 gsc_popup_set_current_info		(GscPopup *self,
							 const gchar *info);
/*FIXME Remove*/
gint		 gsc_popup_get_num_active_pages		(GscPopup *self);
/*FIXME Remove*/
void		 gsc_popup_show_or_update		(GtkWidget *widget);

/*FIXME Remove*/
void		 gsc_popup_bottom_bar_set_visible	(GscPopup *self,
							 gboolean visible);
/*FIXME Remove*/
gboolean	 gsc_popup_bottom_bar_get_visible	(GscPopup *self);

/*FIXME Add new get_bottom_bar*/

gboolean	 gsc_popup_autoselect			(GscPopup *self);

/*FIXME Change to set_filter_visible_func*/
gboolean	 gsc_popup_filter_visible		(GscPopup *self,
							 GscPopupFilterVisibleFunc func,
							 gpointer user_data);

G_END_DECLS

#endif 

/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-completion-priv.h
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

#ifndef GSC_COMPLETION_PRIV_H
#define GSC_COMPLETION_PRIV_H

G_BEGIN_DECLS

/* External signals */
enum
{
	TEXT_VIEW_KP,
	TEXT_VIEW_DESTROY,
	TEXT_VIEW_FOCUS_OUT,
	TEXT_VIEW_BUTTON_PRESS,
	LAST_EXTERNAL_SIGNAL
};

typedef struct _GscTree
{
	GtkScrolledWindow *scroll;
	GtkTreeView *treeview;
	GtkListStore *list_store;
	GtkTreeModelFilter *model_filter;
	
	gboolean filter_active;
	gpointer filter_data;
	GscCompletionFilterFunc filter_func;
} GscTree;

typedef struct _GscCompletionPage
{
	gchar *name;
	GscTree *tree;
} GscCompletionPage;

struct _GscCompletionPriv
{
	/* Widget and popup variables*/
	GtkWidget *info_window;
	GtkWidget *info_button;
	GtkWidget *notebook;
	GtkWidget *tab_label;
	GtkWidget *next_page_button;
	GtkWidget *prev_page_button;
	GtkWidget *bottom_bar;
	
	GList *pages;
	GscCompletionPage *active_page;
	gboolean destroy_has_run;
	gboolean manage_keys;
	gboolean remember_info_visibility;
	gboolean info_visible;
	gboolean select_on_show;
	
	/* Completion management */
	GtkTextView *view;
	GList *triggers;
	GList *prov_trig;
	GscTrigger *active_trigger;
	
	/*TRUE if the completion mechanism is active*/
	gboolean active;
	gulong signals_ids[LAST_EXTERNAL_SIGNAL];
	
};

void		_gsc_completion_update_info_pos		(GscCompletion *self);
gboolean	_gsc_completion_select_next 		(GscCompletion *self,
							 gint rows);
gboolean	_gsc_completion_select_previous 	(GscCompletion *self,
						 	 gint rows);
gboolean 	_gsc_completion_select_last		(GscCompletion *self);
gboolean	_gsc_completion_select_first		(GscCompletion *self);
gboolean	_gsc_completion_select_current_proposal	(GscCompletion *self);


G_END_DECLS

#endif 

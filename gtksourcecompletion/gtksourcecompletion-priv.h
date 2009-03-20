/*
 * gtksourcecompletion-priv.h
 * This file is part of gtksourcecompletion
 *
 * Copyright (C) 2007 -2009 Jesús Barbero Rodríguez <chuchiperriman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA.
 */

#ifndef GTK_SOURCE_COMPLETION_PRIV_H
#define GTK_SOURCE_COMPLETION_PRIV_H

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

typedef struct _GtkSourceCompletionTree
{
	GtkTreeView *treeview;
	GtkListStore *list_store;
	GtkTreeModelFilter *model_filter;
	
	gboolean filter_active;
	gpointer filter_data;
	GtkSourceCompletionFilterFunc filter_func;
} GtkSourceCompletionTree;

typedef struct _GtkSourceCompletionPage
{
	gchar *name;
	GtkSourceCompletionTree *tree;
} GtkSourceCompletionPage;

struct _GtkSourceCompletionPriv
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
	GtkSourceCompletionPage *active_page;
	gboolean destroy_has_run;
	gboolean manage_keys;
	gboolean remember_info_visibility;
	gboolean info_visible;
	gboolean select_on_show;
	
	/* Completion management */
	GtkTextView *view;
	GList *triggers;
	GList *prov_trig;
	GtkSourceCompletionTrigger *active_trigger;
	
	/*TRUE if the completion mechanism is active*/
	gboolean active;
	gulong signals_ids[LAST_EXTERNAL_SIGNAL];
	
};

void		_gtk_source_completion_update_info_pos		(GtkSourceCompletion *self);
gboolean	_gtk_source_completion_select_next 		(GtkSourceCompletion *self,
								 gint rows);
gboolean	_gtk_source_completion_select_previous		(GtkSourceCompletion *self,
							 	 gint rows);
gboolean 	_gtk_source_completion_select_last		(GtkSourceCompletion *self);
gboolean	_gtk_source_completion_select_first		(GtkSourceCompletion *self);
gboolean	_gtk_source_completion_select_current_proposal	(GtkSourceCompletion *self);


G_END_DECLS

#endif 

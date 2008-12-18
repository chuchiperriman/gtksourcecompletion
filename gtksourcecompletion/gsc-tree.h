/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-tree.h
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
 
#ifndef GSC_TREE_H
#define GSC_TREE_H

#include <gtk/gtk.h>
#include "gsc-proposal.h"

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GSC_TYPE_TREE              (gsc_tree_get_type())
#define GSC_TREE(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GSC_TYPE_TREE, GscTree))
#define GSC_TREE_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GSC_TYPE_TREE, GscTreeClass))
#define GSC_IS_TREE(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GSC_TYPE_TREE))
#define GSC_IS_TREE_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_TREE))
#define GSC_TREE_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GSC_TYPE_TREE, GscTreeClass))

typedef struct _GscTreePriv GscTreePriv;

typedef struct _GscTree GscTree;

struct _GscTree
{
	GtkTreeView parent;
	
	GscTreePriv *priv;
};

typedef struct _GscTreeClass GscTreeClass;

struct _GscTreeClass
{
	GtkTreeViewClass parent_class;
	
	void (* proposal_selected)(GscTree *tree,
			       GscProposal *proposal);
	void (* selection_changed)(GscTree *tree,
				   GscProposal *proposal);
};

GType		 gsc_tree_get_type			(void) G_GNUC_CONST;

GtkWidget	*gsc_tree_new				(void);

gboolean	 gsc_tree_get_selected_proposal		(GscTree *self,
							 GscProposal **proposal);

void		 gsc_tree_add_data			(GscTree *self,
							 GscProposal* data);

void		 gsc_tree_clear				(GscTree *self);

gboolean	 gsc_tree_select_first			(GscTree *self);

gboolean	 gsc_tree_select_last			(GscTree *self);

gboolean	 gsc_tree_select_previous		(GscTree *self, 
							 gint rows);

gboolean	 gsc_tree_select_next			(GscTree *self, 
							 gint rows);

gint		 gsc_tree_get_num_proposals		(GscTree *self);

void		 gsc_tree_filter			(GscTree *self,
							 const gchar* filter);

G_END_DECLS
#endif

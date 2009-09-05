/*
 * gscmodel.h
 * This file is part of gsc
 *
 * Copyright (C) 2009 - Jesse van den Kieboom
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef __GSC_COMPLETION_MODEL_H__
#define __GSC_COMPLETION_MODEL_H__

#include <gtk/gtk.h>
#include "gsc-proposal.h"

G_BEGIN_DECLS

#define GTK_TYPE_SOURCE_COMPLETION_MODEL		(gsc_completion_model_get_type ())
#define GSC_COMPLETION_MODEL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_SOURCE_COMPLETION_MODEL, GscModel))
#define GSC_COMPLETION_MODEL_CONST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_SOURCE_COMPLETION_MODEL, GscModel const))
#define GSC_COMPLETION_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_SOURCE_COMPLETION_MODEL, GscModelClass))
#define GTK_IS_SOURCE_COMPLETION_MODEL(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_SOURCE_COMPLETION_MODEL))
#define GTK_IS_SOURCE_COMPLETION_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_SOURCE_COMPLETION_MODEL))
#define GSC_COMPLETION_MODEL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_SOURCE_COMPLETION_MODEL, GscModelClass))

typedef struct _GscModel	GscModel;
typedef struct _GscModelClass	GscModelClass;
typedef struct _GscModelPrivate	GscModelPrivate;

#include "gsc-provider.h"

struct _GscModel {
	GObject parent;
	
	GscModelPrivate *priv;
};

struct _GscModelClass {
	GObjectClass parent_class;
	
	void	(*items_added)		(GscModel *model);
	void	(*filter_done)		(GscModel *model);
};

enum
{
	GSC_COMPLETION_MODEL_COLUMN_LABEL,
	GSC_COMPLETION_MODEL_COLUMN_MARKUP,
	GSC_COMPLETION_MODEL_COLUMN_ICON,
	GSC_COMPLETION_MODEL_COLUMN_PROPOSAL,
	GSC_COMPLETION_MODEL_COLUMN_PROVIDER,
	GSC_COMPLETION_MODEL_N_COLUMNS
};

GType gsc_completion_model_get_type (void) G_GNUC_CONST;

GscModel *
		gsc_completion_model_new 	(void);

void		gsc_completion_model_run_add_proposals (GscModel *model);

void		gsc_completion_model_append 	(GscModel           *model,
							 GscProvider        *provider,
							 GscProposal        *proposal);

void		gsc_completion_model_set_proposals (GscModel	    *model,
							   GscProvider 	    *provider,
							   GList		       	    *proposals);

gboolean	gsc_completion_model_is_empty 	(GscModel           *model,
                                                         gboolean                            invisible);

guint		gsc_completion_model_n_proposals (GscModel           *model,
                                                         GscProvider        *provider);

void 		gsc_completion_model_clear 	(GscModel           *model);

gboolean 	gsc_completion_model_iter_previous (GscModel         *model,
							   GtkTreeIter                      *iter);
							 
gboolean 	gsc_completion_model_iter_last 	(GscModel           *model,
							 GtkTreeIter                        *iter);

void		gsc_completion_model_cancel_add_proposals (GscModel  *model);

G_END_DECLS

#endif /* __GSC_COMPLETION_MODEL_H__ */


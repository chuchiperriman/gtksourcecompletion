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

#ifndef __GSC_MODEL_H__
#define __GSC_MODEL_H__

#include <gtk/gtk.h>
#include "gsc-proposal.h"

G_BEGIN_DECLS

#define GSC_TYPE_MODEL		(gsc_model_get_type ())
#define GSC_MODEL(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_MODEL, GscModel))
#define GSC_MODEL_CONST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_MODEL, GscModel const))
#define GSC_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_MODEL, GscModelClass))
#define GSC_IS_MODEL(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_MODEL))
#define GSC_IS_MODEL_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_MODEL))
#define GSC_MODEL_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_MODEL, GscModelClass))

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
	GSC_MODEL_COLUMN_LABEL,
	GSC_MODEL_COLUMN_MARKUP,
	GSC_MODEL_COLUMN_ICON,
	GSC_MODEL_COLUMN_PROPOSAL,
	GSC_MODEL_COLUMN_PROVIDER,
	GSC_MODEL_N_COLUMNS
};

GType gsc_model_get_type (void) G_GNUC_CONST;

GscModel *
		gsc_model_new 	(void);

void		gsc_model_run_add_proposals (GscModel *model);

void		gsc_model_append 	(GscModel           *model,
							 GscProvider        *provider,
							 GscProposal        *proposal);

void		gsc_model_set_proposals (GscModel	    *model,
							   GscProvider 	    *provider,
							   GList		       	    *proposals);

gboolean	gsc_model_is_empty 	(GscModel           *model,
                                                         gboolean                            invisible);

guint		gsc_model_n_proposals (GscModel           *model,
                                                         GscProvider        *provider);

void 		gsc_model_clear 	(GscModel           *model);

gboolean 	gsc_model_iter_previous (GscModel         *model,
							   GtkTreeIter                      *iter);
							 
gboolean 	gsc_model_iter_last 	(GscModel           *model,
							 GtkTreeIter                        *iter);

void		gsc_model_cancel_add_proposals (GscModel  *model);

gboolean	gsc_model_iter_is_header			(GscModel	*model,
								 GtkTreeIter	*iter);

G_END_DECLS

#endif /* __GSC_MODEL_H__ */


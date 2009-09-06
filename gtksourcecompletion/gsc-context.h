/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*- 
 * gsccontext.h
 * 
 * Copyright  (C)  2009  Jesús Barbero Rodríguez <chuchiperriman@gmail.com>
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA
 * 
 */

#ifndef GSC_COMPLETION_CONTEXT_H
#define GSC_COMPLETION_CONTEXT_H

#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GSC_TYPE_CONTEXT                              \
   (gsc_context_get_type())
#define GSC_COMPLETION_CONTEXT(obj)                              \
   (G_TYPE_CHECK_INSTANCE_CAST ((obj),                                  \
                                GSC_TYPE_CONTEXT,     \
                                GscContext))
#define GSC_COMPLETION_CONTEXT_CLASS(klass)                      \
   (G_TYPE_CHECK_CLASS_CAST ((klass),                                   \
                             GSC_TYPE_CONTEXT,        \
                             GscContextClass))
#define GTK_IS_CONTEXT(obj)                           \
   (G_TYPE_CHECK_INSTANCE_TYPE ((obj),                                  \
                                GSC_TYPE_CONTEXT))
#define GTK_IS_CONTEXT_CLASS(klass)                   \
   (G_TYPE_CHECK_CLASS_TYPE ((klass),                                   \
                             GSC_TYPE_CONTEXT))
#define GSC_COMPLETION_CONTEXT_GET_CLASS(obj)                    \
   (G_TYPE_INSTANCE_GET_CLASS ((obj),                                   \
                               GSC_TYPE_CONTEXT,      \
                               GscContextClass))

typedef struct _GscContextPrivate GscContextPrivate;
typedef struct _GscContext      GscContext;
typedef struct _GscContextClass GscContextClass;

#include "gsc-model.h"

struct _GscContextClass
{
	GObjectClass parent_class;
};

struct _GscContext
{
	GObject parent;
	GscContextPrivate *priv;
};

GType	 			 gsc_context_get_type		(void) G_GNUC_CONST;

GscContext	*gsc_context_new		(GscModel	*model,
										 GtkTextView			*view,
										 GList				*providers);

void				 gsc_context_add_proposals	(GscContext	*context,
										 GscProvider	*provider,
										 GList	 			*proposals);

void				 gsc_context_finish		(GscContext	*context);

GtkTextView			*gsc_context_get_view		(GscContext	*context);

void				 gsc_context_get_iter		(GscContext	*context,
										 GtkTextIter 			*iter);

gchar				*gsc_context_get_criteria	(GscContext	*context);

GList				*gsc_context_get_proposals	(GscContext	*context,
										 GscProvider	*provider);

GList				*gsc_context_get_providers	(GscContext 	*context);

gboolean			 gsc_context_is_valid		(GscContext	*context);

void				 gsc_context_update		(GscContext	*context);

void				 gsc_context_set_filter_provider(GscContext	*context,
										   GscProvider	*provider);

GscProvider	*gsc_context_get_filter_provider(GscContext *context);

G_END_DECLS

#endif

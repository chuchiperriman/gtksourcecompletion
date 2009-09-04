/*
 * gtksourcecompletion.h
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
 
#ifndef GSC_COMPLETION_H
#define GSC_COMPLETION_H

#include <gtk/gtk.h>
#include <gtksourceview/gtksourcecompletioninfo.h>
#include <gtksourceview/gtksourcecompletionprovider.h>

G_BEGIN_DECLS

/*
 * Type checking and casting macros
 */
#define GSC_TYPE_SOURCE_COMPLETION              (gsc_completion_get_type())
#define GSC_COMPLETION(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GSC_TYPE_SOURCE_COMPLETION, Gsc))
#define GSC_COMPLETION_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GSC_TYPE_SOURCE_COMPLETION, GscClass))
#define GSC_IS_SOURCE_COMPLETION(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GSC_TYPE_SOURCE_COMPLETION))
#define GSC_IS_SOURCE_COMPLETION_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_SOURCE_COMPLETION))
#define GSC_COMPLETION_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GSC_TYPE_SOURCE_COMPLETION, GscClass))

#define GSC_COMPLETION_ERROR		(gsc_completion_error_quark ())

typedef struct _GscPrivate GscPrivate;
typedef struct _GscCompletion Gsc;
typedef struct _GscClass GscClass;

typedef enum
{
	GSC_COMPLETION_ERROR_ALREADY_BOUND = 0,
	GSC_COMPLETION_ERROR_NOT_BOUND,
} GscError;

/* Forward declaration of GtkSourceView */
struct _GtkSourceView;

struct _Gsc
{
	GObject parent;

	GscPrivate *priv;
};

struct _GscClass
{
	GObjectClass parent_class;

	gboolean 	(* proposal_activated)		(GscCompletion         *completion,
	                                                 GscProvider *provider,
							 GscProposal *proposal);
	void 		(* show)			(GscCompletion         *completion);
	void		(* hide)			(GscCompletion         *completion);
};

GType		 gsc_completion_get_type			(void) G_GNUC_CONST;

GQuark		 gsc_completion_error_quark		(void);

gboolean	 gsc_completion_add_provider		(GscCompletion          *completion,
								 GscProvider  *provider,
								 GError                      **error);

gboolean	 gsc_completion_remove_provider		(GscCompletion          *completion,
								 GscProvider  *provider,
								 GError                      **error);

GList		*gsc_completion_get_providers		(GscCompletion         *completion,
                                                                 const gchar                 *capabilities);
gboolean	 gsc_completion_show			(GscCompletion         *completion,
								 GList                       *providers,
								 GtkTextIter                 *place);

void		 gsc_completion_hide			(GscCompletion         *completion);

GscInfo *
		 gsc_completion_get_info_window		(GscCompletion         *completion);

struct _GtkSourceView *
		 gsc_completion_get_view			(Gsc	     *completion);

G_END_DECLS

#endif 

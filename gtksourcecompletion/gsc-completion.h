/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-completion.h
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

#ifndef GSC_COMPLETION_H
#define GSC_COMPLETION_H

#include <gtk/gtk.h>
#include "gsc-provider.h"
#include "gsc-info.h"

G_BEGIN_DECLS

/*
 * FIXME: And this here?
 */
#define USER_REQUEST_TRIGGER_NAME "user-request"

/*
 * Type checking and casting macros
 */
#define GSC_TYPE_COMPLETION              (gsc_completion_get_type())
#define GSC_COMPLETION(obj)              (G_TYPE_CHECK_INSTANCE_CAST((obj), GSC_TYPE_COMPLETION, GscCompletion))
#define GSC_COMPLETION_CLASS(klass)      (G_TYPE_CHECK_CLASS_CAST((klass), GSC_TYPE_COMPLETION, GscCompletionClass))
#define GSC_IS_COMPLETION(obj)           (G_TYPE_CHECK_INSTANCE_TYPE((obj), GSC_TYPE_COMPLETION))
#define GSC_IS_COMPLETION_CLASS(klass)   (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_COMPLETION))
#define GSC_COMPLETION_GET_CLASS(obj)    (G_TYPE_INSTANCE_GET_CLASS((obj), GSC_TYPE_COMPLETION, GscCompletionClass))

#define DEFAULT_PAGE "Default"

typedef gboolean (* GscCompletionFilterFunc) (GscProposal *proposal,
					      gpointer     user_data);

typedef struct _GscCompletionPriv GscCompletionPriv;
typedef struct _GscCompletion GscCompletion;
typedef struct _GscCompletionClass GscCompletionClass;

struct _GscCompletion
{
	GtkWindow parent;

	GscCompletionPriv *priv;
};

struct _GscCompletionClass
{
	GtkWindowClass parent_class;

	gboolean (* proposal_selected)(GscCompletion *completion,
				       GscProposal *proposal);
	gboolean (* display_info)     (GscCompletion *completion,
				       GscProposal *proposal);
};

/* ********************* Control Functions ******************** */
/* 
 * FIXME This functions will be deleted when we insert GscCompletion
 * into GtkSourceView. These functions store the relationship between the
 * GscCompletions and the GtkTextView.
 */

GscCompletion	*gsc_completion_get_from_view		(GtkTextView *view);

/* ************************************************************* */

GType		 gsc_completion_get_type		(void) G_GNUC_CONST;

GtkWidget	*gsc_completion_new			(GtkTextView *view);

GtkTextView	*gsc_completion_get_view		(GscCompletion *self);

GscTrigger	*gsc_completion_get_trigger		(GscCompletion *self,
							 const gchar *trigger_name);

GscProvider	*gsc_completion_get_provider		(GscCompletion *self,
							 const gchar *prov_name);

gboolean	 gsc_completion_register_provider	(GscCompletion *self,
							 GscProvider *provider,
							 GscTrigger *trigger);

gboolean	 gsc_completion_unregister_provider	(GscCompletion *self,
							 GscProvider *provider,
							 GscTrigger *trigger);

gboolean	 gsc_completion_register_trigger	(GscCompletion *self,
							 GscTrigger *trigger);

gboolean	 gsc_completion_unregister_trigger	(GscCompletion *self,
							 GscTrigger *trigger);

GscTrigger	*gsc_completion_get_active_trigger	(GscCompletion *self);

gboolean	 gsc_completion_trigger_event		(GscCompletion *self,
							 GscTrigger *trigger);

void		 gsc_completion_finish_completion	(GscCompletion *self);

void		 gsc_completion_filter_proposals	(GscCompletion *self,
							 GscCompletionFilterFunc func,
							 gpointer user_data);

void		 gsc_completion_set_active 		(GscCompletion *self,
							 gboolean active);

gboolean	 gsc_completion_get_active 		(GscCompletion *self);

GtkWidget	*gsc_completion_get_bottom_bar		(GscCompletion *self);

GscInfo		*gsc_completion_get_info_widget		(GscCompletion *self);

gint		gsc_completion_get_page_pos		(GscCompletion *self,
							 const gchar *page_name);

gint		gsc_completion_get_n_pages		(GscCompletion *self);

gboolean	gsc_completion_set_page_pos		(GscCompletion *self,
							 const gchar *page_name,
							 gint position);

G_END_DECLS

#endif 

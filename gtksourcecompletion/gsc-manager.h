/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion.h
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

#ifndef _GSC_H_
#define _GSC_H_

#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include "gsc-provider.h"
#include "gsc-trigger.h"
#include "gsc-popup.h"


G_BEGIN_DECLS

/*
 * FIXME: And this here?
 */
#define USER_REQUEST_TRIGGER_NAME "user-request"

#define GSC_TYPE_MANAGER             (gsc_manager_get_type ())
#define GSC_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_MANAGER, GscManager))
#define GSC_MANAGER_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_MANAGER, GscManagerClass))
#define GSC_IS_MANAGER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_MANAGER))
#define GSC_IS_MANAGER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_MANAGER))
#define GSC_MANAGER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_MANAGER, GscManagerClass))

typedef struct _GscManagerPrivate GscManagerPrivate;

typedef struct _GscManager GscManager;

struct _GscManager
{
	GObject parent_instance;
	
	GscManagerPrivate *priv;
};

typedef struct _GscManagerClass GscManagerClass;

struct _GscManagerClass
{
	GObjectClass parent_class;
};

GType		 gsc_manager_get_type			 (void) G_GNUC_CONST;


/* ********************* Control Functions ******************** */
/*
 * This functions will be deleted when we insert GscManager
 * into GtkSourceView. These functions store the relationship between the
 * GscManagers and the GtkTextView.
 */

GscManager	*gsc_manager_get_from_view		(GtkTextView *view);

/* ************************************************************* */

GscManager	*gsc_manager_new			(GtkTextView *view);

GtkTextView	*gsc_manager_get_view			(GscManager *self);

gboolean	 gsc_manager_is_visible			(GscManager *self);

gboolean	 gsc_manager_register_provider		(GscManager *self,
							 GscProvider *provider,
							 const gchar *trigger_name);

gboolean	 gsc_manager_unregister_provider	(GscManager *self,
							 GscProvider *provider,
							 const gchar *trigger_name);

GscProvider	*gsc_manager_get_provider		(GscManager *self,
							 const gchar* provider_name);

void		 gsc_manager_register_trigger		(GscManager *self,
							 GscTrigger *trigger);

void		 gsc_manager_unregister_trigger		(GscManager *self,
							 GscTrigger *trigger);

GscTrigger	*gsc_manager_get_trigger		(GscManager *self,
							 const gchar* trigger_name);

GscTrigger	*gsc_manager_get_active_trigger		(GscManager *self);

void		 gsc_manager_activate			(GscManager *self);

void		 gsc_manager_deactivate			(GscManager *self);

void		 gsc_manager_finish_completion		(GscManager *self);

void		 gsc_manager_trigger_event		(GscManager *self,
							 const gchar *trigger_name, 
							 gpointer event_data);

void		 gsc_manager_set_current_info		(GscManager *self,
							 gchar *info);

void		 gsc_manager_set_key			(GscManager *self,
							 KeysType type,
							 const gchar* keys);

gchar 		*gsc_manager_get_key			(GscManager *self,
							 KeysType type);

gboolean	 gsc_manager_manage_key			(GscManager *self,
							 GdkEventKey *event);

GtkWidget	*gsc_manager_get_widget			(GscManager *self);

G_END_DECLS

#endif /* _GSC_H_ */

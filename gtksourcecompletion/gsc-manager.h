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

#define USER_REQUEST_TRIGGER_NAME "user-request"

#define GSC_TYPE_MANAGER             (gsc_manager_get_type ())
#define GSC_MANAGER(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_MANAGER, GscManager))
#define GSC_MANAGER_CLASS(klass)             (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_MANAGER, GscManagerClass))
#define GSC_IS_MANAGER(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_MANAGER))
#define GSC_IS_MANAGER_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_MANAGER))
#define GSC_MANAGER_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_MANAGER, GscManagerClass))

typedef struct _GscManagerPrivate GscManagerPrivate;

typedef struct _GscManagerClass GscManagerClass;
typedef struct _GscManager GscManager;

/**
 * GscManagerEventOptions:
 * @popup_options: Options to tell how the popup will be shown
 * @autoselect: If TRUE, the completion selects automatically if there
 * is only one proposal and only one page
 * 
 */
typedef struct _GscManagerEventOptions GscManagerEventOptions;

struct _GscManagerEventOptions{
	GscPopupPositionType position_type;
	GscPopupFilterType filter_type;
	gboolean autoselect;
};

struct _GscManagerClass
{
	GObjectClass parent_class;
};

struct _GscManager
{
	GObject parent_instance;
	GscManagerPrivate *priv;
};


GType 
gsc_manager_get_type (void) G_GNUC_CONST;


/* ********************* Control Functions ******************** */
/*
 * This functions will be deleted when we insert GscManager
 * into GtkSourceView. These functions store the relationship between the
 * GscManagers and the GtkTextView.
 */

/**
 * gsc_manager_get_from_view:
 * @view: the GtkSourceView
 *
 * Returns NULL if the GtkTextView haven't got an associated GscManager
 * or the GscManager of this GtkTextView
 * 
 **/
GscManager*
gsc_manager_get_from_view(GtkTextView *view);

/* ************************************************************* */

/**
 * gsc_manager_new:
 * @view: a #GtkSourceView.
 *
 * Creates a new #GscManager asociated to a GtkSourceView
 *
 * Returns: value: A new #GscManager
 **/
GscManager* 
gsc_manager_new (GtkTextView *view);

/**
 * gsc_manager_get_view:
 * @completion: the #GscManager
 *
 * Returns: The internal #GtkTextView of this completion.
 * 
 **/
GtkTextView* 
gsc_manager_get_view(GscManager *completion);

/**
 * gsc_manager_is_visible:
 * @completion: The #GscManager
 *
 * Returns TRUE if the completion popup is visible.
 *
 */
gboolean
gsc_manager_is_visible(GscManager *completion);

/**
 * gsc_manager_register_provider:
 * @completion: the #GscManager
 * @provider: The #GscProvider.
 * @trigger_name: The trigger name what you want to register this provider
 *
 * This function register the provider into the completion and reference it. When 
 * an event is raised, completion call to the provider to get the data. When the user
 * select a proposal, it call the provider to tell it this action and the provider do
 * that it want (normally inserts some text)
 * 
 * Returns TRUE if it was registered or FALSE if not (because it has been already registered,
 * or the trigger don't exists)
 *
 **/
gboolean
gsc_manager_register_provider(GscManager *completion, 
					GscProvider *provider,
					const gchar *trigger_name);

/**
 * gsc_manager_unregister_provider:
 * @completion: the #GscManager
 * @provider: The #GscProvider.
 * @trigger_name: The trigger name what you want to unregister this provider
 *
 * This function unregister the provider.
 * 
 * Returns TRUE if it was unregistered or FALSE if not (because it doesn't exists,
 * or the trigger don't exists)
 * 
 **/
gboolean
gsc_manager_unregister_provider(GscManager *completion,
					  GscProvider *provider,
					  const gchar *trigger_name);

/**
 * gsc_manager_get_provider:
 * @completion: The #GscManager
 * @provider_name: Provider's name that you are looking for.
 *
 * Returns The provider if the completion has this provider registered or 
 * NULL if not.
 *
 */
GscProvider*
gsc_manager_get_provider(GscManager *completion,
				   const gchar* provider_name);

/**
 * gsc_manager_register_trigger:
 * @completion: The #GscManager
 * @trigger: The trigger to register
 *
 * This function register a completion trigger. If the completion is actived
 * then this method activate the trigger. This function reference the trigger
 * object
 */
void
gsc_manager_register_trigger(GscManager *completion,
				       GscTrigger *trigger);

/**
 * gsc_manager_unregister_trigger:
 * @completion: The #GscManager
 * @trigger: The trigger to unregister
 *
 * This function unregister a completion trigger. If the completion is actived
 * then this method deactivate the trigger. This function reference the trigger
 * object
 */																
void
gsc_manager_unregister_trigger(GscManager *completion,
					 GscTrigger *trigger);

/**
 * gsc_manager_get_trigger:
 * @completion: The #GscManager
 * @trigger_name: The trigger name to get
 *
 * This function return the trigger with this name.
 *
 * Returns The trigger or NULL if not exists
 *
 */
GscTrigger*
gsc_manager_get_trigger(GscManager *completion,
				  const gchar* trigger_name);


/**
 * gsc_manager_get_active_trigger:
 * @completion: The #GscManager
 *
 * This function return the active trigger. The active trigger is the last
 * trigger raised if the completion is active. If the completion is not visible then
 * there is no an active trigger.
 *
 * Returns The trigger or NULL if completion is not active
 *
 */
GscTrigger*
gsc_manager_get_active_trigger(GscManager *completion);

/**
 * gsc_manager_activate:
 * @completion: The #GscManager
 *
 * This function activate the completion mechanism. The completion connects 
 * all signals and activate all registered triggers.
 */
void
gsc_manager_activate(GscManager *completion);

/**
 * gsc_manager_deactivate:
 * @completion: The #GscManager
 *
 * This function deactivate the completion mechanism. The completion disconnect
 * all signals and deactivate all registered triggers.
 */
void
gsc_manager_deactivate(GscManager *completion);

/**
 * gsc_manager_finish_completion:
 * @completion: The #GscManager
 *
 * This function finish the completion if it is active (visible).
 */
void
gsc_manager_finish_completion(GscManager *completion);

/**
 * gsc_manager_trigger_event:
 * @completion: the #GscManager
 * @trigger_name: The event name to raise
 * @event_data: This object will be passed to the providers to give them some special information of the event
 *
 * Calling this function, the completion call to all providers to get data and, if 
 * they return data, it shows the completion to the user. 
 * 
 **/
void 
gsc_manager_trigger_event(GscManager *completion, 
			  const gchar *trigger_name, 
			  gpointer event_data);

/**
 * gsc_manager_trigger_event_with_opts:
 * @completion: the #GscManager
 * @trigger_name: The event name to raise
 * @options: Options to tell the completion how it must to work.
 * @event_data: This object will be passed to the providers to give them some special information of the event
 *
 * Calling this function, the completion call to all providers to get data and, if 
 * they return data, it shows the completion to the user. Use this function if
 * you want to show the popup with special parameters (position, filter, etc)
 * 
 **/
void 
gsc_manager_trigger_event_with_opts(GscManager *completion, 
				    const gchar *trigger_name,
				    GscManagerEventOptions *options,
				    gpointer event_data);

/**
 * gsc_manager_set_current_info:
 * @self: The #GscManager
 * @info: Info markup to be shown into for current proposal.
 *
 * You can use this function when a GscProposal emit the 
 * display-info signal to set the current info.
 */
void
gsc_manager_set_current_info(GscManager *self,
			     gchar *info);

G_END_DECLS

#endif /* _GSC_H_ */

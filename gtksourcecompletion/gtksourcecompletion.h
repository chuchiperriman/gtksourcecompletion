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

#ifndef _GTK_SOURCE_COMPLETION_H_
#define _GTK_SOURCE_COMPLETION_H_

#include <glib-object.h>
#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>

G_BEGIN_DECLS

#define GTK_TYPE_SOURCE_COMPLETION             (gtk_source_completion_get_type ())
#define GTK_SOURCE_COMPLETION(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_SOURCE_COMPLETION, GtkSourceCompletion))
#define GTK_SOURCE_COMPLETION_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_SOURCE_COMPLETION, GtkSourceCompletionClass))
#define GTK_IS_SOURCE_COMPLETION(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_SOURCE_COMPLETION))
#define GTK_IS_SOURCE_COMPLETION_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_SOURCE_COMPLETION))
#define GTK_SOURCE_COMPLETION_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_SOURCE_COMPLETION, GtkSourceCompletionClass))

typedef struct _GtkSourceCompletionPrivate GtkSourceCompletionPrivate;

typedef struct _GtkSourceCompletionClass GtkSourceCompletionClass;
typedef struct _GtkSourceCompletion GtkSourceCompletion;

#include "gtksourcecompletion-provider.h"
#include "gtksourcecompletion-trigger.h"

struct _GtkSourceCompletionClass
{
	GObjectClass parent_class;
};

struct _GtkSourceCompletion
{
	GObject parent_instance;
	GtkSourceCompletionPrivate *priv;
};

GType 
gtk_source_completion_get_type (void) G_GNUC_CONST;

/********************** Control Functions *********************/
/*
 * This functions will be deleted when we insert GtkSourceCompletion
 * into GtkSourceView. These functions store the relationship between the
 * GtkSourceCompletions and the GtkTextView.
 */

/**
 * gtk_source_completion_get_from_view:
 * @view: the GtkSourceView
 *
 * Returns NULL if the GtkTextView haven't got an associated GtkSourceCompletion
 * or the GtkSourceCompletion of this GtkTextView
 * 
 **/
GtkSourceCompletion*
gtk_source_completion_get_from_view(GtkTextView *view);

/***************************************************************/

/**
 * gtk_source_completion_new:
 * @view: a #GtkSourceView.
 *
 * Creates a new #GtkSourceCompletion asociated to a GtkSourceView
 *
 * Returns: value: A new #GtkSourceCompletion
 **/
GtkSourceCompletion* 
gtk_source_completion_new (GtkTextView *view);

/**
 * gtk_source_completion_get_view:
 * @completion: the #GtkSourceCompletion
 *
 * Returns: The internal #GtkTextView of this completion.
 * 
 **/
GtkTextView* 
gtk_source_completion_get_view(GtkSourceCompletion *completion);

/**
 * gtk_source_completion_is_visible:
 * @completion: The #GtkSourceCompletion
 *
 * Returns TRUE if the completion popup is visible.
 *
 */
gboolean
gtk_source_completion_is_visible(GtkSourceCompletion *completion);

/**
 * gtk_source_completion_register_provider:
 * @completion: the #GtkSourceCompletion
 * @provider: The #GtkSourceCompletionProvider.
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
gtk_source_completion_register_provider(GtkSourceCompletion *completion, 
					GtkSourceCompletionProvider *provider,
					const gchar *trigger_name);

/**
 * gtk_source_completion_unregister_provider:
 * @completion: the #GtkSourceCompletion
 * @provider: The #GtkSourceCompletionProvider.
 * @trigger_name: The trigger name what you want to unregister this provider
 *
 * This function unregister the provider.
 * 
 * Returns TRUE if it was unregistered or FALSE if not (because it doesn't exists,
 * or the trigger don't exists)
 * 
 **/
gboolean
gtk_source_completion_unregister_provider(GtkSourceCompletion *completion,
					  GtkSourceCompletionProvider *provider,
					  const gchar *trigger_name);

/**
 * gtk_source_completion_get_provider:
 * @completion: The #GtkSourceCompletion
 * @provider_name: Provider's name that you are looking for.
 *
 * Returns The provider if the completion has this provider registered or 
 * NULL if not.
 *
 */
GtkSourceCompletionProvider*
gtk_source_completion_get_provider(GtkSourceCompletion *completion,
				   const gchar* provider_name);

/**
 * gtk_source_completion_register_trigger:
 * @completion: The #GtkSourceCompletion
 * @trigger: The trigger to register
 *
 * This function register a completion trigger. If the completion is actived
 * then this method activate the trigger. This function reference the trigger
 * object
 */
void
gtk_source_completion_register_trigger(GtkSourceCompletion *completion,
				       GtkSourceCompletionTrigger *trigger);

/**
 * gtk_source_completion_unregister_trigger:
 * @completion: The #GtkSourceCompletion
 * @trigger: The trigger to unregister
 *
 * This function unregister a completion trigger. If the completion is actived
 * then this method deactivate the trigger. This function reference the trigger
 * object
 */																
void
gtk_source_completion_unregister_trigger(GtkSourceCompletion *completion,
					 GtkSourceCompletionTrigger *trigger);

/**
 * gtk_source_completion_get_trigger:
 * @completion: The #GtkSourceCompletion
 * @trigger_name: The trigger name to get
 *
 * This function return the trigger with this name.
 *
 * Returns The trigger or NULL if not exists
 *
 */
GtkSourceCompletionTrigger*
gtk_source_completion_get_trigger(GtkSourceCompletion *completion,
				  const gchar* trigger_name);


/**
 * gtk_source_completion_get_active_trigger:
 * @completion: The #GtkSourceCompletion
 *
 * This function return the active trigger. The active trigger is the last
 * trigger raised if the completion is active. If the completion is not visible then
 * there is no an active trigger.
 *
 * Returns The trigger or NULL if completion is not active
 *
 */
GtkSourceCompletionTrigger*
gtk_source_completion_get_active_trigger(GtkSourceCompletion *completion);

/**
 * gtk_source_completion_activate:
 * @completion: The #GtkSourceCompletion
 *
 * This function activate the completion mechanism. The completion connects 
 * all signals and activate all registered triggers.
 */
void
gtk_source_completion_activate(GtkSourceCompletion *completion);

/**
 * gtk_source_completion_deactivate:
 * @completion: The #GtkSourceCompletion
 *
 * This function deactivate the completion mechanism. The completion disconnect
 * all signals and deactivate all registered triggers.
 */
void
gtk_source_completion_deactivate(GtkSourceCompletion *completion);

/**
 * gtk_source_completion_finish_completion:
 * @completion: The #GtkSourceCompletion
 *
 * This function finish the completion if it is active (visible).
 */
void
gtk_source_completion_finish_completion(GtkSourceCompletion *completion);

/**
 * gtk_source_completion_trigger_event:
 * @completion: the #GtkSourceCompletion
 * @trigger_name: The event name to raise
 * @event_data: This object will be passed to the providers to give them some special information of the event
 *
 * Calling this function, the completion call to all providers to get data and, if 
 * they return data, it shows the completion to the user. 
 * 
 **/
void 
gtk_source_completion_trigger_event(GtkSourceCompletion *completion, 
				    const gchar *trigger_name, 
				    gpointer event_data);
/**
 * gtk_source_completion_set_current_info:
 * @self: The #GtkSourceCompletion
 * @info: Info markup to be shown into for current proposal.
 *
 * You can use this function when a GtkSourceCompletionProposal emit the 
 * display-info signal to set the current info.
 */
void
gtk_source_completion_set_current_info(GtkSourceCompletion *self,
					     gchar *info);

/**
 * gtk_source_completion_set_autoselect:
 * @self: The #GtkSourceCompletion
 * @autoselect: TRUE enable autoselect or FALSE to disable autoselect
 *
 * If Autoselect is enabled then the completion selects the current completion
 * proposal if there is only one.
 */
void
gtk_source_completion_set_autoselect(GtkSourceCompletion *self,
					   gboolean autoselect);

/**
 * gtk_source_completion_get_autoselect:
 * @self: The #GtkSourceCompletion
 *
 * If Autoselect is enabled then the completion selects the current completion
 * proposal if there is only one.
 *
 * Returns TRUE if autoselect is enabled;
 */
gboolean
gtk_source_completion_get_autoselect(GtkSourceCompletion *self);

G_END_DECLS

#endif /* _GTK_SOURCE_COMPLETION_H_ */

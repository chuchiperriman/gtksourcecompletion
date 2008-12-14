/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-manager.c
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
 
#include <gdk/gdkkeysyms.h>
#include <string.h>
#include "gsc-manager.h"
#include "gsc-i18n.h"
#include "gsc-proposal.h"
#include "gsc-utils.h"

static gboolean lib_initialized = FALSE;

#define DEFAULT_INFO_KEYS "<Control>i"
#define DEFAULT_PAGE_NEXT_KEYS "Right"
#define DEFAULT_PAGE_PREV_KEYS "Left"

#define GSC_MANAGER_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GSC_TYPE_MANAGER, GscManagerPrivate))

/*
 * FIXME: I do not like this names. Maybe without the IS_GET_TEXT prefix?
 */
/* Internal signals */
enum
{
	IS_GTK_TEXT_VIEW_KP,
	IS_GTK_TEXT_DESTROY,
	IS_GTK_TEXT_FOCUS_OUT,
	IS_GTK_TEXT_BUTTON_PRESS,
	LAST_SIGNAL
};

/* Properties */
enum {
	PROP_0,
	PROP_INFO_KEYS,
	PROP_NEXT_PAGE_KEYS,
	PROP_PREV_PAGE_KEYS
};

typedef struct 
{
	guint key;
	GdkModifierType mods;
} KeyDef;

struct _GscManagerPrivate
{
	GtkTextView *text_view;
	GscPopup *popup;
	GList *triggers;
	/*Providers of the triggers*/
	GHashTable *trig_prov;
	GList *providers;
	gulong internal_signal_ids[LAST_SIGNAL];
	gboolean active;
	GscTrigger *active_trigger;
	KeyDef keys[KEYS_LAST];
};

G_DEFINE_TYPE(GscManager, gsc_manager, G_TYPE_OBJECT);

/*
 * FIXME: Nacho: why this can't go directly in the hash?
 * do we really need a hash for this?
 */
struct _ProviderList
{
	GList *prov_list;
};
typedef struct _ProviderList ProviderList;


/* **************** GtkTextView-GscManager Control *********** */

/*
 * We save a map with a GtkTextView and his GscManager. If you 
 * call twice to gsc_proposal_new, the second time it returns
 * the previous created GscManager, not creates a new one
 *
 * FIXME We will remove this functions when we will integrate 
 * Gsc in GtkSourceView
 */

static GHashTable *completion_map = NULL;

static GscManager* 
completion_control_get_completion (GtkTextView *view)
{
	if (completion_map == NULL)
		completion_map = g_hash_table_new (g_direct_hash,
						   g_direct_equal);

	return g_hash_table_lookup (completion_map, view);
}

static void 
completion_control_add_completion (GtkTextView *view,
				   GscManager *comp)
{
	g_hash_table_insert (completion_map, view,comp);
}

static void 
completion_control_remove_completion (GtkTextView *view)
{
	g_hash_table_remove (completion_map, view);
}
/* ********************************************************************* */

static void
prov_list_free (gpointer prov_list)
{
	ProviderList *pl = (ProviderList*)prov_list;
	
	g_list_free (pl->prov_list);
	g_free (pl);
}

static void
end_completion (GscManager *self)
{
	if (GTK_WIDGET_VISIBLE (self->priv->popup))
	{
		gtk_widget_hide (GTK_WIDGET (self->priv->popup));
	}
	else
	{
		GList *providers = self->priv->providers;
		
		do 
		{
			GscProvider *provider =  GSC_PROVIDER (providers->data);
			gsc_provider_finish (provider);
			
		}while ((providers = g_list_next (providers)) != NULL);
	
		self->priv->active_trigger = NULL;
	}
}

static void
popup_proposal_select_cb (GtkWidget *popup,
			  GscProposal *proposal,
			  gpointer user_data)
{
	GscManager *self = GSC_MANAGER (user_data);
	
	gsc_proposal_apply (proposal, self->priv->text_view);
	end_completion (self);
}

static void
popup_hide_cb (GtkWidget *widget,
	       gpointer user_data)
{
	GscManager *self = GSC_MANAGER (user_data);
	
	end_completion (self);
}

static void
view_destroy_event_cb (GtkWidget *widget,
		       gpointer user_data)
{
	GscManager *self = GSC_MANAGER (user_data);
	
	g_object_unref (self);
}

static gboolean
view_focus_out_event_cb (GtkWidget *widget,
			 GdkEventFocus *event,
			 gpointer user_data)
{
	GscManager *self = GSC_MANAGER (user_data);
	
	if (gsc_manager_is_visible (self)
	    && GTK_WIDGET_HAS_FOCUS (self->priv->popup))
		end_completion (self);
	
	return FALSE;
}

static gboolean
view_button_press_event_cb (GtkWidget *widget,
			    GdkEventButton *event,
			    gpointer user_data)
{
	GscManager *self = GSC_MANAGER (user_data);
	
	if (gsc_manager_is_visible (self))
		end_completion (self);

	return FALSE;
}

static void
free_provider_list (gpointer list)
{
	GList *start = (GList*)list;
	GList *temp = (GList*)list;
	
	if (temp != NULL)
	{
		do
		{
			g_object_unref (G_OBJECT (temp->data));
			
		}while ((temp = g_list_next (temp)) != NULL);
		
		g_list_free (start);
	}
}

static void
free_trigger_list (gpointer list)
{
	GList *start = (GList*)list;
	GList *temp = (GList*)list;
	
	if (temp != NULL)
	{
		do
		{
			g_object_unref (G_OBJECT (temp->data));
			
		}while ((temp = g_list_next (temp)) != NULL);
		
		g_list_free (start);
	}
}

static gboolean
view_key_press_event_cb (GtkWidget *view,
			 GdkEventKey *event, 
			 gpointer user_data)
{
	GscManager *self = GSC_MANAGER (user_data);
	
	if (gsc_manager_is_visible (self))
		return gsc_manager_manage_key (self, event);

	return FALSE;
}

static void
gsc_manager_set_property (GObject      *object,
			  guint         prop_id,
			  const GValue *value,
			  GParamSpec   *pspec)
{
	GscManager *self = GSC_MANAGER (object);
	g_return_if_fail (GSC_IS_MANAGER (object));

	switch (prop_id)
	{
		case PROP_INFO_KEYS:
			gsc_manager_set_key (self, KEYS_INFO,
					     g_value_get_string (value));
			break;
		case PROP_NEXT_PAGE_KEYS:
			gsc_manager_set_key (self, KEYS_PAGE_NEXT,
					     g_value_get_string (value));
			break;
		case PROP_PREV_PAGE_KEYS:
			gsc_manager_set_key (self, KEYS_PAGE_PREV,
					     g_value_get_string (value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gsc_manager_get_property (GObject    *object,
			  guint       prop_id,
			  GValue     *value,
			  GParamSpec *pspec)
{
	GscManager *self = GSC_MANAGER(object);
	
	g_return_if_fail (GSC_IS_MANAGER (object));

	switch (prop_id)
	{
		case PROP_INFO_KEYS:
			g_value_set_string (value,
					    gsc_manager_get_key (self, KEYS_INFO));
			break;
		case PROP_NEXT_PAGE_KEYS:
			g_value_set_string (value,
					    gsc_manager_get_key (self, KEYS_PAGE_NEXT));
			break;
		case PROP_PREV_PAGE_KEYS:
			g_value_set_string (value,
					    gsc_manager_get_key (self, KEYS_PAGE_PREV));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gsc_manager_init (GscManager *self)
{
	gint i;

	if (!lib_initialized)
	{
		bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
		bind_textdomain_codeset (GETTEXT_PACKAGE, "UTF-8");
		lib_initialized = TRUE;
	}
	
	self->priv = GSC_MANAGER_GET_PRIVATE (self);
	
	self->priv->active = FALSE;
	self->priv->providers = NULL;
	self->priv->triggers = NULL;
	self->priv->popup = NULL;
	self->priv->active_trigger = NULL;
	self->priv->trig_prov = g_hash_table_new_full (g_str_hash,
						       g_str_equal,
						       g_free,
						       (GDestroyNotify)prov_list_free);

	gsc_manager_set_key (self, KEYS_INFO, DEFAULT_INFO_KEYS);
	gsc_manager_set_key (self, KEYS_PAGE_NEXT, DEFAULT_PAGE_NEXT_KEYS);
	gsc_manager_set_key (self, KEYS_PAGE_PREV, DEFAULT_PAGE_PREV_KEYS);

	self->priv->popup = GSC_POPUP (gsc_popup_new ());
	
	for (i = 0; i < LAST_SIGNAL; i++)
	{
		self->priv->internal_signal_ids[i] = 0;
	}
	
	g_signal_connect (self->priv->popup, 
			  "proposal-selected",
			  G_CALLBACK (popup_proposal_select_cb),
			  self);
	
	g_signal_connect (self->priv->popup, 
			  "hide",
			  G_CALLBACK (popup_hide_cb),
			  self);
}

static void
gsc_manager_finalize (GObject *object)
{
	GscManager *self = GSC_MANAGER (object);
	
	if (self->priv->active)
		gsc_manager_deactivate (self);
	
	gtk_widget_destroy (GTK_WIDGET (self->priv->popup));

	free_provider_list (self->priv->providers);
	free_trigger_list (self->priv->triggers);
	g_hash_table_destroy (self->priv->trig_prov);

	completion_control_remove_completion (self->priv->text_view);
}

static void
gsc_manager_class_init (GscManagerClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);

	object_class->get_property = gsc_manager_get_property;
	object_class->set_property = gsc_manager_set_property;
	object_class->finalize = gsc_manager_finalize;

	/**
	 * GscManager:info-keys:
	 *
	 * Keys to show/hide the info window
	 */
	g_object_class_install_property (object_class,
					 PROP_INFO_KEYS,
					 g_param_spec_string ("info-keys",
							      _("Keys to show/hide the info window"),
							      _("Keys to show/hide the info window"),
							      DEFAULT_INFO_KEYS,
							      G_PARAM_READWRITE));
	/**
	 * GscManager:next-page-keys:
	 *
	 * Keys to show the next completion page
	 */
	g_object_class_install_property (object_class,
					 PROP_NEXT_PAGE_KEYS,
					 g_param_spec_string ("next-page-keys",
							      _("Keys to show the next completion page"),
							      _("Keys to show the next completion page"),
							      DEFAULT_PAGE_NEXT_KEYS,
							      G_PARAM_READWRITE));
							      
	/**
	 * GscManager:previous-page-keys:
	 *
	 * Keys to show the previous completion page
	 */
	g_object_class_install_property (object_class,
					 PROP_PREV_PAGE_KEYS,
					 g_param_spec_string ("previous-page-keys",
							      _("Keys to show the previous completion page"),
							      _("Keys to show the previous completion page"),
							      DEFAULT_PAGE_PREV_KEYS,
							      G_PARAM_READWRITE));

	g_type_class_add_private (object_class, sizeof (GscManagerPrivate));
}

/**
 * gsc_manager_new:
 * @view: a #GtkSourceView.
 *
 * Creates a new #GscManager asociated to a GtkSourceView
 *
 * Returns: value: A new #GscManager
 **/
GscManager*
gsc_manager_new (GtkTextView *view)
{
	GscManager *self;

	g_assert (view != NULL);
	
	self = completion_control_get_completion (view);
	if (self != NULL)
	{
		return self;
	}

	self = GSC_MANAGER (g_object_new (GSC_TYPE_MANAGER, NULL));
	self->priv->text_view = view;
	
	completion_control_add_completion (view, self);
	
	return self;
}

/**
 * gsc_manager_set_key:
 * @self: The #GscManager
 * @type: Key what you want to set
 * @keys: Keys to be assigned to the key type
 *
 * Sets the keys for the key type. By example you can set the info keys 
 * by passing KEYS_INFO type and "<control>i" keys.
 *
 */
void
gsc_manager_set_key (GscManager *self,
		     KeysType type,
		     const gchar* keys)
{
	guint key;
	GdkModifierType mods;
	
	g_return_if_fail (GSC_IS_MANAGER (self));
	
	gtk_accelerator_parse (keys, &key, &mods);
	g_return_if_fail (key != 0);
	
	self->priv->keys[type].key = key;
	self->priv->keys[type].mods = mods;
}

/**
 * gsc_manager_get_key:
 * @self: The #GscManager
 * @type: The key type what you want to get
 *
 * Returns: New allocated string representation with #gtk_accelerator_name
 */
gchar *
gsc_manager_get_key (GscManager *self,
		     KeysType type)
{
	g_return_val_if_fail (GSC_IS_MANAGER (self), NULL);

	return gtk_accelerator_name (self->priv->keys[type].key,
				     self->priv->keys[type].mods);
}

/**
 * gsc_manager_manage_key:
 * @self: The #GscManager
 * @event: Key event to be managed
 *
 * Manage the event keys. If it is Return, it will select the current proposal, 
 * if it is a Down key then selects the next proposal etc.
 *
 * Returns: TRUE if the key has been used.
 */
gboolean
gsc_manager_manage_key (GscManager *self,
		        GdkEventKey *event)
{
	gboolean catched = FALSE;
	gboolean ret = FALSE;
	gboolean selected = FALSE;
	
	g_return_val_if_fail (GSC_IS_MANAGER (self), FALSE);
	
	switch (event->keyval)
 	{
		case GDK_Escape:
		{
			gtk_widget_hide (GTK_WIDGET (self->priv->popup));
			catched = TRUE;
			ret = TRUE;
			break;
		}
 		case GDK_Down:
		{
			ret = gsc_popup_select_next (self->priv->popup, 1);
			catched = TRUE;
			break;
		}
		case GDK_Page_Down:
		{
			ret = gsc_popup_select_next (self->priv->popup, 5);
			catched = TRUE;
			break;
		}
		case GDK_Up:
		{
			ret = gsc_popup_select_previous (self->priv->popup, 1);
			if (!ret)
				ret = gsc_popup_select_first (self->priv->popup);
			catched = TRUE;
			break;
		}
		case GDK_Page_Up:
		{
			ret = gsc_popup_select_previous (self->priv->popup, 5);
			catched = TRUE;
			break;
		}
		case GDK_Home:
		{
			ret = gsc_popup_select_first (self->priv->popup);
			catched = TRUE;
			break;
		}
		case GDK_End:
		{
			ret = gsc_popup_select_last (self->priv->popup);
			catched = TRUE;
			break;
		}
		case GDK_Return:
		case GDK_Tab:
		{
			selected = gsc_popup_select_current_proposal (self->priv->popup);
			gtk_widget_hide (GTK_WIDGET (self->priv->popup));
			catched = TRUE;
			if (selected)
				ret = TRUE;
			else
				ret = FALSE;
			break;
		}
	}
	if (!catched)
	{
		if (gsc_compare_keys (self->priv->keys[KEYS_INFO].key,
				      self->priv->keys[KEYS_INFO].mods,
				      event))
		{
			gsc_popup_toggle_proposal_info (self->priv->popup);
			ret = TRUE;
		}
		else if (gsc_compare_keys (self->priv->keys[KEYS_PAGE_NEXT].key,
					   self->priv->keys[KEYS_PAGE_NEXT].mods,
					   event))
		{
			gsc_popup_page_next (self->priv->popup);
			ret = TRUE;
		}
		else if (gsc_compare_keys (self->priv->keys[KEYS_PAGE_PREV].key,
					   self->priv->keys[KEYS_PAGE_PREV].mods,
					   event))
		{
			gsc_popup_page_previous (self->priv->popup);
			ret = TRUE;
		}
	}
	return ret;
}

/**
 * gsc_manager_register_provider:
 * @self: the #GscManager
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
gsc_manager_register_provider (GscManager *self,
			       GscProvider *provider,
			       const gchar *trigger_name)
{
	GscTrigger *trigger;
	ProviderList *pl;
	GscProvider *prov;
	
	g_return_val_if_fail (GSC_IS_MANAGER (self), FALSE);
	trigger = gsc_manager_get_trigger (self, trigger_name);
	
	if (trigger == NULL)
		return FALSE;
	
	pl = g_hash_table_lookup (self->priv->trig_prov, trigger_name);
	g_assert (pl != NULL);
	
	pl->prov_list = g_list_append (pl->prov_list, provider);
	
	prov = gsc_manager_get_provider(self, gsc_provider_get_name (provider));
	if (prov != NULL)
		return FALSE;
	
	self->priv->providers = g_list_append (self->priv->providers, provider);
	g_object_ref (provider);

	return TRUE;
}

/**
 * gsc_manager_unregister_provider:
 * @self: the #GscManager
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
gsc_manager_unregister_provider (GscManager *self,
				 GscProvider *provider,
				 const gchar *trigger_name)
{
	GscTrigger *trigger;
	ProviderList *pl;
	
	g_return_val_if_fail (GSC_IS_MANAGER (self), FALSE);
	g_return_val_if_fail (g_list_find (self->priv->providers, provider) != NULL,
			      FALSE);
	
	trigger = gsc_manager_get_trigger (self, trigger_name);
	if (trigger == NULL)
		return FALSE;
		
	pl = g_hash_table_lookup (self->priv->trig_prov, trigger_name);
	g_assert (pl != NULL);
	
	pl->prov_list = g_list_remove (pl->prov_list, provider);
	
	self->priv->providers = g_list_remove (self->priv->providers,
					       provider);
	g_object_unref (provider);
	
	return TRUE;
}

/**
 * gsc_manager_get_view:
 * @self: the #GscManager
 *
 * Returns: The internal #GtkTextView of this completion.
 * 
 **/
GtkTextView*
gsc_manager_get_view (GscManager *self)
{
	g_return_val_if_fail (GSC_IS_MANAGER (self), NULL);

	return self->priv->text_view;
}

/*
 * FIXME: Maybe this func should be splitted
 */
/**
 * gsc_manager_trigger_event:
 * @self: the #GscManager
 * @trigger_name: The event name to raise
 * @event_data: This object will be passed to the providers to give them some special information of the event
 *
 * Calling this function, the completion call to all providers to get data and, if 
 * they return data, it shows the completion to the user. 
 * 
 **/
void
gsc_manager_trigger_event (GscManager *self, 
			   const gchar *trigger_name,
			   gpointer event_data)
{
	GList* data_list;
	GList* original_list;
	GList* final_list = NULL;
	GList *providers_list;
	GscProvider *provider;
	GscTrigger *trigger;
	gint proposals = 0;
	GscProposal *last_proposal = NULL;
	ProviderList *pl;

	g_return_if_fail (GSC_IS_MANAGER (self));

	trigger = gsc_manager_get_trigger (self, trigger_name);
	g_return_if_fail (trigger != NULL);
	
	if (!GTK_WIDGET_HAS_FOCUS (self->priv->text_view))
		return;
	
	/*
	 * If the completion is visble and there is a trigger active, you cannot
	 * raise a different trigger until the current trigger finish
	 */
	if (gsc_manager_is_visible (self)
	    && self->priv->active_trigger != trigger)
	{
		return;
	}
	
	gsc_popup_clear (self->priv->popup);
	
	pl = g_hash_table_lookup (self->priv->trig_prov, trigger_name);
	if (pl == NULL)
		return;
	
	/*providers_list = self->priv->providers;*/
	providers_list = pl->prov_list;
	
	if (providers_list == NULL)
	{
		if (gsc_manager_is_visible (self))
			end_completion (self);
		
		return;
	}
	
	/*Getting the data...*/
	do
	{
		provider =  GSC_PROVIDER (providers_list->data);
		data_list = gsc_provider_get_proposals (provider, trigger);
		
		if (data_list != NULL)
		{
			original_list = data_list;
			do
			{
				final_list = g_list_append (final_list,
							    data_list->data);
			}while ((data_list = g_list_next (data_list)) != NULL);
			
			g_list_free (original_list);
		}
	}while ((providers_list = g_list_next (providers_list)) != NULL);
	
	if (final_list == NULL)
	{
		if (gsc_manager_is_visible (self))
			end_completion (self);
			
		return;
	}
	
	data_list = final_list;
	/* Insert the data into the model */
	do
	{
		last_proposal = GSC_PROPOSAL (data_list->data);
		
		gsc_popup_add_data (self->priv->popup,
				    last_proposal);
		++proposals;
	}while ((data_list = g_list_next (data_list)) != NULL);
	
	g_list_free (final_list);
	/* If there are not proposals, we don't show the popup */
	if (proposals > 0)
	{
		gint x, y;
		GtkWindow *win;
		
		if (!GTK_WIDGET_HAS_FOCUS (self->priv->text_view))
			return;

		gsc_get_window_position_in_cursor (GTK_WINDOW (self->priv->popup),
						   self->priv->text_view,
						   &x, &y);
		gtk_window_move (GTK_WINDOW (self->priv->popup),
				 x, y);
		gsc_popup_show_or_update (GTK_WIDGET (self->priv->popup));

		/*Set the focus to the View, not the completion popup*/
		win = GTK_WINDOW (gtk_widget_get_ancestor (GTK_WIDGET (self->priv->text_view),
				  GTK_TYPE_WINDOW));
		gtk_window_present (win);
		gtk_widget_grab_focus (GTK_WIDGET (self->priv->text_view));

		self->priv->active_trigger = trigger;
	}
	else if (GTK_WIDGET_VISIBLE (self->priv->popup))
	{
		end_completion (self);
	}
}

/**
 * gsc_manager_is_visible:
 * @self: The #GscManager
 *
 * Returns: %TRUE if the completion popup is visible.
 */
gboolean
gsc_manager_is_visible (GscManager *self)
{
	g_return_val_if_fail (GSC_IS_MANAGER (self), FALSE);

	return GTK_WIDGET_VISIBLE (self->priv->popup);
}

/**
 * gsc_manager_get_from_view:
 * @view: the GtkSourceView
 *
 * Returns: %NULL if the GtkTextView haven't got an associated GscManager
 * or the GscManager of this GtkTextView
 **/
GscManager *
gsc_manager_get_from_view (GtkTextView *view)
{
	return completion_control_get_completion (view);
}

/**
 * gsc_manager_get_provider:
 * @self: The #GscManager
 * @provider_name: Provider's name that you are looking for.
 *
 * Returns: The provider if the completion has this provider registered or 
 * NULL if not.
 */
GscProvider *
gsc_manager_get_provider (GscManager *self,
			  const gchar* provider_name)
{
	GList *plist = self->priv->providers;
	GscProvider *provider;
	
	g_return_val_if_fail (GSC_IS_MANAGER (self), NULL);
	
	if (plist != NULL)
	{
		do
		{
			provider =  GSC_PROVIDER (plist->data);
			
			if (strcmp (gsc_provider_get_name (provider),
				    provider_name) == 0)
			{
				return provider;
			}
		}while ((plist = g_list_next (plist)) != NULL);
	}
	
	return NULL;
}

/**
 * gsc_manager_register_trigger:
 * @self: The #GscManager
 * @trigger: The trigger to register
 *
 * This function register a completion trigger. If the completion is actived
 * then this method activate the trigger. This function reference the trigger
 * object
 */
void
gsc_manager_register_trigger (GscManager *self,
			      GscTrigger *trigger)
{
	const gchar* trigger_name;
	ProviderList *pl;
	
	g_return_if_fail (GSC_IS_MANAGER (self));
	
	trigger_name = gsc_trigger_get_name (trigger);
	pl = g_hash_table_lookup (self->priv->trig_prov, trigger_name);

	/*Only register the trigger if it has not been registered yet*/
	if (pl == NULL)
	{
		const gchar *tn;
		ProviderList *pl; //FIXME: Another ProviderList with the same name?
	
		self->priv->triggers = g_list_append (self->priv->triggers,
						      trigger);
		g_object_ref (trigger);
		
		tn = gsc_trigger_get_name (trigger);
		
		/*
		 * FIXME: I do not like this malloc here either. Maybe g_slice_new ?
		 */
		pl = g_malloc0 (sizeof (ProviderList));
		
		pl->prov_list = NULL;
		
		g_hash_table_insert (self->priv->trig_prov, g_strdup (tn), pl);
		if (self->priv->active)
		{
			gsc_trigger_activate (trigger);
		}
	}
}

/**
 * gsc_manager_unregister_trigger:
 * @self: The #GscManager
 * @trigger: The trigger to unregister
 *
 * This function unregister a completion trigger. If the completion is actived
 * then this method deactivate the trigger. This function reference the trigger
 * object
 */
void
gsc_manager_unregister_trigger (GscManager *self,
			        GscTrigger *trigger)
{
	g_return_if_fail (GSC_IS_MANAGER (self));
	g_return_if_fail (g_list_find (self->priv->triggers, trigger) != NULL);
	
	self->priv->triggers = g_list_remove (self->priv->triggers, trigger);
	
	if (self->priv->active)
	{
		gsc_trigger_deactivate (trigger);
	}
	g_hash_table_remove (self->priv->trig_prov,
			     gsc_trigger_get_name (trigger));

	g_object_unref (trigger);
}

/**
 * gsc_manager_get_trigger:
 * @self: The #GscManager
 * @trigger_name: The trigger name to get
 *
 * This function return the trigger with this name.
 *
 * Returns: The trigger or NULL if not exists
 *
 */
GscTrigger*
gsc_manager_get_trigger (GscManager *self,
			 const gchar* trigger_name)
{
	GList *plist;
	GscTrigger *trigger;
	
	g_return_val_if_fail (GSC_IS_MANAGER (self), FALSE);
	
	plist = self->priv->triggers;
	
	if (plist != NULL)
	{
		do
		{
			trigger =  GSC_TRIGGER (plist->data);
			
			if (strcmp (gsc_trigger_get_name (trigger),
				    trigger_name) == 0)
			{
				return trigger;
			}
		}while ((plist = g_list_next (plist)) != NULL);
	}
	
	return FALSE;
}

/**
 * gsc_manager_activate:
 * @self: The #GscManager
 *
 * This function activate the completion mechanism. The completion connects 
 * all signals and activate all registered triggers.
 */
void
gsc_manager_activate (GscManager *self)
{
	GList *plist;
	GscTrigger *trigger;

	g_return_if_fail (GSC_MANAGER (self));

	self->priv->internal_signal_ids[IS_GTK_TEXT_VIEW_KP] = 
			g_signal_connect (self->priv->text_view,
					  "key-press-event",
					  G_CALLBACK (view_key_press_event_cb),
					  self);
	self->priv->internal_signal_ids[IS_GTK_TEXT_DESTROY] = 
			g_signal_connect (self->priv->text_view,
					  "destroy",
					  G_CALLBACK (view_destroy_event_cb),
					  self);
	self->priv->internal_signal_ids[IS_GTK_TEXT_FOCUS_OUT] = 
			g_signal_connect (self->priv->text_view,
					  "focus-out-event",
					  G_CALLBACK (view_focus_out_event_cb),
					  self);
	self->priv->internal_signal_ids[IS_GTK_TEXT_BUTTON_PRESS] = 
			g_signal_connect (self->priv->text_view,
					  "button-press-event",
					  G_CALLBACK (view_button_press_event_cb),
					  self);

	/* We activate the triggers*/
	plist = self->priv->triggers;
	
	if (plist != NULL)
	{
		do
		{
			trigger =  GSC_TRIGGER (plist->data);
			
			gsc_trigger_activate (trigger);
		}while ((plist = g_list_next (plist)) != NULL);
	}	
	
	self->priv->active = TRUE;
}

/**
 * gsc_manager_deactivate:
 * @self: The #GscManager
 *
 * This function deactivate the completion mechanism. The completion disconnect
 * all signals and deactivate all registered triggers.
 */
void
gsc_manager_deactivate (GscManager *self)
{
	GList *plist;
	GscTrigger *trigger;
	gint i;
	
	g_return_if_fail (GSC_MANAGER (self));
	
	for (i = 0; i < LAST_SIGNAL; i++)
	{
		if (g_signal_handler_is_connected (self->priv->text_view, 
						   self->priv->internal_signal_ids[i]))
		{
			g_signal_handler_disconnect (self->priv->text_view,
						     self->priv->internal_signal_ids[i]);
		}
		self->priv->internal_signal_ids[i] = 0;
	}
	
	plist = self->priv->triggers;
	
	if (plist != NULL)
	{
		do
		{
			trigger =  GSC_TRIGGER (plist->data);
			
			gsc_trigger_deactivate (trigger);
		}while ((plist = g_list_next (plist)) != NULL);
	}
	
	self->priv->active = FALSE;
}

/**
 * gsc_manager_finish_completion:
 * @self: The #GscManager
 *
 * This function finish the completion if it is active (visible).
 */
void
gsc_manager_finish_completion (GscManager *self)
{
	g_return_if_fail (GSC_MANAGER (self));

	if (gsc_manager_is_visible (self))
	{
		end_completion (self);
	}
}

/**
 * gsc_manager_get_active_trigger:
 * @self: The #GscManager
 *
 * This function return the active trigger. The active trigger is the last
 * trigger raised if the completion is active. If the completion is not visible then
 * there is no an active trigger.
 *
 * Returns: The trigger or NULL if completion is not active
 */
GscTrigger*
gsc_manager_get_active_trigger (GscManager *self)
{
	g_return_val_if_fail (GSC_MANAGER (self), NULL);

	return self->priv->active_trigger;
}

/*
 * FIXME: Use cont gchar * and allocate the memory
 */
/**
 * gsc_manager_set_current_info:
 * @self: The #GscManager
 * @info: Info markup to be shown into for current proposal.
 *
 * You can use this function when a GscProposal emit the 
 * display-info signal to set the current info.
 */
void
gsc_manager_set_current_info (GscManager *self,
			      gchar *info)
{
	gsc_popup_set_current_info (self->priv->popup, info);
}


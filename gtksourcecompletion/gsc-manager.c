/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion.c
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

/* Internal signals */
enum
{
	IS_GTK_TEXT_VIEW_KP,
	IS_GTK_TEXT_DESTROY,
	IS_GTK_TEXT_FOCUS_OUT,
	IS_GTK_TEXT_BUTTON_PRESS,
	IS_LAST_SIGNAL
};

/* Properties */
enum {
	PROP_0,
	PROP_INFO_KEYS,
	PROP_NEXT_PAGE_KEYS,
	PROP_PREV_PAGE_KEYS
};

struct _GscManagerPrivate
{
	GtkTextView *text_view;
	GscPopup *popup;
	GList *triggers;
	/*Providers of the triggers*/
	GHashTable *trig_prov;
	GList *providers;
	gulong internal_signal_ids[IS_LAST_SIGNAL];
	gboolean active;
	GscTrigger *active_trigger;
};

#define GSC_MANAGER_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GSC_TYPE_MANAGER, GscManagerPrivate))

struct _ProviderList
{
	GList *prov_list;
};
typedef struct _ProviderList ProviderList;

static GObjectClass* parent_class = NULL;

/* **************** GtkTextView-GscManager Control *********** */

/*
 * We save a map with a GtkTextView and his GscManager. If you 
 * call twice to gsc_proposal_new, the second time it returns
 * the previous created GscManager, not creates a new one
 */

static GHashTable *completion_map = NULL;

static GscManager* 
completion_control_get_completion(GtkTextView *view)
{
	if (completion_map==NULL)
		completion_map = g_hash_table_new(g_direct_hash,g_direct_equal);

	return g_hash_table_lookup(completion_map,view);
}

static void 
completion_control_add_completion(GtkTextView *view,GscManager *comp)
{
	g_hash_table_insert(completion_map,view,comp);
}

static void 
completion_control_remove_completion(GtkTextView *view)
{
	g_hash_table_remove(completion_map,view);
}
/* ********************************************************************* */

static void
_prov_list_free(gpointer prov_list)
{
	ProviderList *pl = (ProviderList*)prov_list;
	g_list_free(pl->prov_list);
	g_free(pl);
}

static void
end_completion (GscManager *completion)
{
	
	if (GTK_WIDGET_VISIBLE(completion->priv->popup))
	{
		gtk_widget_hide(GTK_WIDGET(completion->priv->popup));
	}
	else
	{
		GList *providers = completion->priv->providers;
		do
		{
			GscProvider *provider =  GSC_PROVIDER(providers->data);
			gsc_provider_finish(provider);
			
		}while((providers = g_list_next(providers)) != NULL);
	
		completion->priv->active_trigger = NULL;
	}
}

static void
_popup_proposal_select_cb(GtkWidget *popup,
		      GscProposal *proposal,
		      gpointer user_data)
{
	GscManager *completion = GSC_MANAGER(user_data);
	gsc_proposal_apply(proposal,completion->priv->text_view);
	end_completion (completion);
}

static void
_popup_hide_cb(GtkWidget *widget,
	       gpointer user_data)
{
	GscManager *self = GSC_MANAGER(user_data);
	end_completion(self);
}

static void
view_destroy_event_cb(GtkWidget *widget,
		      gpointer user_data)
{
	GscManager *completion = GSC_MANAGER(user_data);
	g_object_unref(completion);
}

static gboolean
view_focus_out_event_cb(GtkWidget *widget,
			GdkEventFocus *event,
			gpointer user_data)
{
	GscManager *completion = GSC_MANAGER(user_data);
	if (gsc_manager_is_visible(completion) && GTK_WIDGET_HAS_FOCUS(completion->priv->popup))
	{
		end_completion(completion);
	}
	return FALSE;
}

static gboolean
view_button_press_event_cb(GtkWidget *widget,
			   GdkEventButton *event,
			   gpointer user_data)
{
	GscManager *completion = GSC_MANAGER(user_data);
	if (gsc_manager_is_visible(completion))
	{
		end_completion(completion);
	}
	return FALSE;
}

static void
free_provider_list(gpointer list)
{
	GList *start = (GList*)list;
	GList *temp = (GList*)list;
	if (temp != NULL)
	{
		do
		{
			g_object_unref(G_OBJECT(temp->data));
			
		}while((temp = g_list_next(temp)) != NULL);
		g_list_free(start);
	}
	
}

static void
free_trigger_list(gpointer list)
{
	GList *start = (GList*)list;
	GList *temp = (GList*)list;
	if (temp != NULL)
	{
		do
		{
			g_object_unref(G_OBJECT(temp->data));
			
		}while((temp = g_list_next(temp)) != NULL);
		g_list_free(start);
	}
	
}

static gboolean
view_key_press_event_cb(GtkWidget *view,
			GdkEventKey *event, 
			gpointer user_data)
{
	GscManager *self = GSC_MANAGER(user_data);
	if (gsc_manager_is_visible(self))
		return gsc_popup_manage_key(self->priv->popup,event);
	return FALSE;
}

static void
gsc_manager_set_property (GObject      *object,
				    guint         prop_id,
				    const GValue *value,
				    GParamSpec   *pspec)
{
	GscManager *self;

	g_return_if_fail (GSC_IS_MANAGER (object));

	self = GSC_MANAGER(object);

	switch (prop_id)
	{
		case PROP_INFO_KEYS:
			gsc_popup_set_key(self->priv->popup,KEYS_INFO,g_value_get_string(value));
			break;
		case PROP_NEXT_PAGE_KEYS:
			gsc_popup_set_key(self->priv->popup,KEYS_PAGE_NEXT,g_value_get_string(value));
			break;
		case PROP_PREV_PAGE_KEYS:
			gsc_popup_set_key(self->priv->popup,KEYS_PAGE_PREV,g_value_get_string(value));
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
	GscManager *self;

	g_return_if_fail (GSC_IS_MANAGER (object));

	self = GSC_MANAGER(object);

	switch (prop_id)
	{
		case PROP_INFO_KEYS:
			g_value_set_string(value,
					   gsc_popup_get_key(self->priv->popup,KEYS_INFO));
			break;
		case PROP_NEXT_PAGE_KEYS:
			g_value_set_string(value,
					   gsc_popup_get_key(self->priv->popup,KEYS_PAGE_NEXT));
			break;
		case PROP_PREV_PAGE_KEYS:
			g_value_set_string(value,
					   gsc_popup_get_key(self->priv->popup,KEYS_PAGE_PREV));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gsc_manager_init (GscManager *completion)
{
	if (!lib_initialized)
	{
		bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
		bind_textdomain_codeset(GETTEXT_PACKAGE,"UTF-8");
		lib_initialized = TRUE;
	}
	gint i;
	completion->priv = GSC_MANAGER_GET_PRIVATE(completion);
	
	completion->priv->active = FALSE;
	completion->priv->providers = NULL;
	completion->priv->triggers = NULL;
	completion->priv->popup = NULL;
	completion->priv->active_trigger = NULL;
	completion->priv->trig_prov = g_hash_table_new_full(g_str_hash,
							    g_str_equal,
							    g_free,
							    (GDestroyNotify)_prov_list_free);

	completion->priv->popup = GSC_POPUP(gsc_popup_new());
	
	for (i=0;i<IS_LAST_SIGNAL;i++)
	{
		completion->priv->internal_signal_ids[i] = 0;
	}
	
	g_signal_connect(completion->priv->popup, 
			 "proposal-selected",
			 G_CALLBACK(_popup_proposal_select_cb),
			 (gpointer) completion);
	
	g_signal_connect(completion->priv->popup, 
			 "hide",
			 G_CALLBACK(_popup_hide_cb),
			 (gpointer) completion);

}

static void
gsc_manager_finalize (GObject *object)
{
	GscManager *completion = GSC_MANAGER(object);
	if (completion->priv->active)
	{
		gsc_manager_deactivate(completion);
	}
	
	gtk_widget_destroy(GTK_WIDGET(completion->priv->popup));

	free_provider_list(completion->priv->providers);
	free_trigger_list(completion->priv->triggers);
	g_hash_table_destroy(completion->priv->trig_prov);

	completion_control_remove_completion(completion->priv->text_view);

}

static void
gsc_manager_class_init (GscManagerClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));

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

	g_type_class_add_private (object_class, sizeof(GscManagerPrivate));				
}

GType
gsc_manager_get_type (void)
{
	static GType our_type = 0;

	if(our_type == 0)
	{
		static const GTypeInfo our_info =
		{
			sizeof (GscManagerClass), /* class_size */
			(GBaseInitFunc) NULL, /* base_init */
			(GBaseFinalizeFunc) NULL, /* base_finalize */
			(GClassInitFunc) gsc_manager_class_init, /* class_init */
			(GClassFinalizeFunc) NULL, /* class_finalize */
			NULL /* class_data */,
			sizeof (GscManager), /* instance_size */
			0, /* n_preallocs */
			(GInstanceInitFunc) gsc_manager_init, /* instance_init */
			NULL /* value_table */  
		};

		our_type = g_type_register_static (G_TYPE_OBJECT, "GscManager",
		                                   &our_info, 0);
	}

	return our_type;
}

GscManager*
gsc_manager_new (GtkTextView *view)
{
	g_assert(view!=NULL);
	
	GscManager *completion = completion_control_get_completion(view);
	if (completion !=NULL)
	{
		return completion;
	}

	completion = GSC_MANAGER (g_object_new (GSC_TYPE_MANAGER, NULL));
	completion->priv->text_view = view;
	
	completion_control_add_completion(view,completion);
	
	return completion;
}

gboolean
gsc_manager_register_provider(GscManager *completion,
					GscProvider *provider,
					const gchar *trigger_name)
{
    
	GscTrigger *trigger = gsc_manager_get_trigger(completion,trigger_name);
	if (trigger==NULL) return FALSE;
	ProviderList *pl = g_hash_table_lookup(completion->priv->trig_prov,trigger_name);
	g_assert(pl!=NULL);
	pl->prov_list = g_list_append(pl->prov_list,provider);
	GscProvider *prov = gsc_manager_get_provider(
			completion,gsc_provider_get_name(provider));
	if (prov!=NULL) return FALSE;
	completion->priv->providers = g_list_append(completion->priv->providers,provider);
	g_object_ref(provider);
    
	return TRUE;
}

gboolean
gsc_manager_unregister_provider(GscManager *completion,
					GscProvider *provider,
					const gchar *trigger_name)
{
	g_return_val_if_fail(g_list_find(completion->priv->providers, provider) != NULL,FALSE);
	GscTrigger *trigger = gsc_manager_get_trigger(completion,trigger_name);
	if (trigger==NULL) return FALSE;
	ProviderList *pl = g_hash_table_lookup(completion->priv->trig_prov,trigger_name);
	g_assert(pl!=NULL);
	pl->prov_list = g_list_remove(pl->prov_list,provider);
	
	completion->priv->providers = g_list_remove(completion->priv->providers, provider);
	g_object_unref(provider);
	return TRUE;
}

GtkTextView*
gsc_manager_get_view(GscManager *completion)
{
	return completion->priv->text_view;
}

void
gsc_manager_trigger_event(GscManager *completion, 
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

	trigger = gsc_manager_get_trigger(completion,trigger_name);
	g_return_if_fail(trigger!=NULL);
	
	if (!GTK_WIDGET_HAS_FOCUS(completion->priv->text_view))
		return;
		
	/*
	 * If the completion is visble and there is a trigger active, you cannot
	 * raise a different trigger until the current trigger finish
	 */
	if (gsc_manager_is_visible(completion) && completion->priv->active_trigger != trigger)
	{
		g_debug("no");
		return;
	}
	
	gsc_popup_clear(completion->priv->popup);
	
	ProviderList *pl = g_hash_table_lookup(completion->priv->trig_prov,trigger_name);
	if (pl==NULL) return;
	/*providers_list = completion->priv->providers;*/
	providers_list = pl->prov_list;
	
	if (providers_list != NULL)
	{
		/*Getting the data...*/
		do
		{
			provider =  GSC_PROVIDER(providers_list->data);
			data_list = gsc_provider_get_proposals (provider, trigger);
			if (data_list != NULL)
			{
				original_list = data_list;
				do
				{
					final_list = g_list_append(final_list, data_list->data);
				}while((data_list = g_list_next(data_list)) != NULL);
				g_list_free(original_list);
			}
			
		}while((providers_list = g_list_next(providers_list)) != NULL);
		
		if (final_list!=NULL)
		{
			data_list = final_list;
			/* Insert the data into the model */
			do
			{
				last_proposal = (GscProposal*)data_list->data;
				gsc_popup_add_data(completion->priv->popup,
						   last_proposal);
				++proposals;
				
			}while((data_list = g_list_next(data_list)) != NULL);
			g_list_free(final_list);
			/* If there are not proposals, we don't show the popup */
			if (proposals > 0)
			{
				if (!GTK_WIDGET_HAS_FOCUS(completion->priv->text_view))
					return;

				gint x, y;
				gsc_get_window_position_in_cursor(GTK_WINDOW(completion->priv->popup),completion->priv->text_view,&x,&y);
				gtk_window_move(GTK_WINDOW(completion->priv->popup), x, y);
				gsc_popup_show_or_update(GTK_WIDGET(completion->priv->popup));

				/*Set the focus to the View, not the completion popup*/
				GtkWindow *win = GTK_WINDOW(gtk_widget_get_ancestor(GTK_WIDGET(completion->priv->text_view),GTK_TYPE_WINDOW));
				gtk_window_present(win);
				gtk_widget_grab_focus(GTK_WIDGET(completion->priv->text_view));

				completion->priv->active_trigger = trigger;
			}
			else if (GTK_WIDGET_VISIBLE(completion->priv->popup))
				end_completion (completion);
		}
		else if (gsc_manager_is_visible(completion))
			end_completion (completion);
	}
	else
	{
		if (gsc_manager_is_visible(completion))
			end_completion (completion);
	}
}

gboolean
gsc_manager_is_visible(GscManager *completion)
{
	return GTK_WIDGET_VISIBLE(completion->priv->popup);
}

GscManager*
gsc_manager_get_from_view(GtkTextView *view)
{
	return completion_control_get_completion(view);
}

GscProvider*
gsc_manager_get_provider(GscManager *completion,
				   const gchar* provider_name)
{
	GList *plist = completion->priv->providers;
	GscProvider *provider;	
	if (plist != NULL)
	{
		do
		{
			provider =  GSC_PROVIDER(plist->data);
			if (strcmp(gsc_provider_get_name(provider),provider_name)==0)
			{
				return provider;
			}
		}while((plist = g_list_next(plist)) != NULL);
	}

	return NULL;
}

void
gsc_manager_register_trigger(GscManager *completion,
					GscTrigger *trigger)
{
    const gchar* trigger_name = gsc_trigger_get_name(trigger);
	ProviderList *pl = g_hash_table_lookup(completion->priv->trig_prov,trigger_name);
    /*Only register the trigger if it has not been registered yet*/
    if (pl==NULL)
    {

    	completion->priv->triggers = g_list_append(completion->priv->triggers,trigger);
    	g_object_ref(trigger);
    	const gchar *tn = gsc_trigger_get_name(trigger);
        ProviderList *pl = g_malloc0(sizeof(ProviderList));
        pl->prov_list = NULL;
    	g_hash_table_insert(completion->priv->trig_prov,g_strdup(tn),pl);
    	if (completion->priv->active)
    	{
    		gsc_trigger_activate(trigger);
    	}
    }
}

void
gsc_manager_unregister_trigger(GscManager *completion,
					 GscTrigger *trigger)
{
	g_return_if_fail(g_list_find(completion->priv->triggers, trigger) != NULL);
	completion->priv->triggers = g_list_remove(completion->priv->triggers, trigger);
	if (completion->priv->active)
	{
		gsc_trigger_deactivate(trigger);
	}
	g_hash_table_remove(completion->priv->trig_prov,
			    gsc_trigger_get_name(trigger));
	g_object_unref(trigger);
}

GscTrigger*
gsc_manager_get_trigger(GscManager *completion,
				  const gchar* trigger_name)
{
	GList *plist = completion->priv->triggers;
	GscTrigger *trigger;	
	if (plist != NULL)
	{
		do
		{
			trigger =  GSC_TRIGGER(plist->data);
			if (strcmp(gsc_trigger_get_name(trigger),trigger_name)==0)
			{
				return trigger;
			}
		}while((plist = g_list_next(plist)) != NULL);
	}

	return FALSE;
}

void
gsc_manager_activate(GscManager *completion)
{
	completion->priv->internal_signal_ids[IS_GTK_TEXT_VIEW_KP] = 
			g_signal_connect(completion->priv->text_view,
					 "key-press-event",
					 G_CALLBACK(view_key_press_event_cb),
					 (gpointer) completion);
	completion->priv->internal_signal_ids[IS_GTK_TEXT_DESTROY] = 
			g_signal_connect(completion->priv->text_view,
					 "destroy",
					 G_CALLBACK(view_destroy_event_cb),
					 (gpointer)completion);
	completion->priv->internal_signal_ids[IS_GTK_TEXT_FOCUS_OUT] = 
			g_signal_connect(completion->priv->text_view,
					 "focus-out-event",
					 G_CALLBACK(view_focus_out_event_cb),
					 (gpointer)completion);
	completion->priv->internal_signal_ids[IS_GTK_TEXT_BUTTON_PRESS] = 
			g_signal_connect(completion->priv->text_view,
					 "button-press-event",
					 G_CALLBACK(view_button_press_event_cb),
					 (gpointer)completion);

	/* We activate the triggers*/
	GList *plist = completion->priv->triggers;
	GscTrigger *trigger;	
	if (plist != NULL)
	{
		do
		{
			trigger =  GSC_TRIGGER(plist->data);
			gsc_trigger_activate(trigger);

		}while((plist = g_list_next(plist)) != NULL);
	}	
	
	completion->priv->active = TRUE;
}

void
gsc_manager_deactivate(GscManager *completion)
{
	gint i;
	for (i=0;i<IS_LAST_SIGNAL;i++)
	{
		if (g_signal_handler_is_connected(completion->priv->text_view, 
						  completion->priv->internal_signal_ids[i]))
		{
			g_signal_handler_disconnect (completion->priv->text_view,
				completion->priv->internal_signal_ids[i]);
		}
		completion->priv->internal_signal_ids[i] = 0;
	}
	
	GList *plist = completion->priv->triggers;
	GscTrigger *trigger;	
	if (plist != NULL)
	{
		do
		{
			trigger =  GSC_TRIGGER(plist->data);
			gsc_trigger_deactivate(trigger);

		}while((plist = g_list_next(plist)) != NULL);
	}	
	
	completion->priv->active = FALSE;
}

void
gsc_manager_finish_completion(GscManager *completion)
{
	if (gsc_manager_is_visible(completion))
	{
		end_completion(completion);
	}
}

GscTrigger*
gsc_manager_get_active_trigger(GscManager *completion)
{
	return completion->priv->active_trigger;
}

void
gsc_manager_set_current_info(GscManager *self,
			     gchar *info)
{
	gsc_popup_set_current_info(self->priv->popup,info);
}


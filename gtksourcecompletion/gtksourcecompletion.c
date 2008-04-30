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
#include "gtksourcecompletion.h"
#include "gtksourcecompletion-i18n.h"
#include "gtksourcecompletion-item.h"
#include "gtksourcecompletion-utils.h"
#include "gsv-completion-popup.h"

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

typedef enum {
	KEYS_INFO,
	KEYS_PAGE_NEXT,
	KEYS_PAGE_PREV,
	KEYS_LAST
} KeysType;

typedef struct 
{
	guint key;
	GdkModifierType mods;
} CompletionKeys;

struct _GtkSourceCompletionPrivate
{
	GtkTextView *text_view;
	GsvCompletionPopup *popup;
	GList *triggers;
	/*Providers of the triggers*/
	GHashTable *trig_prov;
	GList *providers;
	gulong internal_signal_ids[IS_LAST_SIGNAL];
	gboolean active;
	CompletionKeys keys[KEYS_LAST];
	const gchar *active_trigger;
};

struct _GtkSourceCompletionItem
{
	int id;
	gchar *name;
	const GdkPixbuf *icon;
	int priority;
	gpointer user_data;
};

#define GTK_SOURCE_COMPLETION_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE ((object), GTK_TYPE_SOURCE_COMPLETION, GtkSourceCompletionPrivate))

struct _ProviderList
{
	GList *prov_list;
};
typedef struct _ProviderList ProviderList;

static GObjectClass* parent_class = NULL;

/* **************** GtkTextView-GtkSourceCompletion Control *********** */

/*
 * We save a map with a GtkTextView and his GtkSourceCompletion. If you 
 * call twice to gtk_source_completion_item_new, the second time it returns
 * the previous created GtkSourceCompletion, not creates a new one
 */

static GHashTable *completion_map = NULL;

static GtkSourceCompletion* 
completion_control_get_completion(GtkTextView *view)
{
	if (completion_map==NULL)
		completion_map = g_hash_table_new(g_direct_hash,g_direct_equal);

	return g_hash_table_lookup(completion_map,view);
}

static void 
completion_control_add_completion(GtkTextView *view,GtkSourceCompletion *comp)
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

static gint
_item_priority_compare (gconstpointer v1,
					gconstpointer v2)
{
	GtkSourceCompletionItem *i1 = (GtkSourceCompletionItem*) v1;
	GtkSourceCompletionItem *i2 = (GtkSourceCompletionItem*) v2;
	
	return i2->priority - i1->priority;
	
}

static void
end_completion (GtkSourceCompletion *completion)
{
	gtk_widget_hide(GTK_WIDGET(completion->priv->popup));

	GList *providers = completion->priv->providers;
	do
	{
		GtkSourceCompletionProvider *provider =  GTK_SOURCE_COMPLETION_PROVIDER(providers->data);
		gtk_source_completion_provider_end_completion(provider,completion);
			
	}while((providers = g_list_next(providers)) != NULL);
	
	completion->priv->active_trigger = NULL;
}

static void
_set_keys(GtkSourceCompletion *completion, KeysType type, const gchar* keys)
{
	guint key;
	GdkModifierType mods;
	gtk_accelerator_parse(keys,&key,&mods);
	g_return_if_fail(key!=0);
	completion->priv->keys[type].key = key;
	completion->priv->keys[type].mods = mods;
}

static gboolean
_compare_keys(GtkSourceCompletion *completion, KeysType type, GdkEventKey *event)
{
	guint modifiers = gtk_accelerator_get_default_mod_mask ();
	if (completion->priv->keys[type].mods == 0 &&
	    (event->state & modifiers)==0)
	{
		if (event->keyval == completion->priv->keys[type].key)
		{
			return TRUE;
		}
	}else if ((event->state & completion->priv->keys[type].mods) && 
	    event->keyval == completion->priv->keys[type].key)
	{
		return TRUE;
	}
	
	return FALSE;
}

static gboolean
_popup_tree_selection(GtkSourceCompletion *completion)
{
	GtkSourceCompletionItem *item;
	GtkSourceCompletionProvider *prov;
	if (gsv_completion_popup_get_selected_item(completion->priv->popup,&item))
	{
		prov = gtk_source_completion_item_get_provider(item);
		gtk_source_completion_provider_data_selected(prov,completion, item);
		end_completion (completion);
		return TRUE;
	}
	
	return FALSE;

}

static gboolean
view_key_press_event_cb(GtkWidget *view,
					GdkEventKey *event, 
					gpointer user_data)
{
	/* Catch only keys of popup movement */
	gboolean ret = FALSE;
	gboolean catched = FALSE;
	GtkSourceCompletion *completion;
	g_assert(GTK_IS_SOURCE_COMPLETION(user_data));
	completion = GTK_SOURCE_COMPLETION(user_data);
	
	if (gtk_source_completion_is_visible(completion))
	{
		guint modifiers = gtk_accelerator_get_default_mod_mask ();
		if ((event->state & modifiers)==0)
		{
			switch (event->keyval)
		 	{
				case GDK_Escape:
				case GDK_space:
				{
					end_completion (completion);
					catched = TRUE;
					break;
				}
		 		case GDK_Down:
				{
					ret = gsv_completion_popup_select_next(completion->priv->popup, 1);
					catched = TRUE;
					break;
				}
				case GDK_Page_Down:
				{
					ret = gsv_completion_popup_select_next(completion->priv->popup, 5);
					catched = TRUE;
					break;
				}
				case GDK_Up:
				{
					if (gsv_completion_popup_select_previous(completion->priv->popup, 1))
					{
						ret = TRUE;
					}
					else
					{
						ret = gsv_completion_popup_select_first(completion->priv->popup);
					}
					catched = TRUE;
					break;
				}
				case GDK_Page_Up:
				{
					ret = gsv_completion_popup_select_previous(completion->priv->popup, 5);
					catched = TRUE;
					break;
				}
				case GDK_Home:
				{
					ret = gsv_completion_popup_select_first(completion->priv->popup);
					catched = TRUE;
					break;
				}
				case GDK_End:
				{
					ret = gsv_completion_popup_select_last(completion->priv->popup);
					catched = TRUE;
					break;
				}
				case GDK_Return:
				case GDK_Tab:
				{
					ret = _popup_tree_selection(completion);
					if (!ret)
					{
						end_completion(completion);
					}
					catched = TRUE;
					break;
				}
			}
		}
		if (!catched)
		{
			if (_compare_keys(completion,KEYS_INFO,event))
			{
				/*View information of the item */
				gsv_completion_popup_toggle_item_info(completion->priv->popup);
				ret = TRUE;
			}else if (_compare_keys(completion,KEYS_PAGE_NEXT,event))
			{
				g_debug("next");
				gsv_completion_popup_page_next(completion->priv->popup);
				ret = TRUE;
				
			}else if (_compare_keys(completion,KEYS_PAGE_PREV,event))
			{
				g_debug("prev");
				gsv_completion_popup_page_previous(completion->priv->popup);
				ret = TRUE;
				
			}
		}

	}
	
	return ret;

}

static void
_popup_item_select_cb(GtkWidget *popup,
							GtkSourceCompletionItem *item,
							gpointer user_data)
{
	GtkSourceCompletion *completion = GTK_SOURCE_COMPLETION(user_data);
	GtkSourceCompletionProvider *prov;
	prov = gtk_source_completion_item_get_provider(item);
	gtk_source_completion_provider_data_selected(prov,completion, item);
	end_completion (completion);
}

static void
view_destroy_event_cb(GtkWidget *widget,
				gpointer user_data)
{
	GtkSourceCompletion *completion = GTK_SOURCE_COMPLETION(user_data);
	g_object_unref(completion);
}

static gboolean
view_focus_out_event_cb(GtkWidget *widget,
				GdkEventFocus *event,
				gpointer user_data)
{
	GtkSourceCompletion *completion = GTK_SOURCE_COMPLETION(user_data);
	if (gtk_source_completion_is_visible(completion))
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
	GtkSourceCompletion *completion = GTK_SOURCE_COMPLETION(user_data);
	if (gtk_source_completion_is_visible(completion))
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

static void
gtk_source_completion_set_property (GObject      *object,
				    guint         prop_id,
				    const GValue *value,
				    GParamSpec   *pspec)
{
	GtkSourceCompletion *self;

	g_return_if_fail (GTK_IS_SOURCE_COMPLETION (object));

	self = GTK_SOURCE_COMPLETION(object);

	switch (prop_id)
	{
		case PROP_INFO_KEYS:
			_set_keys(self, KEYS_INFO, g_value_get_string(value));
			break;
		case PROP_NEXT_PAGE_KEYS:
			_set_keys(self,KEYS_PAGE_NEXT,g_value_get_string(value));
			break;
		case PROP_PREV_PAGE_KEYS:
			_set_keys(self,KEYS_PAGE_PREV,g_value_get_string(value));
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gtk_source_completion_get_property (GObject    *object,
				guint       prop_id,
				GValue     *value,
				GParamSpec *pspec)
{
	GtkSourceCompletion *self;

	g_return_if_fail (GTK_IS_SOURCE_COMPLETION (object));

	self = GTK_SOURCE_COMPLETION(object);

	switch (prop_id)
	{
		case PROP_INFO_KEYS:
			g_value_set_string(value,gtk_accelerator_name(self->priv->keys[KEYS_INFO].key,
								      self->priv->keys[KEYS_INFO].mods));
			break;
		case PROP_NEXT_PAGE_KEYS:
			g_value_set_string(value,gtk_accelerator_name(self->priv->keys[KEYS_PAGE_NEXT].key,
								      self->priv->keys[KEYS_PAGE_NEXT].mods));
			break;
		case PROP_PREV_PAGE_KEYS:
			g_value_set_string(value,gtk_accelerator_name(self->priv->keys[KEYS_PAGE_PREV].key,
								      self->priv->keys[KEYS_PAGE_PREV].mods));
			break;

		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, prop_id, pspec);
			break;
	}
}

static void
gtk_source_completion_init (GtkSourceCompletion *completion)
{
	g_debug("Init GtkSourceCompletion");
	if (!lib_initialized)
	{
		bindtextdomain (GETTEXT_PACKAGE, PACKAGE_LOCALE_DIR);
		bind_textdomain_codeset(GETTEXT_PACKAGE,"UTF-8");
		lib_initialized = TRUE;
	}
	gint i;
	completion->priv = GTK_SOURCE_COMPLETION_GET_PRIVATE(completion);
	
	completion->priv->active = FALSE;
	completion->priv->providers = NULL;
	completion->priv->triggers = NULL;
	completion->priv->popup = NULL;
	completion->priv->active_trigger = NULL;
	completion->priv->trig_prov = g_hash_table_new_full(g_str_hash,
			g_str_equal,
			g_free,
			(GDestroyNotify)_prov_list_free);
	
	_set_keys(completion, KEYS_INFO, DEFAULT_INFO_KEYS);
	_set_keys(completion, KEYS_PAGE_NEXT, DEFAULT_PAGE_NEXT_KEYS);
	_set_keys(completion, KEYS_PAGE_PREV, DEFAULT_PAGE_PREV_KEYS);
	
	for (i=0;i<IS_LAST_SIGNAL;i++)
	{
		completion->priv->internal_signal_ids[i] = 0;
	}

}

static void
gtk_source_completion_finalize (GObject *object)
{
	GtkSourceCompletion *completion = GTK_SOURCE_COMPLETION(object);
	g_debug("Finish GtkSourceCompletion");
	if (completion->priv->active)
	{
		gtk_source_completion_deactivate(completion);
	}
	
	gtk_widget_destroy(GTK_WIDGET(completion->priv->popup));

	free_provider_list(completion->priv->providers);
	free_trigger_list(completion->priv->triggers);
	g_hash_table_destroy(completion->priv->trig_prov);

	completion_control_remove_completion(completion->priv->text_view);

}

static void
gtk_source_completion_class_init (GtkSourceCompletionClass *klass)
{
	GObjectClass* object_class = G_OBJECT_CLASS (klass);
	parent_class = G_OBJECT_CLASS (g_type_class_peek_parent (klass));

	object_class->get_property = gtk_source_completion_get_property;
	object_class->set_property = gtk_source_completion_set_property;
	object_class->finalize = gtk_source_completion_finalize;

	/**
	 * GtkSourceCompletion:info-keys:
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
	 * GtkSourceCompletion:next-page-keys:
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
	 * GtkSourceCompletion:previous-page-keys:
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

	g_type_class_add_private (object_class, sizeof(GtkSourceCompletionPrivate));				
}

GType
gtk_source_completion_get_type (void)
{
	static GType our_type = 0;

	if(our_type == 0)
	{
		static const GTypeInfo our_info =
		{
			sizeof (GtkSourceCompletionClass), /* class_size */
			(GBaseInitFunc) NULL, /* base_init */
			(GBaseFinalizeFunc) NULL, /* base_finalize */
			(GClassInitFunc) gtk_source_completion_class_init, /* class_init */
			(GClassFinalizeFunc) NULL, /* class_finalize */
			NULL /* class_data */,
			sizeof (GtkSourceCompletion), /* instance_size */
			0, /* n_preallocs */
			(GInstanceInitFunc) gtk_source_completion_init, /* instance_init */
			NULL /* value_table */  
		};

		our_type = g_type_register_static (G_TYPE_OBJECT, "GtkSourceCompletion",
		                                   &our_info, 0);
	}

	return our_type;
}

GtkSourceCompletion*
gtk_source_completion_new (GtkTextView *view)
{

	g_assert(view!=NULL);
	
	GtkSourceCompletion *completion = completion_control_get_completion(view);
	if (completion !=NULL)
	{
		return completion;
	}

	completion = GTK_SOURCE_COMPLETION (g_object_new (GTK_TYPE_SOURCE_COMPLETION, NULL));
	completion->priv->text_view = view;
	
	completion->priv->popup = GSV_COMPLETION_POPUP(gsv_completion_popup_new(view));
	
	g_signal_connect(completion->priv->popup, 
						"item-selected",
						G_CALLBACK(_popup_item_select_cb),
						(gpointer) completion);

	completion_control_add_completion(view,completion);
	
	return completion;
}

gboolean
gtk_source_completion_register_provider(GtkSourceCompletion *completion,
					GtkSourceCompletionProvider *provider,
					const gchar *trigger_name)
{
    
	GtkSourceCompletionTrigger *trigger = gtk_source_completion_get_trigger(completion,trigger_name);
	if (trigger==NULL) return FALSE;
	ProviderList *pl = g_hash_table_lookup(completion->priv->trig_prov,trigger_name);
	g_assert(pl!=NULL);
	pl->prov_list = g_list_append(pl->prov_list,provider);
	GtkSourceCompletionProvider *prov = gtk_source_completion_get_provider(
			completion,gtk_source_completion_provider_get_name(provider));
	if (prov!=NULL) return FALSE;
	completion->priv->providers = g_list_append(completion->priv->providers,provider);
	g_object_ref(provider);
    
	return TRUE;
}

gboolean
gtk_source_completion_unregister_provider(GtkSourceCompletion *completion,
					GtkSourceCompletionProvider *provider,
					const gchar *trigger_name)
{
	g_return_val_if_fail(g_list_find(completion->priv->providers, provider) != NULL,FALSE);
	GtkSourceCompletionTrigger *trigger = gtk_source_completion_get_trigger(completion,trigger_name);
	if (trigger==NULL) return FALSE;
	ProviderList *pl = g_hash_table_lookup(completion->priv->trig_prov,trigger_name);
	g_assert(pl!=NULL);
	pl->prov_list = g_list_remove(pl->prov_list,provider);
	
	completion->priv->providers = g_list_remove(completion->priv->providers, provider);
	g_object_unref(provider);
	return TRUE;
}

GtkTextView*
gtk_source_completion_get_view(GtkSourceCompletion *completion)
{
	return completion->priv->text_view;
}

void
gtk_source_completion_trigger_event(GtkSourceCompletion *completion, 
					const gchar *trigger_name, 
					gpointer event_data)
{
	GList* data_list;
	GList* original_list;
	GList* final_list = NULL;
	GList *providers_list;
	GtkSourceCompletionProvider *provider;
	GtkSourceCompletionTrigger *trigger;

	trigger = gtk_source_completion_get_trigger(completion,trigger_name);
	g_return_if_fail(trigger!=NULL);
	
	if (!GTK_WIDGET_HAS_FOCUS(completion->priv->text_view))
		return;
	
	gsv_completion_popup_clear(completion->priv->popup);
	
	ProviderList *pl = g_hash_table_lookup(completion->priv->trig_prov,trigger_name);
	if (pl==NULL) return;
	/*providers_list = completion->priv->providers;*/
	providers_list = pl->prov_list;
	
	if (providers_list != NULL)
	{
		/*Getting the data...*/
		do
		{
			provider =  GTK_SOURCE_COMPLETION_PROVIDER(providers_list->data);
			data_list = gtk_source_completion_provider_get_data (
							provider, completion, trigger);
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
			/*Order the data*/
			final_list = g_list_sort (final_list,_item_priority_compare);
			data_list = final_list;
			/* Insert the data into the model */
			do
			{
				gsv_completion_popup_add_data(completion->priv->popup,
									(GtkSourceCompletionItem*)data_list->data);
			}while((data_list = g_list_next(data_list)) != NULL);
			g_list_free(final_list);
			/* If there are not items, we don't show the popup */
			if (gsv_completion_popup_has_items(completion->priv->popup))
			{
				if (!GTK_WIDGET_HAS_FOCUS(completion->priv->text_view))
					return;
				gsv_completion_popup_refresh(completion->priv->popup);
				completion->priv->active_trigger = trigger_name;
			}
			else
			{
				if (GTK_WIDGET_VISIBLE(completion->priv->popup))
				{
					end_completion (completion);
				}
			}
		}
		else
		{
			if (gtk_source_completion_is_visible(completion))
			{
				end_completion (completion);
			}
		}
	}
	else
	{
		if (gtk_source_completion_is_visible(completion))
		{
			end_completion (completion);
		}
	}
	
}

gboolean
gtk_source_completion_is_visible(GtkSourceCompletion *completion)
{
	return GTK_WIDGET_VISIBLE(completion->priv->popup);
}

GtkSourceCompletion*
gtk_source_completion_get_from_view(
																GtkTextView *view)
{
	return completion_control_get_completion(view);
}

GtkSourceCompletionProvider*
gtk_source_completion_get_provider(GtkSourceCompletion *completion,
								const gchar* provider_name)
{
	GList *plist = completion->priv->providers;
	GtkSourceCompletionProvider *provider;	
	if (plist != NULL)
	{
		do
		{
			provider =  GTK_SOURCE_COMPLETION_PROVIDER(plist->data);
			if (strcmp(gtk_source_completion_provider_get_name(provider),provider_name)==0)
			{
				return provider;
			}
		}while((plist = g_list_next(plist)) != NULL);
	}

	return NULL;
}

void
gtk_source_completion_register_trigger(GtkSourceCompletion *completion,
								GtkSourceCompletionTrigger *trigger)
{
    const gchar* trigger_name = gtk_source_completion_trigger_get_name(trigger);
	ProviderList *pl = g_hash_table_lookup(completion->priv->trig_prov,trigger_name);
    /*Only register the trigger if it has not been registered yet*/
    if (pl==NULL)
    {

    	completion->priv->triggers = g_list_append(completion->priv->triggers,trigger);
    	g_object_ref(trigger);
    	const gchar *tn = gtk_source_completion_trigger_get_name(trigger);
        ProviderList *pl = g_malloc0(sizeof(ProviderList));
        pl->prov_list = NULL;
    	g_hash_table_insert(completion->priv->trig_prov,g_strdup(tn),pl);
    	if (completion->priv->active)
    	{
    		gtk_source_completion_trigger_activate(trigger);
    	}
    }
}

void
gtk_source_completion_unregister_trigger(GtkSourceCompletion *completion,
								GtkSourceCompletionTrigger *trigger)
{
	g_return_if_fail(g_list_find(completion->priv->triggers, trigger) != NULL);
	completion->priv->triggers = g_list_remove(completion->priv->triggers, trigger);
	if (completion->priv->active)
	{
		gtk_source_completion_trigger_deactivate(trigger);
	}
	g_hash_table_remove(completion->priv->trig_prov,gtk_source_completion_trigger_get_name(trigger));
	g_object_unref(trigger);
}

GtkSourceCompletionTrigger*
gtk_source_completion_get_trigger(GtkSourceCompletion *completion,
								const gchar* trigger_name)
{
	GList *plist = completion->priv->triggers;
	GtkSourceCompletionTrigger *trigger;	
	if (plist != NULL)
	{
		do
		{
			trigger =  GTK_SOURCE_COMPLETION_TRIGGER(plist->data);
			if (strcmp(gtk_source_completion_trigger_get_name(trigger),trigger_name)==0)
			{
				return trigger;
			}
		}while((plist = g_list_next(plist)) != NULL);
	}

	return FALSE;
}

void
gtk_source_completion_activate(GtkSourceCompletion *completion)
{
	g_debug("Activating GtkSourceCompletion");
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
	GtkSourceCompletionTrigger *trigger;	
	if (plist != NULL)
	{
		do
		{
			trigger =  GTK_SOURCE_COMPLETION_TRIGGER(plist->data);
			gtk_source_completion_trigger_activate(trigger);

		}while((plist = g_list_next(plist)) != NULL);
	}	
	
	completion->priv->active = TRUE;
}

void
gtk_source_completion_deactivate(GtkSourceCompletion *completion)
{
	g_debug("Deactivating GtkSourceCompletion");
	gint i;
	for (i=0;i<IS_LAST_SIGNAL;i++)
	{
		if (g_signal_handler_is_connected(completion->priv->text_view, completion->priv->internal_signal_ids[i]))
		{
			g_signal_handler_disconnect (completion->priv->text_view,
				completion->priv->internal_signal_ids[i]);
		}
		completion->priv->internal_signal_ids[i] = 0;
	}
	
	GList *plist = completion->priv->triggers;
	GtkSourceCompletionTrigger *trigger;	
	if (plist != NULL)
	{
		do
		{
			trigger =  GTK_SOURCE_COMPLETION_TRIGGER(plist->data);
			gtk_source_completion_trigger_deactivate(trigger);

		}while((plist = g_list_next(plist)) != NULL);
	}	
	
	completion->priv->active = FALSE;
}

void
gtk_source_completion_finish_completion(GtkSourceCompletion *completion)
{
	if (gtk_source_completion_is_visible(completion))
	{
		end_completion(completion);
	}
}

const gchar*
gtk_source_completion_get_active_trigger_name(GtkSourceCompletion *completion)
{
	return completion->priv->active_trigger;
}

	

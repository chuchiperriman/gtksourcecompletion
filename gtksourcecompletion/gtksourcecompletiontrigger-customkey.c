 /* gsc-trigger-customkey.c - Type here a short description of your trigger
 *
 * Copyright (C) 2008 - perriman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:gsc-trigger-customkey
 * @title: GtkSourceCompletionTriggerCustomkey
 * @short_description: Custom keys trigger
 *
 * This object trigger a completion event when the user press the configured
 * keys.
 * 
 */

#include <glib/gprintf.h>
#include <string.h>
#include <ctype.h>
#include "gsc-utils.h"
#include "gtksourcecompletiontrigger-customkey.h"

#define GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
								GTK_TYPE_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY, \
								GtkSourceCompletionTriggerCustomkeyPrivate))

/* User request signals */
enum
{
	CKP_GTK_TEXT_VIEW_KP,
	LAST_SIGNAL
};

struct _GtkSourceCompletionTriggerCustomkeyPrivate
{
	GtkSourceCompletion *completion;
	
	gulong signals[LAST_SIGNAL];
	gchar *trigger_name;
	guint key;
	GdkModifierType mod;
};

static void	 gtk_source_completion_trigger_customkey_iface_init	(GtkSourceCompletionTriggerIface *iface);

G_DEFINE_TYPE_WITH_CODE (GtkSourceCompletionTriggerCustomkey,
			 gtk_source_completion_trigger_customkey,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_SOURCE_COMPLETION_TRIGGER,
				 		gtk_source_completion_trigger_customkey_iface_init))

static gboolean
view_key_press_event_cb (GtkWidget *view,
			 GdkEventKey *event, 
			 gpointer user_data)
{
	GtkSourceCompletionTriggerCustomkey *self;
	guint s;
	guint key;
	
	self = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY (user_data);
	s = event->state & gtk_accelerator_get_default_mod_mask();
	key = gdk_keyval_to_lower (self->priv->key);
	
	if (s == self->priv->mod && gdk_keyval_to_lower(event->keyval) == key)
	{
		gtk_source_completion_trigger_event (self->priv->completion,
						     GTK_SOURCE_COMPLETION_TRIGGER (self));
		return TRUE;
	}
	
	return FALSE;
}

static const gchar *
gtk_source_completion_trigger_customkey_real_get_name (GtkSourceCompletionTrigger *base)
{
	GtkSourceCompletionTriggerCustomkey *self;
	
	self = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY (base);
	
	return self->priv->trigger_name;
}

static gboolean
gtk_source_completion_trigger_customkey_real_activate (GtkSourceCompletionTrigger* base)
{
	GtkSourceCompletionTriggerCustomkey *self;
	GtkTextView *view;

	self = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY (base);
	view = gtk_source_completion_get_view (self->priv->completion);
	g_assert (GTK_IS_TEXT_VIEW (view));
	
	self->priv->signals[CKP_GTK_TEXT_VIEW_KP] =  
			g_signal_connect_data (view,
					       "key-press-event",
					       G_CALLBACK (view_key_press_event_cb),
					       (gpointer) self,
					       NULL,
					       0);
	return TRUE;
}

static gboolean
gtk_source_completion_trigger_customkey_real_deactivate (GtkSourceCompletionTrigger* base)
{
	GtkSourceCompletionTriggerCustomkey *self;
	GtkTextView *view;
	
	self = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY (base);
	view = gtk_source_completion_get_view (self->priv->completion);
	
	g_signal_handler_disconnect (view,
				     self->priv->signals[CKP_GTK_TEXT_VIEW_KP]);
	
	return FALSE;
}

static void 
gtk_source_completion_trigger_customkey_init (GtkSourceCompletionTriggerCustomkey * self)
{
	self->priv = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY_GET_PRIVATE (self);
	
	self->priv->trigger_name = NULL;
}

static void 
gtk_source_completion_trigger_customkey_finalize(GObject *object)
{
	GtkSourceCompletionTriggerCustomkey *self;
	
	self = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY (object);
	
	g_free (self->priv->trigger_name);
	g_object_unref (self->priv->completion);

	G_OBJECT_CLASS (gtk_source_completion_trigger_customkey_parent_class)->finalize (object);
}

static void 
gtk_source_completion_trigger_customkey_class_init (GtkSourceCompletionTriggerCustomkeyClass * klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	g_type_class_add_private (klass, sizeof (GtkSourceCompletionTriggerCustomkeyPrivate));

	object_class->finalize = gtk_source_completion_trigger_customkey_finalize;
}

static void 
gtk_source_completion_trigger_customkey_iface_init (GtkSourceCompletionTriggerIface * iface)
{
	iface->get_name   = gtk_source_completion_trigger_customkey_real_get_name;
	iface->activate   = gtk_source_completion_trigger_customkey_real_activate;
	iface->deactivate = gtk_source_completion_trigger_customkey_real_deactivate;
}

/**
 * gtk_source_completion_trigger_customkey_new:
 * @completion: The #GtkSourceCompletion
 * @trigger_name: The trigger name wich will be user the we trigger the event.
 * @keys: The string representation of the keys that we will
 * use to activate the event. You can get this 
 * string with #gtk_accelerator_name
 *
 * This is a generic trigger. You tell the name and the key and the trigger
 * will be triggered when the user press this key (or keys).
 *
 * Returns: a new #GtkSourceCompletionTriggerCustomkey
 *
 */
GtkSourceCompletionTriggerCustomkey* 
gtk_source_completion_trigger_customkey_new (GtkSourceCompletion *completion,
					     const gchar* trigger_name,
					     const gchar* keys)
{
	GtkSourceCompletionTriggerCustomkey *self;
	
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION (completion), NULL);
	g_return_val_if_fail (trigger_name != NULL, NULL);
	
	self = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY (g_object_new (GTK_TYPE_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY,
								      NULL));
	
	self->priv->completion = g_object_ref (completion);
	self->priv->trigger_name = g_strdup (trigger_name);
	gtk_source_completion_trigger_customkey_set_keys (self, keys);
	
	return self;
}

/**
 * gtk_source_completion_trigger_customkey_set_keys:
 * @self: The #GtkSourceCompletionTriggerCustomkey 
 * @keys: The string representation of the keys that we will
 * use to activate the user request event. You can get this 
 * string with #gtk_accelerator_name
 *
 * Assign the keys that we will use to activate the user request event.
 */
void
gtk_source_completion_trigger_customkey_set_keys (GtkSourceCompletionTriggerCustomkey *self,
						  const gchar* keys)
{
	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY (self));

	gtk_accelerator_parse (keys, &self->priv->key, &self->priv->mod);
}



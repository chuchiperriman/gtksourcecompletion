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

#include <glib/gprintf.h>
#include <string.h>
#include <ctype.h>
#include "gsc-utils.h"
#include "gsc-trigger-customkey.h"

#define GSC_TRIGGER_CUSTOMKEY_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
					      GSC_TYPE_TRIGGER_CUSTOMKEY, \
					      GscTriggerCustomkeyPrivate))

/* User request signals */
enum
{
	CKP_GTK_TEXT_VIEW_KP,
	LAST_SIGNAL
};

struct _GscTriggerCustomkeyPrivate
{
	GscCompletion *completion;
	
	gulong signals[LAST_SIGNAL];
	gchar *trigger_name;
	guint key;
	GdkModifierType mod;
};

static void	 gsc_trigger_customkey_iface_init	(GscTriggerIface *iface);

G_DEFINE_TYPE_WITH_CODE (GscTriggerCustomkey,
			 gsc_trigger_customkey,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GSC_TYPE_TRIGGER,
				 		gsc_trigger_customkey_iface_init))

static gboolean
view_key_press_event_cb (GtkWidget *view,
			 GdkEventKey *event, 
			 gpointer user_data)
{
	GscTriggerCustomkey *self = GSC_TRIGGER_CUSTOMKEY (user_data);
	
	guint key = gdk_keyval_to_lower (self->priv->key);
	
	if ((event->state & self->priv->mod) ==  self->priv->mod &&
		gdk_keyval_to_lower (event->keyval) == key)
	{
		gsc_completion_trigger_event (self->priv->completion,
					      GSC_TRIGGER (self));
		return TRUE;
	}
	return FALSE;
}

static const gchar* 
gsc_trigger_customkey_real_get_name (GscTrigger *base)
{
	GscTriggerCustomkey *self = GSC_TRIGGER_CUSTOMKEY (base);
	
	return self->priv->trigger_name;
}

static gboolean
gsc_trigger_customkey_real_activate (GscTrigger* base)
{
	GscTriggerCustomkey *self = GSC_TRIGGER_CUSTOMKEY (base);
	GtkTextView *view;

	view = gsc_completion_get_view (self->priv->completion);
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
gsc_trigger_customkey_real_deactivate (GscTrigger* base)
{
	GscTriggerCustomkey *self = GSC_TRIGGER_CUSTOMKEY (base);
	GtkTextView *view;
	
	view = gsc_completion_get_view (self->priv->completion);
	
	g_signal_handler_disconnect (view,
				     self->priv->signals[CKP_GTK_TEXT_VIEW_KP]);
	
	return FALSE;
}

static void 
gsc_trigger_customkey_init (GscTriggerCustomkey * self)
{
	self->priv = GSC_TRIGGER_CUSTOMKEY_GET_PRIVATE (self);
	
	self->priv->trigger_name = NULL;
}

static void 
gsc_trigger_customkey_finalize(GObject *object)
{
	GscTriggerCustomkey *self = GSC_TRIGGER_CUSTOMKEY (object);
	
	g_free (self->priv->trigger_name);
	g_object_unref (self->priv->completion);

	G_OBJECT_CLASS (gsc_trigger_customkey_parent_class)->finalize (object);
}

static void 
gsc_trigger_customkey_class_init (GscTriggerCustomkeyClass * klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	g_type_class_add_private (klass, sizeof (GscTriggerCustomkeyPrivate));

	object_class->finalize = gsc_trigger_customkey_finalize;
}

static void 
gsc_trigger_customkey_iface_init (GscTriggerIface * iface)
{
	iface->get_name   = gsc_trigger_customkey_real_get_name;
	iface->activate   = gsc_trigger_customkey_real_activate;
	iface->deactivate = gsc_trigger_customkey_real_deactivate;
}

/**
 * gsc_trigger_customkey_new:
 * @completion: The #GscCompletion
 * @trigger_name: The trigger name wich will be user the we trigger the event.
 * @keys: The string representation of the keys that we will
 * use to activate the event. You can get this 
 * string with #gtk_accelerator_name
 *
 * This is a generic trigger. You tell the name and the key and the trigger
 * will be triggered when the user press this key (or keys).
 *
 * Returns: a new #GscTriggerCustomkey
 *
 */
GscTriggerCustomkey* 
gsc_trigger_customkey_new (GscCompletion *completion,
			   const gchar* trigger_name, 
			   const gchar* keys)
{

	g_return_val_if_fail (GSC_IS_COMPLETION (completion), NULL);
	g_return_val_if_fail (trigger_name != NULL, NULL);

	GscTriggerCustomkey *self;
	
	self = GSC_TRIGGER_CUSTOMKEY (g_object_new (GSC_TYPE_TRIGGER_CUSTOMKEY,
				      NULL));
	
	self->priv->completion = g_object_ref (completion);
	self->priv->trigger_name = g_strdup (trigger_name);
	gsc_trigger_customkey_set_keys (self, keys);
	
	return self;
}

/**
 * gsc_trigger_customkey_set_keys:
 * @self: The #GscTriggerCustomkey 
 * @keys: The string representation of the keys that we will
 * use to activate the user request event. You can get this 
 * string with #gtk_accelerator_name
 *
 * Assign the keys that we will use to activate the user request event.
 */
void
gsc_trigger_customkey_set_keys (GscTriggerCustomkey *self,
				const gchar* keys)
{
	g_return_if_fail (GSC_IS_TRIGGER_CUSTOMKEY (self));

	gtk_accelerator_parse (keys, &self->priv->key, &self->priv->mod);
}



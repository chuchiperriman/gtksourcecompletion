 /* gsc-userrequest-trigger.c - Type here a short description of your plugin
 *
 * Copyright (C) 2007 - perriman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include <glib/gprintf.h>
#include <string.h>
#include <ctype.h>
#include "gtksourcecompletion-utils.h"
#include "gsc-userrequest-trigger.h"

/* User request signals */
enum
{
	URS_GTK_TEXT_VIEW_KP,
	URS_LAST_SIGNAL
};

struct _GscUserRequestTriggerPrivate {
	gulong signals[URS_LAST_SIGNAL];
	GtkSourceCompletion *completion;
	guint key;
	GdkModifierType mod;
};

#define GSC_USERREQUEST_TRIGGER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_GSC_USERREQUEST_TRIGGER, GscUserRequestTriggerPrivate))

enum  {
	GSC_USERREQUEST_TRIGGER_DUMMY_PROPERTY,
};

static const gchar* gsc_userrequest_trigger_real_get_name(GtkSourceCompletionTrigger* base);
static gboolean gsc_userrequest_trigger_real_activate (GtkSourceCompletionTrigger* base);
static gboolean gsc_userrequest_trigger_real_deactivate (GtkSourceCompletionTrigger* base);

static gpointer gsc_userrequest_trigger_parent_class = NULL;
static GtkSourceCompletionTriggerIface* gsc_userrequest_trigger_parent_iface = NULL;


static gboolean
user_request_view_key_press_event_cb(GtkWidget *view,
					GdkEventKey *event, 
					gpointer user_data)
{
	GscUserRequestTrigger *self = GSC_USERREQUEST_TRIGGER(user_data);
	GtkSourceCompletion* completion = self->priv->completion;
		
	if (completion != NULL)
	{
		
		if ((event->state & self->priv->mod) && event->keyval == self->priv->key)
		{
			gtk_source_completion_trigger_event(completion,GSC_USERREQUEST_TRIGGER_NAME,NULL);
			return TRUE;
		}
		
	}
	
	return FALSE;
	
} 

static const gchar* gsc_userrequest_trigger_real_get_name(GtkSourceCompletionTrigger *self)
{
	return GSC_USERREQUEST_TRIGGER_NAME;
}

static gboolean
gsc_userrequest_trigger_real_activate (GtkSourceCompletionTrigger* base)
{
	g_debug("Activating UR trigger");
	GscUserRequestTrigger *self = GSC_USERREQUEST_TRIGGER(base);
	GtkSourceCompletion* comp = self->priv->completion;
	GtkTextView *view = gtk_source_completion_get_view(comp);

	g_assert(GTK_IS_TEXT_VIEW(view));
	self->priv->signals[URS_GTK_TEXT_VIEW_KP] =  
			g_signal_connect_data(view,
						"key-press-event",
						G_CALLBACK(user_request_view_key_press_event_cb),
						(gpointer) self,
						(GClosureNotify)NULL,
						0);
	return TRUE;
}

static gboolean
gsc_userrequest_trigger_real_deactivate (GtkSourceCompletionTrigger* base)
{
	g_debug("Deactivating UR trigger");
	GscUserRequestTrigger *self = GSC_USERREQUEST_TRIGGER(base);
	GtkTextView *view = gtk_source_completion_get_view(self->priv->completion);
	g_signal_handler_disconnect(view,self->priv->signals[URS_GTK_TEXT_VIEW_KP]);
	return TRUE;
}

static void gsc_userrequest_trigger_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec)
{
}


static void gsc_userrequest_trigger_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec)
{
}

static void gsc_userrequest_trigger_init (GscUserRequestTrigger * self)
{
	self->priv = g_new0(GscUserRequestTriggerPrivate, 1);
	gtk_accelerator_parse("<Control>Return",&self->priv->key,&self->priv->mod);
	g_debug("Init UR trigger");
}

static void gsc_userrequest_trigger_finalize(GObject *object)
{
	g_debug("Finish UR trigger");
	GscUserRequestTrigger *self;
	self = GSC_USERREQUEST_TRIGGER(object);
	G_OBJECT_CLASS(gsc_userrequest_trigger_parent_class)->finalize(object);
}

static void gsc_userrequest_trigger_class_init (GscUserRequestTriggerClass * klass)
{
	gsc_userrequest_trigger_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->get_property = gsc_userrequest_trigger_get_property;
	G_OBJECT_CLASS (klass)->set_property = gsc_userrequest_trigger_set_property;
	G_OBJECT_CLASS (klass)->finalize = gsc_userrequest_trigger_finalize;
}

static void gsc_userrequest_trigger_interface_init (GtkSourceCompletionTriggerIface * iface)
{
	gsc_userrequest_trigger_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_name = gsc_userrequest_trigger_real_get_name;
	iface->activate = gsc_userrequest_trigger_real_activate;
	iface->deactivate = gsc_userrequest_trigger_real_deactivate;
}

GType gsc_userrequest_trigger_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (GscUserRequestTriggerClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gsc_userrequest_trigger_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GscUserRequestTrigger), 0, (GInstanceInitFunc) gsc_userrequest_trigger_init };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "GscUserRequestTrigger", &g_define_type_info, 0);
		static const GInterfaceInfo gsc_userrequest_trigger_info = { (GInterfaceInitFunc) gsc_userrequest_trigger_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		g_type_add_interface_static (g_define_type_id, GTK_SOURCE_COMPLETION_TYPE_TRIGGER, &gsc_userrequest_trigger_info);
	}
	return g_define_type_id;
}

/**
 * gsc_userrequest_trigger_new:
 *
 * Returns The new #GscUserRequestTrigger
 *
 */
GscUserRequestTrigger*
gsc_userrequest_trigger_new(GtkSourceCompletion *completion)
{
	GscUserRequestTrigger *self = GSC_USERREQUEST_TRIGGER (g_object_new (TYPE_GSC_USERREQUEST_TRIGGER, NULL));
	self->priv->completion = completion;
	return self;
}

/**
 * gsc_userrequest_trigger_set_keys:
 * @self: The #GscUserRequestTrigger 
 * @keys: The string representation of the keys that we will
 * use to activate the user request event. You can get this 
 * string with #gtk_accelerator_name
 *
 * Assign the keys that we will use to activate the user request event
 */
void
gsc_userrequest_trigger_set_keys(GscUserRequestTrigger * self, const gchar* keys)
{
	gtk_accelerator_parse(keys,&self->priv->key,&self->priv->mod);
	g_debug("user request keys: %s",keys);
}


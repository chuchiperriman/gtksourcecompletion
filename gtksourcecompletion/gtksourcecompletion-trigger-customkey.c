 /* gtksourcecompletion-trigger-customkey.c - Type here a short description of your trigger
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
#include "gtksourcecompletion-utils.h"
#include "gtksourcecompletion-trigger-customkey.h"

/* User request signals */
enum
{
	CKP_GTK_TEXT_VIEW_KP,
	CKP_LAST_SIGNAL
};

struct _GtkSourceCompletionTriggerCustomkeyPrivate {
	GtkSourceCompletion *completion;
	gulong signals[CKP_LAST_SIGNAL];
	gchar *trigger_name;
	guint key;
	GdkModifierType mod;
};

#define GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_TYPE_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY, GtkSourceCompletionTriggerCustomkeyPrivate))

static const gchar* 
gtk_source_completion_trigger_customkey_real_get_name(GtkSourceCompletionTrigger* base);
static gboolean 
gtk_source_completion_trigger_customkey_real_activate (GtkSourceCompletionTrigger* base);
static gboolean 
gtk_source_completion_trigger_customkey_real_deactivate (GtkSourceCompletionTrigger* base);

static gpointer gtk_source_completion_trigger_customkey_parent_class = NULL;
static GtkSourceCompletionTriggerIface* gtk_source_completion_trigger_customkey_parent_iface = NULL;

static gboolean
_view_key_press_event_cb(GtkWidget *view,
			 GdkEventKey *event, 
			 gpointer user_data)
{
	GtkSourceCompletionTriggerCustomkey *self = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY(user_data);
	GtkSourceCompletion* completion = self->priv->completion;
		
	if (completion != NULL)
	{
		
		if ((event->state & self->priv->mod) && event->keyval == self->priv->key)
		{
			gtk_source_completion_trigger_event(completion,self->priv->trigger_name,NULL);
			return TRUE;
		}
		
	}
	return FALSE;
} 


static const gchar* 
gtk_source_completion_trigger_customkey_real_get_name(GtkSourceCompletionTrigger *base)
{
	GtkSourceCompletionTriggerCustomkey *self = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY(base);
	return self->priv->trigger_name;
}

static gboolean
gtk_source_completion_trigger_customkey_real_activate (GtkSourceCompletionTrigger* base)
{
	GtkSourceCompletionTriggerCustomkey *self = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY(base);
	g_debug("Activating Customkey trigger: %s",self->priv->trigger_name);
	GtkSourceCompletion* comp = self->priv->completion;
	GtkTextView *view = gtk_source_completion_get_view(comp);

	g_assert(GTK_IS_TEXT_VIEW(view));
	self->priv->signals[CKP_GTK_TEXT_VIEW_KP] =  
			g_signal_connect_data(view,
					      "key-press-event",
					      G_CALLBACK(_view_key_press_event_cb),
					      (gpointer) self,
					      (GClosureNotify)NULL,
					      0);
	return TRUE;
}

static gboolean
gtk_source_completion_trigger_customkey_real_deactivate (GtkSourceCompletionTrigger* base)
{
	g_debug("Deactivating Customkey trigger");
	GtkSourceCompletionTriggerCustomkey *self = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY(base);
	GtkTextView *view = gtk_source_completion_get_view(self->priv->completion);
	g_signal_handler_disconnect(view,self->priv->signals[CKP_GTK_TEXT_VIEW_KP]);
	return FALSE;
}

static void 
gtk_source_completion_trigger_customkey_get_property (GObject * object, 
						guint property_id, 
						GValue * value, 
						GParamSpec * pspec)
{
}


static void 
gtk_source_completion_trigger_customkey_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec)
{
}

static void 
gtk_source_completion_trigger_customkey_init (GtkSourceCompletionTriggerCustomkey * self)
{
	self->priv = g_new0(GtkSourceCompletionTriggerCustomkeyPrivate, 1);
	self->priv->trigger_name = NULL;
	g_debug("Init Customkey trigger");
}

static void 
gtk_source_completion_trigger_customkey_finalize(GObject *object)
{
	g_debug("Finish Customkey trigger");
	GtkSourceCompletionTriggerCustomkey *self;
	self = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY(object);
	g_free(self->priv->trigger_name);
	G_OBJECT_CLASS(gtk_source_completion_trigger_customkey_parent_class)->finalize(object);
}

static void 
gtk_source_completion_trigger_customkey_class_init (GtkSourceCompletionTriggerCustomkeyClass * klass)
{
	gtk_source_completion_trigger_customkey_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->get_property = gtk_source_completion_trigger_customkey_get_property;
	G_OBJECT_CLASS (klass)->set_property = gtk_source_completion_trigger_customkey_set_property;
	G_OBJECT_CLASS (klass)->finalize = gtk_source_completion_trigger_customkey_finalize;
}

static void 
gtk_source_completion_trigger_customkey_interface_init (GtkSourceCompletionTriggerIface * iface)
{
	gtk_source_completion_trigger_customkey_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_name = gtk_source_completion_trigger_customkey_real_get_name;
	iface->activate = gtk_source_completion_trigger_customkey_real_activate;
	iface->deactivate = gtk_source_completion_trigger_customkey_real_deactivate;
}

GType 
gtk_source_completion_trigger_customkey_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = {sizeof (GtkSourceCompletionTriggerCustomkeyClass),
							     (GBaseInitFunc) NULL,
							     (GBaseFinalizeFunc) NULL, 
							     (GClassInitFunc) gtk_source_completion_trigger_customkey_class_init, 
							     (GClassFinalizeFunc) NULL, 
							     NULL, 
							     sizeof (GtkSourceCompletionTriggerCustomkey), 
							     0, 
							     (GInstanceInitFunc) gtk_source_completion_trigger_customkey_init 
							     };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "GtkSourceCompletionTriggerCustomkey", 
							   &g_define_type_info, 
							   0);
		static const GInterfaceInfo gtk_source_completion_trigger_customkey_info = {(GInterfaceInitFunc) gtk_source_completion_trigger_customkey_interface_init, 
									  (GInterfaceFinalizeFunc) NULL,
									  NULL};
		g_type_add_interface_static (g_define_type_id,
					     GTK_TYPE_SOURCE_COMPLETION_TRIGGER, 
					     &gtk_source_completion_trigger_customkey_info);
	}
	return g_define_type_id;
}

void
gtk_source_completion_trigger_customkey_set_keys(GtkSourceCompletionTriggerCustomkey * self, const gchar* keys)
{
	gtk_accelerator_parse(keys,&self->priv->key,&self->priv->mod);
}


GtkSourceCompletionTriggerCustomkey* 
gtk_source_completion_trigger_customkey_new(GtkSourceCompletion *completion,
			  const gchar* trigger_name, 
			  const gchar* keys)
{
	GtkSourceCompletionTriggerCustomkey *self = GTK_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY (g_object_new (GTK_TYPE_SOURCE_COMPLETION_TRIGGER_CUSTOMKEY, NULL));
	self->priv->completion = completion;
	self->priv->trigger_name = g_strdup(trigger_name);
	gtk_source_completion_trigger_customkey_set_keys(self,keys);
	return self;
}


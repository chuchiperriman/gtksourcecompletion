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

/* User request signals */
enum
{
	CKP_GTK_TEXT_VIEW_KP,
	CKP_LAST_SIGNAL
};

struct _GscTriggerCustomkeyPrivate {
	GscManager *completion;
	gulong signals[CKP_LAST_SIGNAL];
	gchar *trigger_name;
	guint key;
	GdkModifierType mod;
	GscManagerEventOptions *options;
};

#define GSC_TRIGGER_CUSTOMKEY_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GSC_TYPE_TRIGGER_CUSTOMKEY, GscTriggerCustomkeyPrivate))

static const gchar* 
gsc_trigger_customkey_real_get_name(GscTrigger* base);
static gboolean 
gsc_trigger_customkey_real_activate (GscTrigger* base);
static gboolean 
gsc_trigger_customkey_real_deactivate (GscTrigger* base);

static gpointer gsc_trigger_customkey_parent_class = NULL;
static GscTriggerIface* gsc_trigger_customkey_parent_iface = NULL;

static gboolean
_view_key_press_event_cb(GtkWidget *view,
			 GdkEventKey *event, 
			 gpointer user_data)
{
	GscTriggerCustomkey *self = GSC_TRIGGER_CUSTOMKEY(user_data);
	GscManager* completion = self->priv->completion;
	
	if (completion != NULL)
	{       
		/*This is for a gtk bug!!!!*/
		guint key = self->priv->key;
		if ((event->state & GDK_SHIFT_MASK) == GDK_SHIFT_MASK)
			key = gdk_keyval_to_upper(key);
		
		if ((event->state & self->priv->mod) ==  self->priv->mod &&
			event->keyval == key)
		{
			if (self->priv->options!=NULL)
				gsc_manager_trigger_event_with_opts(completion,
								    self->priv->trigger_name,
								    self->priv->options,
								    NULL);
			else
				gsc_manager_trigger_event(completion,self->priv->trigger_name,NULL);
			return TRUE;
		}
		
	}
	return FALSE;
} 


static const gchar* 
gsc_trigger_customkey_real_get_name(GscTrigger *base)
{
	GscTriggerCustomkey *self = GSC_TRIGGER_CUSTOMKEY(base);
	return self->priv->trigger_name;
}

static gboolean
gsc_trigger_customkey_real_activate (GscTrigger* base)
{
	GscTriggerCustomkey *self = GSC_TRIGGER_CUSTOMKEY(base);
	g_debug("Activating Customkey trigger: %s",self->priv->trigger_name);
	GscManager* comp = self->priv->completion;
	GtkTextView *view = gsc_manager_get_view(comp);

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
gsc_trigger_customkey_real_deactivate (GscTrigger* base)
{
	g_debug("Deactivating Customkey trigger");
	GscTriggerCustomkey *self = GSC_TRIGGER_CUSTOMKEY(base);
	GtkTextView *view = gsc_manager_get_view(self->priv->completion);
	g_signal_handler_disconnect(view,self->priv->signals[CKP_GTK_TEXT_VIEW_KP]);
	return FALSE;
}

static void 
gsc_trigger_customkey_get_property (GObject * object, 
						guint property_id, 
						GValue * value, 
						GParamSpec * pspec)
{
}


static void 
gsc_trigger_customkey_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec)
{
}

static void 
gsc_trigger_customkey_init (GscTriggerCustomkey * self)
{
	self->priv = g_new0(GscTriggerCustomkeyPrivate, 1);
	self->priv->trigger_name = NULL;
	self->priv->options = NULL;
	g_debug("Init Customkey trigger");
}

static void 
gsc_trigger_customkey_finalize(GObject *object)
{
	g_debug("Finish Customkey trigger");
	GscTriggerCustomkey *self;
	self = GSC_TRIGGER_CUSTOMKEY(object);
	g_free(self->priv->trigger_name);
	if (self->priv->options != NULL)
		g_free(self->priv->options);

	G_OBJECT_CLASS(gsc_trigger_customkey_parent_class)->finalize(object);
}

static void 
gsc_trigger_customkey_class_init (GscTriggerCustomkeyClass * klass)
{
	gsc_trigger_customkey_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->get_property = gsc_trigger_customkey_get_property;
	G_OBJECT_CLASS (klass)->set_property = gsc_trigger_customkey_set_property;
	G_OBJECT_CLASS (klass)->finalize = gsc_trigger_customkey_finalize;
}

static void 
gsc_trigger_customkey_interface_init (GscTriggerIface * iface)
{
	gsc_trigger_customkey_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_name = gsc_trigger_customkey_real_get_name;
	iface->activate = gsc_trigger_customkey_real_activate;
	iface->deactivate = gsc_trigger_customkey_real_deactivate;
}

GType 
gsc_trigger_customkey_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = {sizeof (GscTriggerCustomkeyClass),
							     (GBaseInitFunc) NULL,
							     (GBaseFinalizeFunc) NULL, 
							     (GClassInitFunc) gsc_trigger_customkey_class_init, 
							     (GClassFinalizeFunc) NULL, 
							     NULL, 
							     sizeof (GscTriggerCustomkey), 
							     0, 
							     (GInstanceInitFunc) gsc_trigger_customkey_init 
							     };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "GscTriggerCustomkey", 
							   &g_define_type_info, 
							   0);
		static const GInterfaceInfo gsc_trigger_customkey_info = {(GInterfaceInitFunc) gsc_trigger_customkey_interface_init, 
									  (GInterfaceFinalizeFunc) NULL,
									  NULL};
		g_type_add_interface_static (g_define_type_id,
					     GSC_TYPE_TRIGGER, 
					     &gsc_trigger_customkey_info);
	}
	return g_define_type_id;
}

void
gsc_trigger_customkey_set_keys(GscTriggerCustomkey * self, const gchar* keys)
{
	gtk_accelerator_parse(keys,&self->priv->key,&self->priv->mod);
}


GscTriggerCustomkey* 
gsc_trigger_customkey_new(GscManager *completion,
			  const gchar* trigger_name, 
			  const gchar* keys)
{
	GscTriggerCustomkey *self = GSC_TRIGGER_CUSTOMKEY (g_object_new (GSC_TYPE_TRIGGER_CUSTOMKEY, NULL));
	self->priv->completion = completion;
	self->priv->trigger_name = g_strdup(trigger_name);
	gsc_trigger_customkey_set_keys(self,keys);
	return self;
}

void
gsc_trigger_customkey_set_opts(GscTriggerCustomkey *self,
				GscManagerEventOptions *options)
{
	/*We must clone the data?*/
	self->priv->options = options;
}



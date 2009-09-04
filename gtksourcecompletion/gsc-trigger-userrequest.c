 /* gsc-trigger-userrequest.c - Type here a short description of your trigger
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
 * SECTION:gsc-trigger-userrequest
 * @title: GscTriggerUserRequest
 * @short_description: User Request trigger
 *
 * This object trigger a completion event when the user press the configured
 * keys.
 * 
 */

#include <glib/gprintf.h>
#include <string.h>
#include <ctype.h>
#include "gsc-utils.h"
#include "gsc-trigger-userrequest.h"

#define GSC_TRIGGER_USERREQUEST_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
					      GSC_TYPE_TRIGGER_USERREQUEST, \
					      GscTriggerUserRequestPrivate))

struct _GscTriggerUserRequestPrivate
{
	GscCompletion *completion;

	gulong kp_handler;
	gulong ins_handler;
	gulong del_handler;	
	guint key;
	GdkModifierType mod;
	gint line;
	gint line_offset;
};

static void	 gsc_trigger_userrequest_iface_init	(GscTriggerIface *iface);

G_DEFINE_TYPE_WITH_CODE (GscTriggerUserRequest,
			 gsc_trigger_userrequest,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GSC_TYPE_TRIGGER,
				 		gsc_trigger_userrequest_iface_init))

static gboolean
filter_func (GscProposal *proposal,
	     gpointer user_data)
{
	const gchar *label = gsc_proposal_get_label (proposal);
	const gchar *text = (const gchar*)user_data;
	return g_str_has_prefix (label, text);
}

static gboolean
view_key_press_event_cb (GtkWidget *view,
			 GdkEventKey *event, 
			 gpointer user_data)
{
	GscTriggerUserRequest *self = GSC_TRIGGER_USERREQUEST (user_data);
	guint s = event->state & gtk_accelerator_get_default_mod_mask();
	guint key = gdk_keyval_to_lower (self->priv->key);
	
	if (s == self->priv->mod && gdk_keyval_to_lower(event->keyval) == key)
	{
		GtkTextBuffer *buffer = gtk_text_view_get_buffer (GSC_TEXT_VIEW(view));
		GtkTextMark *insert = gtk_text_buffer_get_insert(buffer);
		GtkTextIter location;
		gtk_text_buffer_get_iter_at_mark(buffer,&location,insert);
		
		self->priv->line = gtk_text_iter_get_line (&location);
		self->priv->line_offset = gtk_text_iter_get_line_offset (&location);
		
		gsc_completion_trigger_event (self->priv->completion,
					      GSC_TRIGGER (self));
		return TRUE;
	}
	
	return FALSE;
}

static gboolean
delete_range_cb (GtkWidget *buffer,
		 GtkTextIter *start,
		 GtkTextIter *end,
		 gpointer user_data)
{
	
	GscTriggerUserRequest *self = GSC_TRIGGER_USERREQUEST (user_data);
	
	if (GSC_WIDGET_VISIBLE (self->priv->completion) &&
	    gsc_completion_get_active_trigger(self->priv->completion) == GSC_TRIGGER (self))
	{
		if (gtk_text_iter_get_line (start) != self->priv->line ||
		    gtk_text_iter_get_line_offset (start) < self->priv->line_offset)
		{
			gsc_completion_finish_completion (self->priv->completion);
		}
		else
		{
			GtkTextView *view = gsc_completion_get_view (self->priv->completion);
			/*Filter the current proposals */
			gchar *temp = gsc_get_last_word (GSC_TEXT_VIEW (view));
			gsc_completion_filter_proposals (self->priv->completion,
							 filter_func,
							 temp);
			g_free (temp);
		}
	}
	return FALSE;
}

static void
insert_text_cb (GtkTextBuffer *buffer,
		GtkTextIter* location,
		gchar *text,
		gint len,
		gpointer user_data)
{
	GscTriggerUserRequest *self = GSC_TRIGGER_USERREQUEST (user_data);
	
	/*Raise the event if completion is not visible*/
	if (GSC_WIDGET_VISIBLE (self->priv->completion))
	{
		/*If completion is visible, filter the content if the trigger si autowords*/
		if (gsc_completion_get_active_trigger(self->priv->completion) == GSC_TRIGGER (self))
		{
			if (gsc_char_is_separator (g_utf8_get_char (text)) ||
			    gtk_text_iter_get_line (location) != self->priv->line ||
			    gtk_text_iter_get_line_offset (location) < self->priv->line_offset)
			{
				gsc_completion_finish_completion (self->priv->completion);
			}
			else
			{
				/*Filter the current proposals */
				GtkTextView *view = gsc_completion_get_view (self->priv->completion);
				gchar *temp = gsc_get_last_word (view);
				gsc_completion_filter_proposals (self->priv->completion,
								 filter_func,
								 temp);
				g_free (temp);
			}
		}
	}
}

static const gchar* 
gsc_trigger_userrequest_real_get_name (GscTrigger *base)
{
	return GSC_TRIGGER_USERREQUEST_NAME;
}

static gboolean
gsc_trigger_userrequest_real_activate (GscTrigger* base)
{
	GscTriggerUserRequest *self = GSC_TRIGGER_USERREQUEST (base);
	GtkTextView *view;
	GtkTextBuffer *buffer;
	view = gsc_completion_get_view (self->priv->completion);
	buffer = gtk_text_view_get_buffer (view);
	
	g_assert (GSC_IS_TEXT_VIEW (view));
	
	self->priv->kp_handler = g_signal_connect (view,
					       "key-press-event",
					       G_CALLBACK (view_key_press_event_cb),
					       self);
	
	self->priv->del_handler = g_signal_connect_after (buffer,
							  "delete-range",
							  G_CALLBACK (delete_range_cb),
							  self);

	self->priv->ins_handler = g_signal_connect_after (buffer,
							  "insert-text",
							  G_CALLBACK (insert_text_cb),
							  self);
	
	return TRUE;
}

static gboolean
gsc_trigger_userrequest_real_deactivate (GscTrigger* base)
{
	GscTriggerUserRequest *self = GSC_TRIGGER_USERREQUEST (base);
	GtkTextView *view;
	GtkTextBuffer *buffer;
	
	view = gsc_completion_get_view (self->priv->completion);
	buffer = gtk_text_view_get_buffer (view);
	
	g_signal_handler_disconnect (view,
				     self->priv->kp_handler);
	g_signal_handler_disconnect (buffer,
				     self->priv->ins_handler);
	g_signal_handler_disconnect (buffer,
				     self->priv->del_handler);
	self->priv->kp_handler = 0;
	self->priv->ins_handler = 0;
	self->priv->del_handler = 0;
	return FALSE;
}

static void 
gsc_trigger_userrequest_init (GscTriggerUserRequest * self)
{
	self->priv = GSC_TRIGGER_USERREQUEST_GET_PRIVATE (self);
	
	self->priv->line = 0;
	self->priv->line_offset = 0;
	self->priv->kp_handler = 0;
	self->priv->ins_handler = 0;
	self->priv->del_handler = 0;
	gtk_accelerator_parse ("<Control>Return", &self->priv->key, &self->priv->mod);
}

static void 
gsc_trigger_userrequest_finalize(GObject *object)
{
	GscTriggerUserRequest *self = GSC_TRIGGER_USERREQUEST (object);
	
	g_object_unref (self->priv->completion);

	G_OBJECT_CLASS (gsc_trigger_userrequest_parent_class)->finalize (object);
}

static void 
gsc_trigger_userrequest_class_init (GscTriggerUserRequestClass * klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	g_type_class_add_private (klass, sizeof (GscTriggerUserRequestPrivate));

	object_class->finalize = gsc_trigger_userrequest_finalize;
}

static void 
gsc_trigger_userrequest_iface_init (GscTriggerIface * iface)
{
	iface->get_name   = gsc_trigger_userrequest_real_get_name;
	iface->activate   = gsc_trigger_userrequest_real_activate;
	iface->deactivate = gsc_trigger_userrequest_real_deactivate;
}

/**
 * gsc_trigger_userrequest_new:
 * @completion: The #GscCompletion
 *
 * This is a generic trigger. You tell the name and the key and the trigger
 * will be triggered when the user press this key (or keys).
 *
 * Returns: a new #GscTriggerUserRequest
 *
 */
GscTriggerUserRequest* 
gsc_trigger_userrequest_new (GscCompletion *completion)
{

	g_return_val_if_fail (GSC_IS_COMPLETION (completion), NULL);

	GscTriggerUserRequest *self;
	
	self = GSC_TRIGGER_USERREQUEST (g_object_new (GSC_TYPE_TRIGGER_USERREQUEST,
				      NULL));
	
	self->priv->completion = g_object_ref (completion);
	
	return self;
}

/**
 * gsc_trigger_userrequest_set_keys:
 * @self: The #GscTriggerUserRequest 
 * @keys: The string representation of the keys that we will
 * use to activate the user request event. You can get this 
 * string with #gtk_accelerator_name
 *
 * Assign the keys that we will use to activate the user request event.
 */
void
gsc_trigger_userrequest_set_keys (GscTriggerUserRequest *self,
				  const gchar* keys)
{
	g_return_if_fail (GSC_IS_TRIGGER_USERREQUEST (self));
	
	gtk_accelerator_parse (keys, &self->priv->key, &self->priv->mod);
}



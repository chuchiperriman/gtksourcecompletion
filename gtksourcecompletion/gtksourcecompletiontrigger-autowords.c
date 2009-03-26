/*
 * gtksourcecompletiontrigger-autowords.c
 * This file is part of gtksourcecompletion
 *
 * Copyright (C) 2007 -2009 Jesús Barbero Rodríguez <chuchiperriman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:gsc-trigger-autowords
 * @title: GtkSourceCompletionTriggerAutowords
 * @short_description: Autocompletion words trigger
 *
 * This object trigger a completion event when the user writes a words with 
 * a configured lenght (default 3 characters). You can configure a delay and
 * the completion event will be triggered n millisecons after the user 
 * insertion.
 * 
 */

#include <glib/gprintf.h>
#include <string.h>
#include <ctype.h>
#include <gtksourceview/gtksourceview.h>
#include <gdk/gdkkeysyms.h>
#include "gtksourcecompletionutils.h"
#include "gtksourcecompletiontrigger-autowords.h"
#include "gtksourcecompletioni18n.h"

#define DEFAULT_MIN_LEN (3)
#define DEFAULT_DELAY (200)

#define GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
								GTK_TYPE_SOURCE_COMPLETION_TRIGGER_AUTOWORDS, \
								GtkSourceCompletionTriggerAutowordsPrivate))

/* Autocompletion signals */
enum
{
	AS_GTK_TEXT_VIEW_KR,
	AS_GTK_TEXT_BUFFER_IT,
	LAST_SIGNAL
};
	
struct _GtkSourceCompletionTriggerAutowordsPrivate
{
	GtkSourceCompletion* completion;
	GtkTextView *view;
	
	gulong signals[LAST_SIGNAL];
	
	gchar *actual_word;
	
	guint source_id;
	guint delay;
	guint min_len;
	gint text_offset;
};

enum
{
	PROP_0,
	PROP_MIN_LEN,
	PROP_DELAY
};

static void	 gtk_source_completion_trigger_autowords_iface_init	(GtkSourceCompletionTriggerIface *iface);

G_DEFINE_TYPE_WITH_CODE (GtkSourceCompletionTriggerAutowords,
			 gtk_source_completion_trigger_autowords,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GTK_TYPE_SOURCE_COMPLETION_TRIGGER,
				 		gtk_source_completion_trigger_autowords_iface_init))

static gint
get_text_offset (GtkSourceCompletionTriggerAutowords *self)
{
	GtkTextBuffer *buffer;
	GtkTextMark* mark;
	GtkTextIter iter;
	
	buffer = gtk_text_view_get_buffer (self->priv->view);
	mark = gtk_text_buffer_get_insert (buffer);
	gtk_text_buffer_get_iter_at_mark (buffer, &iter, mark);
	
	return gtk_text_iter_get_offset (&iter);
}

static gboolean
autocompletion_raise_event (gpointer event)
{
	GtkSourceCompletionTriggerAutowords *self;
	gchar* word;
	gint offset;
	
	self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (event);
	
	/*Check if the user has changed the cursor position.If yes, we don't complete*/
	offset = get_text_offset (self);
	
	if (offset != self->priv->text_offset)
		return FALSE;
	
	word = gtk_source_completion_utils_get_word (self->priv->view);
	
	if (strlen (word) >= self->priv->min_len)
	{
		g_free (self->priv->actual_word);
		self->priv->actual_word = word;
		
		gtk_source_completion_trigger_event (self->priv->completion,
						     GTK_SOURCE_COMPLETION_TRIGGER (self));
	}
	else
	{
		GtkSourceCompletionTrigger *active_trigger;
		
		active_trigger = gtk_source_completion_get_active_trigger (self->priv->completion);
		if (active_trigger && strcmp (gtk_source_completion_trigger_get_name (active_trigger),
					      GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS_NAME) == 0)
		{
			gtk_source_completion_finish_completion (self->priv->completion);
		}
	}
	return FALSE;
}

static gboolean
autocompletion_key_release_cb (GtkWidget *view,
			       GdkEventKey *event, 
			       gpointer user_data)
{
	GtkSourceCompletionTriggerAutowords *self;
	guint keyval = event->keyval;
	
	self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (user_data);
	
	if (GDK_BackSpace == keyval)
	{
		/* Only update the completion if the popup is visible */
		if (GTK_WIDGET_VISIBLE (self->priv->completion))
		{
			if (self->priv->source_id != 0)
			{
				/* Stop the event because the user is written very fast*/
				g_source_remove (self->priv->source_id);
				self->priv->source_id = 0;
			}
	
			self->priv->text_offset = get_text_offset (self);
			/*raise event in 0,5 seconds*/
			self->priv->source_id = g_timeout_add (self->priv->delay,
							       autocompletion_raise_event,
							       self);
		}
	}

	return FALSE;
}

static void
autocompletion_insert_text_cb (GtkTextBuffer *buffer,
			       GtkTextIter* location,
			       gchar *text,
			       gint len,
			       gpointer user_data)
{
	GtkSourceCompletionTriggerAutowords *self;
	
	self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (user_data);
	
	/* Prevent "paste" */
	if (len <= 2)
	{
		if (self->priv->source_id != 0)
		{
			/* Stop the event because the user is written very fast*/
			g_source_remove (self->priv->source_id);
			self->priv->source_id = 0;
		}

		self->priv->text_offset = get_text_offset (self);
		/*raise event in 0,5 seconds*/
		self->priv->source_id = g_timeout_add (self->priv->delay,
						       autocompletion_raise_event,
						       self);
	}
}

static const gchar * 
gtk_source_completion_trigger_autowords_real_get_name (GtkSourceCompletionTrigger *self)
{
	return GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS_NAME;
}

static gboolean
gtk_source_completion_trigger_autowords_real_activate (GtkSourceCompletionTrigger* base)
{
	GtkSourceCompletionTriggerAutowords *self;
	
	self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (base);
	
	self->priv->signals[AS_GTK_TEXT_VIEW_KR] = 
		g_signal_connect_data (self->priv->view,
				       "key-release-event",
				       G_CALLBACK (autocompletion_key_release_cb),
				       self,
				       NULL,
				       G_CONNECT_AFTER);

	self->priv->signals[AS_GTK_TEXT_BUFFER_IT] = 
		g_signal_connect_after (gtk_text_view_get_buffer (self->priv->view),
					"insert-text",
					G_CALLBACK (autocompletion_insert_text_cb),
					self);
	
	return TRUE;
}

static gboolean
gtk_source_completion_trigger_autowords_real_deactivate (GtkSourceCompletionTrigger* base)
{
	GtkSourceCompletionTriggerAutowords *self;
	GtkTextBuffer *buffer;
	
	self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (base);
	
	if (g_signal_handler_is_connected (self->priv->view,
					   self->priv->signals[AS_GTK_TEXT_VIEW_KR]))
	{
		g_signal_handler_disconnect (self->priv->view,
					     self->priv->signals[AS_GTK_TEXT_VIEW_KR]);
		self->priv->signals[AS_GTK_TEXT_VIEW_KR] = 0;
	}

	buffer = gtk_text_view_get_buffer (self->priv->view);	
	if (g_signal_handler_is_connected (buffer,
					   self->priv->signals[AS_GTK_TEXT_BUFFER_IT]))
	{
		g_signal_handler_disconnect (buffer,
					     self->priv->signals[AS_GTK_TEXT_BUFFER_IT]);
		self->priv->signals[AS_GTK_TEXT_BUFFER_IT] = 0;
	}
	
	return FALSE;
}

static void 
gtk_source_completion_trigger_autowords_get_property (GObject *object,
						      guint property_id,
						      GValue *value,
						      GParamSpec *pspec)
{
	GtkSourceCompletionTriggerAutowords *self;
	
	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (object));

	self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (object);

	switch (property_id)
	{
		case PROP_DELAY:
			g_value_set_int (value,
					 self->priv->delay);
			break;
		case PROP_MIN_LEN:
			g_value_set_int (value,
					 self->priv->min_len);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}


static void 
gtk_source_completion_trigger_autowords_set_property (GObject *object,
						      guint property_id,
						      const GValue *value,
						      GParamSpec *pspec)
{
	GtkSourceCompletionTriggerAutowords *self;
	
	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (object));

	self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (object);

	switch (property_id)
	{
		case PROP_DELAY:
			self->priv->delay = g_value_get_int (value);
			break;
		case PROP_MIN_LEN:
			self->priv->min_len = g_value_get_int (value);
			break;
		default:
			G_OBJECT_WARN_INVALID_PROPERTY_ID (object, property_id, pspec);
			break;
	}
}

static void 
gtk_source_completion_trigger_autowords_init (GtkSourceCompletionTriggerAutowords *self)
{
	self->priv = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS_GET_PRIVATE (self);
	
	self->priv->actual_word = NULL;
	self->priv->source_id = 0;
	self->priv->delay = DEFAULT_DELAY;
	self->priv->min_len = DEFAULT_MIN_LEN;
}

static void 
gtk_source_completion_trigger_autowords_finalize(GObject *object)
{
	GtkSourceCompletionTriggerAutowords *self;
	
	self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (object);

	if (self->priv->source_id != 0)
	{
		g_source_remove (self->priv->source_id);
		self->priv->source_id = 0;
	}

	g_free (self->priv->actual_word);

	G_OBJECT_CLASS (gtk_source_completion_trigger_autowords_parent_class)->finalize (object);
}

static void 
gtk_source_completion_trigger_autowords_class_init (GtkSourceCompletionTriggerAutowordsClass * klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GtkSourceCompletionTriggerAutowordsPrivate));

	object_class->get_property = gtk_source_completion_trigger_autowords_get_property;
	object_class->set_property = gtk_source_completion_trigger_autowords_set_property;
	object_class->finalize     = gtk_source_completion_trigger_autowords_finalize;
	
	/**
	 * GscCompletion:delay:
	 *
	 * Sets the delay between the key pressed and the trigger event
	 */
	g_object_class_install_property (object_class,
					 PROP_DELAY,
					 g_param_spec_int ("delay",
							   _("Delay between the key pressed and the trigger event"),
							   _("Delay between the key pressed and the trigger event"),
							   0,
							   10000,
							   DEFAULT_DELAY,
							   G_PARAM_READWRITE));
	/**
	 * GscCompletion:min-len:
	 *
	 * Sets the minimum word length to be autocompleted
	 */
	g_object_class_install_property (object_class,
					 PROP_MIN_LEN,
					 g_param_spec_int ("min-len",
							   _("Minimum word length to be autocompleted"),
							   _("Minimum word length to be autocompleted"),
							   0,
							   100,
							   DEFAULT_MIN_LEN,
							   G_PARAM_READWRITE));
}

static void 
gtk_source_completion_trigger_autowords_iface_init (GtkSourceCompletionTriggerIface * iface)
{
	iface->get_name   = gtk_source_completion_trigger_autowords_real_get_name;
	iface->activate   = gtk_source_completion_trigger_autowords_real_activate;
	iface->deactivate = gtk_source_completion_trigger_autowords_real_deactivate;
}

/**
 * gtk_source_completion_trigger_autowords_new:
 * @completion: The #GscCompletion where the triggered will be used
 *
 * Returns: A new #GtkSourceCompletionTriggerAutowords
 */
GtkSourceCompletionTriggerAutowords*
gtk_source_completion_trigger_autowords_new (GtkSourceCompletion *completion)
{
	GtkSourceCompletionTriggerAutowords *self;
	
	g_return_val_if_fail (GTK_IS_SOURCE_COMPLETION (completion), NULL);
	
	self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (g_object_new (GTK_TYPE_SOURCE_COMPLETION_TRIGGER_AUTOWORDS,
								      NULL));
	
	self->priv->completion = completion;
	self->priv->view = gtk_source_completion_get_view (completion);
	
	return self;
}

/**
 * gtk_source_completion_trigger_autowords_set_delay:
 * @self: The #GtkSourceCompletionTriggerAutowords
 * @delay: milliseconds to delay the autocompletion event.
 *
 * The delay time is the time between the last user key pressed
 * and the instant when the trigger call to the completion. If 
 * delay is 2000 then the user press a key and 2 seconds later
 * this trigger call to the completion if the user don't press
 * another key.
 *
 */
void
gtk_source_completion_trigger_autowords_set_delay (GtkSourceCompletionTriggerAutowords* self,
						   guint delay)
{
	g_return_if_fail (GTK_IS_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (self));

	self->priv->delay = delay;
}



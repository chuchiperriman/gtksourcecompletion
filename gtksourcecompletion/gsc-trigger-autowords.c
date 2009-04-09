/* Copyright (C) 2007 - Jesús Barbero <chuchiperriman@gmail.com>
 * 
 *  This file is part of gtksourcecompletion.
 *
 *  gtksourcecompletion is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  gtksourcecompletion is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

/**
 * SECTION:gsc-trigger-autowords
 * @title: GscTriggerAutowords
 * @short_description: Autocompletion words trigger
 *
 * This object trigger a completion event when the user writes a words with 
 * a configured lenght (default 3 characters). You can configure a delay and
 * the completion event will be triggered n millisecons after the user 
 * insertion.
 * 
 */

#include <string.h>
#include "gsc-utils.h"
#include "gsc-trigger-autowords.h"
#include "gsc-i18n.h"

#define DEFAULT_MIN_LEN (3)
#define DEFAULT_DELAY (200)

#define GSC_TRIGGER_AUTOWORDS_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
					      GSC_TYPE_TRIGGER_AUTOWORDS, \
					      GscTriggerAutowordsPrivate))

struct _GscTriggerAutowordsPrivate
{
	GscCompletion* completion;
	GtkTextView *view;
	gulong ins_handler;
	gulong del_handler;
	guint source_id;
	guint delay;
	guint min_len;
	
	gint line;
	gint line_offset;
};

enum
{
	PROP_0,
	PROP_MIN_LEN,
	PROP_DELAY
};

static void	 gsc_trigger_autowords_iface_init	(GscTriggerIface *iface);

G_DEFINE_TYPE_WITH_CODE (GscTriggerAutowords,
			 gsc_trigger_autowords,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GSC_TYPE_TRIGGER,
				 		gsc_trigger_autowords_iface_init))

static gboolean
autocompletion_filter_func (GscProposal *proposal,
			    gpointer user_data)
{
	const gchar *label = gsc_proposal_get_label (proposal);
	const gchar *text = (const gchar*)user_data;
	return g_str_has_prefix (label, text);
}

static gboolean
autocompletion_raise_event (gpointer event)
{
	GscTriggerAutowords *self = GSC_TRIGGER_AUTOWORDS (event);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (self->priv->view);
	GtkTextMark *insert_mark = gtk_text_buffer_get_insert (buffer);
	GtkTextIter iter;
	gchar* word;

	/*Check if the user has changed the cursor position.If yes, we don't complete*/	
	gtk_text_buffer_get_iter_at_mark (buffer, &iter, insert_mark);
	if ((gtk_text_iter_get_line (&iter) != self->priv->line) ||
	    (gtk_text_iter_get_line_offset (&iter) != self->priv->line_offset))
	{
		return FALSE;
	}
	    
	word = gsc_get_last_word_and_iter (self->priv->view,
					   NULL, NULL);
	if (strlen (word) >= self->priv->min_len)
	{
		gsc_completion_trigger_event (self->priv->completion,
					      GSC_TRIGGER (self));
	}
	g_free (word);
	
	return FALSE;
}

static gboolean
autocompletion_delete_range_cb (GtkWidget *view,
				GtkTextIter *start,
				GtkTextIter *end,
				gpointer user_data)
{
	
	GscTriggerAutowords *self = GSC_TRIGGER_AUTOWORDS (user_data);
	
	if (GTK_WIDGET_VISIBLE (self->priv->completion) &&
	    gsc_completion_get_active_trigger(self->priv->completion) == GSC_TRIGGER (self))
	{
		if (gtk_text_iter_get_line (start) != self->priv->line ||
		    gtk_text_iter_get_line_offset (start) < self->priv->line_offset)
		{
			gsc_completion_finish_completion (self->priv->completion);
		}
		else
		{
			/*Filter the current proposals */
			gchar *temp = gsc_get_last_word (self->priv->view);
			gsc_completion_filter_proposals (self->priv->completion,
							 autocompletion_filter_func,
							 temp);
			g_free (temp);
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
	GscTriggerAutowords *self = GSC_TRIGGER_AUTOWORDS (user_data);
	
	/*Raise the event if completion is not visible*/
	if (!GTK_WIDGET_VISIBLE (self->priv->completion))
	{
		/* Prevent "paste" */
		if (len <= 2)
		{
			if (self->priv->source_id != 0)
			{
				/* Stop the event because the user is written very fast*/
				g_source_remove (self->priv->source_id);
				self->priv->source_id = 0;
			}

			self->priv->line = gtk_text_iter_get_line (location);
			self->priv->line_offset = gtk_text_iter_get_line_offset (location);
			/*raise event in 0,5 seconds*/
			self->priv->source_id = g_timeout_add (self->priv->delay,
							       autocompletion_raise_event,
							       self);
		}
	}
	else
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
				gchar *temp = gsc_get_last_word (self->priv->view);
				gsc_completion_filter_proposals (self->priv->completion,
								 autocompletion_filter_func,
								 temp);
				g_free (temp);
			}
		}
	}
}

static const gchar* 
gsc_trigger_autowords_real_get_name (GscTrigger *self)
{
	return GSC_TRIGGER_AUTOWORDS_NAME;
}

static gboolean
gsc_trigger_autowords_real_activate (GscTrigger* base)
{
	GscTriggerAutowords *self = GSC_TRIGGER_AUTOWORDS (base);
	
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (self->priv->view);
	self->priv->del_handler = g_signal_connect_after (buffer,
							  "delete-range",
							  G_CALLBACK (autocompletion_delete_range_cb),
							  self);

	self->priv->ins_handler = g_signal_connect_after (buffer,
							  "insert-text",
							  G_CALLBACK (autocompletion_insert_text_cb),
							  self);
	return TRUE;
}

static gboolean
gsc_trigger_autowords_real_deactivate (GscTrigger* base)
{
	GscTriggerAutowords *self = GSC_TRIGGER_AUTOWORDS (base);
	GtkTextBuffer *buffer = gtk_text_view_get_buffer (self->priv->view);
	
	g_signal_handler_disconnect (buffer,
				     self->priv->ins_handler);
	self->priv->ins_handler = 0;
	g_signal_handler_disconnect (buffer,
				     self->priv->del_handler);
	self->priv->del_handler = 0;
	return FALSE;
}

static void 
gsc_trigger_autowords_get_property (GObject *object, 
				    guint property_id,
				    GValue *value,
				    GParamSpec *pspec)
{
	g_return_if_fail (GSC_IS_TRIGGER_AUTOWORDS (object));

	GscTriggerAutowords *self = GSC_TRIGGER_AUTOWORDS (object);

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
gsc_trigger_autowords_set_property (GObject *object,
				    guint property_id,
				    const GValue *value,
				    GParamSpec *pspec)
{
	g_return_if_fail (GSC_IS_TRIGGER_AUTOWORDS (object));

	GscTriggerAutowords *self = GSC_TRIGGER_AUTOWORDS (object);

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
gsc_trigger_autowords_init (GscTriggerAutowords *self)
{
	self->priv = GSC_TRIGGER_AUTOWORDS_GET_PRIVATE (self);
	
	self->priv->source_id = 0;
	self->priv->delay = DEFAULT_DELAY;
	self->priv->min_len = DEFAULT_MIN_LEN;
	self->priv->line = 0;
	self->priv->line_offset = 0;
}

static void 
gsc_trigger_autowords_finalize(GObject *object)
{
	GscTriggerAutowords *self = GSC_TRIGGER_AUTOWORDS (object);

	if (self->priv->source_id != 0)
	{
		g_source_remove (self->priv->source_id);
		self->priv->source_id = 0;
	}

	G_OBJECT_CLASS (gsc_trigger_autowords_parent_class)->finalize (object);
}

static void 
gsc_trigger_autowords_class_init (GscTriggerAutowordsClass * klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GscTriggerAutowordsPrivate));

	object_class->get_property = gsc_trigger_autowords_get_property;
	object_class->set_property = gsc_trigger_autowords_set_property;
	object_class->finalize     = gsc_trigger_autowords_finalize;
	
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
gsc_trigger_autowords_iface_init (GscTriggerIface * iface)
{
	iface->get_name   = gsc_trigger_autowords_real_get_name;
	iface->activate   = gsc_trigger_autowords_real_activate;
	iface->deactivate = gsc_trigger_autowords_real_deactivate;
}

/**
 * gsc_trigger_autowords_new:
 * @completion: The #GscCompletion where the triggered will be used
 *
 * Returns: A new #GscTriggerAutowords
 */
GscTriggerAutowords*
gsc_trigger_autowords_new (GscCompletion *completion)
{
	GscTriggerAutowords *self;
	
	g_return_val_if_fail (GSC_IS_COMPLETION (completion), NULL);
	
	self = GSC_TRIGGER_AUTOWORDS (g_object_new (GSC_TYPE_TRIGGER_AUTOWORDS, NULL));
	
	self->priv->completion = completion;
	self->priv->view = gsc_completion_get_view (completion);
	
	return self;
}

/**
 * gsc_trigger_autowords_set_delay:
 * @self: The #GscTriggerAutowords
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
gsc_trigger_autowords_set_delay (GscTriggerAutowords* self,
				 guint delay)
{
	g_return_if_fail (GSC_IS_TRIGGER_AUTOWORDS (self));

	self->priv->delay = delay;
}



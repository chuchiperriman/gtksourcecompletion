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
#include <glib/gprintf.h>
#include <string.h>
#include <ctype.h>
#include <gtksourceview/gtksourceview.h>
#include "gtksourcecompletion-utils.h"
#include "gtksourcecompletion-trigger-autowords.h"

#define MIN_LEN 3
#define AUTOCOMPLETION_DELAY 200


/* Autocompletion signals */
enum
{
	AS_GTK_TEXT_VIEW_KR,
	AS_GTK_TEXT_VIEW_IT,
	AS_LAST_SIGNAL
};

struct _GtkSourceCompletionTriggerAutowordsPrivate {
	gulong signals[AS_LAST_SIGNAL];
	gchar *actual_word;
	GtkSourceCompletion* completion;
	GtkTextView *view;
	guint source_id;
	guint delay;
	gint text_offset;
};

#define GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), GTK_SOURCE_COMPLETION_TYPE_TRIGGER_AUTOWORDS, GtkSourceCompletionTriggerAutowordsPrivate))

enum  {
	GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS_DUMMY_PROPERTY,
};

static const gchar* 
gtk_source_completion_trigger_autowords_real_get_name(GtkSourceCompletionTrigger* base);
static gboolean 
gtk_source_completion_trigger_autowords_real_activate (GtkSourceCompletionTrigger* base);
static gboolean 
gtk_source_completion_trigger_autowords_real_deactivate (GtkSourceCompletionTrigger* base);
static gboolean
autocompletion_raise_event(gpointer event);

static gpointer gtk_source_completion_trigger_autowords_parent_class = NULL;
static GtkSourceCompletionTriggerIface* gtk_source_completion_trigger_autowords_parent_iface = NULL;

static gint
_get_text_offset(GtkSourceCompletionTriggerAutowords *self)
{
	GtkTextBuffer *buffer = gtk_text_view_get_buffer(self->priv->view);
	GtkTextMark* mark = gtk_text_buffer_get_insert(buffer);
	GtkTextIter iter;
	gtk_text_buffer_get_iter_at_mark(buffer,&iter,mark);
	return gtk_text_iter_get_offset(&iter);
}

static gboolean
autocompletion_key_release_cb (GtkWidget *view,
			       GdkEventKey *event, 
			       gpointer user_data)
{
	guint keyval = event->keyval;
	GtkSourceCompletionTriggerAutowords *self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS(user_data);
	GtkSourceCompletion *completion = self->priv->completion;
	if (completion != NULL)
	{
		if (GDK_BackSpace == keyval)
		{
			/* We only actualize the completion if the popup is visible */
			if (gtk_source_completion_is_visible(self->priv->completion))
			{
				if (self->priv->source_id!=0)
				{
					/* Stop the event because the user is written very fast*/
					g_source_remove(self->priv->source_id);
					self->priv->source_id = 0;
				}
	
				self->priv->text_offset = _get_text_offset(self);
				/*raise event in 0,5 seconds*/
				self->priv->source_id = g_timeout_add(self->priv->delay,
								      autocompletion_raise_event,
								      self);	
			}
		}
	}

	return FALSE;
}

static void
autocompletion_insert_text_cb(GtkTextBuffer *buffer,
			      GtkTextIter* location,
			      gchar *text,
			      gint len,
			      gpointer user_data)
{
	GtkSourceCompletionTriggerAutowords *self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS(user_data);
	/*TODO see the maximun length of a single UTF-8 character*/
	if (len<=2)
	{
		if (self->priv->source_id!=0)
		{
			/* Stop the event because the user is written very fast*/
			g_source_remove(self->priv->source_id);
			self->priv->source_id = 0;
		}

		self->priv->text_offset = _get_text_offset(self);
		/*raise event in 0,5 seconds*/
		self->priv->source_id = g_timeout_add(self->priv->delay,
						      autocompletion_raise_event,
						      self);
	}
}

static gboolean
autocompletion_raise_event(gpointer event)
{
	gchar* word;
	GtkSourceCompletionTrigger *active_trigger = NULL;
	GtkSourceCompletionTriggerAutowords *self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS(event);
	/*Check if the user has changed the cursor position.If yes, we don't complete*/
	gint offset = _get_text_offset(self);
	if (offset != self->priv->text_offset)
		return FALSE;
		
	GtkSourceCompletion *completion = self->priv->completion;
	GtkSourceView *source_view = GTK_SOURCE_VIEW(self->priv->view);
	word = gtk_source_view_get_last_word_and_iter(GTK_TEXT_VIEW(source_view), NULL, NULL);
	if (strlen(word) >= MIN_LEN)
	{
		g_free(self->priv->actual_word);
		self->priv->actual_word = word;
		gtk_source_completion_trigger_event(completion,
							GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS_NAME,
							self);
	}
	else
	{
		active_trigger = gtk_source_completion_get_active_trigger(self->priv->completion);
		if (active_trigger!=NULL && strcmp(gtk_source_completion_trigger_get_name(active_trigger),GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS_NAME)==0)
		{
			gtk_source_completion_finish_completion(completion);
		}
	}
	return FALSE;
}

static const gchar* gtk_source_completion_trigger_autowords_real_get_name(GtkSourceCompletionTrigger *self)
{
	return GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS_NAME;
}

static gboolean
gtk_source_completion_trigger_autowords_real_activate (GtkSourceCompletionTrigger* base)
{
	g_debug("Activating Autocompletion trigger");
	GtkSourceCompletionTriggerAutowords *self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS(base);
	self->priv->signals[AS_GTK_TEXT_VIEW_KR] = g_signal_connect_data(self->priv->view,
						"key-release-event",
						G_CALLBACK(autocompletion_key_release_cb),
						(gpointer) self,
						(GClosureNotify)NULL,
						G_CONNECT_AFTER);

	self->priv->signals[AS_GTK_TEXT_VIEW_IT] = g_signal_connect_after(gtk_text_view_get_buffer(self->priv->view),
						"insert-text",
						G_CALLBACK(autocompletion_insert_text_cb),
						(gpointer) self);
	
	return TRUE;
}

static gboolean
gtk_source_completion_trigger_autowords_real_deactivate (GtkSourceCompletionTrigger* base)
{
	g_debug("Deactivating Autocompletion trigger");
	GtkSourceCompletionTriggerAutowords *self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS(base);
	gint i;
	
	for(i=0;i < AS_LAST_SIGNAL;i++)
	{
		if (g_signal_handler_is_connected(self->priv->view,self->priv->signals[i]))
		{
			g_signal_handler_disconnect(self->priv->view,self->priv->signals[i]);
		}
	}
	return FALSE;
}

static void 
gtk_source_completion_trigger_autowords_get_property (GObject * object, 
					 guint property_id, 
					 GValue * value, 
					 GParamSpec * pspec)
{
}


static void 
gtk_source_completion_trigger_autowords_set_property (GObject * object, 
					 guint property_id, 
					 const GValue * value, 
					 GParamSpec * pspec)
{
}

static void 
gtk_source_completion_trigger_autowords_init (GtkSourceCompletionTriggerAutowords * self)
{
	self->priv = g_new0(GtkSourceCompletionTriggerAutowordsPrivate, 1);
	g_debug("Init Autocompletion trigger");
}

static void 
gtk_source_completion_trigger_autowords_finalize(GObject *object)
{
	g_debug("Finish Autocompletion trigger");
	GtkSourceCompletionTriggerAutowords *self;
	self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS(object);
	g_free(self->priv->actual_word);
	G_OBJECT_CLASS(gtk_source_completion_trigger_autowords_parent_class)->finalize(object);
}

static void 
gtk_source_completion_trigger_autowords_class_init (GtkSourceCompletionTriggerAutowordsClass * klass)
{
	gtk_source_completion_trigger_autowords_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->get_property = gtk_source_completion_trigger_autowords_get_property;
	G_OBJECT_CLASS (klass)->set_property = gtk_source_completion_trigger_autowords_set_property;
	G_OBJECT_CLASS (klass)->finalize = gtk_source_completion_trigger_autowords_finalize;
}

static void 
gtk_source_completion_trigger_autowords_interface_init (GtkSourceCompletionTriggerIface * iface)
{
	gtk_source_completion_trigger_autowords_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_name = gtk_source_completion_trigger_autowords_real_get_name;
	iface->activate = gtk_source_completion_trigger_autowords_real_activate;
	iface->deactivate = gtk_source_completion_trigger_autowords_real_deactivate;
}

GType 
gtk_source_completion_trigger_autowords_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (GtkSourceCompletionTriggerAutowordsClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gtk_source_completion_trigger_autowords_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GtkSourceCompletionTriggerAutowords), 0, (GInstanceInitFunc) gtk_source_completion_trigger_autowords_init };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "GtkSourceCompletionTriggerAutowords", &g_define_type_info, 0);
		static const GInterfaceInfo gtk_source_completion_trigger_autowords_info = { (GInterfaceInitFunc) gtk_source_completion_trigger_autowords_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		g_type_add_interface_static (g_define_type_id, GTK_SOURCE_COMPLETION_TYPE_TRIGGER, &gtk_source_completion_trigger_autowords_info);
	}
	return g_define_type_id;
}

GtkSourceCompletionTriggerAutowords*
gtk_source_completion_trigger_autowords_new(GtkSourceCompletion *completion)
{
	GtkSourceCompletionTriggerAutowords *self = GTK_SOURCE_COMPLETION_TRIGGER_AUTOWORDS (g_object_new (GTK_SOURCE_COMPLETION_TYPE_TRIGGER_AUTOWORDS, NULL));
	self->priv->completion = completion;
	
	self->priv->actual_word = NULL;
	self->priv->delay = AUTOCOMPLETION_DELAY;
	self->priv->view = gtk_source_completion_get_view(completion);
	self->priv->source_id = 0;
	
	return self;
}

void
gtk_source_completion_trigger_autowords_set_delay(GtkSourceCompletionTriggerAutowords* trigger, 
				     guint delay)
{
	trigger->priv->delay = delay;
}


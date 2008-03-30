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
#include "gsc-autocompletion-trigger.h"

#define MIN_LEN 3
#define AUTOCOMPLETION_DELAY 200


/* Autocompletion signals */
enum
{
	AS_GTK_TEXT_VIEW_KR,
	AS_GTK_TEXT_VIEW_IT,
	AS_LAST_SIGNAL
};

struct _GscAutocompletionTriggerPrivate {
	gulong signals[AS_LAST_SIGNAL];
	gchar *actual_word;
	GtkSourceCompletion* completion;
	GtkTextView *view;
	guint source_id;
	guint delay;
	gint text_offset;
};

#define GSC_AUTOCOMPLETION_TRIGGER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_GSC_AUTOCOMPLETION_TRIGGER, GscAutocompletionTriggerPrivate))

enum  {
	GSC_AUTOCOMPLETION_TRIGGER_DUMMY_PROPERTY,
};

static const gchar* gsc_autocompletion_trigger_real_get_name(GtkSourceCompletionTrigger* base);
static gboolean gsc_autocompletion_trigger_real_activate (GtkSourceCompletionTrigger* base);
static gboolean gsc_autocompletion_trigger_real_deactivate (GtkSourceCompletionTrigger* base);

static gpointer gsc_autocompletion_trigger_parent_class = NULL;
static GtkSourceCompletionTriggerIface* gsc_autocompletion_trigger_parent_iface = NULL;

static gboolean
autocompletion_raise_event(
								gpointer event);

static gint
_get_text_offset(GscAutocompletionTrigger *self)
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
	GscAutocompletionTrigger *self = GSC_AUTOCOMPLETION_TRIGGER(user_data);
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
				self->priv->source_id = g_timeout_add(self->priv->delay,autocompletion_raise_event,self);	
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
	GscAutocompletionTrigger *self = GSC_AUTOCOMPLETION_TRIGGER(user_data);
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
		self->priv->source_id = g_timeout_add(self->priv->delay,autocompletion_raise_event,self);
	}
}

static gboolean
autocompletion_raise_event(
								gpointer event)
{
	gchar* word;
	GscAutocompletionTrigger *self = GSC_AUTOCOMPLETION_TRIGGER(event);
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
							GSC_AUTOCOMPLETION_TRIGGER_NAME,
							self);
	}
	return FALSE;
}

static const gchar* gsc_autocompletion_trigger_real_get_name(GtkSourceCompletionTrigger *self)
{
	return GSC_AUTOCOMPLETION_TRIGGER_NAME;
}

static gboolean
gsc_autocompletion_trigger_real_activate (GtkSourceCompletionTrigger* base)
{
	g_debug("Activating Autocompletion trigger");
	GscAutocompletionTrigger *self = GSC_AUTOCOMPLETION_TRIGGER(base);
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
gsc_autocompletion_trigger_real_deactivate (GtkSourceCompletionTrigger* base)
{
	g_debug("Deactivating Autocompletion trigger");
	GscAutocompletionTrigger *self = GSC_AUTOCOMPLETION_TRIGGER(base);
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

static void gsc_autocompletion_trigger_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec)
{
}


static void gsc_autocompletion_trigger_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec)
{
}

static void gsc_autocompletion_trigger_init (GscAutocompletionTrigger * self)
{
	self->priv = g_new0(GscAutocompletionTriggerPrivate, 1);
	g_debug("Init Autocompletion trigger");
}

static void gsc_autocompletion_trigger_finalize(GObject *object)
{
	g_debug("Finish Autocompletion trigger");
	GscAutocompletionTrigger *self;
	self = GSC_AUTOCOMPLETION_TRIGGER(object);
	g_free(self->priv->actual_word);
	G_OBJECT_CLASS(gsc_autocompletion_trigger_parent_class)->finalize(object);
}

static void gsc_autocompletion_trigger_class_init (GscAutocompletionTriggerClass * klass)
{
	gsc_autocompletion_trigger_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->get_property = gsc_autocompletion_trigger_get_property;
	G_OBJECT_CLASS (klass)->set_property = gsc_autocompletion_trigger_set_property;
	G_OBJECT_CLASS (klass)->finalize = gsc_autocompletion_trigger_finalize;
}

static void gsc_autocompletion_trigger_interface_init (GtkSourceCompletionTriggerIface * iface)
{
	gsc_autocompletion_trigger_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_name = gsc_autocompletion_trigger_real_get_name;
	iface->activate = gsc_autocompletion_trigger_real_activate;
	iface->deactivate = gsc_autocompletion_trigger_real_deactivate;
}

GType gsc_autocompletion_trigger_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (GscAutocompletionTriggerClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gsc_autocompletion_trigger_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GscAutocompletionTrigger), 0, (GInstanceInitFunc) gsc_autocompletion_trigger_init };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "GscAutocompletionTrigger", &g_define_type_info, 0);
		static const GInterfaceInfo gsc_autocompletion_trigger_info = { (GInterfaceInitFunc) gsc_autocompletion_trigger_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		g_type_add_interface_static (g_define_type_id, GTK_SOURCE_COMPLETION_TYPE_TRIGGER, &gsc_autocompletion_trigger_info);
	}
	return g_define_type_id;
}

GscAutocompletionTrigger*
gsc_autocompletion_trigger_new(GtkSourceCompletion *completion)
{
	GscAutocompletionTrigger *self = GSC_AUTOCOMPLETION_TRIGGER (g_object_new (TYPE_GSC_AUTOCOMPLETION_TRIGGER, NULL));
	self->priv->completion = completion;
	
	self->priv->actual_word = NULL;
	self->priv->delay = AUTOCOMPLETION_DELAY;
	self->priv->view = gtk_source_completion_get_view(completion);
	self->priv->source_id = 0;
	
	return self;
}

void
gsc_autocompletion_trigger_set_delay(GscAutocompletionTrigger* trigger, guint delay)
{
	trigger->priv->delay = delay;
}


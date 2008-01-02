 /* gsc-documentwords-provider.c - Type here a short description of your plugin
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
#include "gsc-autocompletion-trigger.h"
#include "gsc-documentwords-provider.h"

#define ICON_FILE ICON_DIR"/document-words-icon.png"

struct _GscDocumentwordsProviderPrivate {
	GCompletion *completion;
	gboolean is_completing;
	GHashTable *current_words;
	GList *temp_list;
	GdkPixbuf *icon;
};

#define GSC_DOCUMENTWORDS_PROVIDER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_GSC_DOCUMENTWORDS_PROVIDER, GscDocumentwordsProviderPrivate))
enum  {
	GSC_DOCUMENTWORDS_PROVIDER_DUMMY_PROPERTY,
};
static const gchar* gsc_documentwords_provider_real_get_name(GtkSourceCompletionProvider *self);
static GList* gsc_documentwords_provider_real_get_data (GtkSourceCompletionProvider* base, GtkTextView* completion, const gchar* event_name, gpointer event_data);
static void gsc_documentwords_provider_real_end_completion (GtkSourceCompletionProvider* base, GtkTextView* view);
static void gsc_documentwords_provider_real_data_selected (GtkSourceCompletionProvider* base, GtkTextView* completion, GtkSourceCompletionItem* data);
static gchar* gsc_documentwords_provider_real_get_item_info_markup (GtkSourceCompletionProvider *self, GtkSourceCompletionItem *item);
static gpointer gsc_documentwords_provider_parent_class = NULL;
static GtkSourceCompletionProviderIface* gsc_documentwords_provider_gtk_source_completion_provider_parent_iface = NULL;

const int  NEWLINE = '\n';

static int 
is_separador(gunichar c)
{
	if (g_unichar_isalnum(c) == 0 && c != '_')
	{
		return 1;
	}
	else
	{
		return 0;
	}
}

static gboolean
is_valid_word(gchar *current_word, gchar *completion_word)
{
	if (g_utf8_collate(current_word,completion_word) == 0 ||
			g_utf8_strlen(completion_word,5)<4)
		return FALSE;
		
	return TRUE;
}

static GHashTable*
get_all_words(gchar* text)
{
	GHashTable *result = g_hash_table_new_full(
						g_str_hash,
						g_str_equal,
						g_free,
						NULL);
	gint state, pos = 0, posini = 0, posfin = 0;
	gchar *inicio = NULL, *fin = NULL, *actual = NULL, *temp = NULL;
	gunichar character;

	/*state = 0 when we not have a word, state = 1 when we are into a word*/
	state = 0;
	actual = text;
	character = g_utf8_get_char(actual);
	while (character != 0)
	{
		if (is_separador(character))
		{
			if (state == 1)
			{	
				fin = actual;
				posfin = pos;
				/*strncpy(word,inicio, posfin-posini );
				word[posfin-posini] = '\0';*/
				temp = g_strndup(inicio,posfin-posini);
				g_hash_table_insert(result,temp,NULL);
			}
			state = 0;
			
		}
		else if (state == 0)
		{
			//prev word = blank = new word
			inicio = actual;
			posini = pos;
			state = 1;
		}
	    
		//actual++;
		actual = g_utf8_next_char(actual);
		character = g_utf8_get_char(actual);
		pos++;
	}

	if (state == 1 && (pos-posini > 0))
	{
		/*strncpy(word,inicio, pos-posini );
		word[pos-posini] = '\0';
		g_hash_table_insert(result,g_strdup(word),NULL);*/
		g_hash_table_insert(result,g_strndup(inicio,pos-posini),NULL);
	}
	
	return result;
}

static void
clean_current_words(GscDocumentwordsProvider* self)
{
	g_completion_clear_items(self->priv->completion);
	/*Clean the previous data*/
	if (self->priv->current_words!=NULL)
	{	
		g_hash_table_destroy(self->priv->current_words);
		self->priv->current_words = NULL;
	}
	g_list_free(self->priv->temp_list);
	self->priv->temp_list = NULL;
}

static void
gh_add_key_to_list(gpointer key,
					gpointer value,
					gpointer user_data)
{
	GscDocumentwordsProvider *self = GSC_DOCUMENTWORDS_PROVIDER(user_data);
	self->priv->temp_list = g_list_append(self->priv->temp_list,key);
}

static const gchar* gsc_documentwords_provider_real_get_name(GtkSourceCompletionProvider *self)
{
	return GSC_DOCUMENTWORDS_PROVIDER_NAME;
}

static GList* 
gsc_documentwords_provider_real_get_data (GtkSourceCompletionProvider* base, GtkTextView* view, const gchar* event_name, gpointer event_data)
{
	GscDocumentwordsProvider *self = GSC_DOCUMENTWORDS_PROVIDER(base);
	gchar* text;
	gchar* current_word = gtk_source_view_get_last_word(view);
	GtkSourceCompletionItem *data;
	GList *completion_list = NULL;
	GList *data_list = NULL;
	
	/* 
	* We must stop the autocompletion event because the word is not correct
	* (The user wrotte an special character)
	* TODO This will change when we change to the new trigger API
	*/
	if ((strcmp(event_name,GSC_AUTOCOMPLETION_TRIGGER_NAME)==0) &&
			(current_word == NULL || strcmp("",current_word)==0))
	{
		if (self->priv->is_completing)
		{
			gsc_documentwords_provider_real_end_completion(base,view);
		}
		
		return NULL;
	}
	
	if (!self->priv->is_completing)
	{
		/* Load GCompletion */
		GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(view);
		g_object_get(text_buffer, "text", &text, NULL);
	
		self->priv->current_words = get_all_words(text);
		g_hash_table_foreach(self->priv->current_words,gh_add_key_to_list,self);
		g_completion_add_items(self->priv->completion, self->priv->temp_list);
		g_free(text);
		g_list_free(self->priv->temp_list);
		self->priv->temp_list = NULL;
	}
		
	completion_list = g_completion_complete_utf8(self->priv->completion,current_word,NULL);
	
	gint i = 0;
	if (completion_list!=NULL)
	{
		do
		{
			if (i>500)
			{
				g_debug("too many items (>500)");
				break;
			}
			i++;
			if (is_valid_word(current_word,completion_list->data))
			{
				data = gtk_source_completion_item_new(0,completion_list->data,self->priv->icon,15,NULL);
				data_list = g_list_append(data_list,data);
			}
		}while( (completion_list = g_list_next(completion_list)) != NULL);
	}
	
	if (data_list!=NULL)
	{
		self->priv->is_completing = TRUE;
	}
	else
	{
		clean_current_words(self);
		self->priv->is_completing = FALSE;
	}

	
	/* GtkSourceCompletion frees this list and data */
	return data_list;
}

static void gsc_documentwords_provider_real_end_completion (GtkSourceCompletionProvider* base, GtkTextView* completion)
{
	GscDocumentwordsProvider *self = GSC_DOCUMENTWORDS_PROVIDER(base);
	/*Clean current word list*/
	clean_current_words(self);
	
	self->priv->is_completing = FALSE;
	
}

static void gsc_documentwords_provider_real_data_selected (GtkSourceCompletionProvider* base, GtkTextView* text_view, GtkSourceCompletionItem* data)
{
	gtk_source_view_replace_actual_word(text_view,
					gtk_source_completion_item_get_name(data));
}

static gchar*
gsc_documentwords_provider_real_get_item_info_markup(GtkSourceCompletionProvider *self,
				GtkSourceCompletionItem *item)
{
	return NULL;
}

static void gsc_documentwords_provider_get_property (GObject * object, guint property_id, GValue * value, GParamSpec * pspec)
{
}


static void gsc_documentwords_provider_set_property (GObject * object, guint property_id, const GValue * value, GParamSpec * pspec)
{
}

static void gsc_documentwords_provider_finalize(GObject *object)
{
	g_debug("Finish GscDocumentwordsProvider");
	GscDocumentwordsProvider *self;
	self = GSC_DOCUMENTWORDS_PROVIDER(object);
	clean_current_words(self);
	gdk_pixbuf_unref (self->priv->icon);
	g_completion_free(self->priv->completion);
	
	G_OBJECT_CLASS(gsc_documentwords_provider_parent_class)->finalize(object);
}


static void gsc_documentwords_provider_class_init (GscDocumentwordsProviderClass * klass)
{
	g_debug("Init GscDocumentwordsProvider");
	gsc_documentwords_provider_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->get_property = gsc_documentwords_provider_get_property;
	G_OBJECT_CLASS (klass)->set_property = gsc_documentwords_provider_set_property;
	G_OBJECT_CLASS (klass)->finalize = gsc_documentwords_provider_finalize;
}


static void gsc_documentwords_provider_gtk_source_completion_provider_interface_init (GtkSourceCompletionProviderIface * iface)
{
	gsc_documentwords_provider_gtk_source_completion_provider_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_name = gsc_documentwords_provider_real_get_name;
	iface->get_data = gsc_documentwords_provider_real_get_data;
	iface->end_completion = gsc_documentwords_provider_real_end_completion;
	iface->data_selected = gsc_documentwords_provider_real_data_selected;
	iface->get_item_info_markup = gsc_documentwords_provider_real_get_item_info_markup;
}


static void gsc_documentwords_provider_init (GscDocumentwordsProvider * self)
{
	self->priv = g_new0(GscDocumentwordsProviderPrivate, 1);
	self->priv->completion = g_completion_new(NULL);
	self->priv->current_words = NULL;
	self->priv->temp_list = NULL;
	self->priv->is_completing = FALSE;
	self->priv->icon = gdk_pixbuf_new_from_file(ICON_FILE,NULL);
}

GType gsc_documentwords_provider_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (GscDocumentwordsProviderClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gsc_documentwords_provider_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GscDocumentwordsProvider), 0, (GInstanceInitFunc) gsc_documentwords_provider_init };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "GscDocumentwordsProvider", &g_define_type_info, 0);
		static const GInterfaceInfo gtk_source_completion_provider_info = { (GInterfaceInitFunc) gsc_documentwords_provider_gtk_source_completion_provider_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		g_type_add_interface_static (g_define_type_id, GTK_SOURCE_COMPLETION_TYPE_PROVIDER, &gtk_source_completion_provider_info);
	}
	return g_define_type_id;
}

/**
 * gsc_documentwords_provider_new:
 *
 * Returns The new #GscDocumentwordsProvider
 *
 */
GscDocumentwordsProvider*
gsc_documentwords_provider_new()
{
	return GSC_DOCUMENTWORDS_PROVIDER (g_object_new (TYPE_GSC_DOCUMENTWORDS_PROVIDER, NULL));
}


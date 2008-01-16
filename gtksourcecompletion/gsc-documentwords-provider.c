/* Copyright (C) 2007 - Jesús Barbero <chuchiperriman@gmail.com>
 *  *
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
	GscDocumentwordsProviderSortType sort_type;
};

#define GSC_DOCUMENTWORDS_PROVIDER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_GSC_DOCUMENTWORDS_PROVIDER, GscDocumentwordsProviderPrivate))
enum  {
	GSC_DOCUMENTWORDS_PROVIDER_DUMMY_PROPERTY,
};
static const gchar* gsc_documentwords_provider_real_get_name(GtkSourceCompletionProvider *self);
static GList* gsc_documentwords_provider_real_get_data (GtkSourceCompletionProvider* base, GtkSourceCompletion* completion, GtkSourceCompletionTrigger* trigger);
static void gsc_documentwords_provider_real_end_completion (GtkSourceCompletionProvider* base, GtkSourceCompletion* completion);
static void gsc_documentwords_provider_real_data_selected (GtkSourceCompletionProvider* base, GtkSourceCompletion* completion, GtkSourceCompletionItem* data);
static gchar* gsc_documentwords_provider_real_get_item_info_markup (GtkSourceCompletionProvider *self, GtkSourceCompletionItem *item);
static gpointer gsc_documentwords_provider_parent_class = NULL;
static GtkSourceCompletionProviderIface* gsc_documentwords_provider_gtk_source_completion_provider_parent_iface = NULL;

static gboolean
pred_is_separator(gunichar ch, gpointer user_data)
{
	return gsc_char_is_separator(ch);
}

static gint
utf8_len_compare(gconstpointer a, gconstpointer b)
{
    glong lena,lenb;
    lena = g_utf8_strlen(gtk_source_completion_item_get_name((GtkSourceCompletionItem*)a),-1);
    lenb = g_utf8_strlen(gtk_source_completion_item_get_name((GtkSourceCompletionItem*)b),-1);
    if (lena==lenb)
        return 0;
    else if (lena<lenb)
        return -1;
    else
        return 1;
}

static GHashTable*
get_all_words( GtkTextBuffer *buffer )
{
	GtkTextIter start_iter;
    GtkTextIter prev_iter;
    gchar *word;
	GHashTable *result = g_hash_table_new_full(
						g_str_hash,
						g_str_equal,
						g_free,
						NULL);
	
	
	gtk_text_buffer_get_start_iter(buffer,&start_iter);
    prev_iter = start_iter;
	while (gtk_text_iter_forward_find_char(
			&start_iter,
			(GtkTextCharPredicate)pred_is_separator,
			NULL,
			NULL))
	{
        word = gtk_text_iter_get_text(&prev_iter,&start_iter);
        if (strlen(word)>0)
        {
            /*TODO Try to eliminate this g_strdup*/
            g_hash_table_insert(result,g_strdup(word),NULL);
        }
        prev_iter = start_iter;
        gtk_text_iter_forward_char(&prev_iter);
	}

    if (!gtk_text_iter_is_end(&prev_iter))
    {
        gtk_text_buffer_get_end_iter(buffer,&start_iter);
        word = gtk_text_iter_get_text(&prev_iter,&start_iter);
        if (strlen(word)>0)
        {
            /*TODO Try to eliminate this g_strdup*/
            g_hash_table_insert(result,g_strdup(word),NULL);
        }
        prev_iter = start_iter;
        gtk_text_iter_forward_char(&prev_iter);
        
    }
			
	return result;
}

static gboolean
is_valid_word(gchar *current_word, gchar *completion_word)
{
	if (g_utf8_collate(current_word,completion_word) == 0 ||
			g_utf8_strlen(completion_word,-1)<4)
		return FALSE;
		
	return TRUE;
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

static GList*
_sort_completion_list(GscDocumentwordsProvider *self, GList *data_list)
{
	switch(self->priv->sort_type)
	{
		case GSC_DOCUMENTWORDS_PROVIDER_SORT_BY_LENGTH:
		{
			data_list = g_list_sort(data_list, (GCompareFunc)utf8_len_compare );
			break;
		}
		default: 
			break;
	}
	
	return data_list;
}


static const gchar* gsc_documentwords_provider_real_get_name(GtkSourceCompletionProvider *self)
{
	return GSC_DOCUMENTWORDS_PROVIDER_NAME;
}

static GList* 
gsc_documentwords_provider_real_get_data (GtkSourceCompletionProvider* base, GtkSourceCompletion* completion, GtkSourceCompletionTrigger *trigger)
{
	GscDocumentwordsProvider *self = GSC_DOCUMENTWORDS_PROVIDER(base);
	GtkTextView *view = gtk_source_completion_get_view(completion);
	gchar* current_word = gtk_source_view_get_last_word(view);
	gchar *cleaned_word;
	GtkSourceCompletionItem *data;
	GList *completion_list = NULL;
	GList *data_list = NULL;
	
	cleaned_word = gsc_clear_word(current_word);
	g_free(current_word);
	/* 
	* We must stop the autocompletion event because the word is not correct
	* (The user wrotte an special character)
	* TODO This will change when we change to the new trigger API
	*/
	if ( IS_GSC_AUTOCOMPLETION_TRIGGER(trigger) && cleaned_word == NULL)
	{
		if (self->priv->is_completing)
		{
			gsc_documentwords_provider_real_end_completion(base,completion);
		}
		
		return NULL;
	}
	
	if (!self->priv->is_completing)
	{
		/* Load GCompletion */
		GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(view);
		self->priv->current_words = get_all_words(text_buffer);
		g_hash_table_foreach(self->priv->current_words,gh_add_key_to_list,self);
		g_completion_add_items(self->priv->completion, self->priv->temp_list);
		g_list_free(self->priv->temp_list);
		self->priv->temp_list = NULL;
	}
	
	completion_list = g_completion_complete_utf8(self->priv->completion, cleaned_word, NULL);
	
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
			if (is_valid_word(cleaned_word,completion_list->data))
			{
				data = gtk_source_completion_item_new(0,completion_list->data,self->priv->icon,15,NULL);
				data_list = g_list_append(data_list,data);
			}
		}while( (completion_list = g_list_next(completion_list)) != NULL);
	}
	
	if (data_list!=NULL)
	{
		self->priv->is_completing = TRUE;
		data_list = _sort_completion_list(self,data_list);
	}
	else
	{
		clean_current_words(self);
		self->priv->is_completing = FALSE;
	}
	
	g_free(cleaned_word);

	/* GtkSourceCompletion frees this list and data */
	return data_list;
}

static void gsc_documentwords_provider_real_end_completion (GtkSourceCompletionProvider* base, GtkSourceCompletion* completion)
{
	GscDocumentwordsProvider *self = GSC_DOCUMENTWORDS_PROVIDER(base);
	/*Clean current word list*/
	clean_current_words(self);
	
	self->priv->is_completing = FALSE;
	
}

static void gsc_documentwords_provider_real_data_selected (GtkSourceCompletionProvider* base, GtkSourceCompletion* completion, GtkSourceCompletionItem* data)
{
	GtkTextView *view = gtk_source_completion_get_view(completion);
	gtk_source_view_replace_actual_word(view,
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
	self->priv->sort_type = GSC_DOCUMENTWORDS_PROVIDER_SORT_BY_LENGTH;
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

void
gsc_documentwords_provider_set_sort_type(GscDocumentwordsProvider *prov,
											 GscDocumentwordsProviderSortType sort_type)
{
	prov->priv->sort_type = sort_type;
}

GscDocumentwordsProviderSortType
gsc_documentwords_provider_get_sort_type(GscDocumentwordsProvider *prov)
{
	return prov->priv->sort_type;
}

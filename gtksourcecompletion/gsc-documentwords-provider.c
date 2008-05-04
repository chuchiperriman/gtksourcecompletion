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
#include "gsc-documentwords-provider.h"
#include "gtksourcecompletion-utils.h"

#define ICON_FILE ICON_DIR"/document-words-icon.png"

struct _GscDocumentwordsProviderPrivate {
	gboolean is_completing;
	GHashTable *current_words;
	GList *data_list;
	gchar *cleaned_word;
	GdkPixbuf *icon;
	gint count;
	GscDocumentwordsProviderSortType sort_type;
	GtkTextIter start_iter;
};

#define GSC_DOCUMENTWORDS_PROVIDER_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_GSC_DOCUMENTWORDS_PROVIDER, GscDocumentwordsProviderPrivate))
enum  {
	GSC_DOCUMENTWORDS_PROVIDER_DUMMY_PROPERTY,
};
static const gchar* 
gsc_documentwords_provider_real_get_name(GtkSourceCompletionProvider *self);
static GList* 
gsc_documentwords_provider_real_get_data (GtkSourceCompletionProvider* base, 
					  GtkSourceCompletion* completion, 
					  GtkSourceCompletionTrigger* trigger);
static void 
gsc_documentwords_provider_real_finish (GtkSourceCompletionProvider* base, 
						GtkSourceCompletion* completion);

static GtkSourceCompletionProviderIface* gsc_documentwords_provider_parent_iface = NULL;
static gpointer gsc_documentwords_provider_parent_class = NULL;

static gboolean
pred_is_separator(gunichar ch, gpointer user_data)
{
	return gsc_char_is_separator(ch);
}

static gint
utf8_len_compare(gconstpointer a, gconstpointer b)
{
    glong lena,lenb;
    lena = g_utf8_strlen(gtk_source_completion_proposal_get_name((GtkSourceCompletionProposal*)a),-1);
    lenb = g_utf8_strlen(gtk_source_completion_proposal_get_name((GtkSourceCompletionProposal*)b),-1);
    if (lena==lenb)
        return 0;
    else if (lena<lenb)
        return -1;
    else
        return 1;
}

static GHashTable*
get_all_words(GscDocumentwordsProvider* self, GtkTextBuffer *buffer )
{
	GtkTextIter start_iter;
	GtkTextIter prev_iter;
	gchar *word;
	GHashTable *result = g_hash_table_new_full(g_str_hash,
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
		if (gtk_text_iter_compare(&self->priv->start_iter,&prev_iter)!=0)
		{
			word = gtk_text_iter_get_text(&prev_iter,&start_iter);
        
			if (strlen(word)>0)
				g_hash_table_insert(result,word,NULL);
			else
				g_free(word);
		}
		prev_iter = start_iter;
		gtk_text_iter_forward_char(&prev_iter);
	}

	if (!gtk_text_iter_is_end(&prev_iter))
	{
		gtk_text_buffer_get_end_iter(buffer,&start_iter);
		if (gtk_text_iter_compare(&self->priv->start_iter,&prev_iter)!=0)
		{
			word = gtk_text_iter_get_text(&prev_iter,&start_iter);
			if (strlen(word)>0)
				g_hash_table_insert(result,word,NULL);
			else
				g_free(word);
		}
		prev_iter = start_iter;
	        gtk_text_iter_forward_char(&prev_iter);
	}
	return result;
}

static gboolean
is_valid_word(gchar *current_word, gchar *completion_word)
{
	gint len_cur = strlen (current_word);
	if (g_utf8_collate(current_word,completion_word) == 0 ||
			g_utf8_strlen(completion_word,-1)<4)
			return FALSE;

	if (strncmp(current_word,completion_word,len_cur)==0)
	{
		return TRUE;
	}
	return FALSE;
}

static void
clean_current_words(GscDocumentwordsProvider* self)
{
	/*Clean the previous data*/
	if (self->priv->current_words!=NULL)
	{	
		g_hash_table_destroy(self->priv->current_words);
		self->priv->current_words = NULL;
	}
}

/*
 * Check the proposals hash and inserts the completion proposal into the final list
 */
static void
gh_add_key_to_list(gpointer key,
		   gpointer value,
		   gpointer user_data)
{

	GscDocumentwordsProvider *self = GSC_DOCUMENTWORDS_PROVIDER(user_data);
	if (self->priv->count>=500)
	{
		return;
	}
	GtkSourceCompletionProposal *data;
	if (is_valid_word(self->priv->cleaned_word,(gchar*)key))
	{
		self->priv->count++;
		data = gtk_source_completion_proposal_new(0,
						      (gchar*)key,
						       self->priv->icon,
						       15,
						       NULL);
		self->priv->data_list = g_list_append(self->priv->data_list,data);
	}
}

static GList*
_sort_completion_list(GscDocumentwordsProvider *self, GList *data_list)
{
	switch(self->priv->sort_type)
	{
		case GSC_DOCUMENTWORDS_PROVIDER_SORT_BY_LENGTH:
		{
			data_list = g_list_sort(data_list,
						(GCompareFunc)utf8_len_compare );
			break;
		}
		default: 
			break;
	}
	
	return data_list;
}


static const gchar* 
gsc_documentwords_provider_real_get_name(GtkSourceCompletionProvider *self)
{
	return GSC_DOCUMENTWORDS_PROVIDER_NAME;
}

static GList* 
gsc_documentwords_provider_real_get_data (GtkSourceCompletionProvider* base, 
					  GtkSourceCompletion* completion, 
					  GtkSourceCompletionTrigger *trigger)
{
	GscDocumentwordsProvider *self = GSC_DOCUMENTWORDS_PROVIDER(base);
	GtkTextView *view = gtk_source_completion_get_view(completion);
	
	gchar* current_word = gtk_source_view_get_last_word_and_iter(view,
								     &self->priv->start_iter,
								     NULL);
	self->priv->cleaned_word = gsc_clear_word(current_word);
	g_free(current_word);
	if (self->priv->cleaned_word == NULL)
	{
		if (self->priv->is_completing)
		{
			gsc_documentwords_provider_real_finish(base,
							       completion);
		}
		return NULL;
	}
	
	if (!self->priv->is_completing)
	{
		
		GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(view);
		self->priv->current_words = get_all_words(self,text_buffer);
	}
	
	self->priv->data_list = NULL;
	self->priv->count = 0;
	g_hash_table_foreach(self->priv->current_words,gh_add_key_to_list,self);
	g_free(self->priv->cleaned_word);
	self->priv->cleaned_word = NULL;
	
	if (self->priv->data_list!=NULL)
	{
		self->priv->is_completing = TRUE;
		self->priv->data_list = _sort_completion_list(self,
							      self->priv->data_list);
	}
	else
	{
		clean_current_words(self);
		self->priv->is_completing = FALSE;
	}

	/* GtkSourceCompletion frees this list and data */
	return self->priv->data_list;
}

static void 
gsc_documentwords_provider_real_finish (GtkSourceCompletionProvider* base, 
						GtkSourceCompletion* completion)
{
	GscDocumentwordsProvider *self = GSC_DOCUMENTWORDS_PROVIDER(base);
	/*Clean current word list*/
	clean_current_words(self);
	
	self->priv->is_completing = FALSE;
	
}

static void 
gsc_documentwords_provider_get_property (GObject * object, 
					 guint property_id, 
					 GValue * value, 
					 GParamSpec * pspec)
{
}


static void 
gsc_documentwords_provider_set_property (GObject * object, 
					 guint property_id, 
					 const GValue * value, 
					 GParamSpec * pspec)
{
}

static void 
gsc_documentwords_provider_finalize(GObject *object)
{
	g_debug("Finish GscDocumentwordsProvider");
	GscDocumentwordsProvider *self;
	self = GSC_DOCUMENTWORDS_PROVIDER(object);
	clean_current_words(self);
	g_free(self->priv->cleaned_word);
	self->priv->cleaned_word = NULL;
	self->priv->data_list = NULL;
	self->priv->count= 0;
	gdk_pixbuf_unref (self->priv->icon);
	//g_completion_free(self->priv->completion);
	
	G_OBJECT_CLASS(gsc_documentwords_provider_parent_class)->finalize(object);
}


static void 
gsc_documentwords_provider_class_init (GscDocumentwordsProviderClass * klass)
{
	g_debug("Init GscDocumentwordsProvider");
	gsc_documentwords_provider_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->get_property = gsc_documentwords_provider_get_property;
	G_OBJECT_CLASS (klass)->set_property = gsc_documentwords_provider_set_property;
	G_OBJECT_CLASS (klass)->finalize = gsc_documentwords_provider_finalize;
}


static void 
gsc_documentwords_provider_interface_init (GtkSourceCompletionProviderIface * iface)
{
	gsc_documentwords_provider_parent_iface = g_type_interface_peek_parent (iface);
	iface->get_name = gsc_documentwords_provider_real_get_name;
	iface->get_data = gsc_documentwords_provider_real_get_data;
	iface->finish = gsc_documentwords_provider_real_finish;
}


static void gsc_documentwords_provider_init (GscDocumentwordsProvider * self)
{
	self->priv = g_new0(GscDocumentwordsProviderPrivate, 1);
	//self->priv->completion = g_completion_new(NULL);
	self->priv->current_words = NULL;
	//self->priv->temp_list = NULL;
	self->priv->is_completing = FALSE;
	self->priv->count=0;
	self->priv->cleaned_word=NULL;
	self->priv->icon = gdk_pixbuf_new_from_file(ICON_FILE,NULL);
	self->priv->sort_type = GSC_DOCUMENTWORDS_PROVIDER_SORT_BY_LENGTH;
}

GType gsc_documentwords_provider_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = {sizeof (GscDocumentwordsProviderClass), 
							     (GBaseInitFunc) NULL,
							     (GBaseFinalizeFunc) NULL, 
							     (GClassInitFunc) gsc_documentwords_provider_class_init, 
							     (GClassFinalizeFunc) NULL, 
							     NULL, 
							     sizeof (GscDocumentwordsProvider), 
							     0, 
							     (GInstanceInitFunc) gsc_documentwords_provider_init 
							     };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, 
							   "GscDocumentwordsProvider", 
							   &g_define_type_info,
							   0);
		static const GInterfaceInfo gtk_source_completion_provider_info = {(GInterfaceInitFunc) gsc_documentwords_provider_interface_init,
										   (GInterfaceFinalizeFunc) NULL, 
										   NULL};
		g_type_add_interface_static (g_define_type_id, 
					     GTK_SOURCE_COMPLETION_TYPE_PROVIDER, 
					     &gtk_source_completion_provider_info);
	}
	return g_define_type_id;
}

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

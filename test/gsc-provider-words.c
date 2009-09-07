/* 
 *  gsc-provider-words.c - Type here a short description of your plugin
 *
 *  Copyright (C) 2008 - perriman
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <string.h>
#include "gsc-provider-words.h"
#include <gtksourcecompletion/gsc-completion.h>
#include <gtksourcecompletion/gsc-item.h>
#include <gtksourcecompletion/gsc-utils.h>

#define GSC_PROVIDER_WORDS_GET_PRIVATE(object)(G_TYPE_INSTANCE_GET_PRIVATE((object), GSC_TYPE_PROVIDER_WORDS, GscProviderWordsPrivate))

static void	 gsc_provider_words_iface_init	(GscProviderIface *iface);

struct _GscProviderWordsPrivate
{
	gchar *name;
	GdkPixbuf *icon;
	GdkPixbuf *proposal_icon;
	gboolean is_completing;
	GHashTable *current_words;
	GList *data_list;
	gchar *cleaned_word;
	gint count;
	GscProviderWordsSortType sort_type;
	GtkTextIter start_iter;
};

G_DEFINE_TYPE_WITH_CODE (GscProviderWords,
			 gsc_provider_words,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GSC_TYPE_PROVIDER,
				 		gsc_provider_words_iface_init))

static gboolean
pred_is_separator(gunichar ch, gpointer user_data)
{
	return gsc_utils_is_separator(ch);
}

static gint
utf8_len_compare(gconstpointer a, gconstpointer b)
{
    glong lena,lenb;
    lena = g_utf8_strlen(gsc_proposal_get_label((GscProposal*)a),-1);
    lenb = g_utf8_strlen(gsc_proposal_get_label((GscProposal*)b),-1);
    if (lena==lenb)
        return 0;
    else if (lena<lenb)
        return -1;
    else
        return 1;
}

static GHashTable*
get_all_words(GscProviderWords* self, GtkTextBuffer *buffer )
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
	if (g_utf8_strlen(completion_word, -1) < 3)
		return FALSE;
	
	if (current_word==NULL)
		return TRUE;
		
	gint len_cur = strlen (current_word);
	if (g_utf8_collate(current_word,completion_word) == 0)
			return FALSE;

	if (len_cur!=0 && strncmp(current_word,completion_word,len_cur)==0)
	{
		return TRUE;
	}
	return FALSE;
}

static void
clean_current_words(GscProviderWords* self)
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

	GscProviderWords *self = GSC_PROVIDER_WORDS(user_data);
	if (self->priv->count>=500)
	{
		return;
	}
	GscItem *data;
	if (is_valid_word(self->priv->cleaned_word,(gchar*)key))
	{
		self->priv->count++;
		data = gsc_item_new (key, key, self->priv->proposal_icon, NULL);
		/*data = gsc_proposal_new((gchar*)key,
					NULL,
					self->priv->icon);*/
		self->priv->data_list = g_list_append(self->priv->data_list,data);
	}
}

static GList*
_sort_completion_list(GscProviderWords *self, GList *data_list)
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


static const gchar * 
gsc_provider_words_get_name (GscProvider *self)
{
	return GSC_PROVIDER_WORDS (self)->priv->name;
}

static GdkPixbuf * 
gsc_provider_words_get_icon (GscProvider *self)
{
	return GSC_PROVIDER_WORDS (self)->priv->icon;
}

static void
gsc_provider_words_populate_completion (GscProvider *base,
					GscContext  *context)
{
	GscProviderWords *self = GSC_PROVIDER_WORDS (base);
	GtkTextIter current_iter;
	GtkTextIter end_iter;
	GtkTextView *view;

	view = gsc_context_get_view (context);
	GtkTextBuffer *text_buffer = gtk_text_view_get_buffer(view);
	gsc_utils_get_iter_at_insert (view, &current_iter);
	
	gchar* current_word = gsc_utils_get_word_iter(text_buffer,
						      &current_iter,
						      &self->priv->start_iter,
						      &end_iter);
	
	self->priv->cleaned_word = gsc_utils_clear_word(current_word);
	g_free(current_word);
	
	if (!self->priv->is_completing)
	{
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

	/* GscManager frees this list and data */
	gsc_context_add_proposals (context, base, self->priv->data_list);
}

/*
static gboolean
gsc_provider_words_filter_proposal (GscProvider *provider,
                                   GtkSourceCompletionProposal *proposal,
                                   GtkTextIter                 *iter,
                                   const gchar                 *criteria)
{
	const gchar *label;
	
	label = gtk_source_completion_proposal_get_label (proposal);
	return g_str_has_prefix (label, criteria);
}
*/

static const gchar *
gsc_provider_words_get_capabilities (GscProvider *provider)
{
	return GSC_COMPLETION_CAPABILITY_INTERACTIVE ","
	       GSC_COMPLETION_CAPABILITY_AUTOMATIC;
}

static void 
gsc_provider_words_finalize (GObject *object)
{
	GscProviderWords *provider = GSC_PROVIDER_WORDS (object);
	
	g_free (provider->priv->name);
	
	if (provider->priv->icon != NULL)
	{
		g_object_unref (provider->priv->icon);
	}
	
	if (provider->priv->proposal_icon != NULL)
	{
		g_object_unref (provider->priv->proposal_icon);
	}

	G_OBJECT_CLASS (gsc_provider_words_parent_class)->finalize (object);
}

static void 
gsc_provider_words_class_init (GscProviderWordsClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);
	
	object_class->finalize = gsc_provider_words_finalize;
	
	g_type_class_add_private (object_class, sizeof(GscProviderWordsPrivate));
}

static void
gsc_provider_words_iface_init (GscProviderIface *iface)
{
	iface->get_name = gsc_provider_words_get_name;
	iface->get_icon = gsc_provider_words_get_icon;

	iface->populate_completion = gsc_provider_words_populate_completion;
	//iface->filter_proposal = gsc_provider_words_filter_proposal;
	iface->get_capabilities = gsc_provider_words_get_capabilities;
}

static void 
gsc_provider_words_init (GscProviderWords * self)
{
	GtkIconTheme *theme;
	gint width;
	
	self->priv = GSC_PROVIDER_WORDS_GET_PRIVATE (self);
	
	theme = gtk_icon_theme_get_default ();

	gtk_icon_size_lookup (GTK_ICON_SIZE_MENU, &width, NULL);
	self->priv->proposal_icon = gtk_icon_theme_load_icon (theme,
	                                                      GTK_STOCK_FILE,
	                                                      width,
	                                                      GTK_ICON_LOOKUP_USE_BUILTIN,
	                                                      NULL);
}

GscProviderWords *
gsc_provider_words_new (const gchar *name,
                       GdkPixbuf   *icon)
{
	GscProviderWords *ret = g_object_new (GSC_TYPE_PROVIDER_WORDS, NULL);
	
	ret->priv->name = g_strdup (name);
	
	if (icon != NULL)
	{
		ret->priv->icon = g_object_ref (icon);
	}

	return ret;
}

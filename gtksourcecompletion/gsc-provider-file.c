/* 
 *  gsc-provider-file.c - Type here a short description of your plugin
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

#include "gsc-provider-file.h"
#include "gsc-utils.h"

/*TODO Change the icon*/
#define ICON_FILE ICON_DIR"/document-words-icon.png"

#define GSC_PROVIDER_FILE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), \
					  GSC_TYPE_PROVIDER_FILE, \
					  GscProviderFilePrivate))

struct _GscProviderFilePrivate
{
	GFile* file_uri;
	GdkPixbuf *icon;
	GList *current_props;
	gchar *current_word;
	GtkTextView *view;
};

static void 		 gsc_provider_file_iface_init		(GscProviderIface *iface);

G_DEFINE_TYPE_WITH_CODE (GscProviderFile,
			 gsc_provider_file,
			 G_TYPE_OBJECT,
			 G_IMPLEMENT_INTERFACE (GSC_TYPE_PROVIDER,
				 		gsc_provider_file_iface_init))

static const gchar* 
gsc_provider_file_real_get_name (GscProvider* self)
{
	return GSC_PROVIDER_FILE_NAME;
}

static gboolean
table_foreach (gpointer key,
	       gpointer value,
	       gpointer user_data)
{
	GscProviderFile *self = GSC_PROVIDER_FILE (user_data);
	GscProposal* prop = NULL;
	
	if (gsc_is_valid_word (self->priv->current_word, (gchar*)key))
	{
		prop = gsc_proposal_new ((gchar*)key,
					 NULL,
					 self->priv->icon);
		self->priv->current_props = g_list_append (self->priv->current_props,
							   prop);
	}
	
	/*For free the current item into the table*/
	return TRUE;
}

static GList*
get_words (GscProviderFile *self,
	   gchar* buffer)
{
	gchar* ini = NULL;
	gchar* cur = buffer;
	gunichar c;
	gint count = 0;
	gint words = 0;
	GHashTable *table;
	
	self->priv->current_word = gsc_get_last_word_cleaned (self->priv->view);
	
	table = g_hash_table_new_full (g_str_hash,
				       g_str_equal,
				       g_free,
				       NULL);
	
	/*
	 * FIXME: I do not like this loop
	 */
	while (TRUE)
	{
		ini = cur;
		count = 0;
		
		c = g_utf8_get_char (cur);
		
		while (!gsc_char_is_separator (c))
		{
			count++;
			cur = g_utf8_next_char (cur);
			c = g_utf8_get_char (cur);
		}
		
		if (count > 0)
		{
			g_hash_table_insert (table,
					     g_strndup (ini, count),
					     NULL);
			words++;
		}
		
		if (*cur == '\0')
			break;
		
		cur = g_utf8_next_char (cur);
	}
	
	/*
	 * FIXME: We should check if current_props is not NULL and free the
	 * memory in that case.
	 */
	self->priv->current_props = NULL;
	g_hash_table_foreach_remove (table,
				     table_foreach,
				     self);
	
	g_hash_table_unref (table);
	
	return g_list_first (self->priv->current_props);
}

static GList* 
gsc_provider_file_real_get_proposals (GscProvider* base,
				      GscTrigger *trigger)
{
	GscProviderFile *self = GSC_PROVIDER_FILE (base);
	GFileInfo *info;
	guint64 size;
	GFileInputStream *stream;
	gboolean result;
	gsize nchars;
	gchar *buffer = NULL;
	GList *proposals = NULL;
	
	//FIXME: Use GError
	info = g_file_query_info (self->priv->file_uri,
				  G_FILE_ATTRIBUTE_STANDARD_SIZE,
				  G_FILE_QUERY_INFO_NONE,
				  NULL,
				  NULL);
	if (info == NULL)
	{
		g_warning ("The file does not exist");
		return NULL;
	}
	
	size = g_file_info_get_attribute_uint64 (info,
						 G_FILE_ATTRIBUTE_STANDARD_SIZE);
	g_object_unref (info);
	
	/*
	 * FIXME: I do not like this
	 */
	buffer = g_malloc (size + 1);
	if (buffer == NULL && size != 0)
	{
		g_warning ("This file is too big. Unable to allocate memory.");
		return NULL;
	}
	
	//FIXME: Use GError
	stream = g_file_read (self->priv->file_uri,
			      NULL, NULL);
	if (stream == NULL)
	{
		g_warning ("Could not open file");
		return NULL;
	}
	
	//FIXME: Use Gerror
	result = g_input_stream_read_all (G_INPUT_STREAM (stream),
					  buffer, size, &nchars,
					  NULL, NULL);
	if (!result)
	{
		g_free (buffer);
		return NULL;
	}
	
	if (buffer)
	{
		buffer[size] = '\0';
	}
	
	if (nchars > 0)
	{
		if (!g_utf8_validate (buffer, nchars, NULL))
		{
			gsize bytes_read;
			gchar *converted_text;

			converted_text = g_locale_to_utf8 (buffer,
							   nchars,
							   &bytes_read,
							   &nchars,
							   NULL);
			/* TODO support for other encodings
			converted_text = g_convert (buffer,
						   nchars,
						   "UTF-8",
						   "ISO-8859-1",
						   &bytes_read,
						   &new_len,
						   NULL);
			*/
			
			if (converted_text == NULL)
			{
				g_free (buffer);
				g_warning ("The file does not look like a text file"
					   " or the file encoding is not supported.");
				return NULL;
			}
			g_free (buffer);
			buffer = converted_text;
		}
	}
	
	proposals = get_words (self, buffer);
	g_free (buffer);
	
	return proposals;
}

static void
gsc_provider_file_real_finish (GscProvider* base)
{

}

static void 
gsc_provider_file_finalize (GObject *object)
{
	GscProviderFile *self = GSC_PROVIDER_FILE (object);
	
	if (self->priv->file_uri != NULL)
		g_object_unref (self->priv->file_uri);
	
	if (self->priv->current_props != NULL)
	{
		g_list_foreach (self->priv->current_props,
				(GFunc) g_object_unref, NULL);
		g_list_free (self->priv->current_props);
	}
	
	gdk_pixbuf_unref (self->priv->icon);
	
	G_OBJECT_CLASS (gsc_provider_file_parent_class)->finalize (object);
}


static void 
gsc_provider_file_class_init (GscProviderFileClass *klass)
{
	GObjectClass *object_class = G_OBJECT_CLASS (klass);

	g_type_class_add_private (klass, sizeof (GscProviderFilePrivate));

	object_class->finalize = gsc_provider_file_finalize;
}

static void
gsc_provider_file_iface_init (GscProviderIface *iface)
{
	iface->get_name      = gsc_provider_file_real_get_name;
	iface->get_proposals = gsc_provider_file_real_get_proposals;
	iface->finish        = gsc_provider_file_real_finish;
}


static void 
gsc_provider_file_init (GscProviderFile * self)
{
	self->priv = GSC_PROVIDER_FILE_GET_PRIVATE (self);
	
	self->priv->file_uri = NULL;
	self->priv->icon = gdk_pixbuf_new_from_file (ICON_FILE, NULL);
	self->priv->current_props = NULL;
	self->priv->view = NULL;
}

GscProviderFile*
gsc_provider_file_new (GtkTextView *view)
{
	GscProviderFile *self = GSC_PROVIDER_FILE (g_object_new (GSC_TYPE_PROVIDER_FILE, NULL));
	
	self->priv->view = view;
	
	return self;
}

void
gsc_provider_file_set_file (GscProviderFile *self,
			    const gchar *file)
{
	g_return_if_fail (GSC_IS_PROVIDER_FILE (self));
	g_return_if_fail (file != NULL);
	
	if (self->priv->file_uri != NULL)
		g_object_unref (self->priv->file_uri);
	
	self->priv->file_uri = g_file_new_for_path (file);
}

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

struct _GscProviderFilePrivate {
	GFile* file_uri;
	GdkPixbuf *icon;
	GList *current_props;
	gchar *current_word;
	GtkTextView *view;
};

#define GSC_PROVIDER_FILE_GET_PRIVATE(o) (G_TYPE_INSTANCE_GET_PRIVATE ((o), TYPE_GSC_PROVIDER_FILE, GscProviderFilePrivate))

static const gchar* 
gsc_provider_file_real_get_name (GscProvider* self);
static GList* 
gsc_provider_file_real_get_proposals (GscProvider* base,
				      GscTrigger *trigger);
static void 
gsc_provider_file_real_finish (GscProvider* base);

static gpointer 
gsc_provider_file_parent_class = NULL;
static GscProviderIface* 
gsc_provider_file_parent_iface = NULL;


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
	GscProposal* prop = NULL;
	GscProviderFile *self = GSC_PROVIDER_FILE(user_data);
	
	if (gsc_is_valid_word(self->priv->current_word,(gchar*)key))
	{
		prop = gsc_proposal_new((gchar*)key,
					NULL,
					self->priv->icon);
		self->priv->current_props = g_list_append(self->priv->current_props,prop);
	}
	
	/*For free the current item into the table*/
	return TRUE;
}

static GList*
get_words(GscProviderFile *self, gchar* buffer)
{

	self->priv->current_word = gsc_get_last_word_cleaned(self->priv->view);
	GHashTable *table = g_hash_table_new_full(g_str_hash,
						g_str_equal,
						g_free,
						NULL);
	gchar* ini = NULL;
	gchar* cur = buffer;
	gunichar c;
	gint count = 0;
	gint words = 0;
	while (TRUE)
	{
		ini = cur;
		count = 0;
		c = g_utf8_get_char(cur);
		while(!gsc_char_is_separator(c))
		{
			count++;
			cur = g_utf8_next_char(cur);
			c = g_utf8_get_char(cur);
		}
		if (count > 0)
		{
			g_hash_table_insert(table,g_strndup(ini,count),NULL);
			words++;
		}
		
		if (*cur == '\0')
			break;
		
		cur = g_utf8_next_char(cur);
	}
	
	self->priv->current_props = NULL;
	g_hash_table_foreach_remove(table,
				    table_foreach,
				    self);
	
	g_hash_table_unref(table);
	return g_list_first(self->priv->current_props);
	
}

static GList* 
gsc_provider_file_real_get_proposals (GscProvider* base,
				      GscTrigger *trigger)
{
	GscProviderFile *self = GSC_PROVIDER_FILE(base);
	GFileInfo *info = g_file_query_info (self->priv->file_uri,
				G_FILE_ATTRIBUTE_STANDARD_SIZE,
				G_FILE_QUERY_INFO_NONE,
				NULL,
				NULL);
	if (info == NULL)
	{
		g_warning("The file does not exist");
		return NULL;
	}
	
	guint64 size; 
	size = g_file_info_get_attribute_uint64 (info, G_FILE_ATTRIBUTE_STANDARD_SIZE);
	g_object_unref(info);
	
	gchar *buffer = NULL;
	buffer = g_malloc (size + 1);
	if (buffer == NULL && size != 0)
	{
		g_warning("This file is too big. Unable to allocate memory.");
		return NULL;
	}
	GFileInputStream *stream;
	gboolean result;
	gsize nchars;
	stream = g_file_read (self->priv->file_uri, NULL, NULL);
	if (stream == NULL)
	{
		g_warning("Could not open file");
		return NULL;
	}
	result = g_input_stream_read_all (G_INPUT_STREAM (stream), 
					buffer, size, &nchars, NULL, NULL);
	if (!result)
	{
		g_free(buffer);
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
				g_warning("The file does not look like a text file or the file encoding is not supported.");
				return NULL;
			}
			g_free (buffer);
			buffer = converted_text;
		}
	}
		
	GList *proposals = get_words(self,buffer);
	g_free (buffer);
	return proposals;
}

static void 
gsc_provider_file_real_finish (GscProvider* base)
{

}

static void 
gsc_provider_file_finalize(GObject *object)
{
	GscProviderFile *self;
	self = GSC_PROVIDER_FILE(object);
	if (self->priv->file_uri!=NULL)
		g_object_unref (self->priv->file_uri);
	self->priv->file_uri = NULL;
	gdk_pixbuf_unref (self->priv->icon);
	G_OBJECT_CLASS(gsc_provider_file_parent_class)->finalize(object);
}


static void 
gsc_provider_file_class_init (GscProviderFileClass *klass)
{
	gsc_provider_file_parent_class = g_type_class_peek_parent (klass);
	G_OBJECT_CLASS (klass)->finalize = gsc_provider_file_finalize;
}

static void
gsc_provider_file_interface_init (GscProviderIface *iface)
{
	gsc_provider_file_parent_iface = g_type_interface_peek_parent (iface);
	
	iface->get_name = gsc_provider_file_real_get_name;
	iface->get_proposals = gsc_provider_file_real_get_proposals;
	iface->finish = gsc_provider_file_real_finish;
}


static void 
gsc_provider_file_init (GscProviderFile * self)
{
	self->priv = g_new0(GscProviderFilePrivate, 1);
	self->priv->file_uri = NULL;
	self->priv->icon = gdk_pixbuf_new_from_file(ICON_FILE,NULL);
	self->priv->current_props = NULL;
	self->priv->view = NULL;
}

GType gsc_provider_file_get_type ()
{
	static GType g_define_type_id = 0;
	if (G_UNLIKELY (g_define_type_id == 0)) {
		static const GTypeInfo g_define_type_info = { sizeof (GscProviderFileClass), (GBaseInitFunc) NULL, (GBaseFinalizeFunc) NULL, (GClassInitFunc) gsc_provider_file_class_init, (GClassFinalizeFunc) NULL, NULL, sizeof (GscProviderFile), 0, (GInstanceInitFunc) gsc_provider_file_init };
		g_define_type_id = g_type_register_static (G_TYPE_OBJECT, "GscProviderFile", &g_define_type_info, 0);
		static const GInterfaceInfo gsc_provider_info = { (GInterfaceInitFunc) gsc_provider_file_interface_init, (GInterfaceFinalizeFunc) NULL, NULL};
		g_type_add_interface_static (g_define_type_id, GSC_TYPE_PROVIDER, &gsc_provider_info);
	}
	return g_define_type_id;
}

GscProviderFile*
gsc_provider_file_new(GtkTextView *view)
{
	GscProviderFile *self = GSC_PROVIDER_FILE (g_object_new (GSC_TYPE_PROVIDER_FILE, NULL));
	self->priv->view = view;
	return self;
}

void
gsc_provider_file_set_file(GscProviderFile *self,
			   const gchar* file)
{
	g_return_if_fail(file!=NULL);
	if (self->priv->file_uri!=NULL)
		g_object_unref (self->priv->file_uri);
	self->priv->file_uri = g_file_new_for_path(file);	
}

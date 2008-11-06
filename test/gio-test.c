#include <stdlib.h>
#include <string.h>
#include <glib.h>
#include <glib/gprintf.h>
#include <gtk/gtk.h>
#include <gio/gio.h>
#include "../gtksourcecompletion/gsc-utils.h"
#include "../gtksourcecompletion/gsc-proposal.h"

static gboolean
table_foreach (gpointer key,
		gpointer value,
		gpointer user_data)
{
	GscProposal* prop = NULL;
	GList **list = (GList**) user_data;
	prop = gsc_proposal_new((gchar*)key,
				NULL,
				NULL);
	*list = g_list_append(*list,prop);
	
	return TRUE;
}

static GList*
get_words(gchar* buffer)
{
	GHashTable *table = g_hash_table_new_full(g_str_hash,
						g_str_equal,
						g_free,
						NULL);
	gchar* ini = NULL;
	gchar* cur = buffer;
	gunichar c;
	g_debug("------------------------");
	
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
			GString *str = g_string_new_len(ini,count);
			//g_printf("Palabra [%s]\n", str->str);
			g_hash_table_insert(table,str->str,NULL);
			g_string_free(str,FALSE);
			words++;
		}
		
		if (*cur == '\0')
			break;
		
		cur = g_utf8_next_char(cur);
	}
	
	GList *proposals = NULL;
	g_hash_table_foreach_remove(table,
				    table_foreach,
				    &proposals);
	
	g_printf("words: %i, different words: %i\n",words,g_list_length(proposals));
	g_printf("table: %i\n",g_hash_table_size(table));
	g_hash_table_unref(table);
	return g_list_first(proposals);
	
}

int main( int argc, const char* argv[] )
{
	if (argc < 2)
	{
		g_debug("faltan parametros");
		exit(-1);
	}
	g_type_init();
	GFile *gio_uri = g_file_new_for_path(argv[1]);
	GFileInfo *info = g_file_query_info (gio_uri,
				G_FILE_ATTRIBUTE_STANDARD_SIZE,
				G_FILE_QUERY_INFO_NONE,
				NULL,
				NULL);
	
	if (info == NULL)
	{
		g_object_unref(gio_uri);
		g_debug("El fichero no existe");
		exit(-1);
	}
	guint64 size; 
	size = g_file_info_get_attribute_uint64 (info, G_FILE_ATTRIBUTE_STANDARD_SIZE);
	g_object_unref(info);
	g_debug("Tamaño del fichero: %d",size);
	
	gchar *buffer = NULL;
	buffer = g_malloc (size + 1);
	if (buffer == NULL && size != 0)
	{
		g_debug("This file is too big. Unable to allocate memory.");
		g_object_unref (gio_uri);
		exit(-1);
	}
	GFileInputStream *stream;
	gboolean result;
	gsize nchars;
	stream = g_file_read (gio_uri, NULL, NULL);
	if (stream == NULL)
	{
		g_debug("Could not open file");
		g_object_unref (gio_uri);

		exit(-1);
	}
	result = g_input_stream_read_all (G_INPUT_STREAM (stream), 
					buffer, size, &nchars, NULL, NULL);
	if (!result)
	{
		g_free(buffer);
		g_debug("Error while reading from file");
		g_object_unref (gio_uri);
		exit(-1);
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
				g_debug("The file does not look like a text file or the file encoding is not supported.");
				g_object_unref (gio_uri);
				exit(-1);
			}
			g_free (buffer);
			buffer = converted_text;
			g_debug("transformado");
			//nchars = strlen (converted_text);
		}
	}
	else
		g_debug("nchars <= 0");
		
	g_debug("buffer:\n%s",buffer);
	
	GList *proposals = get_words(buffer);
	GList *p = proposals;
	GscProposal *prop = NULL;
	do
	{
		prop = GSC_PROPOSAL(p->data);
		g_debug("Proposal: %s",gsc_proposal_get_label(prop));
		g_object_unref(prop);
	}while((p = g_list_next(p)) != NULL);
	g_list_free(proposals);
	
	g_free (buffer);	
	g_object_unref(gio_uri);
	return 0;	
	
}


/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-utils.c
 *
 *  Copyright (C) 2007 - Chuchiperriman <chuchiperriman@gmail.com>
 *
 *  This library is free software; you can redistribute it and/or
 *  modify it under the terms of the GNU Lesser General Public
 *  License as published by the Free Software Foundation; either
 *  version 2.1 of the License, or (at your option) any later version.
 *
 *  This library is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 *  Lesser General Public License for more details.

 *  You should have received a copy of the GNU Lesser General Public
 *  License along with this library; if not, write to the Free Software
 *  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 */

#include <string.h> 
#include "gsc-utils.h"

gboolean
gsc_char_is_separator(const gunichar ch)
{
	if (g_unichar_isalnum(ch) || ch == g_utf8_get_char("_"))
	{
		return FALSE;
	}
	
	return TRUE;
}

gchar*
gsc_get_last_word_and_iter(GtkTextView *text_view, 
					GtkTextIter *start_word, 
					GtkTextIter *end_word)
{
	GtkTextMark* insert_mark;
	GtkTextBuffer* text_buffer;
	GtkTextIter actual,temp;
	GtkTextIter *start_iter;
	gchar* text;
	gunichar ch;
	gboolean found, no_doc_start;
	
	if (start_word != NULL)
	{
		start_iter = start_word;
	}
	else
	{
		start_iter = &temp;
	}
	
	text_buffer = gtk_text_view_get_buffer(text_view);
	insert_mark = gtk_text_buffer_get_insert(text_buffer);
	gtk_text_buffer_get_iter_at_mark(text_buffer,&actual,insert_mark);
	
	*start_iter = actual;
	if (end_word!=NULL)
	{
		*end_word = actual;
	}
	
	found = FALSE;
	while ((no_doc_start = gtk_text_iter_backward_char(start_iter)) == TRUE)
	{
		ch = gtk_text_iter_get_char(start_iter);
		//TODO Do better
		if (gsc_char_is_separator(ch))
		{
			found = TRUE;
			break;
		}
	}
	
	if (!no_doc_start)
	{
		gtk_text_buffer_get_start_iter(text_buffer,start_iter);
		text = gtk_text_iter_get_text (start_iter, &actual);
	}
	else
	{
	
		if (found)
		{
			gtk_text_iter_forward_char(start_iter);
			text = gtk_text_iter_get_text (start_iter, &actual);
		}
		else
		{
			*start_iter = actual;
			text = "";
		}
	}
	
	return text;
}

gchar*
gsc_get_last_word(GtkTextView *text_view)
{
	return gsc_get_last_word_and_iter(text_view, NULL, NULL);
}

void
gsc_get_cursor_pos(GtkTextView *text_view, 
				gint *x, 
				gint *y)
{
	GdkWindow *win;
	GtkTextMark* insert_mark;
	GtkTextBuffer* text_buffer;
	GtkTextIter start;
	GdkRectangle location;
	gint win_x, win_y;
	gint xx, yy;

	text_buffer = gtk_text_view_get_buffer(text_view);
	insert_mark = gtk_text_buffer_get_insert(text_buffer);
	gtk_text_buffer_get_iter_at_mark(text_buffer,&start,insert_mark);
	gtk_text_view_get_iter_location(text_view,
														&start,
														&location );
	gtk_text_view_buffer_to_window_coords (text_view,
						GTK_TEXT_WINDOW_WIDGET,
						location.x, 
						location.y,
						&win_x, 
						&win_y);

	win = gtk_text_view_get_window (text_view, 
	                                GTK_TEXT_WINDOW_WIDGET);
	gdk_window_get_origin (win, &xx, &yy);
	
	*x = win_x + xx;
	*y = win_y + yy + location.height;
}

gchar*
gsc_get_text(GtkTextView *text_view)
{
	GtkTextIter start, end;
	GtkTextBuffer *buffer;
	
	buffer = gtk_text_view_get_buffer(text_view);
	gtk_text_buffer_get_start_iter (buffer, &start);
	gtk_text_buffer_get_end_iter (buffer, &end);
	return gtk_text_buffer_get_text(buffer,&start,&end,FALSE);
	
}

void
gsc_replace_actual_word(GtkTextView *text_view, 
				    const gchar* text)
{
	GtkTextBuffer *buffer;
	GtkTextIter word_start, word_end;
	
	buffer = gtk_text_view_get_buffer(text_view);
	gtk_text_buffer_begin_user_action(buffer);
	
	gsc_get_last_word_and_iter(text_view,&word_start, &word_end);

	GtkTextMark *mark = gtk_text_buffer_create_mark(buffer,"temp_replace",&word_start, TRUE);
	gtk_text_buffer_delete(buffer,&word_start,&word_end);
	gtk_text_buffer_get_iter_at_mark(buffer,&word_start,mark);
	gtk_text_buffer_insert(buffer, &word_start, text,-1);
	gtk_text_buffer_delete_mark(buffer,mark);
	gtk_text_buffer_end_user_action(buffer);
}

gchar*
gsc_clear_word(const gchar* word)
{
	int len = g_utf8_strlen(word,-1);
	int i;
	const gchar *temp = word;
	
	for (i=0;i<len;i++)
	{
		if (gsc_char_is_separator(g_utf8_get_char(temp)))
			temp = g_utf8_next_char(temp);
		else
			return g_strdup(temp);
		
	}
	return NULL;
}

gchar *
gsc_compute_line_indentation (GtkTextView *view,
			     GtkTextIter *cur)
{
	GtkTextIter start;
	GtkTextIter end;

	gunichar ch;
	gint line;

	line = gtk_text_iter_get_line (cur);

	gtk_text_buffer_get_iter_at_line (gtk_text_view_get_buffer (view),
					  &start,
					  line);

	end = start;

	ch = gtk_text_iter_get_char (&end);

	while (g_unichar_isspace (ch) &&
	       (ch != '\n') &&
	       (ch != '\r') &&
	       (gtk_text_iter_compare (&end, cur) < 0))
	{
		if (!gtk_text_iter_forward_char (&end))
			break;

		ch = gtk_text_iter_get_char (&end);
	}

	if (gtk_text_iter_equal (&start, &end))
		return NULL;

	return gtk_text_iter_get_slice (&start, &end);
}

gchar*
gsc_get_text_with_indent(const gchar* content,gchar *indent)
{
	GString *fin = NULL;
	gchar *ret = NULL;
	gint len = strlen(content);
	gint i;
	gint last_line = 0;
	for (i=0;i < len;i++)
	{
		if (content[i] == '\n' || content[i] =='\r')
		{
			if (fin==NULL)
				fin = g_string_new_len(content,i+1);
			else
			{
				fin = g_string_append_len(fin,&content[last_line+1],i - last_line);
			}
			fin = g_string_append(fin,indent);
			last_line = i;
		}
	}
	if (fin==NULL)
		ret = g_strdup(content);
	else
	{
		if (last_line < len -1)
		{
			fin = g_string_append_len(fin,&content[last_line+1],len - last_line);
		}
		ret = g_string_free(fin,FALSE);
	}
	return ret;
}


void
gsc_insert_text_with_indent(GtkTextView *view, const gchar* text)
{
	GtkTextBuffer * buffer = gtk_text_view_get_buffer(view);
	GtkTextMark *insert = gtk_text_buffer_get_insert(buffer);
	GtkTextIter cur;
	gtk_text_buffer_get_iter_at_mark(buffer,&cur,insert);
	gchar *indent = gsc_compute_line_indentation(view,&cur);
	gchar *indent_text = gsc_get_text_with_indent(text, indent);
	g_free(indent);
	gtk_text_buffer_insert_at_cursor(buffer,indent_text,-1);
	g_free(indent_text);
	gtk_text_view_scroll_mark_onscreen(view,insert);
}


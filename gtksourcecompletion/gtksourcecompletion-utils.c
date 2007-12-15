/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-utils.c
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
 
#include "gtksourcecompletion-utils.h"

/**
* gtk_source_view_get_last_word_and_iter:
* @text_view: The #GtkTextView
* @start_word: if != NULL then assign it the start position of the word
* @end_word: if != NULL then assing it the end position of the word
* 
* Returns: the last word written in the #GtkTextView or ""
*
**/
gchar*
gtk_source_view_get_last_word_and_iter(GtkTextView *text_view, 
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
		if (ch == ' ' || ch == '\n' || ch == '\r' || ch == '\t'
			|| ch == ',' || ch == '.' || ch == '(' || ch == ')'
			|| ch == '{' || ch == '}' || ch == ';' || ch == ':')
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
 
/**
 * gtk_source_view_get_last_word:
 * @text_view: The #GtkTextView
 *
 * Returns: the last word written in the #GtkTextView or ""
 */
gchar*
gtk_source_view_get_last_word(GtkTextView *text_view)
{
	return gtk_source_view_get_last_word_and_iter(text_view, NULL, NULL);
}

/** 
 * gtk_source_view_get_cursor_pos:
 * @text_view: The #GtkTextView
 * @x: Assign the x position of the cursor
 * @y: Assign the y position of the cursor
 *
 * Gets the cursor position on the screen.
 */
void
gtk_source_view_get_cursor_pos(GtkTextView *text_view, 
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
                                        location.x, location.y,
                                        &win_x, &win_y);

	win = gtk_text_view_get_window (text_view, 
                                GTK_TEXT_WINDOW_WIDGET);
	gdk_window_get_origin (win, &xx, &yy);
	
	*x = win_x + xx;
	*y = win_y + yy + location.height;
}

/**
 * gtc_gsv_get_text: 
 * @text_view: The #GtkTextView 
 *
 * Returns the #GtkTextView content .
 */
gchar*
gtc_gsv_get_text(GtkTextView *text_view)
{
	GtkTextIter start, end;
	GtkTextBuffer *buffer;
	
	buffer = gtk_text_view_get_buffer(text_view);
	gtk_text_buffer_get_start_iter (buffer, &start);
	gtk_text_buffer_get_end_iter (buffer, &end);
	return gtk_text_buffer_get_text(buffer,&start,&end,FALSE);
	
}

/**
 * gtk_source_view_replace_actual_word:
 * @text_view: The #GtkTextView
 * @text: The text to be inserted instead of the current word
 * 
 * Replaces the current word in the #GtkTextView with the new word
 *
 */
void
gtk_source_view_replace_actual_word(GtkTextView *text_view, 
					const gchar* text)
{
	GtkTextBuffer *buffer;
	GtkTextIter word_start, word_end;
	
	buffer = gtk_text_view_get_buffer(text_view);
	gtk_text_buffer_begin_user_action(buffer);
	
	gtk_source_view_get_last_word_and_iter(text_view,&word_start, &word_end);
									   
	gtk_text_buffer_delete(buffer,&word_start,&word_end);
	gtk_text_buffer_insert(buffer, &word_start, text,-1);
	
	gtk_text_buffer_end_user_action(buffer);
}

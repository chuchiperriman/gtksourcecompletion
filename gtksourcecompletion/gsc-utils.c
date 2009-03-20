/*
 * gsc-utils.c
 * This file is part of gtksourcecompletion
 *
 * Copyright (C) 2007 -2009 Jesús Barbero Rodríguez <chuchiperriman@gmail.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, 
 * Boston, MA 02111-1307, USA.
 */

/**
 * SECTION:gsc-utils
 * @title: Gsc Utils
 * @short_description: Useful functions
 *
 */
 
#include <string.h> 
#include "gsc-utils.h"

gboolean
gsc_utils_char_is_separator(const gunichar ch)
{
	if (g_unichar_isprint(ch) && 
	    (g_unichar_isalnum(ch) || ch == g_utf8_get_char("_")))
	{
		return FALSE;
	}
	
	return TRUE;
}

gchar*
gsc_utils_view_get_last_word_and_iter(GtkTextView *text_view, 
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
		if (gsc_utils_char_is_separator(ch))
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
			/*FIXME dup this var?*/
			text = "";
		}
	}
	
	return text;
}

gchar*
gsc_utils_view_get_last_word(GtkTextView *text_view)
{
	return gsc_utils_view_get_last_word_and_iter(text_view, NULL, NULL);
}

void
gsc_utils_view_get_cursor_pos(GtkTextView *text_view, 
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

void
gsc_utils_view_replace_current_word(GtkTextView *text_view, 
				    const gchar* text)
{
	GtkTextBuffer *buffer;
	GtkTextIter word_start, word_end;
	
	buffer = gtk_text_view_get_buffer(text_view);
	gtk_text_buffer_begin_user_action(buffer);
	
	gsc_utils_view_get_last_word_and_iter(text_view,&word_start, &word_end);

	GtkTextMark *mark = gtk_text_buffer_create_mark(buffer,
							"temp_replace",
							&word_start,
							TRUE);
	gtk_text_buffer_delete(buffer,&word_start,&word_end);
	gtk_text_buffer_get_iter_at_mark(buffer,&word_start,mark);
	gtk_text_buffer_insert(buffer, &word_start, text,-1);
	gtk_text_buffer_delete_mark(buffer,mark);
	gtk_text_buffer_end_user_action(buffer);
}

gboolean 
gsc_utils_window_get_position_at_cursor(GtkWindow *window,
					GtkTextView *view,
					gint *x,
					gint *y,
					gboolean *resized)
{
	gint w, h, xtext, ytext, ytemp;
	gint sw = gdk_screen_width();
	gint sh = gdk_screen_height();
	gboolean resize = FALSE;
	gboolean up = FALSE;
	gsc_utils_view_get_cursor_pos(view,x,y);
	
	gtk_window_get_size(window, &w, &h);
	
	/* Processing x position and width */
	if (w > (sw - 8))
	{
		/* Resize to view all the window */
		resize = TRUE;
		w = sw -8;
	}
	
	/* Move position to view all the window */
	if ((*x + w) > (sw - 4))
	{
		*x = sw - w -4;
	}

	/* Processing y position and height */
	
	/* 
	If we cannot show it down, we show it up and if we cannot show it up, we
	show the window at the largest position 
	*/
	if ((*y + h) > sh)
	{
		PangoLayout* layout = 
			gtk_widget_create_pango_layout(GTK_WIDGET(view), NULL);
		pango_layout_get_pixel_size(layout,&xtext,&ytext);
		ytemp = *y - ytext;
		/* Cabe arriba? */
		if ((ytemp - h) >= 4)
		{
			*y = ytemp - h;
			up = TRUE;
		}
		else
		{
			/* 
			Si no cabe arriba, lo ponemos donde haya más espacio
			y redimensionamos la ventana
			*/
			if ((sh - *y) > ytemp)
			{
				h = sh - *y - 4;
			}
			else
			{
				*y = 4;
				h = ytemp -4;
				up = TRUE;
			}
			resize = TRUE;
		}
		g_object_unref(layout);
	}
	
	if (resize)
		gtk_window_resize(window, w, h);

	if (resized != NULL)
		*resized = resize;
	
	return up;
}




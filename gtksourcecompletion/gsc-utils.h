/*
 * gsc-utils.h
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
 
#ifndef GTK_SNIPPETS_GSV_UTILS_H
#define GTK_SNIPPETS_GSV_UTILS_H

#include <gtk/gtk.h>

/**
 * gsc_utils_char_is_separator:
 * @ch: The character to check
 *
 * A separator is a character like (, an space etc. An _ is not a separator
 *
 * Returns TRUE if the ch is a separator
 */
gboolean
gsc_utils_char_is_separator(gunichar ch);

/**
* gsc_utils_view_get_last_word_and_iter:
* @text_view: The #GtkTextView
* @start_word: if != NULL then assign it the start position of the word
* @end_word: if != NULL then assing it the end position of the word
* 
* Returns: the last word written in the #GtkTextView or ""
*
**/
gchar*
gsc_utils_view_get_last_word_and_iter(GtkTextView *text_view, 
				      GtkTextIter *start_word, 
				      GtkTextIter *end_word);

/**
 * gsc_utils_view_get_last_word:
 * @text_view: The #GtkTextView
 *
 * Returns: the last word written in the #GtkTextView or ""
 */
gchar*
gsc_utils_view_get_last_word(GtkTextView *text_view);

/** 
 * gsc_utils_view_get_cursor_pos:
 * @text_view: The #GtkTextView
 * @x: Assign the x position of the cursor
 * @y: Assign the y position of the cursor
 *
 * Gets the cursor position on the screen.
 */
void
gsc_utils_view_get_cursor_pos(GtkTextView *text_view, 
			      gint *x, 
			      gint *y);

/**
 * gsc_utils_view_replace_current_word:
 * @text_view: The #GtkTextView
 * @text: The text to be inserted instead of the current word
 * 
 * Replaces the current word in the #GtkTextView with the new word
 *
 */
void
gsc_utils_view_replace_current_word(GtkTextView *text_view, 
				    const gchar* text);

/**
 * gsc_utils_window_get_position_at_cursor:
 * @window: Window to set
 * @view: Parent view where we get the cursor position
 * @x: The returned x position
 * @y: The returned y position
 *
 * Returns: TRUE if the position is over the text and FALSE if 
 * the position is under the text.
 */
gboolean 
gsc_utils_window_get_position_at_cursor(GtkWindow *window,
					GtkTextView *view,
					gint *x,
					gint *y,
					gboolean *resized);

#endif 

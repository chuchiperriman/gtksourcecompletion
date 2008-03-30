/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-utils.h
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

#ifndef GTK_SNIPPETS_GSV_UTILS_H
#define GTK_SNIPPETS_GSV_UTILS_H

#include <gtk/gtk.h>

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
					GtkTextIter *end_word);

/**
 * gtk_source_view_get_last_word:
 * @text_view: The #GtkTextView
 *
 * Returns: the last word written in the #GtkTextView or ""
 */
gchar*
gtk_source_view_get_last_word(GtkTextView *text_view);

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
					gint *y);

/**
 * gtc_gsv_get_text: 
 * @text_view: The #GtkTextView 
 *
 * Returns the #GtkTextView content .
 */
gchar* 
gtc_gsv_get_text(GtkTextView *text_view);

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
					const gchar* text);

/**
 * gsc_char_is_separator:
 * @ch: The character to check
 *
 * A separator is a character like (, an space etc. An _ is not a separator
 *
 * Returns TRUE if the ch is a separator
 */
gboolean
gsc_char_is_separator(gunichar ch);

/**
 * gsc_clear_word:
 * @word: The word to be cleaned
 * 
 * Clean the word eliminates the special characters at the start of this word.
 * By example "$variable" is cleaned to "variable"
 *
 * Returns New allocated string with the word cleaned. If all characters are 
 * separators, it return NULL;
 *
 */
gchar*
gsc_clear_word(const gchar* word);

#endif 

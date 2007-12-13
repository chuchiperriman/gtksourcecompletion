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

gchar*
gtk_source_view_get_last_word_and_iter(GtkTextView *text_view, 
					GtkTextIter *start_word, 
					GtkTextIter *end_word);

gchar*
gtk_source_view_get_last_word(GtkTextView *text_view);

void
gtk_source_view_get_cursor_pos(GtkTextView *text_view, 
					gint *x, 
					gint *y);

gchar* 
gtc_gsv_get_text(GtkTextView *text_view);

void
gtk_source_view_replace_actual_word(GtkTextView *text_view, 
					const gchar* text);

#endif 

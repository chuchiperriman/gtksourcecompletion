/*
 * gsc-utils.h
 * This file is part of gsc
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
 
#ifndef __GSC_COMPLETION_UTILS_H__
#define __GSC_COMPLETION_UTILS_H__

#include <gtk/gtk.h>

G_BEGIN_DECLS

void		 gsc_utils_get_iter_at_insert		(GtkTextView	*view,
							 GtkTextIter    *iter);

gchar	     	*gsc_utils_clear_word			(const gchar* word);

gboolean	 gsc_utils_is_separator			(gunichar         ch);

gchar		*gsc_utils_get_word_iter		(GtkTextBuffer *text_buffer, 
									 GtkTextIter     *current,
									 GtkTextIter     *start_word, 
									 GtkTextIter     *end_word);

gchar		*gsc_utils_get_word			(GtkTextBuffer *text_view);

void		 gsc_utils_replace_word		(GtkTextBuffer *text_buffer,
									 GtkTextIter     *iter,
									 const gchar     *text,
									 gint             len);

void		 gsc_utils_replace_current_word	(GtkTextBuffer *text_buffer, 
									 const gchar     *text,
									 gint             len);

void		 gsc_utils_move_to_iter		(GtkWindow       *window,
									 GtkTextView *view,
									 GtkTextIter     *iter);

void		 gsc_utils_move_to_cursor		(GtkWindow       *window,
									 GtkTextView   *view);

G_END_DECLS

#endif /* __GSC_ITEM_H__ */

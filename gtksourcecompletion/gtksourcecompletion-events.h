/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksource-completion-completion.h
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
 
#ifndef _GTK_SOURCE_COMPLETION_EVENTS_H_
#define _GTK_SOURCE_COMPLETION_EVENTS_H_

#include <glib.h>
#include <gtksourceview/gtksourceview.h>

#include "gtksourcecompletion.h"

G_BEGIN_DECLS

/***************** User Request Event *****************/

#define USER_REQUEST_EVENT_NAME "UserRequestEvent"

void
gtk_source_completion_active_user_request_event(GtkSourceCompletion *comp);

void
gtk_source_completion_user_request_event_keys(GtkSourceCompletion *comp, const gchar* keys);

/******************************************************/

/***************** Autocompletion Event *****************/

#define AUTOCOMPLETION_EVENT_NAME "AutocompletionEvent"

typedef struct _AutoCompletionEvent AutoCompletionEvent;

void
gtk_source_completion_autocompletion_event_enable(GtkSourceCompletion *comp);

void 
gtk_source_completion_autocompletion_event_disable(GtkSourceCompletion *comp);

void 
gtk_source_completion_autocompletion_event_delay(GtkSourceCompletion *comp,guint delay);

const gchar*
gtk_source_completion_autocompletion_get_word(AutoCompletionEvent *event);
/******************************************************/


G_END_DECLS

#endif /* _GTK_SOURCE_COMPLETION_EVENTS_H_ */

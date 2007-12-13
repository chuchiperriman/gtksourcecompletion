/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gtksourcecompletion-events.h
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
#include <unistd.h>
#include "gtksourcecompletion-events.h"
#include "gtksourcecompletion-utils.h"


enum EC_EVENTS{
	EC_UR_EVENT,
	EC_AC_EVENT,
	EC_LAST_EVENT
};

static gpointer
event_control_get_event(
								GtkTextView *view, 
								enum EC_EVENTS ec_event);

static gboolean
event_control_exist_event(
								GtkTextView *view, 
								enum EC_EVENTS ec_event);

static void
event_control_remove_event(
								GtkTextView *view,
								enum EC_EVENTS ec_event);

static gboolean
event_control_add_event(
								GtkTextView *view,
								gpointer event,
								enum EC_EVENTS ec_event);

/***************** User Request Event *****************/

/* User request signals */
enum
{
	URS_GTK_TEXT_VIEW_KP,
	URS_LAST_SIGNAL
};

typedef struct _UserRequestEvent UserRequestEvent;

struct _UserRequestEvent
{
	gulong signals[URS_LAST_SIGNAL];
	GtkSourceCompletion *completion;
	GtkTextView *view;
};

static gboolean
user_request_view_key_press_event_cb(GtkWidget *view,
					GdkEventKey *event, 
					gpointer user_data)
{
	/*UserRequestEvent *ur_event = (UserRequestEvent*)user_data;*/
	UserRequestEvent *ur_event = (UserRequestEvent*)user_data;
	GtkSourceCompletion* completion = ur_event->completion;
		
	if (completion != NULL)
	{
		
		if ((event->state & GDK_CONTROL_MASK) && event->keyval == GDK_Return)
		{
			gtk_source_completion_raise_event(completion,USER_REQUEST_EVENT_NAME,NULL);
			return TRUE;
		}
		
	}
	
	return FALSE;
	
} 

static void
ur_weak_ref_completion(gpointer event, GObject completion)
{
	UserRequestEvent *ur_event = (UserRequestEvent*)event;
	/*
	 * If we disconnect the key-press-event signal the g_signal_connect_data 
	 * frees UserRequestEvent
	 */
	event_control_remove_event(ur_event->view, EC_UR_EVENT);
	g_signal_handler_disconnect(ur_event->view,ur_event->signals[URS_GTK_TEXT_VIEW_KP]);
}

void
gtk_source_completion_active_user_request_event(GtkSourceCompletion *comp)
{
	GtkTextView *view = gtk_source_completion_get_view(comp);
	if (event_control_exist_event(GTK_TEXT_VIEW(view),EC_UR_EVENT))
	{
		return;
	}
	UserRequestEvent *event = g_malloc0(sizeof(UserRequestEvent));
	g_assert(GTK_IS_TEXT_VIEW(view));
	event->signals[URS_GTK_TEXT_VIEW_KP] =  
			g_signal_connect_data(view,
						"key-press-event",
						G_CALLBACK(user_request_view_key_press_event_cb),
						(gpointer) event,
						(GClosureNotify)g_free,
						0);
	event->completion = comp;
	event->view = view;
	g_object_weak_ref(G_OBJECT(comp),(GWeakNotify)ur_weak_ref_completion,event);
	event_control_add_event(view,event,EC_UR_EVENT);
}

/******************************************************/

/***************** Autocompletion Event *****************/

#define MIN_LEN 3
#define AUTOCOMPLETION_DELAY 500

/* Autocompletion signals */
enum
{
	AS_GTK_TEXT_VIEW_KR,
	AS_GTK_TEXT_VIEW_KP,
	AS_LAST_SIGNAL
};

struct _AutoCompletionEvent
{
	gulong signals[AS_LAST_SIGNAL];
	gchar *actual_word;
	GtkSourceCompletion* completion;
	GtkTextView *view;
	guint source_id;
	guint delay;
};

static gboolean
autocompletion_raise_event(
								gpointer event);


static gboolean
autocompletion_key_press_cb (GtkWidget *view,
					GdkEventKey *event, 
					gpointer user_data)
{
	AutoCompletionEvent *ac_event = (AutoCompletionEvent*)user_data;
	if (ac_event->source_id!=0)
	{
			/* Stop the event because the user is written very fast*/
			g_source_remove(ac_event->source_id);
			ac_event->source_id = 0;
	}
	return FALSE;
}

static gboolean
autocompletion_key_release_cb (GtkWidget *view,
					GdkEventKey *event, 
					gpointer user_data)
{
	gchar* word;
	gboolean res = FALSE;
	guint keyval = event->keyval;
	GtkSourceView *source_view = GTK_SOURCE_VIEW(view);
	AutoCompletionEvent *ac_event = (AutoCompletionEvent*)user_data;
	GtkSourceCompletion *completion = ac_event->completion;
	if (completion != NULL)
	{
		/*
		* If not is a character key do nothing
		* TODO There are more characters like (,[]{} etc.
		*/ 
		if  ((!(event->state & GDK_CONTROL_MASK))
			&& ( (GDK_A <= keyval && keyval <= GDK_Z)
			|| (GDK_a <= keyval && keyval <= GDK_z)
			|| (GDK_0 <= keyval && keyval <= GDK_9)		
			|| GDK_underscore == keyval))
		{
			/*raise event in 0,5 seconds*/
			if (ac_event->source_id==0)
			{
				ac_event->source_id = g_timeout_add(ac_event->delay,autocompletion_raise_event,ac_event);
			}
		}
		else if (GDK_BackSpace == keyval)
		{
			/*TODO Only do this if completion is visible*/
			if (gtk_source_completion_is_visible(completion))
			{
				word = gtk_source_view_get_last_word_and_iter(GTK_TEXT_VIEW(source_view), NULL, NULL);
				if((strlen(word) >= MIN_LEN))
				{
					g_free(ac_event->actual_word);	
					ac_event->actual_word = word;
					gtk_source_completion_raise_event(completion,
										AUTOCOMPLETION_EVENT_NAME,
										ac_event);
				}
			}
		}
	}

	return res;
}

static gboolean
autocompletion_raise_event(
								gpointer event)
{
	gchar* word;
	AutoCompletionEvent *ac_event = (AutoCompletionEvent*)event;
	GtkSourceCompletion *completion = ac_event->completion;
	GtkSourceView *source_view = GTK_SOURCE_VIEW(ac_event->view);
	word = gtk_source_view_get_last_word_and_iter(GTK_TEXT_VIEW(source_view), NULL, NULL);
	if (strlen(word) >= MIN_LEN)
	{
		g_free(ac_event->actual_word);
		ac_event->actual_word = word;
		gtk_source_completion_raise_event(completion,
							AUTOCOMPLETION_EVENT_NAME,
							ac_event);
	}
	return FALSE;
}

static void
autocompletion_event_free(gpointer event)
{
	g_debug("Autocompletion event free");
	AutoCompletionEvent *ac_event = (AutoCompletionEvent*)event;
	g_free(ac_event->actual_word);
	g_free(ac_event);
	
}

static void
ac_weak_ref_completion(gpointer event, GObject completion)
{
	g_debug("autocompletion weak");
	AutoCompletionEvent *ac_event = (AutoCompletionEvent*)event;
	/*
	 * The object who has called this funcion, call to autocompletion_free 
	 */
	event_control_remove_event(ac_event->view,EC_AC_EVENT);
	g_signal_handler_disconnect(ac_event->view,ac_event->signals[AS_GTK_TEXT_VIEW_KP]);
	g_signal_handler_disconnect(ac_event->view,ac_event->signals[AS_GTK_TEXT_VIEW_KR]);
}

void
gtk_source_completion_autocompletion_event_enable(GtkSourceCompletion *comp)
{

	GtkTextView *view = gtk_source_completion_get_view(comp);
	if (event_control_exist_event(GTK_TEXT_VIEW(view),EC_AC_EVENT))
	{
		g_debug("Autocompletion exists");
		return;
	}
	AutoCompletionEvent *event = g_malloc0(sizeof(AutoCompletionEvent));
	g_assert(GTK_IS_TEXT_VIEW(view));
	event->actual_word = NULL;
	event->delay = AUTOCOMPLETION_DELAY;
	event->completion = comp;
	event->view = view;
	event->signals[AS_GTK_TEXT_VIEW_KR] = 
			g_signal_connect_data(view,
						"key-release-event",
						G_CALLBACK(autocompletion_key_release_cb),
						(gpointer) event,
						(GClosureNotify)autocompletion_event_free,
						G_CONNECT_AFTER);
	event->signals[AS_GTK_TEXT_VIEW_KP] = 
			g_signal_connect(view,
						"key-press-event",
						G_CALLBACK(autocompletion_key_press_cb),
						(gpointer) event);
	event->source_id = 0;
	g_object_weak_ref(G_OBJECT(comp),(GWeakNotify)ac_weak_ref_completion,event);
	event_control_add_event(view,event,EC_AC_EVENT);
}

void 
gtk_source_completion_autocompletion_event_disable(GtkSourceCompletion *comp)
{
	GtkTextView *view = gtk_source_completion_get_view(comp);
	AutoCompletionEvent *event = event_control_get_event(view,EC_AC_EVENT);
	if (event!=NULL)
	{
		/*
		 * If we disconnect the key-press-event signal the g_signal_connect_data 
		 * frees AutoCompletionEvent 
		 */
		/*Disconect weak reference because key-press-event frees the event*/
		g_object_weak_unref(G_OBJECT(comp),(GWeakNotify)ac_weak_ref_completion,event);
		g_signal_handler_disconnect(event->view,event->signals[AS_GTK_TEXT_VIEW_KP]);
		g_signal_handler_disconnect(event->view,event->signals[AS_GTK_TEXT_VIEW_KR]);
		event_control_remove_event(event->view,EC_AC_EVENT);
		g_debug("asdfas");

	}
}

void 
gtk_source_completion_autocompletion_event_delay(GtkSourceCompletion *comp,guint delay)
{
	GtkTextView *view = gtk_source_completion_get_view(comp);
	AutoCompletionEvent *event = event_control_get_event(view,EC_AC_EVENT);
	if (event!=NULL)
	{
		event->delay = delay;
	}
}

const gchar*
gtk_source_completion_autocompletion_get_word(AutoCompletionEvent *event)
{
	return event->actual_word;
}
/******************************************************/


/***************** GtkTextView - events control ****************/

/*
 * We save a static hashtable with the GtkTextView and the active events in them.
 * With this information we can check if a GtkTextView has an event an then we
 * don't connect the same signal twice.
 */

static GHashTable *view_list = NULL;


struct _EventsInfo
{
	GtkTextView *view;
	gpointer events[EC_LAST_EVENT];
	AutoCompletionEvent *ac_event;
	UserRequestEvent *ur_event;
};

typedef struct _EventsInfo EventsInfo;

static EventsInfo*
event_info_new()
{
	int i;
	EventsInfo *ei = g_malloc0(sizeof(EventsInfo));
	ei->view = NULL;
	for(i=0;i<EC_LAST_EVENT;i++)
	{
		ei->events[i] = NULL;
	}
	return ei;
}

static gpointer
event_control_get_event(
								GtkTextView *view, 
								enum EC_EVENTS ec_event)
{
	gpointer res = NULL;
	
	if (view_list!=NULL)
	{
		EventsInfo *ei;
		ei = (EventsInfo*)g_hash_table_lookup(view_list,view);
		if (ei != NULL)
		{
			res = ei->events[ec_event];
		}
	}

	return res;

}

static gboolean
event_control_exist_event(
								GtkTextView *view, 
								enum EC_EVENTS ec_event)
{
	return event_control_get_event(view,ec_event) != NULL;
}

/*
 * Returns TRUE if add the event or FALSE if not because the 
 * event exists in the same view
 */
static gboolean
event_control_add_event(
								GtkTextView *view,
								gpointer event,
								enum EC_EVENTS ec_event)
{
	gboolean res = FALSE;

	if (view_list==NULL)
	{
		view_list = g_hash_table_new(g_direct_hash,g_direct_equal);
	}

	EventsInfo *ei;
	ei = (EventsInfo*)g_hash_table_lookup(view_list,view);
	if (ei == NULL)
	{
		ei = event_info_new();
		g_hash_table_insert(view_list,view,ei);
	}
	if (ei->events[ec_event] == NULL)
	{
		ei->events[ec_event] = event;
		res = TRUE;
	}

	return res;

}


static void
event_control_remove_event(
								GtkTextView *view,
								enum EC_EVENTS ec_event)
{
	g_assert(view_list!=NULL);
	EventsInfo *ei;
	ei = (EventsInfo*)g_hash_table_lookup(view_list,view);
	g_assert(ei!=NULL);
	g_assert(ei->events[ec_event]!=NULL);
	ei->events[ec_event] = NULL;

}

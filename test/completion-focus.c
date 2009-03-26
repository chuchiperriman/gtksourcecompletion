/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*- */
/*
 * main.c
 * Copyright (C) perriman 2007 <chuchiperriman@gmail.com>
 * 
 * main.c is free software.
 * 
 * You may redistribute it and/or modify it under the terms of the
 * GNU General Public License, as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 * 
 * main.c is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with main.c.  If not, write to:
 * 	The Free Software Foundation, Inc.,
 * 	51 Franklin Street, Fifth Floor
 * 	Boston, MA  02110-1301, USA.
 */

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <config.h>

#include <gtk/gtk.h>
#include <gdk/gdkkeysyms.h>
#include <gtksourceview/gtksourceview.h>
#include <gtksourcecompletion/gtksourcecompletion.h>
#include <gtksourcecompletion/gtksourcecompletiontrigger-customkey.h>
#include <gtksourcecompletion/gtksourcecompletiontrigger-autowords.h>
#include <gtksourcecompletion/gtksourcecompletioninfo.h>
#include "gtksourcecompletionutils.h"

#include "gsc-documentwords-provider.h"
#include "gsc-provider-file.h"
#include "gsc-provider-test.h"


static GtkWidget *view;
static GtkSourceCompletion *comp;
static GtkSourceCompletionInfo *info;
static GtkWidget *custom = NULL;
static GtkWidget *entry = NULL;

static const gboolean change_keys = FALSE;


static gboolean
filter_func (GtkSourceCompletionProposal *proposal,
	     gpointer user_data)
{
	const gchar *label = gtk_source_completion_proposal_get_label (proposal);
	return g_str_has_prefix (label, "sp");
}

static void
destroy_cb(GtkObject *object,gpointer   user_data)
{
	gtk_main_quit ();
}

static void
send_focus_change (GtkWidget *widget,
		   gboolean   in)
{
	GdkEvent *fevent = gdk_event_new (GDK_FOCUS_CHANGE);
 
	g_object_ref (widget);
 
	if (in)
		GTK_WIDGET_SET_FLAGS (widget, GTK_HAS_FOCUS);
	else
		GTK_WIDGET_UNSET_FLAGS (widget, GTK_HAS_FOCUS);
 
	fevent->focus_change.type = GDK_FOCUS_CHANGE;
	fevent->focus_change.window = g_object_ref (widget->window);
	fevent->focus_change.in = in;
 
	gtk_widget_event (widget, fevent);
 
	g_object_notify (G_OBJECT (widget), "has-focus");
 
	g_object_unref (widget);
	gdk_event_free (fevent);
}

static void
focus_popup_widget (GtkWidget *win, GtkWidget *view)
{
	GtkWidget          *toplevel;
	toplevel = gtk_widget_get_toplevel (GTK_WIDGET (view));

	/*
	 * Group the window to control the click on the parent
	 */
        if (GTK_WINDOW (toplevel)->group)
                gtk_window_group_add_window (GTK_WINDOW (toplevel)->group,
                                             GTK_WINDOW (win));
        else if (GTK_WINDOW (win)->group)
                gtk_window_group_remove_window (GTK_WINDOW (win)->group,
                                                GTK_WINDOW (win));

	gtk_window_set_modal(GTK_WINDOW (win), TRUE);
	
	gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (view), FALSE);
	gtk_widget_realize (win);
	
	gtk_window_present_with_time (GTK_WINDOW (win), GDK_CURRENT_TIME);
	gtk_window_activate_focus (GTK_WINDOW (win));
	gtk_widget_grab_focus (GTK_WIDGET (win));
	
	send_focus_change (win, TRUE);
}

static void 
unfocus_popup_widget (GtkWindow *popup, GtkWidget *view)
{
	gtk_window_set_modal(GTK_WINDOW (popup), FALSE);
	send_focus_change (GTK_WIDGET (popup), FALSE);
	gtk_text_view_set_cursor_visible (GTK_TEXT_VIEW (view), TRUE);
	send_focus_change (view, TRUE);
}

static gint
focus_out_event_cb (GtkWidget *widget, GdkEventFocus *event, gpointer useless)
{	
	gtk_widget_queue_draw (widget);

	if (GTK_WIDGET_VISIBLE(GTK_WIDGET(info)))
	{
		unfocus_popup_widget (GTK_WINDOW (info), view);
		gtk_widget_hide (GTK_WIDGET (info));
	}
	 
	return FALSE;
}

static gboolean
entry_key_press_event_cb (GtkWidget *widget,
			  GdkEventKey *event,
			  GtkWidget *view)
{
	if (event->keyval == GDK_Escape)
	{
		unfocus_popup_widget (GTK_WINDOW (info), view);
	}
	return FALSE;
}

static gboolean
button_press_event_cb (GtkWidget *widget,
		       GdkEventButton *event,
		       gpointer user_data)
{
	g_debug ("info button");
	
	/*
	 * With the info window grouped, this signal is emited when press on 
	 * the parent window (the main window)
	 */
	GtkWidget          *toplevel;
	toplevel = gtk_widget_get_toplevel (GTK_WIDGET (info));
	
	unfocus_popup_widget (GTK_WINDOW (toplevel), view);
	
	return TRUE;
}

static gboolean
window_button_press_event_cb (GtkWidget *widget,
			      GdkEventButton *event,
			      gpointer user_data)
{
	g_debug ("window button");
	
	GtkWidget          *toplevel;
	toplevel = gtk_widget_get_toplevel (GTK_WIDGET (info));
	
	unfocus_popup_widget (GTK_WINDOW (toplevel), view);
	
	//return TRUE;
	return FALSE;
}

static gboolean
key_press(GtkWidget   *widget,
	GdkEventKey *event,
	gpointer     user_data)
{
	if (event->keyval == GDK_F4)
	{
		/* Usage with custom widget */
		if (GTK_WIDGET_VISIBLE(GTK_WIDGET(info)))
		{
			gtk_widget_hide(GTK_WIDGET(info));
		}
		else
		{
			gtk_source_completion_info_set_custom(info, custom);
			/*
			GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(custom_view));
			gtk_text_buffer_set_text(buffer,
						 gsc_gsv_get_text(GTK_TEXT_VIEW(view)),
						 -1);
			*/
			gtk_entry_set_text(GTK_ENTRY(entry), gsc_gsv_get_text(GTK_TEXT_VIEW(view)));
			gtk_source_completion_info_move_to_cursor(info,GTK_TEXT_VIEW(view));
			gtk_widget_show(GTK_WIDGET(info));
			gtk_widget_grab_focus (view);
			/*
			send_focus_change (info, FALSE);
			send_focus_change (view, TRUE);
			*/
		}
	}
	else if (event->keyval == GDK_F5)
	{
		gtk_source_completion_info_set_custom(info, NULL);
		/* Normal info usage with the default widget*/
		if (GTK_WIDGET_VISIBLE(GTK_WIDGET(info)))
		{
			gtk_widget_hide(GTK_WIDGET(info));
		}
		else
		{
			gtk_source_completion_info_set_markup(info,
					    gsc_gsv_get_text(GTK_TEXT_VIEW(view)));
			
			gtk_source_completion_info_move_to_cursor(info,GTK_TEXT_VIEW(view));
			gtk_widget_show(GTK_WIDGET(info));
		}
	}
	else if (event->keyval == GDK_F6)
	{
		GtkWidget *win = NULL;
		if (GTK_WIDGET_VISIBLE(GTK_WIDGET(info)))
			win = GTK_WIDGET (info);
		else
			win = GTK_WIDGET (comp);
			
		
		focus_popup_widget (win, view);
		
	}
	else if (event->keyval == GDK_F9)
	{
		gtk_source_completion_filter_proposals (comp,
					  filter_func,
					  NULL);
		return TRUE;
	}
	else if (event->keyval == GDK_F10)
	{
		//gsc_manager_info_set_visible (comp, TRUE);
		gboolean vis;
		g_object_get (comp, "info-visible", &vis, NULL);
		g_object_set (comp,"info-visible", !vis,NULL);
		return TRUE;
	}
	
	return FALSE;
}

GtkWidget*
create_tooltip_window (void)
{
	GtkWidget *window;
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_resize(GTK_WINDOW(window),400,200);
	GtkWidget *view = gtk_source_view_new();
	GtkWidget *scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_container_add(GTK_CONTAINER(scroll),view);
	gtk_container_add(GTK_CONTAINER(window),scroll);
	gtk_widget_show_all(window);
	return window;
}

GtkWidget*
create_window (void)
{
	GtkWidget *window;
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_resize(GTK_WINDOW(window),800,600);
	view = gtk_source_view_new();
	GtkWidget *scroll = gtk_scrolled_window_new(NULL,NULL);
	gtk_container_add(GTK_CONTAINER(scroll),view);
	gtk_container_add(GTK_CONTAINER(window),scroll);
	gtk_widget_set_has_tooltip(view,TRUE);
	
	g_signal_connect (view, "focus-out-event",
			  G_CALLBACK (focus_out_event_cb), NULL);

	
	g_signal_connect(view, "key-release-event", G_CALLBACK(key_press), NULL);
	
	g_signal_connect(window, "destroy", G_CALLBACK(destroy_cb), NULL);
	
	g_signal_connect (view, "button_press_event",
                          G_CALLBACK (window_button_press_event_cb),
                          NULL);
	
	return window;
}

static void
set_custom_keys(GtkSourceCompletion *comp)
{
	if (change_keys)
	{
		GValue val = {0};
		g_value_init(&val,G_TYPE_STRING);
		g_value_set_string(&val,"<Control>u");
		g_object_set_property(G_OBJECT(comp),"info-keys",&val);
		g_value_reset(&val);
		g_object_get_property(G_OBJECT(comp),"info-keys",&val);
		g_debug("info keys: %s",g_value_get_string(&val));
		g_value_reset(&val);
		g_value_set_string(&val,"o");
		g_object_set_property(G_OBJECT(comp),"previous-page-keys",&val);
		g_value_reset(&val);
		g_value_set_string(&val,"p");
		g_object_set_property(G_OBJECT(comp),"next-page-keys",&val);
	}
}

static void
comp_started (GtkSourceCompletion *comp, gpointer user_data)
{
	g_debug ("Started");
}

static void
comp_finished (GtkSourceCompletion *comp, gpointer user_data)
{
	g_debug ("finished");
}

static void
create_completion(void)
{
	GscDocumentwordsProvider *prov = gsc_documentwords_provider_new(GTK_TEXT_VIEW(view));

	GscProviderFile *prov_file = gsc_provider_file_new(GTK_TEXT_VIEW(view));
	gsc_provider_file_set_file(prov_file,"/tmp/main.c");
	
	GscProviderTest *prov_test = gsc_provider_test_new(GTK_TEXT_VIEW(view));
	
	//GscCutilsProvider *prov_cutils = gsc_cutils_provider_new();
	comp = GTK_SOURCE_COMPLETION(gtk_source_completion_new(GTK_TEXT_VIEW(view)));
	
	/*FIXME
	g_object_set (comp,
		      "autoselect", TRUE,
		      NULL);
	*/
	set_custom_keys(comp);
	/*FIXME
	g_signal_connect(comp,"completion-started",G_CALLBACK(comp_started),NULL);
	g_signal_connect(comp,"completion-finished",G_CALLBACK(comp_finished),NULL);
	*/
	
	
	GtkSourceCompletionTriggerCustomkey *ur_trigger = gtk_source_completion_trigger_customkey_new(comp,"User Request Trigger","<Control>Return");
	GtkSourceCompletionTriggerAutowords *ac_trigger = gtk_source_completion_trigger_autowords_new(comp);
	g_object_set (ac_trigger,
		      "delay", 500,
		      "min-len", 4,
		      NULL);
	
	gtk_source_completion_register_trigger(comp,GTK_SOURCE_COMPLETION_TRIGGER(ur_trigger));
	gtk_source_completion_register_trigger(comp,GTK_SOURCE_COMPLETION_TRIGGER(ac_trigger));
	
	gtk_source_completion_register_provider(comp,GTK_SOURCE_COMPLETION_PROVIDER(prov),GTK_SOURCE_COMPLETION_TRIGGER (ac_trigger));
	gtk_source_completion_register_provider(comp,GTK_SOURCE_COMPLETION_PROVIDER(prov_test),GTK_SOURCE_COMPLETION_TRIGGER (ac_trigger));
	gtk_source_completion_register_provider(comp,GTK_SOURCE_COMPLETION_PROVIDER(prov),GTK_SOURCE_COMPLETION_TRIGGER (ur_trigger));
	gtk_source_completion_register_provider(comp,GTK_SOURCE_COMPLETION_PROVIDER(prov_file),GTK_SOURCE_COMPLETION_TRIGGER (ur_trigger));
	//gtk_source_completion_register_provider(comp,prov_cutils,GSC_USERREQUEST_TRIGGER_NAME);
	gtk_source_completion_set_active(comp, TRUE);
	g_object_unref(prov);
	g_object_unref(ur_trigger);
	g_object_unref(ac_trigger);
	
}

static GtkSourceCompletionInfo*
create_info()
{
	info = gtk_source_completion_info_new();
	GtkRequisition req = {100,100};
	gtk_widget_size_request(GTK_WIDGET(info),&req);
	gtk_source_completion_info_set_adjust_height(info,TRUE,100000);
	gtk_source_completion_info_set_adjust_width(info,TRUE,100000);

	GtkWidget          *toplevel;
	toplevel = gtk_widget_get_toplevel (GTK_WIDGET (view));
	
	/*
	if (GTK_WINDOW (toplevel)->group)
                gtk_window_group_add_window (GTK_WINDOW (toplevel)->group,
                                             GTK_WINDOW (info));
	*/
	
	entry = GTK_WIDGET (gtk_entry_new());
	gtk_entry_set_text (GTK_ENTRY (entry), "chuchisadfas dfas dfasd fasd asd fasdf asd fad f ad");
	gtk_entry_set_has_frame (GTK_ENTRY (entry), TRUE);
	g_signal_connect (entry, "key_press_event",
                          G_CALLBACK (entry_key_press_event_cb),
                          view);
                          
	
                          
	g_object_ref(entry);
	gtk_widget_show (entry);

	custom = gtk_event_box_new ();
        
	GtkWidget *frame = gtk_frame_new (NULL);
	gtk_frame_set_shadow_type (GTK_FRAME (frame), GTK_SHADOW_ETCHED_IN);
	gtk_widget_show (frame);
	
	gtk_container_add (GTK_CONTAINER (custom),
			   frame);
 
	GtkWidget *vbox = gtk_vbox_new (FALSE, 0);
	gtk_widget_show (vbox);
	gtk_container_add (GTK_CONTAINER (frame), vbox);
	gtk_container_set_border_width (GTK_CONTAINER (vbox), 3);

	gtk_container_add (GTK_CONTAINER (vbox),
			   entry);

	gtk_widget_add_events (GTK_WIDGET (info), GDK_BUTTON_PRESS_MASK);
	g_signal_connect (info, "button_press_event",
                          G_CALLBACK (button_press_event_cb),
                          view);
	return info;
}

int
main (int argc, char *argv[])
{
 	GtkWidget *window;
	
	gtk_set_locale ();
	gtk_init (&argc, &argv);

	window = create_window ();
	info = create_info();
	create_completion();
	
	gtk_widget_show_all (window);

	gtk_main ();
	return 0;
}



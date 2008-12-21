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
#include <gtksourcecompletion/gsc-manager.h>
#include <gtksourcecompletion/gsc-documentwords-provider.h>
#include <gtksourcecompletion/gsc-trigger-customkey.h>
#include <gtksourcecompletion/gsc-trigger-autowords.h>
#include <gtksourcecompletion/gsc-provider-file.h>
#include <gtksourcecompletion/gsc-info.h>
#include <gtksourcecompletion/gsc-utils.h>

#include "gsc-provider-test.h"


static GtkWidget *view;
static GscManager *comp;
static GscInfo *info;
static gboolean cambio = FALSE;
static gboolean cambio_tool = FALSE;
static GtkWidget *custom = NULL;
static GtkWidget *custom_view = NULL;

static const gboolean change_keys = FALSE;

static void
destroy_cb(GtkObject *object,gpointer   user_data)
{
	gtk_main_quit ();
}

void
dev_button_cn (GtkButton *button, gpointer user_data)
{
	g_debug("Open devhelp");
}

gboolean query_tooltip_cb (GtkWidget  *widget,
                                                        gint        x,
                                                        gint        y,
                                                        gboolean    keyboard_mode,
                                                        GtkTooltip *tooltip,
                                                        gpointer    user_data) 
{
  GtkTextIter iter;
  GtkTextView *text_view = GTK_TEXT_VIEW (widget);

  if (keyboard_mode)
    {
    	g_debug("keyboard_mode true");
      gint offset;

      g_object_get (text_view->buffer, "cursor-position", &offset, NULL);
      gtk_text_buffer_get_iter_at_offset (text_view->buffer, &iter, offset);
    }
  else
    {
      gint bx, by, trailing;

      gtk_text_view_window_to_buffer_coords (text_view, GTK_TEXT_WINDOW_TEXT,
					     x, y, &bx, &by);
      gtk_text_view_get_iter_at_position (text_view, &iter, &trailing, bx, by);
    }

	GtkTextBuffer *buffer = gtk_text_view_get_buffer(text_view);
	GtkTextIter end_iter = iter;
	if (!gtk_text_iter_forward_char(&end_iter))
		return FALSE;
	gchar* tool = gtk_text_buffer_get_text(buffer,&iter, &end_iter,FALSE);
	
	gchar *oldtool = gtk_widget_get_tooltip_text(widget);
	if (oldtool != NULL && g_strcmp0(oldtool,tool) != 0)
	{
		tool = NULL;
	}
	
  if (tool != NULL){
    //gtk_tooltip_set_text (tooltip, tool);
    if (cambio_tool)
	    gtk_tooltip_set_text (tooltip, "aaaaaa");
    else 
    	gtk_tooltip_set_text (tooltip, tool);
    	
    cambio_tool = !cambio_tool;
   }
  else
   return FALSE;

  return TRUE;

}

static gboolean
key_press(GtkWidget   *widget,
	GdkEventKey *event,
	gpointer     user_data)
{
	if (event->keyval == GDK_F3)
	{
		g_debug("Show tooltip");
		//gtk_widget_set_tooltip_text(view,"holaaaaaaaaaaaaa");
		gtk_widget_trigger_tooltip_query(widget);
		return TRUE;
	}
	else if (event->keyval == GDK_F4)
	{
		/* Usage with custom widget */
		if (GTK_WIDGET_VISIBLE(GTK_WIDGET(info)))
		{
			gtk_widget_hide(GTK_WIDGET(info));
		}
		else
		{
			gsc_info_set_custom(info, custom);
			/*
			GtkTextBuffer *buffer = gtk_text_view_get_buffer(GTK_TEXT_VIEW(custom_view));
			gtk_text_buffer_set_text(buffer,
						 gsc_gsv_get_text(GTK_TEXT_VIEW(view)),
						 -1);
			*/
			gtk_label_set_text(GTK_LABEL(custom), gsc_gsv_get_text(GTK_TEXT_VIEW(view)));
			gsc_info_move_to_cursor(info,GTK_TEXT_VIEW(view));
			gtk_widget_show(GTK_WIDGET(info));
		}
	}
	else if (event->keyval == GDK_F5)
	{
		gsc_info_set_custom(info, NULL);
		/* Normal info usage with the default widget*/
		if (GTK_WIDGET_VISIBLE(GTK_WIDGET(info)))
		{
			gtk_widget_hide(GTK_WIDGET(info));
		}
		else
		{
			gsc_info_set_markup(info,
					    gsc_gsv_get_text(GTK_TEXT_VIEW(view)));
			
			gsc_info_move_to_cursor(info,GTK_TEXT_VIEW(view));
			gtk_widget_show(GTK_WIDGET(info));
		}
	}
	else if (event->keyval == GDK_F6)
	{
		/* Filter the proposals starting by "ccc" */
		/*
		FIXME We will implement this when Nacho finish to integrate
		gsc-tree into gsc-popup
		*/
		
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
	g_signal_connect(view, "key-release-event", G_CALLBACK(key_press), NULL);
	
	g_signal_connect(window, "destroy", G_CALLBACK(destroy_cb), NULL);
	
	g_signal_connect(view,"query-tooltip",G_CALLBACK(query_tooltip_cb),NULL);
	
	return window;
}

static void
set_custom_keys(GscManager *comp)
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
create_completion(void)
{
	GscDocumentwordsProvider *prov = gsc_documentwords_provider_new(GTK_TEXT_VIEW(view));

	GscProviderFile *prov_file = gsc_provider_file_new(GTK_TEXT_VIEW(view));
	gsc_provider_file_set_file(prov_file,"/tmp/main.c");
	
	GscProviderTest *prov_test = gsc_provider_test_new(GTK_TEXT_VIEW(view));
	
	//GscCutilsProvider *prov_cutils = gsc_cutils_provider_new();
	comp = gsc_manager_new(GTK_TEXT_VIEW(view));
	set_custom_keys(comp);
	GscTriggerCustomkey *ur_trigger = gsc_trigger_customkey_new(comp,"User Request Trigger","<Control>Return");
	GscTriggerAutowords *ac_trigger = gsc_trigger_autowords_new(comp);
	g_object_set (ac_trigger,
		      "delay", 500,
		      "min-len", 4,
		      NULL);
	
	gsc_manager_register_trigger(comp,GSC_TRIGGER(ur_trigger));
	gsc_manager_register_trigger(comp,GSC_TRIGGER(ac_trigger));
	
	gsc_manager_register_provider(comp,GSC_PROVIDER(prov),gsc_trigger_get_name (GSC_TRIGGER (ac_trigger)));
	gsc_manager_register_provider(comp,GSC_PROVIDER(prov_test),gsc_trigger_get_name (GSC_TRIGGER (ac_trigger)));
	gsc_manager_register_provider(comp,GSC_PROVIDER(prov),"User Request Trigger");
	gsc_manager_register_provider(comp,GSC_PROVIDER(prov_file),"User Request Trigger");
	//gtk_source_completion_register_provider(comp,prov_cutils,GSC_USERREQUEST_TRIGGER_NAME);
	gsc_manager_activate(comp);
	g_object_unref(prov);
	g_object_unref(ur_trigger);
	g_object_unref(ac_trigger);
	
}

static GscInfo*
create_info()
{
	info = gsc_info_new();
	GtkRequisition req = {100,100};
	gtk_widget_size_request(GTK_WIDGET(info),&req);
	gsc_info_set_adjust_height(info,TRUE,100000);
	gsc_info_set_adjust_width(info,TRUE,100000);
	
	/*
	custom = gtk_scrolled_window_new (NULL, NULL);
	
	custom_view = gtk_text_view_new();
	
	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (custom),
					GTK_POLICY_AUTOMATIC,
					GTK_POLICY_AUTOMATIC);
	gtk_scrolled_window_add_with_viewport (GTK_SCROLLED_WINDOW (custom),
					       custom_view);
	
	gsc_info_set_bottom_bar_visible(GSC_INFO (info), TRUE);
	
	gtk_widget_show (custom_view);
	*/
	custom = gtk_label_new("chuchisadfas dfas dfasd fasd asd fasdf asd fad f ad");
	g_object_ref(custom);
	gtk_widget_show (custom);
	
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



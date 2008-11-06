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
#include <gtksourceview/gtksourceview.h>
#include <gtksourcecompletion/gsc-manager.h>
#include <gtksourcecompletion/gsc-documentwords-provider.h>
#include <gtksourcecompletion/gsc-trigger-customkey.h>
#include <gtksourcecompletion/gsc-trigger-autowords.h>
#include <gtksourcecompletion/gsc-provider-file.h>

GtkWidget *view;
static const gboolean change_keys = FALSE;

static void
destroy_cb(GtkObject *object,gpointer   user_data)
{
	gtk_main_quit ();
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
	
	g_signal_connect(window, "destroy", G_CALLBACK(destroy_cb), NULL);
	
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
	//GscCutilsProvider *prov_cutils = gsc_cutils_provider_new();
	GscManager *comp = gsc_manager_new(GTK_TEXT_VIEW(view));
	set_custom_keys(comp);
	GscTriggerCustomkey *ur_trigger = gsc_trigger_customkey_new(comp,"User Request Trigger","<Control>Return");
	GscManagerEventOptions *opts = g_new0(GscManagerEventOptions,1);
	opts->popup_options.filter_type = GSC_POPUP_FILTER_TREE;
	gsc_trigger_customkey_set_opts(ur_trigger,opts);
	GscTriggerAutowords *ac_trigger = gsc_trigger_autowords_new(comp);
	gsc_manager_register_trigger(comp,GSC_TRIGGER(ur_trigger));
	gsc_manager_register_trigger(comp,GSC_TRIGGER(ac_trigger));
	gsc_manager_register_provider(comp,GSC_PROVIDER(prov),GSC_TRIGGER_AUTOWORDS_NAME);
	gsc_manager_register_provider(comp,GSC_PROVIDER(prov),"User Request Trigger");
	gsc_manager_register_provider(comp,GSC_PROVIDER(prov_file),"User Request Trigger");
	//gtk_source_completion_register_provider(comp,prov_cutils,GSC_USERREQUEST_TRIGGER_NAME);
	gsc_manager_activate(comp);
	g_object_unref(prov);
	g_object_unref(ur_trigger);
	g_object_unref(ac_trigger);
		
}


int
main (int argc, char *argv[])
{
 	GtkWidget *window;
	
	gtk_set_locale ();
	gtk_init (&argc, &argv);

	window = create_window ();
	create_completion();
	
	gtk_widget_show_all (window);

	gtk_main ();
	return 0;
}



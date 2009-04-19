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
#include "gsc-completion.h"
#include "gsc-info.h"
#include "gsc-trigger-customkey.h"
#include "gsc-trigger-autowords.h"
#include "gsc-utils.h"

#include "gsc-provider-test.h"


static GtkWidget *view;
static GscCompletion *comp;

static const gboolean change_keys = FALSE;

typedef struct 
{
	GtkWidget *box;
	GtkWidget *header;
	GtkWidget *content;
	GtkWidget *foot;
} CustomWidget;


static gboolean
filter_func (GscProposal *proposal,
	     gpointer user_data)
{
	const gchar *label = gsc_proposal_get_label (proposal);
	return g_str_has_prefix (label, "sp");
}

static void
destroy_cb(GtkObject *object,gpointer   user_data)
{
	gtk_main_quit ();
}

static gboolean
display_info_cb (GscCompletion *comp,
		 GscProposal *prop,
		 gpointer user_data)
{
	CustomWidget *cw = (CustomWidget*) user_data;
	gchar *text;
	
	text = g_strdup_printf ("Header of: %s", gsc_proposal_get_label (prop));
	gtk_label_set_text (GTK_LABEL (cw->header), text);
	g_free (text);
	
	text = g_strdup_printf ("Content: \n%s", gsc_proposal_get_info (prop));
	gtk_label_set_text (GTK_LABEL (cw->content), text);
	g_free (text);
	
	text = g_strdup_printf ("Foot of: %s", gsc_proposal_get_label (prop));
	gtk_label_set_text (GTK_LABEL (cw->foot), text);
	g_free (text);
	return TRUE;
}

static gboolean
key_press(GtkWidget   *widget,
	GdkEventKey *event,
	gpointer     user_data)
{
	if (event->keyval == GDK_F9)
	{
		gsc_completion_filter_proposals (comp,
					  filter_func,
					  NULL);
		return TRUE;
	}
	
	return FALSE;
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
	
	return window;
}

static void
create_completion(void)
{
	GscProviderTest *prov_test = gsc_provider_test_new(GTK_TEXT_VIEW(view));
	
	//GscCutilsProvider *prov_cutils = gsc_cutils_provider_new();
	comp = GSC_COMPLETION(gsc_completion_new(GTK_TEXT_VIEW(view)));
	GscTriggerCustomkey *ur_trigger = gsc_trigger_customkey_new(comp,"User Request Trigger","<Control>Return");
	GscTriggerAutowords *ac_trigger = gsc_trigger_autowords_new(comp);

	g_object_set (ac_trigger,
		      "delay", 500,
		      "min-len", 4,
		      NULL);
	
	gsc_completion_register_trigger(comp,GSC_TRIGGER(ur_trigger));
	gsc_completion_register_trigger(comp,GSC_TRIGGER(ac_trigger));
	
	gsc_completion_register_provider(comp,GSC_PROVIDER(prov_test),GSC_TRIGGER (ac_trigger));
	//gtk_source_completion_register_provider(comp,prov_cutils,GSC_USERREQUEST_TRIGGER_NAME);
	gsc_completion_set_active(comp, TRUE);
	g_object_unref(prov_test);
	g_object_unref(ur_trigger);
	g_object_unref(ac_trigger);
	
	/*Custom info widget*/
	
	GtkWidget *box = gtk_vbox_new (FALSE,2);
	GtkWidget *header = gtk_label_new ("Header");
	GtkWidget *content = gtk_label_new ("Content");
	GtkWidget *foot = gtk_label_new ("Foot");
	GtkWidget *btbox = gtk_hbox_new (FALSE,2);
	GtkWidget *next_page_icon = gtk_image_new_from_stock (GTK_STOCK_GO_FORWARD,
						   GTK_ICON_SIZE_SMALL_TOOLBAR);
	GtkWidget *prev_page_icon = gtk_image_new_from_stock (GTK_STOCK_GO_BACK,
						   GTK_ICON_SIZE_SMALL_TOOLBAR);
	gtk_box_pack_start (GTK_BOX (btbox),
			    next_page_icon,
			    FALSE,
			    FALSE,
			    1);
	gtk_box_pack_start (GTK_BOX (btbox),
			    prev_page_icon,
			    FALSE,
			    FALSE,
			    1);
	
	gtk_box_pack_start (GTK_BOX (box),
			    header,
			    FALSE,
			    FALSE,
			    1);
	gtk_box_pack_start (GTK_BOX (box),
			    content,
			    TRUE,
			    TRUE,
			    1);
	gtk_box_pack_start (GTK_BOX (box),
			  foot,
			  FALSE,
			  FALSE,
			  1);
	gtk_box_pack_end (GTK_BOX (box),
			  btbox,
			  FALSE,
			  FALSE,
			  1);
			  
	GscInfo *info = gsc_completion_get_info_widget (comp);
	gsc_info_set_custom(info, box);
	
	gtk_widget_show_all (box);
	
	CustomWidget *cw = g_malloc0 (sizeof (CustomWidget));
	cw->box = box;
	cw->header = header;
	cw->content = content;
	cw->foot = foot;
	
	/*Connect to "display-info" signal to set the content into the custom widget*/
	g_signal_connect(comp,"display-info",G_CALLBACK(display_info_cb),cw);
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



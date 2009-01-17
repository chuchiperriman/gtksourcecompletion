/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-popup-priv.h
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

#ifndef GSC_POPUP_PRIV_H
#define GSC_POPUP_PRIV_H

/*
 * We need this private header to use it into the manager without publish
 * some fields and some functions to the user.
 */

G_BEGIN_DECLS

typedef struct _GscPopupPage
{
	gchar *name;
	GtkWidget *view;
} GscPopupPage;

struct _GscPopupPriv
{
	GtkWidget *info_window;
	GtkWidget *info_button;
	GtkWidget *notebook;
	GtkWidget *tab_label;
	GtkWidget *next_page_button;
	GtkWidget *prev_page_button;
	GtkWidget *bottom_bar;
	
	GList *pages;
	GscPopupPage *active_page;

	gboolean destroy_has_run;
};

void	_gsc_popup_info_hide	(GscPopup *self);
void	_gsc_popup_info_show	(GscPopup *self);

G_END_DECLS

#endif 

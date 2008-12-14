/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-info.h
 *
 *  Copyright (C) 2008 - ChuchiPerriman <chuchiperriman@gmail.com>
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

#ifndef _GSC_INFO_H
#define _GSC_INFO_H

#include <glib-object.h>
#include <glib.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GSC_TYPE_INFO             (gsc_info_get_type ())
#define GSC_INFO(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_INFO, GscInfo))
#define GSC_INFO_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_INFO, GscInfoClass)
#define GSC_IS_INFO(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_INFO))
#define GSC_IS_INFO_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_INFO))
#define GSC_INFO_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_INFO, GscInfoClass))

typedef struct _GscInfoPrivate GscInfoPrivate;

typedef struct _GscInfo GscInfo;

struct _GscInfo
{
	GtkWindow parent;
	
	GscInfoPrivate *priv;
};

typedef struct _GscInfoClass GscInfoClass;

struct _GscInfoClass
{
	GtkWindowClass parent_class;
};

typedef enum
{
	GSC_INFO_TYPE_SHORT,
	GSC_INFO_TYPE_EXTENDED
} GscInfoType;

GType		 gsc_info_get_type			(void) G_GNUC_CONST;

GscInfo		*gsc_info_new				(void);

void		 gsc_info_move_to_cursor		(GscInfo* self,
							 GtkTextView *view);

void		 gsc_info_set_markup			(GscInfo* self,
							 const gchar* markup);

void		 gsc_info_set_info_type			(GscInfo* self,
							 GscInfoType type);

GscInfoType	 gsc_info_get_info_type			(GscInfo* self);


void		 gsc_info_set_adjust_height		(GscInfo* self,
							 gboolean adjust,
							 gint max_height);

void		 gsc_info_set_adjust_width		(GscInfo* self,
							 gboolean adjust,
							 gint max_width);

void		 gsc_info_set_custom			(GscInfo* self,
							 GtkWidget *custom_widget);

GtkWidget	*gsc_info_get_custom			(GscInfo* self);

void		 gsc_info_set_bottom_bar_visible	(GscInfo* self,
							 gboolean visible);

GtkWidget	*gsc_info_get_bottom_bar		(GscInfo* self);

G_END_DECLS

#endif

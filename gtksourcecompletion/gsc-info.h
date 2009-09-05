/*
 * gscinfo.h
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

#ifndef __GSC_COMPLETION_INFO_H__
#define __GSC_COMPLETION_INFO_H__

#include <glib-object.h>
#include <gtk/gtk.h>

G_BEGIN_DECLS

#define GTK_TYPE_SOURCE_COMPLETION_INFO             (gsc_completion_info_get_type ())
#define GSC_COMPLETION_INFO(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GTK_TYPE_SOURCE_COMPLETION_INFO, GscInfo))
#define GSC_COMPLETION_INFO_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GTK_TYPE_SOURCE_COMPLETION_INFO, GscInfoClass)
#define GTK_IS_SOURCE_COMPLETION_INFO(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GTK_TYPE_SOURCE_COMPLETION_INFO))
#define GTK_IS_SOURCE_COMPLETION_INFO_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GTK_TYPE_SOURCE_COMPLETION_INFO))
#define GSC_COMPLETION_INFO_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GTK_TYPE_SOURCE_COMPLETION_INFO, GscInfoClass))

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
	
	void	(*before_show)	(GscInfo *info);
};

GType		 gsc_completion_info_get_type		(void) G_GNUC_CONST;

GscInfo *
		 gsc_completion_info_new			(void);

void		 gsc_completion_info_move_to_iter	(GscInfo *info,
								 GtkTextView             *view,
								 GtkTextIter             *iter);

void		 gsc_completion_info_set_sizing		(GscInfo *info,
								 gint                     width,
								 gint                     height,
								 gboolean                 shrink_width,
								 gboolean                 shrink_height);

void		 gsc_completion_info_set_widget		(GscInfo *info,
								 GtkWidget               *widget);

GtkWidget	*gsc_completion_info_get_widget		(GscInfo *info);

void		 gsc_completion_info_process_resize	(GscInfo *info);

G_END_DECLS

#endif /* __GSC_COMPLETION_INFO_H__ */

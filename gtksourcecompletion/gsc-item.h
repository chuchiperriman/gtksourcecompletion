/*
 * gscitem.h
 * This file is part of gsc
 *
 * Copyright (C) 2009 - Jesse van den Kieboom
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
 * Foundation, Inc., 51 Franklin St, Fifth Floor, 
 * Boston, MA  02110-1301  USA
 */

#ifndef __GSC_ITEM_H__
#define __GSC_ITEM_H__

#include <glib-object.h>
#include "gsc-proposal.h"

G_BEGIN_DECLS

#define GSC_TYPE_ITEM			(gsc_item_get_type ())
#define GSC_ITEM(obj)			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_ITEM, GscItem))
#define GSC_ITEM_CONST(obj)		(G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_ITEM, GscItem const))
#define GSC_ITEM_CLASS(klass)		(G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_ITEM, GscItemClass))
#define GSC_IS_ITEM(obj)		(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_ITEM))
#define GSC_IS_ITEM_CLASS(klass)	(G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_ITEM))
#define GSC_ITEM_GET_CLASS(obj)	(G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_ITEM, GscItemClass))

typedef struct _GscItem		GscItem;
typedef struct _GscItemClass	GscItemClass;
typedef struct _GscItemPrivate	GscItemPrivate;

struct _GscItem {
	GObject parent;
	
	GscItemPrivate *priv;
};

struct _GscItemClass {
	GObjectClass parent_class;
};

GType 			 gsc_item_get_type 		(void) G_GNUC_CONST;

GscItem *gsc_item_new 		(const gchar *label,
									 const gchar *action,
									 GdkPixbuf   *icon,
									 const gchar *info);

GscItem *gsc_item_new_with_markup	(const gchar *markup,
									 const gchar *action,
									 GdkPixbuf   *icon,
									 const gchar *info);

GscItem *gsc_item_new_from_stock	(const gchar *label,
								 	 const gchar *action,
								 	 const gchar *stock,
								 	 const gchar *info);

G_END_DECLS

#endif /* __GSC_ITEM_H__ */

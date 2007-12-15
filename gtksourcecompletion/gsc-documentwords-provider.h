/*
 * gsc-documentwords-provider.h - Type here a short description of your plugin
 *
 * Copyright (C) 2007 - perriman
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __DOCUMENTWORDS_PLUGIN_H__
#define __DOCUMENTWORDS_PLUGIN_H__

#include <glib.h>
#include <glib-object.h>
#include "gtksourcecompletion-provider.h"
#include "gtksourcecompletion.h"

G_BEGIN_DECLS

#define TYPE_GSC_DOCUMENTWORDS_PROVIDER (gsc_documentwords_provider_get_type ())
#define GSC_DOCUMENTWORDS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_GSC_DOCUMENTWORDS_PROVIDER, GscDocumentwordsProvider))
#define GSC_DOCUMENTWORDS_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GSC_DOCUMENTWORDS_PROVIDER, GscDocumentwordsProviderClass))
#define IS_GSC_DOCUMENTWORDS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_GSC_DOCUMENTWORDS_PROVIDER))
#define IS_GSC_DOCUMENTWORDS_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_GSC_DOCUMENTWORDS_PROVIDER))
#define GSC_DOCUMENTWORDS_PROVIDER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GSC_DOCUMENTWORDS_PROVIDER, GscDocumentwordsProviderClass))

#define GSC_DOCUMENTWORDS_PROVIDER_NAME "GscDocumentwordsProvider"

typedef struct _GscDocumentwordsProvider GscDocumentwordsProvider;
typedef struct _GscDocumentwordsProviderClass GscDocumentwordsProviderClass;
typedef struct _GscDocumentwordsProviderPrivate GscDocumentwordsProviderPrivate;

struct _GscDocumentwordsProvider {
	GObject parent;
	GscDocumentwordsProviderPrivate *priv;	
};

struct _GscDocumentwordsProviderClass {
	GObjectClass parent;
};

GscDocumentwordsProvider* 
gsc_documentwords_provider_new();

GType gsc_documentwords_provider_get_type ();

G_END_DECLS

#endif

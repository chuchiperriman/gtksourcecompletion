/*
 * ##(FILENAME) - ##(DESCRIPTION)
 *
 * Copyright (C) ##(DATE_YEAR) - ##(AUTHOR_FULLNAME)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifndef __##(PLUGIN_ID.upper)_PROVIDER_H__
#define __##(PLUGIN_ID.upper)_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include "gtksourcecompletion-provider.h"
#include "gtksourcecompletion.h"

G_BEGIN_DECLS


#define TYPE_GSC_##(PLUGIN_ID.upper)_PROVIDER (gsc_##(PLUGIN_ID.lower)_provider_get_type ())
#define GSC_##(PLUGIN_ID.upper)_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), TYPE_GSC_##(PLUGIN_ID.upper)_PROVIDER, Gsc##(PLUGIN_ID.camel)Provider))
#define GSC_##(PLUGIN_ID.upper)_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), TYPE_GSC_##(PLUGIN_ID.upper)_PROVIDER, Gsc##(PLUGIN_ID.camel)ProviderClass))
#define IS_GSC_##(PLUGIN_ID.upper)_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), TYPE_GSC_##(PLUGIN_ID.upper)_PROVIDER))
#define IS_GSC_##(PLUGIN_ID.upper)_PROVIDER_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), TYPE_GSC_##(PLUGIN_ID.upper)_PROVIDER))
#define GSC_##(PLUGIN_ID.upper)_PROVIDER_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), TYPE_GSC_##(PLUGIN_ID.upper)_PROVIDER, Gsc##(PLUGIN_ID.camel)ProviderClass))

#define GSC_##(PLUGIN_ID.upper)_PROVIDER_NAME "Gsc##(PLUGIN_ID.camel)"

typedef struct _Gsc##(PLUGIN_ID.camel)Provider Gsc##(PLUGIN_ID.camel)Provider;
typedef struct _Gsc##(PLUGIN_ID.camel)ProviderClass Gsc##(PLUGIN_ID.camel)ProviderClass;
typedef struct _Gsc##(PLUGIN_ID.camel)ProviderPrivate Gsc##(PLUGIN_ID.camel)ProviderPrivate;

struct _Gsc##(PLUGIN_ID.camel)Provider {
	GObject parent;
	Gsc##(PLUGIN_ID.camel)ProviderPrivate *priv;	
};

struct _Gsc##(PLUGIN_ID.camel)ProviderClass {
	GObjectClass parent;
};

GType gsc_##(PLUGIN_ID.lower)_provider_get_type ();

Gsc##(PLUGIN_ID.camel)Provider* gsc_##(PLUGIN_ID.lower)_provider_new();

G_END_DECLS

#endif

/* 
 *  ##(FILENAME) - ##(DESCRIPTION)
 *
 *  Copyright (C) ##(DATE_YEAR) - ##(AUTHOR_FULLNAME)
 *
 *  This program is free software: you can redistribute it and/or modify
 *  it under the terms of the GNU Lesser General Public License as published by
 *  the Free Software Foundation, either version 3 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Lesser General Public License for more details.
 *
 *  You should have received a copy of the GNU Lesser General Public License
 *  along with Foobar.  If not, see <http://www.gnu.org/licenses/>.
 */
#ifndef __##(PLUGIN_ID.upper)_PROVIDER_H__
#define __##(PLUGIN_ID.upper)_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtksourcecompletion/gsc-provider.h>
#include <gtksourcecompletion/gsc-manager.h>

G_BEGIN_DECLS

#define GSC_TYPE_PROVIDER_##(PLUGIN_ID.upper) (gsc_provider_##(PLUGIN_ID.lower)_get_type ())
#define GSC_PROVIDER_##(PLUGIN_ID.upper)(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_PROVIDER_##(PLUGIN_ID.upper), GscProvider##(PLUGIN_ID.camel)))
#define GSC_PROVIDER_##(PLUGIN_ID.upper)_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_PROVIDER_##(PLUGIN_ID.upper), GscProvider##(PLUGIN_ID.camel)Class))
#define GSC_IS_PROVIDER_##(PLUGIN_ID.upper)(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_PROVIDER_##(PLUGIN_ID.upper)))
#define GSC_IS_PROVIDER_##(PLUGIN_ID.upper)_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_PROVIDER_##(PLUGIN_ID.upper)))
#define GSC_PROVIDER_##(PLUGIN_ID.upper)_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_PROVIDER_##(PLUGIN_ID.upper), GscProvider##(PLUGIN_ID.camel)Class))

#define GSC_PROVIDER_##(PLUGIN_ID.upper)_NAME "GscProvider##(PLUGIN_ID.camel)"

typedef struct _GscProvider##(PLUGIN_ID.camel) GscProvider##(PLUGIN_ID.camel);
typedef struct _GscProvider##(PLUGIN_ID.camel)Class GscProvider##(PLUGIN_ID.camel)Class;
typedef struct _GscProvider##(PLUGIN_ID.camel)Private GscProvider##(PLUGIN_ID.camel)Private;

struct _GscProvider##(PLUGIN_ID.camel) {
	GObject parent;
	GscProvider##(PLUGIN_ID.camel)Private *priv;	
};

struct _GscProvider##(PLUGIN_ID.camel)Class {
	GObjectClass parent;
};

GType gsc_provider_##(PLUGIN_ID.lower)_get_type ();

GscProvider##(PLUGIN_ID.camel)* gsc_provider_##(PLUGIN_ID.lower)_new();

G_END_DECLS

#endif

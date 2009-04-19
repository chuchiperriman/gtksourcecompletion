/* 
 *  gsc-provider-file.h - Type here a short description of your plugin
 *
 *  Copyright (C) 2008 - perriman
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
#ifndef __GSC_FILE_PROVIDER_H__
#define __GSC_FILE_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include <gio/gio.h>
#include "gsc-provider.h"

G_BEGIN_DECLS

#define GSC_TYPE_PROVIDER_FILE             (gsc_provider_file_get_type ())
#define GSC_PROVIDER_FILE(obj)             (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_PROVIDER_FILE, GscProviderFile))
#define GSC_PROVIDER_FILE_CLASS(klass)     (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_PROVIDER_FILE, GscProviderFileClass))
#define GSC_IS_PROVIDER_FILE(obj)          (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_PROVIDER_FILE))
#define GSC_IS_PROVIDER_FILE_CLASS(klass)  (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_PROVIDER_FILE))
#define GSC_PROVIDER_FILE_GET_CLASS(obj)   (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_PROVIDER_FILE, GscProviderFileClass))

#define GSC_PROVIDER_FILE_NAME "GscProviderFile"

typedef struct _GscProviderFile GscProviderFile;
typedef struct _GscProviderFileClass GscProviderFileClass;
typedef struct _GscProviderFilePrivate GscProviderFilePrivate;

struct _GscProviderFile
{
	GObject parent;
	
	GscProviderFilePrivate *priv;
};

struct _GscProviderFileClass
{
	GObjectClass parent;
};

GType 		 gsc_provider_file_get_type		(void);

GscProviderFile *gsc_provider_file_new			(GtkTextView *view);

void		 gsc_provider_file_set_file		(GscProviderFile *self,
							 const gchar* file);

G_END_DECLS

#endif

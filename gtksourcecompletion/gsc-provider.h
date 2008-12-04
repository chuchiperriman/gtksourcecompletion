/* -*- Mode: C; tab-width: 8; indent-tabs-mode: t; c-basic-offset: 8; coding: utf-8 -*-
 *  gsc-provider.h
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
 
#ifndef __GSC_PROVIDER_H__
#define __GSC_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "gsc-proposal.h"
#include "gsc-trigger.h"

G_BEGIN_DECLS


#define GSC_TYPE_PROVIDER (gsc_provider_get_type ())
#define GSC_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_PROVIDER, GscProvider))
#define GSC_IS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_PROVIDER))
#define GSC_PROVIDER_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GSC_TYPE_PROVIDER, GscProviderIface))

typedef struct _GscProvider GscProvider;
typedef struct _GscProviderIface GscProviderIface;

struct _GscProviderIface
{
	GTypeInterface g_iface;
	
	const gchar* (*get_name)       (GscProvider *self);
	GList*       (*get_proposals)  (GscProvider* self,
				        GscTrigger *trigger);
	void         (*finish)         (GscProvider* self);
};

GType         gsc_provider_get_type            (void);


const gchar  *gsc_provider_get_name      (GscProvider* self);

GList        *gsc_provider_get_proposals (GscProvider* self, 
					  GscTrigger *trigger);
				
void          gsc_provider_finish        (GscProvider* self);

G_END_DECLS

#endif

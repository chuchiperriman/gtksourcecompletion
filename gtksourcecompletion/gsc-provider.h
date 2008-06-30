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

G_BEGIN_DECLS


#define GSC_TYPE_PROVIDER (gsc_provider_get_type ())
#define GSC_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_PROVIDER, GscProvider))
#define GSC_IS_PROVIDER(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_PROVIDER))
#define GSC_PROVIDER_GET_INTERFACE(obj) (G_TYPE_INSTANCE_GET_INTERFACE ((obj), GSC_TYPE_PROVIDER, GscProviderIface))

typedef struct _GscProvider GscProvider;
typedef struct _GscProviderIface GscProviderIface;

#include "gsc-manager.h"
#include "gsc-trigger.h"
#include "gsc-proposal.h"

struct _GscProviderIface {
	
	GTypeInterface parent;
	const gchar* (*get_name) (GscProvider *self);
	GList* (*get_proposals) (GscProvider* self,
	 		    GscManager* completion, 
	 		    GscTrigger *trigger);
	void (*finish) (GscProvider* self,
	 			GscManager* completion);							
};

GType 
gsc_provider_get_type ();

/**
 * gsc_provider_get_name:
 * @self: the #GscProvider
 *
 * The provider name. By example: "Document word completion provider"
 *
 * Returns: The provider's name
 * 
 **/
const gchar*
gsc_provider_get_name(GscProvider* self);

/**
 * gsc_provider_get_proposals:
 * @self: the #GscProvider
 * @completion: The #GscManager.
 * @trigger: The #GscTrigger that raise the event
 *
 * The completion call this function when an event is raised.
 * This function may return a list of #GscProposal to be shown
 * in the popup to the user.
 *
 * Returns: a list of #GscProposal or NULL if there are no proposals
 * 
 **/
GList* 
gsc_provider_get_proposals (GscProvider* self, 
					 GscManager* completion, 
					 GscTrigger *trigger);

/**
 * gsc_provider_finish:
 * @self: the #GscProvider
 * @view: The #GscManager.
 *
 * The completion call this function when it is goint to hide the popup
 * 
 **/					
void 
gsc_provider_finish (GscProvider* self, 
				       GscManager* completion);

G_END_DECLS

#endif

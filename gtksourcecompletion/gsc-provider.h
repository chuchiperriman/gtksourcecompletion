/*
 * gscprovider.h
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

#ifndef __GSC_PROVIDER_H__
#define __GSC_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtk/gtk.h>
#include "gsc-proposal.h"
#include "gsc-info.h"

G_BEGIN_DECLS

#define GSC_TYPE_PROVIDER 			(gsc_provider_get_type ())
#define GSC_PROVIDER(obj) 			(G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_PROVIDER, GscProvider))
#define GSC_IS_PROVIDER(obj) 			(G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_PROVIDER))
#define GSC_PROVIDER_GET_INTERFACE(obj) 	(G_TYPE_INSTANCE_GET_INTERFACE ((obj), GSC_TYPE_PROVIDER, GscProviderIface))

#define GSC_COMPLETION_CAPABILITY_INTERACTIVE "standard::interactive"
#define GSC_COMPLETION_CAPABILITY_AUTOMATIC "standard::automatic"

typedef struct _GscProvider GscProvider;
typedef struct _GscProviderIface GscProviderIface;

#include "gsc-context.h"

struct _GscProviderIface
{
	GTypeInterface g_iface;
	
	const gchar	*(*get_name)       	(GscProvider *provider);
	GdkPixbuf	*(*get_icon)       	(GscProvider *provider);
	void		 (*populate_completion)	(GscProvider *provider,
						 GscContext  *context);

	const gchar     *(*get_capabilities)	(GscProvider *provider);

	GtkWidget 	*(*get_info_widget)	(GscProvider *provider,
						 GscProposal *proposal);
	void		 (*update_info)		(GscProvider *provider,
						 GscProposal *proposal,
						 GscInfo     *info);

	gboolean	 (*activate_proposal)	(GscProvider *provider,
						 GscProposal *proposal,
						 GtkTextIter                 *iter);
};

GType		 gsc_provider_get_type		(void);


const gchar	*gsc_provider_get_name		(GscProvider *provider);

GdkPixbuf	*gsc_provider_get_icon		(GscProvider *provider);

void		 gsc_provider_populate_completion	(GscProvider *provider,
									 GscContext  *context);

const gchar 	*gsc_provider_get_capabilities 	(GscProvider *provider);

GtkWidget	*gsc_provider_get_info_widget		(GscProvider *provider,
									 GscProposal *proposal);

void 		 gsc_provider_update_info		(GscProvider *provider,
									 GscProposal *proposal,
									 GscInfo     *info);

gboolean	 gsc_provider_activate_proposal 	(GscProvider *provider,
									 GscProposal *proposal,
									 GtkTextIter                 *iter);

G_END_DECLS

#endif

/*
 * gsc-trigger-userrequest.h - Type here a short description of your trigger
 *
 * Copyright (C) 2008 - perriman
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

#ifndef GSC_TRIGGER_USERREQUEST_H__
#define GSC_TRIGGER_USERREQUEST_H__

#include <glib.h>
#include <glib-object.h>
#include "gsc-trigger.h"
#include "gsc-completion.h"

G_BEGIN_DECLS

#define GSC_TYPE_TRIGGER_USERREQUEST (gsc_trigger_userrequest_get_type ())
#define GSC_TRIGGER_USERREQUEST(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_TRIGGER_USERREQUEST, GscTriggerUserRequest))
#define GSC_TRIGGER_USERREQUEST_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_TRIGGER_USERREQUEST, GscTriggerUserRequestClass))
#define GSC_IS_TRIGGER_USERREQUEST(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_TRIGGER_USERREQUEST))
#define GSC_IS_TRIGGER_USERREQUEST_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_TRIGGER_USERREQUEST))
#define GSC_TRIGGER_USERREQUEST_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_TRIGGER_USERREQUEST, GscTriggerUserRequestClass))

#define GSC_TRIGGER_USERREQUEST_NAME "User Request Trigger"

typedef struct _GscTriggerUserRequest GscTriggerUserRequest;
typedef struct _GscTriggerUserRequestClass GscTriggerUserRequestClass;
typedef struct _GscTriggerUserRequestPrivate GscTriggerUserRequestPrivate;

struct _GscTriggerUserRequest
{
	GObject parent;
	
	GscTriggerUserRequestPrivate *priv;	
};

struct _GscTriggerUserRequestClass
{
	GObjectClass parent;
};

GType			 gsc_trigger_userrequest_get_type	(void) G_GNUC_CONST;

GscTriggerUserRequest	*gsc_trigger_userrequest_new	(GscCompletion *completion);

void			 gsc_trigger_userrequest_set_keys	(GscTriggerUserRequest *self,
							 const gchar *keys);

G_END_DECLS

#endif

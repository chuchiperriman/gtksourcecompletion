/*
 * gsc-private.h
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

#ifndef __GSC_COMPLETION_PIVATE_H__
#define __GSC_COMPLETION_PIVATE_H__

#include <gtk/gtk.h>
#include "gsc-completion.h"

GscCompletion*gsc_completion_new (struct _GtkSourceView *source_view);

#endif /* __GSC_COMPLETION_PIVATE_H__ */


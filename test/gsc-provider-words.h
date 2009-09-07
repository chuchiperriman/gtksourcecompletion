/* 
 *  gsc-provider-words.h - Type here a short description of your plugin
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
#ifndef __WORDS_PROVIDER_H__
#define __WORDS_PROVIDER_H__

#include <glib.h>
#include <glib-object.h>
#include <gtksourcecompletion/gsc-provider.h>

G_BEGIN_DECLS

#define GSC_TYPE_PROVIDER_WORDS (gsc_provider_words_get_type ())
#define GSC_PROVIDER_WORDS(obj) (G_TYPE_CHECK_INSTANCE_CAST ((obj), GSC_TYPE_PROVIDER_WORDS, GscProviderWords))
#define GSC_PROVIDER_WORDS_CLASS(klass) (G_TYPE_CHECK_CLASS_CAST ((klass), GSC_TYPE_PROVIDER_WORDS, GscProviderWordsClass))
#define GSC_IS_PROVIDER_WORDS(obj) (G_TYPE_CHECK_INSTANCE_TYPE ((obj), GSC_TYPE_PROVIDER_WORDS))
#define GSC_IS_PROVIDER_WORDS_CLASS(klass) (G_TYPE_CHECK_CLASS_TYPE ((klass), GSC_TYPE_PROVIDER_WORDS))
#define GSC_PROVIDER_WORDS_GET_CLASS(obj) (G_TYPE_INSTANCE_GET_CLASS ((obj), GSC_TYPE_PROVIDER_WORDS, GscProviderWordsClass))

#define GSC_PROVIDER_WORDS_NAME "GscProviderWords"

typedef enum{
	GSC_DOCUMENTWORDS_PROVIDER_SORT_NONE,
	GSC_DOCUMENTWORDS_PROVIDER_SORT_BY_LENGTH
} GscProviderWordsSortType;

typedef struct _GscProviderWords GscProviderWords;
typedef struct _GscProviderWordsPrivate GscProviderWordsPrivate;
typedef struct _GscProviderWordsClass GscProviderWordsClass;

struct _GscProviderWords
{
	GObject parent;
	
	GscProviderWordsPrivate *priv;
};

struct _GscProviderWordsClass
{
	GObjectClass parent;
};

GType		 gsc_provider_words_get_type	(void) G_GNUC_CONST;

GscProviderWords *gsc_provider_words_new (const gchar *name,
                                        GdkPixbuf   *icon);

G_END_DECLS

#endif

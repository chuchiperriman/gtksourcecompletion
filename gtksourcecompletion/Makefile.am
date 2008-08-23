INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DGTK_SOURCE_COMPLETIONLOCALEDIR=\""$(gtksourcecompletionlocaledir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DICON_DIR=\""$(datadir)/gtksourcecompletion/pixmaps"\"	\
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(PACKAGE_CFLAGS) \
	`pkg-config --cflags gtksourceview-2.0` \
	`pkg-config --cflags libglade-2.0`

AM_CFLAGS =\
	 -Wall\
	 -g
	 
lib_LTLIBRARIES = \
	libgtksourcecompletion-1.0.la

BUILT_SOURCES =                         \
        gsc-marshal.c                 \
        gsc-marshal.h

libgtksourcecompletion_headers = 	\
	gsc-provider.h	\
	gsc-trigger.h		\
	gsc-utils.h		\
	gsc-manager.h				\
	gsc-proposal.h			\
	gsc-trigger-customkey.h	\
	gsc-trigger-autowords.h	\
	gsc-documentwords-provider.h		\
	gsc-tree.h				\
	gsc-i18n.h			\
	gsc-popup.h

libgtksourcecompletion_1_0_la_SOURCES = \
	$(BUILT_SOURCES)		\
	gsc-manager.c			\
	gsc-provider.c		\
	gsc-trigger.c		\
	gsc-utils.c			\
	gsc-proposal.c			\
	gsc-trigger-customkey.c	\
	gsc-trigger-autowords.c	\
	gsc-documentwords-provider.c		\
	gsc-tree.c				\
	gsc-popup.c				\
	$(libgtksourcecompletion_headers)
	

libgtksourcecompletion_1_0_la_LIBADD = \
	$(PACKAGE_LIBS) 
	
libgtksourcecompletion_1_0_la_LDFLAGS = \
	-version-info 1:0:0 	\
	`pkg-config --libs gtksourceview-2.0`

INST_H_FILES = \
	gsc-provider.h	\
	gsc-trigger.h		\
	gsc-utils.h		\
	gsc-manager.h				\
	gsc-proposal.h			\
	gsc-trigger-customkey.h	\
	gsc-trigger-autowords.h	\
	gsc-providers.h		\
	gsc-tree.h				\
	gsc-i18n.h			\
	gsc-popup.h

gsc-marshal.h: gsc-marshal.list $(GLIB_GENMARSHAL)
	$(GLIB_GENMARSHAL) $< --header --prefix=gtksourcecompletion_marshal > $@

gsc-marshal.c: gsc-marshal.list $(GLIB_GENMARSHAL)
	echo "#include \"gsc-marshal.h\"" > $@ && \
	$(GLIB_GENMARSHAL) $< --body --prefix=gtksourcecompletion_marshal >> $@

EXTRA_DIST=gsc-marshal.list

CLEAN_FILES=$(BUILT_SOURCES)

libgtksourcecompletion_1_0_includedir = $(includedir)/gtksourcecompletion-1.0/gtksourcecompletion

libgtksourcecompletion_1_0_include_HEADERS =          \
	$(libgtksourcecompletion_headers)             

#headerdir = $(prefix)/include/gtksourcecompletion

#header_DATA = $(INST_H_FILES) 

## File created by the gnome-build tools

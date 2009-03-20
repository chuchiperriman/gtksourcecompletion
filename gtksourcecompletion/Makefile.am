INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DGTK_SOURCE_COMPLETIONLOCALEDIR=\""$(gtksourcecompletionlocaledir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DICON_DIR=\""$(datadir)/gtksourcecompletion/pixmaps"\"	\
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	$(PACKAGE_CFLAGS) \
	`pkg-config --cflags gtksourceview-2.0`

AM_CFLAGS =\
	 -Wall\
	 -g
	 
lib_LTLIBRARIES = \
	libgtksourcecompletion-2.0.la

BUILT_SOURCES =		\
	gsc-marshal.c	\
	gsc-marshal.h

libgtksourcecompletion_headers = 	\
	gtksourcecompletionprovider.h	\
	gtksourcecompletiontrigger.h	\
	gsc-utils.h			\
	gtksourcecompletionproposal.h	\
	gsc-i18n.h			\
	gtksourcecompletion.h		\
	gtksourcecompletioninfo.h	\
	gtksourcecompletiontrigger-autowords.h	\
	gtksourcecompletiontrigger-customkey.h

libgtksourcecompletion_2_0_la_SOURCES = \
	$(BUILT_SOURCES)		\
	gtksourcecompletionprovider.c	\
	gtksourcecompletiontrigger.c	\
	gsc-utils.c			\
	gtksourcecompletionproposal.c	\
	gtksourcecompletion.c		\
	gtksourcecompletioninfo.c	\
	gtksourcecompletiontrigger-autowords.c	\
	gtksourcecompletiontrigger-customkey.c	\
	$(libgtksourcecompletion_headers)
	

libgtksourcecompletion_2_0_la_LIBADD = \
	$(PACKAGE_LIBS) 
	
libgtksourcecompletion_2_0_la_LDFLAGS = \
	-version-info 1:0:0 	\
	`pkg-config --libs gtksourceview-2.0`

INST_H_FILES = \
	gtksourcecompletionprovider.h	\
	gtksourcecompletiontrigger.h	\
	gsc-utils.h			\
	gtksourcecompletionproposal.h	\
	gtksourceproviders.h		\
	gsc-i18n.h			\
	gtksourcecompletion.h		\
	gtksourcecompletioninfo.h	\
	gtksourcecompletiontrigger-autowords.h	\
	gtksourcecompletiontrigger-customkey.h

gsc-marshal.h: gsc-marshal.list $(GLIB_GENMARSHAL)
	$(GLIB_GENMARSHAL) $< --header --prefix=gtksourcecompletion_marshal > $@

gsc-marshal.c: gsc-marshal.list $(GLIB_GENMARSHAL)
	echo "#include \"gsc-marshal.h\"" > $@ && \
	$(GLIB_GENMARSHAL) $< --body --prefix=gtksourcecompletion_marshal >> $@

EXTRA_DIST=gsc-marshal.list

CLEAN_FILES=$(BUILT_SOURCES)

libgtksourcecompletion_2_0_includedir = $(includedir)/gtksourcecompletion-2.0/gtksourcecompletion

libgtksourcecompletion_2_0_include_HEADERS =	\
	$(libgtksourcecompletion_headers)

#headerdir = $(prefix)/include/gtksourcecompletion

#header_DATA = $(INST_H_FILES) 

## File created by the gnome-build tools

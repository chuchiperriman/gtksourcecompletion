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
	libgtksourcecompletion-3.0.la

BUILT_SOURCES =		\
	gtksourcecompletionmarshal.c	\
	gtksourcecompletionmarshal.h

libgtksourcecompletion_headers = 	\
	gtksourcecompletionprovider.h	\
	gtksourcecompletiontrigger.h	\
	gtksourcecompletionutils.h			\
	gtksourcecompletionproposal.h	\
	gtksourcecompletioni18n.h			\
	gtksourcecompletion.h		\
	gtksourcecompletioninfo.h	\
	gtksourcecompletiontrigger-autowords.h	\
	gtksourcecompletiontrigger-customkey.h

libgtksourcecompletion_3_0_la_SOURCES = \
	$(BUILT_SOURCES)		\
	gtksourcecompletionprovider.c	\
	gtksourcecompletiontrigger.c	\
	gtksourcecompletionutils.c			\
	gtksourcecompletionproposal.c	\
	gtksourcecompletion.c		\
	gtksourcecompletioninfo.c	\
	gtksourcecompletiontrigger-autowords.c	\
	gtksourcecompletiontrigger-customkey.c	\
	$(libgtksourcecompletion_headers)
	

libgtksourcecompletion_3_0_la_LIBADD = \
	$(PACKAGE_LIBS) 
	
libgtksourcecompletion_3_0_la_LDFLAGS = \
	-version-info 1:0:0 	\
	`pkg-config --libs gtksourceview-2.0`

INST_H_FILES = \
	gtksourcecompletionprovider.h	\
	gtksourcecompletiontrigger.h	\
	gtksourcecompletionutils.h			\
	gtksourcecompletionproposal.h	\
	gtksourceproviders.h		\
	gtksourcecompletioni18n.h			\
	gtksourcecompletion.h		\
	gtksourcecompletioninfo.h	\
	gtksourcecompletiontrigger-autowords.h	\
	gtksourcecompletiontrigger-customkey.h

gtksourcecompletionmarshal.h: gtksourcecompletionmarshal.list $(GLIB_GENMARSHAL)
	$(GLIB_GENMARSHAL) $< --header --prefix=gtksourcecompletion_marshal > $@

gtksourcecompletionmarshal.c: gtksourcecompletionmarshal.list $(GLIB_GENMARSHAL)
	echo "#include \"gtksourcecompletionmarshal.h\"" > $@ && \
	$(GLIB_GENMARSHAL) $< --body --prefix=gtksourcecompletion_marshal >> $@

EXTRA_DIST=gtksourcecompletionmarshal.list

CLEAN_FILES=$(BUILT_SOURCES)

libgtksourcecompletion_3_0_includedir = $(includedir)/gtksourcecompletion-3.0/gtksourcecompletion

libgtksourcecompletion_3_0_include_HEADERS =	\
	$(libgtksourcecompletion_headers)

#headerdir = $(prefix)/include/gtksourcecompletion

#header_DATA = $(INST_H_FILES) 

## File created by the gnome-build tools

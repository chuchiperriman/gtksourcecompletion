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

libgtk_source_completion_headers = 	\
	gtksourcecompletion-provider.h	\
	gtksourcecompletion-trigger.h		\
	gtksourcecompletion-utils.h		\
	gtksourcecompletion.h				\
	gtksourcecompletion-proposal.h			\
	gsc-userrequest-trigger.h			\
	gsc-customkey-trigger.h			\
	gsc-autocompletion-trigger.h		\
	gsc-documentwords-provider.h		\
	gtksourcecompletion-tree.h				\
	gtksourcecompletion-i18n.h			\
	gtksourcecompletion-popup.h

libgtk_source_completion_1_0_la_SOURCES = \
	gtksourcecompletion.c			\
	gtksourcecompletion-provider.c		\
	gtksourcecompletion-trigger.c		\
	gtksourcecompletion-utils.c			\
	gtksourcecompletion-proposal.c			\
	gsc-userrequest-trigger.c			\
	gsc-customkey-trigger.c			\
	gsc-autocompletion-trigger.c		\
	gsc-documentwords-provider.c		\
	gtksourcecompletion-tree.c				\
	gtksourcecompletion-popup.c				\
	$(libgtk_source_completion_headers)
	

libgtk_source_completion_1_0_la_LIBADD = \
	$(PACKAGE_LIBS) 
	
libgtk_source_completion_1_0_la_LDFLAGS = \
	-version-info 1:0:0 -module	\
	`pkg-config --libs gtksourceview-2.0`

INST_H_FILES = \
	gtksourcecompletion-provider.h	\
	gtksourcecompletion-trigger.h		\
	gtksourcecompletion-utils.h		\
	gtksourcecompletion.h				\
	gtksourcecompletion-proposal.h			\
	gsc-userrequest-trigger.h			\
	gsc-autocompletion-trigger.h		\
	gtksourcecompletion-providers.h		\
	gtksourcecompletion-tree.h				\
	gtksourcecompletion-i18n.h			\
	gtksourcecompletion-popup.h
	
libgtk_source_completion_1_0_includedir = $(includedir)/gtksourcecompletion-1.0/gtksourcecompletion

libgtk_source_completion_1_0_include_HEADERS =          \
	$(libgtk_source_completion_headers)             

#headerdir = $(prefix)/include/gtksourcecompletion

#header_DATA = $(INST_H_FILES) 

## File created by the gnome-build tools

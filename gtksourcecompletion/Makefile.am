icondir = $(datadir)/gtksourcecompletion/pixmaps
icon_DATA = pixmaps/*.png

INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DGTKSOURCECOMPLETIONLOCALEDIR=\""$(gtksourcecompletionlocaledir)"\" \
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DICON_DIR=\""$(icondir)"\" \
	$(PACKAGE_CFLAGS) \
	`pkg-config --cflags gtksourceview-1.0` \
	`pkg-config --cflags libglade-2.0`

AM_CFLAGS =\
	 -Wall\
	 -g
	 
lib_LTLIBRARIES = \
	libgtksourcecompletion-1.0.la

libgtksourcecompletion_headers = 	\
	gtksourcecompletion-provider.h	\
	gtksourcecompletion-trigger.h		\
	gtksourcecompletion-utils.h		\
	gtksourcecompletion.h				\
	gsc-userrequest-trigger.h			\
	gsc-autocompletion-trigger.h		\
	gsc-documentwords-provider.h

libgtksourcecompletion_1_0_la_SOURCES = \
	gtksourcecompletion.c			\
	gtksourcecompletion-provider.c		\
	gtksourcecompletion-trigger.c		\
	gtksourcecompletion-utils.c		\
	gsc-userrequest-trigger.c			\
	gsc-autocompletion-trigger.c		\
	gsc-documentwords-provider.c		\
	$(libgtksourcecompletion_headers)
	

libgtksourcecompletion_1_0_la_LIBADD = \
	$(PACKAGE_LIBS) 
	
libgtksourcecompletion_1_0_la_LDFLAGS = \
	-version-info 1:0:0 -module	\
	`pkg-config --libs gtksourceview-1.0`

INST_H_FILES = \
	gtksourcecompletion-provider.h	\
	gtksourcecompletion-trigger.h		\
	gtksourcecompletion-utils.h		\
	gtksourcecompletion.h				\
	gsc-userrequest-trigger.h			\
	gsc-autocompletion-trigger.h		\
	gtksourcecompletion-providers.h	
	
libgtksourcecompletion_1_0_includedir = $(includedir)/gtksourcecompletion-1.0/gtksourcecompletion

libgtksourcecompletion_1_0_include_HEADERS =          \
	$(libgtksourcecompletion_headers)             

#headerdir = $(prefix)/include/gtksourcecompletion

#header_DATA = $(INST_H_FILES) 

EXTRA_DIST = $(icon_DATA)
	
## File created by the gnome-build tools


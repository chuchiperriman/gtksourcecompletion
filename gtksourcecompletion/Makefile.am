INCLUDES = \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DGTKSOURCECOMPLETIONLOCALEDIR=\""$(gtksourcecompletionlocaledir)"\" \
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

libgtksourcecompletion_headers = 	\
	gtksourcecompletion-provider.h	\
	gtksourcecompletion-trigger.h		\
	gtksourcecompletion-utils.h		\
	gtksourcecompletion.h				\
	gtksourcecompletion-proposal.h			\
	gsc-userrequest-trigger.h			\
	gsc-customkey-trigger.h			\
	gsc-autocompletion-trigger.h		\
	gsc-documentwords-provider.h		\
	gsv-completion-tree.h				\
	gtksourcecompletion-i18n.h			\
	gsv-completion-popup.h

libgtksourcecompletion_1_0_la_SOURCES = \
	gtksourcecompletion.c			\
	gtksourcecompletion-provider.c		\
	gtksourcecompletion-trigger.c		\
	gtksourcecompletion-utils.c			\
	gtksourcecompletion-proposal.c			\
	gsc-userrequest-trigger.c			\
	gsc-customkey-trigger.c			\
	gsc-autocompletion-trigger.c		\
	gsc-documentwords-provider.c		\
	gsv-completion-tree.c				\
	gsv-completion-popup.c				\
	$(libgtksourcecompletion_headers)
	

libgtksourcecompletion_1_0_la_LIBADD = \
	$(PACKAGE_LIBS) 
	
libgtksourcecompletion_1_0_la_LDFLAGS = \
	-version-info 1:0:0 -module	\
	`pkg-config --libs gtksourceview-2.0`

INST_H_FILES = \
	gtksourcecompletion-provider.h	\
	gtksourcecompletion-trigger.h		\
	gtksourcecompletion-utils.h		\
	gtksourcecompletion.h				\
	gtksourcecompletion-item.h			\
	gsc-userrequest-trigger.h			\
	gsc-autocompletion-trigger.h		\
	gtksourcecompletion-providers.h		\
	gsv-completion-tree.h				\
	gtksourcecompletion-i18n.h			\
	gsv-completion-popup.h
	
libgtksourcecompletion_1_0_includedir = $(includedir)/gtksourcecompletion-1.0/gtksourcecompletion

libgtksourcecompletion_1_0_include_HEADERS =          \
	$(libgtksourcecompletion_headers)             

#headerdir = $(prefix)/include/gtksourcecompletion

#header_DATA = $(INST_H_FILES) 

## File created by the gnome-build tools

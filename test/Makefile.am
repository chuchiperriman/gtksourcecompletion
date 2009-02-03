bin_PROGRAMS = gio-test completion-simple completion-focus 

INCLUDES=	\
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-DICON_DIR=\""$(datadir)/gtksourcecompletion/pixmaps"\" \
	-I$(top_srcdir)/gtksourcecompletion	\
	`pkg-config --cflags gtksourceview-2.0` \
	`pkg-config --cflags gio-2.0` \
	$(PACKAGE_CFLAGS)

AM_CFLAGS = \
	-Wall	

noinst_PROGRAMS = gio-test completion-simple completion-focus

gio_test_SOURCES = 		\
	gio-test.c

gio_test_LDFLAGS =  \
	`pkg-config --libs gtksourceview-2.0`	\
	`pkg-config --libs gio-2.0`

gio_test_LDADD = 	\
	$(top_builddir)/gtksourcecompletion/libgtksourcecompletion-2.0.la	\
	$(PACKAGE_LIBS)

completion_simple_SOURCES = 		\
	gsc-documentwords-provider.c	\
	gsc-documentwords-provider.h	\
	gsc-provider-file.c		\
	gsc-provider-file.h		\
	completion-simple.c		\
	gsc-provider-test.h		\
	gsc-provider-test.c

completion_simple_LDFLAGS =  \
	`pkg-config --libs gtksourceview-2.0`	\
	`pkg-config --libs gio-2.0`

completion_simple_LDADD = 	\
	$(top_builddir)/gtksourcecompletion/libgtksourcecompletion-2.0.la	\
	$(PACKAGE_LIBS)

completion_focus_SOURCES = 		\
	gsc-documentwords-provider.c	\
	gsc-documentwords-provider.h	\
	gsc-provider-file.c		\
	gsc-provider-file.h		\
	completion-focus.c		\
	gsc-provider-test.h		\
	gsc-provider-test.c

completion_focus_LDFLAGS =  \
	`pkg-config --libs gtksourceview-2.0`	\
	`pkg-config --libs gio-2.0`

completion_focus_LDADD = 	\
	$(top_builddir)/gtksourcecompletion/libgtksourcecompletion-2.0.la	\
	$(PACKAGE_LIBS)

bin_PROGRAMS = gio-test completion-demo testtooltips

INCLUDES=	\
	-DPACKAGE_SRC_DIR=\""$(srcdir)"\" \
	-DPACKAGE_LOCALE_DIR=\""$(prefix)/$(DATADIRNAME)/locale"\" \
	-DPACKAGE_DATA_DIR=\""$(datadir)"\" \
	-I$(top_srcdir)/gtksourcecompletion	\
	`pkg-config --cflags gtksourceview-2.0` \
	`pkg-config --cflags gio-2.0` \
	$(PACKAGE_CFLAGS)

AM_CFLAGS = \
	-Wall	

noinst_PROGRAMS = gio-test completion-demo testtooltips

gio_test_SOURCES = 		\
	gio-test.c

gio_test_LDFLAGS =  \
	`pkg-config --libs gtksourceview-2.0`	\
	`pkg-config --libs gio-2.0`

gio_test_LDADD = 	\
	$(top_builddir)/gtksourcecompletion/libgtksourcecompletion-1.0.la	\
	$(PACKAGE_LIBS)

completion_demo_SOURCES = 		\
	completion-demo.c		\
	gsc-trigger-dotexample.h	\
	gsc-trigger-dotexample.c

completion_demo_LDFLAGS =  \
	`pkg-config --libs gtksourceview-2.0`	\
	`pkg-config --libs gio-2.0`

completion_demo_LDADD = 	\
	$(top_builddir)/gtksourcecompletion/libgtksourcecompletion-1.0.la	\
	$(PACKAGE_LIBS)

testtooltips_SOURCES = 		\
	testtooltips.c

testtooltips_LDFLAGS =  \
	`pkg-config --libs gtksourceview-2.0`	\
	`pkg-config --libs gio-2.0`

testtooltips_LDADD = 	\
	$(top_builddir)/gtksourcecompletion/libgtksourcecompletion-1.0.la	\
	$(PACKAGE_LIBS)
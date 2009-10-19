GTKDOC_CFLAGS = -I$(top_srcdir) $(DEP_CFLAGS)
GTKDOC_LIBS = $(top_builddir)/gtksourcecompletion/libgtksourcecompletion-2.0.la

## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = 1.7

# The name of the module.
DOC_MODULE = gtksourcecompletion-2.0

# The top-level SGML file.
DOC_MAIN_SGML_FILE = gtksourcecompletion-docs.sgml

# Extra options to supply to gtkdoc-scan
SCAN_OPTIONS = --rebuild-sections --rebuild-types

# The directory containing the source code. Relative to $(srcdir)
DOC_SOURCE_DIR = ../..

# Used for dependencies
HFILE_GLOB = $(top_srcdir)/gtksourcecompletion/*.h
CFILE_GLOB = $(top_srcdir)/gtksourcecompletion/*.c

# Headers to ignore
IGNORE_HFILES =					\
	config.h		\
	xmalloc.h		\
	provider-template.h			\
	provider-template.c			\
	proposal-template.h			\
	proposal-template.c			\
	trigger-template.h			\
	trigger-template.c	\
	getopt.h			\
	system.h			\
	gsc-i18n.h		\
	gsc-i18n.c		\
	gsc-trigger-dotexample.h		\
	gsc-trigger-dotexample.c		\
	gsc-provider-test.h			\
	gsc-provider-test.c			\
	gsc-provider-file.h			\
	gsc-provider-file.c			\
	gsc-documentwords-provider.c		\
	gsc-documentwords-provider.h		\
	gsc-trigger-interactive.c		\
	gsc-trigger-interactive.h		\
	gsc-provider-devhelp.c			\
	gsc-provider-devhelp.h			\
	gsc-provider-words.c			\
	gsc-provider-words.h			\
	gsc-marshal.h		

# Extra options to supply to gtkdoc-mkdb
MKDB_OPTIONS = --sgml-mode --output-format=xml --ignore-files=trio

# Images to copy into HTML directory
HTML_IMAGES =

# Extra SGML files that are included by $(DOC_MAIN_SGML_FILE)
#content_files =			\
#	version.xml		\
#	lang-tutorial.sgml	\
#	lang-reference.sgml

# Extra options to supply to gtkdoc-fixref
FIXXREF_OPTIONS=

# include common portion ...
include $(top_srcdir)/gtk-doc.make

# Other files to distribute
#EXTRA_DIST +=

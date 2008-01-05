GTKDOC_CFLAGS = -I$(top_srcdir) $(DEP_CFLAGS)
GTKDOC_LIBS = $(top_builddir)/gtksourcecompletion/libgtksourcecompletion-1.0.la

## Process this file with automake to produce Makefile.in
AUTOMAKE_OPTIONS = 1.7

# The name of the module.
DOC_MODULE = gtksourcecompletion-1.0

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
	trigger-template.h			\
	trigger-template.c

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

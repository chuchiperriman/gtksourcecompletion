dnl Process this file with autoconf to produce a configure script.

AC_INIT(gtksourcecompletion,0.6.0)
dnl Every other copy of the package version number gets its value from here
AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)

dnl create a config.h file (Automake will add -DHAVE_CONFIG_H)
AM_CONFIG_HEADER(config.h)

AC_SUBST(VERSION)

ISODATE=`date +%Y-%m-%d`
AC_SUBST(ISODATE)

AC_CANONICAL_HOST

dnl pkg-config configuration
PKG_CHECK_MODULES(DEP, gtksourceview-2.0)
AC_SUBST(DEP_CFLAGS)
AC_SUBST(DEP_LIBS)

dnl Checks for programs.
AC_PROG_INSTALL
AC_PROG_CC
AM_PROG_LIBTOOL

AC_CONFIG_MACRO_DIR(m4)

dnl Checks for libraries.

############i18n support###############
AC_PROG_INTLTOOL([0.23])

GETTEXT_PACKAGE=gtksourcecompletion
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED([GETTEXT_PACKAGE], ["$GETTEXT_PACKAGE"],["Descripcion"])

ALL_LINGUAS="es fr it"
AM_GLIB_GNU_GETTEXT

gtksourcecompletionlocaledir='${prefix}/${DATADIRNAME}/locale'
AC_SUBST(gtksourcecompletionlocaledir)
########################################


dnl Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS(unistd.h sys/param.h sys/time.h time.h sys/mkdev.h sys/sysmacros.h string.h memory.h fcntl.h dirent.h sys/ndir.h ndir.h alloca.h locale.h )

jm_CHECK_TYPE_STRUCT_UTIMBUF
AC_HEADER_MAJOR
AC_FUNC_ALLOCA
AC_STRUCT_TM
AC_STRUCT_ST_BLOCKS
AC_FUNC_CLOSEDIR_VOID
AC_CHECK_FUNCS(mkfifo)
AC_CHECK_FUNC(mknod)
# check for gtk-doc
GTK_DOC_CHECK(1.8)
dnl Checks for typedefs, structures, and compiler characteristics.

dnl Checks for library functions.

AC_PATH_PROG(GLIB_GENMARSHAL, glib-genmarshal)

AC_OUTPUT([
Makefile
gtksourcecompletion-1.0.pc
gtksourcecompletion.lsm
gtksourcecompletion.spec
gtksourcecompletion/Makefile
test/Makefile
pixmaps/Makefile
docs/Makefile
docs/reference/Makefile
po/Makefile.in
])
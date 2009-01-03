#!/bin/sh
# Run this to generate all the initial makefiles, etc.

gtkdocize || exit 1

srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.
srcdir=`cd $srcdir && pwd`

PKG_NAME="gtksourcecompletion"

(test -f $srcdir/configure.ac \
  && test -f $srcdir/ChangeLog \
  && test -d $srcdir/gtksourcecompletion) || {
    echo -n "**Error**: Directory "\`$srcdir\'" does not look like the"
    echo " top-level $PKG_NAME directory"
    exit 1
}

which gnome-autogen.sh || {
    echo "You need to install gnome-common from the GNOME CVS"
    exit 1
}
USE_GNOME2_MACROS=1 . gnome-autogen.sh
conf_flags="--enable-maintainer-mode"

#!/bin/sh
srcdir=`dirname $0`
test -z "$srcdir" && srcdir=.

(cd $srcdir; aclocal; \
libtoolize --force; automake --add-missing; autoconf)

$srcdir/configure --enable-maintainer-mode "$@"

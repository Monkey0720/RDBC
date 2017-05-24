dnl $Id: configure.in,v 1.4 2012/10/17 09:46:59 bbannier Exp $
dnl     This file is part of the RDBC
dnl     Author: Valeriy Onuchin <onuchin@sirius.ihep.su>

dnl Process this file with autoconf to produce a configure script.
AC_INIT(acinclude.m4) dnl a source file from your sub dir

AC_CONFIG_AUX_DIR(admin)

AC_CANONICAL_SYSTEM 
AC_ARG_PROGRAM

RDBC_MAJOR_VERSION=1
RDBC_MINOR_VERSION=0
RDBC_MICRO_VERSION=3

RDBC_VERSION=$RDBC_MAJOR_VERSION.$RDBC_MINOR_VERSION.$RDBC_MICRO_VERSION

dnl libtool versioning
dnl
dnl version is current:revision:age
dnl versioning works like this:
dnl on implementation change:     revision++
dnl on interface change:          revision=0, current++
dnl on interface addition:        age++
dnl on interface removal:         age=0

LT_RELEASE=$RDBC_MAJOR_VERSION.$RDBC_MINOR_VERSION
LT_CURRENT=2
LT_REVISION=0
LT_AGE=0

VERSION=$RDBC_VERSION
PACKAGE=RDBC

AC_SUBST(RDBC_MAJOR_VERSION)
AC_SUBST(RDBC_MINOR_VERSION)
AC_SUBST(RDBC_MICRO_VERSION)
AC_SUBST(RDBC_VERSION)

AC_SUBST(LT_RELEASE)
AC_SUBST(LT_CURRENT)
AC_SUBST(LT_REVISION)
AC_SUBST(LT_AGE)

dnl Initialize automake stuff
AM_INIT_AUTOMAKE($PACKAGE, $VERSION)
AC_PROG_INSTALL

dnl make $ROOTSYS the default for the installation
AC_PREFIX_DEFAULT(${ROOTSYS:-/usr/local/root})

AM_ENABLE_SHARED(yes)
AM_ENABLE_STATIC(no)

AM_CONFIG_HEADER(config.h)

dnl Initialize libtool
AM_PROG_LIBTOOL

dnl Initialize maintainer mode
AM_MAINTAINER_MODE

dnl AC_CANONICAL_HOST

AM_CXXFLAGS="-g -O0"
AM_CFLAGS="-g -O0"

AC_PROG_CXX
AC_PROG_CXXCPP
AC_LANG_CPLUSPLUS

AC_CHECK_HEADERS(exception string,,AC_MSG_ERROR([Header $ac_hdr not found.]))
AC_CHECK_HEADERS(set set.h)
AC_CHECK_HEADERS(vector vector.h)
AC_CHECK_HEADERS(iostream istream iostream.h)
AC_CHECK_HEADERS(sstream strstream strstream.h)
AC_CHECK_HEADERS(ctime cstring cstdlib cstdio)

AC_CHECK_THREADS

if test "x$enable_threads" = "xyes"
then

#perform some checks for reentrant functions
AC_CHECK_FUNCS(localtime_r)

fi


AC_CHECK_FUNCS(strtoll strtoq snprintf)

AC_CHECK_CXX_EH

if test "x$ac_cv_cxx_eh" != "xyes" 
then
        AC_MSG_ERROR([$CXX does provide correct exception handling])
fi


AC_CHECK_CXX_NS

if test "x$ac_cv_cxx_ns" != "xyes"
then
        AC_MSG_ERROR([$CXX does not support namespaces])
fi


AC_CHECK_CXX_STL

if test "x$ac_cv_cxx_stl" != "xyes"
then
        AC_MSG_ERROR([STL is not available])
fi


AC_CHECK_ODBC

if test "x$odbc_ok" != "xyes"
then
        AC_CHECK_IODBC
fi


if test "x$iodbc_ok" != "xyes" &&
   test "x$odbc_ok" != "xyes" 
then
        AC_MSG_ERROR([No ODBC driver manager configured])
fi

AC_ARG_WITH(odbc-version,
[  --with-odbc-version=VER Force ODBCVER to VER],
[
if test "x$withval" != "xyes"
then
        AC_DEFINE_UNQUOTED(ODBCVER,$withval)
fi
])

AC_CHECK_HEADERS(inttypes.h)

AC_CHECK_SIZEOF(int,0)
AC_CHECK_SIZEOF(long,0)
AC_CHECK_SIZEOF(long long,0)

AC_CHECK_ROOT

ODBCXX_DIR=$odbc_dir
AC_SUBST(ODBCXX_DIR)

dnl add here all your Makefiles
AC_OUTPUT(Makefile RDBC.spec rdbc/Makefile odbc/Makefile include/Makefile include/RDBC/Makefile macros/Makefile  )



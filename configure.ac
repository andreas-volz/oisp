dnl Process this file with autoconf to produce a configure script.
dnl Created by Anjuta application wizard.

AC_INIT(oisp, 0.1)

AM_INIT_AUTOMAKE(AC_PACKAGE_NAME, AC_PACKAGE_VERSION)
AM_CONFIG_HEADER(config.h)
AM_MAINTAINER_MODE

AC_CONFIG_MACRO_DIR([m4])

AC_ISC_POSIX
AC_PROG_CXX
AM_PROG_CC_STDC
AC_HEADER_STDC

AX_CXX_COMPILE_STDCXX_0X
if test "$ax_cv_cxx_compile_cxx0x_native" = yes ||
   test "$ax_cv_cxx_compile_cxx0x_cxx" = yes ||
   test "$ax_cv_cxx_compile_cxx0x_gxx" = yes; then
  CXXFLAGS="$CXXFLAGS -std=c++0x"
fi

AM_PROG_LIBTOOL

dnl Check for option to enable profiling
AC_MSG_CHECKING(whether to enable profiling)
AC_ARG_ENABLE(profiling,
        [  --enable-profiling=[no/yes]        enables profiling build (default=yes)],,
                 enable_profiling=no)

AS_IF( [ ! test "x$enable_profiling" != "xyes"],[
  AC_DEFINE(PROFILING, 1, [Define to enable profiling build])
  PKG_CHECK_MODULES(PROFILING, StopClock >= 0.1)
  PROFILING_CFLAGS="$PROFILING_CFLAGS -pg"
],[
  AC_MSG_RESULT(no)
])

dnl Check for boost
#AX_BOOST_BASE
#if test "$succeeded" != "yes" ; then
#  echo "Error: You need to install the boost library!"
#  exit
#fi

#AX_BOOST_FILESYSTEM
#if test "x$ax_cv_boost_filesystem" = "xno"; then
#  echo
#  echo "Error: You need to install the boost filesystem library!"
#  echo
#  exit
#fi

dnl Check for headers
ACX_PTHREAD
if test x"$acx_pthread_ok" = xyes; then
  AC_DEFINE(HAVE_PTHREAD_H, 1, [Define to enable pthread support])
  AM_CONDITIONAL(HAVE_PTHREAD, test x"$acx_pthread_ok" = xyes)
fi

dnl Check for edce_cc
EDJE_DEF=""
AC_SUBST(EDJE_DEF)
AC_ARG_WITH(edje-cc,
        AC_HELP_STRING([--with-edje-cc=PATH],[Specify a specific path to edje_cc]),
[
  v=$withval;
  edje_cc=$v
  echo "  Enlightenment edje_cc explicitly set to "$edje_cc;
],[
  edje_cc=$(pkg-config --variable=prefix edje)/bin/edje_cc
])
AC_SUBST(edje_cc)

ACX_PTHREAD(true,exit)

PKG_CHECK_MODULES(GLIBMM, glibmm-2.4 gthread-2.0)
AC_SUBST(GLIBMM_CFLAGS)
AC_SUBST(GLIBMM_LIBS)

PKG_CHECK_MODULES(GPS, libgps)
AC_SUBST(GPS_CFLAGS)
AC_SUBST(GPS_LIBS)

PKG_CHECK_MODULES(SIGC, sigc++-2.0)
AC_SUBST(SIGC_CFLAGS)
AC_SUBST(SIGC_LIBS)

PKG_CHECK_MODULES(DBUSMM, dbus-c++-1)
AC_SUBST(DBUSMM_CFLAGS)
AC_SUBST(DBUSMM_LIBS)

PKG_CHECK_MODULES(DBUSMMGLIB, dbus-c++-1 dbus-c++-glib-1)
AC_SUBST(DBUSMM_CFLAGS)
AC_SUBST(DBUSMM_LIBS)

PKG_CHECK_MODULES(DBUSMMECORE, dbus-c++-1 dbus-c++-ecore-1)
AC_SUBST(DBUSMM_CFLAGS)
AC_SUBST(DBUSMM_LIBS)

PKG_CHECK_MODULES(EFL, evasxx edjexx ecorexx evas cairo emotionxx)
AC_SUBST(EFL_CFLAGS)
AC_SUBST(EFL_LIBS)

PKG_CHECK_MODULES(GTK, gtk+-2.0)
AC_SUBST(GTK_CFLAGS)
AC_SUBST(GTK_LIBS)

PKG_CHECK_MODULES(OICF, OICFControl OICFNavigation OICFMedia)
AC_SUBST(OICF_CFLAGS)
AC_SUBST(OICF_LIBS)

PKG_CHECK_MODULES(OICFPROVIDER, OICFControlProvider OICFNavigationProvider OICFMediaProvider)
AC_SUBST(OICFPROVIDER_CFLAGS)
AC_SUBST(OICFPROVIDER_LIBS)


#dnl Set PACKAGE SOURCE DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`

#dnl Set PACKAGE PREFIX
if test "x${prefix}" = "xNONE"; then
  packageprefix=${ac_default_prefix}
else
  packageprefix=${prefix}
fi

#dnl Set PACKAGE DIRS in config.h
packagedatadir=share/${PACKAGE}
packagedocdir=doc/${PACKAGE}
packagepixmapsdir=share/pixmaps/${PACKAGE}

#dnl Subst PACKAGE_DATA_DIR.
NO_PREFIX_PACKAGE_DATA_DIR="${packagedatadir}"
AC_SUBST(NO_PREFIX_PACKAGE_DATA_DIR)
PACKAGE_DATA_DIR="${packageprefix}/${packagedatadir}"
AC_SUBST(PACKAGE_DATA_DIR)

dnl Subst PACKAGE_DOC_DIR.
NO_PREFIX_PACKAGE_DOC_DIR="${packagedocdir}"
AC_SUBST(NO_PREFIX_PACKAGE_DOC_DIR)
PACKAGE_DOC_DIR="${packageprefix}/${packagedocdir}"
AC_SUBST(PACKAGE_DOC_DIR)

dnl Subst PACKAGE_PIXMAPS_DIR.
NO_PREFIX_PACKAGE_PIXMAPS_DIR="${packagepixmapsdir}"
AC_SUBST(NO_PREFIX_PACKAGE_PIXMAPS_DIR)
PACKAGE_PIXMAPS_DIR="${packageprefix}/${packagepixmapsdir}"
AC_SUBST(PACKAGE_PIXMAPS_DIR)

AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${packageprefix}/${packagedatadir}","")
AC_DEFINE_UNQUOTED(SYSTEM_DATA_DIR, "${packageprefix}/${systemdatadir}","")
AC_DEFINE_UNQUOTED(PACKAGE_DOC_DIR, "${packageprefix}/${packagedocdir}","")
AC_DEFINE_UNQUOTED(PACKAGE_PIXMAPS_DIR, "${packageprefix}/${packagepixmapsdir}","")
AC_DEFINE_UNQUOTED(SYSTEM_PIXMAPS_DIR, "${packageprefix}/${systempixmapsdir}","")
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}","")

PKG_CHECK_MODULES(LIBOSMSCOUT, libosmscout-map libosmscout libosmscout-map-cairo)
PKG_CHECK_MODULES(EMOTION, emotionxx)
PKG_CHECK_MODULES(UDEV, libudev)
PKG_CHECK_MODULES(STOPCLOCK, StopClock)
AC_OUTPUT([
Makefile
src/Makefile
src/OISPControl/Makefile
src/OISPControl_keyb/Makefile
src/OISPNavigation_libosmscout/Makefile
src/OISPMedia/Makefile
data/Makefile
data/themes/Makefile
data/themes/default/Makefile
data/themes/default/images/Makefile
data/themes/default/fonts/Makefile
])

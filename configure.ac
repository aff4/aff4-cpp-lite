#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ([2.59])
AC_INIT([aff4], [2.0.3], [info@evimetry.com])
AC_CONFIG_HEADERS([aff4config.h])
AC_CONFIG_SRCDIR([src/aff4.cc])
AC_CONFIG_MACRO_DIR([m4])
AC_CONFIG_AUX_DIR([config])
AC_CANONICAL_HOST

AC_SUBST(ARCH,[host_cpu])

AM_INIT_AUTOMAKE([1.10 foreign])
# Checks for programs.
AC_PROG_CXX
AC_LANG([C++])
AC_PROG_INSTALL
AC_PROG_LIBTOOL
AC_PROG_LN_S
AX_CXX_COMPILE_STDCXX_11([noext],[mandatory])

AM_CONDITIONAL([HAVE_WINDRES], [which windres > /dev/null])

AC_CHECK_PROGS([DOXYGEN], [doxygen])
if test -z "$DOXYGEN";
   then AC_MSG_WARN([Doxygen not found - continuing without Doxygen support])
fi

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([cstdlib string sstream stack set vector map memory algorithm mutex])

# Checks for typedefs, structures, and compiler characteristics.
AC_TYPE_SIZE_T
AC_CHECK_HEADER_STDBOOL
AC_TYPE_INT32_T
AC_TYPE_INT64_T
AC_TYPE_UINT16_T
AC_TYPE_UINT32_T
AC_TYPE_UINT64_T
AC_TYPE_UINT8_T

# Common libraries
PKG_CHECK_MODULES([ZLIB], [zlib], [], [AC_MSG_ERROR(Could not find zlib libraries.)])
PKG_CHECK_MODULES([RAPTOR2], [raptor2], [], [AC_MSG_ERROR(Could not find raptor2 libraries.)])
PKG_CHECK_MODULES([LZ4], [liblz4], [], [AC_MSG_ERROR(Could not find lz4 libraries.)])
AC_CHECK_LIB([snappy], [snappy_compress], [],
[
  AC_MSG_ERROR([Could not find libsnappy library])
])

# Optionals for unit testing
PKG_CHECK_MODULES([CPPUNIT], [cppunit >= 1.13.0], [AM_CONDITIONAL(HAVE_CPPUNIT, true)], [AC_MSG_WARN(Unit Tests disabled - missing cppunit)])
PKG_CHECK_MODULES([SSL], [openssl >= 1.0.2], [AM_CONDITIONAL(HAVE_OPENSSL, true)], [AC_MSG_WARN(Unit Tests disabled - missing OpenSSL)])

AH_VERBATIM([__STDC_FORMAT_MACROS],
[/* Enable PRIx64, et al. macros on all platforms */
#ifndef __STDC_FORMAT_MACROS
 #define __STDC_FORMAT_MACROS 1
#endif])

# Stick in "-Werror" if you want to be more aggressive.
# (No need to use AC_SUBST on this default substituted environment variable.)
# Only add these additional CFLAGS if we are using GCC. Other C compilers may
# not support them.
if test x"$GCC" == "xyes" ; then
    AX_CHECK_COMPILE_FLAG([ -Wall -Wextra -Wpedantic ], 
        [CXXFLAGS="$CXXFLAGS -Wall -Wextra -Wpedantic "])
fi

# Add debug support
AC_ARG_ENABLE(debug,
  AS_HELP_STRING(
    [--enable-debug],
    [enable debugging, default: no]),
    [case "${enableval}" in
      yes) debug=true ;;
      no)  debug=false ;;
      *)   AC_MSG_ERROR([bad value ${enableval} for --enable-debug]) ;;
    esac],
    [debug=false])
AM_CONDITIONAL(DEBUG, test x"$debug" = x"true")
AM_COND_IF(DEBUG,
    AC_DEFINE(DEBUG, 1, [Define to 0 if this is a release build]),
    AC_DEFINE(DEBUG, 0, [Define to 1 or higher if this is a debug build]))


AM_CONDITIONAL([HAVE_DOXYGEN],[test -n "$DOXYGEN"])
AM_COND_IF([HAVE_DOXYGEN], [AC_CONFIG_FILES([docs/Doxyfile])])

AC_CONFIG_FILES([
Makefile 
src/Makefile 
tests/Makefile
docs/Makefile
examples/Makefile
libaff4.pc])
AC_OUTPUT

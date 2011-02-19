# get rid of that stupid cache mechanism
rm -f config.cache

AC_INIT(rage, 0.3.0.042, enlightenment-devel@lists.sourceforge.net)
AC_PREREQ(2.52)
AC_CONFIG_SRCDIR(configure.ac)

AM_INIT_AUTOMAKE(1.6 dist-bzip2)
AM_CONFIG_HEADER(config.h)
m4_ifdef([AM_SILENT_RULES], [AM_SILENT_RULES([yes])])
AC_C_BIGENDIAN
AC_ISC_POSIX
AC_PROG_CC
AM_PROG_CC_STDC
AM_PROG_CC_C_O

AC_HEADER_STDC
AC_C_CONST

AM_ENABLE_SHARED
AM_PROG_LIBTOOL

if test "x${bindir}" = 'x${exec_prefix}/bin'; then
  if test "x${exec_prefix}" = "xNONE"; then
    if test "x${prefix}" = "xNONE"; then
      bindir="${ac_default_prefix}/bin";
    else
      bindir="${prefix}/bin";
    fi
  else
    if test "x${prefix}" = "xNONE"; then
      bindir="${ac_default_prefix}/bin";
    else
      bindir="${prefix}/bin";
    fi
  fi
fi

if test "x${libdir}" = 'x${exec_prefix}/lib'; then
  if test "x${exec_prefix}" = "xNONE"; then
    if test "x${prefix}" = "xNONE"; then
      libdir="${ac_default_prefix}/lib";
    else
      libdir="${prefix}/lib";
    fi
  else
    if test "x${prefix}" = "xNONE"; then
      libdir="${ac_default_prefix}/lib";
    else
      libdir="${prefix}/lib";
    fi
  fi
fi

dnl Set PACKAGE_DATA_DIR in config.h.
if test "x${prefix}" = "xNONE"; then
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${ac_default_prefix}/share/${PACKAGE}", "Package installed data destination")
else
  AC_DEFINE_UNQUOTED(PACKAGE_DATA_DIR, "${prefix}/share/${PACKAGE}", "Package installed data destination")
fi

dnl Set PACKAGE_BIN_DIR in config.h.
if test "x${bindir}" = 'xNONE'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${ac_default_prefix}/bin", [Install Location for Executables])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${prefix}/bin", [Install Location for Executables])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_BIN_DIR, "${bindir}", [Install Location for Executables])
fi

dnl Set PACKAGE_LIB_DIR in config.h.
if test "x${libdir}" = 'xNONE'; then
  if test "x${prefix}" = "xNONE"; then
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${ac_default_prefix}/lib", [Install Location for Libraries])
  else
    AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${prefix}/lib", [Install Location for Libraries])
  fi
else
  AC_DEFINE_UNQUOTED(PACKAGE_LIB_DIR, "${libdir}", [Install Location for Libraries])
fi

dnl Set PACKAGE_SOURCE_DIR in config.h.
packagesrcdir=`cd $srcdir && pwd`
AC_DEFINE_UNQUOTED(PACKAGE_SOURCE_DIR, "${packagesrcdir}", [Source Directory])

PKG_CHECK_MODULES([EET], eet >= 1.4.0)
PKG_CHECK_MODULES([EVAS], evas >= 1.0.0)
PKG_CHECK_MODULES([EDJE], edje >= 1.0.0)
PKG_CHECK_MODULES([ECORE], [
   ecore-con >= 1.0.0
   ecore-evas >= 1.0.0
   ecore-ipc >= 1.0.0
   ecore-file >= 1.0.0 ])
PKG_CHECK_MODULES([ELEMENTARY], 
	[
   elementary >= 0.6.0.063
  ]
)
PKG_CHECK_MODULES([EMOTION], emotion)

PKG_CHECK_MODULES([SQLITE3], sqlite3)

if test -z "$HUMOR"; then
AC_CHECK_LIB(Kenny, life_signs, , [
  echo "  Oh my god, they killed Kenny!  You bastards!"
	AC_DEFINE(HAVE_HUMOR, 1, [Defined if user has a sense of humor.])
])
fi

my_libs="-lm "$dlopen_libs""
AC_SUBST(my_libs)

##################
# Static Modules #
##################

AC_ARG_ENABLE(static_modules,
  [AC_HELP_STRING([--disable-static-modules], [disable static modules.
  [[default=enabled]]])],
  [use_static_modules=$enableval],
  [use_static_modules="yes"]
)
AM_CONDITIONAL([USE_STATIC_MODULES], test "x${use_static_modules}" = "xyes")
if test "x$use_static_modules" = "xyes"; then
  AC_DEFINE(USE_STATIC_MODULES, 1, [Static Modules])
fi

AC_OUTPUT([
Makefile
src/Makefile
src/bin/Makefile
data/Makefile
])

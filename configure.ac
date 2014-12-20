#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.59)
AC_INIT(girl, 0.8.0)
AM_INIT_AUTOMAKE([no-dist-gzip dist-xz])
AC_CONFIG_SRCDIR([src/girl.c])
AC_CONFIG_HEADER([config.h])

# Checks for programs.
AC_PROG_CC
# AC_PROG_LIBTOOL

AC_PATH_TOOL([GIRL_HELPER_PLAYER], [totem], [/usr/bin/totem],
             [$PATH$PATH_SEPARATOR/usr/bin$PATH_SEPARATOR]dnl
     [/usr/sbin$PATH_SEPARATOR/usr/etc$PATH_SEPARATOR/etc])

record_msg="no (http://streamripper.sf.net/)"
try_record=:
girl_record=false
AC_ARG_WITH([recording],AS_HELP_STRING([--with-recording],[Support recording with streamripper]), [
	if test x$withval = xno; then
	   try_record=false
	   record_msg = "no (not requested)"
	fi
])
record_prefix=$ac_default_prefix

if $try_record; then
   AC_PATH_TOOL([GIRL_HELPER_RECORD], [streamripper], [/usr/bin/streamripper],
					   [$PATH$PATH_SEPARATOR/usr/bin$PATH_SEPARATOR]dnl
					   [/usr/sbin$PATH_SEPARATOR/usr/etc$PATH_SEPARATOR/etc])
	AC_MSG_CHECKING([for recording])
	if test -n "${GIRL_HELPER_RECORD}"; then
	   AC_DEFINE([HAVE_GIRL_RECORD],1,[have record])
	   record_msg="yes"
	   girl_record=true
	else
	   AC_DEFINE([HAVE_GIRL_RECORD],0,[don't have record])
	   record_msg="no (http://streamripper.sf.net/)"
	   girl_record=false
        fi
	AC_MSG_RESULT([$record_msg])
fi
AM_CONDITIONAL([HAVE_GIRL_RECORD], [$girl_record])
AC_SUBST([HAVE_GIRL_RECORD])

# Checks for libraries.

PKG_CHECK_MODULES(GIRL,
		  gtk+-2.0 >= 2.0 \
		  libgnome-2.0 >= 2.0 \
		  libxml-2.0 >= 2.0 \
		  gnome-vfs-2.0 >= 2.0 \
		  libgnomeui-2.0 >= 2.0 \
		  libgnomecanvas-2.0 >= 2.0)
GIRL_PKG_DEPS="gtk+-2.0, libgnome-2.0, libxml-2.0, gnome-vfs-2.0, libgnomeui-2.0 libgnomecanvas-2.0"

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([stdlib.h string.h unistd.h])

# Checks for typedefs, structures, and compiler characteristics.

# Checks for library functions.
AC_FUNC_MALLOC
AC_CHECK_FUNCS([strdup])

# ***********                                                                   
# Translation                                                                   
# ***********                                                                   

AM_GLIB_GNU_GETTEXT
IT_PROG_INTLTOOL([0.50.1])

GETTEXT_PACKAGE=AC_PACKAGE_NAME
AC_SUBST(GETTEXT_PACKAGE)
AC_DEFINE_UNQUOTED(GETTEXT_PACKAGE, "$GETTEXT_PACKAGE", [Gettext package.])

AC_SUBST(GIRL_LIBS)
AC_SUBST(GIRL_CFLAGS)
AC_SUBST(GIRL_PKG_DEPS)
AC_SUBST(GIRL_HELPER_PLAYER)

AC_OUTPUT([
Makefile
data/Makefile
data/girl.desktop.in
data/pixmaps/Makefile
data/icons/Makefile
data/icons/16x16/Makefile
data/icons/22x22/Makefile
data/icons/24x24/Makefile
data/icons/32x32/Makefile
data/icons/48x48/Makefile
data/icons/256x256/Makefile
man/Makefile
po/Makefile.in
src/Makefile
girl.spec
])

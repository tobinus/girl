SUBDIRS = data src po

datadir = @datadir@

EXTRA_DIST = config.rpath m4/ChangeLog  \
	Makefile \
	ChangeLog \
	README \
	YP-DIRS \
	AUTHORS \
	LETTER \
	NEWS \
	THANKS \
	TODO \
	girl.spec.in

DISTCLEANFILES =                \
        intltool-extract        \
        intltool-merge          \
        intltool-update

MAINTAINERCLEANFILES = \
	$(srcdir)/aclocal.m4 \
	$(srcdir)/config.guess \
	$(srcdir)/config.h.in \
	$(srcdir)/config.sub \
	$(srcdir)/depcomp \
	$(srcdir)/gtk-doc.make \
	$(srcdir)/install-sh \
	$(srcdir)/ltmain.sh \
	$(srcdir)/missing \
	$(srcdir)/mkinstalldirs

-include $(top_srcdir)/git.mk

ACLOCAL_AMFLAGS = -I m4

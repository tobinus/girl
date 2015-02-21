SUBDIRS = data man src po debian

datadir = @datadir@

EXTRA_DIST = config.rpath m4/ChangeLog  \
	AUTHORS \
	ChangeLog \
	BROADCAST \
	HACKING \
	LETTER \
	Makefile \
	NEWS \
	NEWS-0.5 \
	NEWS-0.6 \
	NEWS-0.7 \
	NEWS-0.8 \
	NEWS-0.9 \
	NEWS-1.0 \
	NEWS-1.1 \
	NEWS-1.2 \
	NEWS-1.3 \
	NEWS-1.4 \
	README \
	THANKS \
	TODO \
	YP-DIRS \
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

AM_CFLAGS = $(GOAL_CFLAGS)
AM_LDFLAGS= $(GOAL_LIBS)

bin_PROGRAMS = girl
girl_SOURCES = \
	       girl.c \
	       girl-gui.c \
	       girl-listener.c \
	       girl-program.c \
	       girl-runners.c \
	       girl-streams.c \
	       girl-station.c

girl_CFLAGS  = $(GIRL_CFLAGS) \
	       -DGIRL_DATADIR=\"$(datadir)/girl\" \
	       -DDATADIR=\"$(datadir)\" \
	       -DGIRL_DEBUG=1 \
	       -DGIRL_CFG \
	       -DGIRL_HELPER_PLAYER=\"$(GIRL_HELPER_PLAYER)\" \
	       -DGIRL_HELPER_RECORD=\"$(GIRL_HELPER_RECORD)\" \
	       -DGNOMELOCALEDIR=\"$(datadir)/locale\"
girl_LDADD = $(GIRL_LIBS)

girldir = $(datadir)/girl
girl_DATA = girl-2.1.dtd girl-logo.png girl.png girl.xml

EXTRA_DIST = girl.h girl-station.h girl-gui.h girl-listener.h girl-program.h girl-runners.h girl-streams.h girl-2.1.dtd girl-logo.png girl.png girl.xml

GOAL_CFLAGS=`pkg-config --cflags gtk+-2.0 libgnome-2.0 libxml-2.0 gnome-vfs-2.0 libgnomeui-2.0 libgnomecanvas-2.0` -Wall -g -DHAVE_CONFIG_H -I. -DGNOMELOCALEDIR=\""`pkg-config --variable=prefix libgnomeui-2.0`/share/locale"\" -DGIRL_DATADIR=\""$(prefix)/share/girl"\" -DDATADIR=\""`pkg-config --variable=prefix libgnomeui-2.0`/share"\" -DGIRL_DEBUG=1 -DGIRL_CFG -DGIRL_HELPER_PLAYER=\"$(GIRL_HELPER_PLAYER)\" -DGIRL_HELPER_RECORD=\"$(GIRL_HELPER_RECORD)\"
GOAL_LIBS=`pkg-config --libs gtk+-2.0 libgnome-2.0 libxml-2.0 gnome-vfs-2.0 libgnomeui-2.0 libgnomecanvas-2.0`

AM_CFLAGS = $(GOAL_CFLAGS)
AM_LDFLAGS= $(GOAL_LIBS)

bin_PROGRAMS = girl
girl_SOURCES = girl.c girl-gui.c girl-listener.c girl-program.c girl-runners.c girl-streams.c girl-canvas.c girl-station.c

girldir = $(datadir)/girl
girl_DATA = girl-1.0.dtd girl-logo.png girl.png girl.xml

EXTRA_DIST = girl.h girl-station.h girl-gui.h girl-listener.h girl-program.h girl-runners.h girl-streams.h girl-canvas.h girl-1.0.dtd girl-logo.png girl.png girl.xml

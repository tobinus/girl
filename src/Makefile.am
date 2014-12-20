GOAL_CFLAGS=`pkg-config --cflags gtk+-2.0 libgnome-2.0 libxml-2.0 gnome-vfs-2.0 libgnomeui-2.0 libgnomecanvas-2.0` -Wall -g -DHAVE_CONFIG_H -I. -DGNOMELOCALEDIR=\""`pkg-config --variable=prefix libgnomeui-2.0`/share/locale"\" -DDATADIR=\""`pkg-config --variable=prefix libgnomeui-2.0`/share"\" -DGIRL_DEBUG -DGIRL_CFG -DGIRL_HELPER_PLAYER=\"$(GIRL_HELPER_PLAYER)\" -DGIRL_HELPER_RECORD=\"$(GIRL_HELPER_RECORD)\"
GOAL_LIBS=`pkg-config --libs gtk+-2.0 libgnome-2.0 libxml-2.0 gnome-vfs-2.0 libgnomeui-2.0 libgnomecanvas-2.0`

AM_CFLAGS = $(GOAL_CFLAGS)
AM_LDFLAGS= $(GOAL_LIBS)

bin_PROGRAMS = girl
girl_SOURCES = girl.c girl-gui.c girl-listener.c girl-program.c girl-runners.c girl-station.c girl-streams.c girl-canvas.c

girldir = $(datadir)/girl
girl_DATA = listeners.xml programs.xml stations.xml streams.xml girl-0.1.dtd girl-0.2.dtd girl-0.3.dtd girl-0.4.dtd girl-0.5.dtd girl-0.6.dtd girl-0.7.dtd girl-0.8.dtd listener-1.0.dtd listener-1.1.dtd programs-0.1.dtd girl-logo.png girl.png

EXTRA_DIST = girl.h girl-gui.h girl-listener.h girl-program.h girl-runners.h girl-station.h girl-streams.h girl-canvas.h listeners.xml programs.xml stations.xml streams.xml girl-0.1.dtd girl-0.2.dtd girl-0.3.dtd girl-0.4.dtd girl-0.5.dtd girl-0.6.dtd girl-0.7.dtd girl-0.8.dtd listener-1.0.dtd listener-1.1.dtd programs-0.1.dtd girl-logo.png girl.png

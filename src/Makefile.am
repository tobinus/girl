GOAL_CFLAGS=`pkg-config --cflags gtk+-2.0 libgnome-2.0 libxml-2.0 gnome-vfs-2.0 libgnomeui-2.0 libgnomecanvas-2.0` -Wall -g -DHAVE_CONFIG_H -I. -DGNOMELOCALEDIR=\""`pkg-config --variable=prefix libgnomeui-2.0`/share/locale"\" -DDATADIR=\""`pkg-config --variable=prefix libgnomeui-2.0`/share"\" -DDEBUG -DGIRL_CFG
GOAL_LIBS=`pkg-config --libs gtk+-2.0 libgnome-2.0 libxml-2.0 gnome-vfs-2.0 libgnomeui-2.0 libgnomecanvas-2.0`

AM_CFLAGS = $(GOAL_CFLAGS)
AM_LDFLAGS= $(GOAL_LIBS)

bin_PROGRAMS = girl
girl_SOURCES = girl.c girl-gui.c girl-listener.c girl-station.c girl-canvas.c

girldir = $(datadir)/girl
girl_DATA = listeners.xml stations.xml girl-0.1.dtd girl-0.2.dtd girl-0.3.dtd listener-1.0.dtd girl-logo.png girl.png

EXTRA_DIST = girl.h girl-gui.h girl-listener.h girl-station.h girl-canvas.h listeners.xml stations.xml girl-0.1.dtd girl-0.2.dtd girl-0.3.dtd listener-1.0.dtd girl-logo.png girl.png

INCLUDES = 

CFLAGS += @GTK_CFLAGS@ -DPT_DATADIR=\"@PT_DATADIR@\"

bin_PROGRAMS = gpowertweak

gpowertweak_SOURCES = \
	about.c	\
	gtkgui.c \
	tab.c \
	menus.c	\
	tree.c	\
	mainwindow.c \
	loadprofile.c \
	widgetvalues.c \
	filesel.c

gpowertweak_LDADD =\
	../backends/client/libclient.la\
	../libpowertweak/libpowertweak.la 

gpowertweak_LDFLAGS = -export-dynamic @GTK_LIBS@ 

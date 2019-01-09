libexecdir = $(libdir)/powertweak/plugins
CFLAGS += -pedantic @XML_CFLAGS@ -DPLUGINDIR=\"$(libexecdir)/\" -DCOREPLUGINDIR=\"$(libexecdir)/core/\" -DPROFILEDIR=\"@PT_DATADIR@/profile/auto/\" @XML_CFLAGS_MOVED@
LDFLAGS = @XML_LIBS@ -export-dynamic

noinst_LTLIBRARIES = libpowertweak.la

libpowertweak_la_SOURCES = 	\
	libpowertweak.c \
	misc.c \
	register.c \
	plugin.c \
	profile.c \
	config.c \
	values.c \
	socketlib.c \
	logfile.c

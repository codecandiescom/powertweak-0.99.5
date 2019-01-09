INCLUDES = 

libexecdir = $(libdir)/powertweak/plugins
CFLAGS += -DPLUGINDIR=\"$(libexecdir)\" -DPROFILEDIR=\"@PT_DATADIR@/profile/auto/\"
LDFLAGS = @XML_LIBS@ -ldl -export-dynamic

sbin_PROGRAMS = powertweakd

powertweakd_SOURCES = \
	server.c \
	utils.c \
	daemon.c

powertweakd_LDADD =\
	../libpowertweak/libpowertweak.la 

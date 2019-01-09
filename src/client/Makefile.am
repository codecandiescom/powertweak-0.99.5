INCLUDES = 

CFLAGS += -DPT_DATADIR=\"@PT_DATADIR@\"

bin_PROGRAMS = lspowertweak

lspowertweak_SOURCES = \
	lspowertweak.c	

lspowertweak_LDADD =\
	-ldl\
	../backends/client/libclient.la\
	../libpowertweak/libpowertweak.la 

lspowertweak_LDFLAGS = -export-dynamic 

#@GTK_LIBS@

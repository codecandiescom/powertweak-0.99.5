SUBDIRS = misc
INCLUDES =

CFLAGS += @XML_CFLAGS@ -DPT_DATADIR=\"@PT_DATADIR@\" @XML_CFLAGS_MOVED@

libexecdir = $(libdir)/powertweak/plugins

libexec_LTLIBRARIES = libproctweaks.la

libproctweaks_la_SOURCES = \
		proctweaks.c \
		xml.c	\
		proc.c	\
		methods.c

 

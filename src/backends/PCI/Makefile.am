SUBDIRS = .

INCLUDES =

CFLAGS += @XML_CFLAGS@ -DPT_DATADIR=\"@PT_DATADIR@\" @XML_CFLAGS_MOVED@
LDFLAGS =  -no-undefined

libexecdir = $(libdir)/powertweak/plugins

libexec_LTLIBRARIES = libpciinfo.la libpcilatency.la libpcixml.la 

libpciinfo_la_SOURCES = \
		pci.c \
		info-page.c

libpcilatency_la_SOURCES = \
		pci.c \
		latency.c

libpcixml_la_SOURCES = \
		pci.c \
		xml.c

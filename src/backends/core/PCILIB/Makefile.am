libexecdir = $(libdir)/powertweak/plugins/core

libexec_LTLIBRARIES = libpcilib.la

libpcilib_la_SOURCES = \
	proc.c common.c access.c names.c filter.c generic.c \
	header.h internal.h pci.h pciconfig.h pciutils.h \
	plugin.c

libpcilib_la_LIBADD=\
	../../../libpowertweak/libpowertweak.la

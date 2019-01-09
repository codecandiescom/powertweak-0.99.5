SUBDIRS = x86
INCLUDES =

libexecdir = $(libdir)/powertweak/plugins

CFLAGS += 

libexec_LTLIBRARIES = libcputweaks.la


libcputweaks_la_LIBADD = \
	x86/libcpux86.la \
	x86/PowerNow/libpowernow.la

libcputweaks_la_SOURCES = \
	cpu.c

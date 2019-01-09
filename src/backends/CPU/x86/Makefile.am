SUBDIRS = PowerNow
INCLUDES =

CFLAGS += @XML_CFLAGS@ -DPT_DATADIR=\"@PT_DATADIR@\" @XML_CFLAGS_MOVED@

noinst_LTLIBRARIES = libcpux86.la

libcpux86_la_SOURCES = \
	msr.c \
	cpuid.c \
	cpuid_UP.s \
	info-page.c \
	identify.c \
	identify-AMD.c \
	identify-Cyrix.c \
	identify-Intel.c \
	identify-IDT.c \
	identify-Rise.c \
	identify-Transmeta.c \
	xml.c \
	havecpuid.c \
	cputweaks.c \
	mtrr.c

if HAVEGPM
GPML = -lgpm
else
GPML = 
endif

SUBDIRS = tvision

CPPFLAGS += -Itvision/include -I../include -W -Wall \
	-DNO_STREAM -DNO_STREAMABLE \
	-DPT_DATADIR=\"@PT_DATADIR@\" -D_GNU_SOURCE

bin_PROGRAMS = powertweak

powertweak_SOURCES = main.cc fixup.cc \
	toutline.cc  leftwindow.cc rightwindow.cc ptcheckbox.cc \
	ptspinbox.cc pttext.cc scrldlg.cpp scrlgrp.cpp ptradiobutton.cc 
	

powertweak_LDFLAGS = -export-dynamic

powertweak_LDADD = \
	tvision/classes/libturbovision.la\
	$(GPML)\
	-lncurses\
	-ldl\
	../backends/client/libclient.la \
	../libpowertweak/libpowertweak.la

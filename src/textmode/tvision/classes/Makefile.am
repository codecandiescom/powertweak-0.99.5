if HAVEGPM
   GPMD = -DHAVEGPM
else
   GPMD = 
endif

CPPFLAGS += -I../include -DNO_STREAM -DNO_STREAMABLE -DSAFE_CODE $(GPMD)

noinst_LTLIBRARIES = libturbovision.la

libturbovision_la_SOURCES = fpbase.cc tapplica.cc tbackgro.cc \
	tbutton.cc  tchdirdi.cc tcheckbo.cc tclrdisp.cc tcluster.cc \
	tcollect.cc tcommand.cc tdesktop.cc \
	tdialog.cc  tdircoll.cc tdirlist.cc tdisplay.cc tdrawbuf.cc \
	tevent.cc   tfilecol.cc tfiledia.cc \
	tfileinf.cc tfileinp.cc tfilelis.cc tfilterv.cc \
	tframe.cc   tgroup.cc   thistory.cc thistvie.cc \
	thistwin.cc thwmouse.cc tindicat.cc tinputli.cc tlabel.cc   \
	tlistbox.cc tlistvie.cc tmenubar.cc tmenubox.cc \
	tmenuvie.cc tmonosel.cc tmouse.cc   tnscolle.cc tnssorte.cc \
	tobject.cc  tpalette.cc tparamte.cc tpoint.cc   tprogini.cc \
	tprogram.cc tpwobj.cc   tpwreado.cc tpwritte.cc tradiobu.cc \
	trangeva.cc trescoll.cc tscreen.cc  tscrollb.cc \
	tscrolle.cc tscropts.cc tsortedc.cc tsortedl.cc tstatict.cc \
	tstatusd.cc tstatusl.cc tstrinde.cc tstringc.cc tstringl.cc \
	tstrlist.cc tstrmcla.cc tstrmtyp.cc tsubmenu.cc ttermina.cc \
	ttextdev.cc tvalidat.cc \
	tview.cc    tvtext1.cc  twindow.cc  \
	newstr.cc   drivers2.cc names.cc    gkey.cc     \
	histlist.cc bc.cc 	gkey2.cc    msgbox.cc 	misc.cc     \
	ticks.cc

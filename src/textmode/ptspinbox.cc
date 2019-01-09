/*
 *  $Id: ptspinbox.cc,v 1.2 2000/11/22 21:26:58 fenrus Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#define Uses_TEventQueue
#define Uses_TEvent
#define Uses_TProgram
#define Uses_TApplication
#define Uses_TKeys
#define Uses_TRect
#define Uses_TMenuBar
#define Uses_TSubMenu
#define Uses_TMenuItem
#define Uses_TStatusLine
#define Uses_TStatusItem
#define Uses_TStatusDef
#define Uses_TDeskTop
#define Uses_TView
#define Uses_TWindow
#define Uses_TFrame
#define Uses_TScroller
#define Uses_TScrollBar
#define Uses_ScrollDialog
#define Uses_ScrollGroup
#define Uses_TDialog
#define Uses_TButton
#define Uses_TSItem
#define Uses_TCheckBoxes
#define Uses_TRadioButtons
#define Uses_TLabel
#define Uses_TInputLine
#define Uses_TOutline
#define cpOutlineViewer "\x6\x7\x3\x8"
#define Uses_TOutlineViewer
#define Uses_TOutline
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TKeys
#define Uses_TScroller
#define Uses_TPalette
#define Uses_TValidator
#include <assert.h>
#include <tv.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dlg.h"
#include "outline.h"
#include "leftwindow.h"
#include "ptspinbox.h"

extern "C" {
	#include <powertweak.h>
}


TSpinbox::TSpinbox(const TRect& bounds, int maxlen, 
		struct tweak *tweaks)  : TInputLine(bounds,maxlen)
{
	char valuestr[500];
	tweak = tweaks;
	snprintf(valuestr,498,"%i",get_value_int(tweak->TempValue));
	setData(valuestr);
	
}

void TSpinbox::handleEvent(TEvent& event)
{

  int value = 0;
  switch (event.what)
  {
    case evKeyboard:

        switch (ctrlToArrow(event.keyDown.keyCode))
        {
        case kbUp:
		event.keyDown.keyCode = kbShiftTab;
                break;
        case kbDown:
		event.keyDown.keyCode = kbTab;
                break;
	}
  }

  TInputLine::handleEvent(event);
  if (data!=NULL) {
	value = atoi(data);
	set_value_int(tweak->TempValue,value);	
  }
}

void createSpinbox(int *Y,int width,struct tweak *twk, ScrollDialog *view)
{
	TRect bounds1 = TRect(2,*Y,2+width-12,(*Y)+1);
	TRect bounds2 = TRect(2+width-10,*Y,2+width,(*Y)+1);
	TSpinbox *sb;

	(*Y)+=2;

	sb = new TSpinbox(bounds2,16,twk);
	
	TLabel *lab = new TLabel(bounds1,twk->WidgetText,sb);

	twk->Widget = sb;

//	TRangeValidator *val = new TRangeValidator(twk->MinValue,twk->MaxValue);
//	sb->SetValidator(val);
	view->scrollGroup->insert(lab);
	view->scrollGroup->insert(sb);
}

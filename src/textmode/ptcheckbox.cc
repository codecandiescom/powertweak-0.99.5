/*
 *  $Id: ptcheckbox.cc,v 1.1 2000/11/19 18:52:19 davej Exp $
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

#include <assert.h>
#include <tv.h>
#include "dlg.h"
#include "outline.h"
#include "leftwindow.h"
#include "ptcheckbox.h"

extern "C" {
	#include <powertweak.h>
}


TCheckbox::TCheckbox(const TRect& bounds, TSItem *Strings, 
		struct tweak *tweaks)  : TCheckBoxes(bounds,Strings)
{

	tweak = tweaks;	
	if (mark(0)!=get_value_int(tweak->TempValue))
		press(0);
}

void TCheckbox::press(int item) 
{
	TCheckBoxes::press(item);
	set_value_int(tweak->TempValue,mark(item));
}


void TCheckbox::handleEvent(TEvent& event)
{

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

  TCheckBoxes::handleEvent(event);
}

void createCheckbox(int *Y,int width,struct tweak *twk, ScrollDialog *view)
{
	TRect bounds = TRect(2,*Y,2+width,(*Y)+1);

	TSItem *string = new TSItem(twk->WidgetText,NULL);

	(*Y)+=2;

	TCheckbox *cb = new TCheckbox(bounds,string,twk) ;
	view->scrollGroup->insert( cb);
	twk->Widget =cb; 
}

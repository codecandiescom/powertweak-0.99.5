/*
 *  $Id: ptradiobutton.cc,v 1.1 2000/11/19 18:52:19 davej Exp $
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
#include "ptradiobutton.h"

extern "C" {
	#include <powertweak.h>
}

void ActivateNthSubItem(struct tweak *tweak,int N)
{
	int i=0;
	if (tweak==NULL)
		return;
	tweak = tweak->Sub;
	while (tweak!=NULL) {
		if (i==N)
			set_value_int(tweak->TempValue,1);
		else
			set_value_int(tweak->TempValue,0);
		i++;
		tweak=tweak->Next;
			
	}
}


TRadiobutton::TRadiobutton(const TRect& bounds, TSItem *Strings, 
		struct tweak *tweaks)  : TRadioButtons(bounds,Strings)
{

	tweak = tweaks;	
	if (mark(0)!=get_value_int(tweak->TempValue))
		press(0);
}

void TRadiobutton::press(int item) 
{
	TRadioButtons::press(item);
	set_value_int(tweak->TempValue,mark(item));
	ActivateNthSubItem(tweak,item);
}


void TRadiobutton::handleEvent(TEvent& event)
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

  TRadioButtons::handleEvent(event);
}

TSItem *parse_list(struct tweak *tweak, int *count)
{
	if (tweak==NULL)
		return NULL;

	TSItem *string = new TSItem(tweak->WidgetText,
		parse_list(tweak->Next,count));

	(*count)++;

	return string;
} 

void createRadiobutton(int *Y,int width,struct tweak *twk, ScrollDialog *view)
{	
	int listsize=0;
	
	assert(twk->Sub!=NULL);
	TSItem *string = parse_list(twk->Sub,&listsize);
	TRect bounds = TRect(2,*Y,2+width,(*Y)+listsize);

	(*Y)+=1+listsize;

	TRadiobutton *cb = new TRadiobutton(bounds,string,twk) ;
	view->scrollGroup->insert( cb);
	twk->Widget =cb; 
}

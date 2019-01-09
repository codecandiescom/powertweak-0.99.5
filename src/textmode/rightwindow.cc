/*
 *  $Id: rightwindow.cc,v 1.7 2001/10/03 00:15:31 davej Exp $
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
#define Used_TDialog
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
#define Uses_TOutlineViewer
#define Uses_TOutline
#define Uses_TEvent
#define Uses_TDrawBuffer
#define Uses_TKeys
#define Uses_TScroller
#define Uses_TPalette
#define Uses_TDesktop
#define Uses_TView
#define Uses_TProgram

#include <assert.h>
#include <stdio.h>
#include <tv.h>
#include "dlg.h"
#include "outline.h"
#include "rightwindow.h"
#include "ptcheckbox.h"
#include "ptspinbox.h"
#include "pttext.h"
#include "ptradiobutton.h"

extern "C" {
	#include <powertweak.h>
}

extern TWindow *ptactivewindow;


void add_widgets(ScrollDialog *window, struct tweak *tweak, int width, int* Y)
{
	while (tweak!=NULL) {
		switch (tweak->Type)  {	
			case TYPE_CHECKBOX:
				createCheckbox(Y,width,tweak,window);
				break;
			case TYPE_SPINBOX:
			case TYPE_SLIDER:
				createSpinbox(Y,width,tweak,window);
				break;
			case TYPE_INFO_STRING:
			case TYPE_INFO_INT:
			case TYPE_INFO_BOOL:
			case TYPE_TEXT:
				createText(Y,width,tweak,window);
				break;
			case TYPE_FRAME:
			case TYPE_HFRAME:
				add_widgets(window,tweak->Sub,width,Y);
				break;
			case TYPE_RADIO_GROUP:
				createRadiobutton(Y,width,tweak,window);
				break;
		}

		tweak=tweak->Next;
		
	}
}

TWindow *makeWindow(TApplication *myapp,struct tweak *tweak,char *title, int winwidth)
{
	int Y=2,width;
	TRect bound = TRect(myapp->getExtent());
	bound.a.x = bound.b.x-winwidth;bound.b.y-=2;
	ScrollDialog *window = new ScrollDialog(bound,title,sbVertical);
	window->state &= ~sfModal;
	window->flags &= ~wfClose;
	window->flags |= wfGrow;
	window->growMode = gfGrowHiX | gfGrowHiY;

	width = bound.b.x-bound.a.x-6;
	
	add_widgets(window,tweak,width,&Y);

	window->hide();
	window->selectNext(FALSE);
	window->scrollGroup->selectNext(False);
	if (Y<40) 
		Y=40;
	window->scrollGroup->setLimit(0, Y+4);
	myapp->deskTop->insert(window);

	return window;
}

void startWindow(TApplication *myapp, int winwidth)
{
	TRect bound = TRect(myapp->getExtent());
	bound.a.x = bound.b.x-winwidth;bound.b.y-=2;
	TDialog *window = new TDialog(bound,"Powertweak-Linux");
	window->state &= ~sfModal;
	window->flags &= ~wfClose;
	window->flags |= wfGrow;
	window->growMode = gfGrowHiX | gfGrowHiY;

	TRect bounds = TRect(window->getExtent());
	bounds.grow(-1,-1);

	TStaticText *lab = new TStaticText(bounds,
	"Powertweak Linux 0.99.5\n(C) 2000 by Dave Jones and Arjan van de Ven"
	"\n \nPowertweak is a program to tune your kernel and hardware \n"
	"settings for optimal performance."
	"\n \nIf you have a graphical display (X11), you can also use \n"
	"gpowertweak, the GTK version of Powertweak.");

	window->insert(lab);

	ptactivewindow = window;
	
	myapp->deskTop->insert(window);

}


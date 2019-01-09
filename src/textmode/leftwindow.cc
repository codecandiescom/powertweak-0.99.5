/*
 *  $Id: leftwindow.cc,v 1.1 2000/11/19 18:52:19 davej Exp $
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

extern "C" {
	#include <powertweak.h>
}


TLeftWindow::TLeftWindow(const TRect& r, const char *aTitle, short aNumber,struct tweak *tweaks):
	TWindow(r,aTitle,aNumber),
	TWindowInit(&TLeftWindow::initFrame)
{
	TRect bounds = r;

	TRect bounds2 =TRect(r.b.x-1,r.a.y+1,r.b.x,r.b.y-1);

	TScrollBar *vScrollBar = new TScrollBar(bounds2);
	assert(vScrollBar!=NULL);
	vScrollBar->options |= ofPostProcess;
	vScrollBar->growMode = gfGrowHiY;
	insert ( vScrollBar );

	bounds2 = TRect(r.a.x+1,r.b.y-1,r.b.x-1,r.b.y);
	TScrollBar *hScrollBar = new TScrollBar(bounds2);
	assert(hScrollBar!=NULL);
	hScrollBar->options |= ofPostProcess;
	hScrollBar->growMode = gfGrowHiX;
	insert ( hScrollBar );


	flags &= ~wfClose; /* No close button! */
	state &= ~sfShadow; /* No shadow */
	bounds.grow(-1,-1);
	growMode = gfGrowHiY;
	insert( new TOutline(bounds,hScrollBar,vScrollBar,tweaks) );
}

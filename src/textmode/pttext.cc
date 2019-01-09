/*
 *  $Id: pttext.cc,v 1.5 2000/12/23 12:56:30 fenrus Exp $
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



void createText(int *Y,int width,struct tweak *twk, ScrollDialog *view)
{
	TRect bounds = TRect(2,*Y,2+width,(*Y)+1);
	
	char *FullString=NULL;
	char Buffer[50];

	(*Y)+=2;

	if ( (twk->Type==TYPE_INFO_STRING) || (twk->Type==TYPE_TEXT)){	
		if (twk->TempValue.strVal!=NULL) {
			int stringlen;
			stringlen = strlen(twk->WidgetText)+strlen(twk->TempValue.strVal)+5;
			FullString=(char*)malloc(stringlen); 
			assert(FullString!=NULL);
			strncpy(FullString,twk->WidgetText,stringlen-1);	
			strncat(FullString," : ",stringlen-1-strlen(FullString));
			strncat(FullString,twk->TempValue.strVal,stringlen-1-strlen(FullString));
		} else {
		  FullString=strdup(twk->WidgetText);
		}
	}

	if (twk->Type==TYPE_INFO_INT) {	
		int stringlen;
		stringlen = strlen(twk->WidgetText)+20;
		FullString=(char*)malloc(stringlen); 
		assert(FullString!=NULL);
		strncpy(FullString,twk->WidgetText,stringlen-1);	
		strncat(FullString," : ",stringlen-1-strlen(FullString));
		snprintf(Buffer,49,"%i",get_value_int(twk->TempValue));
		strncat(FullString,Buffer,stringlen-1-strlen(FullString));
	}

	if (twk->Type==TYPE_INFO_BOOL) {	
		int stringlen;
		stringlen = strlen(twk->WidgetText)+20;
		FullString=(char*)malloc(stringlen); 
		assert(FullString!=NULL);
		if (get_value_int(twk->TempValue))
			strncpy(FullString,"[X] ",stringlen);
		else
			strncpy(FullString,"[ ] ",stringlen);
 		strncat(FullString,twk->WidgetText,stringlen-1-strlen(FullString));	
	}


	if (FullString==NULL)
		return;	

	TStaticText *lab = new TStaticText(bounds,FullString);
	view->scrollGroup->insert(lab);
	free(FullString);
}

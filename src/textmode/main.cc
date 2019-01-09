/*
 *  $Id: main.cc,v 1.12 2003/04/14 13:19:05 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */


/*

 UGH C++

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
#define Uses_TOutline
#define cpOutlineViewer "\x6\x7\x3\x8"
#define Uses_TOutlineViewer
#define Uses_TOutline
#define Uses_TEvent
#define Uses_TScroller
#define Uses_ScrollDialog
#define Uses_ScrollGroup
#define Uses_TFileDialog

#include <tv.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "dlg.h"
#include "outline.h"
#include "leftwindow.h"
#include "rightwindow.h"
#include "textmode.h"


extern "C" {
	#include <powertweak.h>
	#include <client.h>
}


const int cmOpenProfile	= 200;
const int cmApplyTweaks	= 201;
const int cmSaveTweaks	= 202;
int HaveDaemon = 1;

class TMyApp : public TApplication
{

public:
	TMyApp();
	static TMenuBar *initMenuBar(TRect r);
	static TStatusLine *initStatusLine(TRect r);
	virtual void handleEvent(TEvent& event);

	void newDialog(struct tweak *tweaks, int width);
	void do_profile( char *fileSpec );
};

struct tweak *tweaks;


TMyApp::TMyApp() :
	TProgInit( &TMyApp::initStatusLine,
		       &TMyApp::initMenuBar,
		       &TMyApp::initDeskTop
		     )
{
}

TMenuBar *TMyApp::initMenuBar( TRect r )
{
    r.b.y = r.a.y + 1;    // set bottom line 1 line below top line
    return new TMenuBar( r,
        *new TSubMenu( "~F~ile", kbAltF )+
            *new TMenuItem( "~O~pen profile", cmOpenProfile, kbF3, hcNoContext, "F3" )+
            newLine()+
            *new TMenuItem( "~A~pply changes", cmApplyTweaks, kbF4, hcNoContext, "F4" )+
            *new TMenuItem( "~S~ave config", cmSaveTweaks, kbF5, hcNoContext, "F5" )+
            newLine()+

            *new TMenuItem( "E~x~it", cmQuit, kbF7, hcNoContext, "F7" )


            // new dialog menu added here
        );
}

TStatusLine *TMyApp::initStatusLine(TRect r)
{
    r.a.y = r.b.y - 1;     // move top to 1 line above bottom
    return new TStatusLine( r,
        *new TStatusDef( 0, 0xFFFF ) +
        // set range of help contexts
            *new TStatusItem( "~F10~ Menu", kbF10, cmMenu ) +
            *new TStatusItem( "~F7~ Exit", kbAltX, cmQuit ) +
            // define an item
            *new TStatusItem( "~F6~ Next window", kbF6, cmNext )
            // and another one
        );
}

void TMyApp::newDialog(struct tweak *tweaks,int width)
{
	TRect bounds = getExtent();
	bounds.b.y-=2;
	bounds.b.x=bounds.a.x+width;
	TLeftWindow *pd = new TLeftWindow(bounds,"System",0,tweaks);

	deskTop->insert(pd);
}

/* 
 * GuiToDaemon copies all settings in the GUI to the actual
 * values in the backends/daemon.
 * This is useful for Ok / Apply buttons
 */
void GuiToDaemon (struct tweak *T, int immediate)
{

	if (T == NULL)
		return;

	while (T != NULL) {
		if (T->Sub != NULL)
			GuiToDaemon (T->Sub, immediate);

		if ((T->Type != TYPE_NODE_ONLY) && (T->ChangeValue != NULL)) {
			T->ChangeValue (T, T->TempValue, immediate);
		}

		T = T->Next;
	}
}

void TMyApp::do_profile( char *fileSpec )
{
    char *olddir;

    olddir = get_current_dir_name();
 
    if (olddir==NULL)
	return;


    chdir(PT_DATADIR "/profile/gui/");

    TFileDialog *d= new TFileDialog(fileSpec, "Open a profile", "~N~ame", fdOpenButton,100);
 

		

    if( d != 0 && deskTop->execView( d ) != cmCancel )
        {
        char fileName[PATH_MAX];
        d->getFileName( fileName,PATH_MAX );
	merge_profile(fileName);
	
    }
    chdir(olddir);
    free(olddir);
    destroy( d );
}


void TMyApp::handleEvent(TEvent& event)
{
	TApplication::handleEvent(event);
	if (event.what == evCommand) 
		switch (event.message.command)  {
			case cmSaveTweaks:
				GuiToDaemon(tweaks,0);
				clearEvent(event);
				if (HaveDaemon)
					Save_config();
				else
 					save_tree_to_config(tweaks,"/etc/powertweak/tweaks");
				break;
			case cmApplyTweaks:
				GuiToDaemon(tweaks,1);
				clearEvent(event);
				break;

			case cmOpenProfile:	
				do_profile("*");
				break;

		}
}


void AttachWindows(TApplication *myApp, struct tweak *tweak, int width)
{
	if (tweak==NULL)	
		return;
	if (tweak->Type!=TYPE_TREE)
		return;
	AttachWindows(myApp,tweak->Sub,width);
	AttachWindows(myApp,tweak->Next,width);

	if (tweak->Sub==NULL)
		return;
	if (tweak->Sub->Type==TYPE_TREE)
		return;

	tweak->Widget = makeWindow(myApp,tweak->Sub,tweak->WidgetText,width);

}

void show_error (char *title, char *message)
{
	fprintf (stderr, "%s%s", title, message);
}


extern struct tweak *tweaks;

int main(int argc, char **argv)
{
	struct tweak *localtweaks,*toptweak;
	int width;
	int error;

	if (getuid()!=0) {
		printf ("You need to be root to run this program.\n");
		exit(-1);
	}

	if (argc<0) {
		RegisterTweak(NULL,*argv);
		/* a virtual cookie for whoever allows us to get rid of this */
	}

	error = InitClientBackend(&localtweaks);

	if (localtweaks == NULL)
		LoadPluginsIfNecessary(&HaveDaemon, &show_error, error, &localtweaks);
	if (localtweaks == NULL)
		exit(-1);

	TMyApp myApp;

	FixUpTree(localtweaks);
	toptweak=alloc_tweak(TYPE_NODE_ONLY);assert(toptweak!=NULL);
	toptweak->WidgetText = strdup("Tweaks");
	toptweak->Sub = localtweaks;
	toptweak->Tooltip = (void*)-1;
	TRect rect = myApp.getExtent();
	width = 3*(rect.b.x-rect.a.x)/4;
	AttachWindows(&myApp,localtweaks,width);
	startWindow(&myApp,width);
	myApp.newDialog(toptweak,rect.b.x-rect.a.x-width);

	tweaks = toptweak->Sub;
	myApp.run();

	if (HaveDaemon) {
		CloseClientBackend ();
	} else {
		DestroyAllTweaks (tweaks);
		ShutdownPlugins();
		DestroyInitFuncList();
	}

	return 0;
}

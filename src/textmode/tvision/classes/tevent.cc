/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H�hne to be used for RHIDE.

 *
 *
 */

#include <string.h>
#define Uses_TEventQueue
#define Uses_TEvent
#define Uses_TScreen
#include <tv.h>
#include <gkey.h>

TMouse *TEventQueue::mouse = NULL;
// SET: egcs gets upset if we partially initialize structures and egcs
// 2.91.66 even crash under Linux (not in DOS, but prints "(null)").
TEvent TEventQueue::eventQueue[eventQSize];
TEvent *TEventQueue::eventQHead = TEventQueue::eventQueue;
TEvent *TEventQueue::eventQTail = TEventQueue::eventQueue;
Boolean TEventQueue::mouseIntFlag = False;

ushort TEventQueue::eventCount = 0;

Boolean TEventQueue::mouseEvents  = False;
Boolean TEventQueue::mouseReverse = False;
ushort TEventQueue::doubleDelay = 8;
ushort TEventQueue::repeatDelay = 8;
ushort TEventQueue::autoTicks	= 0;
ushort TEventQueue::autoDelay	= 0;

MouseEventType	TEventQueue::lastMouse;
MouseEventType	TEventQueue::curMouse;
MouseEventType	TEventQueue::downMouse;
ushort TEventQueue::downTicks	= 0;

TEventQueue::TEventQueue()
{
 // SET: Just in case. Note: I can't debug it!
 memset((void *)&eventQueue[0],0,sizeof(TEvent)*eventQSize);
 resume();
}

static int TEventQueue_suspended = 1;

void resume_keyboard();
void suspend_keyboard();

void TEventQueue::resume()
{
	 if (!TEventQueue_suspended) return;
    resume_keyboard();
	 mouseEvents =	False;
    if (!mouse)
      mouse = new TMouse();
	 if( mouse->present()	==	False	)
		  mouse->resume();
	 if( mouse->present()	==	False	)
		  return;
	 mouse->getEvent( curMouse );
	 lastMouse = curMouse;
	 
	 mouseEvents =	True;
	 mouse->setRange( TScreen::getCols()-1,	TScreen::getRows()-1	);
	 TEventQueue_suspended = 0;
}

void TEventQueue::suspend()
{
  if (TEventQueue_suspended) return;
	 if (mouse->present()	==	True)	mouse->suspend();
/* I think here is the right place for clearing the
   buffer */
  TGKey::clear();
  suspend_keyboard();
  TEventQueue_suspended	= 1;
}

TEventQueue::~TEventQueue()
{
 suspend();
 // SET: Destroy the mouse object
 if (mouse)
    delete mouse;
}

void TEventQueue::getMouseEvent(	TEvent& ev )
{
	 if( mouseEvents == True )
		  {

		  getMouseState( ev );

		  if(	ev.mouse.buttons == 0 && lastMouse.buttons != 0	)
				{
				ev.what = evMouseUp;
//            int buttons = lastMouse.buttons;
				lastMouse =	ev.mouse;
//            ev.mouse.buttons = buttons;
				return;
				}

		  if(	ev.mouse.buttons != 0 && lastMouse.buttons == 0	)
				{
				if( ev.mouse.buttons	==	downMouse.buttons	&&
					 ev.mouse.where == downMouse.where &&
					 ev.what	- downTicks	<=	doubleDelay	)
						  ev.mouse.doubleClick = True;

				downMouse =	ev.mouse;
				autoTicks =	downTicks =	ev.what;
				autoDelay =	repeatDelay;
				ev.what = evMouseDown;
				lastMouse =	ev.mouse;
				return;
				}

		  ev.mouse.buttons =	lastMouse.buttons;

		  if(	ev.mouse.where	!=	lastMouse.where )
				{
				ev.what = evMouseMove;
				lastMouse =	ev.mouse;
				return;
				}

		  if(	ev.mouse.buttons != 0 && ev.what	- autoTicks	> autoDelay	)
				{
				autoTicks =	ev.what;
				autoDelay =	1;
				ev.what = evMouseAuto;
				lastMouse =	ev.mouse;
				return;
				}
		  }

	 ev.what	= evNothing;
}

void TEventQueue::getMouseState(	TEvent &	ev	)
{
	 if( eventCount == 0	)
		  {
		  TMouse::getEvent(ev.mouse);
		  ev.what =	TICKS();
		  }
	 else
		  {
	ev	= *eventQHead;
		  if(	++eventQHead >= eventQueue	+ eventQSize )
				eventQHead = eventQueue;
		  eventCount--;
		  }
	 if( mouseReverse	!=	False	&&	ev.mouse.buttons != 0 && ev.mouse.buttons	!=	3 )
		  ev.mouse.buttons ^= 3;
}

void TEvent::getKeyEvent()
{
 if (TGKey::kbhit())
   {
    TGKey::fillTEvent(*this);
    // SET: That's a special case, when the keyboard indicates the event
    // is mouse up it means the keyboard forced an event in the mouse module.
    if (what==evMouseUp)
       getMouseEvent();
   }
 else
    what=evNothing;
}



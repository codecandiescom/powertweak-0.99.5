/*
 *      Turbo Vision - Version 2.0
 *
 *      Copyright (c) 1994 by Borland International
 *      All Rights Reserved.
 *

Modified by Robert H”hne to be used for RHIDE.

 *
 *
 */
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <stdio.h>

#define Uses_TEventQueue
#define Uses_TScreen
#define Uses_TObject
#define Uses_TMouse
#define Uses_TApplication
#include <tv.h>

static TEventQueue *teq = 0;

void TApplication::resume()
{
/*  if (1) save_mouse_state();*/
  TScreen::resume();
  TEventQueue::resume();
/*  if (0) restore_my_mouse_state();*/
  resetIdleTime(); // Don't count this time
}

void TApplication::suspend()
{
/*  if (0) save_my_mouse_state();*/
  TEventQueue::suspend();
  TScreen::suspend();
/*  if (1) restore_mouse_state();*/
}

void initHistory();
void doneHistory();

TApplication::TApplication() :
    TProgInit( &TApplication::initStatusLine,
                  &TApplication::initMenuBar,
                  &TApplication::initDeskTop
                )
{
    if (!teq)
      teq = new TEventQueue();
    initHistory();
}

TApplication::~TApplication()
{
    doneHistory();
    delete teq;
    teq = 0;
}

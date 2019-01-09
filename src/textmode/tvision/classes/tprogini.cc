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

#define Uses_TProgram
#define Uses_TStatusLine
#define Uses_TDeskTop
#define Uses_TMenuBar
#define Uses_TScreen
#include <tv.h>

static TScreen *tsc;

TProgInit::TProgInit( TStatusLine *(*cStatusLine)( TRect ),
                            TMenuBar *(*cMenuBar)( TRect ),
                            TDeskTop *(*cDeskTop )( TRect )
                          ) :
    createStatusLine( cStatusLine ),
    createMenuBar( cMenuBar ),
    createDeskTop( cDeskTop )
{
 tsc=new TScreen();
}

TProgInit::~TProgInit()
{
 delete tsc;
 tsc=0;
}

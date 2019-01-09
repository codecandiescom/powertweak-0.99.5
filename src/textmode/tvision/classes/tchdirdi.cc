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
// SET: Moved the standard headers here because according to DJ
// they can inconditionally declare symbols like NULL
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#define Uses_MsgBox
#define Uses_TChDirDialog
#define Uses_TRect
#define Uses_TInputLine
#define Uses_TLabel
#define Uses_THistory
#define Uses_TScrollBar
#define Uses_TDirListBox
#define Uses_TButton
#define Uses_TEvent
#define Uses_TDirEntry
#define Uses_TDirCollection
#define Uses_TChDirDialog
#define Uses_opstream
#define Uses_ipstream
#define Uses_TStreamableClass
#include <tv.h>

TChDirDialog::TChDirDialog( ushort opts, ushort histId ) :
    TDialog( TRect( 16, 2, 64, 20 ), _("Change Directory") )
    , TWindowInit( &TChDirDialog::initFrame )
{
    char *tmp;
    options |= ofCentered;

    dirInput = new TInputLine( TRect( 3, 3, 30, 4 ), FILENAME_MAX );
    insert( dirInput );
    tmp = _("Directory ~n~ame");
    insert( new TLabel( TRect( 2, 2, 3+cstrlen(tmp), 3 ), tmp, dirInput ));
    insert( new THistory( TRect( 30, 3, 33, 4 ), dirInput, histId ) );

    TScrollBar *sb = new TScrollBar( TRect( 32, 6, 33, 16 ) );
    insert( sb );
    dirList = new TDirListBox( TRect( 3, 6, 32, 16 ), sb );
    insert( dirList );
    tmp = _("Directory ~t~ree");
    insert( new TLabel( TRect( 2, 5, 3+cstrlen(tmp), 6 ), tmp, dirList ) );

    okButton = new TButton( TRect( 35, 6, 45, 8 ), _("~O~K"), cmOK, bfDefault );
    insert( okButton );
    chDirButton = new TButton( TRect( 35, 9, 45, 11 ), _("~C~hdir"), cmChangeDir, bfNormal );
    insert( chDirButton );
    insert( new TButton( TRect( 35, 12, 45, 14 ), _("~R~evert"), cmRevert, bfNormal ) );
    if( (opts & cdHelpButton) != 0 )
        insert( new TButton( TRect( 35, 15, 45, 17 ), _("Help"), cmHelp, bfNormal ) );
    if( (opts & cdNoLoadDir) == 0 )
        setUpDialog();
    selectNext( False );
}

uint32 TChDirDialog::dataSize()
{
    return 0;
}

void TChDirDialog::shutDown()
{
    dirList = 0;
    dirInput = 0;
    okButton = 0;
    chDirButton = 0;
    TDialog::shutDown();
}

void TChDirDialog::getData( void * )
{
}

void TChDirDialog::handleEvent( TEvent& event )
{
    TDialog::handleEvent( event );
    switch( event.what )
        {
        case evCommand:
            {
            char curDir[PATH_MAX];
            switch( event.message.command )
                {
                case cmRevert:
                    getCurDir( curDir );
                    break;
                case cmChangeDir:
                    {
                    TDirEntry *p = dirList->list()->at( dirList->focused );
                    strncpy( curDir, p->dir(), PATH_MAX-1 );
                    if( curDir[strlen(curDir)-1] != DIRSEPARATOR )
                        strncat( curDir, DIRSEPARATOR_,PATH_MAX-1-strlen(curDir) );
                    chdir(curDir);
                    break;
                    }
                case cmDirSelection:
                    chDirButton->makeDefault((Boolean)event.message.infoPtr);
                    return; // Do not use break here ! 
                default:
                    return;
                }
            dirList->newDirectory( curDir );
            strncpy( dirInput->data, curDir, PATH_MAX -1 );
            dirInput->drawView();
            dirList->select();
            clearEvent( event );
            }
        default:
            break;
        }
}

void TChDirDialog::setData( void * )
{
}

void TChDirDialog::setUpDialog()
{
    if( dirList != 0 )
        {
	char curDir[PATH_MAX];
        getCurDir( curDir );
        dirList->newDirectory( curDir );
        if( dirInput != 0 )
            {
            strncpy( dirInput->data, curDir, PATH_MAX -1 );
            dirInput->drawView();
            }
        }
}

Boolean TChDirDialog::valid( ushort command )
{
  if ( command != cmOK )
    return True;
  if( chdir( dirInput->data ) != 0 )
  {
    messageBox( _("Invalid directory"), mfError | mfOKButton );
    return False;
  }
  return True;
}

#if !defined( NO_STREAM )

void TChDirDialog::write( opstream& os )
{
    TDialog::write( os );
    os << dirList << dirInput << okButton << chDirButton;
}

void *TChDirDialog::read( ipstream& is )
{
    TDialog::read( is );
    is >> dirList >> dirInput >> okButton >> chDirButton;
    setUpDialog();
    return this;
}

TStreamable *TChDirDialog::build()
{
    return new TChDirDialog( streamableInit );
}

#endif // NO_STREAM

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
#include <stdio.h>

#define Uses_TFileDialog
#define Uses_MsgBox
#define Uses_TRect
#define Uses_TFileInputLine
#define Uses_TButton
#define Uses_TLabel
#define Uses_TFileList
#define Uses_THistory
#define Uses_TScrollBar
#define Uses_TEvent
#define Uses_TFileInfoPane
#define Uses_opstream
#define Uses_ipstream
#define Uses_TStreamableClass
#include <tv.h>

#include <errno.h>
#include <ctype.h>

// File dialog flags
const int
    ffOpen        = 0x0001,
    ffSaveAs      = 0x0002;

const ushort
    cmOpenDialogOpen    = 100,
    cmOpenDialogReplace = 101;

TFileDialog::TFileDialog( const char *aWildCard,
                          const char *aTitle,
                          const char *inputName,
                          ushort aOptions,
                          uchar histId
                        ) :
    TWindowInit( &TFileDialog::initFrame ),
    TDialog( TRect( 15, 1, 64, 21 ), aTitle ),
    directory( 0 )
{ // SET: Enlarged the window 1 line and added 1 line to the list moving
  // labels 1 line up
    char *tmp;
    options |= ofCentered;
    // SET: Allow it to grow
    growMode = gfGrowAll;
    flags   |= wfGrow | wfZoom;
    strncpy( wildCard, aWildCard,PATH_MAX );

    fileName = new TFileInputLine( TRect( 3, 2, 31, 3 ), PATH_MAX );
    strncpy( fileName->data, wildCard, PATH_MAX );
    fileName->growMode=gfGrowHiX;
    insert( fileName );

    insert( new TLabel( TRect( 2, 1, 3+cstrlen(inputName), 2 ),
                        inputName,
                        fileName
                      ) );
    THistory *his=new THistory(TRect(31,2,34,3),fileName,histId);
    // SET: This and more settings to make it grow nicely
    his->growMode=gfGrowLoX | gfGrowHiX;
    insert(his);
    
    int longNames=TV_HaveLFNs(); // SET
    TScrollBar *sb = longNames ?
                     new TScrollBar( TRect( 34, 5, 35, 16 ) ) :
                     new TScrollBar( TRect( 3, 15, 34, 16 ) );
    insert( sb );
    insert(fileList=new TFileList(TRect(3,5,34,longNames ? 16 : 15),sb));
    fileList->growMode=gfGrowHiX | gfGrowHiY;

    tmp = _("~F~iles");
    insert( new TLabel( TRect( 2, 4, 3+cstrlen(tmp), 5 ), tmp, fileList ) );

    ushort opt = bfDefault;
    TRect r( 35, 2, 46, 4 );
    
    TButton *bt;
    if( (aOptions & fdOpenButton) != 0 )
        {
        bt=new TButton(r,_("~O~pen"),cmFileOpen,opt);
        bt->growMode=gfGrowLoX | gfGrowHiX;
        insert(bt);
        opt = bfNormal;
        r.a.y += 2;
        r.b.y += 2;
        }

    if( (aOptions & fdOKButton) != 0 )
        {
        bt=new TButton(r,_("~O~K"),cmFileOpen,opt);
        bt->growMode=gfGrowLoX | gfGrowHiX;
        insert(bt);
        opt = bfNormal;
        r.a.y += 2;
        r.b.y += 2;
        }

    if( (aOptions & fdSelectButton) != 0 )
        {
        bt=new TButton(r,_("~S~elect"),cmFileSelect,opt);
        bt->growMode=gfGrowLoX | gfGrowHiX;
        insert(bt);
        opt = bfNormal;
        r.a.y += 2;
        r.b.y += 2;
        }

    if( (aOptions & fdReplaceButton) != 0 )
        {
        bt=new TButton(r,_("~R~eplace"),cmFileReplace,opt);
        bt->growMode=gfGrowLoX | gfGrowHiX;
        insert(bt);
        opt = bfNormal;
        r.a.y += 2;
        r.b.y += 2;
        }

    if( (aOptions & fdClearButton) != 0 )
        {
        bt=new TButton(r,_("~C~lear"),cmFileClear,opt);
        bt->growMode=gfGrowLoX | gfGrowHiX;
        insert(bt);
        opt = bfNormal;
        r.a.y += 2;
        r.b.y += 2;
        }

    bt=new TButton(r,_("Cancel"),cmCancel,bfNormal);
    bt->growMode=gfGrowLoX | gfGrowHiX;
    insert(bt);
    r.a.y += 2;
    r.b.y += 2;

    if( (aOptions & fdHelpButton) != 0 )
        {
        bt=new TButton(r,_("~H~elp"),cmHelp,bfNormal);
        bt->growMode=gfGrowLoX | gfGrowHiX;
        insert(bt);
        opt = bfNormal;
        r.a.y += 2;
        r.b.y += 2;
        }

    TFileInfoPane *fip=new TFileInfoPane(TRect(1,16,48,19));
    //fip->growMode=gfGrowHiX | gfGrowHiY;
    fip->growMode=gfGrowHiX | gfGrowHiY | gfGrowLoY;
    insert(fip);

    selectNext( False );
    if( (aOptions & fdNoLoadDir) == 0 )
        readDirectory();
    else
        setUpCurDir(); // SET: We must setup the current directory anyways
}

// SET: Avoid a size smaller than the starting one
void TFileDialog::sizeLimits(TPoint& min, TPoint& max)
{
 TDialog::sizeLimits(min,max);
 min.x=64-15;
 min.y=21-1;
}

TFileDialog::~TFileDialog()
{
    delete[] directory;
}

void TFileDialog::shutDown()
{
    fileName = 0;
    fileList = 0;
    TDialog::shutDown();
}

Boolean relativePath( const char *path )
{
    if( path[0] != EOS && (path[0] == '/' || path[0] == '\\' || 
                           (ucisalpha(path[0]) && path[1] == ':')) )
        return False;
    else
        return True;
}

#if 0 // not longer used
static void noWildChars( char *dest, const char *src )
{
    while( *src != EOS )
        {
        if( *src != '?' && *src != '*' )
            *dest++ = *src;
        src++;
        }
    *dest = EOS;
}
#endif

static void trim( char *dest, const char *src )
{
    const char *end = NULL;
    while( *src != EOS && ucisspace( *src ) )
        src++;
    {
/* when a filename contains spaces (not tested) */
      end = src + strlen(src) - 1;
      if (end < src) end = src;
      else
      {
        while (end > src && ucisspace(*end)) end--;
      }
    }
    while( *src != EOS && src <= end )
        *dest++ = *src++;
    *dest = EOS;
}

void TFileDialog::getFileName( char *s, int bufferlen )
{
  char buf[PATH_MAX];

  trim( buf, fileName->data );
  if ( relativePath( buf ) == True )
  {
    strncpy( buf, directory , PATH_MAX);
    trim( buf + strlen(buf), fileName->data );
  }
  fexpand( buf );
  strncpy( s, buf, bufferlen );
}

void TFileDialog::handleEvent(TEvent& event)
{
    TDialog::handleEvent(event);
    if( event.what == evCommand )
        switch( event.message.command )
            {
            case cmFileOpen:
            case cmFileReplace:
            case cmFileClear:
            case cmFileSelect:
                {
                endModal(event.message.command);
                clearEvent(event);
                }
                break;
            default:
                break;
            }
    // SET: From TV 2.0
    else if( event.what == evBroadcast && event.message.command == cmFileDoubleClicked )
        {
        event.what = evCommand;
        event.message.command = cmOK;
        putEvent( event );
        clearEvent( event );
        }

}

void TFileDialog::readDirectory()
{
    fileList->readDirectory( wildCard );
    setUpCurDir();
}

void TFileDialog::setUpCurDir()
{
    delete[] directory;
    char curDir[PATH_MAX];
    getCurDir( curDir );
    directory = newStr( curDir );
}

void TFileDialog::setData( void *rec )
{
    TDialog::setData( rec );
    if( *(char *)rec != EOS && isWild( (char *)rec ) )
        {
        valid( cmFileInit );
        fileName->select();
        }
}

void TFileDialog::getData( void *rec )
{
    getFileName( (char *)rec, PATH_MAX );
}

Boolean TFileDialog::checkDirectory( const char *str )
{
    if( pathValid( str ) )
        return True;
    else
        {
        messageBox( _("Invalid drive or directory"), mfError | mfOKButton );
        fileName->select();
        return False;
        }
}

Boolean TFileDialog::valid(ushort command)
{
    char fName[PATH_MAX];
    char name[PATH_MAX];
    char dir[PATH_MAX];

    if (!TDialog::valid(command))
        return False;

    if ((command == cmValid) || (command == cmCancel))
        return True;

    getFileName( fName, PATH_MAX );
    if (command != cmFileClear)
    {
        if(isWild(fName))
        {
            expandPath(fName, dir, name);
            if (checkDirectory(dir))
            {
                delete (char *)directory;
                directory = newStr(dir);
                strncpy(wildCard, name,PATH_MAX);
                if (command != cmFileInit)
                    fileList->select();
                fileList->readDirectory(directory, wildCard);
            }
        }
        else if (isDir(fName))
        {
            if (checkDirectory(fName))
            {
                delete (char *)directory;
                strncat(fName, DIRSEPARATOR_,PATH_MAX-1-strlen(fName));
                directory = newStr(fName);
                if (command != cmFileInit)
                    fileList->select();
                fileList->readDirectory(directory, wildCard);
            }
        }
        else if (validFileName(fName))
            return True;
        else
        {
            messageBox( _("Invalid file name."), mfError | mfOKButton );
            return False;
        }
    }
    else
       return True;
    return False; // To shut up GCC's warning
}

#if !defined( NO_STREAM )
void TFileDialog::write( opstream& os )
{
    TDialog::write( os );
    os.writeString( wildCard );
    os << fileName << fileList;
}

void *TFileDialog::read( ipstream& is )
{
    TDialog::read( is );
    is.readString( wildCard, sizeof(wildCard) );
    is >> fileName >> fileList;
    readDirectory();
    return this;
}

TStreamable *TFileDialog::build()
{
    return new TFileDialog( streamableInit );
}
#endif // NO_STREAM


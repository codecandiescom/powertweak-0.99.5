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
#include <string.h>

#define Uses_TDirListBox
#define Uses_TEvent
#define Uses_TDirCollection
#define Uses_TChDirDialog
#define Uses_TDirEntry
#define Uses_TButton
#define Uses_TStreamableClass
#include <tv.h>

TDirListBox::TDirListBox( const TRect& bounds, TScrollBar *aScrollBar ) :
    TListBox( bounds, 1, aScrollBar ),
    cur( 0 )
{
    *dir = EOS;
}

TDirListBox::~TDirListBox()
{ 
   if ( list() )
      destroy( list() );
}

void TDirListBox::getText( char *text, ccIndex item, short maxChars )
{
	strncpy( text, list()->at(item)->text(), maxChars );
	text[maxChars] = '\0';
}

void TDirListBox::handleEvent( TEvent& event )
{
    if( event.what == evMouseDown && event.mouse.doubleClick )
        {
        event.what = evCommand;
        event.message.command = cmChangeDir;
        putEvent( event );
        clearEvent( event );
        }
    else
        TListBox::handleEvent( event );
}

Boolean TDirListBox::isSelected( ccIndex item )
{
    return Boolean( item == cur );
}

void TDirListBox::showDrives( TDirCollection * )
{
}


#if 0
extern "C" unsigned short ffattrib(struct ffblk *);
extern "C" char *ffname(struct ffblk *);
#define A(s) ffattrib(&s)
#define N(s) ffname(&s)
#else
#define A(s) s.ff_attrib
#define N(s) s.ff_name
#endif

#include <dirent.h>
#include <sys/stat.h>

void TDirListBox::showDirs( TDirCollection *dirs )
{
    const int indentSize = 2;
    int indent = indentSize;

    char buf[PATH_MAX*2];
    memset( buf, ' ', sizeof( buf ) );
    char *name = buf + PATH_MAX;

    // The first ramification of the tree
    char *org = name - strlen(pathDir);
    strncpy( org, pathDir,PATH_MAX-strlen(name)+strlen(org));

    char *curDir = dir;
    char *end = dir + 1;
    char hold = *end;
    *end = EOS;         // mark end of drive name
    strncpy( name, curDir, PATH_MAX);
    dirs->insert( new TDirEntry( org, name ) );
    
    *end = hold;        // restore full path
    curDir = end;
    while( (end = strchr( curDir, DIRSEPARATOR )) != 0 )
        {
        *end = EOS;
        strncpy( name, curDir, size_t(end-curDir)+1 );
        name[size_t(end-curDir)] = EOS;
        dirs->insert( new TDirEntry( org - indent, dir ) );
        *end = DIRSEPARATOR;
        curDir = end+1;
        indent += indentSize;
        }

    cur = dirs->getCount() - 1;

    end = strrchr( dir, DIRSEPARATOR );
    char path[PATH_MAX];
    strncpy( path, dir, size_t(end-dir+1) );
    end = path + unsigned(end-dir)+1;
    *end = 0;
    
    Boolean isFirst = True;
    DIR *d=opendir(path);
    if (d)
      {
       struct dirent *ent;
       while( (ent=readdir(d))!=0 )
           {
           struct stat st;
           strncpy(end,ent->d_name,PATH_MAX-1-(end-dir));
           stat(path,&st);
           if( S_ISDIR(st.st_mode) && strcmp(ent->d_name,".")!=0 &&
               strcmp(ent->d_name,"..")!=0)
               {
               if( isFirst )
                   {
                   memcpy( org, firstDir, strlen(firstDir)+1 );
                   isFirst = False;
                   }
               else
                   memcpy( org, middleDir, strlen(middleDir)+1 );
               strncpy( name, ent->d_name ,PATH_MAX);
               strncpy( end, ent->d_name, PATH_MAX-1-(end-dir) );
               dirs->insert( new TDirEntry( org - indent, path ) );
               }
           }
       closedir(d);
      }

    char *p = dirs->at(dirs->getCount()-1)->text();
    char *i = strchr( p, graphics[0] );
    if( i == 0 )
        {
        i = strchr( p, graphics[1] );
        if( i != 0 )
            *i = graphics[0];
        }
    else
        {
        *(i+1) = graphics[2];
        *(i+2) = graphics[2];
        }
}


void TDirListBox::newDirectory( const char *str )
{
    strncpy( dir, str,PATH_MAX );
    TDirCollection *dirs = new TDirCollection( 5, 5 );
    showDirs( dirs );
    newList( dirs );
    focusItem( cur );
}

void TDirListBox::setState( ushort nState, Boolean enable )
{
    TListBox::setState( nState, enable );
    if( (nState & sfFocused) != 0 )
        message(owner, evCommand, cmDirSelection, (void *)enable);
}

#if !defined( NO_STREAM )
TStreamable *TDirListBox::build()
{
    return new TDirListBox( streamableInit );
}
#endif // NO_STREAM



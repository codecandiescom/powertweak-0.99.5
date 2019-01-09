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

#define Uses_fpbase
#define Uses_fpstream
#include <tv.h>

fpstream::fpstream()
{
}

fpstream::fpstream( const char* name, int omode, int prot ) :
        fpbase( name, omode | ios::out | ios::bin, prot )
{
}

fpstream::fpstream( int f ) : fpbase( f )
{
}

fpstream::fpstream(int f, char* b, int len) : fpbase(f, b, len)
{
}

fpstream::~fpstream()
{
}

filebuf *fpstream::rdbuf()
{
    return fpbase::rdbuf();
}

void fpstream::open( const char *name, int omode, int prot )
{
    fpbase::open( name, omode | ios::in | ios::out | ios::bin, prot );
}


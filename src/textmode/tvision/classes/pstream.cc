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
#include <stdlib.h>

#define Uses_TStreamableTypes
#define Uses_TStreamableClass
#define Uses_TStreamable
#define Uses_pstream
#include <tv.h>

pstream::pstream( streambuf *sb )
{
    init( sb );
}

pstream::~pstream()
{
}

void pstream::deInitTypes(void)
{
 if (types)
    delete types;
}

void pstream::initTypes()
{
 if (!types)
   {
    types=new TStreamableTypes;
    atexit(pstream::deInitTypes);
   }
}

int pstream::rdstate() const
{
    return state;
}

int pstream::eof() const
{
    return state & ios::eofbit;
}

int pstream::fail() const
{
    return state & (ios::failbit | ios::badbit);
}

int pstream::bad() const
{
    return state & (ios::badbit);
}

int pstream::good() const
{
    return state == 0;
}

void pstream::clear( int i )
{
    state = (i & 0xFF);
}

void pstream::registerType( TStreamableClass *ts )
{ 
    types->registerType( ts ); 
}

pstream::operator void *() const
{
    return fail() ? 0 : (void *)this;
}

int pstream::operator! () const
{
    return fail();
}

streambuf * pstream::rdbuf() const
{
    return bp;
}

pstream::pstream()
{
}

void pstream::error( StreamableError )
{
    abort();
}

void pstream::error( StreamableError, const TStreamable& )
{
    abort();
}

void pstream::init( streambuf *sbp )
{
    state = 0;
    bp = sbp;
}

void pstream::setstate( int b )
{
    state |= (b&0xFF);
}

TStreamableTypes * pstream::types;

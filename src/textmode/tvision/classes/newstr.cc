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

#include <tv.h>

#include <string.h>

char *newStr( const char *s )
{
    int stringlen;
    if( s == 0 )
        return 0;

    stringlen =  strlen(s)+1 ;
    char *temp = new char[stringlen];
    strncpy( temp, s,stringlen );
    return temp;
}

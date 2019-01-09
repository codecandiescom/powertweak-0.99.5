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

#if !defined( __CONFIG_H )
#define __CONFIG_H

#include <limits.h>

const int eventQSize = 16;
const int maxCollectionSize = UINT_MAX/sizeof( void * );

// May be I'll remove that sometimes
const int maxViewWidth = 1024;

const int maxFindStrLen    = 80;
const int maxReplaceStrLen = 80;

#define interrupt
#define huge
#define far
#define near
#define _FAR
#define __LARGE__
#define FP_OFF(x) (int)(x)
#define movmem(src,dst,size) memmove(dst,src,size)
#define heapcheck() 1
#define ltoa itoa
#define DIRSEPARATOR '/'
#define DIRSEPARATOR_ "/"
#define PATHSEPARATOR ':'
#define PATHSEPARATOR_ ":"

int getcurdir(int, char *);
extern "C" long __filelength(int);
#define filelength __filelength

#if 0
/* I do not longer use them. I use now only PATH_MAX */
#define USE_LFN 1
#define MAXLFN 256 /* length of a filename or ext */
#define MAXLFNPATH 512
#endif

#include <intl.h>
#include <sys/types.h>

/* These are only symbolic constants, which are defined on DJGPP
   in <dir.h> */

#define FA_ARCH   0x01
#define FA_DIREC  0x02
#define FA_RDONLY 0x04

#endif	// __CONFIG_H

/*
 *	$Id: common.c,v 1.1 2001/10/06 09:23:21 davej Exp $
 *
 *	Linux PCI Utilities -- Common Functions
 *
 *	Copyright (c) 1997--1999 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>

#include <pciutils.h>

void __attribute__ ((noreturn))
    die (char *msg, ...)
{
	va_list args;

	va_start (args, msg);
	fputs ("powertweak: ", stderr);
	vfprintf (stderr, msg, args);
	fputc ('\n', stderr);
	exit (1);
}

void *xmalloc (unsigned int howmuch)
{
	void *p = malloc (howmuch);
	if (!p)
		die ("Unable to allocate %d bytes of memory", howmuch);
	return p;
}

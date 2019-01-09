/*
 *	$Id: pciutils.h,v 1.1 2001/10/06 09:23:21 davej Exp $
 *
 *	Linux PCI Utilities -- Declarations
 *
 *	Copyright (c) 1997--1999 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#include "pci.h"

void __attribute__((noreturn)) die(char *msg, ...);
void *xmalloc(unsigned int howmuch);

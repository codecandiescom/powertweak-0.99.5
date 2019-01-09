/*
 *  $Id: libxmlcompat.h,v 1.3 2000/09/24 18:27:28 fenrus Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */
#ifndef _INCLUDE_GUARD_LIBXMLCOMPAT_H
#define _INCLUDE_GUARD_LIBXMLCOMPAT_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <tree.h> /* libxml */

/* Check if this is 1.8 or 2.x */
#ifdef LIBXML_VERSION_NUMBER

#if ( LIBXML_VERSION_NUMBER < 10808)
#define xmlChildrenNode childs
#define xmlRootNode root
#define LIBXML_TEST_VERSION
#endif

#endif

#endif

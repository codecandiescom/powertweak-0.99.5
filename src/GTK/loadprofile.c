/*
 *  $Id: loadprofile.c,v 1.9 2001/05/28 02:41:00 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 *  Routines used to create the GTK GUI
 */

#include <stdio.h>
#include <stdlib.h>
#include <gtk/gtk.h>
#include <powertweak.h>
#include <config.h>

#include "gtkgui.h"
#include "filesel.h"


void LoadFile (char *ProfileFilename)
{
	g_free (ProfileFilename);
}


void LoadProfile (void)
{
	GetFilename ("Load profile", PT_DATADIR "/profile/gui/", LoadFile);
}

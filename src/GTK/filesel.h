/*
 *  $Id: filesel.h,v 1.3 2000/12/13 13:14:41 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 *  Routines used to create the GTK GUI
 */

#include <gtk/gtk.h>

typedef struct {
	void (*func) (gchar *);
	GtkWidget *filesel;
} typFileSelectionData;


extern char *sFilename;
void GetFilename (char *sTitle, char *path, void (*callback) (char *));

/*
 *  $Id: gtkgui.c,v 1.41 2003/07/08 11:42:12 svenud Exp $
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
#include <string.h>
#include <unistd.h>

#include "gtkgui.h"
#include <client.h>
#include <client-utils.h>

int WindowWidth;
int WindowHeight;
int LeftWindowHeight;
int RightWindowHeight;

int HaveDaemon = 1;

struct tweak *tweaks;


void want_config_saved()
{
	if (HaveDaemon)
		Save_config(); /* client backend */
	else
		save_tree_to_config (tweaks, "/etc/powertweak/tweaks");
}

/* 
 * GuiToDaemon copies all settings in the GUI to the actual
 * values in the backends/daemon.
 * This is useful for Ok / Apply buttons
 */
void GuiToDaemon (struct tweak *T, int immediate)
{

	if (T == NULL)
		return;

	while (T != NULL) {
		if (T->Sub != NULL)
			GuiToDaemon (T->Sub, immediate);

		if ((T->Type != TYPE_NODE_ONLY) && (T->ChangeValue != NULL)) {
			T->ChangeValue (T, T->TempValue, immediate);
		}

		T = T->Next;
	}
}


void RefreshValuesFromDaemon (struct tweak *T)
{
	if (T == NULL)
		return;

	while (T != NULL) {
		if (T->Sub != NULL)
			RefreshValuesFromDaemon (T->Sub);

		if ((T->Type != TYPE_NODE_ONLY) && (T->GetValue != NULL)) {
			T->GetValueRaw (T);
		}

		T = T->Next;
	}
}

char *programname;

int main (int argc, char **argv)
{
	struct tweak *localtweaks;
	int error=0;

	gtk_init (&argc, &argv);

	if (getuid()!=0) {
		show_error ("Error!", "gpowertweak only runs as root.");
		exit (-1);
	}

	programname = strdup("Powertweak " VERSION);

	error = InitClientBackend (&localtweaks);
	if (localtweaks == NULL)
		LoadPluginsIfNecessary(&HaveDaemon, &show_error, error, &localtweaks);

	/* If all went ok, do the main stuff. */
	if (localtweaks != NULL) {
		MainWindow (localtweaks, HaveDaemon);
		gtk_main ();
	}

	/* Shut down gracefully. */
	if (HaveDaemon) {
		CloseClientBackend ();
	} else {
		DestroyAllTweaks (localtweaks);
		ShutdownPlugins();
		DestroyInitFuncList();
	}
	
	free(programname);
	return 0;
}

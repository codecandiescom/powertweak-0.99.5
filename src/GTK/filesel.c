/*
 *  $Id: filesel.c,v 1.4 2001/08/20 00:11:34 davej Exp $
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

char *sFilename=NULL;

static void FileOk (pt_unused GtkWidget *w, gpointer data)
{
	char *sTempFile;
	typFileSelectionData *localdata;
	GtkWidget *filesel;

	localdata = (typFileSelectionData *) data;
	filesel = localdata->filesel;

	sTempFile = gtk_file_selection_get_filename(
		GTK_FILE_SELECTION (filesel));

	if (sFilename)
		g_free (sFilename);

	sFilename = g_strdup (sTempFile);
	(*(localdata->func)) (sFilename);

	gtk_widget_destroy (filesel);
}


static void destroy (GtkWidget *widget, gpointer *data)
{
	gtk_grab_remove (widget);
	g_free (data);
}


void GetFilename (char *title, char *path, void (*callback) (char *))
{
	GtkWidget *filesel;
	typFileSelectionData *data;

	/* If value is there from last time we used the dialog,
	   we free the memory. */
	if (sFilename) {
		free (sFilename);
		sFilename = NULL;
	}

	filesel = gtk_file_selection_new (title);

	data = g_malloc (sizeof (typFileSelectionData));
	data->func = callback;
	data->filesel = filesel;

	gtk_file_selection_set_filename(GTK_FILE_SELECTION(
		filesel), path);

	gtk_signal_connect (GTK_OBJECT (filesel), "destroy",
		(GtkSignalFunc) destroy, data);

	gtk_signal_connect (GTK_OBJECT (GTK_FILE_SELECTION
		(filesel)->ok_button), "clicked",
		(GtkSignalFunc) FileOk, data);

	gtk_signal_connect_object (GTK_OBJECT (GTK_FILE_SELECTION
		(filesel)->cancel_button), "clicked",
		(GtkSignalFunc) gtk_widget_destroy, (gpointer) filesel);

	gtk_widget_show (filesel);
	gtk_grab_add (filesel);
}

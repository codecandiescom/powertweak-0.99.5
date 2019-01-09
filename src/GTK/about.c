/*
 *  $Id: about.c,v 1.9 2003/04/14 13:18:57 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <powertweak.h>
#include <config.h>
#include <gtk/gtk.h>

#define ALERT 0
#define ALERT_QUIT 1	/* quit app after Ok is clicked. */


void CloseDialog (GtkWidget pt_unused *widget, gpointer data)
{
	gtk_widget_destroy (GTK_WIDGET (data));
}


void ClosingDialog (GtkWidget * widget, gpointer pt_unused data)
{
	gtk_grab_remove (GTK_WIDGET (widget));
}

static gint exit_app (GtkWidget pt_unused *widget, gpointer pt_unused data)
{
	gtk_main_quit();
	return (TRUE);
}


GtkWidget * show_popup (char *title, char *message, int type)
{
	static GtkWidget *label;
	GtkWidget *dialog;
	GtkWidget *button;

	dialog = gtk_dialog_new ();

	gtk_signal_connect (GTK_OBJECT (dialog), "destroy", GTK_SIGNAL_FUNC (ClosingDialog), &dialog);

	gtk_window_set_title (GTK_WINDOW (dialog), title);
	gtk_container_border_width (GTK_CONTAINER (dialog), 5);

	label = gtk_label_new (message);

	gtk_misc_set_padding (GTK_MISC (label), 10, 10);
	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->vbox), label, TRUE, TRUE, 0);
	gtk_widget_show (label);

	button = gtk_button_new_with_label ("Ok");
	if (type == ALERT_QUIT)
		gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (exit_app), dialog);
	else
		gtk_signal_connect (GTK_OBJECT (button), "clicked", GTK_SIGNAL_FUNC (CloseDialog), dialog);

	GTK_WIDGET_SET_FLAGS (button, GTK_CAN_DEFAULT);

	gtk_box_pack_start (GTK_BOX (GTK_DIALOG (dialog)->action_area), button, TRUE, TRUE, 0);
	gtk_widget_grab_default (button);
	gtk_widget_show (button);
	gtk_widget_show (dialog);
	gtk_grab_add (dialog);
	return dialog;
}

void NotImplementedYet()
{
	GtkWidget *dialog;

	dialog = show_popup ("PowerTweak", "Not Implemented yet :)", ALERT);
	gtk_main();
}

void show_error (char *title, char *message)
{
	GtkWidget *dialog;

	dialog = show_popup (title, message, ALERT_QUIT);
	gtk_main();
}

void show_about (void)
{
	GtkWidget *dialog;
	char about_message[] = "Powertweak-Linux\nVersion " VERSION "\
Project homepage: http://www.powertweak.org\n\
Please report any bugs to powertweak-linux@lists.sourceforge.net";

	dialog = show_popup ("About", about_message, ALERT);
}

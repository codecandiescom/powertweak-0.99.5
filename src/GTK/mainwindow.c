/*
 *  $Id: mainwindow.c,v 1.40 2003/07/08 14:22:00 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2001 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <powertweak.h>
#include <config.h>
#include <gtk/gtk.h>
#include <string.h>

#include "gtkgui.h"
#include <client.h>

extern void want_config_saved();

/*SVEN - can we use gnome themes?*/

GtkWidget *rightpane;
GtkWidget *MainWin=NULL;
GtkTooltips *tooltips;

extern char* programname;

static gint destroyapp (GtkWidget pt_unused * widget, gpointer pt_unused data)
{
	gtk_main_quit ();
	return (TRUE);
}

static gint refreshfunction (GtkWidget pt_unused * widget, gpointer pt_unused data)
{
	RefreshValuesFromDaemon (data);
	update_GTK_tree_values(data);
	return (TRUE);
}
static gint applyfunction (GtkWidget pt_unused * widget, gpointer data)
{
	/*SVEN - we should bring up a windoww that shows the user what parameters they are
	   changing and what to. (and give the option to cancel...)
	*/
	GuiToDaemon (data, TRUE);
	update_GTK_tree_values(data);
	return (TRUE);

}

static gint savefunction (GtkWidget pt_unused * widget, gpointer data)
{
	GuiToDaemon (data, FALSE);
	want_config_saved();
	return (TRUE);
}


static void hide_widget (GtkWidget * widget, gpointer pt_unused data)
{
	gtk_widget_hide (widget);
}


void hide_all_tabs (void)
{
	gtk_container_foreach (GTK_CONTAINER (rightpane), GTK_SIGNAL_FUNC (hide_widget), NULL);
}


static void Initial_pane (GtkWidget * widget)
{
	GtkWidget *text;

	char *t = "Powertweak " VERSION "\n (C) 2003 by Dave Jones, Arjan van de Ven"
		"\n\nPowertweak is a program to tune your kernel and hardware settings\n"
		"for optimal performance.\n"
		"On the left side of this window are the different categories for the \n"
		"settings that can be tuned.";

	text = gtk_text_new (NULL, NULL);

	gtk_text_insert (GTK_TEXT (text), NULL, NULL, NULL, t, strlen (t));

	gtk_text_set_editable (GTK_TEXT (text), FALSE);

	gtk_box_pack_start (GTK_BOX (widget), text, TRUE, TRUE, 0);
	gtk_widget_show (text);
}


GtkWidget *MainWindow (struct tweak *tweaks, int haveDaemon)
{
	GtkWidget *mainwindow;
	GtkWidget *vbox;
	GtkWidget *Menubar;
	GtkWidget *vbox_right;
	GtkWidget *lframe;
	GtkWidget *hpane;
	GtkWidget *buttonbox;
	GtkWidget *scrolled_win;
	GtkWidget *tree;
	GtkWidget *separator;
	GtkWidget *applybutton, *savebutton, *quitbutton, *refreshbutton = NULL;


	/* First the main window and its handlers */
	mainwindow = gtk_window_new (GTK_WINDOW_TOPLEVEL);
	MainWin = mainwindow;
	g_assert (mainwindow != NULL);

	gtk_signal_connect (GTK_OBJECT (mainwindow), "delete_event", GTK_SIGNAL_FUNC (destroyapp), NULL);
	gtk_window_set_title (GTK_WINDOW (mainwindow), programname);

	/* 640x400 + 640x480 */
	switch (gdk_screen_width()) {
	case 640:
		WindowWidth = 620;
		if (gdk_screen_height()==400)
			WindowHeight = 400;
		else
			WindowHeight = 430;
		break;
	case 800:
		WindowWidth = 750;
		WindowHeight = 450;
		break;
	default:	/* Unknown resolution. */
		if (gdk_screen_width() > 800) {
			WindowWidth = 900;
			WindowHeight = 700;
		} else {
			WindowWidth = ((gdk_screen_width() / 10 ) * 9);
			WindowHeight = ((gdk_screen_height() / 10 ) * 9);
		}
		break;
	}


	gtk_widget_set_usize (mainwindow, WindowWidth, WindowHeight);

	//FIXME: This should be proportional to resolution.
	LeftWindowHeight = WindowHeight - 100;
	RightWindowHeight = WindowHeight - 70;

	tooltips = gtk_tooltips_new();


	/* Divide first in 3 vertical areas, later do the middle box in 2 horizontal panes:

	 *  +------+
	 *  +------+
	 *  | |    |
	 *  | |    |
	 *  | |    |
	 *  +------+
	 *  | Ok C | 
	 *  +------+
	 */

	vbox = gtk_vbox_new (FALSE, 0);

	/* Build the pull down menus. */
	Menubar = CreateMenus (mainwindow);

	/* Now the center horizontal pane */
	hpane = gtk_hpaned_new ();
	gtk_paned_handle_size (GTK_PANED (hpane), 10);
	gtk_paned_gutter_size (GTK_PANED (hpane), 10);

	scrolled_win = gtk_scrolled_window_new (NULL, NULL);
	gtk_widget_set_usize (scrolled_win, (WindowWidth / 4), LeftWindowHeight);

	gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolled_win), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

	rightpane = gtk_hbox_new (FALSE, 0);
	
	tree = BuildTree (tweaks);

	Initial_pane (rightpane);

	gtk_container_add (GTK_CONTAINER (scrolled_win), tree);
	gtk_widget_show (tree);
	gtk_widget_show (scrolled_win);


	lframe = gtk_frame_new ("System.");
	gtk_container_set_border_width (GTK_CONTAINER (lframe), 10);
	gtk_frame_set_shadow_type (GTK_FRAME (lframe), GTK_SHADOW_IN);
	gtk_frame_set_label_align (GTK_FRAME (lframe), 0, 0);
	gtk_widget_show (lframe);
	gtk_container_add (GTK_CONTAINER (lframe), scrolled_win);
	gtk_paned_add1 (GTK_PANED (hpane), lframe);

	/* Right hand side of the GUI */
	vbox_right = gtk_vbox_new (FALSE, 0);

	gtk_container_add (GTK_CONTAINER (vbox_right), rightpane);
	gtk_widget_show (rightpane);
	gtk_paned_add2 (GTK_PANED (hpane), vbox_right);



	gtk_widget_show (vbox_right);


	separator = gtk_hseparator_new ();
	
	/* Buttons at the bottom of the GUI. */
	if (haveDaemon == 1)
	{/*TODO - fix it!! does not work for local mode*/
		refreshbutton = gtk_button_new_with_label ("Refresh / Reset");
	}
	applybutton = gtk_button_new_with_label ("Apply");
	savebutton = gtk_button_new_with_label ("Save");
	quitbutton = gtk_button_new_with_label ("Quit");
	if (haveDaemon == 1)
	{/*TODO - fix it!! does not work for local mode*/
		gtk_signal_connect (GTK_OBJECT (refreshbutton), "clicked", GTK_SIGNAL_FUNC (refreshfunction), tweaks);

	}
	gtk_signal_connect (GTK_OBJECT (applybutton), "clicked", GTK_SIGNAL_FUNC (applyfunction), tweaks);
	gtk_signal_connect (GTK_OBJECT (savebutton), "clicked", GTK_SIGNAL_FUNC (savefunction), tweaks);
	gtk_signal_connect (GTK_OBJECT (quitbutton), "clicked", GTK_SIGNAL_FUNC (destroyapp), NULL);
	gtk_widget_show (refreshbutton);
	gtk_widget_show (applybutton);
	gtk_widget_show (savebutton);
	gtk_widget_show (quitbutton);

	/* Pack the buttons into the hbox */
	buttonbox = gtk_hbox_new (FALSE, 0);
	gtk_container_border_width (GTK_CONTAINER (buttonbox), 20);
	gtk_box_pack_start (GTK_BOX (buttonbox), refreshbutton, TRUE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (buttonbox), applybutton, TRUE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (buttonbox), savebutton, TRUE, FALSE, 0);
	gtk_box_pack_start (GTK_BOX (buttonbox), quitbutton, TRUE, FALSE, 0);


	/* Pack everything into the vbox. */
	gtk_box_pack_start (GTK_BOX (vbox), Menubar, FALSE, FALSE, 0);
	gtk_widget_show (Menubar);
	gtk_box_pack_start (GTK_BOX (vbox), hpane, TRUE, TRUE, 0);
	gtk_widget_show (hpane);
	gtk_box_pack_start (GTK_BOX (vbox), separator, FALSE, FALSE, 0);
	gtk_widget_show (separator);
	gtk_box_pack_start (GTK_BOX (vbox), buttonbox, FALSE, FALSE, 0);
	gtk_widget_show (buttonbox);


	gtk_container_add (GTK_CONTAINER (mainwindow), vbox);
	gtk_widget_show (vbox);
	gtk_widget_show (mainwindow);

	return mainwindow;
}

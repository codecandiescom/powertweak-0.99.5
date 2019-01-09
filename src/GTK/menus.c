/*
 *  $Id: menus.c,v 1.14 2003/04/14 13:18:59 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 * Routines for creating/handling GTK menus.
 */

#include <stdio.h>
#include <gtk/gtk.h>
#include "gtkgui.h"
#include "powertweak.h"

static void QuitApp (gpointer pt_unused callback_data, guint pt_unused callback_action, GtkWidget pt_unused * widget)
{
	gtk_main_quit ();
}


static void ShowMenu (gpointer pt_unused callback_data, pt_unused guint callback_action, pt_unused GtkWidget *widget)
{
}

// FIXME: Placeholder routines. Will go away as soon as these
// routines get written somewhere else.
static void LoadStateConfig (gpointer pt_unused callback_data, pt_unused guint callback_action, pt_unused GtkWidget *widget)
{
	NotImplementedYet();
}

static void LoadStateCurrent (gpointer pt_unused callback_data, pt_unused guint callback_action, pt_unused GtkWidget *widget)
{
	NotImplementedYet();
}

GtkWidget *CreateMenus (GtkWidget * window)
{
	GtkAccelGroup *accel_group;
	GtkItemFactory *item_factory;

	static GtkItemFactoryEntry menu_items[] = {
		{"/_File", NULL, 0, 0, "<Branch>"},
		{"/File/tearoff1", NULL, ShowMenu, 0, "<Tearoff>"},
		{"/File/Load profile", NULL, LoadProfile, 0, 0},
		{"/File/Load state", NULL, NULL, 0, "<Branch>"},
		{"/File/Load state/Configuration file", NULL, LoadStateConfig, 0, 0},
		{"/File/Load state/Current system settings", NULL, LoadStateCurrent, 0, 0},
		{"/File/Sep1", NULL, ShowMenu, 0, "<Separator>"},
		{"/File/E_xit", "<control>Q", QuitApp, 0, 0},

		{"/_Help", NULL, 0, 0, "<LastBranch>"},
		{"/Help/_About", NULL, show_about, 0, 0},
	};

	static int nmenu_items = sizeof (menu_items) / sizeof (menu_items[0]);

	accel_group = gtk_accel_group_new ();
	item_factory = gtk_item_factory_new (GTK_TYPE_MENU_BAR, "<blah>", accel_group);
	gtk_item_factory_create_items (item_factory, nmenu_items, menu_items, NULL);
	gtk_accel_group_attach (accel_group, GTK_OBJECT (window));
	return (gtk_item_factory_get_widget (item_factory, "<blah>"));
}

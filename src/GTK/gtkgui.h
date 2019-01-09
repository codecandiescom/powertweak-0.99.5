/*
 *  $Id: gtkgui.h,v 1.15 2003/07/08 11:42:12 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#ifndef _INCLUDE_GUARD_GTKGUI_H_
#define _INCLUDE_GUARD_GTKGUI_H_

#include "powertweak.h"

GtkWidget *MainWindow(struct tweak *tweak, int haveDaemon);
GtkWidget *TabWidget (struct tweak *tweak);
GtkWidget *BuildTree (struct tweak *tweak);
GtkWidget *CreateMenus (GtkWidget *window);
void GuiToDaemon (struct tweak *T, int immediate);
void RefreshValuesFromDaemon(struct tweak *T);	
void hide_all_tabs(void);
void show_about(void);
void LoadProfile(void);

extern GtkWidget *MainWin;
extern int WindowWidth;
extern int WindowHeight;
extern int LeftWindowHeight;
extern int RightWindowHeight;

extern void show_error (char *title, char *message);
extern void NotImplementedYet();
extern void update_GTK_tree_values(struct tweak *tweak);

#endif

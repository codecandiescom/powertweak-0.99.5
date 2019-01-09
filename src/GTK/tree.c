/*
 *  $Id: tree.c,v 1.10 2001/08/20 00:11:34 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 * Routines to create/manage the ctree on the left of the GTK GUI.
 */

#include <stdio.h>
#include <stdlib.h>
#include <powertweak.h>
#include <gtk/gtk.h>

#include "gtkgui.h"

extern GtkWidget *rightpane;

static void callback_selecttreeitem (GtkCTree * ctree, GtkCTreeNode * node, gint pt_unused column,
				     pt_unused gpointer data)
{
	struct tweak *tweak;

	hide_all_tabs ();

	tweak = gtk_ctree_node_get_row_data (GTK_CTREE (ctree), node);
	if (tweak != NULL) {

		if (tweak->Widget != NULL)
			gtk_widget_show (GTK_WIDGET (tweak->Widget));
	}
}

static void OneTreeLevel (GtkWidget * tree, GtkCTreeNode * topleaf, struct tweak *tweak)
{
	struct tweak *current;
	current = tweak;
	while (current != NULL) {
		GtkCTreeNode *leaf;
		struct tweak *l2tweaks;
		gboolean isleaf;

		l2tweaks = current->Sub;
		isleaf = FALSE;

		if ((l2tweaks == NULL) || (l2tweaks->Type != TYPE_TREE))
			isleaf = TRUE;


		leaf = gtk_ctree_insert_node (GTK_CTREE (tree),
					      topleaf, NULL, &(current->WidgetText), 0,
					      NULL, NULL, NULL, NULL, isleaf, FALSE);
		gtk_ctree_node_set_selectable (GTK_CTREE (tree), leaf, isleaf);


		if (!isleaf)
			OneTreeLevel (tree, leaf, l2tweaks);

		if ((l2tweaks != NULL) && (isleaf)) {
			GtkWidget *tab;
			tab = TabWidget (l2tweaks);
			current->Widget = tab;
			gtk_box_pack_start (GTK_BOX (rightpane), tab, TRUE, TRUE, 0);
			gtk_ctree_node_set_row_data_full (GTK_CTREE (tree), leaf, current, NULL);
		}

		current = current->Next;
	}
}


GtkWidget *BuildTree (struct tweak *tweaks)
{
	GtkWidget *tree;

	tree = gtk_ctree_new (1, 0);
	gtk_clist_set_row_height (GTK_CLIST (tree), 15);
	gtk_clist_set_column_width (GTK_CLIST (tree), 0, 150);
	gtk_clist_set_column_auto_resize (GTK_CLIST (tree), 0, TRUE);
	gtk_clist_set_selection_mode (GTK_CLIST (tree), GTK_SELECTION_BROWSE);
	gtk_ctree_set_line_style (GTK_CTREE (tree), GTK_CTREE_LINES_DOTTED);
	gtk_ctree_set_expander_style (GTK_CTREE (tree), GTK_CTREE_EXPANDER_SQUARE);
	gtk_ctree_set_indent (GTK_CTREE (tree), 15);

	gtk_signal_connect (GTK_OBJECT (tree), "tree_select_row", GTK_SIGNAL_FUNC (callback_selecttreeitem), NULL);

	OneTreeLevel (tree, NULL, tweaks);

	return (tree);
}

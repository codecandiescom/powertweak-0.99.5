/*
 *  $Id: config.c,v 1.26 2003/04/07 11:54:34 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 * Configuration file code
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <powertweak.h>


 /* 
  * This function is a helper-function for save_tree_to_config,
  * it does the recursion needed to save a tree.
  */

static void save_subtree_to_file (struct tweak *tree, FILE * file)
{
	if (tree == NULL)
		return;

	while (tree != NULL) {

		if (tree->Type == TYPE_TREE)
			fprintf (file, "# %s \n",tree->WidgetText);

		/*FIXME: Add code to compare origvalue with current value.*/
		if ((tree->ConfigName != NULL) && (tree->GetValue != NULL) &&(tree->ConfigName[0]!='!'))
			fprintf (file, "%s = %i \n", tree->ConfigName, get_value_int(tree->GetValue (tree)));

		if (tree->Sub != NULL)
			save_subtree_to_file (tree->Sub, file);

		tree = tree->Next;
	}
}

 /*
  * This function saves a tweaktree to a configuration file.
  * Returns 0 on success, negative values on failure
  */
int save_tree_to_config (struct tweak *tree, char *filename)
{
	FILE *file;

	file = fopen (filename, "w");
	if (file == NULL)
		return -1;

	save_subtree_to_file (tree, file);

	if (fclose (file)==-EBADF) {
		printf("Oops. You did something we didn't think of.\n");
	}

	return 0;
}


/*
 * Walks a tree, updating its values to whatever the current
 * system state says they are in.
 */
void update_tree_values (struct tweak *tree)
{
	if (tree == NULL)
		return;

	while (tree!= NULL) {
		if (tree->GetValueRaw != NULL)
			tree->GetValueRaw (tree);
		if (tree->Sub != NULL)
			update_tree_values (tree->Sub);

		tree = tree->Next;
	}
}


struct stringlist;

struct stringlist {
	struct stringlist *Next;
	char *Key;
	value_t Value;
};


static struct stringlist *match_and_remove (struct stringlist **list, char *key)
{
	struct stringlist *iter;
	struct stringlist **prev;

	if (key == NULL)
		return NULL;
	if (list == NULL)
		return NULL;

	prev = list;
	iter = *list;

	while (iter != NULL) {
		if (strcmp (iter->Key, key) == 0) {
			(*prev) = iter->Next;
			iter->Next = NULL;
			return iter;
		}

		prev = &(iter->Next);
		iter = iter->Next;
	}

	return NULL;
}

static void recursive_match_and_set (struct tweak *tree, struct stringlist **list, int immediate)
{
	struct stringlist *item;

	while (tree != NULL) {

		if (tree->ConfigName!=NULL && tree->ConfigName[0]=='!') {
			tree=tree->Next;
			continue;
		}

		item = match_and_remove (list, tree->ConfigName);

		if (item != NULL) {
			if (tree->ChangeValue != NULL)
				tree->ChangeValue (tree, item->Value, immediate);
			free (item->Key);
			free (item);
		}

		if (tree->Sub)
			recursive_match_and_set (tree->Sub, list, immediate);

		tree = tree->Next;
	}
}

static void free_list(struct stringlist **list)
{
	if (list==NULL)
		return;
	if (*list==NULL)
		return;
	
	free_list(&(*list)->Next);
	free((*list)->Key);
	free((*list));
	*list = NULL;
}


 /*
  * This function reads a configuration file and calls
  * ChangeValue on all available tweaks.
  * Returns 0 on success, negative values on failure
  *
  * ToDo: Need to find an algorithm that has less than O(n^2) complexity
  *
  */
int merge_config_in_tree (struct tweak *tree, char *filename, int immediate)
{
	struct stringlist *list = NULL;
	struct stringlist **prev = &list;

	char key[200];
	int value;

	FILE *file;


	/* Ok. First the easy part. Read the file into a linked list */

	file = fopen (filename, "r");
	if (file == NULL)
		return -1;

	while (feof (file) == 0) {
		struct stringlist *new;
		int result;

		result = fscanf (file, "%199s = %i \n", key, &value);

		if (result != 2)
			continue;
		if (key[0]=='#')
			continue;

		new = malloc (sizeof (struct stringlist));
		if (new == NULL) {
			printf ("Out of memory!\n");
			return -1;
		}
		memset (new, 0, sizeof (struct stringlist));
		new->Key = strdup (key);
		set_value_int(new->Value, value);
		(*prev) = new;
		prev = &(new->Next);
	}
	if (fclose (file) == -EBADF) {
		printf("Oops. You did something we did something we didn't think of.\n");
	}
	
	/* Now the recursive "match and set" part. Ugh. O(n^2). */

	recursive_match_and_set (tree, &list, immediate);
	
	free_list(&list);

	return 0;
}

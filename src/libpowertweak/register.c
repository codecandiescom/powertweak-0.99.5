/*
 *  $Id: register.c,v 1.21 2003/07/08 11:42:12 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <stdarg.h>
#include <powertweak.h>
#include <config.h>

extern struct tweak *tweaks;

struct shutdown_callback;

struct shutdown_callback {
	struct shutdown_callback *Next;
	shutdown_cb_fn *Function;
};

static struct shutdown_callback *callbacks = NULL;


/*
 * additemtolist_create
 * This function searches a linked list for an item with "WidgetText" set to "Item".
 * If none is found, a new node is added to the list. This new node is freshly allocated.
 *
 * This function returns the address of the matching node (or the new one, if no matching node
 * was found)
 */

static struct tweak *additemtolist_create (char *Item, struct tweak **list)
{
	struct tweak *tweak;
	struct tweak **Prev, *current;

	Prev = list;
	current = *list;

	if (Item == NULL)
		return NULL;


	/* First search for an existing item */
	while (current != NULL) {
		if ((current->WidgetText != NULL) && (strcasecmp (Item, current->WidgetText) == 0))
			return current;
		Prev = &(current->Next);
		current = current->Next;
	}

	/* When we get here, no match is found. First allocate and fill a new node.... */

	tweak = alloc_tweak (TYPE_NODE_ONLY);

	tweak->WidgetText = strdup (Item);

	/* ... and then add it to the end of the list */
	tweak->Next = NULL;
	*Prev = tweak;

	return tweak;
}


/*
 * additemtolist_create
 * This function searches a linked list for an item with "WidgetText" set to "Item".
 * If none is found, the "insert" tweak is added as a new node to the list 
 *
 * This function returns the address of the matching node (can be the "insert" one)
 */

static struct tweak *additemtolist_insert (char *Item, struct tweak **list, struct tweak *insert)
{
	struct tweak *tweak;
	struct tweak **Prev, *current;

	Prev = list;
	current = *list;

	if (Item == NULL)
		return NULL;


	/* Search for the match */
	while (current != NULL) {
		if ((current->WidgetText != NULL) && (strcasecmp (Item, current->WidgetText) == 0)) 
		{
			log_message("duplicate tweak, did not add (%s)\n", current->WidgetText);
			if (insert->Destroy!=NULL)
				insert->Destroy(insert);
			free(insert);
			return current;
		}
		Prev = &(current->Next);
		current = current->Next;
	}

	/* Ok. No match, so add "insert" to the list */
	tweak = insert;
	tweak->Next = NULL;
	*Prev = tweak;

	return tweak;
}


/* RegisterTweak:
 *
 * example usage:
 * RegisterTweak(tweak,"mmt",Tree1,Tree2,TabText);
 *
 * Format specifiers:
 * 'm' : Item on the tree on the left side of the GUI
 * 't' : "Tab" page
 * 'f' : "frame" text
 * 'c' : combo box
 * 'r' : radiobutton group
 */

void RegisterTweak (struct tweak *tweak, char *fmt, ...)
{
	struct tweak **current, *next;
	va_list ap;
	char *str;

	tweak->OrigValue = tweak->GetValue(tweak);

	current = &tweaks;

	if (tweak->ConfigName==NULL) {
		tweak->ConfigName = malloc(33);		/* FIXME: Check this. 19 should be enough on 64bit. No ? */
		assert(tweak->ConfigName!=NULL);
		snprintf(tweak->ConfigName,31,"!%p",(void*)tweak);
	}

	va_start (ap, fmt);
	while (*fmt) {
		str = va_arg (ap, char *);

		next = additemtolist_create (str, current);
		switch (*fmt) {
		case 'm':
			next->Type = TYPE_TREE;
			break;
		case 't':
			next->Type = TYPE_TAB;
			break;
		case 'f':
			next->Type = TYPE_FRAME;
			break;
		case 'h':
			next->Type = TYPE_HFRAME;
			break;
		case 'c':
			next->Type = TYPE_COMBO;
			break;
		case 'r':
			next->Type = TYPE_RADIO_GROUP;
			break;

		}
		current = &(next->Sub);
		fmt++;
	}
	va_end (ap);

	if (NULL == additemtolist_insert (tweak->WidgetText, current, tweak))
	{
		log_message("didn't add to list\n");
		tweak->Destroy(tweak);
		free(tweak);
	}
}

void RegisterShutdownCallback(shutdown_cb_fn *Func) 
{
	struct shutdown_callback *Temp;
	
	Temp = malloc(sizeof(struct shutdown_callback));
	Temp->Next = callbacks;
	Temp->Function = Func;
	callbacks = Temp;
}

static void ShutdownRecursive(struct shutdown_callback *CB)
{
	if (CB==NULL) 
		return;
	ShutdownRecursive(CB->Next);
	CB->Function();
	free(CB);
	return;
}

void ShutdownPlugins(void)
{
	ShutdownRecursive(callbacks);
	callbacks = NULL;
}

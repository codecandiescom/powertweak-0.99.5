/*
 *  $Id: misc.c,v 1.40 2003/07/08 11:42:12 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 * Random useful functions
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <powertweak.h>
#include <config.h>
#include <sys/utsname.h>

int fileexists (const char *filename)
{
	assert (filename != NULL);
	if (access (filename, R_OK) == 0)
		return 1;
	else
		return 0;
}

int filewritable (const char *filename)
{
	assert (filename != NULL);
	if (access (filename, W_OK) == 0)
		return 1;
	else
		return 0;
}

/* convert a string of the form "0001 0010" to 18 */
unsigned long bitstring_to_long (char *text)
{
	unsigned long value;

	if (text == NULL)
		return 0;

	value = 0;
	while (*text != '\0') {
		if (*text == '0')
			value = (value << 1);
		if (*text == '1')
			value = (value << 1) | 1;

		text++;
	}

	return value;
}

/* Generic "is_valid" method function for "lazy" backends  */
int generic_is_valid (struct tweak *tweak, value_t NewValue)
{
	if (tweak == NULL)
		return 0;
	/* Only types 2 and 3 can be generic */
	if ((tweak->Type != TYPE_SPINBOX) && (tweak->Type != TYPE_SLIDER))
		return 1;

	if (get_value_int(NewValue) < tweak->MinValue)
		return 0;
	if (get_value_int(NewValue) > tweak->MaxValue)
		return 0;


	return 1;
}


/* Generic function for converting string widget types to numbers 
 * Returns -1 on failure 
 */
int string_widget_to_int (char *text)
{
	if (text == NULL)
		return -1;

	if (strcasecmp (text, "CHECKBOX") == 0)
		return TYPE_CHECKBOX;

	if (strcasecmp (text, "SPINBUTTON") == 0)
		return TYPE_SPINBOX;

	if (strcasecmp (text, "SLIDER") == 0)
		return TYPE_SLIDER;

	if (strcasecmp (text, "TEXT") == 0)
		return TYPE_TEXT;

	if (strcasecmp (text, "INFOSTRING") == 0)
		return TYPE_INFO_STRING;

	if (strcasecmp (text, "COMBO") == 0)
		return TYPE_COMBO;

	if (strcasecmp (text, "COMBOELEMENT") == 0)
		return TYPE_COMBOELEM;

	if (strcasecmp (text, "LABEL") == 0)
		return TYPE_LABEL;

	if (strcasecmp (text, "INFOBOOL") == 0)
		return TYPE_INFO_BOOL;
		
	if (strcasecmp (text, "RADIOGROUP") == 0)
		return TYPE_RADIO_GROUP;

	if (strcasecmp (text, "RADIOBUTTON") == 0)
		return TYPE_RADIO_OPTION;
	
	return -1;
}

char *strdupcat (char *S1, char *S2)
{
	if (S2 == NULL)
		return S1;

	if (S1 == NULL)
		return strdup (S2);

	S1 = realloc (S1, strlen (S1) + strlen (S2) + 4);
	if (S1 == NULL)
		return NULL;

	strncat (S1, S2, strlen (S1) + strlen (S2) + 2);
	return S1;
}

char *strnewcat (char *S1, char *S2)
{
	char *res;
	res = malloc(4+strlen(S1)+strlen(S2));
	if (res!=NULL) {
		strncpy (res, S1, strlen (S1)+1);
		strncat (res, S2, strlen (S2) + 2);
	}
	return res;
}

#ifndef HAVE_STRNDUP
char *strndup (char *S1, int count)
{
	char *temp;

	temp = malloc (count + 1);
	assert (temp != NULL);
	strncpy (temp, S1, count + 1);
	temp[count] = 0;
	return temp;
}
#endif

static int defaultGetValueInt(struct tweak *tweak) 
{
	value_t value;
	int i;
	if (tweak->GetValue==NULL)
		return 0;
 	value = tweak->GetValue(tweak);
 	i = get_value_int(value);
 	release_value(&value);
	return  i;
}
void default_destructor (struct tweak *tweak)
{
	if (tweak->WidgetText!=NULL) 
	{
/*		printf("default free (%s)\n", tweak->WidgetText);*/
		free(tweak->WidgetText);
		tweak->WidgetText = NULL;
	}
	if (tweak->Description!=NULL) {
		free(tweak->Description);
		tweak->Description = NULL;
	}
	if (tweak->ConfigName!=NULL) {
		free(tweak->ConfigName);
		tweak->ConfigName = NULL;
	}
	release_value(&tweak->TempValue);/*SVEN - err huh?	there is a comment in powertweak.h that says this does not get done here.*/
	release_value(&tweak->OrigValue);
}

struct tweak *alloc_tweak(int type) 
{
	struct tweak *tweak;
	
	tweak = malloc(sizeof(struct tweak));
	/* FIXME: If we fail, we should just propagate the failure back up
	 * instead of trying to do anything clever.  Make sure that all the
	 * alloc_tweak() calls check for NULL first though. */
	if (tweak==NULL) {
		if (sleep(3)>0) {
			log_message("Oops. You did something we didn't think of.\n");
		}
		tweak = malloc(sizeof(struct tweak));
	}
	assert(tweak!=NULL);
	
	memset(tweak,0,sizeof(struct tweak));
	tweak->GetValueInt = &defaultGetValueInt;
	tweak->IsValid = &generic_is_valid;
	tweak->Destroy = &default_destructor;
	tweak->Type = type;
	return tweak;	
}


/*
Lessons learned from this code:
You can only have 2 out of 3:

1) Single-linked lists
2) (bubble)sorting
3) Clean code

*/
void _sort_tweak_list(struct tweak **tweak)
{
	struct tweak **prev,*current;	
	int count=1;
	
	prev = tweak;
	if (tweak==NULL)
	 	return;
	current = *tweak;
	if (current==NULL)
		return;
	
	while (count>0) {
		count =0;
		current = *tweak;
		prev = tweak;
		while (current->Next!=NULL) {
			if (current->Type != TYPE_TREE)
				break;
			if (strcmp(current->WidgetText,current->Next->WidgetText)>0) {
				struct tweak *temp;	
				*prev = current->Next;
				temp = current->Next->Next;
				current->Next->Next = current;
				current->Next = temp;
				count++;	
				break; /* this sucks */
			}
			prev = &(current->Next);
			current = current->Next;
		}	
	}
}


void sort_tweak_list(struct tweak **t)
{
	struct tweak *tweak = *t;

	_sort_tweak_list(t);
	while (tweak!=NULL) {
		if (tweak->Sub != NULL)
			sort_tweak_list(&(tweak->Sub));
		tweak = tweak->Next;
	}
}


void kernel_version(struct kernel_ver *kv)
{
	struct utsname buffer; 
	char *begin;

	uname (&buffer);
	begin = buffer.release;

	kv->major = atoi (begin);
	while ((*begin) != '.')
		begin++;
	begin++;
	kv->minor = atoi (begin);
		while ((*begin) != '.')
	begin++;
		begin++;
	kv->patchlevel = atoi (begin);
}

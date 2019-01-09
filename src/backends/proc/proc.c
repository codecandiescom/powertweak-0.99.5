/*
 *  $Id: proc.c,v 1.7 2003/07/08 11:42:12 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 *  	Misc /proc helper functions
 *
 */

#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <powertweak.h>
#include <errno.h>
#include <string.h>

#include "proc.h"

/* This function sets the default values in the private structure for /proc in struct tweak */
void initialize_private_proc_data (struct private_proc_data *ppd)
{
	assert (ppd != NULL);
	/* most fields default to 0 */
	memset (ppd, 0, sizeof (struct private_proc_data));

	ppd->Mask = 0xffffffff;	/* Check this for 64 bits ?? */
	ppd->OnMask = 0x00000001;
	ppd->Field = -1;
}

/* This returns the content of a /proc file. It returns the "element"th element, unless element
 * is -1; then it assumes there is only 1 element in the proc file after all.
 *
 * Something worrysome is lacking: How to report if the file cannot be opened?
 *
 */
unsigned int read_int_from_proc_file (const char *filename, int element)
{

	FILE *procfh;
	char contents[64];
	char *elementptr = NULL;
	int i;

	if ((procfh = fopen (filename, "r")) == NULL)
		return (FALSE);

	(void)fgets (contents, sizeof (contents), procfh);

	if (fclose (procfh) != 0)
		fprintf (stderr, "Error closing file : %s\t%s\n", filename, strerror(errno));

	elementptr = contents;

	if (element != -1) {
		for (i = 0; i < element; i++) {
			while (*elementptr != '\t')
				elementptr++;
			elementptr++;
		}
	}

	return (atoi (elementptr));
}

char contents[64];

char * read_string_from_proc_file (const char *filename, int element)
{

	FILE *procfh;
	char *elementptr = NULL;
	int i;

	if ((procfh = fopen (filename, "r")) == NULL)
		return (FALSE);

	(void)fgets (contents, sizeof (contents), procfh);

	if (fclose (procfh) != 0)
		fprintf (stderr, "Error closing file : %s\t%s\n", filename, strerror(errno));

	elementptr = contents;

	if (element != -1) {
		for (i = 0; i < element; i++) {
			while (*elementptr != '\t')
				elementptr++;
			elementptr++;
		}
	}

	return /*strdup*/(elementptr);
}

/* The opposite from read_int_from_proc_file .... */
void write_int_to_proc_file (const char *filename, int element, unsigned int value)
{
	FILE *procfh;


	unsigned int elements[64];
	int current = 0, i;
	int stop = 1;
	

	if (element == -1) {	/* Simple case */
		procfh = fopen (filename, "w");
		if (procfh != NULL) {
			fprintf (procfh, "%i", value);
		} else  {
			printf("cannot write to %s \n",filename);
		}
		(void)fclose (procfh);
		return;
	}


	if ((procfh = fopen (filename, "r")) == NULL)
		return;

	/* First read all values */
	while ((feof (procfh)==0) && (stop != 0) && (current < 63)) {
		stop = fscanf (procfh, "%i", &elements[current++]);
	}
	current--;

	(void)fclose (procfh);

	elements[element] = value;


	if ((procfh = fopen (filename, "w")) == NULL)
		return;

	for (i = 0; i < current; i++)
		fprintf (procfh, "%i\t", elements[i]);


	(void)fclose (procfh);
}

/*does not work yet (need an editable INFO_STRING)*/
void write_string_to_proc_file (const char *filename, int element, char *value)
{
	FILE *procfh;

	char * elements[64];
	int current = 0, i;
	int stop = 1;
	

	if (element == -1) {	/* Simple case */
		procfh = fopen (filename, "w");
		if (procfh != NULL) {
			fprintf (procfh, "%s", value);
		} else  {
			printf("cannot write to %s \n",filename);
		}
		(void)fclose (procfh);
		return;
	}


	if ((procfh = fopen (filename, "r")) == NULL)
		return;

	/* First read all values */
	while ((feof (procfh)==0) && (stop != 0) && (current < 63)) 
	{
		char tempStr[256];
		stop = fscanf (procfh, "%s", tempStr);
		elements[current++] = strdup(tempStr);
	}
	current--;

	(void)fclose (procfh);

	free (elements[element]);
	elements[element] = value;


	if ((procfh = fopen (filename, "w")) == NULL)
		return;

	for (i = 0; i < current; i++)
	{
		fprintf (procfh, "%s\t", elements[i]);
		free (elements[i]);
	}


	(void)fclose (procfh);
}


/* This function assumes tweak to be filled from XML and fills the
 * remaining values from the actual /proc file.
 *  
 * If the /proc file does not exists, this function returns 0, on
 * success it returns 1.
 */


int fill_struct_tweak_from_proc (struct tweak *tweak)
{
	struct private_proc_data *pvt;
	unsigned int value;

	if (tweak == NULL)
		return 0;

	pvt = (struct private_proc_data *) tweak->PrivateData;

	if (pvt == NULL)
		return 0;

	if (fileexists (pvt->Filename)==0)
		return 0;
	
	if (filewritable(pvt->Filename)==0)
	{
		tweak->ChangeValue = NULL;
	}

	if (tweak->Type == TYPE_INFO_STRING)
	{
		if (tweak->OrigValue.strVal != NULL)
		{
			free(tweak->OrigValue.strVal);
		}
		tweak->OrigValue.strVal = strdup(read_string_from_proc_file (pvt->Filename, pvt->Field));
	}
	else
	{
		value = read_int_from_proc_file (pvt->Filename, pvt->Field);
		value = value & pvt->Mask;

		if ((tweak->Type == TYPE_CHECKBOX)||(tweak->Type == TYPE_RADIO_OPTION)) {
			if (value != pvt->OnMask)
				value = 0;
			else
				value = 1;
		}
		pvt->value = value;
	}

	return 1;
}

/* 
 * this function "clones" a /proc tweak. This is needed for the wildcards.
 * in that case the tweak is first cloned, and then the respective fields
 * are updated.
 */
struct tweak *clone_proc_tweak(struct tweak *tweak)
{
	struct tweak *new;
	struct private_proc_data *pvt,*private;
	
	assert(tweak!=NULL);
	
	private=tweak->PrivateData;
	assert(private!=NULL);
	
	new = alloc_tweak(tweak->Type);
	pvt = malloc(sizeof(struct private_proc_data));
	if (pvt==NULL) {
		free(new);
		return NULL;
	}
	
	new->PrivateData = (void*)pvt;
	
	if (tweak->WidgetText!=NULL)
		new->WidgetText = strdup(tweak->WidgetText);
	if (tweak->Description!=NULL)
		new->Description = strdup(tweak->Description);
	if (tweak->ConfigName!=NULL)
		new->ConfigName = strdup(tweak->ConfigName);
	new->MinValue = tweak->MinValue;
	new->MaxValue = tweak->MaxValue;
	new->ChangeValue = tweak->ChangeValue;
	new->GetValue = tweak->GetValue;
	new->GetValueInt = tweak->GetValueInt;
	new->GetValueRaw = tweak->GetValueRaw;
	new->IsValid = tweak->IsValid;
	new->Destroy = tweak->Destroy;
	
	if (private->Filename!=NULL)
		pvt->Filename = strdup(private->Filename);
	pvt->Field = private->Field;
	pvt->Mask  = private->Mask;
	pvt->OnMask= private->OnMask;
	pvt->OffMask=private->OffMask;
	pvt->value = private->value;
	
	return new;
}

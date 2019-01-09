/*
 *  $Id: methods.c,v 1.10 2003/07/08 11:42:12 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 *	This file contains the methods for /proc tweak objects.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <powertweak.h>
#include "proc.h"

/* This is the GetValue method in struct tweak. */
value_t proc_get_value (struct tweak *tweak)
{
	value_t val = {0,NULL};
	assert(tweak != NULL);

	if (tweak->Type == TYPE_INFO_STRING)
	{
		val.strVal = strdup(tweak->OrigValue.strVal);
	}
	else
	{
		struct private_proc_data *pvt;
	
		pvt = (struct private_proc_data *) tweak->PrivateData;
	
		assert(pvt != NULL);
	
		set_value_int(val,pvt->value);
	}		
	return val;
}

int proc_get_value_raw(struct tweak *tweak)
{
	return(fill_struct_tweak_from_proc(tweak));
}

/* This is the ChangeValue method in struct tweak */
void proc_change_value (struct tweak *tweak, value_t value, int immediate)
{
	struct private_proc_data *pvt;
	unsigned int temp;

	if (tweak == NULL)
		return;

	pvt = (struct private_proc_data *) tweak->PrivateData;

	if (pvt == NULL)
		return;

	/* Store the new value in the private data */
	pvt->value = get_value_int(value);

	if (immediate==0)
		return;

	/* 0 values for combo/radio-elements should _NOT_ be set */
	if ((tweak->Type == TYPE_COMBOELEM) && (get_value_int(value) == 0))
		return;
	if ((tweak->Type == TYPE_RADIO_OPTION) && (get_value_int(value) == 0))
		return;

	/* If we got here, we should set the actual /proc value */

	temp = read_int_from_proc_file (pvt->Filename, pvt->Field);
	temp = temp & (~(pvt->Mask));

	if (tweak->Type == TYPE_CHECKBOX) {
		if (get_value_int(value)!=0)
			temp |= pvt->OnMask;
		else
			temp |= pvt->OffMask;
	} else {
		temp |= get_value_int(value);
	}

	/* write actual value to proc here */
	if (tweak->Type == TYPE_INFO_STRING)
	{
		/*SVEN need a TYPE_EDIT_STRING*/
/*		write_string_to_proc_file(pvt->Filename,pvt->Field,value.strVal);*/
	}
	else
	{
		write_int_to_proc_file(pvt->Filename,pvt->Field,temp);
	}

	
	//printf ("Going to write %d to %s\n", temp, pvt->Filename);
}


void proc_tweak_destructor (struct tweak *tweak)
{
	struct private_proc_data *private;
	
	assert (tweak!=NULL);
	
	default_destructor (tweak);

	private = tweak->PrivateData;
	if (private==NULL)
		return;
	
	tweak->PrivateData = NULL;
	if (private->Filename!=NULL)
		free(private->Filename);
	free(private);
}

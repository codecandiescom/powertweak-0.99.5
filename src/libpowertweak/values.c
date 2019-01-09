/*
 *  $Id: values.c,v 1.3 2003/07/08 11:42:12 svenud Exp $
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


#include <powertweak.h>


void copy_values(value_t *dest,value_t src)
{	
	assert(dest!=NULL);
	if (dest->strVal!=NULL) 
		free(dest->strVal);
	if (src.strVal!=NULL)
		dest->strVal = strdup(src.strVal);
	else
		dest->strVal = NULL;
	dest->intVal = src.intVal;
}

void copy_and_release_value(value_t *dest,value_t *src)
{	
	assert(dest!=NULL);
	if (dest->strVal!=NULL) 
		free(dest->strVal);
	if (src->strVal!=NULL) {
		dest->strVal = src->strVal;
		src->strVal = NULL;
	} else {
		dest->strVal = NULL;
		free(src->strVal);
		src->strVal = NULL;
	}
	dest->intVal = src->intVal;
}


void release_value(value_t *val)
{
	if (val->strVal!=NULL) {
/*		printf("freeing (%p)", val->strVal);
		printf(" %s\n", val->strVal);*/
		free(val->strVal);
		val->strVal = NULL;
	}
}

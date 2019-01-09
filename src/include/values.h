/*
 *  $Id: values.h,v 1.1 2000/12/23 12:56:30 fenrus Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#ifndef _INCLUDE_GUARD_VALUES_H_
#define _INCLUDE_GUARD_VALUES_H_

typedef struct {
	unsigned int   intVal;
	char *strVal;
} value_t;

/* some access-macros... might turn into functions later */
#define get_value_int(x) ( (x).intVal )
#define set_value_int(x,y) do { (x).intVal = (y); } while (0)


/* Makes a full copy of a value_t */
void copy_values(value_t *dest,value_t src);

/* Releases any memory allocated in a value_t, doesn't release the value_t itself */
void release_value(value_t *val);

/* Copies a value t and the releases the source */
void copy_and_release_value(value_t *dest,value_t *src);

#endif

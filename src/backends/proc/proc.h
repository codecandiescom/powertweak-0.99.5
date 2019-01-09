/*
 *  $Id: proc.h,v 1.5 2003/04/27 13:13:47 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#ifndef _INCLUDE_GUARD_PROC_H_
#define _INCLUDE_GUARD_PROC_H_

/* struct private_proc_data contains
 * everything needed to get/set proc values from the GUI
 */  
struct private_proc_data {
	char	*Filename;	/*ie.   /proc/sys/vm/bdflush */
	int	Field;		/* -1 for /proc files with only 1 item,
				   0 = first integer, 1 = second etc */
	unsigned int	Mask;	/* Bitmask for bitfield /proc items */
	unsigned int	OnMask;	/* Bitmask for "On" */
	unsigned int	OffMask;/* Bitmask for "Off" */
	
	unsigned int	value;
};

void initialize_private_proc_data(/*@out@*/ struct private_proc_data *ppd);
int fill_struct_tweak_from_proc(struct tweak *tweak);
/*@only@*/struct tweak *clone_proc_tweak(struct tweak *tweak);
unsigned int read_int_from_proc_file (const char *filename, int element);
void write_int_to_proc_file (const char *filename, int element, unsigned int value);
void write_string_to_proc_file (const char *filename, int element, char * value);

/* Methods */
void proc_change_value(struct tweak*, value_t, int);
value_t proc_get_value(struct tweak*);
int proc_get_value_raw (struct tweak*);
int proc_validate_value(struct tweak*,value_t);
void load_proc_xmlfile(char *Filename);
void proc_tweak_destructor (struct tweak *tweak);

#endif

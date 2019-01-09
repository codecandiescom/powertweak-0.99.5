/*
 *  $Id: cpu.c,v 1.25 2003/04/26 12:52:26 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <powertweak.h>
#include <config.h>
#include <unistd.h>
#include <string.h>

#include "cpu.h"

static int nrCPUs;
static char *Menu1 = "Hardware";
/*static char *Menu2 = "CPU";*/

void CPU_tweak_destructor (struct tweak *tweak)
{
	struct private_CPU_data *private;

	default_destructor(tweak);

	private = tweak->PrivateData;

	if (private == NULL)
		return;

	if ((private->cpuname != NULL) && (*(private->cpuname)!=NULL)) {
		free (*(private->cpuname));
		*(private->cpuname) = NULL;	/* stop others freeing it too. */
	}

	release_value(&private->value);
	free (tweak->PrivateData);
}


static value_t CPU_get_value(struct tweak *tweak)
{
	struct private_CPU_data *private;
	value_t localvalue={0,NULL};

	assert(tweak!=NULL);
	private=tweak->PrivateData;
	assert(private!=NULL);

	copy_values(&localvalue,private->value);
	return localvalue;
}


/*
 * This routine adds a [0], [1] etc.. to each CPU name in an SMP system.
 */
void Make_CPU_menuname (struct cpu_identity *id)
{
	char *tmpstr;

	if (id->CPUName == NULL)
		return;

	if (nrCPUs == 1)
		return;

	tmpstr = malloc (strlen(id->CPUName) + 5);
	if (tmpstr == NULL)
		return;

	memset (tmpstr, 0, (strlen (id->CPUName) + 5));
	sprintf (tmpstr, "%s [%d]", id->CPUName, id->CPU_number);
	free (id->CPUName);
	id->CPUName = tmpstr;
}


void AddTo_CPU_tree (struct cpu_identity *id, struct tweak *tweak, char *tab)
{
	char cpuNode[10];
	snprintf(cpuNode, 9, "CPU%d", id->CPU_number);
/*	Make_CPU_menuname (id);*/
	RegisterTweak (tweak, "mmt", Menu1, cpuNode, tab);
}


void AddTo_CPU_treehframe (struct cpu_identity *id, struct tweak *tweak, char *tab, char *frame)
{
	char cpuNode[10];
	snprintf(cpuNode, 9, "CPU%d", id->CPU_number);
/*	Make_CPU_menuname (id);*/
	RegisterTweak (tweak, "mmth", Menu1, cpuNode, tab, frame);
}


struct tweak *alloc_CPU_tweak (int CPUnum, int type)
{
	struct tweak *tweak;
	struct private_CPU_data *private;

	tweak = alloc_tweak(type);

	private = malloc (sizeof (struct private_CPU_data));
	if (private == NULL) {
		printf ("Out of memory\n");
		free (tweak);
		return (NULL);
	}
	memset(private,0,sizeof(struct private_CPU_data));
	tweak->PrivateData = (void *)private;
	tweak->Destroy = &CPU_tweak_destructor;
	tweak->GetValue = &CPU_get_value;
	private->CPU_number = CPUnum;

	return tweak;
}


int InitPlugin (int showinfo)
{
	int i;
	struct cpu_identity *id;

	if (!HaveCPUID)
		return FALSE;

#if defined __GLIBC__ && __GLIBC__ >=2
	nrCPUs = sysconf (_SC_NPROCESSORS_CONF);
#else
	nrCPUs = 1;
#endif

	id = malloc (sizeof(struct cpu_identity));
	if (id == NULL) {
		printf ("CPU backend couldn't malloc %d bytes.\n", sizeof(struct cpu_identity));
		return FALSE;
	}

	for (i = 0; i < nrCPUs; i++) {
		memset (id, 0, sizeof(struct cpu_identity));
		id->CPU_number = i;
		identify_CPU (id);
		if (showinfo == TRUE)
			Add_CPU_info_page(id);
		Add_CPU_tweaks(id);
	}

	if (id->CPUName!=NULL)
		free(id->CPUName);
	free (id);

	return TRUE;
}

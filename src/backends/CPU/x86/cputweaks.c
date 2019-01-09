/*
 *  $Id: cputweaks.c,v 1.13 2001/11/06 18:41:21 davej Exp $
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
#include <string.h>

#include "x86.h"
#include "../cpu.h"


void append_to_CPU_name(struct cpu_identity *id, char *TXT)
{
	char *tmpstr;
	int len;

	if (id->CPUName == NULL) {
		id->CPUName = strdup (TXT);
		return;
	}

	len = strlen (id->CPUName) + strlen (TXT) + 1;

	tmpstr = malloc (len);
	if (tmpstr==NULL) {
		printf ("Boom, couldn't malloc %d bytes.\n", len);
		return;
	}
	memset (tmpstr, 0, len);
	sprintf (tmpstr, "%s%s", id->CPUName, TXT);
	free (id->CPUName);
	id->CPUName = tmpstr;
}


void Add_CPU_tweaks(struct cpu_identity *id)
{
	unsigned long long tmp;

	if (rdmsr (0, 0, &tmp) == -1) {	/* Do we have the MSR driver? */
		printf ("MSR driver not loaded.\n");
		return;
	}

	Load_CPU_XML (id);

	/* K6 PowerNOW. */
	if ( (id->vendor==X86_VENDOR_AMD) && 
    	 (id->family==5) && (id->model>=12) )
		PowerNow_tweak(id->CPUName);
}

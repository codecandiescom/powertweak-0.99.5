/*
 *  $Id: cpuid.c,v 1.3 2001/05/28 02:41:00 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#include "x86.h"

void cpuid (int CPU_number, int cpuidindex,
	unsigned long *eax,
	unsigned long *ebx,
	unsigned long *ecx,
	unsigned long *edx)
{
	char cpuname[20];
	unsigned char buffer[16];
	int fh;

	/* Have we established this is uniprocessor ? */
	if (CPU_number == -1) {
		cpuid_UP (cpuidindex, eax, ebx, ecx, edx);
		return;
	}

	/* Ok, this is SMP, so we need to use the /dev/CPU interface. */
	snprintf (cpuname,18, "/dev/cpu/%d/cpuid", CPU_number);
	fh = open (cpuname, O_RDONLY);
	if (fh != -1) {
		lseek (fh, cpuidindex, SEEK_CUR);
		read (fh, &buffer[0], 16);
		*eax = (*(unsigned long *)buffer);
		*ebx = (*(unsigned long *)(buffer+4));
		*ecx = (*(unsigned long *)(buffer+8));
		*edx = (*(unsigned long *)(buffer+12));
		close (fh);
	} else {
		/* Something went wrong, just do UP and hope for the best. */
		cpuid_UP (cpuidindex, eax, ebx, ecx, edx);
	}
}

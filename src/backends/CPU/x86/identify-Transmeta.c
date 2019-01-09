/*
 *  $Id: identify-Transmeta.c,v 1.6 2001/10/01 01:59:00 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <string.h>
#include <powertweak.h>
#include "x86.h"

void identify_Transmeta(struct cpu_identity *id)
{
	unsigned long eax, ebx, ecx, edx;
	unsigned int i;
	char model[60];

	cpuid (id->CPU_number, 1, &eax, &ebx, &ecx, &edx);

	id->stepping = eax & 0xf;
	id->model = (eax >> 4) & 0xf;
	id->family = (eax >> 8) & 0xf;


	switch (id->family) {

	case 5:
		append_to_CPU_name(id,  "Crusoe");

		cpuid (id->CPU_number, 0x80860000, &eax, &ebx, &ecx, &edx);
		if (eax >= 0x80860006 ) {
			for (i=0 ; i<4 ; i++) {
				cpuid (id->CPU_number, i+80860003,
					(void *)&model[0+(i*16)],
					(void *)&model[4+(i*16)],
					(void *)&model[8+(i*16)],
					(void *)&model[12+(i*16)]);
			}
			append_to_CPU_name(id, model);
		}
		break;

	default:
		append_to_CPU_name(id,  "Unknown");
		break;
	}

	cpuid (id->CPU_number, 0, &eax, &ebx, &ecx, &edx);
	memcpy (id->VendorName, &ebx, 4);
	memcpy (id->VendorName+4, &edx, 4);
	memcpy (id->VendorName+8, &ecx, 4);
}

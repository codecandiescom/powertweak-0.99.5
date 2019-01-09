/*
 *  $Id: identify-Rise.c,v 1.5 2001/10/01 01:59:00 davej Exp $
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

void identify_Rise(struct cpu_identity *id)
{
	unsigned long eax, ebx, ecx, edx;

	cpuid (id->CPU_number, 1, &eax, &ebx, &ecx, &edx);

	id->stepping = eax & 0xf;
	id->model = (eax >> 4) & 0xf;
	id->family = (eax >> 8) & 0xf;


	switch (id->family) {

	case 5:
		switch (id->model) {
			case 0:		
			case 2:
			case 8:
			case 9:		append_to_CPU_name (id, "mP6");
						break;
			default:	append_to_CPU_name (id, "???");
						break;
		}
		break;

	default:
		append_to_CPU_name (id, "Unknown");
		break;
	}

	cpuid (id->CPU_number, 0, &eax, &ebx, &ecx, &edx);
	memcpy (id->VendorName, &ebx, 4);
	memcpy (id->VendorName+4, &edx, 4);
	memcpy (id->VendorName+8, &ecx, 4);
}

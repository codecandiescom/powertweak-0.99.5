/*
 *  $Id: identify-IDT.c,v 1.9 2001/10/27 20:48:27 davej Exp $
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

void identify_IDT(struct cpu_identity *id)
{
	unsigned long eax, ebx, ecx, edx;
		
	cpuid (id->CPU_number, 1, &eax, &ebx, &ecx, &edx);

	id->stepping = eax & 0xf;
	id->model = (eax >> 4) & 0xf;
	id->family = (eax >> 8) & 0xf;
	id->type = (eax >> 12) & 0x3;


	switch (id->family) {

	case 5:
		switch (id->model) {
			case 4:		append_to_CPU_name(id,  "Winchip C6");
						break;

			case 8:		append_to_CPU_name(id,  "Winchip ");
						switch (id->stepping) {

							default:	append_to_CPU_name(id,  "2");
										break;
							case 7:
							case 8:
							case 9:		append_to_CPU_name(id,  "2A");
										break;
							case 10:
							case 11:
							case 12:
							case 13:
							case 14:
							case 15:	append_to_CPU_name(id,  "2B");
										break;
						}
						break;

			case 9:		append_to_CPU_name(id,  "Winchip 3");
						break;

			default:	append_to_CPU_name(id,  "Unknown");
						break;
		}
		break;
	case 6:
		switch (id->model) {
			case 5:
			case 6:
				append_to_CPU_name(id,  "Cyrix III Samuel");
				break;
			case 7:
				if (id->stepping < 8)
					append_to_CPU_name(id,  "C3 Samuel 2");
				else
					append_to_CPU_name(id,  "C3 Ezra");
				break;

			default:
				append_to_CPU_name(id,  "Unknown");
				break;
		}
		break;
	}

	cpuid (id->CPU_number, 0, &eax, &ebx, &ecx, &edx);
	memcpy (id->VendorName, &ebx, 4);
	memcpy (id->VendorName+4, &edx, 4);
	memcpy (id->VendorName+8, &ecx, 4);
}

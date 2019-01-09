/*
 *  $Id: identify-Intel.c,v 1.15 2001/12/08 17:14:16 davej Exp $
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

void identify_Intel(struct cpu_identity *id)
{
	unsigned long eax, ebx, ecx, edx;

	cpuid (id->CPU_number, 1, &eax, &ebx, &ecx, &edx);

	id->stepping = eax & 0xf;
	id->model = (eax >> 4) & 0xf;
	id->family = (eax >> 8) & 0xf;
	id->type = (eax >> 12) & 0x3;
	id->brand = (ebx & 0xf);


	switch (id->family) {

	case 4:
		append_to_CPU_name(id, "i486");
		switch (id->model) {
			case 0:		append_to_CPU_name(id, "DX-25/33");
						break;
			case 1:		append_to_CPU_name(id, "DX-50");
						break;
			case 2:		append_to_CPU_name(id, "SX");
						break;
			case 3:		append_to_CPU_name(id, "DX2");
						break;
			case 4:		append_to_CPU_name(id, "SL");
						break;
			case 5:		append_to_CPU_name(id, "SX2");
						break;
			case 7:		append_to_CPU_name(id, "DX2WB");
						break;
			case 8:		append_to_CPU_name(id, "DX4");
						break;
			case 9:		append_to_CPU_name(id, "DX4WB");
						break;
			default:	append_to_CPU_name(id, "Unknown");
						break;
		}
		break;

	case 5:
		append_to_CPU_name(id, "Pentium ");
		switch (id->model) {
			case 0:		append_to_CPU_name(id, "A-step");
						break;
			case 1:		append_to_CPU_name(id, "60/66");
						break;
			case 2:		append_to_CPU_name(id, "75-200");
						break;
			case 3:		append_to_CPU_name(id, "Overdrive");
						break;
			case 4:		append_to_CPU_name(id, "MMX");
						break;
			case 7:		append_to_CPU_name(id, "Mobile");
						break;
			case 8:		append_to_CPU_name(id, "MMX Mobile");
						break;
			default:	append_to_CPU_name(id, "Unknown");
						break;
		}
		break;

	case 6:
		cpuid (id->CPU_number, 2, &eax, &ebx, &ecx, &edx);
		id->L2_cache_size = (edx & 0x3f) << 7;

		switch (id->model) {

			case 0:
				append_to_CPU_name (id, "A-step");
				break;
			
			case 1:
				append_to_CPU_name (id, "Pentium Pro");
				break;

			case 3:
				append_to_CPU_name (id, "Pentium II");
				if (id->stepping == 2)
					append_to_CPU_name (id, " Overdrive");
				break;

			case 4:
				append_to_CPU_name(id, "Pentium II");
				break;

			case 5:
				if (id->L2_cache_size == 0) {
					append_to_CPU_name (id, "Celeron Covington");
					break;
				}
				if (id->L2_cache_size == 256) {
					append_to_CPU_name (id, "Celeron Dixon"); 
					break;
				}
				append_to_CPU_name (id, "Pentium II");
				if (id->L2_cache_size > 256) 
					append_to_CPU_name (id, " Xeon");
				break;

			case 6:
				if (id->L2_cache_size == 128) {
					append_to_CPU_name (id, "Celeron Mendocino");
					break;
				}
				if (id->L2_cache_size == 256) {
					append_to_CPU_name (id, "Mobile Pentium II");
					break;
				}
				append_to_CPU_name (id, "Celeron / Pentium II");
				break;

			case 7:
				append_to_CPU_name (id, "Pentium III");
				if (id->L2_cache_size > 256) 
					append_to_CPU_name(id, " Xeon");
				break;

			case 8:
				if (id->L2_cache_size == 128) {
					append_to_CPU_name (id, "Celeron Coppermine");
					break;
				} else {
					if (id->L2_cache_size > 256) {
						append_to_CPU_name (id, "Pentium III Xeon");
						break;
					} else {
						append_to_CPU_name (id, "Pentium III Coppermine");
						if (id->stepping == 6)
							append_to_CPU_name (id, " (Stepping C-0)");
						if (id->stepping == 8 || id->stepping == 0xA)
							append_to_CPU_name (id, " (Stepping D-0)");
						break;
					}
				}
				break;

			case 0xA:
				switch (id->brand) {
					case 0:
						append_to_CPU_name (id, "Pentium II Deschutes");
						break;
					case 1:
						append_to_CPU_name (id, "Celeron");
						break;
					case 2:
						append_to_CPU_name (id, "Pentium III");
						break;
					case 3:
						append_to_CPU_name (id, "Pentium III Xeon");
						break;
					default:
						append_to_CPU_name (id, "Unknown type");
						break;
				}
				break;

			default:
				append_to_CPU_name (id, "???");
				break;
		}
		break;
		
	case 0x7:
		append_to_CPU_name (id, "Itanium");
		break;

	case 0xF:
		if (id->model == 0) {
			append_to_CPU_name (id, "Pentium 4");
			if (id->stepping == 7)
				append_to_CPU_name (id, " (Stepping B-2)");
			if (id->stepping == 0xA)
				append_to_CPU_name (id, " (Stepping C-1)");
			break;
		} else {
			append_to_CPU_name (id, "???");
		}
	}

	cpuid (id->CPU_number, 0, &eax, &ebx, &ecx, &edx);
	memcpy (id->VendorName, &ebx, 4);
	memcpy (id->VendorName+4, &edx, 4);
	memcpy (id->VendorName+8, &ecx, 4);
}

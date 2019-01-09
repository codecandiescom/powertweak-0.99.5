/*
 *  $Id: identify-AMD.c,v 1.17 2001/10/15 22:41:19 davej Exp $
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

void identify_AMD(struct cpu_identity *id)
{
	unsigned long eax, ebx, ecx, edx;
	char BrandName[48];

	cpuid (id->CPU_number, 1, &eax, &ebx, &ecx, &edx);

	id->stepping = eax & 0xf;
	id->model = (eax >> 4) & 0xf;
	id->family = (eax >> 8) & 0xf;


	switch (id->family) {

	case 4:
		switch (id->model) {
			case 3:		append_to_CPU_name(id,"Am486DX2-WT");		break;
			case 7:		append_to_CPU_name(id,"Am486DX2-WB");		break;
			case 8:		append_to_CPU_name(id,"Am486DX4-WT / Am5x86-WT");	break;
			case 9:		append_to_CPU_name(id,"Am486DX4-WB / Am5x86-WB");	break;
			case 10:	append_to_CPU_name(id,"Elan SC400");		break;
			case 0xe:	append_to_CPU_name(id,"Am5x86-WT");			break;
			case 0xf:	append_to_CPU_name(id,"Am5x86-WB");			break;
			default:	append_to_CPU_name(id,"???");				break;
		}
		break;

	case 5:
		switch (id->model) {
			case 0:		append_to_CPU_name(id,"SSA5 (PR75/PR90/PR100)");	break;
			case 1:		append_to_CPU_name(id,"K5 (PR120/PR133)");			break;
			case 2:		append_to_CPU_name(id,"K5 (PR166)");				break;
			case 3:		append_to_CPU_name(id,"K5 (PR200)");				break;
			case 6:		append_to_CPU_name(id,"K6 (0.30 um)");				break;
			case 7:		append_to_CPU_name(id,"K6 (0.25 um)");				break;
			case 8:		append_to_CPU_name(id,"K6-2");
						if (id->stepping >=8)
							append_to_CPU_name(id,"(CXT core)");
						break;
			case 9:		append_to_CPU_name(id,"K6-3");				break;
			case 12:	append_to_CPU_name(id,"K6-2+ (0.18um)");		break;
			case 13:	append_to_CPU_name(id,"K6-3+ (0.18um)");		break;
			default:	append_to_CPU_name(id,"???");				break;
		}
		break;

	case 6:
		switch (id->model) {
			case 0:		append_to_CPU_name(id, "K7 ES");		break;
			case 1:		append_to_CPU_name(id, "Athlon (0.25um)");
						switch (id->stepping) {
							case 1:	append_to_CPU_name (id, " Rev C1");	break;
							case 2:	append_to_CPU_name (id, " Rev C2");	break;
						}
						break;
			case 2:		append_to_CPU_name(id, "Athlon (0.18um)");
						switch (id->stepping) {
							case 1:	append_to_CPU_name (id, " Rev A1");	break;
							case 2:	append_to_CPU_name (id, " Rev A2");	break;
						}
						break;
			case 3:		append_to_CPU_name(id, "Duron");
						switch (id->stepping) {
							case 0:	append_to_CPU_name(id, " Rev A0");	break;
							case 1:	append_to_CPU_name(id, " Rev A2");	break;
						}
						break;
			case 4:		append_to_CPU_name(id, "Thunderbird");
						switch (id->stepping) {
							case 0:	append_to_CPU_name(id, " Rev A1");	break;
							case 1:	append_to_CPU_name(id, " Rev A2");	break;
							case 2:	append_to_CPU_name(id, " Rev A4-A8");break;
							case 3:	append_to_CPU_name(id, " Rev A9");	break;
						}
						break;
			case 6:		switch (id->stepping) {
							case 0:	append_to_CPU_name(id, "Athlon 4 Rev A0-A1");	break;
							case 1:	append_to_CPU_name(id, "Athlon 4 Rev A2");		break;
							case 2:	append_to_CPU_name(id, "Mobile Duron");			break;
						}
						break;
			case 7:		switch (id->stepping) {
							case 0:	append_to_CPU_name(id, "Duron (Morgan core) Rev A0");	break;
							case 1:	append_to_CPU_name(id, "Duron (Morgan core) Rev A1");	break;
						}
						break;
			default:	append_to_CPU_name(id,"???");	break;
		}
	}

	cpuid (id->CPU_number, 0, &eax, &ebx, &ecx, &edx);
	memcpy (id->VendorName, &ebx, 4);
	memcpy (id->VendorName+4, &edx, 4);
	memcpy (id->VendorName+8, &ecx, 4);

	cpuid (id->CPU_number, 0x80000000, &eax, &ebx, &ecx, &edx);
	if (eax < 0x80000004)
		return;

	cpuid (id->CPU_number, 0x80000002, &eax, &ebx, &ecx, &edx);
	memcpy (BrandName,    &eax, 4);
	memcpy (BrandName+4,  &ebx, 4);
	memcpy (BrandName+8,  &ecx, 4);
	memcpy (BrandName+12, &edx, 4);

	cpuid (id->CPU_number, 0x80000003, &eax, &ebx, &ecx, &edx);
	memcpy (BrandName+16, &eax, 4);
	memcpy (BrandName+20, &ebx, 4);
	memcpy (BrandName+24, &ecx, 4);
	memcpy (BrandName+28, &edx, 4);

	cpuid (id->CPU_number, 0x80000004, &eax, &ebx, &ecx, &edx);
	memcpy (BrandName+32, &eax, 4);
	memcpy (BrandName+36, &ebx, 4);
	memcpy (BrandName+40, &ecx, 4);
	memcpy (BrandName+44, &edx, 4);

	id->CPUName = strdup (BrandName);
}

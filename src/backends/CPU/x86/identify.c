/*
 *  $Id: identify.c,v 1.9 2001/10/23 23:03:25 davej Exp $
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

int TranslateVendor (char *vendorstr)
{
	if (!strncmp(vendorstr, "AuthenticAMD", 12))
		return X86_VENDOR_AMD;
	if (!strncmp(vendorstr, "CentaurHauls", 12))
		return X86_VENDOR_CENTAUR;
	if (!strncmp(vendorstr, "CyrixInstead", 12))
		return X86_VENDOR_CYRIX;
	if (!strncmp(vendorstr, "GenuineIntel", 12))
		return X86_VENDOR_INTEL;
	if (!strncmp(vendorstr, "NexGenDriven", 12))
		return X86_VENDOR_NEXGEN;
	if (!strncmp(vendorstr, "RiseRiseRise", 12))
		return X86_VENDOR_RISE;
	if (!strncmp(vendorstr, "GenuineTMx86", 12))
		return X86_VENDOR_TRANSMETA;
	if (!strncmp(vendorstr, "UMC UMC UMC ", 12))
		return X86_VENDOR_UMC;
	return X86_VENDOR_UNKNOWN;
}


char *IDtoVendor (int vendor)
{
	switch (vendor) {
		case X86_VENDOR_AMD:
			return "AuthenticAMD";
		case X86_VENDOR_CENTAUR:
			return "CentaurHauls";
		case X86_VENDOR_CYRIX:
			return "CyrixInstead";
		case X86_VENDOR_INTEL:
			return "GenuineIntel";
		case X86_VENDOR_NEXGEN:
			return "NexGenDriven";
		case X86_VENDOR_RISE:
			return "RiseRiseRise";
		case X86_VENDOR_TRANSMETA:
			return "GenuineTMx86";
		case X86_VENDOR_UMC:
			return "UMC UMC UMC ";
		default:
			return "UnknownCPU!!";
	}
}


void identify_CPU(struct cpu_identity *id)
{
	unsigned long eax, ebx, ecx, edx, max_cpuid;
	unsigned int i;
	char vendor[12];

	cpuid (id->CPU_number, 0, &eax, &ebx, &ecx, &edx);

	max_cpuid = eax;

	memset(vendor,0,sizeof(vendor));

	for (i=0;i<4;i++)
		vendor[i] =(unsigned char) (ebx >>(8*i));
	for (i=0;i<4;i++)
		vendor[4+i] =(unsigned char) (edx >>(8*i));
	for (i=0;i<4;i++)
		vendor[8+i] =(unsigned char) (ecx >>(8*i));

	id->vendor = TranslateVendor (vendor);

	switch (id->vendor) {

		case X86_VENDOR_AMD:
			if (max_cpuid > 0)
				identify_AMD (id);
			break;

		case X86_VENDOR_CENTAUR:
			if (max_cpuid > 0)
				identify_IDT (id);
			break;

		case X86_VENDOR_CYRIX:
			identify_Cyrix (id);
			break;

		case X86_VENDOR_INTEL:
			if (max_cpuid > 0)
				identify_Intel (id);
			break;

		case X86_VENDOR_NEXGEN:
			break;

		case X86_VENDOR_RISE:
			if (max_cpuid > 0)
				identify_Rise (id);
			break;

		case X86_VENDOR_TRANSMETA:
			if (max_cpuid > 0)
				identify_Transmeta (id);
			break;

		case X86_VENDOR_UMC:
			break;

		case X86_VENDOR_UNKNOWN:
			break;
	}
}

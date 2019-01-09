/*
 *  $Id: info-page.c,v 1.13 2001/10/19 03:14:36 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

#include <powertweak.h>
#include "x86.h"
#include "../cpu.h"


static void AddFeaturesPage (struct cpu_identity *id)
{
	struct tweak *tweak;
	struct private_CPU_data *pvt;
	int i;
	char *Tab = "Features";
	unsigned long eax, ebx, ecx, edx;

	static char *x86_cap_flags[] = {
	  "(fpu) Floating Point Unit onchip.",
	  "(vme) Virtual Mode Extension.",
	  "(de) Debugging Extensions.",
	  "(pse) Page Size Extensions.",
	  "(tsc) Time Stamp Counter.",
	  "(msr) Model Specific Registers.",
	  "(pae) Physical Address Extensions.",
	  "(mce) Machine Check Architecture.",
	  "(cx8) CMPXCHG8 instruction.",
	  "(apic) Advanced Programmable Interrupt Controller.",
	  "(10) - Reserved.",
	  "(sep) Fast System Call.",
	  "(mtrr) Memory Type Range Registers.",
	  "(pge) Page Global Enable.",
	  "(mca) Machine Check Architecture.",
	  "(cmov) Conditional Move instruction.",
	  "(16) - Reserved.",
	  "(pse36) 36 bit Page Size Extensions.",
	  "(psn) 96 bit Processor Serial Number",
	  "(19) - Reserved.",
	  "(20) - Reserved.",
	  "(21) - Reserved.",
	  "(22) - Reserved.",
	  "(mmx) MMX technology",
	  "(24) - Reserved.",
	  "(kni/xmm) Streaming SIMD instructions.",
	  "(26) - Reserved.",
	  "(27) - Reserved.",
	  "(28) - Reserved.",
	  "(29) - Reserved.",
	  "(30) - Reserved.",
	  "(31) - Reserved."
	};

	/* Get the features. */
	cpuid (id->CPU_number, 1, &eax, &ebx, &ecx, &edx);

	/* Modify the capabilities according to chip type */
	switch (id->vendor) {
		case X86_VENDOR_CYRIX:
			x86_cap_flags[24] = "(cxmmx)";
			break;

		case X86_VENDOR_AMD:
			if (id->family == 5 && id->model == 6)
				x86_cap_flags[10] = "(sep)";
			if (id->family < 6)
				x86_cap_flags[16] = "(fcmov)";
			else
				x86_cap_flags[16] = "(pat) Page Attribute Table";
			x86_cap_flags[22] = "(mmxext)";
			x86_cap_flags[24] = "(fxsr)";
			x86_cap_flags[30] = "(3dnowext) Extended 3dNow! instructions.";
			x86_cap_flags[31] = "(3dnow) 3dNow! instructions.";
			cpuid (id->CPU_number, 0x80000001, &eax, &ebx, &ecx, &edx);
			if ((id->model == 13) ||
				(id->model == 9)  ||
				((id->model == 8) && (id->stepping>=8)))
				edx |= (1<<12);
			break;

		case X86_VENDOR_INTEL:
			x86_cap_flags[16] = "(pat) Page Attribute Table";
			x86_cap_flags[24] = "(fxsr)";
			break;

		case X86_VENDOR_CENTAUR:
			if (id->model >=8)   /* Only Winchip2 and above */
				x86_cap_flags[31] = "(3dnow) 3dNow! instructions.";
			break;

		default:
			/* Unknown CPU manufacturer or no special handling needed */
			break;
	}


	for (i=0 ; i < 32; i++) {
		tweak = alloc_CPU_tweak(id->CPU_number, TYPE_INFO_BOOL);
		if (tweak == NULL)
			return;
		pvt = tweak->PrivateData;

		tweak->WidgetText = strdup (x86_cap_flags[i]);
		if (edx & (1 << i))
			set_value_int (pvt->value, 1);
		else
			set_value_int (pvt->value, 0);

		if (strstr(tweak->WidgetText,"Reserved")!=NULL) {
			CPU_tweak_destructor(tweak);
			free(tweak);
		} else {
			AddTo_CPU_tree (id, tweak, Tab);
		}
	}
}


void Add_CPU_info_page (struct cpu_identity *id)
{
	struct tweak *tweak;
	struct private_CPU_data *private=NULL;

	char IdentityStr[]="Identity";
	char InfoStr[]="Information";
	char VendorIDStr[]="Vendor ID";
	char ProcessorStr[]="Processor";
	char FamilyStr[]="Family";
	char ModelStr[]="Model";
	char SteppingStr[]="Stepping";
	char BrandIDStr[]="Brand ID";


	if ((tweak = alloc_CPU_tweak(id->CPU_number, TYPE_INFO_INT)) != NULL) {
		private = tweak->PrivateData;
		tweak->WidgetText = strdup(FamilyStr);
		set_value_int(private->value, id->family);
		AddTo_CPU_treehframe (id, tweak, InfoStr, ModelStr);
	}

	if ((tweak = alloc_CPU_tweak(id->CPU_number, TYPE_INFO_INT)) != NULL) {
		private = tweak->PrivateData;
		tweak->WidgetText = strdup(ModelStr);
		set_value_int(private->value, id->model);
		AddTo_CPU_treehframe (id, tweak, InfoStr, ModelStr);
	}

	if ((tweak = alloc_CPU_tweak(id->CPU_number, TYPE_INFO_INT)) != NULL) {
		private = tweak->PrivateData;
		tweak->WidgetText = strdup(SteppingStr);
		set_value_int(private->value, id->stepping);
		AddTo_CPU_treehframe (id, tweak, InfoStr, ModelStr);
	}

	if (id->vendor == X86_VENDOR_INTEL) {
		if ((tweak = alloc_CPU_tweak(id->CPU_number, TYPE_INFO_INT)) != NULL) {
			private = tweak->PrivateData;
			tweak->WidgetText = strdup(BrandIDStr);
			set_value_int(private->value, id->brand);
			AddTo_CPU_treehframe (id, tweak, InfoStr, ModelStr);
		}
	}

	if ((tweak = alloc_CPU_tweak(id->CPU_number, TYPE_INFO_STRING)) != NULL) {
		private = tweak->PrivateData;
		tweak->WidgetText = strdup(VendorIDStr);
		if (id->VendorName!=NULL)
			private->value.strVal = strdup(id->VendorName);
		AddTo_CPU_treehframe (id, tweak, InfoStr, IdentityStr);
	}

	if ((tweak = alloc_CPU_tweak(id->CPU_number, TYPE_INFO_STRING)) != NULL) {
		private = tweak->PrivateData;
		tweak->WidgetText = strdup(ProcessorStr);
		private->value.strVal = strdup(id->CPUName);
		AddTo_CPU_treehframe (id, tweak, InfoStr, IdentityStr);
	}

	AddFeaturesPage(id);
	ShowMTRRInfo(id);
}

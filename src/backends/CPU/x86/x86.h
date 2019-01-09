#ifndef _INCLUDE_GUARD_X86_H
#define _INCLUDE_GUARD_X86_H

/*
 *  $Id: x86.h,v 1.13 2001/10/23 23:03:25 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <powertweak.h>

/* Vendors (x86) */
#define X86_VENDOR_AMD 0
#define X86_VENDOR_CENTAUR 1
#define X86_VENDOR_CYRIX 2
#define X86_VENDOR_INTEL 3
#define X86_VENDOR_NEXGEN 4
#define X86_VENDOR_RISE 5
#define X86_VENDOR_TRANSMETA 6
#define X86_VENDOR_UMC 7
#define X86_VENDOR_UNKNOWN 0xFF

/*
 * struct private_CPU_data contains everything
 * needed to get/set CPU tweaks from the GUI
 */
struct cpu_identity {
	int CPU_number;
	int vendor;
	int family;
	int model;
	int stepping;
	int type;
	int brand;
	int L1_cache_size;
	int L2_cache_size;  /* For CPUs with onboard L2 cache */
	char VendorName[16];
	char *CPUName;   /* 'made' by Powertweak. */
};

struct private_CPU_data {
	int CPU_number;
	/* MSR stuff */
	unsigned long MSR_Register;
	unsigned long long MSR_Mask;
	unsigned long long MSR_OnMask;
	unsigned long long MSR_OffMask;
	value_t value;
	char **cpuname;
};

void cpuid (int, int, unsigned long *, unsigned long *, unsigned long *, unsigned long *);
void cpuid_UP (int, unsigned long *, unsigned long *, unsigned long *, unsigned long *);

void identify_AMD(struct cpu_identity *id);
void identify_Cyrix(struct cpu_identity *id);
void identify_Intel(struct cpu_identity *id);
void identify_IDT(struct cpu_identity *id);
void identify_Rise(struct cpu_identity *id);
void identify_Transmeta(struct cpu_identity *id);

int TranslateVendor(char *vendorstr);
char * IDtoVendor (int vendorid);
int rdmsr(int cpu, unsigned long msrindex, unsigned long long *val);
int wrmsr(int cpu, unsigned long msrindex, unsigned long long *val);

void Add_CPU_tweaks(struct cpu_identity *id);
void Load_CPU_XML(struct cpu_identity *id);

int HaveCPUID(void);
int PowerNow_tweak(char *CPUNAME);

void append_to_CPU_name(struct cpu_identity *id, char *TXT);
void append_to_brand_name(struct cpu_identity *id, char *TXT);

void ShowMTRRInfo (struct cpu_identity *id);

#endif

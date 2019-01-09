/*
 *  $Id: mtrr.c,v 1.4 2001/10/19 03:14:36 davej Exp $
 *  This file is part of Powertweak Linux.
 *  (C) 2001 Dave Jones, Arjan van de Ven.
 *
 *  Licensed under the terms of the GNU GPL version 2.
 */

#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include <errno.h>
#define MTRR_NEED_STRINGS
#include <asm/mtrr.h>
#include <powertweak.h>
#include <x86.h>
#include "../cpu.h"

#define ERRSTRING strerror (errno)

void ShowMTRRInfo (struct cpu_identity *id)
{
	int fd;
	struct mtrr_gentry gentry;
	struct private_CPU_data *pvt;
	struct tweak *tweak;
	char regbuffer[12];
	char buffer[32];
	char mtrrtab[]="MTRR";

	if (id->CPU_number != 0)	/* Kernel will sync other CPUs. */
		return;

	if ( ( fd = open ("/proc/mtrr", O_RDONLY, 0) ) == -1 ) {
		if (errno == ENOENT)
			return;
	}

	for (gentry.regnum = 0; ioctl (fd, MTRRIOC_GET_ENTRY, &gentry) == 0; ++gentry.regnum) {

		sprintf (regbuffer, "Register: %d", gentry.regnum);

		if (gentry.size < 1) {
			tweak = alloc_CPU_tweak (0, TYPE_LABEL);
			if (tweak==NULL) return;
			pvt = tweak->PrivateData;
			tweak->WidgetText = strdup ("disabled");
			AddTo_CPU_treehframe (id, tweak, mtrrtab, regbuffer);
			continue;
		}

		tweak = alloc_CPU_tweak (0, TYPE_INFO_STRING);
		if (tweak==NULL) return;
		pvt = tweak->PrivateData;
		tweak->WidgetText = strdup ("base:");
		sprintf (buffer, "0x%lx", gentry.base);
		pvt->value.strVal = strdup (buffer);
		AddTo_CPU_treehframe (id, tweak, mtrrtab, regbuffer);

		tweak = alloc_CPU_tweak (0, TYPE_INFO_STRING);
		if (tweak==NULL) return;
		pvt = tweak->PrivateData;
		tweak->WidgetText = strdup ("size:");
		sprintf (buffer, "%ldMB", (gentry.size/1024)/1024);
		pvt->value.strVal = strdup (buffer);
		AddTo_CPU_treehframe (id, tweak, mtrrtab, regbuffer);

		tweak = alloc_CPU_tweak (0, TYPE_INFO_STRING);
		if (tweak==NULL) return;
		pvt = tweak->PrivateData;
		tweak->WidgetText = strdup ("type:");
		pvt->value.strVal = strdup (mtrr_strings[gentry.type]);
		AddTo_CPU_treehframe (id, tweak, mtrrtab, regbuffer);
	}

	close (fd);
}

/*
 *  $Id: tweaks.c,v 1.4 2001/10/02 00:57:09 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <assert.h>
#include <linux/hdreg.h>

#include <powertweak.h>
#include <config.h>
#include "hdparm.h"


static char *makemulti(int n)
{
	char *buf;

	buf = malloc (32);
	if (buf!=NULL) {
		if (n==0)
			snprintf (buf, 32, "No");
		else if (n==1)
			snprintf (buf, 32, "Yes, 1 sector");
		else
			snprintf (buf, 32, "Yes, %d sectors", n);
	}
	return buf;
}
		

void Add_Tweak_Page (char *MenuName, char *DeviceName, int fd)
{
	struct hd_driveid hd;
	struct private_hdparm_data *private;
	struct tweak *tweak;
	char *Menu1 = "Hardware";
	char *Menu2 = "Disk";
	char *Tabname = "Tweaks";
	char *bit32names[]={"Default", "32-bit", "16-bit", "32-bit w/sync"};
	char *Enabled = "Enabled";
	char *Disabled = "Disabled";
	char *mult;
	int dma, bits32, mask, multi;

	ioctl (fd, HDIO_GET_DMA, &dma);
	tweak = alloc_hdparm_tweak (DeviceName, TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("DMA");
	private = (struct private_hdparm_data *) tweak->PrivateData;
	if (dma)
		private->value.strVal = strdup (Enabled);
	else
		private->value.strVal = strdup (Disabled);
	private->Type = HD_GETDMA;
	RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "Transfer type.");

	ioctl (fd, HDIO_GET_32BIT, &bits32);
	tweak = alloc_hdparm_tweak (DeviceName, TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("IO Mode");
	private = (struct private_hdparm_data *) tweak->PrivateData;
	private->value.strVal = strdup (bit32names[bits32]);
	private->Type = HD_GET32BIT;
	RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "Transfer type.");

	memset(&hd, 0, sizeof(hd));
	ioctl (fd, HDIO_GET_IDENTITY, &hd);
	ioctl (fd, HDIO_GET_MULTCOUNT, &multi);
	mult = makemulti(multi);
	if ((hd.max_multsect) && (mult!=NULL)) {
		tweak = alloc_hdparm_tweak (DeviceName, TYPE_INFO_STRING);
		tweak->WidgetText = strdup ("Multisector");
		private = (struct private_hdparm_data *) tweak->PrivateData;
		private->value.strVal = mult;
		private->Type = HD_GETMULT;
		RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "Transfer type.");
	}

	ioctl (fd, HDIO_GET_UNMASKINTR, &mask);
	tweak = alloc_hdparm_tweak (DeviceName, TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("IRQ Unmasking");
	private = (struct private_hdparm_data *) tweak->PrivateData;
	if (mask)
		private->value.strVal = strdup (Enabled);
	else
		private->value.strVal = strdup (Disabled);
	private->Type = HD_GETUNMASK;
	RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "Misc features.");
}

/*
 *  $Id: smart.c,v 1.21 2001/09/29 18:14:44 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 *
 * Based on code from UCSC SMART (http://csl.cse.ucsc.edu/software/smart/
 * ataprint.c
 *
 * Copyright (C) 1999-2000 Michael Cornwell <cornwell@acm.org>
 * Copyright (C) 2000 Andre Hedrick <andre@linux-ide.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * You should have received a copy of the GNU General Public License
 * (for example COPYING); if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <unistd.h>

#include <powertweak.h>
#include "atacmds.h"


int can_enable_smart(int fd)
{
	struct hd_driveid drive;

	if (ataReadHDIdentity(fd,&drive) != 0)
		return 0;
	
	switch (ataSmartSupport(drive)) {
		case 0: /* no support */
			return 0;
		case 1: /* disabled */
			return 0;
		case 2:  /* enabled */
			return 0;
		case 255:
			return 1;
		default:
			return 0;
	}
}


int smartErrorCount(int fd)
{
	struct hd_driveid drive;
	struct ata_smart_values smartval;
	struct ata_smart_thresholds smartthres;
	struct ata_smart_errorlog smarterror;

	if (ataReadHDIdentity(fd,&drive) != 0)
		return -2;
	
	switch (ataSmartSupport(drive)) {
		case 0: /* no support */
			return -2;
		case 1: /* disabled */
			return -2;
		case 2:  /* enabled */
			break;
		case 255:
			if (ataSmartStatus(fd) !=0)
				return -1;
			break;
		default:
			return -2;
	}


	/* for everything else read values and thresholds 
	   are needed */

	if (ataReadSmartValues(fd, &smartval) != 0) 
		return -2;

	if (ataReadSmartThresholds(fd, &smartthres) != 0) 
		return -2;


	if (isSmartErrorLogCapable(smartval) == 0) {
		return -2;	/* device not capable */
	} else {
		if (ataReadErrorLog(fd, &smarterror) != 0) {
			return -2;	/* Errorlog failed */
		} else {
			return smarterror.ata_error_count;
		}
	}
	return -2;
}


struct smart_private {
	char *DeviceName;
	int enabled;
};


static value_t smart_get_value(struct tweak *tweak)
{
	value_t val={0,NULL};
	struct smart_private *pvt;
	int fd;

	pvt=tweak->PrivateData;
	if (pvt==NULL)
		return val;
	fd = open(pvt->DeviceName,O_RDWR);
	if (fd<2)
		return val;

	set_value_int(val,smartErrorCount(fd));
	close (fd);

	return val;	
}


static value_t smart_enable_get_value(struct tweak *tweak)
{
	value_t val={0,NULL};
	struct smart_private *pvt;
	if (tweak==NULL)
		return val;
	pvt=tweak->PrivateData;
	if (pvt==NULL)
		return val;

	set_value_int(val,pvt->enabled);	

	return val;	
}


static void smart_enable_set_value(struct tweak *tweak, value_t value, int immediate)
{
	struct smart_private *pvt;
	int fd;

	pvt=tweak->PrivateData;
	if (pvt==NULL)
		return;

	fd = open(pvt->DeviceName,O_RDWR);
	if (fd<2)
		return;

	pvt->enabled = get_value_int(value);
	if (immediate==0)
		return;

	if (pvt->enabled) {
		ataEnableSmart(fd);
	} else {
		if (ataSmartStatus(fd)==0)
			ataDisableSmart(fd);
	}
	close (fd);
	usleep(5000);
}


static void smart_destroy(struct tweak *tweak)
{
	struct smart_private *pvt;

	default_destructor(tweak);

	pvt=tweak->PrivateData;
	if (pvt!=NULL) {
		free (pvt->DeviceName);
		free (pvt);
	}
}


static struct tweak *SmartEnableTweak(char *hddname, int fd)
{
	struct tweak *tweak;
	struct smart_private *pvt;
	char Buffer[200];

	tweak = alloc_tweak(TYPE_CHECKBOX);
	pvt = calloc(1,sizeof(struct smart_private));
	assert(pvt!=NULL);
	tweak->PrivateData = pvt;

	pvt->enabled = !ataSmartStatus(fd);
	pvt->DeviceName = strdup (hddname);

	tweak->WidgetText = strdup("S.M.A.R.T. Enable");
	tweak->Description = strdup("Check this box to enable S.M.A.R.T.\n"
	"SMART is a protocol for harddrive self-diagnostics\n");
	snprintf (Buffer, CONFIGNAME_MAXSIZE-1, "SMART_ENABLE_%s", hddname);
	tweak->ConfigName = strdup(Buffer);
	tweak->GetValue = smart_enable_get_value;
	tweak->ChangeValue = smart_enable_set_value;
	tweak->Destroy = smart_destroy;

	return tweak;
}


void AddSmart(char *Menuname, char *hddname)
{
	struct tweak *tweak,*enabletweak;
	struct smart_private *pvt;
	char *Menu1 = "Hardware";
	char *Menu2 = "Disk";
	char *Info = "Information";
	int fd;

	fd = open(hddname,O_RDWR);
	if (fd<2)
		return;

	/* is smart supported ? */
	if (smartErrorCount(fd)<-1)	{/* no */ 
		close (fd);
		return;
	}

	tweak = alloc_tweak(TYPE_INFO_INT);
	if (tweak==NULL) {
		close (fd);
		return;
	}
	pvt = calloc(1,sizeof(struct smart_private));
	assert(pvt!=NULL);
	tweak->PrivateData = pvt;

	tweak->WidgetText = strdup("S.M.A.R.T. Error count");
	tweak->Description = strdup("The number of errors according to the harddrive.\n"
	"High and/or increasing numbers indicate a dying drive.\n");
	tweak->ConfigName = NULL;
	tweak->GetValue = smart_get_value;
	tweak->Destroy = smart_destroy;
	pvt->DeviceName = strdup (hddname);

	if (can_enable_smart(fd)) {
		enabletweak = SmartEnableTweak(hddname,dup(fd));
		if (enabletweak!=NULL)
			RegisterTweak (enabletweak, "mmmtf", Menu1, Menu2, Menuname, Info, "S.M.A.R.T.");
	}

	RegisterTweak (tweak, "mmmtf", Menu1, Menu2, Menuname, Info, "S.M.A.R.T.");

	close (fd);
}

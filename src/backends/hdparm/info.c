/*
 *  $Id: info.c,v 1.15 2003/07/06 23:03:05 svenud Exp $
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

#include <errno.h>

#include <powertweak.h>
#include <config.h>

#include "hdparm.h"


static void hdparmRequestStandBy(struct tweak pt_unused *tweak, value_t pt_unused value, int pt_unused immediate)
{
	struct private_hdparm_data *private;
	private = (struct private_hdparm_data *) tweak->PrivateData;

	log_message("asked %s to go standby (NotImplementedYet)\n", private->DeviceName);
}

static void hdparmRequestSuspend(struct tweak pt_unused *tweak, value_t pt_unused value, int pt_unused immediate)
{
	struct private_hdparm_data *private;
	private = (struct private_hdparm_data *) tweak->PrivateData;

	log_message("asked %s to go suspend (NotImplementedYet)\n", private->DeviceName);
}

#ifdef HDIO_SET_ACOUSTIC
static void hdparmChangeAcoustic(struct tweak pt_unused *tweak, value_t pt_unused value, int pt_unused immediate)
{
	struct private_hdparm_data *private;
	int fd;

	private = (struct private_hdparm_data *) tweak->PrivateData;

	set_value_int (private->value, get_value_int (value));
	if (immediate==0)
		return;
	
	fd = open (private->DeviceName, O_RDONLY | O_NONBLOCK);
	if (fd != -1) 
	{
		!ioctl (fd, HDIO_SET_ACOUSTIC, get_value_int(value));
		close(fd);
	}
}
#endif /*HDIO_SET_ACOUSTIC*/


static char * make_buffers (struct hd_driveid *hd)
{
	char *bufptr;
	char buffer[32];
	char onesec[] = "1 Sector";
	char dualport[] = "Dual Port";
	char dualportcache[] = "Dual Port Cache";
	int sz;

	if (hd->buf_size == 0)
	{
		sprintf(buffer, "None");
		return strdup(buffer);
	}

	bufptr = buffer;
	sz = snprintf (buffer, 32, "%dKb ", hd->buf_size / 2);
	bufptr += sz;
	sz = 32 - sz;

	switch (hd->buf_type)
	{
	case 1:
		snprintf (bufptr, sz, "%s", onesec);
		break;
	case 2:
		snprintf (bufptr, sz, "%s", dualport);
		break;
	case 3:
		snprintf (bufptr, sz, "%s", dualportcache);
		break;
	default:
		snprintf (bufptr, sz, "Unknown type [%d]", hd->buf_type);
		break;
	}
	return strdup(buffer);
}


static char * make_geometry (struct hd_driveid *hd)
{
	char *buf;
	buf = malloc (32);
	if (buf != NULL)
		snprintf (buf, 32, "%d/%d/%d", hd->cyls, hd->heads,
			  hd->sectors);
	return buf;
}

#ifdef HDIO_GET_ACOUSTIC
static int hdparmGetAcousticRaw(struct tweak *tweak)
{
	int ret = 0;
	struct private_hdparm_data *private;
	int fd;

	private = (struct private_hdparm_data *) tweak->PrivateData;

	fd = open (private->DeviceName, O_RDONLY | O_NONBLOCK);
	if (fd != -1) 
	{
		long parm;
		
		if (!ioctl (fd, HDIO_GET_ACOUSTIC, &parm))
		{
			private->value.intVal = parm;
			private->value.strVal = NULL;
			private->Type = HD_GET_ACOUSTIC;
			ret = 1;
		}
		
		close(fd);
	}

	return ret;
}
#endif /*HDIO_GET_ACOUSTIC*/


static int hdparmGetPowerModeRaw(struct tweak *tweak)
{
	struct private_hdparm_data *private;
	char	*apm_buf = NULL;
	int fd;

	private = (struct private_hdparm_data *) tweak->PrivateData;
	
	fd = open (private->DeviceName, O_RDONLY | O_NONBLOCK);
	if (fd != -1) 
	{
		apm_buf = malloc (32);
		memset(apm_buf, 0, 32);
		if (apm_buf != NULL)
		{
			unsigned char args[4] = { WIN_CHECKPOWERMODE1, 0, 0, 0 };
			const char *state;
	
			if (ioctl (fd, HDIO_DRIVE_CMD, &args)
				&& (args[0] = WIN_CHECKPOWERMODE2)	 /*try again with 0x98 */
				&& ioctl (fd, HDIO_DRIVE_CMD, &args))
			{
				if ( errno != EIO ||  args[0] != 0
					|| args[1] != 0)
					state = "unknown\0";
				else
					state = "sleeping\0";
			}
			else
			{
				state = (args[2] == 255) ? "active/idle\0" : "standby\0";
			}
			snprintf (apm_buf, 32, "%s", state);
			
			if (private->value.strVal != NULL)
			{
				free(private->value.strVal );
			}
			private->value.strVal = apm_buf;
			private->Type = HD_GET_APM;
		}
		close(fd);
	}

	if (apm_buf != NULL)
	{
		if (strlen(apm_buf) > 0)
			return 1;
	}
	return 0;
}

void Add_Info_Page (char *MenuName, char *DeviceName, int fd)
{
	struct hd_driveid hd;
	struct private_hdparm_data *private;
	struct tweak *tweak;
	char *Menu1 = "Hardware";
	char *Menu2 = "Disk";
	char *Tabname = "Information";
	char *bufptr, *geom;
	int id;

	memset (&hd, 0, sizeof (hd));
	id = ioctl (fd, HDIO_GET_IDENTITY, &hd);
	tweak = alloc_hdparm_tweak (DeviceName, TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Model");
	private = (struct private_hdparm_data *) tweak->PrivateData;
	private->Type = HD_GETID;
	if (id == -1)
	{			/* Olde drive, can't ID it. */
		private->value.strVal = strdup ("Legacy MFM/RLL drive");
		RegisterTweak (tweak, "mmmt", Menu1, Menu2, MenuName, Tabname);
		return;
	}
	private->value.strVal = strdup (hd.model);
	RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "ID.");

	tweak = alloc_hdparm_tweak (DeviceName, TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Firmware");
	private = (struct private_hdparm_data *) tweak->PrivateData;
	private->value.strVal = malloc (9);
	strncpy (private->value.strVal, hd.fw_rev, 8);
	memset (private->value.strVal + 8, 0, 1);
	private->Type = HD_GETFIRMWARE;
	RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "ID.");

	tweak = alloc_hdparm_tweak (DeviceName, TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Serial No.");
	private = (struct private_hdparm_data *) tweak->PrivateData;
	private->value.strVal = strdup (hd.serial_no);
	private->Type = HD_GETSERIAL;
	RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "ID.");

	geom = make_geometry (&hd);
	if ((hd.cyls + hd.heads + hd.sectors != 0) && (geom != NULL))
	{
		tweak = alloc_hdparm_tweak (DeviceName, TYPE_INFO_STRING);
		tweak->WidgetText = strdup ("Geometry");
		private = (struct private_hdparm_data *) tweak->PrivateData;
		private->value.strVal = strdup(geom);
		private->Type = HD_GETGEOMETRY;
		RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "Drive Geometry.");
	}
	free(geom);

	bufptr = make_buffers (&hd);
	if (bufptr != NULL)
	{
		tweak = alloc_hdparm_tweak (DeviceName, TYPE_INFO_STRING);
		tweak->WidgetText = strdup ("Cache");
		private = (struct private_hdparm_data *) tweak->PrivateData;
		private->value.strVal = bufptr;
		private->Type = HD_GETCACHE;
		RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "Cache information.");
	}
	else
	{
		free(bufptr);
	}

#ifdef HDIO_GET_ACOUSTIC
	tweak = alloc_hdparm_tweak (DeviceName, TYPE_SLIDER);
	tweak->WidgetText = strdup ("Acoustic (128=quiet ... 254=fast)");
	tweak->MinValue = 128;
	tweak->MaxValue = 254;
	tweak->GetValueRaw = &hdparmGetAcousticRaw;
#ifdef HDIO_SET_ACOUSTIC
	tweak->ChangeValue = &hdparmChangeAcoustic;
#endif /*HDIO_SET_ACOUSTIC*/
	if (tweak->GetValueRaw(tweak))
	{
		RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "Acoustic");
	}
	else
	{
		hdparm_tweak_destructor(tweak);
		default_destructor(tweak);
	}
#endif /*HDIO_GET_ACOUSTIC*/

	tweak = alloc_hdparm_tweak (DeviceName, TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Drive State");
	tweak->GetValueRaw = &hdparmGetPowerModeRaw;
	if (tweak->GetValueRaw(tweak))
	{	
		RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "state");
		
		/*add a button that should use the changevalue for DriveState (to send a standby command)*/
		tweak = alloc_hdparm_tweak (DeviceName, TYPE_BUTTON);
		tweak->WidgetText = strdup ("Request Standby");
		tweak->ChangeValue = &hdparmRequestStandBy;
		RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "state");

		/*add a button that should use the changevalue for DriveState (to send a standby command)*/
		tweak = alloc_hdparm_tweak (DeviceName, TYPE_BUTTON);
		tweak->WidgetText = strdup ("Request Suspend");
		tweak->ChangeValue = &hdparmRequestSuspend;
		RegisterTweak (tweak, "mmmth", Menu1, Menu2, MenuName, Tabname, "state");
	}
	else
	{
		hdparm_tweak_destructor(tweak);
	}
}

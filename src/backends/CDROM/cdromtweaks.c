/*
 *  $Id: cdromtweaks.c,v 1.14 2001/09/16 02:17:42 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000, 2001 Dave Jones, Arjan van de Ven.
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
#include <linux/cdrom.h>
#include <sys/ioctl.h>
#include <asm/errno.h>

#include <powertweak.h>
#include <config.h>
#include "cdrom.h"


static void CDROM_tweak_destructor (struct tweak *tweak)
{
	struct private_CDROM_data *private;
	assert (tweak!=NULL);

	private = (struct private_CDROM_data*)tweak->PrivateData;
	assert (private!=NULL);

	default_destructor(tweak);

	release_value(&private->value);
	free (private->DeviceName);
	free (tweak->PrivateData);
}


static value_t CDROM_get_value(struct tweak *tweak)
{
	struct private_CDROM_data *private;
	value_t localvalue = {0,NULL};

	assert (tweak!=NULL);
	private = (struct private_CDROM_data*)tweak->PrivateData;
	assert (private!=NULL);

	copy_values(&localvalue,private->value);
	return localvalue;
}


static int CDROM_get_rawvalue (struct tweak *tweak)
{
	struct private_CDROM_data *private;
	char *currentspeedtxt = "current_speed";
	char IDEPathName[24];
	char Buffer[80];
	int currentspeed=0;
	FILE *procfd;

	private = (struct private_CDROM_data *) tweak->PrivateData;

	/* Parse settings file to get currentspeed */
	memset (IDEPathName, 0, sizeof(IDEPathName));
	snprintf (IDEPathName, sizeof(IDEPathName), "/proc/ide%s/settings", private->DeviceName+4);	/* The +4 skips /dev */
	procfd = fopen (IDEPathName, "r");
	if (procfd) {
		while (fgets (Buffer, 80, procfd) != NULL) {
			if (strstr(Buffer, currentspeedtxt) != NULL) {
				sscanf (Buffer+24, "%d", &currentspeed);	/* value */
				sscanf (Buffer+40, "%d", &tweak->MinValue);	/* min */
				sscanf (Buffer+56, "%d", &tweak->MaxValue);	/* max */
				break;
			}
		}

		fclose (procfd);
	} else {
		return FALSE;
	}

	set_value_int (private->value, currentspeed);
	return TRUE;
}


static void CDROM_change_value (struct tweak *tweak, value_t value, int immediate)
{
	struct private_CDROM_data *private;
	int fd;

	assert (tweak!=NULL);
	private = (struct private_CDROM_data *) tweak->PrivateData;
	assert (private!=NULL);

	set_value_int (private->value, get_value_int (value));
	if (immediate==0)
		return;

	fd = open (private->DeviceName, O_RDONLY | O_NONBLOCK);
	if (fd == -1)
		return;

	ioctl (fd, CDROM_SELECT_SPEED, get_value_int (value));
	close (fd);
}


static struct tweak *alloc_CDROM_tweak (int type)
{
	struct tweak *tweak;
	struct private_CDROM_data *private;
	
	tweak = alloc_tweak(type);
	if (tweak==NULL)
		return NULL;

	private = malloc (sizeof (struct private_CDROM_data));
	if (private == NULL) {
		printf ("Out of memory\n");
		free (tweak);
		return NULL;
	}

	memset(private,0,sizeof(struct private_CDROM_data));
	tweak->PrivateData = (void *)private;
	tweak->Destroy = &CDROM_tweak_destructor;
	tweak->GetValue = &CDROM_get_value;
	tweak->GetValueRaw = &CDROM_get_rawvalue;
	tweak->ChangeValue = &CDROM_change_value;
	return tweak;
}


static void AddTo_CDROM_tree (char *MenuName, char *DeviceName, int fd)
{
	struct private_CDROM_data *private;
	struct tweak *tweak;
	char *Menu1 = "Hardware";
	char *Menu2 = "Disk";
	char *Tabname = "CDROM";
	char Buffer[CONFIGNAME_MAXSIZE];
	int caps;

	/* Is this a CDROM ? */
	caps = ioctl (fd, CDROM_GET_CAPABILITY);
	if (caps == -1)
		return;

	tweak = alloc_CDROM_tweak(TYPE_SLIDER);
	private = (struct private_CDROM_data *) tweak->PrivateData;

	snprintf (Buffer, CONFIGNAME_MAXSIZE-1,"%s speed", MenuName);
	tweak->WidgetText = strdup (Buffer);
	tweak->Description = strdup ("This parameter controls the\n\
head-rate of the cdrom player to 150KB/s times this value.\n\
The value 0 is interpreted as auto-selection.\n\
Note, that not all drives support this correctly.");
	snprintf (Buffer,CONFIGNAME_MAXSIZE-1, "%s_CDROM_SPEED", MenuName);
	tweak->ConfigName = strdup (Buffer);
	private->DeviceName = strdup (DeviceName);
	if (CDROM_get_rawvalue(tweak) == TRUE)
		RegisterTweak (tweak, "mmmt", Menu1, Menu2, MenuName, Tabname);
	else {
		tweak->Destroy(tweak);
		free (tweak);
	}
}


/*
 * Build the tree of device names that we can change the
 * elevator values for.
 */
int InitPlugin (pt_unused int showinfo)
{
	struct kernel_ver kv;
	unsigned int i;
	int fd;
	static char *devs[]= {
		"hda", "hdb", "hdc", "hdd", "hde", "hdf", "hdg", "hdh",
		"sda", "sdb", "sdc", "sdd", "sde", "sdf", "sdg", "sdh", };

	char devicebuf[9];

	kernel_version(&kv);
	if (kv.major <2)	/* 1.x.x */
		return 0;
	if (kv.minor <2)	/* We don't want 2.1.x */
		return 0;
	if (kv.minor == 2 && kv.patchlevel < 4) /* <2.2.4 ? */
		return 0;

	for (i=0 ; i<(sizeof (devs)/sizeof(devs[0])); i++) {
		snprintf (devicebuf, 9,"/dev/%s", devs[i]);
		fd = open (devicebuf, O_RDONLY | O_NONBLOCK);
		if (fd != -1) {
			AddTo_CDROM_tree (devs[i], devicebuf, fd);
			close (fd);
		}
	}
	return 1;
}

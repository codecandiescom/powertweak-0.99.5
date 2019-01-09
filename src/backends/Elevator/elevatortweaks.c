/*
 *  $Id: elevatortweaks.c,v 1.13 2001/10/01 03:29:22 davej Exp $
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

#include <powertweak.h>
#include <config.h>
#include "elevator.h"

static void Elevator_tweak_destructor (struct tweak *tweak)
{
	struct private_Elevator_data *private;

	default_destructor(tweak);

	private = (struct private_Elevator_data*)tweak->PrivateData;
	if (private!=NULL) {
		free (private->DeviceName);
		free (tweak->PrivateData);
	}
}


static value_t Elevator_get_value(struct tweak *tweak)
{
	struct private_Elevator_data *private;
	value_t localvalue = {0,NULL};

	assert (tweak!=NULL);
	private = (struct private_Elevator_data*)tweak->PrivateData;
	assert (private!=NULL);

	set_value_int(localvalue, private->value);
	return localvalue;
}

static int Elevator_get_value_raw(struct tweak *tweak)
{
	struct private_Elevator_data *private;
	int fd;
	blkelv_ioctl_arg_t elevator;

	private = (struct private_Elevator_data *) tweak->PrivateData;
	assert (private!=NULL);

	fd = open (private->DeviceName, O_RDONLY | O_NONBLOCK);
	if (fd == -1)
		return FALSE;

	if (ioctl (fd, BLKELVGET, &elevator) <0) {
		close (fd);
		return FALSE;
	}
	close (fd);

	switch (private->type) {

		case ELV_READ:
			private->value = elevator.read_latency; 
			break;
		case ELV_WRITE:
			private->value = elevator.write_latency;
			break;
		case ELV_BOMB:
			private->value = elevator.max_bomb_segments;
			break;
		default:
			/* If we get here, we don't know what the hell happened. */
			return FALSE;
	}

	
	return TRUE;
}


static void Elevator_change_value (struct tweak *tweak, value_t value, int immediate)
{
	struct private_Elevator_data *private;
	blkelv_ioctl_arg_t elevator;
	int fd;

	private = (struct private_Elevator_data *) tweak->PrivateData;
	assert (private!=NULL);

	private->value = get_value_int (value);

	if (immediate==0)
		return;

	fd = open (private->DeviceName, O_RDONLY | O_NONBLOCK);
	if (fd == -1)
		return;

	if (ioctl (fd, BLKELVGET, &elevator) <0)
		return;

	switch (private->type) {

		case ELV_READ:
			elevator.read_latency = get_value_int (value);
			break;
		case ELV_WRITE:
			elevator.write_latency = get_value_int (value);
			break;
		case ELV_BOMB:
			elevator.max_bomb_segments = get_value_int (value);
			break;
		default:
			/* If we get here, we don't know what the hell happened. */
			return;
	}

	ioctl (fd, BLKELVSET, &elevator);
	close (fd);
}

static struct tweak *alloc_Elevator_tweak (int type)
{
	struct tweak *tweak;
	struct private_Elevator_data *private;
	
	tweak = alloc_tweak(type);

	private = malloc (sizeof (struct private_Elevator_data));
	if (private == NULL) {
		printf ("Out of memory\n");
		free (tweak);
		return (NULL);
	}

	memset(private,0,sizeof(struct private_Elevator_data));
	tweak->PrivateData = (void *)private;
	tweak->Destroy = &Elevator_tweak_destructor;
	tweak->GetValue = &Elevator_get_value;
	tweak->GetValueRaw = &Elevator_get_value_raw;
	tweak->ChangeValue = &Elevator_change_value;
	return tweak;
}


static void AddTo_Elevator_tree (char *MenuName, char *DeviceName, int fd)
{
	struct private_Elevator_data *private;
	struct tweak *tweak;
	blkelv_ioctl_arg_t elevator;
	char *Menu1 = "Hardware";
	char *Menu2 = "Disk";
	char *Tabname = "Elevator";
	char Buffer[CONFIGNAME_MAXSIZE];

	if (ioctl (fd, BLKELVGET, &elevator) <0)
		return;

	tweak = alloc_Elevator_tweak(TYPE_SLIDER);
	private = (struct private_Elevator_data *) tweak->PrivateData;
	private->type = ELV_READ;
	tweak->MinValue = 0;
	tweak->MaxValue = 128000;
	snprintf (Buffer, CONFIGNAME_MAXSIZE-1,"%s Read Latency", MenuName);
	tweak->WidgetText = strdup (Buffer);
	tweak->Description = strdup ("The read latency parameter controls\n\
how hard the kernel tries to group disk read-requests.\n\
higher values mean better grouping and higher throughput\n\
from the disk, but longer latency.");
	snprintf (Buffer,CONFIGNAME_MAXSIZE-1, "%s_ELEVATOR_READ_LATENCY", MenuName);
	tweak->ConfigName = strdup (Buffer);
	private->value = elevator.read_latency;
	private->DeviceName = strdup (DeviceName);
	RegisterTweak (tweak, "mmmt", Menu1, Menu2, MenuName, Tabname);

	tweak = alloc_Elevator_tweak(TYPE_SLIDER);
	private = (struct private_Elevator_data *) tweak->PrivateData;
	private->type = ELV_WRITE;
	tweak->MinValue = 0;
	tweak->MaxValue = 128000;
	snprintf (Buffer,CONFIGNAME_MAXSIZE-1, "%s Write Latency", MenuName);
	tweak->WidgetText = strdup (Buffer);
	tweak->Description = strdup ("The write latency parameter controls\n\
how hard the kernel tries to group disk write-requests.\n\
higher values mean better grouping and higher throughput\n\
from the disk, but can possibly affect read performance.");
	snprintf (Buffer, CONFIGNAME_MAXSIZE-1,"%s_ELEVATOR_WRITE_LATENCY", MenuName);
	tweak->ConfigName = strdup (Buffer);
	private->value = elevator.write_latency;
	private->DeviceName = strdup (DeviceName);
	RegisterTweak (tweak, "mmmt", Menu1, Menu2, MenuName, Tabname);

	/* If max bomb segments is 0, it's unsupported. */
	if (elevator.max_bomb_segments != 0) {
		tweak = alloc_Elevator_tweak(TYPE_SLIDER);
		private = (struct private_Elevator_data *) tweak->PrivateData;
		private->type = ELV_BOMB;
		tweak->MinValue = 1;
		tweak->MaxValue = 128;
		snprintf (Buffer,CONFIGNAME_MAXSIZE-1, "%s Max bomb segments", MenuName);
		tweak->WidgetText = strdup (Buffer);
	tweak->Description = strdup ("The 'Max bomb segments' parameter\n\
controls how much write-requests are grouped in the presence of reads.\n\
higher values mean more grouping but increase read-latency.");
		snprintf (Buffer, CONFIGNAME_MAXSIZE-1,"%s_ELEVATOR_BOMB_SEGMENTS", MenuName);
		tweak->ConfigName = strdup (Buffer);
		private->value = elevator.read_latency;
		private->DeviceName = strdup (DeviceName);
		RegisterTweak (tweak, "mmmt", Menu1, Menu2, MenuName, Tabname);
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
	char *devs[]= {
		"hda", "hdb", "hdc", "hdd", "hde", "hdf", "hdg", "hdh",
		"sda", "sdb", "sdc", "sdd", "sde", "sdf", "sdg", "sdh", };
	/* TODO: Add support here for cpq & DAC drivers. */

	int retval = FALSE;
	char devicebuf[9];

	kernel_version(&kv);
	if (kv.major <2)	/* 1.x.x */
		return FALSE;
	if (kv.minor <2)	/* We don't want 2.1.x */
		return FALSE;
	if (kv.minor == 2 && kv.patchlevel < 16) /* <2.2.16 ? */
		return FALSE;
	if (kv.minor == 3 && kv.patchlevel < 99)	/* < 2.3.99 ? */
		return FALSE;

	for (i=0 ; i<(sizeof (devs)/sizeof(devs[0])); i++) {
		snprintf (devicebuf, 9,"/dev/%s", devs[i]);
		fd = open (devicebuf, O_RDONLY | O_NONBLOCK);
		if (fd != -1) {
			AddTo_Elevator_tree (devs[i], devicebuf, fd);
			close (fd);
			retval = TRUE;
		}
	}
	return retval;
}

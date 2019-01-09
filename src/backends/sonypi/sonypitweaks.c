/*
 *  $Id: sonypitweaks.c,v 1.8 2001/10/01 03:25:42 davej Exp $
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
#include <sys/utsname.h>

#include <powertweak.h>
#include <config.h>

#include "sonypi.h"

static char sonydev[]="/dev/sonypi";

static void sonypi_tweak_destructor (struct tweak *tweak)
{
	struct private_sonypi_data *private;

	default_destructor(tweak);

	private = (struct private_sonypi_data*)tweak->PrivateData;
	if (private!=NULL) {
		release_value(&private->value);
		free (tweak->PrivateData);
	}
}


static value_t sonypi_get_value(struct tweak *tweak)
{
	struct private_sonypi_data *private;
	value_t localvalue = {0,NULL};

	assert (tweak!=NULL);
	private = (struct private_sonypi_data*)tweak->PrivateData;
	assert (private!=NULL);

	copy_values(&localvalue,private->value);
	return localvalue;
}


static int sonypi_get_value_raw (struct tweak *tweak)
{
	struct private_sonypi_data *private;
	int fd;
	char brightness;

	private = (struct private_sonypi_data *) tweak->PrivateData;

	fd = open (sonydev, O_RDONLY | O_NONBLOCK);
	if (fd == -1)
		return FALSE;

	if (ioctl (fd, SONYPI_IOCGBRT, &brightness) <0) {
		close (fd);
		return FALSE;
	}

	close (fd);

	set_value_int (private->value, brightness);
	return TRUE;
}


static void sonypi_change_value (struct tweak *tweak, value_t value, int immediate)
{
	struct private_sonypi_data *private;
	int fd;
	char brightness;

	assert (tweak!=NULL);
	private = (struct private_sonypi_data *) tweak->PrivateData;
	assert (private!=NULL);

	set_value_int (private->value, get_value_int (value));

	if (immediate==0)
		return;

	fd = open (sonydev, O_RDONLY | O_NONBLOCK);
	if (fd == -1)
		return;

	brightness = get_value_int (private->value);
	ioctl (fd, SONYPI_IOCSBRT, &brightness);

	close (fd);
}

static struct tweak *alloc_sonypi_tweak (int type)
{
	struct tweak *tweak;
	struct private_sonypi_data *private;
	
	tweak = alloc_tweak(type);

	private = malloc (sizeof (struct private_sonypi_data));
	if (private == NULL) {
		printf ("Out of memory\n");
		free (tweak);
		return (NULL);
	}

	memset(private,0,sizeof(struct private_sonypi_data));
	tweak->PrivateData = (void *)private;
	tweak->Destroy = &sonypi_tweak_destructor;
	tweak->GetValue = &sonypi_get_value;
	tweak->GetValueRaw = &sonypi_get_value_raw;
	tweak->ChangeValue = &sonypi_change_value;
	return tweak;
}


static void AddTo_sonypi_tree (int fd)
{
	struct private_sonypi_data *private;
	struct tweak *tweak;
	char *Menu1 = "Hardware";
	char *Menu2 = "Sony VAIO LCD";
	char *Tabname = "Brightness";
	char Buffer[CONFIGNAME_MAXSIZE];
	char brightness;

	if (ioctl (fd, SONYPI_IOCGBRT, &brightness) <0)
		return;

	tweak = alloc_sonypi_tweak(TYPE_SPINBOX);
	private = (struct private_sonypi_data *) tweak->PrivateData;
	set_value_int (private->value, brightness);
	tweak->MinValue = 0;
	tweak->MaxValue = 255;
	snprintf (Buffer, CONFIGNAME_MAXSIZE-1,"LCD brightness");
	tweak->WidgetText = strdup (Buffer);
	tweak->Description = strdup ("This controls the brightness of\n\
the LCD backlight. Darkening the screen may increase battery life.\n");
	snprintf (Buffer,CONFIGNAME_MAXSIZE-1, "VAIO_LCD_BRIGHTNESS");
	tweak->ConfigName = strdup (Buffer);
	set_value_int (private->value, brightness);
	RegisterTweak (tweak, "mmt", Menu1, Menu2, Tabname);
}


/*
 * See if we can detect a vaio using the sonypi device.
 */
int InitPlugin (pt_unused int showinfo)
{
	int fd;

	fd = open (sonydev, O_RDONLY | O_NONBLOCK);
	if (fd != -1) {
		AddTo_sonypi_tree (fd);
		close (fd);
		return TRUE;
	}
	return FALSE;
}

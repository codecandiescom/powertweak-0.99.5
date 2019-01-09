/*
 *  $Id: hdparm.c,v 1.11 2003/04/18 11:59:26 svenud Exp $
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
#include "smart.h"

/*
 * note, this backend is not aware of GetValueRaw yet, as all
 * the available tweaks are read-only, and shouldn't ever change
 * whilst the daemon is running.
 */

void hdparm_tweak_destructor (struct tweak *tweak)
{
	struct private_hdparm_data *private;

	default_destructor(tweak);

	private = (struct private_hdparm_data*)tweak->PrivateData;
	if (private!=NULL) {
		release_value(&private->value);
		if (private->DeviceName)
			free (private->DeviceName);
		free (tweak->PrivateData);
	}
}


static value_t hdparm_get_value(struct tweak *tweak)
{
	struct private_hdparm_data *private;
	value_t localvalue = {0,NULL};

	assert (tweak!=NULL);
	private = (struct private_hdparm_data*)tweak->PrivateData;
	assert (private!=NULL);

	copy_values(&localvalue,private->value);
	return localvalue;
}


struct tweak *alloc_hdparm_tweak (char *devname, int type)
{
	struct tweak *tweak;
	struct private_hdparm_data *private;

	tweak = alloc_tweak(type);

	private = malloc (sizeof (struct private_hdparm_data));
	if (private == NULL) {
		printf ("Out of memory\n");
		free (tweak);
		return (NULL);
	}

	memset(private,0,sizeof(struct private_hdparm_data));
	tweak->PrivateData = (void *)private;
	tweak->Destroy = &hdparm_tweak_destructor;
	tweak->GetValue = &hdparm_get_value;
	tweak->GetValueRaw = NULL;
	tweak->ChangeValue = NULL;
	private->DeviceName = strdup (devname);
	return tweak;
}


/*
 * Build the tree of device names.
 */
int InitPlugin (int showinfo)
{
	unsigned int i;
	int fd;
	char *devs[]= {
		"hda", "hdb", "hdc", "hdd", "hde", "hdf", "hdg", "hdh", };
	char devicebuf[9];

	for (i=0 ; i<(sizeof (devs)/sizeof(devs[0])); i++) {
		snprintf (devicebuf, sizeof(devicebuf),"/dev/%s", devs[i]);
		fd = open (devicebuf, O_RDONLY | O_NONBLOCK);
		if (fd != -1) {
			if (showinfo==TRUE) {
				Add_Info_Page (devs[i], devicebuf, fd);
				Add_Tweak_Page (devs[i], devicebuf, fd);
			}
			AddSmart(devs[i],devicebuf);
			close (fd);
		}
	}
	return 1;
}

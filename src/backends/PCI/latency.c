/*
 *  $Id: latency.c,v 1.3 2001/11/07 00:36:13 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 * PCI backend core.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <powertweak.h>
#include <string.h>

#include <pci.h>
#include <pciutils.h>


static int add_latency_tweak (struct pci_dev *dev)
{
	struct tweak *tweak;
	struct private_PCI_data *private;
	word cmd;
	char *Tab = "Tweaks";
	char *FrameName = "PCI Latency";
	int max_lat,min_lat;

	cmd = pci_read_word (dev, PCI_COMMAND);
	if (!(cmd & PCI_COMMAND_MASTER))
		return FALSE;

	tweak = alloc_PCI_tweak (dev, TYPE_SLIDER);
	if (tweak == NULL)
		return FALSE;

	tweak->ConfigName=malloc(CONFIGNAME_MAXSIZE);
	if (tweak->ConfigName == NULL) {
		if (tweak->Destroy!=NULL)
			tweak->Destroy(tweak);
		free (tweak);
		return FALSE;
	}

	snprintf (tweak->ConfigName,CONFIGNAME_MAXSIZE, "%.4x%.4x_%02x:%02x:%02x_LATENCY",
		dev->vendor_id, dev->device_id,
		dev->bus, dev->dev, dev->func);

	/* Set the methods and register the tweak */
	tweak->WidgetText = strdup("Latency");
	tweak->MinValue = 0;
	tweak->MaxValue = 248;

	max_lat = pci_read_byte(dev,PCI_MAX_LAT);
	min_lat = pci_read_byte(dev,PCI_MIN_GNT);

	if (max_lat==0)
		tweak->Description = strdup ("Master PCI Latency Timer.\nThe device suggests this value doesn't matter.");
	else if (min_lat == max_lat)
		tweak->Description = strdup ("Master PCI Latency Timer.\nThe device min/max values suggest you shouldn't change this value.");
	else {
		tweak->Description = malloc (300);
		snprintf (tweak->Description,299,"Master PCI Latency Timer.\nThe device suggests this value to be between %i and %i.",min_lat,max_lat);
	}

	private = tweak->PrivateData;
	set_value_int (private->value, pci_read_byte (dev, 0xd));
	private->Mask = 0xFF;
	private->device = dev;
	private->base_address = 0xd;

	AddTo_PCI_tree (tweak, dev, FrameName, Tab, NULL);
	return TRUE;
}


int InitPlugin (int pt_unused showinfo)
{
	struct pci_dev *dev;
	int retval=FALSE;	/* Gets set if at least one tweak added. */

	if (have_core_plugin("libpcilib.so") == FALSE)
		return FALSE;

	dev = pacc->devices;

	while (dev) {
		if (add_latency_tweak(dev) == TRUE)
			retval = TRUE;
		dev = dev->next;
	}

	return retval;
}

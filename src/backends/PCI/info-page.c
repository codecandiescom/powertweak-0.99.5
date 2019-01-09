/*
 *  $Id: info-page.c,v 1.7 2001/11/07 13:02:26 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 * Show PCI specific info
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <powertweak.h>

#include <pci.h>
#include <pciutils.h>
#include <header.h>


static void do_command_bit(int bit, char *bitname, struct pci_dev *dev)
{
	struct tweak *tweak;
	struct private_PCI_data *private;
	char *Tab = "PCI Info";
	char *OptionFrame = "PCI Capabilities";

	if ((tweak = alloc_PCI_tweak(dev, TYPE_INFO_BOOL)) != NULL) {
		private = tweak->PrivateData;
		tweak->WidgetText = strdup(bitname);
		if ( (pci_read_word(dev,PCI_COMMAND)&(bit))!=0)
			set_value_int(private->value, 1);		
		else
			set_value_int(private->value, 0);		
		AddTo_PCI_tree (tweak, dev, OptionFrame, Tab, NULL);
	}
}


static void do_status_bit(int bit, char *bitname, struct pci_dev *dev)
{
	struct tweak *tweak;
	struct private_PCI_data *private;
	char *Tab = "PCI Info";
	char *OptionFrame = "PCI Capabilities";
	
	if ((tweak = alloc_PCI_tweak(dev, TYPE_INFO_BOOL)) != NULL) {
		private = tweak->PrivateData;
		tweak->WidgetText = strdup(bitname);
		if ( (pci_read_word(dev,PCI_STATUS)&(bit))!=0)
			set_value_int(private->value,1);		
		else
			set_value_int(private->value,0);		
		AddTo_PCI_tree (tweak, dev, OptionFrame, Tab, NULL);
	}
}


static void show_PCI_info (struct pci_dev *dev)
{
	struct tweak *tweak;
	struct private_PCI_data *private;
	word class;
	char *Tab = "PCI Info";
	char *VendorID = "Vendor ID:";
	char *DeviceID = "Device ID:";
	char *IDFrame = "ID:";

	/* Display device name as a header. */
	tweak = alloc_PCI_tweak (dev, TYPE_LABEL);
	if (tweak == NULL)
		return;
	tweak->WidgetText = find_devicename (dev);
	AddTo_PCI_tree (tweak, dev, NULL, Tab, NULL);

	/* Now the info. */

	if ((tweak = alloc_PCI_tweak(dev, TYPE_INFO_STRING)) != NULL) {
		private = tweak->PrivateData;
		tweak->WidgetText = strdup (VendorID);
		private->value.strVal = malloc (8);
		snprintf (private->value.strVal, 7, "0x%.4x", dev->vendor_id);
		AddTo_PCI_tree (tweak, dev, IDFrame, Tab, NULL);
	}

	if ((tweak = alloc_PCI_tweak(dev, TYPE_INFO_STRING)) != NULL) {
		private = tweak->PrivateData;
		tweak->WidgetText = strdup (DeviceID);
		private->value.strVal = malloc (8);
		snprintf (private->value.strVal, 7, "0x%.4x", dev->device_id);
		AddTo_PCI_tree (tweak, dev, IDFrame, Tab, NULL);
	}

	do_command_bit (PCI_COMMAND_MASTER, "Busmaster",dev);
	class = pci_read_word (dev, PCI_CLASS_DEVICE);
	if ((class & 0xff00)== 0x0300)
		do_command_bit (PCI_COMMAND_VGA_PALETTE, "VGA Palette snoop", dev);
	do_command_bit (PCI_COMMAND_FAST_BACK, "Fast back2back", dev);

	do_status_bit (PCI_STATUS_66MHZ, "66Mhz operation", dev);
}


int InitPlugin (int showinfo)
{
	struct pci_dev *dev;

	if (showinfo==FALSE)
		return FALSE;

	if (have_core_plugin("libpcilib.so") == FALSE)
		return FALSE;

	dev = pacc->devices;

	while (dev) { 
		show_PCI_info (dev);
		dev = dev->next;
	}
	return TRUE;
}

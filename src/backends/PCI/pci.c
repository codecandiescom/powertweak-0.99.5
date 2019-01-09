/*
 *  $Id: pci.c,v 1.19 2001/10/19 03:11:43 davej Exp $
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


void PCI_change_value (struct tweak *tweak, value_t value, int immediate)
{
	struct private_PCI_data *private;
	unsigned char current_value, dest_value='\0';

	private = tweak->PrivateData;
	if (private == NULL)
		return;

	set_value_int (private->value, get_value_int(value));

	if (immediate == 0)
		return;

	/* here we write the actual PCI register */
	current_value = pci_read_byte (private->device, private->base_address);

	switch (tweak->Type) {
	
		case TYPE_COMBOELEM:
			/* 0 values for combo-elements should _NOT_ be set */
			if (get_value_int(value) == 0)
				return;
			printf ("FIXME!: Unsupported PCI_change_value type\n");
			break;

		case TYPE_CHECKBOX:
			if (get_value_int (private->value) == 1) {
				dest_value = (current_value & ~private->Mask);
				dest_value |= private->OnMask;
			} else {
				dest_value = (current_value & ~private->Mask);
				dest_value |= private->OffMask;
			}
			break;

		case TYPE_SLIDER:
			dest_value = (current_value & ~private->Mask);
			dest_value |= get_value_int (private->value) & private->Mask;
			break;

		/* These types don't have changable properties. */
		case TYPE_LABEL:
		case TYPE_INFO_INT:
		case TYPE_INFO_STRING:
		case TYPE_INFO_BOOL:
			return;
			
		default:
			printf ("FIXME!: Unsupported PCI_change_value type %x\n", tweak->Type);
			break;
	}

	if (current_value != dest_value) {
/*		printf ("Device:%x:%x addr:%lx. current:%x. dest:%x finaldest:%x mask:%lx onmask:%lx offmask:%lx\n",
			private->device->vendor_id, private->device->device_id,
			private->base_address, current_value, get_value_int (private->value), dest_value,
			private->Mask, private->OnMask, private->OffMask);
*/
		pci_write_byte (private->device, private->base_address, dest_value);
	}
}


value_t PCI_get_value (struct tweak *tweak)
{
	struct private_PCI_data *private;
	
	value_t val = { 0 , NULL};

	private = tweak->PrivateData;
	assert (private != NULL);
	
	copy_values(&val, private->value);
	return val;
}


int PCI_get_value_raw (struct tweak *tweak)
{
	struct private_PCI_data *private;
	byte value;

	private = tweak->PrivateData;
	if (private == NULL)
		return FALSE;

	value = (pci_read_byte (private->device, private->base_address) & private->Mask);

	if (tweak->Type == TYPE_CHECKBOX) {
		if (value != private->OnMask)
			value = 0;
		else
			value = 1;
	}

	set_value_int(private->value, value);

	return TRUE;
}


void PCI_tweak_destructor (struct tweak *tweak)
{
	struct private_PCI_data *private;

	default_destructor(tweak);

	private = tweak->PrivateData;
	if (private != NULL) {
		free (private->classname);
		free (private->devicename);
		release_value(&private->value);
		free (private);
	}
}


struct tweak *alloc_PCI_tweak (struct pci_dev *dev, int type)
{
	struct private_PCI_data *private;
	struct tweak *tweak;

	/* First, allocate the structures for one item */
	tweak = alloc_tweak(type);

	private = malloc (sizeof (struct private_PCI_data));
	if (private == NULL) {
		printf ("Out of memory\n");
		if (tweak->Destroy!=NULL)
			tweak->Destroy(tweak);
		free (tweak);
		return (NULL);
	}

	memset (private, 0, sizeof(struct private_PCI_data));
	tweak->PrivateData = (void *) private;
	private->device = dev;
	private->Mask = 0xffffffff;	/* Check this for 64 bits ? */
	private->OnMask = 0x00000001;
	private->value.intVal = 0;
	private->value.strVal = NULL;

	/* Set the methods */
	tweak->Destroy = &PCI_tweak_destructor;
	tweak->ChangeValue = &PCI_change_value;
	tweak->GetValue = &PCI_get_value;
	tweak->GetValueRaw = &PCI_get_value_raw;
	tweak->IsValid = &generic_is_valid;
	return (tweak);
}


static char * find_classname (word class)
{
	char classbuf[64];
	char *CLASS_motherboard = "Motherboard";
	char *CLASS_video = "Video";
	char *CLASS_audio = "Audio";
	char *CLASS_disk = "Disk Controller";

	switch (class & 0xFF00) {

	case 0x0100:
		switch (class) {
			case 0x0101:		/* IDE interface */
			case 0x0104:		/* RAID bus controller */
			case 0x0180:		/* Unknown mass storage controller */
				return strdup (CLASS_disk);
				break;
			default:
				break;
		}
		break;

	case 0x0300:
		return strdup (CLASS_video);
		break;

	/* Multimedia device */
	case 0x0400:
		if (class == 0x0400)	/* Multimedia video */
			return strdup (CLASS_video);
		if (class == 0x0401)	/* Multimedia audio */
			return strdup (CLASS_audio);
		break;

	/* bridge. */
	case 0x0600:
		return strdup (CLASS_motherboard);
		break;

	/* Uncategorised class, use PCILIB to translate. */
	default:
		break;
	}
	snprintf (classbuf, sizeof(classbuf)-1, "%s",
	 (pci_lookup_name (pacc, classbuf, sizeof (classbuf), PCI_LOOKUP_CLASS, class, 0, 0, 0)));
	return (strdup (classbuf));
}


char *find_devicename(struct pci_dev *dev)
{
	struct pci_dev *dev2 = pacc->devices;
	char devicestring[128];
	char namebuf[128];
	int seen = 0;

	pci_lookup_name (pacc, namebuf, sizeof (namebuf),
		 PCI_LOOKUP_VENDOR | PCI_LOOKUP_DEVICE, dev->vendor_id, dev->device_id, 0, 0);

	/* Have we seen any of these devices on the bus before ? */
	while (dev != dev2) {
		if ((dev->vendor_id == dev2->vendor_id) && (dev->device_id == dev2->device_id))
			seen++;
		dev2 = dev2->next;
	}
	if (seen != 0)
		snprintf (devicestring,sizeof(devicestring)-1, "%s [%d]", namebuf, seen + 1);
	else {
		/* We haven't seen any of these yet, now we check
		 * if there are any _after_ this device, so we know whether
		 * to add a [1] or not. */
		dev2 = dev2->next;
		while (dev2) {
			if ((dev->vendor_id == dev2->vendor_id) && (dev->device_id == dev2->device_id)) {
				seen = 1;
				break;
			}
			dev2 = dev2->next;
		}
		if (seen == 1)
			snprintf (devicestring,sizeof(devicestring)-1, "%s [1]", namebuf);
		else
			snprintf (devicestring,sizeof(devicestring)-1, "%s", namebuf);
	}
	return strdup (devicestring);
}


void AddTo_PCI_tree (struct tweak *tweak, struct pci_dev *dev, char *Frame, char *Tab, char *Group)
{
	struct private_PCI_data *private;
	word class;
	char *Menu1 = "Hardware";
	char *Menu2 = "PCI";
	char *Menu3;
	char *devicename;

	private = tweak->PrivateData;

	private->devicename = find_devicename (dev);
	devicename = private->devicename;

	/* Find out the device class so we can cluster similar types. */
	class = pci_read_word (dev, PCI_CLASS_DEVICE);
	private->classname = find_classname (class);
	Menu3 = private->classname;

	switch (tweak->Type) {
	case TYPE_COMBOELEM:
		RegisterTweak (tweak, "mmmmtc", Menu1, Menu2, Menu3, devicename, Tab, Group);
		break;
	case TYPE_RADIO_OPTION:
		if (Frame != NULL)
			RegisterTweak (tweak, "mmmmtfr", Menu1, Menu2, Menu3, devicename, Tab, Frame, Group);
		else 
			RegisterTweak (tweak, "mmmmtr", Menu1, Menu2, Menu3, devicename, Tab, Group);
		break;
	default:
		if (Frame != NULL)
			RegisterTweak (tweak, "mmmmtf", Menu1, Menu2, Menu3, devicename, Tab, Frame);
		else 
			RegisterTweak (tweak, "mmmmt", Menu1, Menu2, Menu3, devicename, Tab);
		break;
	}
}

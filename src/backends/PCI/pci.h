/*
 *  $Id: pci.h,v 1.11 2001/10/19 03:11:43 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#ifndef _INCLUDE_GUARD_PCI_H_
#define _INCLUDE_GUARD_PCI_H_

#include <powertweak.h>
#include <pciutils.h>

extern struct pci_access *pacc;

/* struct private_PCI_data contains
 * everything needed to get/set PCI values from the GUI
 */
struct private_PCI_data {
	struct pci_dev	*device;
	unsigned long base_address;
	unsigned long Mask;
	unsigned long OnMask;
	unsigned long OffMask;
	char *classname;
	char *devicename;
	value_t value;
};

struct tweak *alloc_PCI_tweak (struct pci_dev *dev, int type);
void AddTo_PCI_tree (struct tweak *tweak, struct pci_dev *dev, char *Frame, char *Tab, char *Group);
char *find_devicename(struct pci_dev *dev);
#endif

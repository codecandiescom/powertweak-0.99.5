/*
 *  $Id: plugin.c,v 1.5 2003/04/21 11:37:06 svenud Exp $
 *  This file is part of Powertweak Linux.
 *  (C) 2001 Dave Jones, Arjan van de Ven.
 *
 *  Licensed under the terms of the GNU GPL License version 2.
 */

#include <powertweak.h>
#include <pci.h>
#include <pciutils.h>

struct pci_access *pacc;

int ClosePCILIBBackend (void)
{
/* FIXME: We don't need to pci_free_name_list(pacc);
   here, as pci_cleanup does that for us. But we should
   probably free the namelist somewhere else too to keep
   daemon mem usage down. Not sure where yet though.
 */
	if (pacc!=NULL)
	{
		pci_cleanup (pacc);
		pacc = NULL;
	}

	return TRUE;
}


/* Initialise PCI access routines. */
int InitPlugin (int pt_unused showinfo)
{
	if (!fileexists("/proc/bus/pci"))
		return FALSE;

	pacc = pci_alloc ();
	if (pacc == NULL)
		return FALSE;

	pacc->error = die;
	pci_init (pacc); 
	pci_scan_bus (pacc);

	RegisterShutdownCallback(&ClosePCILIBBackend);

	return TRUE;
}

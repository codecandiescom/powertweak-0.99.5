/*
 *	$Id: access.c,v 1.3 2003/04/07 11:54:33 svenud Exp $
 *
 *	The PCI Library -- User Access
 *
 *	Copyright (c) 1997--1999 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *
 *	Can be freely distributed and used under the terms of the GNU GPL.
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "internal.h"
#include <powertweak.h>

static struct pci_methods *pci_methods[] = {
	&pm_linux_proc,
};

struct pci_access *pci_alloc (void)
{
	struct pci_access *a = malloc (sizeof (struct pci_access));

	if (a!=NULL) {
		bzero (a, sizeof (*a));
		if (fileexists("/usr/share/misc/pci.ids"))
			a->id_file_name = strdup("/usr/share/misc/pci.ids");
		else
			a->id_file_name = strdup("/usr/share/pci.ids");
		pci_methods[0]->config (a);
	}
	return a;
}

void *pci_malloc (struct pci_access *a, int size)
{
	void *x = malloc (size);

	if (!x)
		a->error ("Out of memory (allocation of %d bytes failed)", size);
	return x;
}

void pci_mfree (void *x)
{
	if (x)
		free (x);
}

static void pci_generic_error (char *msg, ...)
{
	va_list args;

	va_start (args, msg);
	fputs ("pcilib: ", stderr);
	vfprintf (stderr, msg, args);
	fputc ('\n', stderr);
	exit (1);
}

static void pci_generic_warn (char *msg, ...)
{
	va_list args;

	va_start (args, msg);
	fputs ("pcilib: ", stderr);
	vfprintf (stderr, msg, args);
	fputc ('\n', stderr);
}

static void pci_generic_debug (char *msg, ...)
{
	va_list args;

	va_start (args, msg);
	vfprintf (stdout, msg, args);
	va_end (args);
}

static void pci_null_debug (char * msg UNUSED, ...)
{
}

void pci_init (struct pci_access *a)
{
	if (!a->error)
		a->error = pci_generic_error;
	if (!a->warning)
		a->warning = pci_generic_warn;
	if (!a->debug)
		a->debug = pci_generic_debug;
	if (!a->debugging)
		a->debug = pci_null_debug;

	a->methods = &pm_linux_proc;
	a->debug ("Decided to use %s\n", a->methods->name);
	a->methods->init (a);
}

void pci_cleanup (struct pci_access *a)
{
	struct pci_dev *d, *e;

	for (d = a->devices; d; d = e) {
		e = d->next;
		pci_free_dev (d);
	}
	if (a->methods)
		a->methods->cleanup (a);
	if (a->id_file_name )
		free(a->id_file_name);
	pci_free_name_list (a);
	pci_mfree (a);
}

void pci_scan_bus (struct pci_access *a)
{
	a->methods->scan (a);
}

struct pci_dev *pci_alloc_dev (struct pci_access *a)
{
	struct pci_dev *d = pci_malloc (a, sizeof (struct pci_dev));

	bzero (d, sizeof (*d));
	d->access = a;
	d->methods = a->methods;
	if (d->methods->init_dev)
		d->methods->init_dev (d);
	return d;
}

int pci_link_dev (struct pci_access *a, struct pci_dev *d)
{
	d->next = a->devices;
	a->devices = d;

	return 1;
}

struct pci_dev *pci_get_dev (struct pci_access *a, int bus, int dev, int func)
{
	struct pci_dev *d = pci_alloc_dev (a);

	d->bus = bus;
	d->dev = dev;
	d->func = func;
	return d;
}

void pci_free_dev (struct pci_dev *d)
{
	if (d->methods->cleanup_dev)
		d->methods->cleanup_dev (d);
	pci_mfree (d);
}

static inline void pci_read_data (struct pci_dev *d, void *buf, int pos, int len)
{
	if (pos & (len - 1))
		d->access->error ("Unaligned read: pos=%02x, len=%d", pos, len);
	if (!d->methods->read (d, pos, buf, len))
		memset (buf, 0xff, len);
}

byte pci_read_byte (struct pci_dev *d, int pos)
{
	byte buf;
	pci_read_data (d, &buf, pos, 1);
	return buf;
}

word pci_read_word (struct pci_dev * d, int pos)
{
	word buf;
	pci_read_data (d, &buf, pos, 2);
	return le16_to_cpu (buf);
}

u32 pci_read_long (struct pci_dev * d, int pos)
{
	u32 buf;
	pci_read_data (d, &buf, pos, 4);
	return le32_to_cpu (buf);
}

int pci_read_block (struct pci_dev *d, int pos, byte * buf, int len)
{
	return d->methods->read (d, pos, buf, len);
}

static inline int pci_write_data (struct pci_dev *d, void *buf, int pos, int len)
{
	if (pos & (len - 1))
		d->access->error ("Unaligned write: pos=%02x,len=%d", pos, len);
	return d->methods->write (d, pos, buf, len);
}

int pci_write_byte (struct pci_dev *d, int pos, byte data)
{
	return pci_write_data (d, &data, pos, 1);
}

int pci_write_word (struct pci_dev *d, int pos, word data)
{
	word buf = cpu_to_le16 (data);
	return pci_write_data (d, &buf, pos, 2);
}

int pci_write_long (struct pci_dev *d, int pos, u32 data)
{
	u32 buf = cpu_to_le32 (data);
	return pci_write_data (d, &buf, pos, 4);
}

int pci_write_block (struct pci_dev *d, int pos, byte * buf, int len)
{
	return d->methods->write (d, pos, buf, len);
}

int pci_fill_info (struct pci_dev *d, int flags)
{
	if (flags & PCI_FILL_RESCAN) {
		flags &= ~PCI_FILL_RESCAN;
		d->known_fields = 0;
	}
	if (flags & ~d->known_fields)
		d->known_fields |= d->methods->fill_info (d, flags & ~d->known_fields);
	return d->known_fields;
}

/*
 *  $Id: xml.c,v 1.18 2003/04/21 11:37:06 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 * XML routines used by PCI backend.
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <ptxml.h>
#include <powertweak.h>
#include <string.h>
#include <parser.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>

#include <powertweak.h>
#include <pci.h>
#include <pciutils.h>
#include <config.h>

#include <tree.h>

#include "pci.h"


static void do_one_item (xmlDocPtr doc, xmlNsPtr pt_unused ns, xmlNodePtr cur, struct pci_dev *dev)
{
	struct tweak *tweak = NULL;
	struct private_PCI_data *private;
	unsigned int i;
	char *Frame = NULL, *Group = NULL;
	char *Tab = "Tweaks";
	xmlChar *XMLProp;

	/* First, allocate the structures for one item */
	tweak = alloc_PCI_tweak (dev, TYPE_NODE_ONLY);
	if (tweak == NULL)
		return;
	private = tweak->PrivateData;


	/* Then iterate over all XML elements and fill the structure */
	assert (cur != NULL);

	XMLProp = xmlGetProp (cur, "base");
	private->base_address = strtoul (XMLProp, NULL, 0);
	free(XMLProp);

	XMLProp = xmlGetProp (cur, "bit");
	if (XMLProp != NULL) {
		unsigned long bitnr;
		bitnr = strtoul (XMLProp, NULL, 0);
		private->Mask = 1 << bitnr;
		private->OnMask = 1 << bitnr;
		private->OffMask = 0 ;
	}
	free(XMLProp);
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		assert (cur->name != NULL);

		match_record ("FRAME", Frame);
		match_record ("GROUP", Group);
		match_record("WIDGETTEXT", tweak->WidgetText);

		if (private->base_address > 255) {
			printf ("%s has a PCI configspace offset of %lx!",
				tweak->WidgetText, private->base_address);
			if (tweak->Destroy)
				tweak->Destroy(tweak);
			free (tweak);
			return;
		}
		
		match_record3("TYPE", tweak->Type, string_widget_to_int);
		if (tweak->Type == TYPE_CHECKBOX ||
			tweak->Type == TYPE_INFO_BOOL) {
			tweak->MinValue = 0;
			tweak->MaxValue = 1;
		} else {
			match_record3("LOW", tweak->MinValue, atoi);
			match_record3("HIGH", tweak->MaxValue, atoi);
		}
		match_record("DESCRIPTION", tweak->Description);
		match_record3("BITMASK", private->Mask, bitstring_to_long);	
		match_record3("ONBITS", private->OnMask, bitstring_to_long);	
		match_record3("OFFBITS", private->OffMask, bitstring_to_long);	
		
		if (!strcasecmp (cur->name, "CONFIGNAME")) {
			char *XMLstr;
	
			tweak->ConfigName = malloc (CONFIGNAME_MAXSIZE);
			if (tweak->ConfigName == NULL) {
				if (tweak->Destroy)
					tweak->Destroy(tweak);
				free (tweak);
				return;
			}
			XMLstr = (char *) xmlNodeListGetString (doc, cur->xmlChildrenNode, 1);
			snprintf (tweak->ConfigName,CONFIGNAME_MAXSIZE, "%.4x%.4x_%02x:%02x:%02x_%s",
					dev->vendor_id, dev->device_id,
					dev->bus, dev->dev, dev->func,
					XMLstr);
			if (XMLstr!=NULL)
				free(XMLstr);
		}	

		cur = cur->next;
	}

	tweak->GetValueRaw(tweak);

	if (tweak->Description == NULL) {
		if (tweak->Type != TYPE_LABEL)
			printf ("2: tweak %s doesn't have a description.\n", tweak->ConfigName);
	} else {  
		for (i=0 ; i< strlen(tweak->Description); i++) {
			if (tweak->Description[i]=='\n')
				tweak->Description[i]=' ';
		}   
	}

	/* FIXME: doesn't respect --no-info. */
	AddTo_PCI_tree (tweak, dev, Frame, Tab, Group);
	if (Frame != NULL)
		free(Frame);
}


static void parse_file (xmlDocPtr doc, xmlNsPtr ns, xmlNodePtr cur)
{
	struct pci_dev *dev;

	word xmlVendor;
	word xmlDevice;


	if (cur == NULL)
		return;

	if ((cur->name != NULL) && (strcasecmp (cur->name, "DEVICE") == 0)) {
		xmlChar *VendorStr,*IDStr;

		VendorStr = xmlGetProp (cur, "vendor");		
		IDStr = xmlGetProp (cur, "id");
		assert (VendorStr != NULL);
		assert (IDStr!= NULL);

		xmlVendor = strtoul (VendorStr, NULL, 0);
		xmlDevice = strtoul (IDStr, NULL, 0);
		
		free(VendorStr);
		free(IDStr);

		dev = pacc->devices;

		while (dev) {
			if ((dev->vendor_id == xmlVendor) && (dev->device_id == xmlDevice)) {

				cur = cur->xmlChildrenNode;
				while (cur != NULL) {
					if ((cur->name != NULL) && (strcasecmp (cur->name, "REGISTER") == 0))
						do_one_item (doc, ns, cur, dev);
					cur = cur->next;
				}
				return;
			} else {
				dev = dev->next;
			}
		}
	}
}

void load_PCI_xmlfile (char *Filename)
{
	xmlDocPtr doc;
	xmlNodePtr cur;

	if (Filename == NULL)
		return;

	doc = xmlParseFile (Filename);
	if (doc == NULL) {
		printf ("Severe XML error: doc == NULL!!\n");
		printf ("Probable cause: file %s not found.\n", Filename);
		return;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		printf ("Severe XML error: cur == NULL");
		xmlFreeDoc(doc);
		return;
	}

	parse_file (doc, NULL, cur);
	xmlFreeDoc(doc);
}


static void load_xml_recursive (char *path)
{
	DIR *dir;
	struct dirent *di;
	char *filebuf;		

	dir = opendir (path);
	if (dir == NULL) {
		printf ("xml datadir for PCI not found \n");
		return;
	}
	di = readdir (dir);
	while (di != NULL) {
		int stringlen;
		struct stat statbuf;
		
		stringlen=strlen(path)+strlen(di->d_name)+4;
			/* 4: 1 for the "/", 1 for the \0 and 2 to make sure */
			
		filebuf = calloc(stringlen,1); assert(filebuf!=NULL);

		strncpy (filebuf, path, stringlen);
		strncat (filebuf, "/",stringlen-1-strlen(filebuf));
		strncat (filebuf, di->d_name, stringlen - strlen (filebuf)-1);

		if (strstr (filebuf, ".xml") != NULL)
			load_PCI_xmlfile (filebuf);

		if ((strcmp (di->d_name, ".") != 0) && (strcmp (di->d_name, "..") != 0)) {
			stat (filebuf, &statbuf);
			if (S_ISDIR (statbuf.st_mode))
				load_xml_recursive (filebuf);
		}


		di = readdir (dir);
		free(filebuf);
	}

	closedir (dir);
}


int InitPlugin (int pt_unused showinfo)
{
	if (have_core_plugin("libpcilib.so") == FALSE)
		return FALSE;

	LIBXML_TEST_VERSION

	load_xml_recursive (PT_DATADIR "/pci");

	return TRUE;
}

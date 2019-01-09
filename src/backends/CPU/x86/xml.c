/*
 *  $Id: xml.c,v 1.39 2003/04/21 11:37:05 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 *	This file contains the XML handling code for the MSR tuning 
 *   backend
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <ptxml.h>
#include <powertweak.h>
#include <parser.h>		/* libxml */
#include <parserInternals.h>
#include <libxmlcompat.h>

#include <sys/types.h>		/* for readdir() */
#include <dirent.h>		/* for readdir() */

#include "x86.h"
#include "../cpu.h"


static void MSR_change_value(struct tweak *tweak, value_t value, int immediate)
{
	struct private_CPU_data *pvt;
	unsigned long long current;
	unsigned long long dest;

	if (tweak == NULL)
		return;
	pvt = tweak->PrivateData;

	if (pvt == NULL)
		return;

	assert (!(pvt->value.strVal));	/* DJ: Will this _ever_ trigger? */

	set_value_int (pvt->value, get_value_int (value));

	if (!immediate)
		return;

	if ((tweak->Type==TYPE_RADIO_OPTION)&&(get_value_int(value)==0))
		return;		

	/* read current MSR value. */
	if (rdmsr(pvt->CPU_number, pvt->MSR_Register, &current) != -1) {

		switch (tweak->Type) {
		case TYPE_CHECKBOX:
			if (get_value_int(pvt->value) == 1) {
				dest = (current & ~pvt->MSR_Mask);
				dest = (dest | pvt->MSR_OnMask);
			} else {
				dest = (current & ~pvt->MSR_Mask);
				dest = (dest | pvt->MSR_OffMask);
			}
			break;

		case TYPE_SPINBOX:
			dest = get_value_int(pvt->value);
			dest = (current & ~pvt->MSR_Mask) | dest;
			break;

		case TYPE_RADIO_OPTION:
			if (get_value_int(pvt->value) == 1) {
				dest = (current & ~pvt->MSR_Mask);
				dest = (dest | pvt->MSR_OnMask);
			}
			break;

		default:
			printf("Unknown widget type in MSR backend: %d\n", tweak->Type);
			break;
		}

		/* write the MSR back. */
		wrmsr(pvt->CPU_number, pvt->MSR_Register, &dest);
	}
}


static value_t MSR_get_value(struct tweak *tweak)
{
	struct private_CPU_data *pvt;

	assert(tweak != NULL);

	pvt = tweak->PrivateData;
	assert(pvt != NULL);

	return pvt->value;
}


static int MSR_get_value_raw(struct tweak *tweak)
{
	struct private_CPU_data *pvt;
	unsigned long long value;

	pvt = tweak->PrivateData;

	if (rdmsr(pvt->CPU_number, pvt->MSR_Register, &value)==-1)
		return FALSE;
	else {
		/* Fill tweak/privatedata with values from the MSR here. */
		if (tweak->Type == TYPE_CHECKBOX) {
			if ((value & pvt->MSR_OnMask) == pvt->MSR_OnMask) {
				value = 1;
			} else {
				value = 0;
			}
		}

		if (tweak->Type == TYPE_SPINBOX)
			value = value & pvt->MSR_Mask;

		set_value_int (pvt->value, value);

		if ((unsigned int) value > tweak->MaxValue) {
			printf ("CPU: Value for \"%s\" (%lld) exceeded max (%d), increasing.\n",
				tweak->WidgetText, value, tweak->MaxValue);
			tweak->MaxValue = value;
		}
	}
	return TRUE;
}


static void Parse_MSR_nodes(struct cpu_identity *id, xmlDocPtr doc, xmlNodePtr cur)
{
	struct private_CPU_data *pvt;
	unsigned int bitnr, i;
	char *Frame = NULL;
	char *Menu1 = "Hardware", *Menu2 = "CPU", *Tab = "Tweaks", *Group = NULL;
	char *registerstr=NULL, *bitstr=NULL;
	volatile char *localcpunames[8];
	struct tweak *tweak;

	if (strcasecmp((char*)cur->name, ("MSR"))!=0)
		return;

	tweak = alloc_CPU_tweak(id->CPU_number, TYPE_NODE_ONLY);

	tweak->ChangeValue = &MSR_change_value;
	tweak->GetValue = &MSR_get_value;
	tweak->GetValueRaw = &MSR_get_value_raw;
	tweak->IsValid = &generic_is_valid;
	pvt = tweak->PrivateData;

	registerstr = xmlGetProp(cur, "register");
	assert(registerstr != NULL);
	pvt->MSR_Register = strtoul(registerstr, NULL, 0);
	free(registerstr);

	bitstr = xmlGetProp(cur, "bit");
	if (bitstr != NULL) {
		bitnr = strtoul(bitstr, NULL, 0);
		pvt->MSR_Mask = 1 << bitnr;
		pvt->MSR_OnMask = 1 << bitnr;
		pvt->MSR_OffMask = 0;
		free(bitstr);
		/* If we had no bit= we will use the <BITMASK>
		 * stuff defined within the <MSR> below. */
	}

	cur = cur->xmlChildrenNode;


	match_record("FRAME", Frame);
	match_record("GROUP", Group);
	match_record("WIDGETTEXT", tweak->WidgetText);
	match_record("DESCRIPTION", tweak->Description);
	match_record3("TYPE", tweak->Type, string_widget_to_int);

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
		snprintf (tweak->ConfigName, CONFIGNAME_MAXSIZE,
			"CPU%d_%s", id->CPU_number, XMLstr);
		if (XMLstr!=NULL)
			free (XMLstr);
	}

	if (tweak->Type == TYPE_CHECKBOX || tweak->Type == TYPE_INFO_BOOL) {
		tweak->MinValue = 0;
		tweak->MaxValue = 1;
	} else {
		match_record3("LOW", tweak->MinValue, atoi);
   		match_record3("HIGH", tweak->MaxValue, atoi);
	}

	match_record3("BITMASK", pvt->MSR_Mask, bitstring_to_long);
	match_record3("ONBITS", pvt->MSR_OnMask, bitstring_to_long);
	match_record3("OFFBITS", pvt->MSR_OffMask, bitstring_to_long);

	if (MSR_get_value_raw(tweak) != TRUE) {
		CPU_tweak_destructor(tweak);
		free(tweak);
	}

	if (tweak->Description == NULL)
		printf ("1: tweak %s doesn't have a description.\n", tweak->ConfigName);
	else {  
		for (i=0 ; i< strlen(tweak->Description); i++) {
			if (tweak->Description[i]=='\n')
				tweak->Description[i]=' ';
		}   
	}

	if (localcpunames[id->CPU_number] == NULL)
		localcpunames[id->CPU_number]  = strdup(id->CPUName);

	pvt->cpuname = (char **) &localcpunames[id->CPU_number];

	switch (tweak->Type) {
		case TYPE_COMBOELEM:
			RegisterTweak(tweak, "mmmtc", Menu1, Menu2, *(pvt->cpuname), Tab, Group);
			printf("COMBO <this is not supposed to work>!\n");
			break;

		case TYPE_RADIO_OPTION:
			if (Frame != NULL) 
				RegisterTweak(tweak, "mmmtfr", Menu1, Menu2, *(pvt->cpuname), Tab, Frame,Group);
			else
				RegisterTweak(tweak, "mmmtr", Menu1, Menu2, *(pvt->cpuname), Tab, Group);
			break;

		default:
			if (Frame != NULL)
				RegisterTweak(tweak, "mmmtf", Menu1, Menu2, *(pvt->cpuname), Tab, Frame);
			else
				RegisterTweak(tweak, "mmmt", Menu1, Menu2, *(pvt->cpuname), Tab);
			break;
	}

	if (Group != NULL)
		free(Group);
	if (Frame != NULL)
		free(Frame);
}


/*
load_MSR_xmlfile() takes care of loading and registering all tweaks
that are present in the file denoted by "Filename".
*/

static void load_MSR_xmlfile (struct cpu_identity *id, char *Filename)
{
	xmlDocPtr doc;
	xmlNodePtr cur;

	if (Filename == NULL)
		return;

	doc = xmlParseFile(Filename);
	if (doc == NULL) {
		printf("Severe XML error: doc == NULL!!\n");
		printf("Probable cause: file %s not found.\n", Filename);
		return;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		printf("Severe XML error: cur == NULL");
		goto fail;
	}

	if (cur->name == NULL)
		goto fail;

	if (strcasecmp((char *) cur->name, "MSRENTRY") != 0)
		goto fail;

	cur = cur->xmlChildrenNode;

	Make_CPU_menuname(id);

	while (cur != NULL) {
		Parse_MSR_nodes(id, doc, cur);
		cur = cur->next;
	}

fail:
	xmlFreeDoc(doc);
}


static void ParseIndex (struct cpu_identity *id, xmlDocPtr doc, xmlNodePtr cur)
{
	int family=0, model=0, stepping=0;
	char *vendor, *filename;
	xmlNodePtr node;

	if (cur->name == NULL)
		return;
	if (strcasecmp((char *) cur->name, "CPUINDEX") != 0)
		return;

	cur = cur->xmlChildrenNode;

	while (cur != NULL) {
		if (cur->name == NULL)
			return;
		if (strcasecmp((char *) cur->name, "CPU") != 0)
			return;

		vendor = xmlGetProp (cur, "vendor");
		if (strncasecmp (vendor, IDtoVendor(id->vendor), 12) != 0)
			goto badvendor;

		filename = xmlGetProp(cur, "file");

		node = cur->xmlChildrenNode;
		while (node != NULL) {
			match_record3("family", family, atoi);
			match_record3("model", model, atoi);
			match_record3("stepping", stepping, atoi);

			if (id->family != family)
				goto NextCPUEntry;
			if (id->model != model)
				goto NextCPUEntry;
			if (id->stepping != stepping)
				goto NextCPUEntry;

			load_MSR_xmlfile (id, filename);
			return;
NextCPUEntry:
			node = node->next;
		}

		free (filename);

badvendor:
		free (vendor);
		cur = cur->next;
	}
}


void Load_CPU_XML(struct cpu_identity *id)
{
	xmlDocPtr doc;
	xmlNodePtr cur;
	char *indexfname= PT_DATADIR "/CPU/index.xml";

	LIBXML_TEST_VERSION

	xmlDoValidityCheckingDefaultValue = 1;

	doc = xmlParseFile(indexfname);
	if (doc == NULL) {
		printf("Severe XML error: Couldn't find CPU Index!!\n");
		printf("Probable cause: file %s not found.\n", indexfname);
		return;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		printf("Severe XML error: cur == NULL");
		xmlFreeDoc(doc);
		return;
	}

	ParseIndex(id, doc, cur);

	xmlFreeDoc(doc);
}

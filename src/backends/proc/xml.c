/*
 *  $Id: xml.c,v 1.21 2003/07/08 11:42:12 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 *	This file contains the XML handling code for the /proc tuning 
 *      backend
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <sys/types.h>		/* for readdir() */
#include <sys/stat.h>		/* for stat() */
#include <dirent.h>		/* for readdir() */

#include <ptxml.h>
#include <powertweak.h>
#include <parser.h>		/* libxml */
#include <libxmlcompat.h>

#include "proc.h"

extern char *strndup (char *S1, int count);

/* Forward function prototypes */
static void parse_file(xmlDocPtr doc, xmlNodePtr cur);
static void do_one_xml_record(xmlDocPtr doc,  xmlNodePtr cur);
static void expand_wildcard_tweak (struct tweak *tweak, char *Menu1, char *Menu2, char *Tab, char *Frame);




/*
load_proc_xmlfile() takes care of loading and registering all tweaks
that are present in the file denoted by "Filename".
*/

void load_proc_xmlfile(char *Filename)
{
	xmlDocPtr doc;
	xmlNodePtr cur;

	if (Filename == NULL) {
		printf ("Odd, load_proc_xmlfile got NULL\n");
		return;
	}

	doc = xmlParseFile(Filename);
	if (doc == NULL) {
		printf("Severe XML error: doc == NULL!!\n");
		printf("Probable cause: file %s not found.\n", Filename);
		return;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		printf("Severe XML error: cur == NULL");
		xmlFreeDoc(doc);
		return;
	}

	parse_file(doc, cur);

	cur = NULL;
	xmlFreeDoc(doc);
}


/* 
parse_file() enumerates over the entries found in a xml file, and calls
the do_one_tweak() handling function on them. Only <PROCENTRY> records
are actually processed.
*/

static void parse_file(xmlDocPtr doc, xmlNodePtr cur)
{
	assert(cur != NULL);	/* isn't supposed to be possible */

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if ((cur->name != NULL) && (strcasecmp((char *) cur->name, "PROCENTRY") == 0))
			do_one_xml_record(doc, cur);

		cur = cur->next;
	}

}



static void do_one_xml_record(xmlDocPtr doc, xmlNodePtr cur)
{
	struct tweak *tweak = NULL;
	struct private_proc_data *private = NULL;
	unsigned int i;
	char *Frame = NULL;
	char *Menu1 = NULL, *Menu2 = NULL, *Menu3 = NULL;
	char *Tab = NULL, *Group = NULL;

	/* First, allocate the structures for one item */
	tweak = alloc_tweak(TYPE_NODE_ONLY);

	private = malloc(sizeof(struct private_proc_data));
	if (private == NULL) {
		printf("malloc failed: Probably out of memory\n");
		free(tweak);
		return;
	}

	initialize_private_proc_data(private);
	tweak->PrivateData = (void *) private;

	/* Set the methods  */
	tweak->Destroy = &proc_tweak_destructor;
	tweak->ChangeValue = &proc_change_value;
	tweak->GetValue = &proc_get_value;
	tweak->GetValueRaw = &proc_get_value_raw;
	tweak->IsValid = &generic_is_valid;

	/* Then iterate over all XML elements and fill the structure */

	assert(cur != NULL); /* cannot happen.. in theory */
	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		assert(cur->name != NULL);
		
		match_record("MENU",Menu1);
		match_record("SUBMENU",Menu2);
		match_record("SUBSUBMENU",Menu3);
		match_record("TAB",Tab);
		match_record("FRAME",Frame);
		match_record("GROUP",Group);
		match_record("WIDGETTEXT",tweak->WidgetText);
		match_record("CONFIGNAME",tweak->ConfigName);
		match_record("FILE",private->Filename);
		match_record("DESCRIPTION",tweak->Description);		
		match_record3("TYPE",tweak->Type,string_widget_to_int);
		match_record3("ELEMENT",private->Field,atoi);
		if (tweak->Type == TYPE_CHECKBOX ||
			tweak->Type == TYPE_INFO_BOOL) {
			tweak->MinValue = 0;
			tweak->MaxValue = 1;
		} else {
			match_record3("LOW",tweak->MinValue,atoi);
			match_record3("HIGH",tweak->MaxValue,atoi);
		}
		match_record3("BITMASK",private->Mask,bitstring_to_long);
		match_record3("ONBITS",private->OnMask,bitstring_to_long);
		match_record3("OFFBITS",private->OffMask,bitstring_to_long);
		
		cur = cur->next;

	}

	/* If it is a wildcard /proc thingy, first explode it into several
	   tweaks and handle those separately... */

	if (strstr(private->Filename, "/*") != NULL) {

		expand_wildcard_tweak(tweak, Menu1, Menu2, Tab, Frame);

		/* the tweak is already freed from memory here, just
		   free the leftovers and return now, all work is done. */
		goto failed;
	}

	/* Now, read the last few bits of information from /proc and
	   register it to the system */

	if (fill_struct_tweak_from_proc(tweak)==0) 
	{
		/* It failed.... probably doesn't exist or is otherwise
		   unsuitable. We should just free it and not register */
		if (tweak->Destroy!=NULL)
			tweak->Destroy(tweak);
		free(tweak);
		goto failed;
	}

	if (tweak->Description == NULL)
		printf ("3: tweak %s doesn't have a description.\n", tweak->ConfigName);
	else {
		for (i=0 ; i< strlen(tweak->Description); i++) {
			if (tweak->Description[i]=='\n')
				tweak->Description[i]=' ';
		}
	}


	/* This is actually a kludge, but for now it has to do */
	switch (tweak->Type) {
		case TYPE_COMBOELEM:
		RegisterTweak(tweak, "mmtc", Menu1, Menu2, Tab, Group);
		printf("COMBO <this is not supposed to work>!\n");
		break;

		case TYPE_RADIO_OPTION:
		if (Menu3 == NULL) {
			if (Frame != NULL) 
				RegisterTweak(tweak, "mmtfr", Menu1, Menu2, Tab, Frame,Group);
			else
				RegisterTweak(tweak, "mmtr", Menu1, Menu2, Tab, Group);
			break;
		} else {
			if (Frame != NULL) 
				RegisterTweak(tweak, "mmmtfr", Menu1, Menu2, Menu3, Tab, Frame,Group);
			else
				RegisterTweak(tweak, "mmmtr", Menu1, Menu2, Menu3, Tab, Group);
			break;
		}

		default:
		if (Menu3 == NULL) {
			if (Frame != NULL)
				RegisterTweak(tweak, "mmtf", Menu1, Menu2, Tab, Frame);
			else
				RegisterTweak(tweak, "mmt", Menu1, Menu2, Tab);
		} else {
			if (Frame != NULL)
				RegisterTweak(tweak, "mmmtf", Menu1, Menu2, Menu3, Tab, Frame);
			else
				RegisterTweak(tweak, "mmmt", Menu1, Menu2, Menu3, Tab);
		}
		break;		
	}

failed:
	if (Menu1!=NULL)
		free(Menu1);
	if (Menu2!=NULL)
		free(Menu2);
	if (Menu3!=NULL)
		free(Menu3);
	if (Tab!=NULL)
		free(Tab);
	if (Group!=NULL)
		free(Group);
	if (Frame!=NULL)
		free(Frame);
}

/*
Ok. I confess. I did write this piece of shit. But I'll deny any knowledge
of it if you ever tell anyone. --AV
Oh, you think this is bad? And the crap above is better ?
I won't tell if you won't. --DJ
*/

/*
Handle wildcarded /proc things by expanding the wildcard.
*/
static void expand_wildcard_tweak (struct tweak *tweak, char *Menu1, char *Menu2, char *Tab, char *Frame)
{
	struct tweak *expanded;
	struct private_proc_data *private;
	char *before, *after;
	char *NewTab, *NewFrame;
	DIR *dir;
	struct dirent *di;

	if (tweak == NULL)
		return;
		
	private = tweak->PrivateData;
	if (private == NULL)
		return;

	NewTab = Tab;
	NewFrame = Frame;

	if (strstr (private->Filename, "/*") == NULL) {
		/* No more wildcard expansion needed */
		if (fill_struct_tweak_from_proc (tweak)==0) {
			if (tweak->Destroy!=NULL)
				tweak->Destroy(tweak);
			free (tweak);
			return;
		}

		if (Frame != NULL)
			RegisterTweak (tweak, "mmtf", Menu1, Menu2, Tab, Frame);
		else
			RegisterTweak (tweak, "mmt", Menu1, Menu2, Tab);
		return;
	}

	before = strndup (private->Filename, strstr (private->Filename, "/*") - private->Filename);
	after = strdup (strstr (private->Filename, "/*") + 2);

	dir = opendir (before);
	if (dir == NULL)
		return;

	di = readdir (dir);
	while (di != NULL) {
		struct stat statbuf;
		char *filebuf;
		int retval,stringlen;
		
		stringlen = strlen(before)+strlen(di->d_name)+4;
		
		filebuf = calloc(stringlen,1);assert(filebuf!=NULL);

		strncpy (filebuf, before, stringlen);
		strncat (filebuf, "/",stringlen-1-strlen(filebuf));
		strncat (filebuf, di->d_name, stringlen - strlen (filebuf)-1);

		retval = stat (filebuf, &statbuf);

		if ((di->d_name[0] != '.')&&(retval==0)) {
			struct private_proc_data *newprivate;
			/* Now we clone the tweak and fill in the resolved wildcards */
			expanded = clone_proc_tweak(tweak);
			newprivate = (struct private_proc_data*)expanded->PrivateData;
			
			free(newprivate->Filename);
			newprivate->Filename = strnewcat(filebuf,after);
			
			if (expanded->ConfigName!=NULL)
				free(expanded->ConfigName);
			expanded->ConfigName = strnewcat(tweak->ConfigName,di->d_name);
			
			if ((Tab == NULL) || (strcmp (Tab, "*") == 0))
				NewTab = di->d_name;
			else if ((Frame == NULL) || (strcmp (Frame, "*") == 0))
				NewFrame = di->d_name;

			expand_wildcard_tweak (expanded, Menu1, Menu2, NewTab, NewFrame);
		}
		free (filebuf);
		di = readdir (dir);
	}

	(void)closedir (dir);
	free(before);
	free(after);
	if (tweak->Destroy!=NULL)
		tweak->Destroy(tweak);
	free (tweak);
	return;
}

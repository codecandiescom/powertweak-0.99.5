/*
 *  $Id: proctweaks.c,v 1.3 2001/09/16 02:21:07 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
/* for readdir/stat and friends */
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <powertweak.h>
#include <config.h>
#include <string.h>
#include "proc.h"
#include <tree.h>
#include <libxmlcompat.h>


extern int xmlDoValidityCheckingDefaultValue;


static void load_xml_recursive (char *path)
{
	DIR *dir;
	struct dirent *di;
	char *filebuf;

	dir = opendir (path);
	if (dir == NULL) {
		printf ("xml datadir for /proc not found \n");
		return;
	}
	di = readdir (dir);
	while (di != NULL) {
		int stringlen;
		struct stat statbuf;
		
		stringlen =strlen(path)+strlen(di->d_name)+4;
		filebuf = calloc(stringlen,1); assert(filebuf!=NULL);

		strncpy (filebuf, path, stringlen);
		strncat (filebuf, di->d_name, stringlen - strlen (filebuf)-1);

		if (strstr (filebuf, ".xml") != NULL)
			load_proc_xmlfile (filebuf);

		if ((strcmp (di->d_name, ".") != 0) && (strcmp (di->d_name, "..") != 0)) {
			int retval;
			retval = stat (filebuf, &statbuf);
			if ((S_ISDIR (statbuf.st_mode))&&(retval==0))
				load_xml_recursive (filebuf);
		}


		free(filebuf);
		di = readdir (dir);
	}

	(void)closedir (dir);

}



int InitPlugin (pt_unused int showinfo)
{
	LIBXML_TEST_VERSION

	xmlDoValidityCheckingDefaultValue = 1;

	load_xml_recursive (PT_DATADIR "/proc/");
	
	return 1;
}

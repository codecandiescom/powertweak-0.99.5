/*
 *  $Id: logfile.c,v 1.4 2003/04/18 11:59:27 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2001 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include <powertweak.h>

/*
 * This formatted printer is far from perfect.
 * Feed it properly, or it will explode.
 * 
 * - Only use recognised formats (current strings only)
 * - No % at EOL.
 * - no \ at EOL.
 */
/*SVEN - change this to use syslog....*/
void log_message(char *fmt, ...)
{
	FILE *logfile=NULL;
	va_list ap;
	char *str;

	logfile = fopen ("/var/log/powertweak.log", "a");

	va_start (ap, fmt);

	while (*fmt) {

		if (*fmt == '%') {
			fmt++;
			if (*fmt == 's') {
				str = va_arg (ap, char *);
				fwrite (str, strlen(str), 1, logfile);
			} else {
				fwrite ("%", 1, 1, logfile);
			}
			/* FIXME: Add other formats. */
			continue;
		}
		if ((*fmt == '\\') && (*(fmt+1)=='n'))
			fwrite ("\n", 1, 1, logfile);

		fwrite (fmt, 1, 1, logfile);

		fmt++;
	}

	va_end (ap);

	fclose (logfile);
}

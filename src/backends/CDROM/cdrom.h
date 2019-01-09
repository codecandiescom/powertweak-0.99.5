/*
 *  $Id: cdrom.h,v 1.2 2001/08/25 19:24:44 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */


/*
 * struct private_CDROM_data contains everything
 * needed to get/set CDROM tweaks from the GUI
 */

struct private_CDROM_data {
	char *DeviceName;
	value_t value;	/* Note, could be a uint right now,
					   but at some point, we may want to
					   change some of the text entries
					   of /proc/ide/ * /settings too. */
};

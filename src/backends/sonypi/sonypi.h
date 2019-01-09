/*
 *  $Id: sonypi.h,v 1.1 2001/06/26 12:12:28 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */


/*
 * struct private_sonypi_data contains everything
 * needed to get/set sonypi tweaks from the GUI
 */

struct private_sonypi_data {
	value_t value;
};

/* this has to match with the kernel structure */

#define SONYPI_IOCGBRT	_IOR('v',0,__u8)
#define SONYPI_IOCSBRT	_IOW('v',0,__u8)

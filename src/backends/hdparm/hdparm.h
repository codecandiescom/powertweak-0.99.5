/*
 *  $Id: hdparm.h,v 1.6 2003/04/18 11:59:26 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <powertweak.h>

#define HD_GETID		1
#define HD_GETFIRMWARE	2
#define HD_GETSERIAL	3
#define HD_GETGEOMETRY	4
#define HD_GETCACHE		5

#define HD_GETDMA		6
#define HD_GET32BIT		7
#define HD_GETMULT		8
#define HD_GETUNMASK	9

#define HD_GET_ACOUSTIC  10
#define HD_GET_APM  11

struct private_hdparm_data{
	char *DeviceName;
	int Type;
	value_t value;
};


struct tweak *alloc_hdparm_tweak (char *devname, int type);
void hdparm_tweak_destructor (struct tweak *tweak);

void Add_Info_Page (char *MenuName, char *DeviceName, int fd);
void Add_Tweak_Page (char *MenuName, char *DeviceName, int fd);
void AddTo_hdparm_tree (char *MenuName, char *DeviceName, int fd);

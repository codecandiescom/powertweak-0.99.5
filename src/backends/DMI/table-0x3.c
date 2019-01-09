#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <powertweak.h>
#include "dmi.h"

int chassis_number = 0; /*counter just in case there are more that one chassis entries
						eg notebook with docking station */

void handle_table_0x3(u8 *data, struct dmi_header *dm)
{
	char tmpbuf[40];
	char information[]="Information";
/*	char systemtxt[]="System";*/
	char chassis[15]="Chassis";
	struct tweak *tweak=NULL;
	struct private_DMI_data *private;
	int ignore = FALSE;
	
	if (chassis_number > 0)
	{
		snprintf(chassis, 15, "Chassis[%i]", chassis_number);
	}
	
	chassis_number += 1;

	/*data[0x02-0x03] handle*/

	DMI_STR("Vendor", data[4], chassis, information);
	DMI_STR("Version", data[6], chassis, information);
	DMI_STR("Serial number", data[7], chassis, information);
	DMI_STR("Asset tag", data[8], chassis, information);

	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Enclosure Type");
	private = (struct private_DMI_data*) tweak->PrivateData;
	switch (data[0x5] & 0x7f) {
		case 1:		private->value.strVal = strdup("Other"); ignore=TRUE;	break;
		case 2:		private->value.strVal = strdup("Unknown"); ignore=TRUE;	break;
		case 3:		private->value.strVal = strdup("Desktop");	break;
		case 4:		private->value.strVal = strdup("Low profile desktop");	break;
		case 5:		private->value.strVal = strdup("Pizza box");	break;
		case 6:		private->value.strVal = strdup("Mini tower");	break;
		case 7:		private->value.strVal = strdup("Tower");	break;
		case 8:		private->value.strVal = strdup("Portable");	break;
		case 9:		private->value.strVal = strdup("Laptop");	break;
		case 0xa:		private->value.strVal = strdup("Notebook");	break;
		case 0xb:		private->value.strVal = strdup("Hand held");	break;
		case 0xc:		private->value.strVal = strdup("Docking station");	break;
		case 0xd:		private->value.strVal = strdup("All in one");	break;
		case 0xe:		private->value.strVal = strdup("Sub notebook");	break;
		case 0xf:		private->value.strVal = strdup("Space saving");	break;
		case 0x10:		private->value.strVal = strdup("Lunch box");	break;
		case 0x11:		private->value.strVal = strdup("Main server chassis");	break;
		case 0x12:		private->value.strVal = strdup("Expansion chassis");	break;
		case 0x13:		private->value.strVal = strdup("Subchassis");	break;
		case 0x14:		private->value.strVal = strdup("Bus expansion chassis");	break;
		case 0x15:		private->value.strVal = strdup("Peripheral chassis");	break;
		case 0x16:		private->value.strVal = strdup("RAID chassis");	break;
		case 0x17:		private->value.strVal = strdup("Rack mount chassis");	break;
		case 0x18:		private->value.strVal = strdup("Sealed case PC");	break;
		case 0x19:		private->value.strVal = strdup("Multi-system chassis");	break;
		default:	private->value.strVal = strdup("Reserved"); ignore=TRUE;	break;
	}
/*	if (!ignore)*/
	{
		RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, chassis, information);
	}

	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Enclosure bootup state");
	private = (struct private_DMI_data*) tweak->PrivateData;
	ignore = FALSE;
	switch (data[0x9]) {
		case 1:		private->value.strVal = strdup("Other"); ignore=TRUE;	break;
		case 2:		private->value.strVal = strdup("Unknown"); ignore=TRUE;	break;
		case 3:		private->value.strVal = strdup("Safe");	break;
		case 4:		private->value.strVal = strdup("Warning");	break;
		case 5:		private->value.strVal = strdup("Critical");	break;
		case 6:		private->value.strVal = strdup("Non-recoverable");	break;
		default:	private->value.strVal = strdup("Reserved"); ignore=TRUE;	break;
	}
/*	if (!ignore)*/
	{
		RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, chassis, information);
	}

	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("PSU bootup state");
	private = (struct private_DMI_data*) tweak->PrivateData;
	ignore = FALSE;
	switch (data[0xa]) {
		case 1:		private->value.strVal = strdup("Other"); ignore=TRUE;	break;
		case 2:		private->value.strVal = strdup("Unknown"); ignore=TRUE;	break;
		case 3:		private->value.strVal = strdup("Safe");	break;
		case 4:		private->value.strVal = strdup("Warning");	break;
		case 5:		private->value.strVal = strdup("Critical");	break;
		case 6:		private->value.strVal = strdup("Non-recoverable");	break;
		default:	private->value.strVal = strdup("Reserved"); ignore=TRUE;	break;
	}
/*	if (!ignore)*/
	{
		RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, chassis, information);
	}

	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Thermal bootup state");
	private = (struct private_DMI_data*) tweak->PrivateData;
	ignore = FALSE;
	switch (data[0xb]) {
		case 1:		private->value.strVal = strdup("Other"); ignore=TRUE;	break;
		case 2:		private->value.strVal = strdup("Unknown"); ignore=TRUE;	break;
		case 3:		private->value.strVal = strdup("Safe");	break;
		case 4:		private->value.strVal = strdup("Warning");	break;
		case 5:		private->value.strVal = strdup("Critical");	break;
		case 6:		private->value.strVal = strdup("Non-recoverable");	break;
		default:	private->value.strVal = strdup("Reserved"); ignore=TRUE;	break;
	}
/*	if (!ignore)*/
	{
		RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, chassis, information);
	}

	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Security state");
	private = (struct private_DMI_data*) tweak->PrivateData;
	ignore = FALSE;
	switch (data[0xc]) {
		case 1:		private->value.strVal = strdup("Other"); ignore=TRUE;	break;
		case 2:		private->value.strVal = strdup("Unknown"); ignore=TRUE;	break;
		case 3:		private->value.strVal = strdup("None");	break;
		case 4:		private->value.strVal = strdup("External interface locked out");	break;
		case 5:		private->value.strVal = strdup("External interface enabled");	break;
		default:	private->value.strVal = strdup("Reserved"); ignore=TRUE;	break;
	}
/*	if (!ignore)*/
	{
		RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, chassis, information);
	}
	/*data[0x0d-0x10] OEM defined DWORD*/
	/*data[0x11] height *//*SVEN - this is wrong on my notebook*/
	snprintf(tmpbuf, 40, "%i", data[0x11]);
	DMI_STR_VALUE("height", tmpbuf, chassis, information);
	/*data[0x12] number of power cords*//*SVEN - this is wrong on my notebook*/
	snprintf(tmpbuf, 40, "%i", data[0x12]);
	DMI_STR_VALUE("Number of Power cords", tmpbuf, chassis, information);
	/*data[0x13] contained element count*/
	/*data[0x14] contained element length*/
	/*data[0x15] contained elements - */
}

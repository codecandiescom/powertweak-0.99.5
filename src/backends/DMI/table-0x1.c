#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <powertweak.h>
#include "dmi.h"

/*SVEN - can we move this to BIOS too?*/

void handle_table_0x1(u8 *data, struct dmi_header *dm)
{
	char information[]="Information";
	char systemtxt[]="System";
	struct tweak *tweak=NULL;
	struct private_DMI_data *private;
	int ignore = FALSE;
	char uuid[20];
		

	DMI_STR("Manufacturer", data[4], systemtxt, information);
	DMI_STR("Product Name", data[5], systemtxt, information);
	DMI_STR("Version", data[6], systemtxt, information);
	DMI_STR("Serial number", data[7], systemtxt, information);
	
	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup("UUID");
	snprintf(uuid, 19, "0x%02X0%02X0%02X0%02X0%02X0%02X0%02X0%02X0%02X0%02X0", data[8], data[9], data[10], data[11], 
					data[12], data[13], data[14], data[15], data[16], data[17]);
	
	private = (struct private_DMI_data*) tweak->PrivateData;
	private->value.strVal = strdup(uuid);
	RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, systemtxt, information);
	
				
	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Wake-up Type");
	private = (struct private_DMI_data*) tweak->PrivateData;
	switch (data[0x18]) {
		case 1:		private->value.strVal = strdup("Other"); ignore=TRUE;	break;
		case 2:		private->value.strVal = strdup("Unknown"); ignore=TRUE;	break;
		case 3:		private->value.strVal = strdup("APM Timer");	break;
		case 4:		private->value.strVal = strdup("Modem ring");	break;
		case 5:		private->value.strVal = strdup("LAN remote");	break;
		case 6:		private->value.strVal = strdup("Power switch");	break;
		case 7:		private->value.strVal = strdup("PCI PME#");	break;
		case 8:		private->value.strVal = strdup("AC power restored");	break;
		default:	private->value.strVal = strdup("Reserved"); ignore=TRUE;	break;
	}
/*	if (!ignore)*/
	{
		RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, systemtxt, information);
	}
}

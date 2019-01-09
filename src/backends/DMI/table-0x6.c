#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <powertweak.h>
#include "dmi.h"

int socket_number = 0;

void handle_table_0x6(u8 *data, struct dmi_header *dm)
{
	char sockettxt[10];
	char information[]="Information";
	char tmpstr[80];
	char *bufptr = tmpstr;
	struct tweak *tweak=NULL;
	struct private_DMI_data *private;
		
	snprintf(sockettxt, 10, "Socket%i", socket_number);
	socket_number++;

	DMI_STR("Socket:", data[0x4], sockettxt, information);
	
	if (data[0x5]!=0xFF) {
		bufptr = tmpstr;
		if ((data[0x5]&0xF0)!=0xF0)
			bufptr += sprintf (bufptr, "%d ", data[5]>>4);
		if ((data[0x5]&0x0F)!=0x0F)
			bufptr += sprintf (bufptr, "%d ", data[5]&0x0F);
		DMI_STR_VALUE("Bank Connections", tmpstr, sockettxt, information);
	}

	if (data[0x6] != 0)
	{
		sprintf (tmpstr, "\tSpeed: %dnS", data[0x6]);
		DMI_STR_VALUE("Socket:", tmpstr, sockettxt, information);
	}
	
	memset(tmpstr, 0, 80);
	dmi_decode_ram(data[0x7]|data[0x8]<<8, tmpstr);
	DMI_STR_VALUE("memory type", tmpstr, sockettxt, information);

	memset(tmpstr, 0, 80);
	bufptr = tmpstr;
	switch(data[0x9]&0x7F) {
		case 0x7D:
			bufptr += sprintf (bufptr, "Unknown");
			break;
		case 0x7E:
			bufptr += sprintf (bufptr, "Disabled");
			break;
		case 0x7F:
			bufptr += sprintf (bufptr, "Not installed");
			break;
		default:
			bufptr += sprintf (bufptr, "%dMbyte", (1<<(data[9]&0x7F)));
			if(data[9]&0x80)
				bufptr += sprintf (bufptr, " (Double sided)");
	}
	DMI_STR_VALUE("Installed memory size", tmpstr, sockettxt, information);
	
	memset(tmpstr, 0, 80);
	bufptr = tmpstr;
	switch(data[0xa]&0x7F) {
		case 0x7D:
			bufptr += sprintf (bufptr, "Unknown");
			break;
		case 0x7E:
			bufptr += sprintf (bufptr, "Disabled");
			break;
		case 0x7F:
			bufptr += sprintf (bufptr, "Not installed");
			break;
		default:
			bufptr += sprintf (bufptr, "%dMbyte", (1<<(data[10]&0x7F)));
			if(data[0xa]&0x80)
				bufptr += sprintf (bufptr, " (Double sided)");
	}
	DMI_STR_VALUE("Enabled memory size", tmpstr, sockettxt, information);

	if((data[0xb]&7)) {
		if(data[0xb]&(1<<0)) {
			snprintf (tmpstr, 80, "BANK HAS UNCORRECTABLE ERRORS (BIOS DISABLED)");
		}
		if(data[0xb]&(1<<1)) {
			snprintf (tmpstr, 80, "BANK LOGGED CORRECTABLE ERRORS AT BOOT");
		}
		if(data[0xb]&(1<<2)) {
			snprintf (tmpstr, 80, "BANK LOGGED CORRECTABLE ERRORS (see event log)");
		}
		DMI_STR_VALUE("Error Status", tmpstr, sockettxt, information);
	}
}

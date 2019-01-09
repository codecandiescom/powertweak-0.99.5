#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <powertweak.h>
#include "dmi.h"

int memory_controller_number = 0;

void handle_table_0x5(u8 *data, pt_unused struct dmi_header *dm)
{
	char tmpbuf[40];
	char *bufptr = 0;
	char information[]="Information";
	char tablename[22]="Memory controller";
	struct tweak *tweak=NULL;
	struct private_DMI_data *private;
	int ignore = FALSE;

	if (memory_controller_number>0)
	{
		snprintf(tablename, 22, "Memory controller%i", memory_controller_number);
	}
	memory_controller_number++;
	
	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Error detecting method");
	private = (struct private_DMI_data*) tweak->PrivateData;
	switch (data[0x4]) {
		case 1:		private->value.strVal = strdup("Other"); ignore=TRUE;	break;
		case 2:		private->value.strVal = strdup("Unknown"); ignore=TRUE;	break;
		case 3:		private->value.strVal = strdup("none");	break;
		case 4:		private->value.strVal = strdup("8-bit parity");	break;
		case 5:		private->value.strVal = strdup("32-bit parity");	break;
		case 6:		private->value.strVal = strdup("64-bit parity");	break;
		case 7:		private->value.strVal = strdup("128-bit parity");	break;
		case 8:		private->value.strVal = strdup("CRC");	break;
		default:	private->value.strVal = strdup("Reserved"); ignore=TRUE;	break;
	}
/*	if (!ignore)*/
	{
		RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, tablename, information);
	}

	/*data[0x05] - ECC*/
	memset(tmpbuf, 0, 40);
	if(data[0x9]&(1<<3)) {
		snprintf (tmpbuf, 40, "Single bit error correcting");
	}
	if(data[0x9]&(1<<4)) {
		snprintf (tmpbuf, 40, "Double bit error correcting");
	}
	if(data[0x9]&(1<<5)) {
		snprintf (tmpbuf, 40, "error Scrubbing");
	}
	DMI_STR_VALUE("Error correcting Capability", tmpbuf, tablename, information);
	
	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Supported Interleave");
	private = (struct private_DMI_data*) tweak->PrivateData;
	switch (data[0x6]) {
		case 1:		private->value.strVal = strdup("Other"); ignore=TRUE;	break;
		case 2:		private->value.strVal = strdup("Unknown"); ignore=TRUE;	break;
		case 3:		private->value.strVal = strdup("One way Interleave");	break;
		case 4:		private->value.strVal = strdup("Two way Interleave");	break;
		case 5:		private->value.strVal = strdup("Four way Interleave");	break;
		case 6:		private->value.strVal = strdup("Eight way Interleave");	break;
		case 7:		private->value.strVal = strdup("Sixteen way Interleave");	break;
		default:	private->value.strVal = strdup("Reserved"); ignore=TRUE;	break;
	}
/*	if (!ignore)*/
	{
		RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, tablename, information);
	}
	
	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Current Interleave");
	private = (struct private_DMI_data*) tweak->PrivateData;
	switch (data[0x7]) {
		case 1:		private->value.strVal = strdup("Other"); ignore=TRUE;	break;
		case 2:		private->value.strVal = strdup("Unknown"); ignore=TRUE;	break;
		case 3:		private->value.strVal = strdup("One way Interleave");	break;
		case 4:		private->value.strVal = strdup("Two way Interleave");	break;
		case 5:		private->value.strVal = strdup("Four way Interleave");	break;
		case 6:		private->value.strVal = strdup("Eight way Interleave");	break;
		case 7:		private->value.strVal = strdup("Sixteen way Interleave");	break;
		default:	private->value.strVal = strdup("Reserved"); ignore=TRUE;	break;
	}
/*	if (!ignore)*/
	{
		RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, tablename, information);
	}
	
	/*data[0x08] - Maximum memory module size*/
	snprintf(tmpbuf, 40, "%lu MB", (1L<<data[0x8]));
	DMI_STR_VALUE("Maximum memory module size", tmpbuf, tablename, information);

	/*data[0x9] - supported speeds*/
	memset(tmpbuf, 0, 40);
	if(data[0x9]&(1<<2)) {
		snprintf (tmpbuf, 40, "70ns");
	}
	if(data[0x9]&(1<<3)) {
		snprintf (tmpbuf, 40, "60ns");
	}
	if(data[0x9]&(1<<4)) {
		snprintf (tmpbuf, 40, "50ns");
	}
	DMI_STR_VALUE("Supported Speeds", tmpbuf, tablename, information);
	
	/*data[0xB] - supported memory types*/
	dmi_decode_ram(data[0xB]&data[0xC]<<8, bufptr);
	DMI_STR_VALUE("supported memory types", bufptr, tablename, information);
	
	/*data[0xD] - memory module voltage*/
	memset(tmpbuf, 0, 40);
	if(data[0xd]&(1<<0)) {
		snprintf (tmpbuf, 40, "5V");
	}
	if(data[0xd]&(1<<1)) {
		snprintf (tmpbuf, 40, "3.3V");
	}
	if(data[0xd]&(1<<2)) {
		snprintf (tmpbuf, 40, "2.9V");
	}
	DMI_STR_VALUE("memory module voltage", tmpbuf, tablename, information);
	
	/*data[0xE] - number of memory slots*/
	snprintf(tmpbuf, 40, "%i", data[0xE]);
	DMI_STR_VALUE("Number of memory Slots", tmpbuf, tablename, information);

}

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <powertweak.h>
#include "dmi.h"

static char *dmi_memory_device_form_factor(u8 num)
{
	static char *memory_device_form_factor[]={
		"",
		"Other",
		"Unknown",
		"SIMM",
		"SIP",
		"Chip",
		"DIP",
		"ZIP",
		"Proprietary Card",
		"DIMM",
		"TSOP",
		"Row of chips",
		"RIMM",
		"SODIMM",
		"SRIMM",
	};
	if (num > 0x0E)
		return "";
	return memory_device_form_factor[num];
}

static char *dmi_memory_device_type(u8 num)
{
	static char *memory_device_type[]={
		"",
		"Other",
		"Unknown",
		"DRAM",
		"EDRAM",
		"VRAM",
		"SRAM",
		"RAM",
		"ROM",
		"FLASH",
		"EEPROM",
		"FEPROM",
		"EPROM",
		"CDRAM",
		"3DRAM",
		"SDRAM",
		"SGRAM",
		"RDRAM",
		"DDR",
	};
	if (num > 0x12)
		return "";
	return memory_device_type[num];
}



int device_number=0;

void handle_table_0x17(u8 *data, struct dmi_header *dm)
{
	char element[20];
	char information[]="Information";
	struct tweak *tweak=NULL;
	struct private_DMI_data *private;
	char tmp[80];
	u16 u;
	
	snprintf(element, 20, "Socket%i", device_number);
	device_number++;
		
/*	snprintf(tmp, 80, "0x%04X", data[5]<<8|data[4]);
	DMI_STR_VALUE("Array Handle", tmp, element, information);
	
	u = data[7]<<8|data[6];
	if (u == 0xffff) {
		snprintf(tmp, 80, "None");
	} else if (u == 0xfffe) {
		snprintf(tmp, 80, "Not Provided");
	} else {
		snprintf(tmp, 80, "0x%04X", u);
	}
	DMI_STR_VALUE("Error Information Handle", tmp, element, information);
*/
	u = data[9]<<8|data[8];
	if (u == 0xffff)
		snprintf(tmp, 80, "Unknown");
	else
		snprintf(tmp, 80, "%u bits", u);
	DMI_STR_VALUE("Total Width", tmp, element, information);

	u = data[11]<<8|data[10];
	if (u == 0xffff)
		snprintf(tmp, 80, "Unknown");
	else
		snprintf(tmp, 80, "%u bits", u);
	DMI_STR_VALUE("Data Width", tmp, element, information);

	u = data[13]<<8|data[12];
	if (u == 0xffff)
		snprintf(tmp, 80, "Unknown");
	else
		snprintf(tmp, 80, "%u %sbyte", (u&0x7fff), (u&0x8000) ? "K" : "M");
	DMI_STR_VALUE("Size", tmp, element, information);

	DMI_STR_VALUE("Form Factor", dmi_memory_device_form_factor(data[14]), element, information);

	if (data[15] != 0) {
		if (data[15] == 0xff)
			snprintf(tmp, 80, "Unknown");
		else
			snprintf(tmp, 80, "0x%02X", data[15]);
		
		DMI_STR_VALUE("Device Set", tmp, element, information);
	}

	DMI_STR("Device Locator", data[0x10], element, information);

	DMI_STR("Bank Locator", data[0x11], element, information);

	DMI_STR_VALUE("Type", dmi_memory_device_type(data[0x12]), element, information);

	u = data[0x14]<<8|data[0x13];
	if (u&0x1ffe)
	{
		char tmpstr[30];
		
		DMI_CAP(3, "Fast-paged ", element, "Type detail");
		DMI_CAP(4, "Static column ", element, "Type detail");
		DMI_CAP(5, "Pseudo-static", element, "Type detail");
		DMI_CAP(6, "RAMBUS", element, "Type detail");
		DMI_CAP(7, "Synchronous", element, "Type detail");
		DMI_CAP(8, "CMOS", element, "Type detail");
		DMI_CAP(9, "EDO", element, "Type detail");
		DMI_CAP(10, "Window DRAM", element, "Type detail");
		DMI_CAP(11, "Cache DRAM", element, "Type detail");
		DMI_CAP(12, "Non-volatile", element, "Type detail");
	}
	
	if (dm->length > 21) {
		u = data[0x16]<<8|data[0x15];
		if (u == 0)
			snprintf(tmp, 80, "Unknown");
		else
			snprintf(tmp, 80, "%u MHz (%.1f ns)", u, (1000.0/u));

		DMI_STR_VALUE("Speed", tmp, element, information);
	}

	if (dm->length > 23)
		DMI_STR("Manufacturer", data[0x17], element, information);

	if (dm->length > 24)
		DMI_STR("Serial Number", data[0x18], element, information);

	if (dm->length > 25)
		DMI_STR("Asset Tag", data[0x19], element, information);

	if (dm->length > 26)
		DMI_STR("Part Number", data[0x1a], element, information);

}

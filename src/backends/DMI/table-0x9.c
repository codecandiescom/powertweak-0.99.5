#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <powertweak.h>
#include "dmi.h"

const char *dmi_bus_width(u8 code)
{
	static const char *width[]={
		"",
		"",
		"",
		"8bit ",
		"16bit ",
		"32bit ",
		"64bit ",
		"128bit "
	};
	if(code>7)
		return "";
	return width[code];
}

const char *dmi_card_size(u8 v)
{
	if(v==3)
		return("Short ");
	if(v==4)
		return("Long ");
	return "";
}
const char *dmi_bus_name(u8 num)
{
	static const char *bus[]={
		"",
		"",
		"",
		"ISA ",
		"MCA ",
		"EISA ",
		"PCI ",
		"PC Card (PCMCIA) ",
		"VLB ",
		"Proprietary ",
		"Processor Card Slot ",
		"Proprietary memory card slot ",
		"I/O Riser ",
		"NuBus ",
		"PCI-66 ",
		"AGP ",
		"AGP 2x ",
		"PCI-X ",
		"AGP 8x "
	};
	static const char *jpbus[]={
		"PC98/C20",
		"PC98/C24",
		"PC98/E",
		"PC98/LocalBus",
		"PC98/Card"
	};
	
	if(num<=0x13)
		return bus[num];
	if(num>=0xA0 && num<=0xA4)
		return jpbus[num - 0xA0];
	return "";
}



char information[]="DMI";

int slot_number = 1;

void handle_table_0x9(u8 *data, struct dmi_header *dm)
{
	char slotDesignation[20];
	char captxt[]="Capabilities";
	char extcaptxt[]="Extended Capabilities";
	char tmpstr[80];
	struct tweak *tweak=NULL;
	struct private_DMI_data *private;
	u32 u;
	
	snprintf(slotDesignation, 20, "%i - %s", slot_number, dmi_string(dm, data[4]));
	slot_number++;
	_DMI_STR_VALUE("Slots", "bus width", dmi_bus_width(data[6]), slotDesignation, information);
	_DMI_STR_VALUE("Slots", "card size", dmi_card_size(data[8]), slotDesignation, information);
	_DMI_STR_VALUE("Slots", "bus name", dmi_bus_name(data[5]), slotDesignation, information);

	memset(tmpstr, 0, 80);
	if(data[0x7]==3) snprintf(tmpstr, 80, "Available");
	if(data[0x7]==4) snprintf(tmpstr, 80, "In use");
	_DMI_STR_VALUE("Slots", "Status", tmpstr, slotDesignation, information);
	
	/*SlotID*/
	if (data[0x5] == 0x4) /*MCA*/
	{
		memset(tmpstr, 0, 80);
		snprintf(tmpstr, 80, "%i", data[0x9]);
		_DMI_STR_VALUE("Slots", "Slot ID", tmpstr, slotDesignation, information);
	}
	else if (data[0x5] == 0x5) /*EISA*/
	{
		memset(tmpstr, 0, 80);
		snprintf(tmpstr, 80, "%i", data[0x9]);
		_DMI_STR_VALUE("Slots", "Slot ID", tmpstr, slotDesignation, information);
	}
	else if ((data[0x5] == 0x6)||
			(data[0x5] == 0xe)||
			(data[0x5] == 0xf)||
			(data[0x5] == 0x10)||
			(data[0x5] == 0x12)||
			(data[0x5] == 0x13))
	{/*PCI, AGP, PCI-X*/
		memset(tmpstr, 0, 80);
		snprintf(tmpstr, 80, "%i", data[0x9]);
		_DMI_STR_VALUE("Slots", "Slot ID", tmpstr, slotDesignation, information);
	}
	else if (data[0x5] == 0x7) /*PCMCIA*/
	{
		memset(tmpstr, 0, 80);
		snprintf(tmpstr, 80, "%i:%i", data[0x9], data[0x0a]);
		_DMI_STR_VALUE("Slots", "ID", tmpstr, slotDesignation, information);
	}
	
	/*slot chara 1 & 2*/				
	u = data[0xb];
	
	_CAP("Slots", 1,  "provides 5v", slotDesignation, captxt);
	_CAP("Slots", 2,  "provides 3.3v", slotDesignation, captxt);
	_CAP("Slots", 3,  "Shared slot opening", slotDesignation, captxt);
	_CAP("Slots", 4,  "PC Card 16 (PCMCIA)", slotDesignation, captxt);
	_CAP("Slots", 5,  "CardBus", slotDesignation, captxt);
	_CAP("Slots", 6,  "Zoom-Video", slotDesignation, captxt);
	_CAP("Slots", 7,  "Modem ring resume", slotDesignation, captxt);
	
	u = data[0xc];
	
	_CAP("Slots", 0,  "PCI Slot supports PME (Power management enable) signal", slotDesignation, extcaptxt);
	_CAP("Slots", 1,  "Slot supports hot-plug devices", slotDesignation, extcaptxt);
	_CAP("Slots", 2,  "slot supports SMBUS signal", slotDesignation, extcaptxt);

}

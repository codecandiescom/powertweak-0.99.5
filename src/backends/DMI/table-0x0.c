#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <powertweak.h>
#include "dmi.h"

/*SVEN - this could be moved to //hardware/BIOS rather than 4 levels down*/

void handle_table_0x0(u8 *data, struct dmi_header *dm)
{
	char captxt[]="Capabilities";
	char extcaptxt[]="Extended Capabilities";
	char biosinfo[]="BIOS Information";
	char information[]="Information";
	char tmpstr[80];
	struct tweak *tweak=NULL;
	struct private_DMI_data *private;
	u32 u;

	DMI_STR ("Vendor", data[4],  biosinfo, information);
	DMI_STR ("Version", data[5],  biosinfo, information);
	DMI_STR ("Release date", data[8], biosinfo, information);

	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup("BIOS base");
	sprintf (tmpstr, "0x%04X0", data[7]<<8|data[6]);
	private = (struct private_DMI_data*) tweak->PrivateData;
	private->value.strVal = strdup(tmpstr);
	RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, "BIOS Information", "Information");

	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup("ROM capacity");
	sprintf (tmpstr, "%dK", 64 * (data[9]+1));
	private = (struct private_DMI_data*) tweak->PrivateData;
	private->value.strVal = strdup(tmpstr);
	RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, "BIOS Information", "Information");

	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup("BIOS image size");
	sprintf (tmpstr, "%dK", (0x10000-(data[7]<<8|data[6]))*16/1024);
	private = (struct private_DMI_data*) tweak->PrivateData;
	private->value.strVal = strdup(tmpstr);
	RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, "BIOS Information", "Information");
	
	u=data[13]<<24|data[12]<<16|data[11]<<8|data[10];
	/*u2=data[17]<<24|data[16]<<16|data[15]<<8|data[14];*/

	DMI_CAP(4,  "ISA", biosinfo, captxt);
	DMI_CAP(5,  "MCA", biosinfo, captxt);
	DMI_CAP(6,  "EISA", biosinfo, captxt);
	DMI_CAP(7,  "PCI", biosinfo, captxt);
	DMI_CAP(8,  "PC Card (PCMCIA)", biosinfo, captxt);
	DMI_CAP(9,  "PnP", biosinfo, captxt);
	DMI_CAP(10, "APM", biosinfo, captxt);
	DMI_CAP(11, "BIOS is flash upgradable", biosinfo, captxt);
	DMI_CAP(12, "BIOS shadowing allowed", biosinfo, captxt);
	DMI_CAP(13, "VL-VESA", biosinfo, captxt);
	DMI_CAP(14, "ESCD", biosinfo, captxt);
	DMI_CAP(15, "Boot from CD", biosinfo, captxt);
	DMI_CAP(16, "Selectable Boot", biosinfo, captxt);
	DMI_CAP(17, "BIOS ROM is socketed", biosinfo, captxt);
	DMI_CAP(18, "Boot from PC Card (PCMCIA)", biosinfo, captxt);
	DMI_CAP(19, "EDD (Enhanced Disk Drive)", biosinfo, captxt);
	DMI_CAP(20, "Int 13h Japanese floppy for NEC 9800", biosinfo, captxt);
	DMI_CAP(21, "Int 13h Japanese floppy for Toshiba", biosinfo, captxt);
	DMI_CAP(22, "Int 13h 5.25\" 360KB floppy services", biosinfo, captxt);
	DMI_CAP(23, "Int 13h 5.25\" 1.2MB floppy services", biosinfo, captxt);
	DMI_CAP(24, "Int 13h 3.5\" 720KB floppy services", biosinfo, captxt);
	DMI_CAP(25, "Int 13h 3.5\" 2.88 floppy services", biosinfo, captxt);
	DMI_CAP(26, "Int 5h Print screen services", biosinfo, captxt);
	DMI_CAP(27, "Int 9h 8042 Keyboard services", biosinfo, captxt);
	DMI_CAP(28, "Int 14h Serial services", biosinfo, captxt);
	DMI_CAP(29, "Int 17h Printer services", biosinfo, captxt);
	DMI_CAP(30, "Int 10h CGA/Mono video services", biosinfo, captxt);
	DMI_CAP(31, "NEC PC-98", biosinfo, captxt);

	u=data[18];
	DMI_CAP(0, "ACPI", biosinfo, extcaptxt);
	DMI_CAP(1, "USB Legacy Support", biosinfo, extcaptxt);
	DMI_CAP(2, "AGP", biosinfo, extcaptxt);
	DMI_CAP(3, "I2O boot", biosinfo, extcaptxt);
	DMI_CAP(4, "LS120 boot", biosinfo, extcaptxt);
	DMI_CAP(5, "ATAPI ZIP boot", biosinfo, extcaptxt);
	DMI_CAP(6, "1394 boot", biosinfo, extcaptxt);
	DMI_CAP(7, "Smart battery", biosinfo, extcaptxt);
}

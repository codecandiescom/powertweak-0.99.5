#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <powertweak.h>
#include "dmi.h"

char *Hardware = "Hardware";

#define HW_STR(str, x, tree, tab) \
	if (x) {\
		tweak = alloc_DMI_tweak (TYPE_INFO_STRING);\
		tweak->WidgetText = strdup (str);\
		private = (struct private_DMI_data*) tweak->PrivateData;\
		private->value.strVal = strdup(dmi_string(dm, x));\
		RegisterTweak (tweak, "mmt", Hardware, tree, tab);\
	}

#define HW_STR_VALUE(str, sx, tree, tab) \
	if (sx) {\
		tweak = alloc_DMI_tweak (TYPE_INFO_STRING);\
		tweak->WidgetText = strdup (str);\
		private = (struct private_DMI_data*) tweak->PrivateData;\
		private->value.strVal = strdup(sx);\
		RegisterTweak (tweak, "mmt", Hardware, tree, tab);\
	}	
	
#define HW_CAP(bit, str, tree, tab)\
	tweak = alloc_DMI_tweak (TYPE_INFO_BOOL);\
	tweak->WidgetText = strdup (str);\
	private = (struct private_DMI_data*) tweak->PrivateData;\
	if (u&(1<<bit))\
		set_value_int (private->value, 1);\
	else\
		set_value_int (private->value, 0);\
	sprintf(tmpstr, "    %s", str);\
	RegisterTweak (tweak, "mmt", Hardware, tree, tab);\

int cpu_number = 0;

void handle_table_0x4(u8 *data, struct dmi_header *dm)
{
	char processor[10];
	char information[]="BIOS - DMI";
	char tmpstr[80];
	u16 dataword;
	u32 u;
	struct tweak *tweak=NULL;
	struct private_DMI_data *private;
		
	snprintf(processor, 10, "CPU%i", cpu_number);
	cpu_number += 1;	

	HW_STR("Socket designation", data[0x4], processor, information);
	HW_STR("Manufacturer", data[0x7], processor, information);
	HW_STR("version", data[0x10], processor, information);
	if (data[0x10])	HW_STR("Serial Number", data[0x20], processor, information);
	if (data[0x21])	HW_STR("Asset Number", data[0x21], processor, information);
	if (data[0x22])	HW_STR("Part Number", data[0x22], processor, information);

	/*data[0x08] - ProcessorID - see 3.3.5.3*/
	
	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Processor type");
	private = (struct private_DMI_data*) tweak->PrivateData;
	switch (data[0x5]) {
		case 1:		private->value.strVal = strdup("Other");	break;
		case 2:		private->value.strVal = strdup("Unknown");	break;
		case 3:		private->value.strVal = strdup("Central processor");	break;
		case 4:		private->value.strVal = strdup("Math processor");	break;
		case 5:		private->value.strVal = strdup("DSP processor");	break;
		case 6:		private->value.strVal = strdup("Video processor");	break;
		default:	private->value.strVal = strdup("Reserved");	break;
	}
	RegisterTweak (tweak, "mmt", Hardware, processor, information);

	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	tweak->WidgetText = strdup ("Processor family");
	private = (struct private_DMI_data*) tweak->PrivateData;
	switch (data[0x6]) {
		case 1:		private->value.strVal = strdup("Other");	break;
		case 2:		private->value.strVal = strdup("Unknown");	break;
		case 3:		private->value.strVal = strdup("8086"); break;
		case 4:		private->value.strVal = strdup("80286");	break;
		case 5:		private->value.strVal = strdup("Intel386");	break;
		case 6:		private->value.strVal = strdup("Intel486");	break;
		case 7:		private->value.strVal = strdup("8087");	break;
		case 8:		private->value.strVal = strdup("80287");	break;
		case 9:		private->value.strVal = strdup("80387");	break;
		case 0xa:	private->value.strVal = strdup("80487");	break;
		case 0xb:	private->value.strVal = strdup("Pentium family");	break;
		case 0xc:	private->value.strVal = strdup("Pentium Pro");	break;
		case 0xd:	private->value.strVal = strdup("Pentium II");	break;
		case 0xe:	private->value.strVal = strdup("Pentium MMX");	break;
		case 0xf:	private->value.strVal = strdup("Celeron");	break;
		case 0x10:	private->value.strVal = strdup("Pentium II Xeon");	break;
		case 0x11:	private->value.strVal = strdup("Pentium III");	break;
		case 0x12:	private->value.strVal = strdup("M1 family");	break;
		case 0x13:	private->value.strVal = strdup("M2 family");	break;
		case 0x14:	private->value.strVal = strdup("");	break;
		case 0x15:	private->value.strVal = strdup("");	break;
		case 0x16:	private->value.strVal = strdup("");	break;
		case 0x17:	private->value.strVal = strdup("");	break;
		case 0x18:	private->value.strVal = strdup("AMD Duron");	break;
		case 0x19:	private->value.strVal = strdup("K5");	break;
		case 0x1a:	private->value.strVal = strdup("K6");	break;
		case 0x1b:	private->value.strVal = strdup("K6-2");	break;
		case 0x1c:	private->value.strVal = strdup("K6-3");	break;
		case 0x1d:	private->value.strVal = strdup("AMD Athlon");	break;
		case 0x1e:	private->value.strVal = strdup("AMD2900");	break;
		case 0x1f:	private->value.strVal = strdup("K6-2+");	break;
		case 0x20:	private->value.strVal = strdup("PowerPC family");	break;
		case 0x21:	private->value.strVal = strdup("PowerPC 601");	break;
		case 0x22:	private->value.strVal = strdup("PowerPC 603");	break;
		case 0x23:	private->value.strVal = strdup("PowerPC 603+");	break;
		case 0x24:	private->value.strVal = strdup("PowerPC 604");	break;
		case 0x25:	private->value.strVal = strdup("PowerPC 620");	break;
		case 0x26:	private->value.strVal = strdup("PowerPC x704");	break;
		case 0x27:	private->value.strVal = strdup("PowerPC 750");	break;
		case 0x28:	private->value.strVal = strdup("");	break;
		case 0x29:	private->value.strVal = strdup("");	break;
		case 0x2a:	private->value.strVal = strdup("");	break;
		case 0x2b:	private->value.strVal = strdup("");	break;
		case 0x2c:	private->value.strVal = strdup("");	break;
		case 0x2d:	private->value.strVal = strdup("");	break;
		case 0x2e:	private->value.strVal = strdup("");	break;
		case 0x2f:	private->value.strVal = strdup("");	break;
		case 0x30:	private->value.strVal = strdup("Alpha");	break;
		case 0x31:	private->value.strVal = strdup("Alpha 21064");	break;
		case 0x32:	private->value.strVal = strdup("Alpha 21066");	break;
		case 0x33:	private->value.strVal = strdup("Alpha 21164");	break;
		case 0x34:	private->value.strVal = strdup("Alpha 21164PC");	break;
		case 0x35:	private->value.strVal = strdup("Alpha 21164a");	break;
		case 0x36:	private->value.strVal = strdup("Alpha 21264");	break;
		case 0x37:	private->value.strVal = strdup("Alpha 21364");	break;
		case 0x38:	private->value.strVal = strdup("");	break;
		case 0x39:	private->value.strVal = strdup("");	break;
		case 0x3a:	private->value.strVal = strdup("");	break;
		case 0x3b:	private->value.strVal = strdup("");	break;
		case 0x3c:	private->value.strVal = strdup("");	break;
		case 0x3d:	private->value.strVal = strdup("");	break;
		case 0x3e:	private->value.strVal = strdup("");	break;
		case 0x3f:	private->value.strVal = strdup("");	break;
		case 0x40:	private->value.strVal = strdup("MIPS family");	break;
		case 0x41:	private->value.strVal = strdup("MIPS R4000");	break;
		case 0x42:	private->value.strVal = strdup("MIPS R4200");	break;
		case 0x43:	private->value.strVal = strdup("MIPS R4400");	break;
		case 0x44:	private->value.strVal = strdup("MIPS R4600");	break;
		case 0x45:	private->value.strVal = strdup("MIPS R10000");	break;
		case 0x50:	private->value.strVal = strdup("SPARC family");	break;
		case 0x51:	private->value.strVal = strdup("SuperSPARC");	break;
		case 0x52:	private->value.strVal = strdup("microSPARC II");	break;
		case 0x53:	private->value.strVal = strdup("microSPARC IIep");	break;
		case 0x54:	private->value.strVal = strdup("UltraSPARC");	break;
		case 0x55:	private->value.strVal = strdup("UltraSPARC II");	break;
		case 0x56:	private->value.strVal = strdup("UltraSPARC III");	break;
		case 0x57:	private->value.strVal = strdup("UltraSPARC III");	break;
		case 0x58:	private->value.strVal = strdup("UltraSPARC IIII");	break;
		case 0x60:	private->value.strVal = strdup("68040 family");	break;
		case 0x61:	private->value.strVal = strdup("68xxx");	break;
		case 0x62:	private->value.strVal = strdup("68000");	break;
		case 0x63:	private->value.strVal = strdup("68010");	break;
		case 0x64:	private->value.strVal = strdup("68020");	break;
		case 0x65:	private->value.strVal = strdup("68030");	break;
		case 0x70:	private->value.strVal = strdup("Hobbit family");	break;
		case 0x78:	private->value.strVal = strdup("Crusoe TM5000 Family");	break;
		case 0x79:	private->value.strVal = strdup("Crusoe TM3000 Family");	break;
		case 0x80:	private->value.strVal = strdup("Weitek");	break;
		case 0x82:	private->value.strVal = strdup("Itanium");	break;
		case 0x83:	private->value.strVal = strdup("AMD Athlon 64");	break;
		case 0x84:	private->value.strVal = strdup("AMD Opteron");	break;
		case 0x90:	private->value.strVal = strdup("PA-RISC family");	break;
		case 0x91:	private->value.strVal = strdup("PA-RISC 8500");	break;
		case 0x92:	private->value.strVal = strdup("PA-RISC 8000");	break;
		case 0x93:	private->value.strVal = strdup("PA-RISC 7300LC");	break;
		case 0x94:	private->value.strVal = strdup("PA-RISC 7200");	break;
		case 0x95:	private->value.strVal = strdup("PA-RISC 7100LC");	break;
		case 0x96:	private->value.strVal = strdup("PA-RISC 7100");	break;
		case 0xa0:	private->value.strVal = strdup("V30 family");	break;
		case 0xb0:	private->value.strVal = strdup("Pentium III Xeon");	break;
		case 0xb1:	private->value.strVal = strdup("Pentium III with SpeedStep");	break;
		case 0xb2:	private->value.strVal = strdup("Pentium 4");	break;
		case 0xb3:	private->value.strVal = strdup("Intel Xeon");	break;
		case 0xb4:	private->value.strVal = strdup("AS400 Family");	break;
		case 0xb5:	private->value.strVal = strdup("Intel Xeon MP");	break;
		case 0xb6:	private->value.strVal = strdup("AMD Athlon XP");	break;
		case 0xb7:	private->value.strVal = strdup("AMD Athlon MP");	break;
		case 0xb8:	private->value.strVal = strdup("Intel Itanuim 2");	break;
		case 0xc8:	private->value.strVal = strdup("IBM 390 Family");	break;
		case 0xc9:	private->value.strVal = strdup("G4");	break;
		case 0xca:	private->value.strVal = strdup("G5");	break;
		case 0xfa:	private->value.strVal = strdup("i860");	break;
		case 0xfb:	private->value.strVal = strdup("i960");	break;
		default:	private->value.strVal = strdup("Reserved");	break;
	}
	RegisterTweak (tweak, "mmt", Hardware, processor, information);

	if (data[0x11] & 0x70) {	/* Sanity check reserved bits (should be 0) */
		tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
		tweak->WidgetText = strdup ("Processor voltage");
		private = (struct private_DMI_data*) tweak->PrivateData;
		if ((data[0x11] & 0x80)==0) {
			switch (data[0x11] & 0x7) {
				case 1:		private->value.strVal = strdup("5V");	break;
				case 2:		private->value.strVal = strdup("3.3V");	break;
				case 4:		private->value.strVal = strdup("2.9V");	break;
				default:	private->value.strVal = strdup("Reserved");	break;
			}
		} else {
			(void) sprintf(tmpstr, "%d.%dV",
				(data[0x11] & 0x7f) / 10,
				(data[0x11] & 0x7f) - (((data[0x11] & 0x7f) / 10)*10) );
			private->value.strVal = strdup(tmpstr);
		}
	RegisterTweak (tweak, "mmt", Hardware, processor, information);
	}

	dataword = (data[0x13]<<8) | data[0x12];
	if (dataword != 0) {
		tweak = alloc_DMI_tweak (TYPE_INFO_INT);
		private = (struct private_DMI_data*) tweak->PrivateData;
		set_value_int (private->value, dataword);
	} else {
		tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
		private = (struct private_DMI_data*) tweak->PrivateData;
		private->value.strVal = strdup("Unknown");
	}
	tweak->WidgetText = strdup ("External clock");
	RegisterTweak (tweak, "mmt", Hardware, processor, information);


	dataword = (data[0x15]<<8) | data[0x14];
	if (dataword != 0) {
		tweak = alloc_DMI_tweak (TYPE_INFO_INT);
		private = (struct private_DMI_data*) tweak->PrivateData;
		set_value_int (private->value, dataword);
	} else {
		tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
		private = (struct private_DMI_data*) tweak->PrivateData;
		private->value.strVal = strdup("Unknown");
	}
	tweak->WidgetText = strdup ("Max speed");
	RegisterTweak (tweak, "mmt", Hardware, processor, information);


	dataword = (data[0x17]<<8) | data[0x16];
	if (dataword != 0) {
		tweak = alloc_DMI_tweak (TYPE_INFO_INT);
		private = (struct private_DMI_data*) tweak->PrivateData;
		set_value_int (private->value, dataword);
	} else {
		tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
		private = (struct private_DMI_data*) tweak->PrivateData;
		private->value.strVal = strdup("Unknown");
	}
	tweak->WidgetText = strdup ("Current speed");
	RegisterTweak (tweak, "mmt", Hardware, processor, information);

	if ((data[0x18] & 0xb8)==0) {	/* Sanity check reserved bits. (should be zero) */
		u=data[0x18];
		HW_CAP(0, "CPU socket populated", processor, information);

		tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
		private = (struct private_DMI_data*) tweak->PrivateData;
		switch (data[0x18] & 0x7) {
			case 0:	private->value.strVal = strdup("Unknown");	break;
			case 1:	private->value.strVal = strdup("CPU Enabled");	break;
			case 2:	private->value.strVal = strdup("CPU Disabled by user via BIOS setup");	break;
			case 3:	private->value.strVal = strdup("CPU Disabled by BIOS (POST Error)");	break;
			case 4:	private->value.strVal = strdup("CPU is idle, waiting to be enabled");	break;
			case 5:
			case 6:	private->value.strVal = strdup("Reserved");	break;
			case 7:	private->value.strVal = strdup("Other");	break;
			default:    private->value.strVal = strdup("Reserved"); break;
		}
		tweak->WidgetText = strdup ("Status");
	RegisterTweak (tweak, "mmt", Hardware, processor, information);
	}

	tweak = alloc_DMI_tweak (TYPE_INFO_STRING);
	private = (struct private_DMI_data*) tweak->PrivateData;
	switch (data[0x19]) {
		case 0x1:	private->value.strVal = strdup("Other");	break;
		case 0x2:	private->value.strVal = strdup("Unknown");	break;
		case 0x3:	private->value.strVal = strdup("Daughter board");	break;
		case 0x4:	private->value.strVal = strdup("ZIF socket");	break;
		case 0x5:	private->value.strVal = strdup("Replaceable piggy back");	break;
		case 0x6:	private->value.strVal = strdup("None");	break;
		case 0x7:	private->value.strVal = strdup("LIF socket");	break;
		case 0x8:	private->value.strVal = strdup("Slot 1");	break;
		case 0x9:	private->value.strVal = strdup("Slot 2");	break;
		case 0xa:	private->value.strVal = strdup("370-pin Socket");	break;
		case 0xb:	private->value.strVal = strdup("Slot A");	break;
		case 0xc:	private->value.strVal = strdup("Slot M");	break;
		case 0xd:	private->value.strVal = strdup("Socket 423");	break;
		case 0xe:	private->value.strVal = strdup("Socket A");	break;
		case 0xf:	private->value.strVal = strdup("Socket 478");	break;
		case 0x10:	private->value.strVal = strdup("Socket 754");	break;
		case 0x11:	private->value.strVal = strdup("Socket 940");	break;
		default:
				sprintf (tmpstr, "%s (%x)", "Reserved", data[19]);
				private->value.strVal = strdup(tmpstr);
				break;
	}
	tweak->WidgetText = strdup ("Upgrade");
	RegisterTweak (tweak, "mmt", Hardware, processor, information);
	
	/*data[0x1a] - L1 cache handle*/
	/*data[0x1c] - L2 cache handle*/
	/*data[0x1e] - L3 cache handle*/
}

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <powertweak.h>
#include "dmi.h"

/*from dmdecode ;)*/
const char *dmi_port_connector_type(u8 code)
{
	static const char *connector_type[]={
		"None",
		"Centronics",
		"Mini Centronics",
		"Proprietary",
		"DB-25 pin male",
		"DB-25 pin female",
		"DB-15 pin male",
		"DB-15 pin female",
		"DB-9 pin male",
		"DB-9 pin female",
		"RJ-11", 
		"RJ-45",
		"50 Pin MiniSCSI",
		"Mini-DIN",
		"Micro-DIN",
		"PS/2",
		"Infrared",
		"HP-HIL",
		"Access Bus (USB)",
		"SSA SCSI",
		"Circular DIN-8 male",
		"Circular DIN-8 female",
		"On Board IDE",
		"On Board Floppy",
		"9 Pin Dual Inline (pin 10 cut)",
		"25 Pin Dual Inline (pin 26 cut)",
		"50 Pin Dual Inline",
		"68 Pin Dual Inline",
		"On Board Sound Input from CD-ROM",
		"Mini-Centronics Type-14",
		"Mini-Centronics Type-26",
		"Mini-jack (headphones)",
		"BNC",
		"1394",
		"PC-98",
		"PC-98Hireso",
		"PC-H98",
		"PC-98Note",
		"PC98Full",
	};
	
	if(code == 0xFF)
		return "Other";
	
	if(code <= 0x21)
		return connector_type[code];
	
	if((code >= 0xA0) && (code <= 0xA4))
		return connector_type[code-0xA0+0x22];
	
	return "";
}

const char *dmi_port_type(u8 code)
{
	static const char *port_type[]={
		"None",
		"Parallel Port XT/AT Compatible",
		"Parallel Port PS/2",
		"Parallel Port ECP",
		"Parallel Port EPP",
		"Parallel Port ECP/EPP",
		"Serial Port XT/AT Compatible",
		"Serial Port 16450 Compatible",
		"Serial Port 16650 Compatible",
		"Serial Port 16650A Compatible",
		"SCSI Port",
		"MIDI Port",
		"Joy Stick Port",
		"Keyboard Port",
		"Mouse Port",
		"SSA SCSI",
		"USB",
		"FireWire (IEEE P1394)",
		"PCMCIA Type I",
		"PCMCIA Type II",
		"PCMCIA Type III",
		"Cardbus",
		"Access Bus Port",
		"SCSI II",
		"SCSI Wide",
		"PC-98",
		"PC-98-Hireso",
		"PC-H98",
		"Video Port",
		"Audio Port",
		"Modem Port",
		"Network Port",
		"8251 Compatible",
		"8251 FIFO Compatible",
	};
	
	if(code == 0xFF)
		return "Other";

	if (code <= 0xA1)
		return port_type[code];

/*	
	if (code <= 0x1F)
		return port_type[code];
	
	if ((code >= 0xA0) && (code <= 0xA1))
		return port_type[code-0xA0+0x20];
*/	
	return "";
}
static char *dmi_memory_array_location(u8 num)
{
	static char *memory_array_location[]={
		"",
		"Other",
		"Unknown",
		"System board or motherboard",
		"ISA add-on card",
		"EISA add-on card",
		"PCI add-on card",
		"MCA add-on card",
		"PCMCIA add-on card",
		"Proprietary add-on card",
		"NuBus",
	};
	static char *jp_memory_array_location[]={
		"PC-98/C20 add-on card",
		"PC-98/C24 add-on card",
		"PC-98/E add-on card",
		"PC-98/Local buss add-on card",
	};
	if(num<=0x0A)
		return memory_array_location[num];
	if(num>=0xA0 && num<0xA3)
		return jp_memory_array_location[num];
	return "";
}

static char *dmi_memory_array_use(u8 num)
{
	static char *memory_array_use[]={
		"",
		"Other",
		"Unknown",
		"System memory",
		"Video memory",
		"Flash memory",
		"Non-volatile RAM",
		"Cache memory",
	};
	if (num > 0x07)
		return "";
	return memory_array_use[num];
}

static char *dmi_memory_array_error_correction_type(u8 num)
{
	static char *memory_array_error_correction_type[]={
		"",
		"Other",
		"Unknown",
		"None",
		"Parity",
		"Single-bit ECC",
		"Multi-bit ECC",
		"CRC",
	};
	if (num > 0x07)
		return "";
	return memory_array_error_correction_type[num];
}


/*end stealing from AlanCox..*/

/*begin stealing from Jean Delvare*/
/*
 * The specification isn't very clear on endianness problems, so we better
 * have macros for these. It also helps us solve problems on systems that
 * don't support non-aligned memory access. This isn't a big issue IMHO,
 * since SMBIOS/DMI is intended mainly for Intel and compatible systems,
 * which are little-endian and support non-aligned memory access. Anyway,
 * you may use the following defines to control the way it works:
 * - Define BIGENDIAN on big-endian systems.
 * - Define ALIGNMENT_WORKAROUND if your system doesn't support
 *   non-aligned memory access. In this case, we use a slower, but safer,
 *   memory access method.
 * - If it happens that the table is supposed to be always little-endian
 *   ordered regardless of the architecture, define TABLE_LITTLEENDIAN.
 * You most probably will have to define none or the three of them.
 */

#ifndef TABLE_LITTLEENDIAN
#	ifdef BIGENDIAN
	typedef struct {
		u32 h;
		u32 l;
	} u64;
#	else /* BIGENDIAN */
	typedef struct {
		u32 l;
		u32 h;
	} u64;
#	endif /* BIGENDIAN */
#	ifdef ALIGNMENT_WORKAROUND
#		ifdef BIGENDIAN
#		define WORD(x) (u16)((x)[1]+((x)[0]<<8))
#		define DWORD(x) (u32)((x)[3]+((x)[2]<<8)+((x)[1]<<16)+((x)[0]<<24))
#		define QWORD(x) (U64(DWORD(x+4), DWORD(x)))
#		else /* BIGENDIAN */
#		define WORD(x) (u16)((x)[0]+((x)[1]<<8))
#		define DWORD(x) (u32)((x)[0]+((x)[1]<<8)+((x)[2]<<16)+((x)[3]<<24))
#		define QWORD(x) (U64(DWORD(x), DWORD(x+4)))
#		endif /* BIGENDIAN */
#	else /* ALIGNMENT_WORKAROUND */
#	define WORD(x) (u16)(*(u16 *)(x))
#	define DWORD(x) (u32)(*(u32 *)(x))
#	define QWORD(x) (*(u64 *)(x))
#	endif /* ALIGNMENT_WORKAROUND */
#else /* TABLE_LITTLEENDIAN */
typedef struct {
	u32 l;
	u32 h;
} u64;
#define WORD(x) (u16)((x)[0]+((x)[1]<<8))
#define DWORD(x) (u32)((x)[0]+((x)[1]<<8)+((x)[2]<<16)+((x)[3]<<24))
#define QWORD(x) (U64(DWORD(x), DWORD(x+4)))
#endif /* TABLE_LITTLEENDIAN */

char out_of_spec[] = "";/*TODO*/
static char tmp_buf[40];
static int dmi_bcd_range(u8 value, u8 low, u8 high)
{
	if(value>0x99 || (value&0x0F)>0x09)
		return 0;
	if(value<low || value>high)
		return 0;
	return 1;
}
/*
 * 3.3.25 Hardware Security (Type 24)
 */

static const char *dmi_hardware_security_status(u8 code)
{
	static const char *status[]={
		"Disabled", /* 0x00 */
		"Enabled",
		"Not Implemented",
		"Unknown" /* 0x03 */
	};
	
	return status[code];
}

/*
 * 3.3.26 System Power Controls (Type 25)
 */

static char *dmi_power_controls_power_on(u8 *p)
{
	char *ptr;
	int len;
	
	ptr = tmp_buf;
	len = 0;
	/* 3.3.26.1 */
	if(dmi_bcd_range(p[0], 0x01, 0x12))
		len += snprintf((ptr+len), 39-len, " %02X", p[0]);
	else
		len += snprintf((ptr+len), 39-len, " *");
	
	if(dmi_bcd_range(p[1], 0x01, 0x31))
		len += snprintf((ptr+len), 39-len, "-%02X", p[1]);
	else
		len += snprintf((ptr+len), 39-len, "-*");
	
	if(dmi_bcd_range(p[2], 0x00, 0x23))
		len += snprintf((ptr+len), 39-len, " %02X", p[2]);
	else
		len += snprintf((ptr+len), 39-len, " *");
	
	if(dmi_bcd_range(p[3], 0x00, 0x59))
		len += snprintf((ptr+len), 39-len, ":%02X", p[3]);
	else
		len += snprintf((ptr+len), 39-len, ":*");
	
	if(dmi_bcd_range(p[4], 0x00, 0x59))
		len += snprintf((ptr+len), 39-len, ":%02X", p[4]);
	else
		len += snprintf((ptr+len), 39-len, ":*");

	return tmp_buf;
}
/*
 * 3.3.27 Voltage Probe (Type 26)
 */

static const char *dmi_voltage_probe_location(u8 code)
{
	/* 3.3.27.1 */
	static const char *location[]={
		"Other", /* 0x01 */
		"Unknown",
		"Processor",
		"Disk",
		"Peripheral Bay",
		"System Management Module",
		"Motherboard",
		"Memory Module",
		"Processor Module",
		"Power Unit",
		"Add-in Card" /* 0x0B */
	};
	
	if(code>=0x01 && code<=0x0B)
		return location[code-0x01];
	return out_of_spec;
}

static const char *dmi_probe_status(u8 code)
{
	/* 3.3.27.1 */
	static const char *status[]={
		"Other", /* 0x01 */
		"Unknown",
		"OK",
		"Non-critical",
		"Critical",
		"Non-recoverable" /* 0x06 */
	};
	
	if(code>=0x01 && code<=0x06)
		return status[code-0x01];
	return out_of_spec;
}

static char *dmi_voltage_probe_value(u16 code)
{
	static char tmp[10] = "";

	if(code!=0x8000)
		snprintf(tmp, 9, " %.3f V", (float)(signed short)code/1000);
	
	return tmp;
}

static char *dmi_voltage_probe_resolution(u16 code)
{
	tmp_buf[0] = '\0';
	if(code!=0x8000)
		snprintf(tmp_buf, 9, " %.1f mV", (float)code/10);
	return tmp_buf;
}

static char *dmi_probe_accuracy(u16 code)
{
	tmp_buf[0] = '\0';
	if(code!=0x8000)
		snprintf(tmp_buf, 10, " %.2f%%", (float)code/100);
	return tmp_buf;
}

/*
 * 3.3.28 Cooling Device (Type 27)
 */

static const char *dmi_cooling_device_type(u8 code)
{
	/* 3.3.28.1 */
	static const char *type[]={
		"Other", /* 0x01 */
		"Unknown",
		"Fan",
		"Centrifugal Blower",
		"Chip Fan",
		"Cabinet Fan",
		"Power Supply Fan",
		"Heat Pipe",
		"Integrated Refrigeration" /* 0x09 */
	};
	static const char *type_0x10[]={
		"Active Cooling", /* 0x10, master.mif says 32 */
		"Passive Cooling" /* 0x11, master.mif says 33 */
	};
	
	if(code>=0x01 && code<=0x09)
		return type[code-0x01];
	if(code>=0x10 && code<=0x11)
		return type_0x10[code-0x10];
	return out_of_spec;
}

static char *dmi_cooling_device_speed(u16 code)
{
	tmp_buf[0] = '\0';
	if(code!=0x8000)
		snprintf(tmp_buf, 9, " %u rpm", code);
	return tmp_buf;
}

/*
 * 3.3.29 Temperature Probe (Type 28)
 */

static const char *dmi_temperature_probe_location(u8 code)
{
	/* 3.3.29.1 */
	static const char *location[]={
		"Other", /* 0x01 */
		"Unknown",
		"Processor",
		"Disk",
		"Peripheral Bay",
		"System Management Module", /* master.mif says SMB MAster */
		"Motherboard",
		"Memory Module",
		"Processor Module",
		"Power Unit",
		"Add-in Card",
		"Front Panel Board",
		"Back Panel Board",
		"Power System Board",
		"Drive Back Plane" /* 0x0F */
	};
	
	if(code>=0x01 && code<=0x0F)
		return location[code-0x01];
	return out_of_spec;
}

static char *dmi_temperature_probe_value(u16 code)
{
	if(code!=0x8000)
		snprintf(tmp_buf, 9, " %.1f deg C", (float)(signed short)code/10);
	return tmp_buf;
}

static char *dmi_temperature_probe_resolution(u16 code)
{
	if(code!=0x8000)
		snprintf(tmp_buf, 9, " %.3f deg C", (float)code/1000);
	return tmp_buf;
}
/*
 * 3.3.30 Electrical Current Probe (Type 29)
 */
static char *dmi_current_probe_value(u16 code)
{
	if(code!=0x8000)
		snprintf(tmp_buf, 9, " %.3f A", (float)(i16)code/1000);
	return tmp_buf;
}

static char *dmi_current_probe_resolution(u16 code)
{
	if(code!=0x8000)
		snprintf(tmp_buf, 9, " %.1f mA", (float)code/10);
	return tmp_buf;
}
/*
 * 3.3.19 32-bit Memory Error Information (Type 18)
 */

static const char *dmi_memory_error_type(u8 code)
{
	/* 3.3.19.1 */
	static const char *type[]={
		"Other", /* 0x01 */
		"Unknown",
		"OK"
		"Bad Read",
		"Parity Error",
		"Single-bit Error",
		"Double-bit Error",
		"Multi-bit Error",
		"Nibble Error",
		"Checksum Error",
		"CRC Error",
		"Corrected Single-bit Error",
		"Corrected Error",
		"Uncorrectable Error" /* 0x0E */
	};
	
	if(code>=0x01 && code<=0x0E)
		return type[code-0x01];
	return out_of_spec;
}

static const char *dmi_memory_error_granularity(u8 code)
{
	/* 3.3.19.2 */
	static const char *granularity[]={
		"Other", /* 0x01 */
		"Unknown",
		"Device Level",
		"Memory Partition Level" /* 0x04 */
	};
	
	if(code>=0x01 && code<=0x04)
		return granularity[code-0x01];
	return out_of_spec;
}

static const char *dmi_memory_error_operation(u8 code)
{
	/* 3.3.19.3 */
	static const char *operation[]={
		"Other", /* 0x01 */
		"Unknown",
		"Read",
		"Write",
		"Partial Write" /* 0x05 */
	};
	
	if(code>=0x01 && code<=0x05)
		return operation[code-0x01];
	return out_of_spec;
}

static char *dmi_memory_error_syndrome(u32 code)
{
	if(code!=0x00000000)
		snprintf(tmp_buf, 9, " 0x%08X", code);
	return tmp_buf;
}

static char *dmi_32bit_memory_error_address(u32 code)
{
	if(code!=0x80000000)
		snprintf(tmp_buf, 9, " 0x%08X", code);
	return tmp_buf;
}
/*
 * 3.3.34 64-bit Memory Error Information (Type 33)
 */

static char *dmi_64bit_memory_error_address(u64 code)
{
	if (!(code.h==0x80000000 && code.l==0x00000000))
		snprintf(tmp_buf, 9, " 0x%08X%08X", code.h, code.l);
	return tmp_buf;
}
/*
 * 3.3.40 System Power Supply (Type 39)
 */

static char *dmi_power_supply_power(u16 code)
{
	if(code!=0x8000)
		snprintf(tmp_buf, 9, " %.3f W", (float)code/1000);
	return tmp_buf;
}

static const char *dmi_power_supply_type(u8 code)
{
	/* 3.3.40.1 */
	static const char *type[]={
		"Other", /* 0x01 */
		"Unknown",
		"Linear",
		"Switching",
		"Battery",
		"UPS",
		"Converter",
		"Regulator" /* 0x08 */
	};
	
	if(code>=0x01 && code<=0x08)
		return type[code-0x01];
	return out_of_spec;
}

static const char *dmi_power_supply_status(u8 code)
{
	/* 3.3.40.1 */
	static const char *status[]={
		"Other", /* 0x01 */
		"Unknown",
		"OK",
		"Non-critical"
		"Critical" /* 0x05 */
	};
	
	if(code>=0x01 && code<=0x05)
		return status[code-0x01];
	return out_of_spec;
}

static const char *dmi_power_supply_range_switching(u8 code)
{
	/* 3.3.40.1 */
	static const char *switching[]={
		"Other", /* 0x01 */
		"Unknown",
		"Manual",
		"Auto-switch",
		"Wide Range",
		"N/A" /* 0x06 */
	};
	
	if(code>=0x01 && code<=0x06)
		return switching[code-0x01];
	return out_of_spec;
}
/*
 * 3.3.35 Management Device (Type 34)
 */

static const char *dmi_management_device_type(u8 code)
{
	/* 3.3.35.1 */
	static const char *type[]={
		"Other", /* 0x01 */
		"Unknown",
		"LM75",
		"LM78",
		"LM79",
		"LM80",
		"LM81",
		"ADM9240",
		"DS1780",
		"MAX1617",
		"GL518SM",
		"W83781D",
		"HT82H791" /* 0x0D */
	};
	
	if(code>=0x01 && code<=0x0D)
		return type[code-0x01];
	return out_of_spec;
}

static const char *dmi_management_device_address_type(u8 code)
{
	/* 3.3.35.2 */
	static const char *type[]={
		"Other", /* 0x01 */
		"Unknown",
		"I/O Port",
		"Memory",
		"SMBus" /* 0x05 */
	};
	
	if(code>=0x01 && code<=0x05)
		return type[code-0x01];
	return out_of_spec;
}
/*
 * 3.3.38 Memory Channel (Type 37)
 */

static const char *dmi_memory_channel_type(u8 code)
{
	/* 3.3.38.1 */
	static const char *type[]={
		"Other", /* 0x01 */
		"Unknown",
		"RAMBus",
		"Synclink" /* 0x04 */
	};
	
	if(code>=0x01 && code<=0x04)
		return type[code-0x01];
	return out_of_spec;
}

/*static void dmi_memory_channel_devices(u8 count, u8 *p, const char *prefix)
{
	int i;
	
	for(i=1; i<=count; i++)
	{
		printf("%sDevice %u Load: %u\n",
			prefix, i, p[3*i]);
		printf("%sDevice %u Handle: 0x%04X\n",
			prefix, i, WORD(p+3*i+1));
	}
}
*/
/*end stealing from Jean Delvare*/


char *Menu1 = "Hardware";
char *BIOS = "BIOS";
char *DMI = "DMI";
char *PORTS = "Ports";


int dmi_board_counter = 0;/*simple counter to make duplicate dmi_boards unique*/
	
void dmi_table(int fd, u32 base, int len, int num)
{
	char information[]="DMI";
	char board[15]="Board";
	char tmpstr[80];
	char *bufptr = tmpstr;
	unsigned char *buf=malloc(len);
	struct dmi_header *dm;
	u8 *data;
	int i=0;
	struct tweak *tweak=NULL;
	struct private_DMI_data *private;

	if (lseek(fd, (long)base, 0)==-1) {
		log_message("dmi: lseek");
		return; 
	}
	if (read(fd, buf, len)!=len) {
		log_message("dmi: read");
		return; 
	}
	data = buf;
	while (i<num) {
		dm=(struct dmi_header *)data;

		bufptr=tmpstr;

		switch (dm->type) {

			case 0: /*BIOS*/
				handle_table_0x0(data, dm);
				break;

			case 1:  /*System*/
				handle_table_0x1(data, dm);
				break;

			case 2:  /*Base Board / Module  !!!!!!!!there can be more than one of these!!!!!!!!!*/
				if (dmi_board_counter>0)
				{
					snprintf(board, 9, "Board[%i]", dmi_board_counter);
				}
				dmi_board_counter++;
				DMI_STR("Manufacturer", data[4], board, information);
				DMI_STR("Product", data[5], board, information);
				DMI_STR("Version", data[6], board, information);
				DMI_STR("Serial number", data[7], board, information);
				DMI_STR("Asset tag", data[8], board, information);
/*feature flags (data[9])*/
				DMI_STR("Location in Chassis", data[10], board, information);
/*Chassis handle (data[11 & 12])*/
/*Board type (data[13])*/
				dmi_decode_board_type(data[13], bufptr);
				DMI_STR_VALUE("Board Type", bufptr, board, information);
/*number of contained object handles (data[14])*/
				{
					int numberOfObjectHandles = data[14];
					int idx;
					for (idx=0; idx < numberOfObjectHandles; idx++)
					{
/*list of object handles (data[15&16]....)*/
					}
				}
			
				break;

			case 3:  /*Chassis*/
				handle_table_0x3(data, dm);
				break;

			case 4:  /*Processor*/
				handle_table_0x4(data, dm);
				break;

			case 5:  /*Memory Controller*//*Obsoleted by 0x16 from spec 2.1*/
				handle_table_0x5(data, dm);			
				break;

			case 6:  /*Socket*//*Obsoleted by 0x17 from spec 2.1*/
				handle_table_0x6(data, dm);
				break;

			case 7:  /*Cache*/
				handle_table_0x7(data, dm);
				break;

			case 8:  /*Port*/
			{
				char *port;
				port = dmi_string(dm,data[6]);
				if ((port == NULL) || (strlen(port) < 2))
				{/*mmm, my AMI bios has an empty External Ref*/
					port = dmi_string(dm,data[4]);
				}
				_DMI_STR(PORTS, "Internal ref", data[0x4], port, information);
				_DMI_STR_VALUE(PORTS, "Internal connector type", dmi_port_connector_type(data[0x5]), port, information);
				_DMI_STR(PORTS, "External ref", data[0x6], port, information);
				_DMI_STR_VALUE(PORTS, "External connector type", dmi_port_connector_type(data[0x7]), port, information);
				_DMI_STR_VALUE(PORTS, "Port type", dmi_port_type(data[0x8]), port, information);
				break;
			}
			case 9:  /*System Slots*/
				handle_table_0x9(data, dm);
				break;
			case 10:  /*OnBoard devices (untested)*/
			{
				int idx, length;
				char element[] = "Onboard Devices";
				
				length = data[0x1];
/*				char tmp[20];
				snprintf(tmp, 20, "%i", length);
				DMI_STR_VALUE(element, "Number of devices", tmp, information);
*/				
				for (idx=4; idx < length; idx=idx+2)
				{
					char *status = "Disabled";
					char type[30];
					char *device;
					device = dmi_string(dm, data[idx+1]);
					
					if (data[idx]&1<<6) status = "Enabled";

					if ((data[idx]&0x80)==0x1) snprintf(type, 30, "Other (%s)", status);
					if ((data[idx]&0x80)==0x2) snprintf(type, 30, "Unknown (%s)", status);
					if ((data[idx]&0x80)==0x3) snprintf(type, 30, "Video (%s)", status);
					if ((data[idx]&0x80)==0x4) snprintf(type, 30, "SCSI controller (%s)", status);
					if ((data[idx]&0x80)==0x5) snprintf(type, 30, "Ethernet (%s)", status);
					if ((data[idx]&0x80)==0x6) snprintf(type, 30, "Token Ring (%s)", status);
					if ((data[idx]&0x80)==0x7) snprintf(type, 30, "Sound (%s)", status);
					
					DMI_STR_VALUE(element, device, status, information);
					
				}
			}
				break;
			case 11:  /*OEM Data*/
			{
				int idx, length;
				char element[] = "OEM Data";
				char tmp[20];
				
				length = data[0x4];
				snprintf(tmp, 20, "%i", length);
				DMI_STR_VALUE("Number of strings", tmp, element, information);
				
				for (idx=0; idx < length; idx++)
				{
					snprintf(tmp, 20, "string %i", idx);
					DMI_STR(tmp, data[5+idx], element, information);
				}				
			}
			break;
			case 12:	/*System config options*/
			{
				int idx, length;
				char element[] = "System config options";
				char tmp[20];
				
				length = data[0x4];
				snprintf(tmp, 20, "%i", length);
				DMI_STR_VALUE("Number of strings", tmp, element, information);
				
				for (idx=0; idx < length; idx++)
				{
					snprintf(tmp, 20, "string %i", idx);
					DMI_STR(tmp, data[5+idx], element, information);
				}				
			}
				break;
			case 13:	/*BIOS Language*/
			{
				int idx, length;
				char element[] = "BIOS Language";
				char tmp[20];
				
				length = data[0x4];
				snprintf(tmp, 20, "%i", length);
				DMI_STR_VALUE("Number of languages", tmp, element, information);
				
				DMI_STR("Current language", data[0x15], element, information);
				
				for (idx=0; idx < length; idx++)
				{
					snprintf(tmp, 20, "language %i", idx);
					DMI_STR(tmp, data[0x16+idx], element, information);
				}				
			}
				break;
			case 14:	/*Group associations*/
/*			{
				char element[] = "Group Associations";
				
				for (u=0; 3*u+7<dm->length; u++) {
					printf("\t\tGroup Name: %s\n",   dmi_string(dm,data[4]));
					printf("\t\t\tType: 0x%02x\n", 	*(data+5+(u*3)));
					printf("\t\t\tHandle: 0x%04x\n",   *(u16*)(data+6+(u*3)));
				}
			}
*/				break;
			case 15:	/*System Event log*/
			{
				char element[] = "System Event log";
				char tmp[20];

				snprintf(tmp, 20, "%d bytes",	data[5]<<8|data[4]);
				DMI_STR_VALUE("Log Area", tmp, element, information);
				snprintf(tmp, 20, "%d",	data[7]<<8|data[6]);
				DMI_STR_VALUE("Log Header At", tmp, element, information);
				snprintf(tmp, 20, "%d",	data[9]<<8|data[8]);
				DMI_STR_VALUE("Log Data At", tmp, element, information);
/*				snprintf(tmp, 20, "%d",		data[0xa]);
				DMI_STR_VALUE("Access type", tmp, element, information);
*/				
				memset(tmp, 0, 20);
				if(data[0xb]&(1<<0))
					snprintf(tmp, 20, "Log Valid: Yes.\n");
				if(data[0xb]&(1<<1))
					snprintf(tmp, 20, "**Log Is Full**.\n");
				DMI_STR_VALUE("Log Status", tmp, element, information);
				
			}
				break;
			case 16:	/*Physical Memory Array*/
			{
				char element[] = "Physical Memory Array";
				char tmp[20];
				u32 u2;
				u16 u;
				
				DMI_STR_VALUE("Location", dmi_memory_array_location(data[4]), element, information);
				
				DMI_STR_VALUE("use", dmi_memory_array_use(data[5]), element, information);

				DMI_STR_VALUE("Memory Error correction", dmi_memory_array_error_correction_type(data[6]), element, information);

				u2 = data[10]<<24|data[9]<<16|data[8]<<8|data[7];
				if (u2 == 0x80000000)
					snprintf(tmp, 20, "Unknown");
				else
					snprintf(tmp, 20, "%u Kbyte", u2);
				DMI_STR_VALUE("Maximum Capacity", tmp, element, information);

				u = data[12]<<8|data[11];
				if (u == 0xffff) {
					snprintf(tmp, 20, "None");
				} else if (u == 0xfffe) {
					snprintf(tmp, 20, "Not Provided");
				} else {
					snprintf(tmp, 20, "0x%04X\n", u);
				}
				DMI_STR_VALUE("Error Information Handle", tmp, element, information);

				snprintf(tmp, 20, "%u\n", data[14]<<8|data[13]);
				DMI_STR_VALUE("Number of Devices", tmp, element, information);

			}
				break;
			case 17:	/*Memory device*/
			{
				handle_table_0x17(data, dm);
			}
				break;
			case 18:	/*32 bit memory Error info*/
/*TODO				tweak = alloc_DMI_tweak (TYPE_NODE_ONLY);
				RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, "32-bit Memory error info", information);*/
				break;
			case 19:	/*Memory array mapped address*/
			{	
				char element[] = "Memory array mapped address";
				char tmp[20];
				u32 u2;
				u16 u;

				u2 = data[0x7]<<24|data[0x6]<<16|data[0x5]<<8|data[0x4];
				if (u2 == 0x80000000)
					snprintf(tmp, 20, "Unknown");
				else
					snprintf(tmp, 20, "0x%x000", u2);
				DMI_STR_VALUE("Starting Address", tmp, element, information);

				u2 = data[0xB]<<24|data[0xA]<<16|data[0x9]<<8|data[0x8];
				if (u2 == 0x80000000)
					snprintf(tmp, 20, "Unknown");
				else
					snprintf(tmp, 20, "0x%x000", u2);
				DMI_STR_VALUE("Ending Address", tmp, element, information);

				u = data[0xD]<<8|data[0xC];
				if (u2 == 0x80000000)
					snprintf(tmp, 20, "Unknown");
				else
					snprintf(tmp, 20, "0x%x", u2);
				DMI_STR_VALUE("Memory array handle", tmp, element, information);

				snprintf(tmp, 20, "%i", data[0xE]);
				DMI_STR_VALUE("Partition width", tmp, element, information);
			}
				break;
			case 20:	/*Memory device mapped address*/
			{	
				char element[] = "Memory device mapped address";
				char tmp[20];
				u32 u2;
				u16 u;

				u2 = data[0x7]<<24|data[0x6]<<16|data[0x5]<<8|data[0x4];
				if (u2 == 0x80000000)
					snprintf(tmp, 20, "Unknown");
				else
					snprintf(tmp, 20, "0x%x000", u2);
				DMI_STR_VALUE("Starting Address", tmp, element, information);

				u2 = data[0xB]<<24|data[0xA]<<16|data[0x9]<<8|data[0x8];
				if (u2 == 0x80000000)
					snprintf(tmp, 20, "Unknown");
				else
					snprintf(tmp, 20, "0x%x000", u2);
				DMI_STR_VALUE("Ending Address", tmp, element, information);

				u = data[0xD]<<8|data[0xC];
				if (u2 == 0x80000000)
					snprintf(tmp, 20, "Unknown");
				else
					snprintf(tmp, 20, "0x%x", u2);
				DMI_STR_VALUE("Memory array handle", tmp, element, information);

				u = data[0xF]<<8|data[0xE];
				if (u2 == 0x80000000)
					snprintf(tmp, 20, "Unknown");
				else
					snprintf(tmp, 20, "0x%x", u2);
				DMI_STR_VALUE("Memory array mapped address handle", tmp, element, information);

				snprintf(tmp, 20, "%i", data[0x10]);
				DMI_STR_VALUE("Partition Row position", tmp, element, information);

				snprintf(tmp, 20, "%i", data[0x11]);
				DMI_STR_VALUE("Interleave position", tmp, element, information);

				snprintf(tmp, 20, "%i", data[0x12]);
				DMI_STR_VALUE("Interleave data depth", tmp, element, information);
			}
				break;
			case 21:	/*Built-in pointing device*/
			{
				char element[] = "Built-in pointing device";
				char tmp[20];

				static const char *PointingDeviceType[] = {"", "Other", "Unknown", "Mouse", "Track Ball", 
						"Track Point", "Glide Point", "Touch Pad", "Touch Screen", 
						"Optical Sensor" };				
				static const char *PointingDeviceInterface[] = {"", "Other", "Unknown", "Serial", "PS/2", 
						"Infrared", "HP-HIL", "Bus Mouse", "ADB (Apple Desktop Bus)",
						"Bus Mouse (DB-9)", "Bus Mouse micro-DIN", "USB"};

				DMI_STR_VALUE("Type", PointingDeviceType[data[0x4]], element, information);
				DMI_STR_VALUE("Interface", PointingDeviceInterface[data[0x5]], element, information);
				snprintf(tmp, 20, "%i", data[0x6]);
				DMI_STR_VALUE("number of buttons", tmp, element, information);

			}
				break;
			case 22:	/*Portable battery*//*untested*/
			{
				char element[] = "Portable battery";
				char tmp[20];
				u16 u;
				static const char *BatteryDeviceChemistry[] = {"", "Other", "Unknown",
						"Lead Acid", "Nickel Cadmium", "Nickel metal hydride", 
						"Lithium-ion", "Zinc air", "Lithium Polymer"};

				DMI_STR("Location", data[0x4], element, information);
				DMI_STR("Manufacturer", data[0x5], element, information);
				DMI_STR("Manufacture date", data[0x6], element, information);
				DMI_STR("Serial Number", data[0x7], element, information);
				DMI_STR("Device Name", data[0x8], element, information);

				DMI_STR_VALUE("Device Chemistry", BatteryDeviceChemistry[data[0x9]], element, information);
					
				u = data[0xb]<<8|data[0xa];
				if (u != 0)
				{
					u = u * data[0x15];
					snprintf(tmp, 20, "%i mWatt hours", u);
					DMI_STR_VALUE("Design Capacity", tmp, element, information);
				}
				u = data[0xd]<<8|data[0xc];
				if (u != 0)
				{
					snprintf(tmp, 20, "%i mVolts", u);
					DMI_STR_VALUE("Design Voltage", tmp, element, information);
				}
				
				DMI_STR("SBDS Version Number", data[0xe], element, information);
				
				if (data[0xf] != 0xff)
				{
					snprintf(tmp, 20, "%i %%", u);
					DMI_STR_VALUE("Max error in battery data", tmp, element, information);
				}
				/*SBDS serial number*/
				/*SBDS manufacture date*/
				/*SBDS device chemistry*/
			}
				break;
			case 23:	/*System reset*/
			{
				char element[] = "System Reset";
				char tmp[20] = "DMI";
				u16 u;
				
				u = data[0x4];

				DMI_CAP(0,  "System reset enabled", element, tmp);
				/*other capabilities use 2 bits :( ...*/

				u = data[0x6]<<8|data[0x5];
				if (u != 0xffff)
				{
					snprintf(tmp, 20, "%i", u);
					DMI_STR_VALUE("Number of automatic system resets sinse the last intentional reset", tmp, element, information);
				}
				u = data[0x8]<<8|data[0x7];
				if (u != 0xffff)
				{
					snprintf(tmp, 20, "%i", u);
					DMI_STR_VALUE("Automatic system reset limit", tmp, element, information);
				}
				u = data[0xa]<<8|data[0x9];
				if (u != 0xffff)
				{
					snprintf(tmp, 20, "%i", u);
					DMI_STR_VALUE("Watchdog Timer interval", tmp, element, information);
				}
				u = data[0xc]<<8|data[0xb];
				if (u != 0xffff)
				{
					snprintf(tmp, 20, "%i", u);
					DMI_STR_VALUE("Reboot Timeout", tmp, element, information);
				}
			}
				break;
			case 24:	/*Hardware security*/
			{
				char element[] = "Hardware Security";
				
				if(len<0x05) break;
				DMI_STR_VALUE("Power-On Password Status:", dmi_hardware_security_status(data[0x04]>>6), element, information);
				DMI_STR_VALUE("Keyboard Password Status:", dmi_hardware_security_status((data[0x04]>>4)&0x3), element, information);
				DMI_STR_VALUE("Administrator Password Status:", dmi_hardware_security_status((data[0x04]>>2)&0x3), element, information);
				DMI_STR_VALUE("Front Panel Reset Status: ", dmi_hardware_security_status(data[0x04]&0x3), element, information);
			}
				break;
			case 25:	/*System power controls*/
			{
				char element[] = "System Power Controls";
			
				if(len<0x09) break;
				DMI_STR_VALUE("Next Scheduled Power-on:", dmi_power_controls_power_on(data+0x04), element, information);
			}
				break;
			case 26:	/*Voltage probe*/
			{
				char element[] = "Voltage Probe";
				
				if(len<0x14) break;
				DMI_STR("Description:", data[0x04], element, information);
				DMI_STR_VALUE("Location:", dmi_voltage_probe_location(data[0x05]&0x1f), element, information);
				DMI_STR_VALUE("Status:", dmi_probe_status(data[0x05]>>5), element, information);
				DMI_STR_VALUE("Maximum Value:", dmi_voltage_probe_value(data[0x7]<<8|data[0x6]), element, information);
				DMI_STR_VALUE("Minimum Value:", dmi_voltage_probe_value(data[0x9]<<8|data[0x8]), element, information);
				DMI_STR_VALUE("Resolution:", dmi_voltage_probe_resolution(data[0xb]<<8|data[0xa]), element, information);
				DMI_STR_VALUE("Tolerance:", dmi_voltage_probe_value(data[0xd]<<8|data[0xc]), element, information);
				DMI_STR_VALUE("Accuracy:", dmi_probe_accuracy(data[0xf]<<8|data[0xe]), element, information);
/*TODO				DMI_STR_VALUE("OEM-specific Information: 0x%08X\n", (data[0x13]<<24|data[0x12]<<16|data[0x11]<<8|data[0x10])), element, information);*/
				
				if(len<0x16) break;
				DMI_STR_VALUE("Nominal Value:", dmi_voltage_probe_value(data[0x15]<<8|data[0x14]), element, information);
			}
				break;
		case 27: /* 3.3.28 Cooling Device */
		{
			char element[] = "Cooling Device";
			char tmp[10];
			
			if(len<0x0C) break;
			if((data[0x5]<<8|data[0x4])!=0xFFFF)
			{
				snprintf(tmp, 9, "0x%04X", (data[0x5]<<8|data[0x4]));
				DMI_STR_VALUE("Temperature Probe Handle:", tmp, element, information);
			}
			DMI_STR_VALUE("Type:", dmi_cooling_device_type(data[0x06]&0x1f), element, information);
			DMI_STR_VALUE("Status:", dmi_probe_status(data[0x06]>>5), element, information);
			if(data[0x07]!=0x00)
			{
				snprintf(tmp, 9, "%u", data[0x07]);
				DMI_STR_VALUE("Cooling Unit Group:", tmp, element, information);
			}
			snprintf(tmp, 9, "0x%08X\n", DWORD(data+0x08));
			DMI_STR_VALUE("OEM-specific Information:", tmp, element, information);
			if(len<0x0E) break;
			DMI_STR_VALUE("Nominal Speed:", dmi_cooling_device_speed(data[0xd]<<8|data[0xc]), element, information);
		}
			break;
		case 28: /* 3.3.29 Temperature Probe */
		{
			char element[] = "Temperature Probe";
			char tmp[10];
			
			if(len<0x14) break;
			DMI_STR("Description:", data[0x04], element, information);
			DMI_STR_VALUE("Location:", dmi_temperature_probe_location(data[0x05]&0x1F), element, information);
			DMI_STR_VALUE("Status:", dmi_probe_status(data[0x05]>>5), element, information);
			DMI_STR_VALUE("Maximum Value:", dmi_temperature_probe_value(data[0x7]<<8|data[0x6]), element, information);

			DMI_STR_VALUE("Minimum Value", dmi_temperature_probe_value(data[0x9]<<8|data[0x8]), element, information);

			DMI_STR_VALUE("Resolution:", dmi_temperature_probe_resolution(data[0xB]<<8|data[0xA]), element, information);
			DMI_STR_VALUE("Tolerance:", dmi_temperature_probe_value(data[0xc]<<8|data[0xc]), element, information);

			DMI_STR_VALUE("Accuracy:", dmi_probe_accuracy(data[0xf]<<8|data[0xe]), element, information);
			snprintf(tmp, 9, "0x%08X", DWORD(data+0x10));
			DMI_STR_VALUE("OEM-specific Information:", tmp, element, information);
			if(len<0x16) break;
			DMI_STR_VALUE("Nominal Value:", dmi_temperature_probe_value(WORD(data+0x14)), element, information);
		}
			break;
		
		case 29: /* 3.3.30 Electrical Current Probe */
		{
			char element[] = "Electrical Current Probe";

			if(len<0x14) break;
			DMI_STR("Description:", data[0x04], element, information);
			DMI_STR_VALUE("Location:", dmi_voltage_probe_location(data[5]&0x1F), element, information);
			DMI_STR_VALUE("Status:", dmi_probe_status(data[0x05]>>5), element, information);
			DMI_STR_VALUE("Maximum Value:", dmi_current_probe_value(WORD(data+0x06)), element, information);
			DMI_STR_VALUE("Minimum Value:", dmi_current_probe_value(WORD(data+0x08)), element, information);
			DMI_STR_VALUE("Resolution:", dmi_current_probe_resolution(WORD(data+0x0A)), element, information);
			DMI_STR_VALUE("Tolerance:", dmi_current_probe_value(WORD(data+0x0C)), element, information);
			DMI_STR_VALUE("Accuracy:", dmi_probe_accuracy(WORD(data+0x0E)), element, information);
/*			DMI_STR_VALUE("OEM-specific Information: 0x%08X\n", DWORD(data+0x10), element, information);*/
			if(len<0x16) break;
			DMI_STR_VALUE("Nominal Value:", dmi_current_probe_value(WORD(data+0x14)), element, information);
		}
			break;
		
		case 30: /* 3.3.31 Out-of-band Remote Access */
		{
			char element[] = "Out-of-band Remote Access";
			if(len<0x06) break;
			DMI_STR("Manufacturer Name:", data[0x04], element, information);
			DMI_STR_VALUE("Inbound Connection:", data[0x05]&(1<<0)?"Enabled":"Disabled", element, information);
			DMI_STR_VALUE("Outbound Connection:", data[0x05]&(1<<1)?"Enabled":"Disabled", element, information);
		}
			break;
		case 31: /* 3.3.32 Boot Integrity Services Entry Point */
			printf("\tBoot Integrity Services Entry Point\n");
			break;
		case 32:	/*System boot info*/
			{
				char element[] = "System boot info";

				switch (data[0xa]) 
				{
					case 0:
						DMI_STR_VALUE("Status", "No errors detected", element, information);
					break;
					case 1:
						DMI_STR_VALUE("Status", "No bootable material", element, information);
					break;
					case 2:
						DMI_STR_VALUE("Status", "\"normal\" operating system failed to load", element, information);
					break;
					case 3:
						DMI_STR_VALUE("Status", "Firmware detected hardware failure", element, information);
					break;
					case 4:
						DMI_STR_VALUE("Status", "Operating system detected failure", element, information);
					break;
					case 5:
						DMI_STR_VALUE("Status", "User requested boot", element, information);
					break;
					case 6:
						DMI_STR_VALUE("Status", "System security violation", element, information);
					break;
					case 7:
						DMI_STR_VALUE("Status", "Previously requested boot image", element, information);
					break;
					case 8:
						DMI_STR_VALUE("Status", "System watchdog timer expired", element, information);
					break;
				}
			}
				break;
		case 33: /* 3.3.34 64-bit Memory Error Information */
		{
			char element[] = "64-bit Memory Error Information";
			
			if(len<0x1F) break;
			DMI_STR_VALUE("Type:", dmi_memory_error_type(data[0x04]), element, information);
			DMI_STR_VALUE("Granularity:", dmi_memory_error_granularity(data[0x05]), element, information);
			DMI_STR_VALUE("Operation:", dmi_memory_error_operation(data[0x06]), element, information);
			DMI_STR_VALUE("Vendor Syndrome:", dmi_memory_error_syndrome(DWORD(data+0x07)), element, information);
			DMI_STR_VALUE("Memory Array Address:", dmi_64bit_memory_error_address(QWORD(data+0x0B)), element, information);
			DMI_STR_VALUE("Device Address:", dmi_64bit_memory_error_address(QWORD(data+0x13)), element, information);
			DMI_STR_VALUE("Resolution:", dmi_32bit_memory_error_address(DWORD(data+0x1B)), element, information);
		}
			break;
		case 34: /* 3.3.35 Management Device */
		{
			char element[] = "Management Device";
			
			if(len<0x0B) break;
			DMI_STR("Description:", data[0x04], element, information);
			DMI_STR_VALUE("Type:", dmi_management_device_type(data[0x05]), element, information);
/*			DMI_STR_VALUE("Address: 0x%08X\n", DWORD(data+0x06), element, information);*/
			DMI_STR_VALUE("Address Type:", dmi_management_device_address_type(data[0x0A]), element, information);
		}
			break;
		
		case 35: /* 3.3.36 Management Device Component */
		{
			char element[] = "Management Device Component";
			
			if(len<0x0B) break;
			DMI_STR("Description:", data[0x04], element, information);
/*			DMI_STR_VALUE("Management Device Handle: 0x%04X\n", WORD(data+0x05), element, information);
			DMI_STR_VALUE("Component Handle: 0x%04X\n", WORD(data+0x07), element, information);
			if(WORD(data+0x09)!=0xFFFF)
				DMI_STR_VALUE("Threshold Handle: 0x%04X\n", WORD(data+0x09), element, information);
*/		}
			break;
		
		case 36: /* 3.3.37 Management Device Threshold Data */
		{
/*			char element[] = "Management Device Threshold Data";
			
			if(len<0x10) break;
			if(WORD(data+0x04)!=0x8000)
				DMI_STR_VALUE("Lower Non-critical Threshold: %d\n", (i16)WORD(data+0x04), element, information);
			if(WORD(data+0x06)!=0x8000)
				DMI_STR_VALUE("Upper Non-critical Threshold: %d\n", (i16)WORD(data+0x06), element, information);
			if(WORD(data+0x08)!=0x8000)
				DMI_STR_VALUE("Lower Critical Threshold: %d\n",	(i16)WORD(data+0x08), element, information);
			if(WORD(data+0x0A)!=0x8000)
				DMI_STR_VALUE("Upper Critical Threshold: %d\n", (i16)WORD(data+0x0A), element, information);
			if(WORD(data+0x0C)!=0x8000)
				DMI_STR_VALUE("Lower Non-recoverable Threshold: %d\n", (i16)WORD(data+0x0C), element, information);
			if(WORD(data+0x0E)!=0x8000)
				DMI_STR_VALUE("Upper Non-recoverable Threshold: %d\n", (i16)WORD(data+0x0E), element, information);
*/		}
			break;
		
		case 37: /* 3.3.38 Memory Channel */
		{
			char element[] = "Memory Channel";
			
			if(len<0x07) break;
			DMI_STR_VALUE("Type:", dmi_memory_channel_type(data[0x04]), element, information);
/*			DMI_STR_VALUE("Maximal Load: %u\n",	data[0x05], element, information);
			DMI_STR_VALUE("Devices: %u\n", data[0x06], element, information);
			if(len<0x07+3*data[0x06]) break;
			dmi_memory_channel_devices(data[0x06], data+0x07, "\t");
*/		}
			break;
		
		case 38: /* 3.3.39 IPMI Device Information */
			/*
			 * We use the word "Version" instead of "Revision", conforming to
			 * the IPMI 1.5 specification. This specification isn't very clear
			 * regarding the I2C slave address. I couldn't understand wether
			 * or not we are supposed to shift it by one bit to the right, so
			 * I leave it untouched. Beware it might be wrong.
			 */
		{
/*			char element[] = "IPMI Device Information";
			
			if(len<0x10) break;
			DMI_STR("Interface Type:", dmi_ipmi_interface_type(data[0x04]), element, information);
			DMI_STR_VALUE("Specification Version: %u.%u\n", data[0x05]>>4, data[0x05]&0x0F, element, information);
			DMI_STR_VALUE("I2C Slave Address: 0x%02x\n", data[0x06], element, information);
			if(data[0x07]!=0xFF)
				DMI_STR_VALUE("NV Storage Device Address: %u\n", data[0x07], element, information);
			else
				DMI_STR_VALUE("NV Storage Device: Not Present\n");
			if(len<0x12)
			{
				DMI_STR_VALUE("Base Address: 0x%08X%08X (%s)\n",
					QWORD(data+0x08).h, QWORD(data+0x08).l,
					QWORD(data+0x08).l&1?"I/O":"Memory-mapped");
				break;
			}
			DMI_STR_VALUE("Base Address: 0x%08X%08X (%s)\n",
				QWORD(data+0x08).h,
				(QWORD(data+0x08).l&~1)|((data[0x10]>>5)&1),
				QWORD(data+0x08).l&1?"I/O":"Memory-mapped");
			DMI_STR_VALUE("Register Spacing: %s\n",
				dmi_ipmi_register_spacing(data[0x10]>>6));
			if(data[0x10]&(1<<3))
			{
				DMI_STR_VALUE("Interrupt Polarity: %s\n",
					data[0x10]&(1<<1)?"Active High":"Active Low");
				DMI_STR_VALUE("Interrupt Trigger Mode: %s\n",
					data[0x10]&(1<<0)?"Level":"Edge");
			}
			if(data[0x11]!=0x00)
			{
				DMI_STR_VALUE("Interrupt Number: %x\n",
					data[0x11]);
			}
*/		}
			break;
		
		case 39: /* 3.3.40 System Power Supply */
		{
			char element[] = "System Power Supply";
			
			if(len<0x10) break;
/*			if(data[0x04]!=0x00)
				DMI_STR_VALUE("Power Unit Group: %u",	data[0x04], element, information);
*/			DMI_STR("Location:",	data[0x05], element, information);
			DMI_STR("Name:", data[0x06], element, information);
			DMI_STR("Manufacturer:", data[0x07], element, information);
			DMI_STR("Serial Number:", data[0x08], element, information);
			DMI_STR("Asset Tag:", data[0x09], element, information);
			DMI_STR("Model Part Number:", data[0x0A], element, information);
			DMI_STR("Revision:", data[0x0B], element, information);
			DMI_STR_VALUE("Max Power Capacity:", dmi_power_supply_power(WORD(data+0x0C)), element, information);

			DMI_STR_VALUE("Status:", dmi_power_supply_status((WORD(data+0x0E)>>7)&0x07), element, information);

			DMI_STR_VALUE("Type: %s\n",	dmi_power_supply_type((WORD(data+0x0E)>>10)&0x0F), element, information);
			DMI_STR_VALUE("Input Voltage Range Switching: %s\n", dmi_power_supply_range_switching((WORD(data+0x0E)>>3)&0x0F), element, information);
			DMI_STR_VALUE("Plugged: %s\n", WORD(data+0x0E)&(1<<2)?"No":"Yes", element, information);
			DMI_STR_VALUE("Hot Replaceable: %s\n", WORD(data+0x0E)&(1<<0)?"Yes":"No", element, information);
			if(len<0x16) break;
/*			if(WORD(data+0x10)!=0xFFFF)
				DMI_STR_VALUE("Input Voltage Probe Handle: 0x%04X\n", WORD(data+0x10), element, information);
			if(WORD(data+0x12)!=0xFFFF)
				DMI_STR_VALUE("Cooling Device Handle: 0x%04X\n", WORD(data+0x12), element, information);
			if(WORD(data+0x14)!=0xFFFF)
				DMI_STR_VALUE("Input Current Probe Handle: 0x%04X\n", WORD(data+0x14), element, information);
*/		}
			break;
		case 126:	/*Inactive*/
/*				tweak = alloc_DMI_tweak (TYPE_NODE_ONLY);
				RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, "Inactive", information);
*/				break;
		case 127:	/*End of table*/
/*				tweak = alloc_DMI_tweak (TYPE_NODE_ONLY);
				RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, "End of table", information);
*/				break;

		default:
				snprintf (tmpstr, 80, "Unknown DMI node type %d", dm->type);
				tweak = alloc_DMI_tweak (TYPE_NODE_ONLY);
				RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, tmpstr, information);
				break;
		}
		data+=dm->length;
		while(*data || data[1])
			data++;
		data+=2;
		i++;
	}
	free(buf);
}

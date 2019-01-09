#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <powertweak.h>
#include "dmi.h"

/*Cache Information*/

void handle_table_0x7(u8 *data, pt_unused struct dmi_header *dm)
{
	char information[]="Information";
	char cache[20];
	char tmpstr[80];
	char *bufptr = tmpstr;
	struct tweak *tweak=NULL;
	struct private_DMI_data *private;
	u32 u;
	char *types[4]={"Internal", "External", "", ""};
	char *modes[4]={"write-through","write-back","",""};

	u=data[6]<<8|data[5];
	snprintf(cache, 20, "L%d Cache", 1+(u&7));
	
	bufptr = tmpstr;
	bufptr += sprintf (bufptr, "L%d %s%s ",
		1+(u&7), (u&(1<<3))?"socketed ":"",
		types[(u>>5)&3]);
	if(u&(1<<7))
		sprintf (bufptr, "%s", modes[(u>>8)&3]);
	else 
		sprintf (bufptr, "disabled");
	
	DMI_STR_VALUE("Cache configuration", tmpstr, cache, information);

	bufptr=tmpstr;
	memset(tmpstr, 0, 80);
	dmi_cache_size(data[7]|data[8]<<8, bufptr);
	DMI_STR_VALUE("Max Cache size", bufptr, cache, information);

	bufptr=tmpstr;
	memset(tmpstr, 0, 80);
	dmi_cache_size(data[0x9]|data[0xa]<<8, bufptr);
	DMI_STR_VALUE("Installed Cache size", bufptr, cache, information);

	/*0x0B Supported SRAM types*/	
	bufptr=tmpstr;
	memset(tmpstr, 0, 80);
	dmi_decode_cache(data[0xb]|data[0xc]<<8, bufptr);
	DMI_STR_VALUE("Supported Cache types", bufptr, cache, information);

	/*0x0D current SRAM type*/	
	bufptr=tmpstr;
	memset(tmpstr, 0, 80);
	dmi_decode_cache(data[0xd]|data[0xe]<<8, bufptr);
	DMI_STR_VALUE("Cache type", bufptr, cache, information);

	/*0x10 Error correction type*/
	memset(tmpstr, 0, 80);
	if (data[0x10]&(1<<0)) snprintf(tmpstr, 80, "Other");
	if (data[0x10]&(1<<1)) snprintf(tmpstr, 80, "Unknown");
	if (data[0x10]&(1<<2)) snprintf(tmpstr, 80, "None");
	if (data[0x10]&(1<<3)) snprintf(tmpstr, 80, "Parity");
	if (data[0x10]&(1<<4)) snprintf(tmpstr, 80, "single-bit ECC");
	if (data[0x10]&(1<<5)) snprintf(tmpstr, 80, "multi-bit ECC");
	DMI_STR_VALUE("Error correction type", tmpstr, cache, information);
	
	/*0x11 System cache type*/
	memset(tmpstr, 0, 80);
	if (data[0x10]&(1<<0)) snprintf(tmpstr, 80, "Other");
	if (data[0x10]&(1<<1)) snprintf(tmpstr, 80, "Unknown");
	if (data[0x10]&(1<<2)) snprintf(tmpstr, 80, "instruction");
	if (data[0x10]&(1<<3)) snprintf(tmpstr, 80, "data");
	if (data[0x10]&(1<<4)) snprintf(tmpstr, 80, "unknown");
	DMI_STR_VALUE("system cache type", tmpstr, cache, information);

	/*0x12 Associativity*/
	memset(tmpstr, 0, 80);
	if (data[0x10]&(1<<0)) snprintf(tmpstr, 80, "Other");
	if (data[0x10]&(1<<1)) snprintf(tmpstr, 80, "Unknown");
	if (data[0x10]&(1<<2)) snprintf(tmpstr, 80, "Direct mapped");
	if (data[0x10]&(1<<3)) snprintf(tmpstr, 80, "2-way Set associative");
	if (data[0x10]&(1<<4)) snprintf(tmpstr, 80, "4-way Set associative");
	if (data[0x10]&(1<<5)) snprintf(tmpstr, 80, "fully associative");
	if (data[0x10]&(1<<6)) snprintf(tmpstr, 80, "8-way Set associative");
	if (data[0x11]&(1<<0)) snprintf(tmpstr, 80, "16-way Set associative");
	DMI_STR_VALUE("Associativity", tmpstr, cache, information);
}

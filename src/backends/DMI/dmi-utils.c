/* $Id: dmi-utils.c,v 1.4 2003/05/25 05:49:02 svenud Exp $ */
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <assert.h>
#include <string.h>

#include <powertweak.h>
#include "dmi.h"


/*char *dmi_string(struct dmi_header *dm, u8 s)
{
	char *bp = (char *)dm;
	bp+=dm->length;
	while (s>0) {
		bp+=strlen(bp);
		bp++;
		s--;
	}
	return bp;
}
*/
/*lets use Alan's*/
char *dmi_string(struct dmi_header *dm, u8 s)
{
	char *bp=(char *)dm;
	if (!s) return NULL;
	
	bp+=dm->length;
	while(s>1)
	{
		bp+=strlen(bp);
		bp++;
		s--;
	}
	return bp;
}

void dmi_cache_size(u16 n, char *ptr)
{
	if(n&(1<<15))
		sprintf (ptr, "%dK", (n&0x7FFF)*64);
	else
		sprintf (ptr, "%dK", n&0x7FFF);
}


/*multiple bits can be set for the supported types (rather than current type)*/
void dmi_decode_cache(u16 c, char *ptr)
{
	if(c&(1<<0))	sprintf(ptr, "Other ");
	if(c&(1<<1))	sprintf(ptr, "Unknown ");
	if(c&(1<<2))	sprintf(ptr, "Non-burst ");
	if(c&(1<<3))	sprintf(ptr, "Burst ");
	if(c&(1<<4))	sprintf(ptr, "Pipeline burst ");
	if(c&(1<<5))	sprintf(ptr, "Synchronous ");
	if(c&(1<<6))	sprintf(ptr, "Asynchronous ");
}


/*this may be wrong - maybe get multiple bits set (my ibmT21 seems to have 0x05)*/
void dmi_decode_ram(u8 data, char *ptr)
{
	if(data&(1<<0))		sprintf(ptr, "OTHER ");
	if(data&(1<<1))		sprintf(ptr, "UNKNOWN ");
	if(data&(1<<2))		sprintf(ptr, "STANDARD ");
	if(data&(1<<3))		sprintf(ptr, "FPM ");
	if(data&(1<<4))		sprintf(ptr, "EDO ");
	if(data&(1<<5))		sprintf(ptr, "PARITY ");
	if(data&(1<<6))		sprintf(ptr, "ECC ");
	if(data&(1<<7))		sprintf(ptr, "SIMM ");
	if(data&(1<<8))		sprintf(ptr, "DIMM ");
	if(data&(1<<9))		sprintf(ptr, "Burst EDO ");
	if(data&(1<<10))	sprintf(ptr, "SDRAM ");
}

void dmi_decode_board_type(u8 c, char *ptr)
{
	switch (c)
	{
	case 1:		sprintf(ptr, "Unknown "); break;
	case 2:		sprintf(ptr, "other "); break;
	case 3:		sprintf(ptr, "Server Blade "); break;
	case 4:		sprintf(ptr, "Connectivity Switch "); break;
	case 5:		sprintf(ptr, "System Management Module "); break;
	case 6:		sprintf(ptr, "Processor Module "); break;
	case 7:		sprintf(ptr, "I/O Module "); break;
	case 8:		sprintf(ptr, "Memory Module "); break;
	case 9:		sprintf(ptr, "Daughter board "); break;
	case 10:	sprintf(ptr, "Motherboard (inclueds processor, memory and I/O) "); break;
	case 11:	sprintf(ptr, "Processor/Memory Module "); break;
	case 12:	sprintf(ptr, "Processor/IO Module "); break;
	case 13:	sprintf(ptr, "Interconnect Board "); break;
	default:	ptr[0] = 0;
	}
}

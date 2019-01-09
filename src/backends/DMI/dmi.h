/*
 *  $Id: dmi.h,v 1.12 2003/07/08 11:42:12 svenud Exp $
 *  This file is part of Powertweak Linux.
 *  (C) 2001 Dave Jones, Arjan van de Ven.
 *
 * Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <powertweak.h>

extern char *Menu1;
extern char *BIOS;
extern char *DMI;
extern char *PORTS;


struct dmi_header
{
	u8  type;
	u8  length; 
	u16 handle; 
};

struct private_DMI_data {
	value_t	value;
};

#define DMI_STR(str, x, tree, tab) \
	if (x) {\
		tweak = alloc_DMI_tweak (TYPE_INFO_STRING);\
		tweak->WidgetText = strdup (str);\
		private = (struct private_DMI_data*) tweak->PrivateData;\
		private->value.strVal = strdup(dmi_string(dm, x));\
		RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, tree, tab);\
	}

#define DMI_STR_VALUE(str, sx, tree, tab) \
	if (sx) {\
		tweak = alloc_DMI_tweak (TYPE_INFO_STRING);\
		tweak->WidgetText = strdup (str);\
		private = (struct private_DMI_data*) tweak->PrivateData;\
		private->value.strVal = strdup(sx);\
		RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, tree, tab);\
	}	

	
#define DMI_CAP(bit, str, tree, tab)\
	tweak = alloc_DMI_tweak (TYPE_INFO_BOOL);\
	tweak->WidgetText = strdup (str);\
	private = (struct private_DMI_data*) tweak->PrivateData;\
	if (u&(1<<bit))\
		set_value_int (private->value, 1);\
	else\
		set_value_int (private->value, 0);\
	sprintf(tmpstr, "    %s", str);\
	RegisterTweak (tweak, "mmmmt", Menu1, BIOS, DMI, tree, tab);

#define _DMI_STR(section, str, x, tree, tab) \
	if (x) {\
		tweak = alloc_DMI_tweak (TYPE_INFO_STRING);\
		tweak->WidgetText = strdup (str);\
		private = (struct private_DMI_data*) tweak->PrivateData;\
		private->value.strVal = strdup(dmi_string(dm, x));\
		RegisterTweak (tweak, "mmmt", Menu1, section, tree, tab);\
	}

#define _DMI_STR_VALUE(section, str, sx, tree, tab) \
	if (sx) {\
		tweak = alloc_DMI_tweak (TYPE_INFO_STRING);\
		tweak->WidgetText = strdup (str);\
		private = (struct private_DMI_data*) tweak->PrivateData;\
		private->value.strVal = strdup(sx);\
		RegisterTweak (tweak, "mmmt", Menu1, section, tree, tab);\
	}	
	
#define _CAP(section, bit, str, tree, tab)\
	tweak = alloc_DMI_tweak (TYPE_INFO_BOOL);\
	tweak->WidgetText = strdup (str);\
	private = (struct private_DMI_data*) tweak->PrivateData;\
	if (u&(1<<bit))\
		set_value_int (private->value, 1);\
	else\
		set_value_int (private->value, 0);\
	sprintf(tmpstr, "    %s", str);\
	RegisterTweak (tweak, "mmmt", Menu1, section, tree, tab);
		
		
struct tweak *alloc_DMI_tweak (int type);
void dmi_table(int fd, u32 base, int len, int num);
char *dmi_string(struct dmi_header *dm, u8 s);
void dmi_cache_size(u16 n, char *ptr);
void dmi_decode_cache(u16 c, char *ptr);
void dmi_decode_ram(u8 data, char *ptr);
void dmi_decode_board_type(u8 c, char *ptr);

void handle_table_0x0(u8 *data, struct dmi_header *dm);
void handle_table_0x1(u8 *data, struct dmi_header *dm);
void handle_table_0x3(u8 *data, struct dmi_header *dm);
void handle_table_0x4(u8 *data, struct dmi_header *dm);
void handle_table_0x5(u8 *data, struct dmi_header *dm);
void handle_table_0x6(u8 *data, struct dmi_header *dm);
void handle_table_0x7(u8 *data, struct dmi_header *dm);
void handle_table_0x9(u8 *data, struct dmi_header *dm);
void handle_table_0x17(u8 *data, struct dmi_header *dm);

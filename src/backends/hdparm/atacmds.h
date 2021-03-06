/*
 * atacmds.h
 *
 * Copyright (C) 1999-2000 Michael Cornwell <cornwell@acm.org>
 * Copyright (C) 2000 Andre Hedrick <andre@linux-ide.org>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2, or (at your option)
 * any later version.
 *
 * You should have received a copy of the GNU General Public License
 * (for example COPYING); if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */


#ifndef _ATACMDS_H_
#define _ATACMDS_H_

#include <sys/ioctl.h>
#include <linux/hdreg.h>
#include <sys/fcntl.h>
#include <sys/types.h>




/* These defines SHOULD BE in the kernel
   if not we define them */

#ifndef WIN_SMART
#define WIN_SMART		0xb0	
#endif

#ifndef SMART_READ_VALUES
#define SMART_READ_VALUES	0xd0
#endif

#ifndef SMART_READ_THRESHOLDS	
#define SMART_READ_THRESHOLDS	0xd1
#endif

#ifndef  SMART_AUTOSAVE 
#define SMART_AUTOSAVE		0xd2
#endif

#ifndef SMART_SAVE
#define SMART_SAVE		0xd3
#endif

#ifndef SMART_IMMEDIATE_OFFLINE	
#define SMART_IMMEDIATE_OFFLINE	0xd4
#endif

#ifndef SMART_READ_LOG_SECTOR
#define SMART_READ_LOG_SECTOR 0xd5
#endif

#ifndef SMART_WRITE_LOG_SECTOR
#define SMART_WRITE_LOG_SECTOR 0xd6
#endif

#ifndef SMART_ENABLE
#define SMART_ENABLE		0xd8
#endif

#ifndef SMART_DISABLE
#define SMART_DISABLE		0xd9
#endif

#ifndef SMART_STATUS
#define SMART_STATUS		0xda
#endif

#ifndef SMART_AUTO_OFFLINE
#define SMART_AUTO_OFFLINE	0xdb
#endif

#define OFFLINE_FULL_SCAN			0
#define SHORT_SELF_TEST				1
#define EXTEND_SELF_TEST			2
#define SHORT_CAPTIVE_SELF_TEST		129
#define EXTEND_CAPTIVE_SELF_TEST	130

#define NUMBER_ATA_SMART_ATTRIBUTES 30

/* Smart Values Data Structures */

/* ata_smart_attribute is the vendor 
   specific in SFF-8035 spec */ 
struct ata_smart_attribute {
	unsigned char id;
	unsigned short status_flag;
	/* note char array used since linux can 
       not handle sizeof short correctly */
	unsigned char normalized;
	unsigned char worse_normal;
	unsigned char raw[6];
	unsigned char reserv;
} __attribute__ ((packed));



/* ata_smart_values is format of the 
   read drive Atrribute command */

struct ata_smart_values {
	unsigned short int        revnumber;
	struct ata_smart_attribute    vendor_attributes [NUMBER_ATA_SMART_ATTRIBUTES];
	unsigned char             offline_data_collection_status;
	unsigned char             self_test_exec_status;
	unsigned short int        total_time_to_complete_off_line;
	unsigned char             vendor_specific_366;
	unsigned char             offline_data_collection_capability;
	unsigned short int        smart_capability;
	unsigned char             errorlog_capability;
	unsigned char             vendor_specific_371;
	unsigned char             short_test_completion_time;
	unsigned char             extend_test_completion_time;
	unsigned char			  reserved_374_385 [12];
	unsigned char             vendor_specific_386_509 [125];
	unsigned char             chksum;
} __attribute__ ((packed));





/* Smart Threshold data structures */

/* Vendor attribute of SMART Threshold */
struct ata_smart_threshold_entry {
	unsigned char id;
	unsigned char normalized_threshold;
	unsigned char reserved[10];
} __attribute__ ((packed));


/* Format of Read SMART THreshold Command */

struct ata_smart_thresholds {
	unsigned short int revnumber;
	struct ata_smart_threshold_entry thres_entries[30];
	unsigned char reserved[149];
	unsigned char chksum;
} __attribute__ ((packed));



struct ata_smart_errorlog_command_struct {
	unsigned char devicecontrolreg;
	unsigned char featuresreg;
	unsigned char sector_count;
	unsigned char sector_number;
	unsigned char cylinder_low;
	unsigned char cylinder_high;
	unsigned char drive_head;
	unsigned char commandreg;
	unsigned int timestamp; /*Really and int but Linux Size of is broken*/
} __attribute__ ((packed));

struct ata_smart_errorlog_error_struct {
	unsigned char error_condition;
	unsigned char extended_error[14];
	unsigned char state;
	unsigned short timestamp; /* Fucked up linux sizeof */
} __attribute__ ((packed));

struct ata_smart_errorlog_struct {
	struct ata_smart_errorlog_command_struct commands[6];
	struct ata_smart_errorlog_error_struct error_struct;
}  __attribute__ ((packed));



struct ata_smart_errorlog {
	unsigned char revnumber;
	unsigned char error_log_pointer;
	struct ata_smart_errorlog_struct errorlog_struct[5];
	unsigned short int ata_error_count;
	unsigned short int non_fatal_count;
	unsigned short int drive_timeout_count;
	unsigned char reserved[53];
} __attribute__ ((packed));


struct ata_smart_selftestlog_struct {
	unsigned char selftestnumber;
	unsigned char selfteststatus;
	unsigned short int timestamp;
	unsigned char selftestfailurecheckpoint;
	unsigned int lbafirstfailure;
	unsigned char vendorspecific[15];
} __attribute__ ((packed));


struct ata_smart_selftestlog {
	unsigned short int revnumber;
	struct ata_smart_selftestlog_struct selftest_struct[21];
	unsigned char vendorspecific[2];
	unsigned char mostrecenttest;
	unsigned char resevered[2];
	unsigned char chksum;
} __attribute__ ((packed));

 


/* Read S.M.A.R.T information from drive */


int ataReadHDIdentity (int device, struct hd_driveid *buf);

int ataReadSmartValues (int device,struct ata_smart_values *);

int ataReadSmartThresholds (int device, struct ata_smart_thresholds *);

int ataReadErrorLog ( int device, struct ata_smart_errorlog *);

int ataReadSelfTestLog (int device, struct ata_smart_selftestlog *);

int ataSmartStatus ( int device);

int ataSetSmartThresholds ( int device, struct ata_smart_thresholds *);


/* Enable/Disable SMART on device */

int ataEnableSmart ( int device );

int ataDisableSmart (int device );

/* Automatic Offline Testing */

int ataEnableAutoOffline ( int device );

int ataDisableAutoOffline (int device );


/* S.M.A.R.T. test commands */

int ataSmartOfflineTest (int device);

int ataSmartExtendSelfTest (int device);

int ataSmartShortSelfTest (int device);

int ataSmartShortCapSelfTest (int device);

int ataSmartExtendCapSelfTest (int device);

int ataSmartSelfTestAbort (int device);

/*Check Parameters of Smart Data */



/* int ataVersionInfo ( struct hd_driveid drive) 
*  Returns the latest compability of ATA/ATAPI Version  
*  the device supports								
* Returns -1 if Version command is not supported
*/
 
int ataVersionInfo ( struct hd_driveid drive);



/*  int ataSmartSupport ( int device, struct hd_driveid drive)
*   Check if S.M.A.R.T. is supported and enabled in drive 
*	returns -1:if S.M.A.R.T. cabability can not be checked
*	returns	0: if drive does not support S.M.A.R.T.
*			1: if drive supports S.M.A.R.T. but not enabled 
*			2: if drive supports S.M.A.R.T. and enabled 
*		  255: if drive supports S.M.A.R.T. but does not   
*			   support ATA-4. 
*	ATA 3 and lower do not support S.M.A.R.T. enabled bit
*   Attempt a Read S.M.A.R.T. attributes to check if enabled
*/ 

int ataSmartSupport ( struct hd_driveid drive);

/* Check SMART for Threshold failure */

int ataCheckSmart ( struct ata_smart_values data, struct ata_smart_thresholds thresholds);

/* int isOfflineTestTime ( struct ata_smart_values data)
*  returns S.M.A.R.T. Offline Test Time in seconds
*/

int isOfflineTestTime ( struct ata_smart_values data);

int isShortSelfTestTime ( struct ata_smart_values data);

int isExtendedSelfTestTime ( struct ata_smart_values data);




int isSmartErrorLogCapable ( struct ata_smart_values data);


int isSupportExecuteOfflineImmediate ( struct ata_smart_values data);


int isSupportAutomaticTimer ( struct ata_smart_values data);


int isSupportOfflineAbort ( struct ata_smart_values data);


int isSupportOfflineSurfaceScan ( struct ata_smart_values data);


int isSupportSelfTest (struct ata_smart_values data);




#endif /* _ATACMDS_H_ */

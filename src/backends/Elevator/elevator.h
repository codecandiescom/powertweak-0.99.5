/*
 *  $Id: elevator.h,v 1.2 2001/08/25 19:25:41 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */


/*
 * struct private_Elevator_data contains everything
 * needed to get/set Elevator tweaks from the GUI
 */

#define ELV_READ	1
#define ELV_WRITE	2
#define ELV_BOMB	3

struct private_Elevator_data {
	char *DeviceName;
	int type;
	unsigned int value;
};

/* this has to match with the kernel structure */
/* current version for ac19 and 2.2.16 */
typedef struct blkelv_ioctl_arg_s {
	int queue_ID;
	int read_latency;
	int write_latency;
	int max_bomb_segments;
} blkelv_ioctl_arg_t;

#define BLKELVGET   _IOR(0x12,106,sizeof(blkelv_ioctl_arg_t))
#define BLKELVSET   _IOW(0x12,107,sizeof(blkelv_ioctl_arg_t))

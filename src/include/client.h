/*
 *  $Id: client.h,v 1.3 2003/04/14 13:19:03 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#ifndef _INCLUDE_GUARD_CLIENT_H_
#define _INCLUDE_GUARD_CLIENT_H

#include <sys/socket.h>
#include <sys/un.h>

#include <client-utils.h>

/* fd countains the fd of the socket */
struct private_client_data {
	int	fd;
};

int create_client (char *socketname, struct sockaddr_un *address);
void Send_goodbye(void);
void Save_config(void);

value_t client_get_value (struct tweak *tweak);
int client_get_value_raw (struct tweak *tweak);
void client_change_value (struct tweak *tweak, value_t value, int immediate);
void client_tweak_destructor (struct tweak *tweak);
int InitClientBackend(struct tweak **tweak);
void CloseClientBackend (void);
#endif

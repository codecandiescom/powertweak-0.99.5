/*
 *  $Id: server.c,v 1.12 2003/04/14 13:19:02 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#include "utils.h"
#include "powertweak.h"


int create_server (char *socketname, struct sockaddr_un *address)
{
	int sock;
	int len;

	if ((sock = socket (PF_UNIX, SOCK_STREAM, 0)) < 0) {
		log_message ("socket\n");
		return (0);
	}

	address->sun_family = AF_UNIX;	/* Unix domain socket */
	len = sizeof (address->sun_path);
	strncpy (address->sun_path, socketname, len);

	if (bind (sock, (struct sockaddr *) address, len)) {
		log_message ("bind\n");
		return (0);
	}

	if (listen (sock, 5)) {
		log_message ("listen\n");
		return (0);
	}
	return (sock);
}

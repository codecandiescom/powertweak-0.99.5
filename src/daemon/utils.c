
/*
 *  $Id: utils.c,v 1.9 2003/04/22 12:43:13 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <sys/socket.h>
#include <sys/un.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include <powertweak.h>

#include "utils.h"

static inline void send_message_raw (int dsocket, char *message)
{
	write (dsocket, message, strlen(message));
}

/* Copies data from file descriptor 'from' to file descriptor 'to'
   until nothing is left to be copied. Exits if an error occurs.   */
void copyData (int from, int to)
{
	char buf[1024];
	int amount;

	while ((amount = read (from, buf, sizeof (buf))) > 0) {
		if (write (to, buf, amount) != amount) {
			log_message ("write");
			exit (1);
		}
	}
	if (amount < 0) {
		log_message ("read");
		exit (1);
	}
}


int wait_on_socket(int sock, struct sockaddr_un *address, int namelen)
{
	int conn;
	socklen_t addrLength;

	addrLength = sizeof (address->sun_family) + namelen;

	conn = accept (sock, (struct sockaddr *) address, &addrLength);
	return (conn);
}


int authenticate (int conn)
{
	struct ucred cr;
	socklen_t cl = sizeof (cr);

	if (getsockopt (conn, SOL_SOCKET, SO_PEERCRED, &cr, &cl) != 0) {
		printf ("Unable to establish authentication\n");
		return (ERROR_CANT_AUTHENTICATE);
	}

	if (cr.uid != 0) {
		close (conn);
		return (ERROR_NOT_ROOT);
	} else {
		return (AUTHENTICATED_OK);
	}
}

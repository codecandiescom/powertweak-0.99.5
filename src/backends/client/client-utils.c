/*
 *  $Id: client-utils.c,v 1.3 2001/05/28 02:41:00 davej Exp $
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
#include <errno.h>

#include <client-utils.h>

void send_message_raw (int csocket, char *message)
{
	int retval,len;
	len=strlen(message);
	while (len>0) { 
		retval = write (csocket, message, strlen(message));	
		if (retval>0) {
			len-=retval;
			message+=retval;
		} else {
			break; /* Oh shit */
		}
	}
}

/* Copies data from file descriptor 'from' to file descriptor 'to'
   until nothing is left to be copied. Exits if an error occurs.   */
void copyData (int from, int to)
{
	char buf[1024];
	int amount;

	while ((amount = read (from, buf, sizeof (buf))) > 0) {
		if (write (to, buf, amount) != amount) {
			perror ("write");
			exit (EXIT_FAILURE);
		}
	}
	if (amount < 0) {
		perror ("read");
		exit (EXIT_FAILURE);
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

	if (getsockopt (conn, SOL_SOCKET, SO_PEERCRED, &cr, &cl) != 0)
		return (ERROR_CANT_AUTHENTICATE);

	if (cr.uid != 0) {
		if (close (conn)==-EBADF) {
			printf("Oops, you did something we didn't think of\n");
		}
		return (ERROR_NOT_ROOT);
	} else {
		return (AUTHENTICATED_OK);
	}
}

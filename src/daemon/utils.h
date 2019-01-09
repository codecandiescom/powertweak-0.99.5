/*
 *  $Id: utils.h,v 1.5 2000/08/20 15:20:02 fenrus Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#define AUTHENTICATED_OK 0
#define ERROR_CANT_AUTHENTICATE -1
#define ERROR_NOT_ROOT -2

int wait_on_socket(int sock, struct sockaddr_un *address, int namelen);
int authenticate (int conn);

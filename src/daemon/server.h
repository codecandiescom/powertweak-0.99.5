/*
 *  $Id: server.h,v 1.4 2001/02/21 23:10:29 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

int create_server (char *socketname, struct sockaddr_un *address);

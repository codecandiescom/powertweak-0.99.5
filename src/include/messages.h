/*
 *  $Id: messages.h,v 1.3 2003/04/14 13:19:04 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */


#ifndef _INCLUDE_GUARD_MESSAGES_H_
#define _INCLUDE_GUARD_MESSAGES_H_

void send_blob(int fd, void *data, int size);
void send_tweak(int fd, struct tweak *tweak);
void send_message(int fd, int message);
void send_value(int fd, value_t value);
void send_int(int fd, int value);
void send_string(int fd, char *string);

struct tweak 	*receive_tweak(int fd);
int 		receive_message(int fd);
value_t 	receive_value(int fd);
int 		receive_int(int fd);
char 		*receive_string(int fd);



#define message_none		0
#define message_hello		1
#define message_bye		2
#define message_saveconfig	3
#define message_ping		4
#define message_pong		5
/* update is "pong and refresh" */
#define message_update		6

/* Messages for sending/receiving the entire tweak tree */
#define message_requesttweaks	50
#define message_tweakresponse	51
#define	message_tweaksub	52
#define message_tweaknext	53
#define message_tweakreturn	54

/* Messages for methods */
#define message_setvalue	100
#define message_getvalue	101
#define message_getvalueraw 102

#endif

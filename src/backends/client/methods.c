/*
 *  $Id: methods.c,v 1.5 2003/04/14 13:19:01 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 *	This file contains the methods for daemon-side objects.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

#include <powertweak.h>
#include "client.h"
#include "messages.h"


/* This is the GetValue method in struct tweak. */
value_t client_get_value (struct tweak *tweak)
{
	struct private_client_data *pvt;
	value_t val = {0,NULL};

	assert(tweak != NULL);
	
	pvt = (struct private_client_data *) tweak->PrivateData;

	assert(pvt != NULL);
	
	if (tweak->ConfigName==NULL)
		return val;
		
	if (strlen(tweak->ConfigName)==0)
		return val;
		
	send_message(pvt->fd,message_getvalue);
	send_string(pvt->fd,tweak->ConfigName);
		
	val = receive_value(pvt->fd);
	
	return val;
}

/* This is the GetValueRaw method in struct tweak. */
int client_get_value_raw (struct tweak *tweak)
{
	struct private_client_data *pvt;
	value_t val = {0,NULL};

	assert(tweak != NULL);
	
	pvt = (struct private_client_data *) tweak->PrivateData;

	assert(pvt != NULL);
	
	if (tweak->ConfigName==NULL)
		return 0;
		
	if (strlen(tweak->ConfigName)==0)
		return 0;
		
	send_message(pvt->fd,message_getvalueraw);
	send_string(pvt->fd,tweak->ConfigName);
		
	val = receive_value(pvt->fd);
	
	return 1;
}

/* This is the ChangeValue method in struct tweak */
void client_change_value (struct tweak *tweak, value_t value, int immediate)
{
	struct private_client_data *pvt;

	if (tweak == NULL)
		return;

	pvt = (struct private_client_data *) tweak->PrivateData;

	if (pvt == NULL)
		return;

	if (tweak->ConfigName==NULL)
		return;
		
	if (strlen(tweak->ConfigName)==0)
		return;
		
	send_message(pvt->fd,message_setvalue);
	send_string(pvt->fd,tweak->ConfigName);
	send_value(pvt->fd,value);
	send_int(pvt->fd,immediate);		
	
}

void client_tweak_destructor (struct tweak *tweak)
{
	struct private_client_data *private;

	default_destructor(tweak);

	private = tweak->PrivateData;
	if (private==NULL)
		return;

	free(private);
}

/*
 *  $Id: client.c,v 1.12 2003/04/14 13:19:01 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <assert.h>
#include <powertweak.h>
#include <errno.h>

#include "client-utils.h"
#include "messages.h"
#include "client.h"

extern struct tweak *tweaks; /* shoot me */

static int socketfd=0;


int create_client (char *socketname, struct sockaddr_un *address)
{
	int sock;
	int len;

	if ((sock = socket (PF_UNIX, SOCK_STREAM, 0)) < 0)
		return (0);

	address->sun_family = AF_UNIX;
	len = sizeof(address->sun_path);
	strncpy (address->sun_path, socketname, len); 

	if (connect (sock, (struct sockaddr *) address, len)!=0)
		return (0);

	return (sock);
}


struct tweak *magic_tweak_code(int fd)
{
	struct tweak *tweak;
	struct private_client_data *private;
	int message;

	tweak = receive_tweak(fd);
	if (tweak==NULL) 
		return NULL;
		
	tweak->GetValue = &client_get_value;
	tweak->GetValueRaw = &client_get_value_raw;
	tweak->ChangeValue = &client_change_value;
	tweak->Destroy = &client_tweak_destructor;
	
	private = malloc(sizeof(struct private_client_data));
	if (private==NULL) {
		tweak->Destroy(tweak);
		free(tweak);
		return NULL;
	}
	private->fd = fd;
	tweak->PrivateData=(void*)private;
		
	message = receive_message(fd);
	if (message==message_tweaksub)
		tweak->Sub = magic_tweak_code(fd);
	if (message==message_tweaknext)
		tweak->Next = magic_tweak_code(fd);
	if (message==message_tweakreturn)
		return tweak;
		
	message = receive_message(fd);
	if (message==message_tweaknext)
		tweak->Next = magic_tweak_code(fd);
	if (message==message_tweakreturn)
		return tweak;
		
	message = receive_message(fd);
	if (message==message_tweakreturn)
		return tweak;
		
	return tweak;	
}

void ReceiveTweakStructure(int fd)
{
	if (tweaks != NULL) {
		printf("Arjan made a big mistake. \n");
		printf("He knew he shouldn't do this, and now it bites.\n");
		exit(0);
	}
	
	send_message(fd,message_requesttweaks);
	
	tweaks = magic_tweak_code(fd);
	
}

void Send_goodbye(void)
{
 	if (socketfd!=0)
 		send_message(socketfd,message_bye);

}

void Save_config(void)
{
 	if (socketfd!=0)
 		send_message(socketfd,message_saveconfig);
}

/*
 * InitClientBackend
 * This is _the_ function a GUI calls for initialisation of libtweak.
 * It in turn initializes all backends and then returns the pointer to the
 * entire (struct tweak*) tree.
 */
int InitClientBackend(struct tweak  **tweak)
{
	int csocket,err;
	struct sockaddr_un address;

	assert (tweaks == NULL);

	*tweak = NULL;

	/* Create a socket for us to talk to the daemon. */
	csocket = create_client ("/var/run/powertweakd", &address); 
	if (csocket == 0)
	{
		printf ("Error creating socket to : %s\t%s\n", "/var/run/powertweakd", strerror(errno));
		return ERROR_CANT_AUTHENTICATE;
	}

	err = authenticate (csocket);

	switch (err) {
		case ERROR_NOT_ROOT:
			printf("You need to be root to do this.\n");
			break;

		case ERROR_CANT_AUTHENTICATE:
			printf ("Couldn't authenticate.\n");
			break;
	}
	if (err != AUTHENTICATED_OK) {
		close (csocket);
		return err;
	}


	send_message(csocket,message_hello);

	ReceiveTweakStructure(csocket);	

	socketfd = csocket;

	*tweak = tweaks;
	
	return AUTHENTICATED_OK;
}


void CloseClientBackend (void)
{
	Send_goodbye();
	DestroyAllTweaks (tweaks);
}

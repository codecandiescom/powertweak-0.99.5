/*
 *  $Id: socketlib.c,v 1.1 2001/11/02 22:01:05 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <powertweak.h>
#include <messages.h>

int HaveError;

void send_blob(int fd, void* data, int size)
{
	int result;
	
	if (HaveError!=0)
		return;
	
	
	result = write(fd,&size,sizeof(size));
	if (result<0) {
		HaveError = 1;
		return;
	}
	result = write(fd,data,size);
	if (result<0) 
		HaveError = 1;
	
}

void send_int(int fd, int value)
{
	if (HaveError!=0)
		return;
	send_blob(fd,&value,sizeof(value));	
}

void send_string(int fd, char *string)
{
	int nul=0,retval;
	if (HaveError!=0)
		return;
	if (string==NULL) {
		retval=write(fd,&nul,sizeof(nul));
		if (retval<0)
			HaveError=1;
	} else
	{
		send_blob(fd,string,strlen(string)+1);
	}	
}

void send_message(int fd, int message) 
{
	if (HaveError!=0)
		return;
	send_blob(fd,&message,sizeof(message));
}

void send_value(int fd, value_t value)
{
	int result;
	int size;
	if (HaveError!=0)
		return;
	
	size=sizeof(int);
	if (value.strVal!=NULL)
		size += strlen(value.strVal);
	
	result = write(fd,&size,sizeof(size));
	if (result<0) {
		HaveError =1;
		return;
	}
	result = write(fd,&value.intVal,sizeof(value.intVal));
	if (result<0) {
		HaveError =1;
		return;
	}
	if ((value.strVal!=NULL)&&(strlen(value.strVal)>0))
		result = write(fd,value.strVal,strlen(value.strVal));
	if (result<0) 
		HaveError =1;
	
}

void send_tweak(int fd, struct tweak *tweak)
{	
	/* This is cheating!!!! */
	if (HaveError!=0)
		return;
	
	send_string(fd, tweak->WidgetText);
	send_string(fd, tweak->Description);
	send_string(fd, tweak->ConfigName);
	send_int(fd, tweak->Type);
	send_int(fd, tweak->MinValue);
	send_int(fd, tweak->MaxValue);
}

static void receive_blob(int fd, void ** data, int *size)
{
	int result;

	if (HaveError!=0) {
		*size = 0;
		*data = NULL;
		return;
	}

	result = read(fd,size,sizeof(*size));
	if (result<0) {
		HaveError = 1;
		*size = 0;
		*data = NULL;
		return;
	}


	if (*size==0) {
		*data = NULL;
		return;
	}

	*data = malloc(*size);
	if (*data==NULL) {
		*size = 0;
		HaveError = 1;
	}

	result = read(fd,*data,*size);
	if (result<0) {
		HaveError = 1;
		*size = 0;
		if (*data!=NULL)
			free(*data);
		*data = NULL;
	}
}

int receive_int(int fd)
{
	int *val;
	int size;

	if (HaveError!=0) {
		return 0;
	}
	receive_blob(fd,(void*)&val,&size);
	if (size!=sizeof(int)) {
		HaveError=1;
		return 0;
	}
	if (val==NULL) {
		HaveError=1;
		return 0;
	}
	size = *val;
	free(val);
	return size;
}

char *receive_string(int fd)
{
	char *data,*data2;
	int len;

	if (HaveError!=0) {
		return NULL;
	}

	receive_blob(fd,(void*)&data,&len);
	if (len==0) 
		return NULL;
	
	data2=malloc(len+1);
	if (data2==NULL) {
		HaveError = 1;
		return NULL;
	}
	memset(data2,0,len+1);
	memcpy(data2,data,len);
	free(data);
	
	return data2;	
}

int receive_message(int fd) 
{
	return receive_int(fd);
}

value_t receive_value(int fd)
{
	value_t value = {0,NULL};
	int result;
	unsigned int size;
	

	if (HaveError!=0) {
		return value;
	}
	
	result = read(fd,&size,sizeof(size));
	if (result<0) {
		HaveError = 1;
		return value;
	}
	result = read(fd,&value.intVal,sizeof(value.intVal));
	if (result<0) {
		HaveError = 1;
		return value;
	}
	if (size>sizeof(int)) {
		/* We received a string. */
		value.strVal=malloc(1+size-sizeof(int));
		if (value.strVal == NULL)
			return value;
		memset(value.strVal,0,1+size-sizeof(int));
		result = read(fd,value.strVal,size-sizeof(int));
		if (result<0) {
			HaveError = 1;
			return value;
		}
	} else {
		/* We received an integer. */
		value.strVal = NULL;
	}
	return value;
}

struct tweak *receive_tweak(int fd)
{	
	struct tweak *tweak;
	
	/* This is cheating!!!! */

	if (HaveError!=0) {
		return NULL;
	}
	
	tweak = alloc_tweak(TYPE_NODE_ONLY);
	
	tweak->WidgetText=receive_string(fd);
	tweak->Description=receive_string(fd);
	tweak->ConfigName=receive_string(fd);
	tweak->Type=receive_int(fd);
	tweak->MinValue=receive_int(fd);
	tweak->MaxValue=receive_int(fd);
	
	return tweak;
}

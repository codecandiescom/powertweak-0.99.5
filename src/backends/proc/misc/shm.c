/*
 *  $Id: shm.c,v 1.7 2001/10/19 02:29:36 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <powertweak.h>



struct  shm_private {
	value_t value;
	long memorysize;
};

/* methods */
static value_t shm_get_value(struct tweak *tweak) 
{
	struct shm_private *pvt;
	pvt=tweak->PrivateData;
	return pvt->value;
}

static int shm_get_value_raw (struct tweak *tweak)
{
	FILE *file;
	struct shm_private *pvt;

	pvt = tweak->PrivateData;

	/* read the initial value */
	file = fopen("/proc/sys/kernel/shmmax","r");
	if (file==NULL)
		return FALSE;

	if (fscanf(file,"%i",&pvt->value.intVal)!=1) {
		fclose (file);
		return FALSE;
	}

	fclose(file);
	return TRUE;
}

static void shm_set_value(struct tweak *tweak,value_t value, int now) 
{
	struct shm_private *pvt;
	FILE *file;
	long long size=0;

	pvt=tweak->PrivateData;
	pvt->value = value; 

	if (now==0)
		return;

	file = fopen("/proc/sys/kernel/shmmax","w");
	if (file==NULL) {
		return;
	}

	size =pvt->value.intVal;
	size *= pvt->memorysize;
	size = size /100;
	fprintf(file,"%lli",size);
	fclose(file);
	
}

static long get_memory_size(void)
{	
	FILE *file;
	char line[3000];
	long bestguess = 0;

	file = fopen("/proc/meminfo","r");
	if (file != NULL) {
		while (!feof(file)) {
			fscanf(file,"%2000s\n",line);
			if (strncmp("Mem:",line,4)==0) {
				fscanf(file,"%li",&bestguess);
			}
		}
		fclose(file);
	}
	return bestguess;
}

static int SHMtweak(void)
{
	struct tweak *shm;
	struct shm_private *pvt;

	shm = alloc_tweak(TYPE_SLIDER);
	pvt = malloc(sizeof(struct shm_private));
	if (pvt==NULL) {
		shm->Destroy(shm);
		free(shm);
		return FALSE;
	}

	memset(pvt,0,sizeof(struct shm_private));
	shm->PrivateData = pvt;

	/* Fill the tweak structure */

	shm->WidgetText = strdup("Maximum shared memory percentage");
	shm->MinValue = 0;
	shm->MaxValue = 200;
	
	/* methods */
	
	shm->GetValue = &shm_get_value;
	shm->GetValueRaw = &shm_get_value_raw;
	shm->ChangeValue = &shm_set_value;

	pvt->memorysize = get_memory_size();
	if (pvt->memorysize<=0) { /* uh oh glibc borked */ 
		free(pvt);
		shm->Destroy(shm);
		free(shm);
		return FALSE;
	}

	if (shm_get_value_raw(shm) == FALSE) {
		free(pvt);
		shm->Destroy(shm);
		free(shm);
		return FALSE;
	}

	set_value_int(pvt->value,(int)((100.0*get_value_int(pvt->value)+1)/pvt->memorysize));
	shm->ConfigName =strdup("KERNEL-SHMALL");	
	shm->Description=strdup(
"This is the limit on the size of each system 5 style shared\n\
memory object in the system. Some large database applications\n\
such as Oracle may require this is raised from the default\n. \
The Oracle install guide states that SHMMAX should be set to\n\
0.5 * Physical Memory ");

	/* register the tweak */

	RegisterTweak(shm,"mmt","Kernel","Resources","Resources");
	return TRUE;
}


int InitPlugin(void)
{
	return SHMtweak();
}


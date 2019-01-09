/*
 *  $Id: powernow.c,v 1.6 2001/10/19 01:23:12 davej Exp $
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
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/io.h>
#include <fcntl.h>
#include <powertweak.h>
#include "powernow.h"


struct  pnow_private {
	value_t value; /* in Mhz */
	int busfreq;   /* in Mhz */
	char *cpuname;
};

/* methods */
static void PowerNOW_tweak_destructor(struct tweak *tweak)
{
	struct pnow_private *private;

	default_destructor (tweak);
	private = tweak->PrivateData;
	if (private != NULL) {
		release_value (&private->value);
		free (private->cpuname);
		free (private);
	}
}

static value_t powernow_get_value(struct tweak *tweak)
{
	struct pnow_private *pvt;
	pvt=tweak->PrivateData;

	return pvt->value;
}

static void powernow_set_value(struct tweak *tweak,value_t value, int immediate)
{
 	struct pnow_private *pvt;
	pvt=tweak->PrivateData;


	pvt->value = value;
	if (immediate==0)
		return;
	
	set_cpu_frequency_K6(get_value_int(value),pvt->busfreq);

}

int PowerNow_tweak(char *CPUNAME)
{
	struct tweak *tweak;
	struct pnow_private *pvt;
	char LINE[1000];
	FILE *file;
	float f=300;

	tweak = alloc_tweak(TYPE_SLIDER);
	pvt = malloc(sizeof(struct pnow_private));
	if (pvt==NULL) {
		free (tweak);
		return FALSE;
	}

	memset(pvt,0,sizeof(struct pnow_private));

	tweak->PrivateData = pvt;

	/* Fill the tweak structure */

	tweak->WidgetText = strdup("PowerNow CPU frequency (Mhz)");
	tweak->MinValue = 200;
	tweak->MaxValue = 600;

	/* methods */
	tweak->Destroy = &PowerNOW_tweak_destructor;
	tweak->GetValue = &powernow_get_value;
	tweak->ChangeValue = &powernow_set_value;

	pvt->busfreq = 100; /* BIG FIXME!!!!!!!!! */
	/* now read the initial value */

	file = fopen("/proc/cpuinfo","r");

	while (feof(file)==0) {
		char *tmp;
		fgets(LINE,950,file);
		tmp = strstr(LINE,"cpu MHz");
		if (tmp!=NULL) {
			tmp =strstr(LINE,":");
			if (tmp!=NULL)
				sscanf(tmp+1,"%f",&f);
		}
	}
	fclose(file);

	printf("Initial frequency estimated as %f \n",f);

	pvt->value.intVal = (int)(f+0.5); /* 0.5 for rounding */


	tweak->ConfigName =strdup("CPU-FREQUENCY");
	tweak->Description=strdup(
"This is the frequency of your CPU. Be careful with this value, \nDO NOT SET IT TOO HIGH!!!\n"
"The actual value will be rounded downwards to a the closest achievable frequency.");

	/* register the tweak */

	pvt->cpuname = strdup (CPUNAME);
	RegisterTweak(tweak,"mmmt","Hardware","CPU", pvt->cpuname, "Tweaks");
	return TRUE;
}

/*
 *  $Id: amdk6plus.c,v 1.2 2001/05/28 02:41:00 davej Exp $
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


static int wrmsr(int cpu, unsigned long msrindex, unsigned long long *val)
{
	char cpuname[16];
	int fh, ret;

	snprintf (cpuname,15, "/dev/cpu/%d/msr", cpu);

	fh = open (cpuname, O_WRONLY);

	if (fh==-1)
		ret = -1;
	else {
		lseek (fh, msrindex, SEEK_CUR);
		ret = (write (fh, val, 8) == 8);
		close (fh);
	}
	return (ret);
}


/* Clock ratio multiplied by 10 */
static int ClockRatio[8] = {	45,  /* 000 -> 4.5x */
				50,  /* 001 -> 5.0x */
				40,  /* 010 -> 4.0x */
				55,  /* 011 -> 5.5x */
				20,  /* 100 -> 2.0x */
				30,  /* 101 -> 3.0x */
				60,  /* 110 -> 6.0x */
				35   /* 111 -> 3.5x */
};



void set_cpu_frequency_K6(unsigned int Mhz, int fsb)
{
	int i;
	unsigned int best=200; /* safe initial values */
	unsigned int besti=4;
	unsigned long outvalue=0,invalue=0;
	unsigned long long msrval;

	/* Get IO permissions */
	if (iopl(3)!=0) {
		printf("Not enough permissions \n");
		return;
	}

	/* Find out which bit-pattern we want */

	for (i=0;i<8;i++) {
		unsigned int newclock;
		newclock = (ClockRatio[i]*fsb/10);
		if ((newclock > best) && (newclock <= (Mhz+1))) {
			/* +1 is for compensating kernel measurement inaccuracies and stupid users */
			best = newclock;
			besti = i;
		}
	}

	/* "besti" now contains the bitpattern of the new multiplier.
	   we now need to transform it to the BVC format, see AMD#23446 */

	outvalue = (1<<12) | (1<<10) | (1<<9) | (besti<<5);

	msrval = 0xFFF1;  	/* FIXME!! we should check if 0xfff0 is available */
	wrmsr(0,0xC0000086, &msrval); /* enable the PowerNow port */
	invalue=inl(0xfff8);
	invalue = invalue & 15;
	outvalue = outvalue | invalue;
	outl(outvalue ,0xFFF8);
	msrval = 0xFFF0;
	wrmsr(0,0xC0000086, &msrval); /* disable it again */


}

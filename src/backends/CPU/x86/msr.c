/*
 *  $Id: msr.c,v 1.4 2001/05/28 02:41:00 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int rdmsr(int cpu, unsigned long msrindex, unsigned long long *val)
{
	char cpuname[16];
	int fh, ret;

	snprintf (cpuname, 15,"/dev/cpu/%d/msr", cpu);

	fh = open (cpuname, O_RDONLY);

	if (fh==-1)
		ret = -1;
	else {
		lseek (fh, msrindex, SEEK_SET);
		ret = (read (fh, val, 8) == 8);
		close (fh);
	}
	return ret;
}


int wrmsr(int cpu, unsigned long msrindex, unsigned long long *val)
{
	char cpuname[16];
	int fh, ret;

	snprintf (cpuname,15, "/dev/cpu/%d/msr", cpu);

	fh = open (cpuname, O_WRONLY);

	if (fh==-1)
		ret = -1;
	else {
		lseek (fh, msrindex, SEEK_SET);
		ret = (write (fh, val, 8) == 8);
		close (fh);
	}
	return (ret);
}

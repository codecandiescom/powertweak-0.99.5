/* Copyright (C) 1996-1998 Robert H”hne, see COPYING.RH for details */
/* This file is part of RHIDE. */

#include <sys/time.h>
#include <time.h>
#include <unistd.h>

unsigned short TICKS(void)
{
  struct timeval val;
  gettimeofday(&val,(struct timezone *)NULL);
  return (val.tv_sec*18 + (val.tv_usec*18)/1000000);
}

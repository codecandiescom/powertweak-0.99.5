#include <config.h>

#define HAVE_PM_LINUX_PROC
#define HAVE_LINUX_BYTEORDER_H
#define PATH_PROC_BUS_PCI "/proc/bus/pci"
#define HAVE_OWN_HEADER_H

#if SIZEOF_VOIDP > 4
	#define HAVE_64BIT_ADDRESS
	#if SIZEOF_LONG > 4
		#define HAVE_LONG_ADDRESS
	#endif		
#endif 

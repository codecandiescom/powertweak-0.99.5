#ifndef _INCLUDE_GUARD_POWERTWEAK_MACROS_H
#define _INCLUDE_GUARD_POWERTWEAK_MACROS_H

/* Simple macros for powertweak */


#ifndef pt_unused
#ifdef __GNUC__
	#define pt_unused __attribute((unused))
#else
	#define pt_unused
#endif
#endif

#ifdef __LCLINT__
	#undef pt_unused
	#define pt_unused
#endif
#ifndef TRUE
	#define TRUE 1
#endif

#ifndef FALSE
	#define FALSE 0
#endif

#endif

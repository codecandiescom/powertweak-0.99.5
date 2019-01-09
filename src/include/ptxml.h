/*
 *  $Id: ptxml.h,v 1.1 2001/10/19 01:38:13 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2001 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#ifndef _INCLUDE_GUARD_PTXML_H_
#define _INCLUDE_GUARD_PTXML_H_

#define match_record(text,variable) \
	do {  if (strcasecmp((char*)cur->name, (text))==0) \
		(variable) = (char *) xmlNodeListGetString(doc, cur->xmlChildrenNode, 1);} while (0)


#define match_record3(text,variable,func) \
	do { char *CHPTR; \
	     CHPTR = (char*)xmlNodeListGetString(doc,cur->xmlChildrenNode,1); \
	     if ( (strcasecmp((char*)cur->name, (text))==0) && \
		 (CHPTR!=NULL) ) \
		(variable) = func(CHPTR); free(CHPTR);} while (0)

#endif


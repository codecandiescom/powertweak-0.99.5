/*
 *  $Id: profile.c,v 1.8 2001/10/19 01:38:13 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <parser.h>		/* libxml */

#include <ptxml.h>
#include <powertweak.h>


static value_t perform_operation(value_t current, int Operator, int Operand)
{
	value_t val = {0,NULL};
	int curval;
	
	val.strVal = current.strVal;
	curval = get_value_int(current);
	
	if ( (Operator == OPERATOR_LT) && (curval >= Operand) )
		curval = Operand - 1;
		
	if ( (Operator == OPERATOR_GT) && (curval <= Operand) )
		curval = Operand + 1;
		
	if ( (Operator == OPERATOR_LET) && (curval > Operand) )
		curval = Operand;
		
	if ( (Operator == OPERATOR_GET) && (curval < Operand) )
		curval = Operand;
		
	if (Operator == OPERATOR_EQ) 
		curval = Operand;
		
	if ( (Operator == OPERATOR_NEQ) && (curval == Operand) )
		curval = Operand+1;
		
	set_value_int(val,curval);
	
	return val;
}

int operand_is_valid(int current, int Operator, int Operand)
{
	int curval;

	curval = current;
	
	if ( (Operator == OPERATOR_LT) && (curval >= Operand) )
		return 0;
		
	if ( (Operator == OPERATOR_GT) && (curval <= Operand) )
		return 0;
		
	if ( (Operator == OPERATOR_LET) && (curval > Operand) )
		return 0;
		
	if ( (Operator == OPERATOR_GET) && (curval < Operand) )
		return 0;
		
	if ( (Operator == OPERATOR_EQ)  && (curval != Operand) )
		return 0;
		
	if ( (Operator == OPERATOR_NEQ) && (curval == Operand) )
		return 0;
		
	return 1;
}

int Operator2Operator(char *Operator)
{
	int op=0;
	
	if (Operator==NULL)
		return 0;
	
	if (strcmp(Operator,"=")==0)
		op = OPERATOR_EQ;
	if (strcmp(Operator,"!=")==0)
		op = OPERATOR_NEQ;
	if (strcmp(Operator,"<=")==0)
		op = OPERATOR_LET;
	if (strcmp(Operator,">=")==0)
		op = OPERATOR_GET;
	if (strcmp(Operator,"<")==0)
		op = OPERATOR_LT;
	if (strcmp(Operator,">")==0)
		op = OPERATOR_GT;
	if (strcmp(Operator,"=")==0)
		op = OPERATOR_EQ;
	if (strcmp(Operator,"!=")==0)
		op = OPERATOR_NEQ;
	if (strcmp(Operator,"leq")==0)
		op = OPERATOR_LET;
	if (strcmp(Operator,"geq")==0)
		op = OPERATOR_GET;
	if (strcmp(Operator,"lt")==0)
		op = OPERATOR_LT;
	if (strcmp(Operator,"gt")==0)
		op = OPERATOR_GT;
	return op;
}

static void actuate_profile_setting(char *Configname, char *Operator, int Operand, int immediate)
{
	int operator;
	struct tweak *tweak;
	
	operator = Operator2Operator(Operator); /* bwhahaha */
	tweak = find_tweak_by_configname(Configname);
	if (tweak==NULL)
		return;
	
	tweak->ChangeValue(tweak,perform_operation(tweak->GetValue(tweak),operator,Operand),immediate);
	
}

static void do_one_xml_record(xmlDocPtr doc, xmlNodePtr cur);

void merge_profile(char *Filename)
{
	xmlDocPtr doc;
	xmlNodePtr cur;

	if (Filename == NULL)
		return;

	doc = xmlParseFile(Filename);
	if (doc == NULL) {
		printf("Severe XML error: doc == NULL!!\n");
		printf("Probable cause: file %s not found.\n", Filename);
		return;
	}

	cur = xmlDocGetRootElement(doc);
	if (cur == NULL) {
		printf("Severe XML error (%s): cur == NULL",Filename);
		xmlFreeDoc(doc);
		return;
	}


	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		if ((cur->name != NULL) && (strcasecmp((char *) cur->name, "SETTING") == 0))
			do_one_xml_record(doc, cur);

		cur = cur->next;
	}
	xmlFreeDoc(doc);

}


static void do_one_xml_record(xmlDocPtr doc, xmlNodePtr cur)
{

	char *Config=NULL,*Operator=NULL;
	int Operand=0;

	assert(cur != NULL); /* cannot happen.. in theory */

	cur = cur->xmlChildrenNode;
	while (cur != NULL) {
		assert(cur->name != NULL);
		
		match_record("CONFIGNAME",Config);
		match_record("OPERATOR",Operator);
		match_record3("VALUE",Operand,atoi);
		
		cur = cur->next;

	}
	
	actuate_profile_setting(Config, Operator, Operand, 1);
	
	if (Config!=NULL)
		free(Config);
	if (Operator!=NULL)
		free(Operator);
	

}


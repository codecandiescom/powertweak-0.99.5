/*
 *  $Id: libpowertweak.c,v 1.43 2003/07/08 11:42:12 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <glob.h>

#include <powertweak.h>
#include <config.h>
#include <client.h>
#include <client-utils.h>

extern struct tweak *tweaks;

/*
 * InitTweakBackend
 * This is _the_ function a GUI calls for initialisation of libtweak.
 * It in turn initializes all backends and then returns the pointer to the
 * entire (struct tweak*) tree.
 */


/*
 * DestroyAllTweaks()
 *
 * This routine recurses through the tweak list freeing up
 * any allocating memory it finds as it goes, and then freeing
 * each tweak in turn.
 */
void DestroyAllTweaks (struct tweak *T)
{
	struct tweak *temp;

	while (T != NULL) {
		/* Destroy subtrees using recursion */
		if (T->Sub != NULL)
			DestroyAllTweaks (T->Sub);

/*		printf("freeing (%p) %s\n", (void *)T, T->WidgetText);*/

		/* Tell backend to free up backend specific allocs */
		if (T->Destroy != NULL)
			T->Destroy (T);
		
		temp = T->Next;
		free (T);
		T = temp;
	}
}


/*
 * DumpTweak()
 *
 * Debugging routine.
 *
 */

static void printspaces(int level)
{
	int i;
	for (i=1;i<=level;i++)
		printf(" ");
}

void DumpTweak (struct tweak *tweak, int level)
{
	printspaces (level);
	printf ("Next: %p\t", (void*)tweak->Next);
	printf ("Sub : %p\n", (void*)tweak->Sub);
	printspaces (level);
	printf ("WidgetText: %s\n", tweak->WidgetText);
	printspaces (level);
	printf ("Description: %s\n", tweak->Description);
	printspaces (level);
	printf ("ConfigName: %s\n", tweak->ConfigName);
	printspaces (level);
	printf ("Type: ");
	switch (tweak->Type) {
	case TYPE_NODE_ONLY:
		printf ("No question");
		break;
	case TYPE_CHECKBOX:
		printf ("Checkbox");
		break;
	case TYPE_SPINBOX:
		printf ("Spinbox");
		break;
	case TYPE_SLIDER:
		printf ("Slider");
		break;
	case TYPE_TEXT:
		printf ("Textlabel");
		break;
	case TYPE_COMBO:
		printf ("Combobox");
		break;
	case TYPE_COMBOELEM:
		printf ("Combobox element");
		break;
	case TYPE_LABEL:
		printf ("Label");
		break;
	case TYPE_INFO_INT:
		printf ("Info-Int");
		break;
	case TYPE_INFO_STRING:
		printf ("Info-String");
		break;
	case TYPE_INFO_BOOL:
		printf ("Info-Bool");
		break;

	case TYPE_TREE:
		printf ("Tree");
		break;
	case TYPE_TAB:
		printf ("Tab");
		break;
	case TYPE_FRAME:
		printf ("Frame");
		break;
	case TYPE_HFRAME:
		printf ("HFrame");
		break;
	case TYPE_RADIO_GROUP:
		printf ("RadioGroup");
		break;
	case TYPE_RADIO_OPTION:
		printf ("RadioOption");
		break;
		
	default:
		printf ("Unknown type!");
		printf ("(%d)", tweak->Type);
		break;
	};

	printf ("\n");
	printspaces (level);
	printf ("MinValue: %d\t", tweak->MinValue);
	printf ("MaxValue: %d\n", tweak->MaxValue);
	printspaces (level);
	printf ("fnChangeValue: %p\t", (void*)tweak->ChangeValue);
	printf ("fnGetValue: %p\t", (void*)tweak->GetValue);
	printf ("fnIsValid: %p\n", (void*)tweak->IsValid);
	printspaces (level);
	printf ("fnDestructor: %p\t", (void*)tweak->Destroy);
	printf ("PrivateData: %p\t", (void*)tweak->PrivateData);
	printf ("Widget: %p\n", (void*)tweak->Widget);
	printspaces (level);
	printf ("TempValue(int): %x\t", get_value_int(tweak->TempValue));
	printf ("TempValue(string): %s\n", tweak->TempValue.strVal);
	printf ("\n");
}


void DumpTweaks (struct tweak *T, int level)
{
	while (T != NULL) {
		DumpTweak (T, level);

		/* Print subtrees using recursion */
		if (T->Sub != NULL)
			DumpTweaks (T->Sub, level+1);

		T = T->Next;
	}
}


static struct tweak *__find_tweak_by_configname(char *name, struct tweak *tweak)
{
	struct tweak *tmp;
	
	if (tweak==NULL)
		return NULL;
	
	if ((tweak->ConfigName!=NULL) && (strcmp(name,tweak->ConfigName)==0)) 
		return tweak;
	
	tmp=__find_tweak_by_configname(name,tweak->Sub);
	if (tmp!=NULL)
		return tmp;

	tmp=__find_tweak_by_configname(name,tweak->Next);
	if (tmp!=NULL)
		return tmp;

	return NULL;		
}

struct tweak *find_tweak_by_configname(char *name)
{
	return __find_tweak_by_configname(name,tweaks);
}


void load_profiles (char *pattern)
{
	glob_t globbuf;
	char **filep, *file;
	int err;
	if ((err = glob (pattern, 0, NULL, &globbuf))) {
		if (err != GLOB_NOMATCH)
			printf ("error while reading profiles in %s\n", pattern);
		return; 
	}
	for (filep = globbuf.gl_pathv; (file = *filep); filep++) {
/*		printf("Attempting to merge %s \n",file);*/
		merge_profile (file); 
	}
	globfree (&globbuf);
}


/*
 * LoadPluginsIfNecessary()
 * Does exactly as the descriptive function name implies.
 * This allows the UIs to function if the daemon isn't loaded.
 * It's a bit of a hack, but it works.
 * Note, that the 'disable info tweaks' is not possible here.
 * It's not that big a deal anyhow, as the switch is only useful
 * for lowering memusage of the daemon.
 */
void LoadPluginsIfNecessary(int *havedaemon, errfn *errfunc,int error, struct tweak **localtweaks)
{
	switch (error) {
		case ERROR_CANT_AUTHENTICATE:
			if (InitPlugins(TRUE)==FALSE)
				return;
			sort_tweak_list(&tweaks);
			merge_config_in_tree (tweaks, "/etc/powertweak/tweaks", 0);
			load_profiles(PROFILEDIR "*");
			*havedaemon = 0;
			*localtweaks = tweaks;
			break;

		case ERROR_NOT_ROOT:
			errfunc ("Error!", "Daemon not running as root!");
			exit (-1);
			break;

		default:
			errfunc ("Error!", "Unknown error!");
			exit (-1);
			break;
	}
}

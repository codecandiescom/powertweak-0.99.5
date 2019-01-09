
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "powertweak.h"

#include "client.h"
#include "client-utils.h"

char *programname;
int HaveDaemon = 1;
struct tweak *tweaks;

void show_error (char *title, char *message)
{
	fprintf (stderr, "%s %s\n", title, message);
}

void printNext(struct tweak *t, int depth)
{
	int i=0;
	value_t val;
	
	for (;i<depth;i++)
	{
		printf("  ");
	}
	printf("%s", t->WidgetText);
	
	if ((t->Type < 100) && (t->GetValue!=NULL))
	{
		printf(" : \t");
		val = t->GetValue(t);
	
		if (val.strVal)
		{
			printf("%s", val.strVal);
		}
		else
		{
			printf("%i", val.intVal);
		}
	}

	printf("\n");
	
	if (t->Sub)
		printNext(t->Sub, depth+1);
	if (t->Next)
		printNext(t->Next, depth);
}

/*TODO - add parameters (like xml output)*/
int main (/*int argc, char **argv*/)
{
	struct tweak *localtweaks;
	int error=0;

	if (getuid()!=0) {
		show_error ("Error!", "gpowertweak only runs as root.");
		exit (-1);
	}

	programname = strdup("Powertweak " VERSION);

	error = InitClientBackend (&localtweaks);
	if (localtweaks == NULL)
		LoadPluginsIfNecessary(&HaveDaemon, &show_error, error, &localtweaks);

	/* If all went ok, do the main stuff. */
	if (localtweaks != NULL) 
	{
		printf("%s (build %s %s)\n", programname, __DATE__, __TIME__);
		printf("%s\n", HaveDaemon ? "connected to powertweakd" : "loading tweaks locally...");
		printf("\n");

		/*loop through the tweaks and print*/
		printNext(localtweaks, 0);
	}

	/* Shut down gracefully. */
	if (HaveDaemon) {
		CloseClientBackend ();
	} else {
		DestroyAllTweaks (localtweaks);
		ShutdownPlugins();
		DestroyInitFuncList();
	}
	
	free(programname);
	return 0;
}

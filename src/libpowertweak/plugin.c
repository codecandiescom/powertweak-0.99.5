/*
 *  $Id: plugin.c,v 1.26 2002/05/23 00:30:18 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 *  	This file takes care of loading plugins
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <assert.h>

/* for readdir/stat and friends */
#include <sys/types.h>
#include <sys/stat.h>
#include <dirent.h>
#include <dlfcn.h>

#include <powertweak.h>

#define CORE_PLUGINS  RTLD_GLOBAL | RTLD_NOW
#define NORMAL_PLUGINS RTLD_GLOBAL | RTLD_LAZY

struct initentry {
	char *pluginname;
	struct initentry *Next;
	plugin_fn *initfunc;
	void	*pluginhandle;
};

static struct initentry *coreinitfunctions = NULL;
static struct initentry *initfunctions = NULL;


static void AddToInitFuncList (plugin_fn *initfunc, void *handle, char *name)
{
	struct initentry *newnode;

	newnode = malloc (sizeof (struct initentry));
	if (newnode == NULL)
		return;
	newnode->pluginname = name;
	newnode->Next = initfunctions;
	newnode->initfunc = initfunc;
	newnode->pluginhandle = handle;
	initfunctions = newnode;
}


void _DestroyInitFuncList (struct initentry *funcs)
{
	struct initentry *tmp, *tmp2;

	assert (funcs!=NULL);

	tmp = funcs;

	while (tmp != NULL) {
		tmp2 = tmp->Next;
		free (tmp->pluginname);
		free (tmp);
		tmp = tmp2;
	}
}
void DestroyInitFuncList (void)
{
	_DestroyInitFuncList(initfunctions);
	_DestroyInitFuncList(coreinitfunctions);
}


int have_core_plugin(char *name)
{
	struct initentry *tmp;

	tmp = coreinitfunctions;

	while (tmp != NULL) {
		if (strcmp (tmp->pluginname, name) == 0)
			return TRUE;
		tmp=tmp->Next;
	}
	return FALSE;
}


static void load_plugin(char *filename, char *name, int type)
{
	void *handle;
	plugin_fn *initfunc=NULL;

	handle = dlopen (filename, type);

	if (handle==NULL) {
		printf("plugin error: %s\n",dlerror());
		return;
	}

	initfunc = (plugin_fn*)dlsym(handle,"InitPlugin");

	if (initfunc!=NULL)
		AddToInitFuncList(initfunc, handle, strdup(name));
}


static int load_plugins (char *path, int core)
{
	DIR *dir;
	struct dirent *di;
	char *filebuf;

	dir = opendir (path);
	if (dir == NULL) {
		printf ("plugin directory '%s' not found \n",path);
		return FALSE;
	}
	di = readdir (dir);
	while (di != NULL) {
		int stringlen;
		
		stringlen = strlen(path)+strlen(di->d_name)+4;
		filebuf= calloc(stringlen,1);assert(filebuf!=NULL);
		strncpy (filebuf, path, stringlen);
		strncat (filebuf, "/", stringlen-1-strlen(filebuf));
		strncat (filebuf, di->d_name, stringlen-1 - strlen (filebuf));

		if ((strstr (filebuf, ".so") != NULL) &&
			(strstr(filebuf,".so.") == NULL) ) {
			load_plugin (filebuf, di->d_name, core);
		}

		di = readdir (dir);
		free(filebuf);
	}

	if (closedir (dir)!=0) {
		printf("Oops. You did something we didn't think of.\n");
		return FALSE;
	}

	return TRUE;
}


/*
 * Walk the list of plugin init routines, and call them.
 * Info: Whether or not info backends should be used.
 */
void InitialisePlugins (int Info)
{
	struct initentry *tmp,**prev;
	tmp = initfunctions;
	prev = &initfunctions;
	while (tmp != NULL) {
		if (!tmp->initfunc(Info)) { /* init failed */
			/*printf ("Initialising plugin %s failed\n", tmp->pluginname);*/
			free (tmp->pluginname);
			*prev = tmp->Next;
			dlclose(tmp->pluginhandle);
			free(tmp);
			tmp = *prev;
		} else {
			prev = &(tmp->Next);
			tmp=tmp->Next;
		}
	}
}


/*
 * Load plugins, call init routines.
 */
int InitPlugins (int Info)
{
	/* First the core plugins */
	if (load_plugins (COREPLUGINDIR, CORE_PLUGINS) == FALSE)
		return FALSE;

	InitialisePlugins(Info);
	coreinitfunctions = initfunctions;
	initfunctions = NULL;

	/* Then the normal ones */
	load_plugins (PLUGINDIR, NORMAL_PLUGINS);
	InitialisePlugins(Info);
	return TRUE;
}


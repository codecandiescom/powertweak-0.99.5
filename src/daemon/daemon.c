/*
 *  $Id: daemon.c,v 1.99 2003/07/06 23:03:06 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000, 2001 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 * This program creates a UNIX domain socket, and listens on it for
 * Powertweak commands (from root only). Mortals are disconnected immediately.
 */

#undef DEBUG

#include <assert.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <powertweak.h>
#include <cvsversion.h>

#include "utils.h"
#include "server.h"

#include <config.h>
#include <messages.h>

struct tweak *tweaks=NULL;

static int gotHUP=0, gotTERM=0;
static int dsocket;
static char nodaemon = 0;
static char showinfotweaks = TRUE;
static char *profile_pattern = PROFILEDIR "*";
static char *config_path = "/etc/powertweak/tweaks";  /*SVEN - what are these doing here? both the client and the server need them*/
static char socketname[]="/var/run/powertweakd";
static char pidFile[] = "/var/run/powertweakd.pid";

extern int HaveError;

static void SendTweakStructure(int fd,struct tweak *tweak)
{
	if ((tweak==NULL) || (HaveError)) 
		return;

	send_tweak(fd,tweak);

	if (tweak->Sub!=NULL) {
		send_message(fd,message_tweaksub);
		SendTweakStructure (fd,tweak->Sub);
	}

	if (tweak->Next!=NULL) {
		send_message(fd,message_tweaknext);
		SendTweakStructure (fd,tweak->Next);
	}
		
	send_message(fd,message_tweakreturn);	
}



static void RemoteGetValue(int fd)
{
	struct tweak *tweak;
	char *ConfigName;
	value_t value = {0, NULL};
	
	ConfigName = receive_string(fd);
	if (ConfigName==NULL)
		return;
	tweak = find_tweak_by_configname(ConfigName);
	free(ConfigName);
	
	if (tweak==NULL) 
	{
		log_message ("Not found (RemoteGetValue): %s\n", ConfigName);
	}
	else
	{
/*SVEN	log_message ("RemoteGetValue(%s)\n", tweak->WidgetText);*/
		value = tweak->GetValue(tweak);
	}

	send_value(fd,value);	
	release_value(&value);
}

static void RemoteGetValueRaw(int fd)
{
	struct tweak *tweak;
	char *ConfigName;
	value_t value = {0, NULL};
	
	ConfigName = receive_string(fd);
	if (ConfigName==NULL)
		return;
	tweak = find_tweak_by_configname(ConfigName);
	free(ConfigName);
	
	if (tweak==NULL) {
		log_message ("Not found (RemoteGetValueRaw): %s\n", ConfigName);
		return;
	}

/*SVEN	log_message ("RemoteGetValueRaw(%s)\n", tweak->WidgetText);*/
	if (tweak->GetValueRaw != NULL)
	{
		tweak->GetValueRaw(tweak);
	}
	else
	{
/*SVEN		log_message ("RemoteGetValueRaw(%s) [no GetValueRaw func ptr]\n", tweak->WidgetText);*/
		value = tweak->GetValue(tweak);
	}
	send_value(fd,value);	
	release_value(&value);
}

static void RemoteSetValue(int fd)
{
	struct tweak *tweak;
	char *ConfigName;
	int i;
	value_t value;
	
	ConfigName = receive_string(fd);
	if (ConfigName==NULL)
		return;
	tweak = find_tweak_by_configname(ConfigName);
	
	value = receive_value(fd);
	i = receive_int(fd);
	
	if ((HaveError==0)&&(tweak->ChangeValue!=NULL))
		tweak->ChangeValue(tweak,value,i);
	release_value(&value);
	free(ConfigName);
}


static void message_loop(int fd)
{
	int message=0;

	while ((message!=message_bye)&&(HaveError==0)) {
		message=receive_message(fd);
		if (HaveError!=0)
			continue;
		switch (message) {
		case message_requesttweaks:
		  	SendTweakStructure(fd, tweaks);
		  	break;
		case message_getvalue:
			RemoteGetValue(fd);
			break;
		case message_getvalueraw:
			RemoteGetValueRaw(fd);
			break;
		case message_setvalue:
			RemoteSetValue(fd);
			break;
		case message_saveconfig:
			save_tree_to_config (tweaks, "/etc/powertweak/tweaks");
			break;
		case message_hello:
		case message_bye:
			break;
		default:
			log_message ("Unknown message : %i\n",message);
		}
	}
}


static void HUP_handler(pt_unused int signum)
{
	gotHUP=1;
	(void) signal (SIGHUP, SIG_IGN);	/* ignore the HUP. */
}


static void TERM_handler(pt_unused int signum)
{
	gotTERM=1;
}


static void restart(void)
{
	DestroyAllTweaks (tweaks);
	tweaks = NULL;
	ShutdownPlugins();
	InitialisePlugins(showinfotweaks);

	if (tweaks == NULL) {
		log_message ("Something happened to the tweaks after SIGHUP!\n");
		ShutdownPlugins();
		DestroyInitFuncList();
		exit (1);
	}

	sort_tweak_list(&tweaks);
	merge_config_in_tree (tweaks, config_path, 1);
	load_profiles(profile_pattern);

	gotHUP = 0;
}


static void usage(char *program_name)
{
	fprintf (stderr, "usage: %s \
[ --config-file file ] [ -c file ] \
[ --profile-dir dir ] [ -p dir ] \
[ --no-daemon ] \
[ --no-info ] \
[ --version ]\n", program_name);
}


static int parse_command_line(int argc, char **argv)
{
	char **argp, *arg;

	for(argp = argv+1; argp <= argv + argc && (arg = *argp); argp++) {
		if (!strcmp(arg, "-p") || !strcmp(arg, "--profile-dir"))
			profile_pattern = *++argp;
		else if (!strcmp(arg, "-c") || !strcmp(arg, "--config-file"))
			config_path = *++argp;
		else if (!strcmp(arg, "--no-daemon"))
			nodaemon = 1;
		else if (!strcmp(arg, "--no-info"))
			showinfotweaks = FALSE;
		else if (!strcmp(arg, "--version")) {
			printf ("Powertweak " VERSION "\n" CVSID "\n");
			exit (EXIT_SUCCESS);
		}
		else
			return -1;
	}

	return 0;
}


static int create_daemon ()
{
	struct sockaddr_un address;
	struct sigaction sa;
	int i;
	FILE *fp;

	if (nodaemon == 0)
	{
		log_message("created Daemon.\n");
		if (daemon (0, 0) == -1)
			goto daemon_fail;
	}

	/*SVEN - check if there is one running already (if not, but the files exist in /var/run)
	we should be able to remove them and start.*/
	
	/* Close any non-standard file descriptors that got left around */
	for (i = 3; i < 7; i++)
		close(i);

	/* Disconnect from the controlling tty. */
#ifdef TIOCNOTTY
	fp = open(_PATH_TTY, O_RDWR | O_NOCTTY);
	if (fp >= 0) {
		(void) ioctl(fp, TIOCNOTTY, NULL);
		close(fp);
	}
#endif

	if ((dsocket = create_server(socketname, &address)) == 0)
	{
		log_message("failed to set up socket\n");
		goto No_socket;
	}

	/* Set up a signal handler for SIGHUP. */
	memset (&sa, 0, sizeof(sa));
	sa.sa_handler = HUP_handler;
	if (sigaction(SIGHUP, &sa, NULL))
	{
		log_message("failed to set up signal handler for SIGHUP\n");
		goto sig_fail;
	}

	/* Set up a signal handler for SIGTERM . */
	memset (&sa, 0, sizeof(sa));
	sa.sa_handler = TERM_handler;
	if (sigaction(SIGTERM, &sa, NULL))
	{
		log_message("failed to set up signal handler for SIGTERM\n");
		goto sig_fail;
	}
	
	/* Stash our PID in /var/run */
	fp = fopen(pidFile, "w");
	if (fp != NULL) {
		fprintf (fp,"%d\n", getpid());
		(void) fclose(fp);
	}
	log_message ("Powertweakd background process started\n");
	return TRUE;

sig_fail:
	close (dsocket);
	unlink (socketname);

No_socket:

daemon_fail:
	return FALSE;
}


static void daemon_loop (void)
{
	struct sockaddr_un address;
	int conn=0;

	/* Main idle loop. */
	while (gotTERM==0) {
		while (conn == 0) {
			conn = wait_on_socket (dsocket, &address, strlen(socketname));
			if (gotHUP == 1) {
				if (conn != 0) {
					close (conn);
					conn = 0;
				}
				restart();
			}
		}

		if (conn<0) {
			conn = 0;
			continue;
		}		

		assert (conn>0);

		HaveError = 0;
		switch (authenticate (conn)) {
			case AUTHENTICATED_OK:
				log_message ("Authenticated connection to powertweakd\n");
				update_tree_values(tweaks);
				message_loop(conn);
				break;

			case ERROR_CANT_AUTHENTICATE:
				log_message ("Authentication failure.\n");
				break;

			case ERROR_NOT_ROOT:
				log_message ("Non root connection to daemon.\n");
				break;
		}
		conn = 0;
	}

	unlink (pidFile);
	close (dsocket);
	unlink (socketname);
	log_message ("Powertweakd exiting\n");
}


int main (int argc, char **argv)
{
	int err = 0;

	if (getuid()!=0) {
		printf("You need to be root to run powertweakd.\n");
		exit(-1);
	}

	if (parse_command_line (argc, argv)) {
		usage(argv[0]);
		err = 1;
		goto No_commandline;
	}

	log_message ("Powertweak " VERSION "\n" CVSID "\n");
	
	if (create_daemon()==FALSE) 
	{
		log_message ("Daemon creation failed.\n");
		exit (-1);
	}

	if (InitPlugins (showinfotweaks)==FALSE)
		goto No_plugins;
 
 	sort_tweak_list(&tweaks);
 
 	if (argc<0)
 		RegisterTweak(NULL,""); /* [AV] Don't ask why this is here */
								/* [DJ] He's right, don't ask. :) */

	merge_config_in_tree (tweaks, config_path, 1);

	load_profiles(profile_pattern);

	if (tweaks == NULL) {
		log_message ("Uh oh. No tweaks found !\n");
		log_message ("Aborting %s.\n", argv[0]);
		err = 1;
		goto No_tweaks;
	}

	log_message ("Waiting for connection.\n");
	daemon_loop();

	DestroyAllTweaks (tweaks);

No_tweaks:
	ShutdownPlugins();
	DestroyInitFuncList();

No_plugins:

No_commandline:
	return err;
}

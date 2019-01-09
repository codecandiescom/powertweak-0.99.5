/*
 *  $Id: powertweak.h,v 1.69 2003/06/19 09:34:00 svenud Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#ifndef _INCLUDE_GUARD_POWERTWEAK_H_
#define _INCLUDE_GUARD_POWERTWEAK_H_

#ifndef TRUE
#define TRUE 1
#endif
#ifndef FALSE
#define FALSE 0
#endif

#include <linux/types.h>
#include "macros.h"
#include <values.h>
#include <config.h>

/*
 * Types (As first defined in PCILIB)
 */

#ifndef POWERTWEAK_TYPES
#define POWERTWEAK_TYPES
typedef __u8 byte;
typedef __u8 u8;
typedef __u16 word;
typedef __u16 u16;
typedef __u32 u32;

typedef signed short i16;

#endif

/* Function types for the 3 methods used in struct tweak.
 *
 * void ChangeValue(struct tweak *tweak, int NewValue, int Immediate) 
 *	Changes the value of a tweak. When Immediate!=0, the underlying
 *	system property is changed as well, otherwise only the value in
 *	the configuration file is changed.
 *
 * int  GetValue(struct tweak *tweak);
 *	Returns the value as it would be written to the configuration
 * 	file if the user selected "Save". If ChangeValue() is ever called,
 *	GetValue will return the value set by that. Otherwise, the value
 *	of the underlying system property is given.
 *
 * int  IsValid(struct tweak *tweak, int NewValue);
 *	Can be used to check if a value would be legal. Returns 1 if the
 *	NewValue is a legal value, 0 otherwise.
 *
 */
struct tweak;		

typedef void (change_fn)(struct tweak *, value_t, int);
typedef int (rawvalue_fn)(struct tweak *);
typedef value_t (value_fn)(struct tweak *);
typedef int (valueint_fn)(struct tweak *);
typedef int (valid_fn)(struct tweak *,value_t); 
typedef void (destructor_fn)(struct tweak *);


/* struct tweak is the way tweaks are passed from the backends to the
 * GUI and have the purpose to shield the GUI from the exact implementation
 * of the backends while providing enough information to actually build
 * the GUI.
 */
 
#define TYPE_NODE_ONLY		0
#define TYPE_CHECKBOX		1
#define TYPE_SPINBOX		2
#define TYPE_SLIDER			3
#define TYPE_TEXT			4
#define TYPE_COMBO			5
#define TYPE_COMBOELEM		6
#define TYPE_LABEL			7
#define TYPE_INFO_INT		8
#define TYPE_INFO_STRING	9
#define TYPE_INFO_BOOL		10
#define TYPE_RADIO_GROUP	11
#define TYPE_RADIO_OPTION	12
#define TYPE_BUTTON				13

#define TYPE_TREE	100
#define TYPE_TAB	101
#define TYPE_FRAME	102
#define TYPE_HFRAME	103

#define CONFIGNAME_MAXSIZE 128

struct tweak {
	/* List management */
	struct tweak *Next;	/* Next UI element of the same order */
	struct tweak *Sub;	/* Sub-menu UI elements */

	/* Actual tweak info */
	char *WidgetText;	/* Text of the tweak in the GUI */
	char *Description;	/* Longer description */
	char *ConfigName;	/* Unique identifier in the config file */

	int Type;		/* Type of tweak: (See #defines above) */

	unsigned int MinValue;		/* Minimum value, types 2 and 3 only */
	unsigned int MaxValue;		/* Maximum value, types 2 and 3 only */

	/* Methods */
	change_fn *ChangeValue;	/* Function pointer for callback */
	value_fn *GetValue;	/* Function pointer to get value from tweak struct */
	rawvalue_fn *GetValueRaw;	/* Fn ptr to get current state */
	valueint_fn *GetValueInt;/* Shortcut to get the integer value,
				   as most tweaks are ints */
	valid_fn *IsValid;	/* Verify a possible answer */
	destructor_fn *Destroy;	/* Destroy anything alloc'd for this tweak */

	void *PrivateData;	/* This is used by the individual
				 * backends to store the information needed
				 * to actually perform the tweak 
				 */

	value_t OrigValue;	/* Value of tweak at startup. */

	/* Fields the GUI can change/use at will. libpowertweak 
	 * doesn't even look at it or fill it, nor free it.
	 * The descriptions are only suggestions for their use.
	 * Do not use from inside the backends.
	 */

	void *Widget;		/* For the GUI to use at will */
	void *Tooltip;		/* Probably will only get used by GTK. */
	value_t  TempValue;	/* Value in the GUI before "Apply" */
};

/* Defines for profile-compare operators */

#define OPERATOR_DC	0
#define OPERATOR_NOT	1		
#define OPERATOR_EQ	2
#define OPERATOR_LT	4

/* shortcuts */

#define OPERATOR_GT	7
#define OPERATOR_GET	5
#define OPERATOR_LET	6
#define OPERATOR_NEQ	3

int operand_is_valid(int current, int Operator, int Operand);
int Operator2Operator(char *Operator);


void merge_profile(char *Filename);


/* Prototypes for functions that are used between layers */

/* Deconstruction */
void DestroyAllTweaks (struct tweak *T);
void default_destructor (struct tweak *tweak);

/* Function used by the backends to register a tweak to the rest of the
 * system.
 */
void RegisterTweak(/*@keep@*/ struct tweak *tweak, char *fmt, ...);
struct tweak *find_tweak_by_configname(char *name);


/* Save and load the tree to a config file */
int save_tree_to_config(struct tweak *tree, char *filename);
int merge_config_in_tree(struct tweak *tree, char *filename, int immediate);


/* Misc functions too simple/universal not to put in libpowertweak */

int fileexists(const char *filename);
int filewritable (const char *filename);

void sort_tweak_list(struct tweak **tweak);

struct tweak *alloc_tweak(int type);

void update_tree_values (struct tweak *tree);

/* Generic "is_valid" function for "lazy" backends  */
int generic_is_valid(struct tweak *tweak, value_t NewValue);

/* function to convert a widget type in string form to a number */
int string_widget_to_int(char *text);

/* convert a string of the form "0001 0010" to 18 */
unsigned long bitstring_to_long(char *text);

char *strdupcat (char *S1, char *S2);
char *strnewcat (char *S1, char *S2);

/* Debugging stuff */
void DumpTweaks (struct tweak *tweak, int level);
void DumpTweak (struct tweak *tweak, int level);

/* Plugin function prototype */
typedef int (plugin_fn)(int useinfotweaks);
typedef int (shutdown_cb_fn)(void);
int InitPlugins (int Info);
void InitialisePlugins (int Info);
void DestroyInitFuncList (void);
int have_core_plugin(char *name);

typedef void (errfn)(char *, char*);
void LoadPluginsIfNecessary(int *havedaemon, errfn *errfunc,int error, struct tweak **localtweaks);

void load_profiles (char *pattern);

/* Shutdown handling */
void RegisterShutdownCallback(shutdown_cb_fn *Func);
void ShutdownPlugins(void);

/* Get kernel version. */
struct kernel_ver {
	int major;
	int minor;
	int patchlevel;
};
void kernel_version (struct kernel_ver *kv);

void log_message(char *fmt, ...);

#endif

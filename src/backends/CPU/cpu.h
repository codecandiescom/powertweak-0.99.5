/*
 *  $Id: cpu.h,v 1.3 2001/09/07 18:46:16 davej Exp $
 *	This file is part of Powertweak Linux.
 *	(C) 2000 Dave Jones, Arjan van de Ven.
 *
 * 	Licensed under the terms of the GNU GPL License version 2.
 *
 */

#include "x86/x86.h"

/* Generic CPU backend functions. */
void CPU_tweak_destructor (struct tweak *tweak);

void Make_CPU_menuname (struct cpu_identity *id);
void AddTo_CPU_tree(struct cpu_identity *id, struct tweak *tweak, char *tab);
void AddTo_CPU_treehframe (struct cpu_identity *id ,struct tweak *tweak, char *tab, char *frame);
struct tweak *alloc_CPU_tweak (int CPUnum, int type);

/* The following funcs are declared in each arch subdir.*/
void identify_CPU(struct cpu_identity *id);
void Add_CPU_info_page(struct cpu_identity *id);

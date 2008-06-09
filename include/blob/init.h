/*
 * init.h: Support for init and exit lists
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 */

#ident "$Id: init.h,v 1.3 2002/01/03 16:07:17 erikm Exp $"

#ifndef BLOB_INIT_H
#define BLOB_INIT_H

#include <blob/types.h>

#define INIT_MAGIC (0x496e6974)		/* "Init" */
#define EXIT_MAGIC (0x45786974)		/* "Exit" */


typedef void(*initfunc_t)(void);


typedef struct {
	u32 magic;
	initfunc_t callback;
	int level;
} initlist_t;


#define __init __attribute__((unused, __section__(".initlist")))

#define __initlist(fn, lvl) \
static initlist_t __init_##fn __init = { \
	magic:    INIT_MAGIC, \
	callback: fn, \
	level:	  lvl }


#define __exit __attribute__((unused, __section__(".exitlist")))

#define __exitlist(fn, lvl) \
static initlist_t __exit_##fn __exit = { \
	magic:    EXIT_MAGIC, \
	callback: fn, \
	level:	  lvl }


/* minimum and maximum levels */
#define INIT_LEVEL_MIN (0)
#define INIT_LEVEL_MAX (99)

/* define some useful levels */
#define INIT_LEVEL_DRIVER_SELECTION	(0)
#define INIT_LEVEL_INITIAL_HARDWARE	(10)
#define INIT_LEVEL_PARAM_LIST		(20)
#define INIT_LEVEL_OTHER_HARDWARE	(30)
#define INIT_LEVEL_OTHER_STUFF		(40)


void init_subsystems(void);
void exit_subsystems(void);


#endif

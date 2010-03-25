/*-------------------------------------------------------------------------
 * Filename:      command.h
 * Version:       $Id: command.h,v 1.2 2001/10/07 15:27:35 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Header file for command.c
 * Created at:    Sun Aug 29 17:23:33 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Tue Sep 28 23:38:04 1999
 *-----------------------------------------------------------------------*/
/*
 * command.c: Command line functions for blob
 *
 * Copyright (C) 1999  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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

/* $Id: command.h,v 1.2 2001/10/07 15:27:35 erikm Exp $ */

#ifndef BLOB_COMMAND_H
#define BLOB_COMMAND_H

#include <blob/types.h>

#define COMMAND_MAGIC (0x436d6420)	/* "Cmd " */


typedef int(*commandfunc_t)(int, char *[]);


typedef struct commandlist {
	u32 magic;
	char *name;
	char *help;
	commandfunc_t callback;
	struct commandlist *next;
} commandlist_t;


#define __command __attribute__((unused, __section__(".commandlist")))

#define __commandlist(fn, nm, hlp) \
static commandlist_t __command_##fn __command = { \
	magic:    COMMAND_MAGIC, \
	name:     nm, \
	help:     hlp, \
	callback: fn }
			 


extern commandlist_t *commands;

int parse_command(char *cmdline);

#define MAX_COMMANDLINE_LENGTH (128)
#define MAX_ARGS (MAX_COMMANDLINE_LENGTH / 4)

void DisplayPrompt(char *prompt);
int GetCommand(char *command, int len, int timeout);


#endif

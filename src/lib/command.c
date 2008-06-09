/*-------------------------------------------------------------------------
 * Filename:      command.c
 * Version:       $Id: command.c,v 1.9 2002/01/03 16:07:18 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Command line functions for blob
 * Created at:    Sun Aug 29 17:23:40 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sun Oct  3 21:08:27 1999
 *-----------------------------------------------------------------------*/
/*
 * command.c: Command line functions for blob
 *
 * Copyright (C) 1999 2000 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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

#ident "$Id: command.c,v 1.9 2002/01/03 16:07:18 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif


#include <blob/command.h>
#include <blob/errno.h>
#include <blob/error.h>
#include <blob/init.h>
#include <blob/serial.h>
#include <blob/time.h>
#include <blob/types.h>
#include <blob/util.h>

/* command list start and end. filled in by the linker */
extern u32 __commandlist_start;
extern u32 __commandlist_end;



/* the first command */
commandlist_t *commands;


static void init_commands(void)
{
	commandlist_t *lastcommand;
	commandlist_t *cmd, *next_cmd;

	commands = (commandlist_t *) &__commandlist_start;
	lastcommand = (commandlist_t *) &__commandlist_end;

	/* make a list */
	/* FIXME: should sort the list in alphabetical order over here */
	cmd = next_cmd = commands;
	next_cmd++;

	while(next_cmd < lastcommand) {
		cmd->next = next_cmd;
		cmd++;
		next_cmd++;
	}
}

__initlist(init_commands, INIT_LEVEL_OTHER_STUFF);


#define STATE_WHITESPACE (0)
#define STATE_WORD (1)
static void parse_args(char *cmdline, int *argc, char **argv)
{
	char *c;
	int state = STATE_WHITESPACE;
	int i;

	*argc = 0;

	if(strlen(cmdline) == 0)
		return;

	/* convert all tabs into single spaces */
	c = cmdline;
	while(*c != '\0') {
		if(*c == '\t')
			*c = ' ';

		c++;
	}
	
	c = cmdline;
	i = 0;

	/* now find all words on the command line */
	while(*c != '\0') {
		if(state == STATE_WHITESPACE) {
			if(*c != ' ') {
				argv[i] = c;
				i++;
				state = STATE_WORD;
			}
		} else { /* state == STATE_WORD */
			if(*c == ' ') {
				*c = '\0';
				state = STATE_WHITESPACE;
			}
		}

		c++;
	}
	
	*argc = i;

#ifdef BLOB_DEBUG
	for(i = 0; i < *argc; i++) {
		printerrprefix();
		SerialOutputString("argv[");
		SerialOutputDec(i);
		SerialOutputString("] = ");
		SerialOutputString(argv[i]);
		serial_write('\n');
	}
#endif
}




/* return the number of commands that match */
static int get_num_command_matches(char *cmdline)
{
	commandlist_t *cmd;
	int len;
	int num_matches = 0;

	len = strlen(cmdline);

	for(cmd = commands; cmd != NULL; cmd = cmd->next) {
		if(cmd->magic != COMMAND_MAGIC) {
#ifdef BLOB_DEBUG
			printerrprefix();
			SerialOutputString(__FUNCTION__ "(): Address = 0x");
			SerialOutputHex((u32)cmd);
			serial_write('\n');
#endif
			return -EMAGIC;
		}

		if(strncmp(cmd->name, cmdline, len) == 0) 
			num_matches++;
	}

	return num_matches;
}


int parse_command(char *cmdline)
{
	commandlist_t *cmd;
	int argc, num_commands, len;
	char *argv[MAX_ARGS];
	
	parse_args(cmdline, &argc, argv);

	/* only whitespace */
	if(argc == 0) 
		return 0;

	num_commands = get_num_command_matches(argv[0]);

	/* error */
	if(num_commands < 0)
		return num_commands;

	/* no command matches */
	if(num_commands == 0)
		return -ECOMMAND;

	/* ambiguous command */
	if(num_commands > 1)
		return -EAMBIGCMD;

	len = strlen(argv[0]);

	/* single command, go for it */
	for(cmd = commands; cmd != NULL; cmd = cmd->next) {
		if(cmd->magic != COMMAND_MAGIC) {
#ifdef BLOB_DEBUG
			printerrprefix();
			SerialOutputString(__FUNCTION__ "(): Address = 0x");
			SerialOutputHex((u32)cmd);
			serial_write('\n');
#endif
			
			return -EMAGIC;
		}

		if(strncmp(cmd->name, argv[0], len) == 0) {
			/* call function */
			return cmd->callback(argc, argv);
		}
	}

	return -ECOMMAND;
}




/* help command */
static int help(int argc, char *argv[])
{
	commandlist_t *cmd;

	/* help on a command? */
	if(argc >= 2) {
		for(cmd = commands; cmd != NULL; cmd = cmd->next) {
			if(strncmp(cmd->name, argv[1], 
				   MAX_COMMANDLINE_LENGTH) == 0) {
				SerialOutputString("Help for '");
				SerialOutputString(argv[1]);
				SerialOutputString("':\n\nUsage: ");
				SerialOutputString(cmd->help);
				return 0;
			}
		}

		return -EINVAL;
	}

	SerialOutputString("The following commands are supported:");

	for(cmd = commands; cmd != NULL; cmd = cmd->next) {
		SerialOutputString("\n* ");
		SerialOutputString(cmd->name);
	}

	SerialOutputString("\nUse \"help command\" to get help on a specific command\n");

	return 0;
}

static char helphelp[] = "help [command]\n"
"Get help on [command], "
"or a list of supported commands if a command is omitted.\n";

__commandlist(help, "help", helphelp);




/* display a prompt, or the standard prompt if prompt == NULL */
void DisplayPrompt(char *prompt)
{
	if(prompt == NULL) {
		SerialOutputString(PACKAGE "> ");
	} else {
		SerialOutputString(prompt);
	}
}




/* more or less like SerialInputString(), but with echo and backspace  */
int __attribute__((weak)) GetCommand(char *command, int len, int timeout)
{
	u32 startTime, currentTime;
	int c;
	int i;
	int numRead;
	int maxRead = len - 1;
	
	TimerClearOverflow();

	startTime = TimerGetTime();

	for(numRead = 0, i = 0; numRead < maxRead;) {
		/* try to get a byte from the serial port */
		while(serial_poll() != 0) {
			currentTime = TimerGetTime();

			/* check timeout value */
			if((currentTime - startTime) > 
			   (timeout * TICKS_PER_SECOND)) {
				/* timeout */
				command[i++] = '\0';
				return(numRead);
			}
		}

		c = serial_read();

		/* check for errors */
		if(c < 0) {
			command[i++] = '\0';
			serial_write('\n');
			printerror(c, "can't read command");
			return c;
		}

		if((c == '\r') || (c == '\n')) {
			command[i++] = '\0';

			/* print newline */
			serial_write('\n');
			return(numRead);
		} else if(c == '\b') { /* FIXME: is this backspace? */
			if(i > 0) {
				i--;
				numRead--;
				/* cursor one position back. */
				SerialOutputString("\b \b");
			}
		} else {
			command[i++] = c;
			numRead++;

			/* print character */
			serial_write(c);
		}
	}

	return(numRead);
}

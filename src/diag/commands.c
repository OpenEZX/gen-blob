/*
 * commands.c: commands for diag
 *
 * Copyright (C) 2001  Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 *
 * $Id: commands.c,v 1.5 2002/01/03 16:07:18 erikm Exp $
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

/* $Id: commands.c,v 1.5 2002/01/03 16:07:18 erikm Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif


#include <blob/command.h>
#include <blob/command_hist.h>
#include <blob/error.h>
#include <blob/reboot.h>
#include <blob/serial.h>
#include <blob/terminal.h>
#include <blob/time.h>
#include <blob/util.h>




__commandlist(reset_terminal, "reset", resethelp);
__commandlist(reboot, "reboot", reboothelp);




/* more or less like SerialInputString(), but with echo and backspace  */
/* unlike the version in libblob, this version has a command history */
int GetCommand(char *command, int len, int timeout)
{
	u32 startTime, currentTime;
	int c;
	int i;
	int numRead;
	int maxRead = len - 1;

	TimerClearOverflow();

	startTime = TimerGetTime();

	cmdhist_reset();

	for(numRead = 0, i = 0; numRead < maxRead;) {
		/* try to get a byte from the serial port */
		while(serial_poll() != 0) {
			currentTime = TimerGetTime();

			/* check timeout value */
			if((currentTime - startTime) > 
			   (timeout * TICKS_PER_SECOND)) {
				/* timeout */
				command[i++] = '\0';
				cmdhist_push( command );
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
			cmdhist_push( command );
			return(numRead);
		} else if(c == '\b') { /* FIXME: is this backspace? */
			if(i > 0) {
				i--;
				numRead--;
				/* cursor one position back. */
				SerialOutputString("\b \b");
			}
		} else if ( c == CMDHIST_KEY_UP ) {
			char *cmd = NULL;
			/* get cmd from history */
			if ( cmdhist_next( &cmd ) != 0 )
				continue;

			/* clear line */
			while ( numRead-- ) {
				SerialOutputString("\b \b");
			}

			/* display it */
			SerialOutputString(cmd);
			i = numRead = strlen( cmd );
			strncpy( command, cmd, MAX_COMMANDLINE_LENGTH );
		} else if ( c == CMDHIST_KEY_DN ) {
			char *cmd = NULL;
			/* get cmd from history */
			if ( cmdhist_prev( &cmd ) != 0 )
				continue;

			/* clear line */
			while ( numRead-- ) {
				SerialOutputString("\b \b");
			}

			/* display it */
			SerialOutputString(cmd);
			i = numRead = strlen( cmd );
			strncpy( command, cmd, MAX_COMMANDLINE_LENGTH );
		} else {
			command[i++] = c;
			numRead++;

			/* print character */
			serial_write(c);
		}
	}

	cmdhist_push( command );
	return(numRead);
}

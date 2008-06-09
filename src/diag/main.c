/*
 * main.c: main file for diag
 *
 * Copyright (C) 2001  Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 *
 * $Id: main.c,v 1.4 2002/01/03 16:07:18 erikm Exp $
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

#ident "$Id: main.c,v 1.4 2002/01/03 16:07:18 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/command.h>
#include <blob/error.h>
#include <blob/init.h>
#include <blob/serial.h>




int main(void)
{
	int numRead;
	char commandline[MAX_COMMANDLINE_LENGTH];
	int retval;

	/* call subsystems */
	init_subsystems();

#ifdef H3600
	serial_init(baud_115200); /* DEBUG */
#endif

	SerialOutputString("\ndiag version " VERSION " for " BOARD_NAME "\n"
			   "Copyright (C) 2001 "
			   "Stefan Eletzhofer and Erik Mouw\n");
	SerialOutputString("diag comes with ABSOLUTELY NO WARRANTY; "
			   "read the GNU GPL for details.\n");
	SerialOutputString("This is free software, and you are welcome "
			   "to redistribute it\n");
	SerialOutputString("under certain conditions; "
			   "read the GNU GPL for details.\n");

	/* the command loop. endless, of course */
	for(;;) {
		DisplayPrompt("diag> ");

		/* wait 10 minutes for a command */
		numRead = GetCommand(commandline, MAX_COMMANDLINE_LENGTH, 600);

		if(numRead > 0) {
			if((retval = parse_command(commandline)) < 0 )
				printerror(retval, NULL);
		}
	}

	return 0;
}

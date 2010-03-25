/*
 * reboot.c: Reboot board
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * $Id: reboot.c,v 1.2 2002/01/06 15:46:17 erikm Exp $
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

/* $Id: reboot.c,v 1.2 2002/01/06 15:46:17 erikm Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/serial.h>




char reboothelp[] = "reboot\n"
"Reboot system\n";


int reboot(int argc, char *argv[])
{
	void (*reset)(void) = (void (*)(void))0;

	SerialOutputString("Rebooting...\n\n");
	serial_flush_output();
	
	reset();

	/* never reached, but anyway... */
	return 0;
}

/*
 * reboot.c: Reboot board
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * $Id: reboot.c,v 1.7 2002/01/06 15:46:17 erikm Exp $
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

#ident "$Id: reboot.c,v 1.7 2002/01/06 15:46:17 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/command.h>
#include <blob/errno.h>
#include <blob/error.h>
#include <blob/init.h>
#include <blob/main.h>
#include <blob/serial.h>

static int reblob(int argc, char *argv[])
{
	void (*blob) (void) = (void (*)(void))BLOB_RAM_BASE;

	/* sanity check */
	if (blob_status.blobType == fromFlash) {
		printerrprefix();
		SerialOutputString("blob not downloaded\n");
		return -EINVAL;
	}

	SerialOutputString("Restarting blob from RAM...\n\n");

	serial_flush_output();

	/* disable subsystems that want to be disabled */
	exit_subsystems();

	blob();

	/* never reached, but anyway... */
	return 0;
}

static char reblobhelp[] = "reblob\n" "Restart blob from RAM\n";

__commandlist(reblob, "reblob", reblobhelp);

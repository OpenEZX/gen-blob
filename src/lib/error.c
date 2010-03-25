/*
 * error.c: error handling functions
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * $Id: error.c,v 1.6 2002/01/03 16:07:18 erikm Exp $
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

/* $Id: error.c,v 1.6 2002/01/03 16:07:18 erikm Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/errno.h>
#include <blob/error.h>
#include <blob/serial.h>
#include <blob/types.h>


static char *error_strings[] = {
	"no error",			/* ENOERROR */
	"invalid argument",		/* EINVAL */
	"not enough parameters",	/* ENOPARAMS */
	"magic value failed",		/* EMAGIC */
	"invalid command",		/* ECOMMAND */
	"not a number",			/* ENAN */
	"alignment error",		/* EALIGN */
	"out of range",			/* ERANGE */
	"timeout exceeded",		/* ETIMEOUT */
	"short file",			/* ETOOSHORT */
	"long file",			/* ETOOLONG */
	"ambiguous command",		/* EAMBIGCMD */
	"can't erase flash block",	/* EFLASHERASE */
	"flash program error",		/* EFLASHPGM */
	"serial port error",		/* ESERIAL */
};


static char *unknown = "unknown error";
static char *errprefix = "*** ";


char *strerror(int errnum)
{
	int num = sizeof(error_strings) / sizeof(char*);

	/* make positive if it is negative */
	if(errnum < 0)
		errnum = -errnum;

	if(errnum >= num)
		return unknown;
	else
		return error_strings[errnum];
}


void printerrprefix(void)
{
	SerialOutputString(errprefix);
}


void printerror(int errnum, char *s)
{
	printerrprefix();
	SerialOutputString(strerror(errnum));

	if(s != NULL) {
		SerialOutputString(": ");
		SerialOutputString(s);
	}

	serial_write('\n');
}

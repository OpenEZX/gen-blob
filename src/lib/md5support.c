/*
 * md5support.c: MD5 support function
 *
 * Copyright (C) 2001  Hewlett-Packard Company
 * Written by Christopher Hoover <ch@hpl.hp.com>
 * Minor modifications by Erik Mouw
 *
 * $Id: md5support.c,v 1.1 2002/01/06 18:59:40 erikm Exp $
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

/* $Id: md5support.c,v 1.1 2002/01/06 18:59:40 erikm Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/md5support.h>
#include <blob/serial.h>
#include <blob/types.h>




/* takes an u32 digest[4] and prints the ASCII MD5 digest. returns
 * number of printed characters on success, or negative error number
 * on failure
 */
int print_md5_digest(const u32 *digest)
{
	int i, c;
	const unsigned char *d = (unsigned char *) digest;

	/* digest is always in little endian order */
	for(i = 0; i < 16; i++) {
		c = (d[i] >> 4) & 0x0f;
		if(c > 9)
			c += ('a' - 10);
		else
			c += '0';

		serial_write(c);
		
		c = d[i] & 0x0f;
		if(c > 9)
			c += ('a' - 10);
		else
			c += '0';

		serial_write(c);
	}

	return i;
}

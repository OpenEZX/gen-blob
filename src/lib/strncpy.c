/*
 * strncpy.c: copy string
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * $Id: strncpy.c,v 1.2 2002/01/06 17:04:36 erikm Exp $
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

/* $Id: strncpy.c,v 1.2 2002/01/06 17:04:36 erikm Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/util.h>




char *strncpy(char *dest, const char *src, size_t n)
{
	while(n > 0) {
		n--;

		if((*dest++ = *src++) == '\0')
			break;
	}

	return dest;
}




/* small variation on strncpy(): null-terminate the destination
 * string
 */
char *strlcpy(char *dest, const char *src, size_t n)
{
	strncpy(dest, src, n);

	if(n > 0)
		dest[n-1] = '\0';

	return dest;
}

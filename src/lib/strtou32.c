/*
 * strtou32.c: convert string to u32
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * $Id: strtou32.c,v 1.1 2001/10/07 22:58:56 erikm Exp $
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

/* $Id: strtou32.c,v 1.1 2001/10/07 22:58:56 erikm Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/types.h>
#include <blob/util.h>




/* test for a digit. return value if digit or -1 otherwise */
static int digitvalue(char isdigit)
{
	if (isdigit >= '0' && isdigit <= '9' )
		return isdigit - '0';
	else
		return -1;
}




/* test for a hexidecimal digit. return value if digit or -1 otherwise */
static int xdigitvalue(char isdigit)
{
	if (isdigit >= '0' && isdigit <= '9' )
		return isdigit - '0';
	if (isdigit >= 'a' && isdigit <= 'f')
		return 10 + isdigit - 'a';
	return -1;
}




/* convert a string to an u32 value. if the string starts with 0x, it
 * is a hexidecimal string, otherwise we treat is as decimal. returns
 * the converted value on success, or -1 on failure. no, we don't care
 * about overflows if the string is too long.
 */
int strtou32(const char *str, u32 *value)
{
	int i;

	*value = 0;

	if(strncmp(str, "0x", 2) == 0) {
		/* hexadecimal mode */
		str += 2;
		
		while(*str != '\0') {
			if((i = xdigitvalue(*str)) < 0)
				return -1;
			
			*value = (*value << 4) | (u32)i;

			str++;
		}
	} else {
		/* decimal mode */
		while(*str != '\0') {
			if((i = digitvalue(*str)) < 0)
				return -1;
			
			*value = (*value * 10) + (u32)i;

			str++;
		}
	}

	return 0;
}

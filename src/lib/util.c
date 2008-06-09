/*-------------------------------------------------------------------------
 * Filename:      util.c
 * Version:       $Id: util.c,v 1.4 2002/01/03 16:07:18 erikm Exp $
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   Simple utility functions for blob
 * Created at:    Wed Aug 25 21:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sun Oct  3 21:10:43 1999
 *-----------------------------------------------------------------------*/
/*
 * util.c: Simple utility functions for blob
 *
 * Copyright (C) 1999  Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
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

#ident "$Id: util.c,v 1.4 2002/01/03 16:07:18 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/types.h>
#include <blob/util.h>
#include <blob/serial.h>




void MyMemCpy(u32 *dest, const u32 *src, int numWords) 
{
#ifdef BLOB_DEBUG
	SerialOutputString("\n### Now copying 0x");
	SerialOutputHex(numWords);
	SerialOutputString(" words from 0x");
	SerialOutputHex((int)src);
	SerialOutputString(" to 0x");
	SerialOutputHex((int)dest);
	serial_write('\n');
#endif

	while(numWords--) {
		*dest++ = *src++;
	}

#ifdef BLOB_DEBUG
	SerialOutputString(" done\n");
#endif
} /* MyMemCpy */




void MyMemCpyChar(char *dest, const char *src, int numBytes)
{
	char *destLim = dest + numBytes;
	
	while(dest < destLim)
		*dest++ = *src++;
} /* MyMemCpyChar */




void MyMemSet(u32 *dest, const u32 wordToWrite, int numWords)
{
	u32 *limit = dest + numWords;
	
	while(dest < limit)
		*dest++ = wordToWrite;
} /* MyMemSet */

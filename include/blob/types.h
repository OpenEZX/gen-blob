/*-------------------------------------------------------------------------
 * Filename:      types.h
 * Version:       $Id: types.h,v 1.1.1.1 2002/07/17 00:32:33 cvs Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Define some handy types for the blob
 * Created at:    Tue Aug 24 19:04:22 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Tue Sep 28 23:45:06 1999
 *-----------------------------------------------------------------------*/
/*
 * types.h: Some handy types and macros for blob
 *
 * Copyright (C) 1999  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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

#ident "$Id: types.h,v 1.1.1.1 2002/07/17 00:32:33 cvs Exp $"

#ifndef BLOB_TYPES_H
#define BLOB_TYPES_H
#include <asm/byteorder.h>

typedef unsigned char		uchar;
typedef volatile unsigned long	vu_long;
typedef volatile unsigned short vu_short;
typedef volatile unsigned char	vu_char;
typedef uint64_t lbaint_t;

typedef signed char s8;
typedef unsigned char u8;

typedef signed short s16;
typedef unsigned short u16;

typedef signed int s32;
typedef unsigned int u32;

typedef enum
{
    FALSE,
    TRUE
}BOOL;
typedef BOOL BOOLEAN;

/* number of nibbles in a word */
#define NIBBLES_PER_WORD (8)

#ifndef NULL
#define NULL 0
#endif

#endif

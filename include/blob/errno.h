/*
 * errno.h: error numbers for blob
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * $Id: errno.h,v 1.4 2002/01/02 01:21:41 erikm Exp $
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

#ident "$Id: errno.h,v 1.4 2002/01/02 01:21:41 erikm Exp $"

#ifndef BLOB_ERRNO_H
#define BLOB_ERRNO_H


#define ENOERROR	0	/* no error at all */
#define EINVAL		1	/* invalid argument */
#define ENOPARAMS	2	/* not enough parameters */
#define EMAGIC		3	/* magic value failed */
#define ECOMMAND	4	/* invalid command */
#define ENAN		5	/* not a number */
#define EALIGN		6	/* addres not aligned */
#define ERANGE		7	/* out of range */
#define ETIMEOUT	8	/* timeout exceeded */
#define ETOOSHORT	9	/* short file */
#define ETOOLONG	10	/* long file */
#define EAMBIGCMD	11	/* ambiguous command */
#define EFLASHERASE	12	/* can't erase flash block */
#define EFLASHPGM	13	/* flash program error */
#define ESERIAL		14	/* serial port error */


#endif

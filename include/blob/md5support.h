/*
 * md5support.h: MD5 support function
 *
 * Copyright (C) 2001  Hewlett-Packard Company
 * Written by Christopher Hoover <ch@hpl.hp.com>
 * Minor modifications by Erik Mouw
 *
 * $Id: md5support.h,v 1.1 2002/01/06 18:59:40 erikm Exp $
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

/* $Id: md5support.h,v 1.1 2002/01/06 18:59:40 erikm Exp $ */

#ifndef BLOB_MD5SUPPORT_H
#define BLOB_MD5SUPPORT_H


#include <blob/types.h>


int print_md5_digest(const u32 *digest);


#endif

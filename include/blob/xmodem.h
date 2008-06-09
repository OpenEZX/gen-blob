/*-------------------------------------------------------------------------
 * Filename:      xmodem.h
 * Version:       $Id: xmodem.h,v 1.1 2002/01/05 20:21:49 erikm Exp $
 * Copyright:     Copyright (C) 2001, Hewlett-Packard Corporation
 * Author:        Christopher Hoover <ch@hpl.hp.com>
 * Description:   Header file for xmodem.c
 * Created at:    Thu Dec 20 02:13:44 PST 2001
 *-----------------------------------------------------------------------*/
/*
 * xmodem.h: xmodem functionality for uploading of kernels and 
 *            the like
 *
 * Copyright (C) 2001, Hewlett-Packard Corporation
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

#ident "$Id: xmodem.h,v 1.1 2002/01/05 20:21:49 erikm Exp $"

#ifndef BLOB_XMODEM_H
#define BLOB_XMODEM_H

int XModemReceive(char *bufBase, int bufLen);

#endif

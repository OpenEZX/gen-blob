/*-------------------------------------------------------------------------
 * Filename:      uucodec.h
 * Version:       $Id: uucodec.h,v 1.1 2001/10/07 15:17:49 erikm Exp $
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   Header file for uucodec.c
 * Created at:    Wed Aug 25 20:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Tue Sep 28 23:46:37 1999
 *-----------------------------------------------------------------------*/
/*
 * uucodec.h: uu{en|de}code functiuonality for uploading of kernels and 
 *            the like
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

/* $Id: uucodec.h,v 1.1 2001/10/07 15:17:49 erikm Exp $ */

#ifndef BLOB_UUCODEC_H
#define BLOB_UUCODEC_H

int UUDecode(char *bufBase, int bufLen);
void UUEncode(char *bufBase, int bufLen);

#endif

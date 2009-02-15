/*-------------------------------------------------------------------------
 * Filename:      main.h
 * Version:       $Id: main.h,v 1.6 2002/01/06 18:59:40 erikm Exp $
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   Header file for main.c
 * Created at:    Mon Aug 23 20:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sun Oct  3 21:44:01 1999
 *-----------------------------------------------------------------------*/
/*
 * main.h: Header file for main.c
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

#ident "$Id: main.h,v 1.6 2002/01/06 18:59:40 erikm Exp $"

#ifndef BLOB_MAIN_H
#define BLOB_MAIN_H

#include <blob/types.h>
#include <blob/serial.h>
#include <asm/setup.h>


typedef enum {
	fromFlash = 0,
	fromDownload = 1
} block_source_t;


typedef struct {
	int kernelSize;
	block_source_t kernelType;
	u32 kernel_md5_digest[4];

	int paramSize;
	block_source_t paramType;
	u32 param_md5_digest[4];

	int ramdiskSize;
	block_source_t ramdiskType;
	u32 ramdisk_md5_digest[4];

	int blobSize;
	block_source_t blobType;
	u32 blob_md5_digest[4];

	serial_baud_t downloadSpeed;
	serial_baud_t terminalSpeed;
	
	int load_ramdisk;
	int boot_delay;
	
	char cmdline[COMMAND_LINE_SIZE];
	
} blob_status_t;


extern blob_status_t blob_status;


#endif

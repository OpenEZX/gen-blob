/*-------------------------------------------------------------------------
 * Filename:      flash.h
 * Version:       $Id: flash.h,v 1.9 2001/12/31 00:28:53 erikm Exp $
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   Flash I/O functions for the blob loader
 * Created at:    Mon Aug 23 20:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sun Oct  3 21:35:12 1999
 *-----------------------------------------------------------------------*/
/*
 * flash.c: Flash I/O functions for the blob bootloader
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

/* $Id: flash.h,v 1.9 2001/12/31 00:28:53 erikm Exp $ */

#ifndef BLOB_FLASH_H
#define BLOB_FLASH_H

#include <blob/types.h>

typedef struct {
	u32 size;
	u32 num;
	int lockable;
} flash_descriptor_t;


typedef int (*flash_erase_func_t)(u32 *);
typedef int (*flash_write_func_t)(u32 *, const u32 *);
typedef int (*flash_lock_block_func_t)(u32 *);
typedef int (*flash_unlock_block_func_t)(u32 *);
typedef int (*flash_query_block_lock_func_t)(u32 *);
typedef int (*flash_enable_vpp_func_t)(void);
typedef int (*flash_disable_vpp_func_t)(void);


typedef struct {
	/* the following functions should be implemented by all flash
	 * drivers*/
	flash_erase_func_t erase;
	flash_write_func_t write;

	flash_lock_block_func_t lock_block;
	flash_unlock_block_func_t unlock_block;
	flash_query_block_lock_func_t query_block_lock;

	/* this is platform specific, so it should be implemented by
	 * the platform specific code. a flash driver can leave these
	 * functions unimplemented, blob will safely work around it */
	flash_enable_vpp_func_t enable_vpp;
	flash_disable_vpp_func_t disable_vpp;
} flash_driver_t;


/* implemented flash drivers */
extern flash_driver_t amd32_flash_driver;
extern flash_driver_t intel16_flash_driver;
extern flash_driver_t intel32_flash_driver;
extern flash_driver_t null_flash_driver;


/* should be filled out by the architecture dependent files */
extern flash_descriptor_t *flash_descriptors;
extern flash_driver_t *flash_driver;


/* flash data mangle functions */
u32 data_from_flash(u32 what);
u32 data_to_flash(u32 what);


/* exported functions */
int flash_erase_region(u32 *start, u32 nwords);
int flash_write_region(u16 *dst,  u16 *src, u32 nwords);
int flash_lock_region(u32 *start, u32 nwords);
int flash_unlock_region(u32 *start, u32 nwords);
int flash_query_region(u32 *start, u32 nwords);


#endif

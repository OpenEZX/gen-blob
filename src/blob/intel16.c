/*
 * intel16.c: Intel 16 bit flash driver
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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

#ident "$Id: intel16.c,v 1.4 2002/01/03 16:07:17 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/errno.h>
#include <blob/flash.h>
#include <blob/util.h>
#include <blob/serial.h>


/* flash commands for a single 16 bit intel flash chip */
#define READ_ARRAY		0x000000FF
#define ERASE_SETUP		0x00000020
#define ERASE_CONFIRM		0x000000D0
#define	PGM_SETUP		0x00000040
#define	STATUS_READ		0x00000070
#define	STATUS_CLEAR		0x00000050
#define STATUS_BUSY		0x00000080
#define STATUS_ERASE_ERR	0x00000020
#define STATUS_PGM_ERR		0x00000010

#define CONFIG_SETUP		0x00000060
#define LOCK_SECTOR		0x00000001
#define UNLOCK_SECTOR		0x000000D0
#define READ_CONFIG		0x00000090
#define LOCK_STATUS_LOCKED	0x00000001
#define LOCK_STATUS_LOCKEDDOWN	0x00000002




static int do_erase(u16 *addr)
{
	u16 result;

	*addr = STATUS_CLEAR;
	barrier();

	/* prepare for erase */
	*addr = ERASE_SETUP;
	barrier();

	/* erase block */
	*addr = ERASE_CONFIRM;
	barrier();

	/* status check */
	do {
		*addr = STATUS_READ;
		barrier();
		result = *addr;
		barrier();
	} while((~result & STATUS_BUSY) != 0);

	/* put flash back into Read Array mode */
	*addr = READ_ARRAY;
	barrier();

	if((result & STATUS_ERASE_ERR) != 0) {
#ifdef BLOB_DEBUG
		SerialOutputString(__FUNCTION__ " failed, result=0x");
		SerialOutputHex(result);
		SerialOutputString(", addr=0x");
		SerialOutputHex((u32) addr);
		serial_write('\n');
#endif
		return -EFLASHERASE;
	}

	return 0;
}




static int do_write(u16 *dst, const u16* src)
{
	u16 result;

	*dst = STATUS_CLEAR;
	barrier();

	/* setup flash for writing */
	*dst = PGM_SETUP;
	barrier();

	/* write data */
	*dst = *src;
	barrier();

	/* status check */
	do {
		*dst = STATUS_READ;
		barrier();

		result = *dst;
		barrier();
	} while((~result & STATUS_BUSY) != 0);

	/* put flash back into Read Array mode */
	*dst = READ_ARRAY;
	barrier();

	if(((result & STATUS_PGM_ERR) != 0) || (*dst != *src)) {
#ifdef BLOB_DEBUG
		SerialOutputString(__FUNCTION__ " failed, result=0x");
		SerialOutputHex(result);
		SerialOutputString(", dst=0x");
		SerialOutputHex((u32) dst);
		SerialOutputString(", *dst=0x");
		SerialOutputHex(*dst);
		SerialOutputString(", *src=0x");
		SerialOutputHex(*src);
		serial_write('\n');
#endif
		return -EFLASHPGM;
	}
	
	return 0;
}




/* erases a flash block at the given address */
/* we have to break this up in two erases at 16 bit aligned addresses
 * (if necessary)
 */
static int flash_erase_intel16(u32 *addr)
{
	int result;
	u16 *addr16 = (u16*)addr;

	/* erase first block */
	result = do_erase(addr16);
	if(result != 0)
		return result;

	addr16++;
	/* if the second address is not erased, also erase it */
	if(*addr16 != 0xffff)
		result = do_erase(addr16);

	if(result != 0)
		return result;
	
	return 0;
}




/* write a flash block at a given location */
/* this has to be broken up into two consectutive 16 bit writes */
static int flash_write_intel16(u32 *dst, const u32* src)
{
	u16 *dst16 = (u16 *)dst;
	u16 *src16 = (u16 *)src;
	int result;
	
	result = do_write(dst16, src16);
	if(result != 0)
		return result;

	dst16++;
	src16++;
	
	result = do_write(dst16, src16);
	if(result != 0)
		return result;

	return 0;
}




static int flash_lock_block_intel16(u32 *blockStart)
{
	u16 *p = (u16 *) blockStart;
	u16 result;

	*p = STATUS_CLEAR;
	barrier();

	*p = CONFIG_SETUP;
	barrier();
	*p = LOCK_SECTOR;
	barrier();

	/* status check */
	*p = STATUS_READ;
	barrier();

	result = *p;
	barrier();

	*p = READ_ARRAY;
	barrier();

	if ((result & STATUS_PGM_ERR) != 0) {
#ifdef BLOB_DEBUG
		SerialOutputString(__FUNCTION__ " failed at 0x");
		SerialOutputHex((u32) blockStart);
		SerialOutputString(", status=0x");
		SerialOutputHex((u32) result);
		serial_write('\n');
#endif
		return -EFLASHPGM;
	}

	return 0;
}




static int flash_unlock_block_intel16(u32 *blockStart)
{
	u16 *p = (u16 *) blockStart;
	u16 result;

	*p = STATUS_CLEAR;
	barrier();

	*p = CONFIG_SETUP;
	barrier();
	*p = UNLOCK_SECTOR;
	barrier();

	/* status check */
	*p = STATUS_READ;
	barrier();

	result = *p;
	barrier();

	*p = READ_ARRAY;
	barrier();

	if ((result & STATUS_PGM_ERR) != 0) {
#ifdef BLOB_DEBUG
		SerialOutputString(__FUNCTION__ " failed at 0x");
		SerialOutputHex((u32) blockStart);
		SerialOutputString(", status=0x");
		SerialOutputHex((u32) result);
		serial_write('\n');
#endif
		return -EFLASHPGM;
	}

	return 0;
}




static int flash_query_block_lock_intel16(u32 *blockStart)
{
	u16 *p = (u16 *) blockStart;
	u16 result;

	*p = READ_CONFIG;
	barrier();
	result = *(p + 2);
	barrier();

	*blockStart = READ_ARRAY;
	barrier();

#ifdef BLOB_DEBUG
	SerialOutputString(__FUNCTION__ ": status of block starting at 0x");
	SerialOutputHex((u32) blockStart);
	SerialOutputString(": 0x");
	SerialOutputHex((u32) result);
	serial_write('\n');
#endif

	return result & (LOCK_STATUS_LOCKED | LOCK_STATUS_LOCKEDDOWN);
}

/* flash driver structure */
flash_driver_t intel16_flash_driver = {
	erase:			flash_erase_intel16,
	write:			flash_write_intel16,
	lock_block:		flash_lock_block_intel16,
	unlock_block:		flash_unlock_block_intel16,
	query_block_lock:	flash_query_block_lock_intel16
};

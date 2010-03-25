/*
 * intel32.c: Intel 32 bit (2x 16 bit) flash driver
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

/* $Id: intel32.c,v 1.4 2002/01/03 16:07:18 erikm Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/errno.h>
#include <blob/flash.h>
#include <blob/util.h>
#include <blob/serial.h>

/* flash commands for two 16 bit intel flash chips */
#define READ_ARRAY		0x00FF00FF
#define ERASE_SETUP		0x00200020
#define ERASE_CONFIRM		0x00D000D0
#define	PGM_SETUP		0x00400040
#define	STATUS_READ		0x00700070
#define	STATUS_CLEAR		0x00500050
#define STATUS_BUSY		0x00800080
#define STATUS_ERASE_ERR	0x00200020
#define STATUS_PGM_ERR		0x00100010

#define CONFIG_SETUP		0x00600060
#define LOCK_SECTOR		0x00010001
#define UNLOCK_SECTOR		0x00D000D0
#define READ_CONFIG		0x00900090
#define LOCK_STATUS_LOCKED	0x00010001
#define LOCK_STATUS_LOCKEDDOWN	0x00020002

/* erases a flash block at the given address */
static int flash_erase_intel32(u32 * addr)
{
	u32 result;

	*addr = data_to_flash(STATUS_CLEAR);
	barrier();

	/* prepare for erase */
	*addr = data_to_flash(ERASE_SETUP);
	barrier();

	/* erase block */
	*addr = data_to_flash(ERASE_CONFIRM);
	barrier();

	/* status check */
	do {
		*addr = data_to_flash(STATUS_READ);
		barrier();
		result = data_from_flash(*addr);
		barrier();
	} while ((~result & STATUS_BUSY) != 0);

	/* put flash back into Read Array mode */
	*addr = data_to_flash(READ_ARRAY);
	barrier();

	if ((result & STATUS_ERASE_ERR) != 0) {
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

/* write a flash block at a given location */
static int flash_write_intel32(u32 * dst, const u32 * src)
{
	u32 result;

	*dst = data_to_flash(STATUS_CLEAR);
	barrier();

	/* setup flash for writing */
	*dst = data_to_flash(PGM_SETUP);
	barrier();

	/* write data */
	*dst = *src;
	barrier();

	/* status check */
	do {
		*dst = data_to_flash(STATUS_READ);
		barrier();

		result = data_from_flash(*dst);
		barrier();
	} while ((~result & STATUS_BUSY) != 0);

	/* put flash back into Read Array mode */
	*dst = data_to_flash(READ_ARRAY);
	barrier();

	if (((result & STATUS_PGM_ERR) != 0) || (*dst != *src)) {
#ifdef BLOB_DEBUG
		SerialOutputString(__FUNCTION__ "failed, result=0x");
		SerialOutputHex(result);
		SerialOutputString(", addr=0x");
		SerialOutputHex((u32) dst);
		serial_write('\n');
#endif
		return -EFLASHPGM;
	}

	return 0;
}

static int flash_lock_block_intel32(u32 * blockStart)
{
	u32 result;

	*blockStart = data_to_flash(STATUS_CLEAR);
	barrier();

	*blockStart = data_to_flash(CONFIG_SETUP);
	barrier();
	*blockStart = data_to_flash(LOCK_SECTOR);
	barrier();

	*blockStart = data_to_flash(READ_ARRAY);
	barrier();

	/* status check */
	*blockStart = data_to_flash(STATUS_READ);
	barrier();

	result = data_from_flash(*blockStart);
	barrier();

	*blockStart = data_to_flash(READ_ARRAY);
	barrier();

	if ((result & STATUS_PGM_ERR) != 0) {
#ifdef FLASH_DEBUG
		SerialOutputString(__FUNCTION__ " failed at 0x");
		SerialOutputHex((u32) blockStart);
		SerialOutputString(", status=0x");
		SerialOutputHex((u32) result);
		SerialOutputByte('\n');
#endif
		return -EFLASHPGM;
	}

	return 0;
}

static int flash_unlock_block_intel32(u32 * blockStart)
{
	u32 result;

	*blockStart = data_to_flash(STATUS_CLEAR);
	barrier();

	*blockStart = data_to_flash(CONFIG_SETUP);
	barrier();
	*blockStart = data_to_flash(UNLOCK_SECTOR);
	barrier();

	*blockStart = data_to_flash(STATUS_READ);
	barrier();

	result = data_from_flash(*blockStart);
	barrier();

	*blockStart = data_to_flash(READ_ARRAY);
	barrier();

	if ((result & STATUS_PGM_ERR) != 0) {
#ifdef FLASH_DEBUG
		SerialOutputString(__FUNCTION__ " failed at 0x");
		SerialOutputHex((u32) blockStart);
		SerialOutputString(", status=0x");
		SerialOutputHex((u32) result);
		SerialOutputByte('\n');
#endif
		return -EFLASHPGM;
	}

	return 0;
}

static int flash_query_block_lock_intel32(u32 * blockStart)
{
	u32 result;

	*blockStart = data_to_flash(READ_CONFIG);
	barrier();
	result = data_from_flash(*(blockStart + 2));
	barrier();

	*blockStart = data_to_flash(READ_ARRAY);
	barrier();

#ifdef FLASH_DEBUG
	SerialOutputString(__FUNCTION__ ": status of block starting at 0x");
	SerialOutputHex((u32) blockStart);
	SerialOutputString(": 0x");
	SerialOutputHex((u32) result);
	SerialOutputByte('\n');
#endif

	return result & (LOCK_STATUS_LOCKED | LOCK_STATUS_LOCKEDDOWN);
}

/* flash driver structure */
flash_driver_t intel32_flash_driver = {
      erase:flash_erase_intel32,
      write:flash_write_intel32,
      lock_block:flash_lock_block_intel32,
      unlock_block:flash_unlock_block_intel32,
      query_block_lock:flash_query_block_lock_intel32
};

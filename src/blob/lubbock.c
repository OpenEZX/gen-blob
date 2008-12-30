/*
 * src/blob/lubbock.c
 *
 * lubbock specific stuff (ezx platform base)
 *
 * Copyright (C) 2005 Motorola Inc.
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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
 * #ident "$Id: assabet.c,v 1.6 2002/01/03 16:07:17 erikm Exp $"
 * 2005-Jun-14 - (Motorola) modified for EZX platform
 * 
 */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/flash.h>
#include <blob/init.h>
#include <blob/serial.h>
#include <blob/command.h>
#include <blob/lcd_ezx.h>
extern int strtou32(const char *str, u32 * value);
#if  ( defined(CPU_pxa250) || defined(CPU_pxa260) )

#define LUB_FLASH_DRIVER (&intel32_flash_driver)
static flash_descriptor_t lubbock_flash_descriptors[] = {
	{
	      size:2 * 128 * 1024,
	      num:128,
      lockable:1},
	{
	 /* NULL block */
	 },
};

#elif ( defined(CPU_pxa210) || defined(CPU_pxa261) )

#define LUB_FLASH_DRIVER (&intel16_flash_driver)
static flash_descriptor_t lubbock_flash_descriptors[] = {
	{
	      size:1 * 128 * 1024,
	      num:128,
      lockable:1},
	{
	 /* NULL block */
	 },
};

#elif ( defined(CPU_pxa262) )	/* Dalhart B0 */

#define LUB_FLASH_DRIVER (&intel16_flash_driver)

static flash_descriptor_t lubbock_flash_descriptors[] = {
	{
	      size:1 * 128 * 1024,
	      num:512,
      lockable:1},
	{
	 /* NULL block */
	 },
};

#endif

void init_lubbock_flash_driver(void)
{
	flash_descriptors = lubbock_flash_descriptors;

	flash_driver = LUB_FLASH_DRIVER;
}

__initlist(init_lubbock_flash_driver, INIT_LEVEL_DRIVER_SELECTION);

static void lubbock_init_hardware(void)
{
	/* select serial driver */
	serial_driver = &pxa_serial_driver;
}

__initlist(lubbock_init_hardware, INIT_LEVEL_DRIVER_SELECTION);

/*********************************************************************
 * cmd_flash_write
 *
 * AUTOR:	SELETZ
 * REVISED:
 *
 * Command wrapper for low-level flash write access
 *
 */
static int cmd_flash_write(int argc, char *argv[])
{
	int ret = 0;
	u32 src = 0L;
	u32 dest = 0L;
	u32 len = 0L;
	u32 tlen = 0L;

	if (argc < 4) {
		ret = -1;
		goto DONE;
	}

	ret = strtou32(argv[1], &src);
	if (ret < 0) {
		ret = -1;
		goto DONE;
	}

	ret = strtou32(argv[2], &dest);
	if (ret < 0) {
		ret = -1;
		goto DONE;
	}

	ret = strtou32(argv[3], &len);
	if (ret < 0) {
		ret = -1;
		goto DONE;
	}

	if (len & (0x3)) {
		len = (len >> 2) + 1;
	} else {
		len = len >> 2;
	}

	// K3 flash 
	flash_unlock_region((u32 *) dest, len);

	printlcd("start erase\n");
	ret = flash_erase_region((u32 *) dest, len);
	if (ret)
		printlcd("erase error!!!\n");
	else
		printlcd("erase done\n");

	printlcd("start program\n");
	tlen = (len << 2);
	ret = flash_write_region((u16 *) dest, (u16 *) src, tlen);
	if (ret)
		printlcd("program error!!!\n");
	else
		printlcd("program done\n");

	// K3 flash 
	flash_lock_region((u32 *) dest, len);

      DONE:
	return ret;
}
static char flashwritehelp[] = "fwrite srcadr destadr size(bytes)\n"
    "flash a memory region\n";
__commandlist(cmd_flash_write, "fwrite", flashwritehelp);

/*********************************************************************
 * cmd_flash_erase
 *
 * AUTOR:	SELETZ
 * REVISED:
 *
 * Command wrapper for low-level flash erasing
 *
 */
static int cmd_flash_erase(int argc, char *argv[])
{
	int ret = 0;
	u32 dest = 0L;
	u32 len = 0L;

	if (argc < 3) {
		ret = -1;
		goto DONE;
	}

	ret = strtou32(argv[1], &dest);
	if (ret < 0) {
		ret = -1;
		goto DONE;
	}

	ret = strtou32(argv[2], &len);
	if (ret < 0) {
		ret = -1;
		goto DONE;
	}

	if (len & (0x3)) {
		len = (len >> 2) + 1;
	} else {
		len = len >> 2;
	}

	// K3 flash 
	flash_unlock_region((u32 *) dest, len);

	ret = flash_erase_region((u32 *) dest, len);

	// K3 flash
	flash_lock_region((u32 *) dest, len);

      DONE:
	return ret;
}

static char flasherasehelp[] = "ferase adr size(bytes)\n"
    "erase a flash region\n";
__commandlist(cmd_flash_erase, "ferase", flasherasehelp);

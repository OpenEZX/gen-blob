/*
 * src/blob/boot_modes.c
 * 
 * support more boot modes in blob
 *
 * Copyright (C) 2005  Motorola
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
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
 * 2005-Jun-06 - (Motorola) init draft
 *
 */
#ifdef HAVE_CONFIG_H
#include <blob/config.h>
#endif

#include <blob/types.h>
#include <blob/pxa.h>
#include <blob/handshake.h>
#include <blob/flash.h>
#include <blob/arch/lubbock.h>
#include <blob/boot_modes.h>
#include "usb_ctl_bvd.h"
#include <blob/lcd_ezx.h>
#include <blob/mmu.h>
#include "pcap.h"
#include <blob/log.h>
#include <common.h>
#include <part.h>
#include <mmc.h>

/* check BP_WDI flag */
#define bp_flag_not_set()	(*(unsigned long *)(BPSIG_ADDR) == NO_FLAG)
#define bp_flag_is_set()	(*(unsigned long *)(BPSIG_ADDR) == WDI_FLAG)

/* keypad macros */

//for P2 board //if( KPAS == (0x280000f4) ) 
//for 64MB dataflash P1 board 
//for P1 work -- if( KPAS == (0x04000022) )
#if defined(HAINAN) || defined(SUMATRA)
#define FLASH_KEY      0x04000025
#define VOL_PLUS_KEY   0x04000042
#define VOL_MINUS_KEY  0x04000002
#define VOL_ENTER_KEY  0x04000025
#define KEY_DEBOUNCE 40000	/* debounce 40ms */
#else

#define FLASH_KEY 0x04000013
#define KEY_DEBOUNCE 40000	/* debounce 40ms */
#endif

static void naked_usb_flash(u32 data);	/* a tool function just start usb flash loop */
extern int is_key_press_down(u32 key_val, u32 Deb_val);
extern void Delay(int uSec);
extern void udc_int_hndlr(int, void *);
extern int usbctl_init(void);
extern void udc_disable(void);
extern void USB_gpio_init(void);
extern struct mem_area io_map[];
/* structure note:
 This module includes all non-boot-linux boot modes in blob,
 such as flash, pass-through mode...etc*/

BOOL check_flash_flag(void)
{
	return (*(unsigned long *)(FLAG_ADDR) == REFLASH_FLAG);
}

BOOL check_valid_code(void)
{
	BOOL rev = TRUE;

#if 0				// wait for ZQ
	u32 *pbarkercode = BARKER_CODE_ADDRESS;

	if ((*pbarkercode) != BARKER_CODE_VALID_VAL) {
		rev = FALSE;
	}
#endif
	return rev;
}

#include <menu.h>
#define CONFIG_FILE "/boot/menu.lst"
#define LINE_HEIGHT 14

static menu_entry_t *go_menu_entry = NULL;
void show_menu(menu_t * menu)
{
	int i;
	menu_entry_t *entry;
	clrScreen();
	printf("===============================\n");
	int y = menu->curr_entry * (LINE_HEIGHT + 4) + 20;
	for (i = 0; i < LINE_HEIGHT; i++)
		drawline(i + y, 0, i + y, 230);

	entry = menu->entry;
	for (i = 0; i < menu->num; i++) {
		if (entry == NULL)
			break;
		printlcd(" ");
		if (menu->curr_entry == entry->id) {
			set_transparence(1);
			printlcd_rev(entry->title);
			set_transparence(0);
			go_menu_entry = entry;
		} else {
			printlcd(entry->title);
		}
		printlcd("\n");
#if 0
		printf("[%d], %08x\n", entry->id, entry->next);
		printf("%08x, %08x\n", entry->title, entry->kernel);
		printf("%08x, %08x\n", entry->param, entry->initrd);
		printf("[%d]%s\n", entry->id, entry->title);
		printf("   +---%s\n", entry->kernel);
		printf("   +---%s\n", entry->param);
		printf("   +---%s\n", entry->initrd);
#endif
		entry = entry->next;
	}
	printf("===============================\n");
}

int boot_menu(menu_t * menu)
{
	menu->curr_entry = 0;
	if (menu->num == 0)
		return -1;
	show_menu(menu);
	while (1) {
		u8 key = read_key();
		/* enter */
		if (key == 0x14 || key == 0x21 || key == 0x23)
			break;
		/* up */
		else if (key == 0x04 || key == 0x32 || key == 0x00 || key == 31) {
			if (menu->curr_entry == 0)
				menu->curr_entry = menu->num - 1;
			else
				menu->curr_entry--;
			show_menu(menu);
			/* down */
		} else if (key == 0x44 || key == 0x11 || key == 0x10) {
			menu->curr_entry++;
			if (menu->curr_entry == menu->num)
				menu->curr_entry = 0;
			show_menu(menu);
		}
		Delay(300000);
	}
	return 0;
}


/* 
 * add enter_simple_pass_through_mode 
 * enter USB pass_through mode and do all initial work for emulator.
 * blob will stay in this function in life time
 */
void enter_simple_pass_through_mode(void)
{
	int ret;
	menu_t menu;
	char *kernel = "/boot/default";
	char *cmdline = "";
	int machid = 0;
	int lcd = 0;

	udc_disable();
	keypad_init();
/*  EnableLCD_8bit_active();
  while (1) {
  	u8 key = read_key();
	printf("key = %02x\n", key);
  }
*/


	if (is_key_press_down(0x04000031, 0) ||
	    is_key_press_down(0x04000013, 0) ||
	    is_key_press_down(0x04000003, 0) ||
	    is_key_press_down(0x04000012, 0) ||
	    is_key_press_down(0x04000042, 0)) {
		EnableLCD_8bit_active();
		init_lubbock_flash_driver();
		init_flash();
		USB_gpio_init();
		usbctl_init();
		GPCR(99) = GPIO_bit(99);	// USB_READY=low
		{
			int i = 5000 * 4;
			while (i--) ;
		}
		GPSR(99) = GPIO_bit(99);	// USB_READY =high
		/* hwuart_init(230400); */
		printlcd("OpenEZX blob 081230\nUSB ready\n");
		for (;;) {
			if (ICPR & 0x800) {
				udc_int_hndlr(0x11, 0);
			}
		}
	}

	/* turn on the power */
	pcap_mmc_power_on(1);
	udelay(1000);
//	EnableLCD_8bit_active();
	ret = mmc_init(0);
//	printlcd("mmc ok\n"); while(1);
	if (ret != 0) {
		EnableLCD_8bit_active();
		printlcd("Cannot find MMC card\n");
		while (1) ;
	}
	file_detectfs();

	ret = parse_conf_file(CONFIG_FILE, &menu);
	if (ret < 0) {
		EnableLCD_8bit_active();
		printlcd("Cannot parse config file\n");
		while (1) ;
	}
	if (menu.default_machid > 0)
		machid = menu.default_machid;

	if (is_key_press_down(0x04000002, 0) ||
	    is_key_press_down(0x04000043, 0) ||
	    is_key_press_down(0x04000013, 0)) {
		EnableLCD_8bit_active();
		ret = boot_menu(&menu);
		if (ret < 0 || !go_menu_entry) {
			printlcd("No suitable kernel entry\n");
			while (1) ;
		}
		kernel = go_menu_entry->kernel;
		if (go_menu_entry->machid > 0)
			machid = go_menu_entry->machid;
		if (go_menu_entry->param)
			cmdline = go_menu_entry->param;
		lcd++;
	}
	{
		int size;
		char *buf = (char *)KERNEL_RAM_BASE;

		if (lcd) printf("Loading %s...\n", kernel);
		size = file_fat_read(kernel, buf, 0x200000);	// 2MB

		/* turn off mmc controler, otherwise 2.4 kernel freezes */
		mmc_exit();
		pcap_mmc_power_on(0);

		if (size > 0) {
			if (lcd) printf("read %d bytes\nbooting...\n", size);
			boot_linux(cmdline, machid);
		}
		if (!lcd)
			EnableLCD_8bit_active();
		printf("Cannot load kernel from:\n   %s\n", kernel);
		while (1) ;
	}
}

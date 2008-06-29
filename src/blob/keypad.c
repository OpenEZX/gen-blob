/*
 * src/blob/keypad.c
 *
 * Keypad driver for ezx platform
 *
 * Copyright (C) 2005 Motorola Inc.
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
 * 2005-APR-06 - (Motorola) init draft
 *
 */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/command.h>
#include <blob/errno.h>
#include <blob/error.h>
#include <blob/init.h>
#include <blob/main.h>
#include <blob/memory.h>
#include <blob/param_block.h>
#include <blob/serial.h>
#include <blob/time.h>
#include <blob/util.h>
#include <blob/time.h>
#include <blob/lcd_ezx.h>
#include <linux/string.h>
#include <blob/log.h>
#include "usb_ctl_bvd.h"
#include "pcap.h"

/* initialize function */
void keypad_init(void)
{
//      printf("keypad_init\n");
	//currently, not touch CKEN- see start.S

	//init gpio
	set_GPIO_mode(97 | GPIO_ALT_FN_3_IN);
	set_GPIO_mode(98 | GPIO_ALT_FN_3_IN);

	set_GPIO_mode(100 | GPIO_ALT_FN_1_IN);
	set_GPIO_mode(101 | GPIO_ALT_FN_1_IN);
	set_GPIO_mode(102 | GPIO_ALT_FN_1_IN);
	set_GPIO_mode(103 | GPIO_ALT_FN_2_OUT);
	set_GPIO_mode(104 | GPIO_ALT_FN_2_OUT);
	set_GPIO_mode(105 | GPIO_ALT_FN_2_OUT);
	set_GPIO_mode(106 | GPIO_ALT_FN_2_OUT);
	set_GPIO_mode(107 | GPIO_ALT_FN_2_OUT);
	set_GPIO_mode(108 | GPIO_ALT_FN_2_OUT);
	//       for(i=0;i<100;i++);  
	//init regs

	KPC = 0x32a7f202;
	//currently keep KPKDI reset value, 100ms
	KPKDI = 0x64;
	/*
	   logvarhex("kpypad_init():KPC=0x",KPC);
	   logvarhex("kpypad_init():KPKDI=0x",KPKDI);
	 */
	return;

}

/* check key press down event */
/* return 1, key pressed down now, ret=0, key not pressed */
/* key_val: value of KPAS to check, simple design, ???fix me: caller has to know KPAS value to check */
/* Deb_val: Debounce value, if we use manual scan, just delay this period (uSec) */

int is_key_press_down(u32 key_val, u32 Deb_val)
{
//      printf("key = %08x\n", KPAS);
	if (KPAS == key_val) {	/* a key down signal got */
		Delay(Deb_val);	/* make debounce */
		if (KPAS == key_val)
			return 1;
		else
			return 0;
	} else
		return 0;
}

u8 read_key()
{
	while (!(0x04000000 & KPAS)) ;
	return (KPAS & 0xff);
}

/* exit list handler */
void keypad_exit(void)
{

}

__initlist(keypad_init, INIT_LEVEL_INITIAL_HARDWARE);
__exitlist(keypad_exit, INIT_LEVEL_INITIAL_HARDWARE);

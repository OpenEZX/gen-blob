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
#include <blob/log.h>
#include "usb_ctl_bvd.h"
#include "pcap.h"

/* initialize function */
void keypad_init(void)
{
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

	KPC = 0x32a7f000; /* Direct Keys disabled */
	//currently keep KPKDI reset value, 100ms
	KPKDI = 0x64;

	Delay(200); /* delay a bit so it works on all hw versions */
	return;

}

u32 read_kpas(void) {
	u32 kpas;
	while (1) {
		kpas = KPAS;
		/* retry until data is valid */
		if (!(kpas & 0xf8000000))
			return kpas;
	}
}

/* check key press down event */
/* return 1, key pressed down now, ret=0, key not pressed */
/* key_val: value of KPAS to check, simple design, ???fix me: caller has to know KPAS value to check */
/* Deb_val: Debounce value, if we use manual scan, just delay this period (uSec) */
int is_key_press_down(u32 key_val, u32 Deb_val)
{
	if (read_kpas() == key_val) {	/* a key down signal got */
		Delay(Deb_val);	/* make debounce */
		if (read_kpas() == key_val)
			return 1;
		else
			return 0;
	} else
		return 0;
}

u8 read_key()
{
	u32 kpas;
	while (1) {
		kpas = read_kpas();
		if (0x04000000 & kpas)
			return (kpas & 0xff);
	}
}

/* exit list handler */
void keypad_exit(void)
{

}

__initlist(keypad_init, INIT_LEVEL_INITIAL_HARDWARE);
__exitlist(keypad_exit, INIT_LEVEL_INITIAL_HARDWARE);

/*
 * ledasm.S: simple LED control code
 *
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
 */
/*
 * Documentation:
 * [1] Intel Corporation, "Intel StrongARM SA-1100 Microprocessor
 *     Developer's Manual", April 1999
 * [2] Intel Corporation, "Intel StrongARM SA-1110 Microprocessor
 *     Advanced Developer's manual, December 1999
 */

.ident "$Id: ledasm.S,v 1.3 2001/10/28 20:29:05 erikm Exp $"

#define __ASSEMBLY__

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>



.text


.globl ledinit
	/* initialise LED GPIO and turn LED on.
	 * clobbers r0 and r2
	 */
ledinit:
	/* low  - bank 2 Lubbock registers /SRAM 
	   high - bank 3 Ethernet Controller
           Need to set MSC1 before trying to write to the HEX LEDs
         */
	/*
	ldr	r0, =MSC1	
	ldr	r2, =MSC1_VAL
	str	r2, [r0]
	ldr	r2, [r0]
	*/

	ldr	r0, =FPGA_BASE_PHYS
	mov	r2, #0xFF
	strh	r2, [r0, #LED_BLANK_OFFSET]

	ldr	r2, =0xb10bb10b
#if (defined(CPU_pxa210) || defined(CPU_pxm261) )
	strh	r2, [r0, #HEX_LED_OFFSET]
#else
	str	r2, [r0, #HEX_LED_OFFSET]
#endif

	/*	
	mrc	p15, 0, r2, c0, c0
	str	r2, [r0, #HEX_LED_OFFSET]
	*/
	mov	pc, lr

.globl led_on
	/* turn LED on. clobbers r0 and r1 */
led_on:
	mov	pc, lr




.globl led_off
	/* turn LED off. clobbers r0 - r2 */
led_off:
	mov	pc, lr


	/* busy wait loop. clobbers r0 */
wait_loop:
	mov	r0, #0x400000
wait_loop1:
	subs	r0, r0, #1
	bne	wait_loop1
	mov	pc, lr


blinky:
	/* blink LED. clobbers r0 and r1 */
	mov	r2, lr
	bl	led_on
	bl	wait_loop
	bl	led_off
	bl	wait_loop
	mov	pc, r2
	

.globl led_blink
	/* blinks LED r0 times */
	/* clobbers r0 - r4 */
led_blink:
	mov	r4, lr
	mov	r3, r0

	/* switch LED off if it wasn't already off */
	bl	led_off
	bl	wait_loop
	
blink0:
	bl	blinky
	subs	r3, r3, #1
	bne	blink0
	mov	pc, r4


/*
 * start.S: blob start code
 *
 * Copyright (C) 2002, 2004-2006 Motorola Inc.	
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl) and
 *                     Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
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
 * This is the blob start code. The SA-1100 jumps to address 0x00000000
 * after a reset. We put a single branch code at this position which jumps
 * to a safe region to do the actual setup. All other vectors just point
 * to an endless loop for the moment.
 *
 * Documentation:
 * [1] Intel Corporation, "Intel StrongARM SA-1100 Microprocessor
 *     Developer's Manual", April 1999
 * [2] S. Furber, "ARM System Architecture", Addison Wesley Longman
 *     Ltd., Essex, England, 1996.
 * [3] Intel Corporation, "Intel StrongARM SA-1110 Microprocessor
 *     Advanced Developer's manual, December 1999
 *
 * 2002-Oct-30 - (Motorola) Modifications to support USB restore
 * 2002-Dec-09 - (Motorola) Changed for exit sleep/user off 
 * 2004-Aug-30 - (Motorola) Intel pm workaround2
 * 2005-Dec-14 - (Motorola) modified for EZX platform
 */

.ident "$Id: start.S,v 1.1.1.1 2002/07/17 00:32:33 cvs Exp $"

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <blob/arch/lubbock.h>
.text
/* Jump vector table as in table 3.1 in [1] */
.globl _start
_start:	b	reset
	b	undefined_instruction
	b	software_interrupt
	b	prefetch_abort
	b	data_abort
	b	not_used
	b	irq
	b	fiq

/* the actual reset code */
.align	4
reset:
	/* get a clue where we are running, so we know what to copy */
	ldr	r1, =0xffffff00
	and	r0, pc, r1 
	
	/* relocate the second stage loader */
	add	r2, r0, #(128 * 1024)	/* blob maximum size is 128kB */
	add	r0, r0, #0x1000		/* skip first 4*1024 bytes */
	ldr	r1, =(BLOB_ABS_BASE_ADDR - 0x1000)
	add	r1, r1, #0x1000		/* skip over here as well */

	cmp	r0, r1
	beq	stack

	/* r0 = source address
	 * r1 = target address
	 * r2 = source end address
	 */
copy_loop:
	ldmia	r0!, {r3-r10}
	stmia	r1!, {r3-r10}
	cmp	r0, r2
	ble	copy_loop

stack:
	/* set up the stack pointer */
	ldr	r0, =(BLOB_ABS_BASE_ADDR - 0x1000)
	add	r1, r0, #(1024 * 1024)
	sub	sp, r1, #0x04
	
	/* blob is copied to ram, so jump to it */
	add	r0, r0, #0x1000

	mov	pc, r0
         

badram:
	b	blinky
	
blinky:
	/* This is test code to blink the LED
           very useful if nothing else works */
	b	blinky
undefined_instruction:
	b	blinky

software_interrupt:
	/* NOTE: This is NOT an error! If you think that blob should return
	 * from software interrupts, you're plain WRONG. The source of the
	 * problem is in the kernel: you should *disable* CONFIG_ANGELBOOT
	 * simply because blob is not angel. -- Erik
	 */
	b	blinky

prefetch_abort:
	b	blinky

data_abort:
	b	blinky

not_used:
	b	blinky

irq:
	b	blinky

fiq:
	b	blinky







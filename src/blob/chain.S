/*
 * chain.S: blob chain loader
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * $Id: chain.S,v 1.2 2001/11/04 23:12:50 erikm Exp $
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
 * This is a blob first stage loader that can be called with a
 * hostile non-linux compatible bootloader, so you'll get a linux
 * compatible chain loader. It's also nice for debugging blob.
 *
 * This loader is position independent.
 */

.ident "$Id: chain.S,v 1.2 2001/11/04 23:12:50 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>


.text

.globl _start
_start:
	/* store our start location */
	sub	r0, pc, #8
	str	r0, START

#ifdef H3600
	/* this is a hack to be able to chain load blob from
	 * bootldr. bootldr checks if it can find the linux zImage
	 * magic value at the ninth word of an image, so we just put
	 * it there.
	 */
	b	led
.org (_start + 9 * 4)
	.word	0x016f2818
led:
#endif

	/* init LED */
	bl	ledinit

	/* assume that the CPU and the memory are already set up at
	 * this point. also assume that interrupts are disabled, and if
	 * not, there is a valid reason for it.
	 */

	/* since we're probably running from RAM, we assume that the
	 * RAM is not broken, so we don't want to run a memory tester at
	 * the part of memory where we are loaded.
	 */

relocate:
	/* get a clue where we are running so we know what to copy */
	ldr	r0, START

	/* relocate the second stage loader */
	add	r2, r0, #(64 * 1024)	/* blob maximum size is 64kB */
	add	r0, r0, #0x400		/* skip first 1024 bytes */
	ldr	r1, BLOB_START

	/* r0 = source address
	 * r1 = target address
	 * r2 = source end address
	 */
copy_loop:
	ldmia	r0!, {r3-r10}
	stmia	r1!, {r3-r10}
	cmp	r0, r2
	ble	copy_loop


	/* turn off the LED. if it stays off it is an indication that
	 * we didn't make it into the C code 
	 */
	bl led_off


	/* blob is copied to ram, so jump to it */
	ldr	r0, BLOB_START
	mov	pc, r0

	
START:		.word	0x00000000
BLOB_START:	.word	BLOB_ABS_BASE_ADDR

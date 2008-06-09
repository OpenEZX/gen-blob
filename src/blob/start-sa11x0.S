/*
 * start.S: blob start code
 *
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
 */

.ident "$Id: start.S,v 1.6 2001/11/15 11:56:20 seletz Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>

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




/* some defines to make life easier */
/* Register addresses can be found in [1] Appendix A */
IC_BASE:        .word   0x90050000
#define ICMR    0x04
	
PWR_BASE:	.word	0x90020000
#define PSSR	0x04
#define PSPR	0x08
#define PPCR	0x14
	
RST_BASE:	.word	0x90030000
#define RCSR	0x04

/* main memory starts at 0xc0000000 */
BLOB_START:	.word	BLOB_ABS_BASE_ADDR


/* The initial CPU speed. Note that the SA11x0 CPUs can be safely overclocked:
 * 190 MHz CPUs are able to run at 221 MHz, 133 MHz CPUs can do 190 Mhz.
 */
cpuspeed:	.word	CPU_SPEED



/* the actual reset code */
reset:
	/* First, mask **ALL** interrupts */
	ldr	r0, IC_BASE
	mov	r1, #0x00
	str	r1, [r0, #ICMR]

	/* switch CPU to correct speed */
	ldr	r0, PWR_BASE
	ldr	r1, cpuspeed
	str	r1, [r0, #PPCR]


	/* init LED */
	bl	ledinit

	/* setup memory */
	bl	memsetup

	/* check if this is a wake-up from sleep */
	ldr	r0, RST_BASE
	ldr	r1, [r0, #RCSR]
	and	r1, r1, #0x0f
	teq	r1, #0x08
	bne	normal_boot	/* no, continue booting */

	/* yes, a wake-up. clear RCSR by writing a 1 (see 9.6.2.1 from [1]) */
	mov	r1, #0x08
	str	r1, [r0, #RCSR]	;

	/* handle Power Manager Sleep Status Register (PSSR)
	 * see 9.5.7.5 from [1]*/
	ldr r1, [r0, #PSSR]
	/* clear DH bit, brings out DRAM from self-refresh */
	orr r1, r1, #0x08
	/* clear PH bit, bring periperal pins out from sleep state */
	orr r1, r1, #0x10
	str r1, [r0, #PSSR]
	
	/* get the value from the PSPR and jump to it */
	ldr	r0, PWR_BASE
	ldr	r1, [r0, #PSPR]
	mov	pc, r1


normal_boot:
	/* check the first 1MB of BLOB_START in increments of 4k */
	mov	r7, #0x1000
	mov	r6, r7, lsl #8	/* 4k << 2^8 = 1MB */
	ldr	r5, BLOB_START

mem_test_loop:
	mov	r0, r5
	bl	testram
	teq	r0, #1
	beq	badram

	add	r5, r5, r7
	subs	r6, r6, r7
	bne	mem_test_loop


relocate:
	adr	r0, _start

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


badram:
	mov	r6, #2
	b	endless_blink

	

endless_blink:
	bl	wait
	mov	r0, r6
	bl	led_blink
	b	endless_blink
		
wait:
	/* busy wait loop*/
	mov	r5, #0x1000000
wait0:
	subs	r5, r5, #1
	bne	wait0
	mov	pc, lr

	
	
	
undefined_instruction:
	mov	r6, #3
	b	endless_blink




software_interrupt:
	/* NOTE: This is NOT an error! If you think that blob should return
	 * from software interrupts, you're plain WRONG. The source of the
	 * problem is in the kernel: you should *disable* CONFIG_ANGELBOOT
	 * simply because blob is not angel. -- Erik
	 */
	mov	r6, #4
	b	endless_blink





prefetch_abort:
	mov	r6, #5
	b	endless_blink




data_abort:
	mov	r6, #6
	b	endless_blink




not_used:
	/* we *should* never reach this */
	mov	r6, #7
	b	endless_blink




irq:
	mov	r6, #8
	b	endless_blink




fiq:
	mov	r6, #9
	b	endless_blink

/*
 * memsetup.S: memory setup for various architectures
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
 * Documentation:
 * [1] Intel Corporation, "Intel StrongARM SA-1100 Microprocessor
 *     Developer's Manual", April 1999
 * [2] Intel Corporation, "Intel StrongARM SA-1110 Microprocessor
 *     Advanced Developer's manual, December 1999
 */

.ident "$Id: memsetup.S,v 1.1.1.1 2002/07/17 00:32:33 cvs Exp $"

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif




.text




MEM_BASE:	.long	0xa0000000
MEM_START:	.long	0xc0000000
#define	MDCNFG	0x0
#define MDCAS0	0x04
#define MDCAS1	0x08
#define MDCAS2	0x0c
#define MCS0	0x10




#if (defined BRUTUS)
mdcas0:		.long	0xc71c703f
mdcas1:		.long	0xffc71c71
mdcas2:		.long	0xffffffff
mdcnfg:		.long	0x0334b22f
mcs0:		.long	0xfff8fff8
#endif

#if (defined NESA)
mdcas0:		.long	0xc71c703f
mdcas1:		.long	0xffc71c71
mdcas2:		.long	0xffffffff
mdcnfg:		.long	0x0334b22f
mcs0:		.long	0xfff84458
#endif

#if defined LART
mdcas0:		.long	0xc71c703f
mdcas1:		.long	0xffc71c71
mdcas2:		.long	0xffffffff
mdcnfg:		.long	0x0334b22f
mcs0:		.long	0xad8c4888
#endif

#if defined PLEB
mdcas0:		.long	0x1c71c01f
mdcas1:		.long	0xff1c71c7
mdcas2:		.long	0xffffffff
mdcnfg:		.long	0x0c7f3ca3
mcs0:		.long	0xfff8fff8
#endif

#if defined SHANNON
mdcas0:		.long	0xc71c703f
mdcas1:		.long	0xffc71c71
mdcas2:		.long	0xffffffff
mdcnfg:		.long	0x0334b21f
mcs0:		.long	0xfff84458
#endif


.globl memsetup
memsetup:
#if defined USE_SA1100
	/* Setup the flash memory */
	ldr	r0, MEM_BASE
	
	ldr	r1, mcs0
	str	r1, [r0, #MCS0]
	
	/* Set up the DRAM */
	
	/* MDCAS0 */
	ldr	r1, mdcas0
	str	r1, [r0, #MDCAS0]

	/* MDCAS1 */
	ldr	r1, mdcas1
	str	r1, [r0, #MDCAS1]

	/* MDCAS2 */
	ldr	r1, mdcas2
	str	r1, [r0, #MDCAS2]

	/* MDCNFG */
	ldr	r1, mdcnfg
	str	r1, [r0, #MDCNFG]

	/* Issue read requests to disabled bank to start refresh */
	/* this is required by the Micron memory on a TuxScreen */
	
	/* Comment from JDB:
	 * This is not strictly necessary. As long as blob writes something to
	 * the serial port between enabling and accessing DRAM (as I think it
	 * does even on Shannon), you can pretty much guarantee that the
	 * required eight refreshes have occurred (as 1 refresh is ~16
	 * microseconds, so even writing two characters at 115k2 covers the
	 * refreshes).
	 *
	 * Comment (on comment) from Erik:
	 * We don't print anymore in the low level loader, so we need this
	 * on all architectures.
	 */
	ldr	r1, MEM_START

.rept	8
	ldr	r0, [r1]
.endr
	
	
#elif defined LUBBOCK
	/* This part is actually for the Assabet only. If your board
	 * uses other settings, you'll have to ifdef them here.
	 */
	/* Set up the SDRAM */
	mov	r1, #0xA0000000		/* MDCNFG base address */

	ldr	r2, =0xAAAAAA7F
	str	r2, [r1, #0x04]		/* MDCAS00 */
	str	r2, [r1, #0x20]		/* MDCAS20 */

	ldr	r2, =0xAAAAAAAA
	str	r2, [r1, #0x08]		/* MDCAS01 */
	str	r2, [r1, #0x24]		/* MDCAS21 */

	ldr	r2, =0xAAAAAAAA
	str	r2, [r1, #0x0C]		/* MDCAS02 */
	str	r2, [r1, #0x28]		/* MDCAS22 */

	ldr	r2, =0x4dbc0327		/* MDREFR */
	str	r2, [r1, #0x1C]

	ldr	r2, =0x72547254		/* MDCNFG */
	str	r2, [r1, #0x00]

	/* Issue read requests to disabled bank to start refresh */

	ldr	r1, =0xC0000000

.rept	8	
	ldr	r0, [r1]
.endr

	mov	r1, #0xA0000000		/* MDCNFG base address */
	
	ldr	r2, =0x72547255		/* Enable the banks */
	str	r2, [r1, #0x00]		/* MDCNFG */
	
/* Static memory chip selects on Assabet: */

	ldr	r2, =0x4b90			/* MCS0 */
	orr	r2,r2,r2,lsl #16
	str	r2, [r1, #0x10]

	ldr	r2, =0x22212419			/* MCS1 */
	str	r2, [r1, #0x14]

	ldr	r2, =0x42196669			/* MCS2 */
	str	r2, [r1, #0x2C]

	ldr	r2, =0xafccafcc			/* SMCNFG */
	str	r2, [r1, #0x30]

/* Set up PCMCIA space */

	ldr	r2, =0x194a994a			/*0x994a994a*/
	str	r2, [r1, #0x18]

/* All SDRAM memory settings should be ready to go... */
/* For best performance, should fill out remaining memory config regs: */


	/* Testing ,Chester */

	mov r3,#0x12000000
	mov r2,#0x5000                 // D9_LED on and D8_LED off 
	str r2,[r3]
	mov r4, #0x20000
gogogo2:
	subs r4, r4, #1
	bne gogogo2

#else
#error "Configuration error: CPU not defined!"
#endif
	
	mov	pc, lr

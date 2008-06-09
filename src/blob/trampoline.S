/*
 * trampoline.S: start of second stage boot loader, jump from assembly to C
 *
 * Copyright (C) 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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

.ident "$Id: trampoline.S,v 1.3 2001/10/28 20:34:41 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

.text

.globl _trampoline
_trampoline:
	/* clear the BSS section */
	ldr	r1, bss_start
	ldr	r0, bss_end
	sub	r0, r0, r1

	/* r1 = start address */
	/* r0 = #number of bytes */
	mov	r2, #0

clear_bss:
	stmia	r1!, {r2}
	subs	r0, r0, #4
	bne	clear_bss

	/* setup the stack pointer */
	ldr	r0, stack_end
	sub	sp, r0, #4

	/* jump to C code */
	bl	main
	/* if main ever returns we just call it again */
	b	_trampoline


bss_start:	.word	__bss_start
bss_end:	.word	__bss_end
stack_end:	.word	__stack_end

/*
 * testmem.S: memory tester, test if there is RAM available at given location
 *
 * Copyright (C) 2001 Russell King (rmk@arm.linux.org.uk)
 *
 * This version clobbers registers r1-r4, so be sure to store their contents
 * in a safe position. This function is not APCS compliant, so only use it
 * from assembly code.
 *
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

.ident "$Id: testmem.S,v 1.1 2001/10/07 15:51:01 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif
	
.text
.globl testram
	@ r0 = address to test
	@ returns r0 = 0 - ram present, r0 = 1 - no ram
	@ clobbers r1 - r4
testram:
        ldmia   r0, {r1, r2}	@ store current value in r1 and r2
        mov	r3, #0x55	@ write 0x55 to first word
        mov     r4, #0xaa	@ 0xaa to second
        stmia   r0, {r3, r4}
        ldmia   r0, {r3, r4}	@ read it back
        teq     r3, #0x55	@ do the values match
        teqeq   r4, #0xaa
        bne     bad		@ oops, no
        mov     r3, #0xaa	@ write 0xaa to first word
        mov     r4, #0x55	@ 0x55 to second
        stmia   r0, {r3, r4}
        ldmia   r0, {r3, r4}	@ read it back 
        teq     r3, #0xaa	@ do the values match
        teqeq   r4, #0x55
bad:    stmia   r0, {r1, r2}	@ in any case, restore old data
        moveq   r0, #0		@ ok - all values matched
        movne   r0, #1		@ no ram at this location
        mov     pc, lr

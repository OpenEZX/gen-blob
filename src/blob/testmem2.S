/*
 * testmem2.S: memory tester, test if there is RAM available at given location
 *
 * Copyright (C) 2001 Russell King (rmk@arm.linux.org.uk)
 *
 * This version doesn't clobber registers r1-r4, so it's safe to call it from
 * C code (fits nicely into the APCS specs).
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

.ident "$Id: testmem2.S,v 1.2 2001/10/07 19:34:17 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

.text
.globl testram
	@ r0 = address to test
	@ returns r0 = 0 - ram present, r0 = 1 - no ram
	@ doesnt clobber r1 - r4
testram:
	stmdb	sp!, {r1-r4, lr}
        ldmia   r0, {r1, r2}
        mov     r3, #0x55	@ write 0x55 to first word
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
	ldmia	sp!, {r1-r4, pc}

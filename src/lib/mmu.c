/*
 * mmu.c
 *
 * MMU handling
 *
 * Copyright (C) 2001-2005  Motorola
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
 * Oct 30,2002 - (Motorola) Created
 *
 */


#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/mmu.h>
#include <blob/types.h>

// memory space map
extern struct mem_area io_map[];

#define MAX_PE	4096

// Section, 1M per entry
static unsigned long mmu_table[MAX_PE] __attribute__ ( (aligned(0x4000)));

void mmu_init(void)
{
	unsigned long i, addr, end;

	// init MMU table
	for (i=0; i < MAX_PE; i++) {
		mmu_table[i] = 0;
	}

	for (i=0; ; i++) {
		addr = io_map[i].start;
		end = addr + io_map[i].size;

		if (addr == end)
			break;

		while( addr < end ) {
			mmu_table[addr>>20] = addr | io_map[i].attr;
			addr += 0x100000;
		}
	}

	// enable MMU
	__asm(
	"\n\
	mrc	p15, 0, r0, c1, c0 \n\
	orr	r0, r0, #0x1000 \n\
	mcr	p15, 0, r0, c1, c0, 0 \n\
	\n\
	ldr	r0, =mmu_table \n\
	mcr	p15, 0, r0, c2, c0 \n\
	\n\
	mov	r0, #0xffffffff        \n\
	mcr	p15, 0, r0, c3, c0, 0  \n\
	\n\
	mcr	p15, 0, r0, c7, c10, 4      \n\
	mrc	p15, 0, r0, c1, c0, 0       \n\
	orr	r0, r0, #5	 	\n\
	b	1f	\n\
	.align 5	\n\
1:	\n\
	mcr	p15, 0, r0, c1, c0, 0	\n\
	\n\
	mrc	p15, 0, R0, c2, c0, 0 \n\
	mov	r0, r0 \n\
	sub	pc, pc, #4 \n\
	\n\
	nop \n\
	nop \n\
	nop \n\
	nop \n\
	":::"r0","memory"
	);
}

void enable_mmu(void)
{
	__asm(
	"\n\
	mrc	p15, 0, r0, c1, c0, 0 \n\
	orr	r0, r0, #0x1000 \n\
	orr	r0, r0, #0x0005	\n\
	b	1f	\n\
	.align 5	\n\
1:	\n\
	mcr	p15, 0, r0, c1, c0, 0 \n\
	\n\
       	mrc     p15, 0, r0, c2, c0, 0	\n\
	mov     r0, r0		\n\
	sub     pc, pc, #4 \n\
	\n\
	nop \n\
	nop \n\
	nop \n\
	":::"r0","memory"
	);
}

void disable_mmu(void)
{
	__asm(
	"\n\
	mrc	p15, 0, r0, c1, c0, 0 \n\
	bic	r0, r0, #0x1000 \n\
	bic	r0, r0, #0x0005 \n\
	b	1f	\n\
	.align 5	\n\
1:	\n\
	mcr	p15, 0, r0, c1, c0, 0	\n\
	\n\
	mov	r0, #0x00000000 \n\
	mcr     p15, 0, r0, c8, c7, 0 \n\
	mcr	p15, 0, r0, c7, c7, 0 \n\
	mcr	p15, 0, r0, c7, c10, 4 \n\
	\n\
	mrc     p15, 0, r0, c2, c0, 0	\n\
	mov     r0, r0		\n\
	sub     pc, pc, #4 \n\
	\n\
	nop \n\
	nop \n\
	nop \n\
	":::"r0","memory"
	);
}

/* clear section descriptor C,B bits */
void set_uncache_unbuffer(struct mem_area *map)
{
	unsigned long addr, end;

	// disable MMU, caches
	disable_mmu();

	// flash address from the head of memory space
	addr = map->start;
	end = addr + map->size;

	// set flash uncachable, unbufferable
	while( addr < end ) {
		mmu_table[addr>>20] &= 0xfffffff3;
		addr += 0x100000;
	}

	// reenable MMU, caches
	enable_mmu();
}



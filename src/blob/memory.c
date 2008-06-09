/*
 * src/blob/memory.c
 *
 * memory test routines for the blob bootloader
 *
 * Copyright (C) 2002, 2005 Motorola Inc.
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
 * 2002-Oct-30 - (Motorola) Add ifdef to test memory map
 * 2005-Jul-01 - (Motorola) Update Blob for Sumatra to match MBM
 * 2005-Oct-19 - (Motorola) Fix: Cursor position is not update properly.
 * 2005-Dec-14 - (Motorola) Mmodified for EZX platform
 *
 */

#ident "$Id: memory.c,v 1.4 2002/01/03 16:07:18 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/memory.h>
#include <blob/serial.h>

/* test in 1MB chunks */
#define TEST_BLOCK_SIZE (1024 * 1024)




/* from testmem2.S */
extern int testram(u32 addr);




memory_area_t memory_map[NUM_MEM_AREAS];




void get_memory_map(void)
{
	int i;

	/* init */
	for(i = 0; i < NUM_MEM_AREAS; i++)
		memory_map[i].used = 0;

#ifdef TEST_MEM_MAP
	/* first write a 0 to all memory locations */
	for(addr = MEMORY_START; addr < MEMORY_END; addr += TEST_BLOCK_SIZE)
		* (u32 *)addr = 0;

	/* scan memory in blocks */
	i = 0;
	for(addr = MEMORY_START; addr < MEMORY_END; addr += TEST_BLOCK_SIZE) {
		if(testram(addr) == 0) {
			/* yes, memory */
			if(* (u32 *)addr != 0) { /* alias? */
#ifdef BLOB_DEBUG
				SerialOutputString("Detected alias at 0x");
				SerialOutputHex(addr);
				SerialOutputString(", aliased from 0x");
				SerialOutputHex(* (u32 *)addr);
				serial_write('\n');
#endif
				if(memory_map[i].used)
					i++;

				continue;
			}

			/* not an alias, write the current address */
			* (u32 *)addr = addr;
#ifdef BLOB_DEBUG			
			SerialOutputString("Detected memory at 0x");
			SerialOutputHex(addr);
			serial_write('\n');
#endif
			/* does this start a new block? */
			if(memory_map[i].used == 0) {
				memory_map[i].start = addr;
				memory_map[i].len = TEST_BLOCK_SIZE;
				memory_map[i].used = 1;
			} else {
				memory_map[i].len += TEST_BLOCK_SIZE;
			}
		} else {
			/* no memory here */
			if(memory_map[i].used == 1)
				i++;
		}
	}
	SerialOutputString("Memory map:\n");
	for(i = 0; i < NUM_MEM_AREAS; i++) {
		if(memory_map[i].used) {
			SerialOutputString("  0x");
			SerialOutputHex(memory_map[i].len);
			SerialOutputString(" @ 0x");
			SerialOutputHex(memory_map[i].start);
			SerialOutputString(" (");
			SerialOutputDec(memory_map[i].len / (1024 * 1024));
			SerialOutputString(" MB)\n");
		}
	}
#else

#if defined(SUMATRA) || defined(HAINAN) || defined(MARTINIQUE)
		memory_map[0].used = 1;
		memory_map[0].start = MEMORY_START;
		memory_map[0].len = 0x1000000;

		memory_map[1].used = 1;
		memory_map[1].start = MEMORY_START + 0x1000000;
		memory_map[1].len = 0x1000000;

		memory_map[2].used = 1;
		memory_map[2].start = MEMORY_START + 0xc000000;
		memory_map[2].len = 0x1000000;

#else
		memory_map[0].used = 1;
		memory_map[0].start = MEMORY_START;
		memory_map[0].len = 0x2000000;
#endif

#if (0)  //For Barbados, we only have 32MB one chip SDRAM
		memory_map[1].used = 1;
		memory_map[1].start = MEMORY_START + 0x1000000;
		memory_map[1].len = 0x1000000;
		
		
		
		addr = MEMORY_START + 0xc000000;
		*(u32 *)addr = 0;
		if(testram(addr) == 0) {
			memory_map[2].used = 1;
			memory_map[2].start = MEMORY_START + 0xc000000;
			memory_map[2].len = 0x1000000;
		}
#endif

#endif
}

/*-------------------------------------------------------------------------
 * Filename:      flash.c
 * Version:       $Id: flash.c,v 1.11 2002/01/03 16:07:17 erikm Exp $ 
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   Flash I/O functions for blob
 * Created at:    Mon Aug 23 20:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sat Jan 15 19:16:34 2000
 *-----------------------------------------------------------------------*/
/*
 * flash.c: Flash I/O functions for blob
 *
 * Copyright (C) 2005  Motorola Inc.
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 * Copyright (C) 1999  Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
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
 * 2005-Dec-14 - (Motorola) For ezx platform
 *
 */

#ident "$Id: flash.c,v 1.11 2002/01/03 16:07:17 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/errno.h>
#include <blob/error.h>
#include <blob/led.h>
#include <blob/util.h>
#include <blob/serial.h>
#include <blob/flash.h>

#include <blob/init.h>
#include <blob/command.h>

extern void printlcd(char *cntrl_string);
/* this is enough for a 16MB flash with 128kB blocks */
#define NUM_FLASH_BLOCKS	(128 * 3)


typedef struct {
	u32 start;
	u32 size;
	int lockable;
} flash_block_t;


static flash_block_t flash_blocks[NUM_FLASH_BLOCKS];
static int num_flash_blocks;

flash_descriptor_t *flash_descriptors;
flash_driver_t *flash_driver;


/* dummy function for enable_vpp and disable_vpp */
int flash_dummy_ok(void)
{
	return 0;
}




/* initialise the flash blocks table */
static void init_flash(void)
{
	int i = 0;
	int j;
	u32 start = 0;
	
#ifdef BLOB_DEBUG
	if(flash_descriptors == NULL) {
		printerrprefix();
		SerialOutputString("undefined flash_descriptors\n");
		return;
	}

	if(flash_driver == NULL) {
		printerrprefix();
		SerialOutputString("undefined flash_driver\n");
		return;
	}
#endif	

	/* fill out missing flash driver functions */
	if(flash_driver->enable_vpp == NULL)
		flash_driver->enable_vpp = flash_dummy_ok;

	if(flash_driver->disable_vpp == NULL)
		flash_driver->disable_vpp = flash_dummy_ok;

	/* initialise flash blocks table */
	num_flash_blocks = 0;

	while(flash_descriptors[i].size != 0) {
#ifdef BLOB_DEBUG
		SerialOutputDec(flash_descriptors[i].num);
		SerialOutputString("x 0x");
		SerialOutputHex(flash_descriptors[i].size);
		SerialOutputString(", ");

		if(!flash_descriptors[i].lockable)
			SerialOutputString("not ");

		SerialOutputString("lockable\n");
#endif

		for(j = 0; j < flash_descriptors[i].num; j++) {
			flash_blocks[num_flash_blocks].start = start;
			flash_blocks[num_flash_blocks].size =
				flash_descriptors[i].size;
			flash_blocks[num_flash_blocks].lockable =
				flash_descriptors[i].lockable;

			start += flash_descriptors[i].size;

			num_flash_blocks++;

			if(num_flash_blocks >= NUM_FLASH_BLOCKS) {
				printerrprefix();
				SerialOutputString("not enough flash_blocks\n");
				break;
			}
		}

		i++;
	}

#ifdef BLOB_DEBUG
	SerialOutputString("Flash map:\n");
	for(i = 0; i < num_flash_blocks; i++) {
		SerialOutputString("  0x");
		SerialOutputHex(flash_blocks[i].size);
		SerialOutputString(" @ 0x");
		SerialOutputHex(flash_blocks[i].start);
		SerialOutputString(" (");
		SerialOutputDec(flash_blocks[i].size / 1024);
		SerialOutputString(" kB), ");

		if(!flash_blocks[i].lockable)
			SerialOutputString("not ");

		SerialOutputString("lockable\n");
	}
#endif
}


__initlist(init_flash, INIT_LEVEL_OTHER_STUFF + 1);


int flash_erase_region(u32 *start, u32 nwords)
{
	u32 *cur;
	u32 *end;
	int rv, i;

	cur = start;
	end = start + nwords;

#if BLOB_DEBUG
	SerialOutputString(__FUNCTION__ "(): erasing 0x");
	SerialOutputHex(nwords);
	SerialOutputString(" (");
	SerialOutputDec(nwords);
	SerialOutputString(") words at 0x");
	SerialOutputHex((u32)start);
	serial_write('\n');
#endif

	flash_driver->enable_vpp();
	i =0;
	while(cur < end) {
			SerialOutputString("erasing dirty block at 0x");
			SerialOutputHex((u32)cur);
			serial_write('\n');

			/* dirty block */
			rv = flash_driver->erase(cur);

			if(rv < 0) {
				printerrprefix();
				SerialOutputString("flash erase error at 0x");
				SerialOutputHex((u32)cur);
				serial_write('\n');
				flash_driver->disable_vpp();
				return rv;
			}

		cur += 0x20000/sizeof(*cur);
		printlcd(".");
		i++;
		if((i%20)==0)
		  printlcd("\n");
	}
        printlcd("\n");	
	flash_driver->disable_vpp();

	return 0;
}



#if 0
/* Write a flash region with a minimum number of erase operations.
 *
 * Flash chips wear from erase operations (that's why flash lifetime
 * is specified in erase cycles), so we try to limit the number of
 * erase operations in this function. Luckily the flash helps us a
 * little bit with this, because it only allows you to change a '1'
 * bit into a '0' during a write operation. This means that 0xffff can
 * be changed into 0x1010, and 0x1010 into 0x0000, but 0x0000 can't be
 * changed into anything else anymore because there are no '1' bits
 * left.
 */
int flash_write_region(u32 *dst, const u32 *src, u32 nwords)
{
	int rv;
	u32 nerrors = 0;
	u32 i = 0;

	u32 nerase = 0;
	u32 nwrite = 0;
	u32 nscandown = 0;
	u32 nskip = 0;

#if defined BLOB_DEBUG
	SerialOutputString(__FUNCTION__ "(): flashing 0x");
	SerialOutputHex(nwords);
	SerialOutputString(" (");
	SerialOutputDec(nwords);
	SerialOutputString(") words from 0x");
	SerialOutputHex((u32)src);
	SerialOutputString(" to 0x");
	SerialOutputHex((u32)dst);
	serial_write('\n');
#endif

	flash_driver->enable_vpp();

	while(i < nwords) {
		/* nothing to write */
		if(dst[i] == src[i]) {
			i++;
			nskip++;
			continue;
		}

		/* different, so write to this location */
		rv = flash_driver->write(&dst[i], &src[i]);
		nwrite++;
		
		if(rv == 0) {
			i++;
		} else {
			nerrors++;
			
			SerialOutputString("erasing at 0x");
			SerialOutputHex((u32)&dst[i]);
			SerialOutputString("...");

			/* erase block at current location */
			rv = flash_driver->erase(&dst[i]);
			nerase++;
			if(rv < 0) {
				/* something is obviously wrong */
				SerialOutputString(" error\n");
				flash_driver->disable_vpp();

				return rv;
			}
				
			SerialOutputString(" scanning down...");

			/* scan down until we find the first
                           non-erased location and restart writing
                           again from that location */
			while((i > 0) &&
			      ((dst[i] != src[i]) || (dst[i] == 0xffffffff))) {
				i--;
				nscandown++;
			}

			SerialOutputString(" resume writing at 0x");
			SerialOutputHex((u32)&dst[i]);
			serial_write('\n');
		}
		
		/* there is something seriously wrong if this is true */
		if(nerrors > 2 * nwords) {
			printerrprefix();
			SerialOutputString("too many flash errors, probably hardware error\n");
			flash_driver->disable_vpp();

			return -EFLASHPGM;
		}
	}

#ifdef BLOB_DEBUG
	SerialOutputDec(nwords);
	SerialOutputString(" words source image\n");
	SerialOutputDec(nwrite);
	SerialOutputString(" words written to flash\n");
	SerialOutputDec(nskip);
	SerialOutputString(" words skipped\n");
	SerialOutputDec(nerase);
	SerialOutputString(" erase operations\n");
	SerialOutputDec(nscandown);
	SerialOutputString(" words scanned down\n");
	serial_write('\n');
#endif

	flash_driver->disable_vpp();

	return 0;
}
#endif

unsigned short flash_program_buf(volatile u16* addr, volatile u16* data, volatile int len)
{
	
    volatile unsigned short *ROM;
    unsigned short stat = 0;
    int timeout = 0x50000;
    int i;
    volatile int tlen;
    volatile unsigned short *taddr,*tdata;
    
    tlen = len;

    ROM = (volatile unsigned short *)((unsigned long)addr);
    taddr=(volatile unsigned short *)((unsigned long)addr);
    tdata=(volatile unsigned short *)((unsigned long)data);

    // Clear any error conditions
    ROM[0] = 0x0050;

while(len>0)
{
  timeout=0x50000;
  ROM[0]=0x00E8;
  
  while(((stat=ROM[0])&0x0080)!=0x0080)
   {
     if(--timeout==0)
     {
      stat=*addr;
	printlcd("\ntimeout1\n");
      goto bad;
     }
   }
   ROM[0]=31;
   
   for(i=0;i<32;i++)
   {
    *addr=*data;
    addr++;
    data++;
   }
   len=len-64;
  ROM[0]=0x00D0;
  timeout=0x50000;
  while(((stat=ROM[0])&0x0080)!=0x0080)
  {
    if(--timeout==0)
     {
      stat=*addr;
	printlcd("\ntimeout2\n");
      goto bad;
     }
  }
  ROM[0]=0x00ff;
}
 
 // ROM[0]=FLASH_Reset;
  stat = 0;
while(tlen>0)
{
   if(*taddr!=*tdata)
    {
        printlcd("\ncheck error\n");
        stat=tlen;
        break;
    }
   tlen-=2;
   taddr ++;
   tdata ++;
}

    // Restore ROM to "normal" mode
 bad:
    ROM[0] = 0x00FF;            
    return stat;
}


//
// CAUTION!  This code must be copied to RAM before execution.  Therefore,
// it must not contain any code which might be position dependent!
//
int flash_write_region(u16 *dst, u16 *src, u32 nwords)
{
    unsigned long size;
    unsigned short stat=0;
    int i;

    i = 0;
    while(nwords>0)
    {
	size = nwords;
	if(size >= 0x20000)
		size = 0x20000;
	stat = flash_program_buf(dst, src, size);
	if(stat)
	{
		printlcd("!!!\n");
		return -EFLASHPGM;
	}
	printlcd(".");
	i++;
	if((i%20)==0)
		printlcd("\n");
	nwords -= size;
	dst += size/sizeof(*dst);
	src += size/sizeof(*src);
    }	
    printlcd("\n");
    return stat;
}



/* given an address, return the flash block index number (or negative
 * error number otherwise
 */
static int find_block(u32 address)
{
	int i;

	for (i = 0; i < num_flash_blocks; i++) {
		u32 start = flash_blocks[i].start;
		int length = flash_blocks[i].size;
		u32 endIncl = start + length - 1;

		if (address >= start && address <= endIncl)
			return i;
	}

	return -ERANGE;
}




/* convert address range to range of flash blocks. returns 0 on
 * success or negative error number on failure.
 */
static int address_range_to_block_range(u32 startAddress, int length, 
					int *startBlock, int *endInclBlock)
{
	int sb, eib;

#ifdef FLASH_DEBUG
	SerialOutputString(__FUNCTION__ ": startAddress = 0x");
	SerialOutputHex(startAddress);
	SerialOutputString(", length = 0x");
	SerialOutputHex(length);
	SerialOutputString("\n");
#endif

	sb = find_block(startAddress);
	eib = find_block(startAddress + length - 1);

	if(sb < 0)
		return sb;

	if(eib < 0)
		return eib;

#ifdef FLASH_DEBUG
	SerialOutputString("sb: ");
	SerialOutputDec(sb);
	SerialOutputString(", eib: ");
	SerialOutputDec(eib);
	SerialOutputString("\n");
#endif

	// would be nice to warn if sb isn't at the beginning of
	// its block or eib isn't at the very end of its block.

	*startBlock = sb;
	*endInclBlock = eib;

	return 0;
}




static int do_flash_lock(u32 *start, u32 nwords, int lock)
{
	int sb, eib;
	int rv;
	int i;
	u32 *addr;

#ifdef BLOB_DEBUG
	SerialOutputString(__FUNCTION__ "(): ");
	if(lock == 0)
		SerialOutputString("un");
	
	SerialOutputString("lock at 0x");
	SerialOutputHex((u32)start);
	SerialOutputString(", nwords = 0x");
	SerialOutputHex(nwords);
	serial_write('\n');
#endif

	rv = address_range_to_block_range((u32) start, nwords * sizeof(u32),
					  &sb, &eib);

	if(rv < 0)
		return rv;

	/* check if it is lockable at all */
	for(i = sb; i <= eib; i++) {
		if(!flash_blocks[i].lockable) {
#ifdef BLOB_DEBUG
			printerrprefix();
			SerialOutputString("can't (un)lock unlockable blocks\n");
#endif
			return -EFLASHPGM;
		}
	}
	
	flash_driver->enable_vpp();

	for(i = sb; i <= eib; i++) {
		addr = (u32 *)flash_blocks[i].start;

		if(lock)
			rv = flash_driver->lock_block(addr);
		else
			rv = flash_driver->unlock_block(addr);

		if(rv < 0) {
			flash_driver->disable_vpp();
#ifdef BLOB_DEBUG
			printerrprefix();
			SerialOutputString("can't (un)lock block at 0x");
			SerialOutputHex((u32)addr);
			serial_write('\n');
#endif
			return rv;
		}
	}
	
	flash_driver->disable_vpp();

	return 0;
}




int flash_lock_region(u32 *start, u32 nwords)
{
	return do_flash_lock(start, nwords, 1);
}




int flash_unlock_region(u32 *start, u32 nwords)
{
	return do_flash_lock(start, nwords, 0);
}




/* return number of blocks in the region locked, 0 if everything is
 * unlocked, or negative error number otherwise */
int flash_query_region(u32 *start, u32 nwords)
{
	int sb, eib, rv, i;
	int cnt = 0;
	u32 *addr;

	rv = address_range_to_block_range((u32) start, nwords * sizeof(u32),
					  &sb, &eib);

	if(rv < 0)
		return rv;

	for(i = sb; i <= eib; i++) {
		addr = (u32 *)flash_blocks[i].start;
		
		if(flash_blocks[i].lockable) {
			rv = flash_driver->query_block_lock(addr);

			if(rv < 0) 
				return rv;

			if(rv > 0)
				cnt++;
		}
	}

	return cnt;
}

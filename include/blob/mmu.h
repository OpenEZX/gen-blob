/*
 * include/blob/mmu.h
 *
 * header file for mmu.c
 *
 * Copyright (C) 2001  Motorola
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
 * 2001-Jan-01 - (Motorola) init draft
 *
 */

#ifndef BLOB_MMU_H
#define BLOB_MMU_H


struct mem_area
{
	unsigned long start;
	unsigned long size;
	unsigned char attr;
};

void mmu_init(void);
void enable_mmu(void);
void disable_mmu(void);
void set_uncache_unbuffer(struct mem_area *);


#endif

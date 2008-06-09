/*
 * memory.h: header file for memory.c
 *
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
 */

#ident "$Id: memory.h,v 1.2 2001/10/07 15:27:35 erikm Exp $"

#ifndef BLOB_MEMORY_H
#define BLOB_MEMORY_H

#include <blob/types.h>

/* The number of memory areas. Needs to be large to be able to detect
 * aliases.
 */
#define NUM_MEM_AREAS	(32)

typedef struct {
	u32 start;
	u32 len;
	int used;
} memory_area_t;




extern memory_area_t memory_map[NUM_MEM_AREAS];



void get_memory_map(void);




#endif

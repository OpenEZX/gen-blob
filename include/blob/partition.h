/*
 * partition.h: flash paritioning
 *
 * Copyright (C) 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 *
 * $Id: partition.h,v 1.2 2001/12/26 23:38:36 erikm Exp $
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

#ident "$Id: partition.h,v 1.2 2001/12/26 23:38:36 erikm Exp $"

#ifndef BLOB_PARTITION_H
#define BLOB_PARTITION_H


#include <blob/types.h>

#define PART_PARTITION_NAMELEN	(32)

typedef struct {
	u32 offset;		/* offset wrt start of flash */
	u32 size;		/* partition size */
	u32 mem_base;		/* load address in RAM */
	u32 entry_point;	/* entry point in RAM */
	u32 flags;
	char name[PART_PARTITION_NAMELEN];
} partition_t;

#define PART_VALID	(1<<0)
#define PART_LOAD	(1<<1)
#define PART_EXPAND	(1<<2)


#define PART_MAGIC (0x50727420)		/* "Prt " */
#define PART_MAX_PARTITIONS (16)	/* 16 partitions ought to be enough */

typedef struct {
	int magic;
	int numpartitions;
	partition_t partition[PART_MAX_PARTITIONS];
} partition_table_t;



#endif

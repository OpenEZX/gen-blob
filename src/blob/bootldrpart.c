/*
 * flashpart.c: bootldr compatible flash paritioning
 *
 * Copyright (C) 2001  Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 *
 * $Id: bootldrpart.c,v 1.2 2001/12/26 23:38:36 erikm Exp $
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
 * The bootldr partition information comes from the linux kernel
 * sources, file drivers/mtd/bootldr.c.
 */

/* $Id: bootldrpart.c,v 1.2 2001/12/26 23:38:36 erikm Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/types.h>
#include <blob/partition.h>
#include <blob/errno.h>
#include <blob/util.h>

#define BOOTLDR_PARTITION_NAMELEN 32
enum LFR_FLAGS {
	LFR_SIZE_PREFIX = 1,	/* prefix data with 4-byte size */
	LFR_PATCH_BOOTLDR = 2,	/* patch bootloader's 0th instruction */
	LFR_KERNEL = 4,		/* add BOOTIMG_MAGIC, imgsize and
				   VKERNEL_BASE to head of programmed
				   region (see bootldr.c) */
	/* LFR_KERNEL is actually never used
	 * so it's safe to ignore -- Erik */
	LFR_EXPAND = 8		/* expand partition size to fit rest
				   of flash */
};

typedef struct {
	char name[BOOTLDR_PARTITION_NAMELEN];
	unsigned long base;
	unsigned long size;
	enum LFR_FLAGS flags;
} FlashRegion;

typedef struct {
	int magic;		/* should be filled with 0x646c7470 (btlp)
				   BOOTLDR_PARTITION_MAGIC */
	int npartitions;

	/* the kernel code uses FlashRegion partition[0] over here,
	 * but because we want to allocate a partition table we'll
	 * have to use a certain maximum amount of partitions. */
	FlashRegion partition[PART_MAX_PARTITIONS];
} BootldrFlashPartitionTable;

#define BOOTLDR_MAGIC		0x646c7462	/* btld: marks a valid
						   bootldr image */
#define BOOTLDR_PARTITION_MAGIC	0x646c7470	/* btlp: marks a valid
						   bootldr partition table
						   in params sector */

#define BOOTLDR_MAGIC_OFFSET	0x20	/* offset 0x20 into the
					   bootldr */
#define BOOTCAP_OFFSET		0X30	/* offset 0x30 into the
					   bootldr */

#define BOOTCAP_WAKEUP	(1<<0)
#define BOOTCAP_PARTITIONS (1<<1)	/* partition table stored
					   in params sector */
#define BOOTCAP_PARAMS_AFTER_BOOTLDR (1<<2)	/* params sector right
						   after bootldr sector(s),
						   else in last sector */

/* used for construct_bootldr_partition_table() */
static BootldrFlashPartitionTable bootldr_ptable;

static BootldrFlashPartitionTable *find_bootldr_partition_table(void)
{
	BootldrFlashPartitionTable *table;

	/* we currently assume that the partition table lives in the
	 * PARAMETER flash block, so we only check if there really is
	 * a partition table. later on we can really search for it.
	 */
	table = (BootldrFlashPartitionTable *) PARAM_FLASH_BASE;

	if (table->magic == BOOTLDR_PARTITION_MAGIC)
		return table;
	else
		return NULL;
}

/* returns number of partitions, or negative error number otherwise */
int read_bootldr_partition_table(partition_table_t * ptable)
{
	BootldrFlashPartitionTable *bootldr;
	int i;

	/* get partition table */
	bootldr = find_bootldr_partition_table();

	/* is it real? */
	if (bootldr == NULL)
		return -EMAGIC;

	ptable->numpartitions = bootldr->npartitions;

	for (i = 0; i < bootldr->npartitions; i++) {
		ptable->partition[i].flags = PART_VALID;

		ptable->partition[i].offset = bootldr->partition[i].base;
		ptable->partition[i].size = bootldr->partition[i].size;
		ptable->partition[i].mem_base = 0;
		ptable->partition[i].entry_point = 0;

		strlcpy(ptable->partition[i].name, bootldr->partition[i].name,
			PART_PARTITION_NAMELEN);

		/* the caller should figure out the real size */
		if (bootldr->partition[i].flags & LFR_EXPAND)
			ptable->partition[i].flags |= PART_EXPAND;

		/* NOTE: this is a hack -- Erik */
		if (strncmp(ptable->partition[i].name, "kernel",
			    PART_PARTITION_NAMELEN) == 0) {
			ptable->partition[i].mem_base = KERNEL_RAM_BASE;
			ptable->partition[i].entry_point = KERNEL_RAM_BASE;
			ptable->partition[i].flags |= PART_LOAD;
		}

		/* NOTE: and this is a hack as well -- Erik */
		if (strncmp(ptable->partition[i].name, "ramdisk",
			    PART_PARTITION_NAMELEN) == 0) {
			ptable->partition[i].mem_base = RAMDISK_RAM_BASE;
			ptable->partition[i].flags |= PART_LOAD;
		}
	}

	return ptable->numpartitions;
}

int construct_bootldr_partition_table(const partition_table_t * src,
				      void **dst, int *len)
{
	int i;

	bootldr_ptable.magic = BOOTLDR_PARTITION_MAGIC;
	bootldr_ptable.npartitions = src->numpartitions;

	for (i = 0; i < src->numpartitions; i++) {
		bootldr_ptable.partition[i].flags = 0;

		strlcpy(bootldr_ptable.partition[i].name,
			src->partition[i].name, BOOTLDR_PARTITION_NAMELEN);
		bootldr_ptable.partition[i].base = src->partition[i].offset;
		bootldr_ptable.partition[i].size = src->partition[i].size;

		if (src->partition[i].flags & PART_EXPAND)
			bootldr_ptable.partition[i].flags |= LFR_EXPAND;
	}

	*dst = &bootldr_ptable;

	*len = sizeof(bootldr_ptable);

	return 0;
}

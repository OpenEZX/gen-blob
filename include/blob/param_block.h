/*-------------------------------------------------------------------------
 * Filename:      param_block.h
 * Version:       $Id: param_block.h,v 1.2 2001/10/07 15:27:35 erikm Exp $
 * Copyright:     Copyright (C) 2001, Russ Dill
 * Author:        Russ Dill <Russ.Dill@asu.edu>
 * Description:   Paramater block tags for blob
 * Created at:    Thu Aug 30 13:01:34 MST 2001
 *-----------------------------------------------------------------------*/
/*
 * param_block.h: Paramater block tags for blob
 * Adapted from linux/asm-arm/setup.h 
 *
 * Copyright (C) 2001 Russ Dill <Russ.Dill@asu.edu>
 * Copyright (C) 1995-2000 Russell King
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

/* $Id: param_block.h,v 1.2 2001/10/07 15:27:35 erikm Exp $ */


#ifndef BLOB_PARAM_BLOCK_H
#define BLOB_PARAM_BLOCK_H

#include <blob/types.h>

#define PTAG_MAGIC	0x32d27000 /* base 26 blob */

/* The list ends with an PTAG_NONE node. */
#define PTAG_NONE	0x00000000

struct ptag_header {
	u32 size;
	u32 ptag;
	
	/* the tag is proccessed if current_conf & conf_mask == conf */
	u32 conf_mask;
	u32 conf;
	
	/* if the tag fails, then current_conf |= fail_set_mask, and 
	 * current_config &= ~fail_clear_mask */ 
	u32 fail_set_mask;
	u32 fail_clear_mask;
};
        
/* The list must start with an PTAG_CORE node */
#define PTAG_CORE	(PTAG_MAGIC | 1)
        
struct ptag_core {
	/* important core paramaters go here */
	u32	terminal;	/* the baud rate at which communication with
				 * the user is performed */
};


/* Some options relating to ramdisks */
#define PTAG_RAMDISK	(PTAG_MAGIC | 2)
    
struct ptag_ramdisk {
	u32 	flags;		/* 1 = load ramdisk, 0 = do not */
};


/* Boot delay */
#define PTAG_BOOTDELAY	(PTAG_MAGIC | 3)
    
struct ptag_bootdelay {
	u32 	delay;		/* boot delay in seconds, 0 for none */
};


/* command line: \0 terminated string */
#define PTAG_CMDLINE	(PTAG_MAGIC | 4)
                
struct ptag_cmdline {
	char	cmdline[1];	/* this is the minimum size */
};


/* bootloader baudrates */
#define PTAG_BAUD	(PTAG_MAGIC | 5)
                
struct ptag_baud { /* see your local sa11x0 manual for acceptable values */
	u32	download;	/* the baud rate at which downloads occur */
};


/* splash screen */
#define PTAG_SPLASH	(PTAG_MAGIC | 6)
                
struct ptag_splash { 
	u32 	data[1];
};


/* network hardware addresses */
#define PTAG_HWADDR	(PTAG_MAGIC | 7)

struct ptag_hwaddr {
	char 	name[10]; 	/* device name (ie: eth0) */ 
	char 	hwaddr[6];
};


/* check a GPIO */
#define PTAG_GPIO	(PTAG_MAGIC | 8)

struct ptag_gpio {
	u32 	mask;
	u32	level;
};


struct ptag {
	struct ptag_header hdr;
	union {
		struct ptag_core	core;
		struct ptag_ramdisk	ramdisk;
		struct ptag_bootdelay	bootdelay;
		struct ptag_cmdline	cmdline;
		struct ptag_baud 	baud;
		struct ptag_splash	splash;
		struct ptag_hwaddr	hwaddr;
		struct ptag_gpio	gpio;
	} u;
};

struct ptagtable {
	u32 ptag;
	int (*parse)(const struct ptag *);
};

#define __ptag __attribute__((unused, __section__(".ptaglist")))
#define __ptagtable(ptag, fn) \
static struct ptagtable __ptagtable_##fn __ptag = { ptag, fn }

#define ptag_member_present(ptag,member)				\
	((unsigned long)(&((struct ptag *)0L)->member + 1)	\
		<= (ptag)->hdr.size * 4)

#define ptag_next(t)	((struct ptag *)((u32 *)(t) + (t)->hdr.size))
#define ptag_size(type)	((sizeof(struct ptag_header) + sizeof(struct type)) >> 2)

#define for_each_ptag(t,base)		\
	for (t = base; t->hdr.size; t = ptag_next(t))

int parse_ptag(const struct ptag *ptag, u32 *conf);
void parse_ptags(void *t, u32 *conf);

#endif

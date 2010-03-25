/*
 * icache.c: i-cache handling
 *
 * Copyright (C) 2001  Russ Dill <Russ.Dill@asu.edu>
 *
 * $Id: icache.c,v 1.3 2001/10/07 20:16:57 erikm Exp $
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

/* $Id: icache.c,v 1.3 2001/10/07 20:16:57 erikm Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/icache.h>
#include <blob/types.h>



void enable_icache(void)
{
	register u32 i;
	
	/* read control register */
	asm ("mrc p15, 0, %0, c1, c0, 0": "=r" (i));

	/* set i-cache */
	i |= 0x1000;

	/* write back to control register */
	asm ("mcr p15, 0, %0, c1, c0, 0": : "r" (i));
}




void disable_icache(void)
{
	register u32 i;

	/* read control register */
	asm ("mrc p15, 0, %0, c1, c0, 0": "=r" (i));

	/* clear i-cache */
	i &= ~0x1000;

	/* write back to control register */
	asm ("mcr p15, 0, %0, c1, c0, 0": : "r" (i));

	/* flush i-cache */
	asm ("mcr p15, 0, %0, c7, c5, 0": : "r" (i));
}

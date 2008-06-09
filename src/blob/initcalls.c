/*
 * src/blob/initcalls.c
 *
 * init and exit calls for blob
 *
 * Copyright (C) 2003, 2005 Motorola Inc.
 * Copyright (C) 2001  Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 *
 * $Id: initcalls.c,v 1.3 2002/01/03 16:07:17 erikm Exp $
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
 * 2003-Oct-17 - (Motorola) Update initlist to enable mmu
 * 2005-Jun-14 - (Motorola) Modified for EZX platform
 *
 */

#ident "$Id: initcalls.c,v 1.3 2002/01/03 16:07:17 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/init.h>
#include <blob/icache.h>
#include <blob/mmu.h>
#include <blob/led.h>
#include <blob/serial.h>
#include <blob/time.h>

/* default serial initialisation */
static void serial_default_init(void)
{
	serial_init(baud_9600);
}


#if 0
/* init calls */
__initlist(serial_default_init,	INIT_LEVEL_INITIAL_HARDWARE);
__initlist(enable_icache,	INIT_LEVEL_INITIAL_HARDWARE);
__initlist(led_init,		INIT_LEVEL_INITIAL_HARDWARE);
__initlist(TimerInit,		INIT_LEVEL_OTHER_HARDWARE);

/* exit calls */
__exitlist(disable_icache,	INIT_LEVEL_INITIAL_HARDWARE);
#endif

/* init calls */
#ifndef MBM
__initlist(mmu_init,		INIT_LEVEL_INITIAL_HARDWARE);
#endif
__initlist(led_init,		INIT_LEVEL_INITIAL_HARDWARE);

/* exit calls */
#ifndef MBM
__exitlist(disable_mmu,		INIT_LEVEL_INITIAL_HARDWARE);
#endif

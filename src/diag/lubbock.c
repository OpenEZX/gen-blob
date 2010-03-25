/*
 * assabet.c: Assabet specific stuff
 *
 * Copyright (C) 2002  Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 *
 * $Id: assabet.c,v 1.1 2002/01/03 16:07:18 erikm Exp $
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

/* $Id: assabet.c,v 1.1 2002/01/03 16:07:18 erikm Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/init.h>
#include <blob/serial.h>




static void lubbock_init_hardware(void)
{
	/* select serial driver */
	serial_driver = &pxa_serial_driver;
}

__initlist(lubbock_init_hardware, INIT_LEVEL_DRIVER_SELECTION);

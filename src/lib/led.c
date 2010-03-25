/*-------------------------------------------------------------------------
 * Filename:      led.c
 * Version:       $Id: led.c,v 1.5 2001/10/15 21:27:05 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   
 * Created at:    Mon Aug 16 15:39:12 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Mon Aug 16 16:55:21 1999
 *-----------------------------------------------------------------------*/
/*
 * led.c: simple LED control functions
 *
 * Copyright (C) 1999 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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
 * All functions assume that the LED is properly initialised by the code
 * in ledasm.S.
 *
 */

/* $Id: led.c,v 1.5 2001/10/15 21:27:05 erikm Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/led.h>
#include <blob/init.h>



static int led_state;
static int led_locked;



void led_init(void)
{
	led_unlock();
	led_on();
}




void led_on(void)
{
	if(led_locked)
		return;

	//GPSR = LED_GPIO;
	led_state = 1;
}




void led_off(void)
{
	if(led_locked)
		return;

	//GPCR = LED_GPIO;
	led_state = 0;
}




void led_toggle(void)
{
	if(led_state)
		led_off();
	else
		led_on();
}




void led_lock(void)
{
	led_locked = 1;
}




void led_unlock(void)
{
	led_locked = 0;
}

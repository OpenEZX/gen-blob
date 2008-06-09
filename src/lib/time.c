/*-------------------------------------------------------------------------
 * Filename:      time.c
 * Version:       $Id: time.c,v 1.3 2001/10/07 20:16:57 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Some easy timer functions for blob
 * Created at:    Tue Aug 24 21:08:25 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sun Oct  3 21:10:21 1999
 *-----------------------------------------------------------------------*/
/*
 * timer.c: Timer functions for blob
 *
 * Copyright (C) 1999 2000 2001  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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

#ident "$Id: time.c,v 1.3 2001/10/07 20:16:57 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/init.h>
#include <blob/led.h>
#include <blob/time.h>


static int numOverflows;


void TimerInit(void)
{
	/* clear counter */
	OSCR = 0;

	/* we don't want to be interrupted */
	OIER = 0;

	/* wait until OSCR > 0 */
	while( OSCR == 0)
		;

	/* clear match register 0 */
	OSMR0 = 0;

	/* clear match bit for OSMR0 */
	OSSR = OSSR_M0;

	numOverflows = 0;
}




/* returns the time in 1/TICKS_PER_SECOND seconds */
u32  TimerGetTime(void)
{
	/* turn LED always on after one second so the user knows that
         * the board is on
	 */
	if((OSCR % TICKS_PER_SECOND) < (TICKS_PER_SECOND >>7))
		led_on();

	return((u32) OSCR);
}




int TimerDetectOverflow(void)
{
	return( OSSR & OSSR_M0);
}



void TimerClearOverflow(void)
{
	if(TimerDetectOverflow())
		numOverflows++;

	OSSR = OSSR_M0;
}



void msleep(unsigned int msec)
{
	u32 ticks, start, end;
	int will_overflow = 0;
	int has_overflow = 0;
	int reached = 0;
	
	if(msec == 0)
		return;

	ticks = (TICKS_PER_SECOND * msec) / 1000;
	
	start = TimerGetTime();

	/* this could overflow, but it nicely wraps around which is
         * exactly what we want
	 */
	end = start + ticks;
	
	/* detect the overflow */
	if(end < start) {
		TimerClearOverflow();
		will_overflow = 1;
	}
	
	do {
		if(will_overflow && !has_overflow) {
			if(TimerDetectOverflow())
				has_overflow = 1;

			continue;
		}

		if(TimerGetTime() >= end)
			reached = 1;
	} while(!reached);
}

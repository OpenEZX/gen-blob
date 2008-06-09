/*-------------------------------------------------------------------------
 * Filename:      clock.c
 * Version:       $Id: clock.c,v 1.3 2001/10/07 23:01:08 erikm Exp $
 * Copyright:     Copyright (C) 2000, Johan Pouwelse
 * Author:        Johan Pouwelse <pouwelse@twi.tudelft.nl>
 * Description:   Clock switching functions
 * Created at:    Sat Mar 25 14:11:22 2000
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sat Mar 25 14:12:42 2000
 *-----------------------------------------------------------------------*/
/*
 * clock.c: Utility to set clock speed and DRAM parameters 
 *
 * Copyright (C) 2000  Johan Pouwelse (pouwelse@twi.tudelft.nl)
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

#ident "$Id: clock.c,v 1.3 2001/10/07 23:01:08 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/command.h>
#include <blob/errno.h>
#include <blob/error.h>
#include <blob/types.h>
#include <blob/sa1100.h>
#include <blob/serial.h>
#include <blob/time.h>
#include <blob/util.h>


/* Struct with the SA-1100 PLL + DRAM parameter registers */
enum {
	SA_PPCR,
	SA_MDCNFG,
	SA_MDCAS0,
	SA_MDCAS1,
	SA_MDCAS2
};




int SetClock(int argc, char *argv[])
{
	int i;
	u32 regs[5];
	u32 startTime, currentTime;
	
	if(argc < 6)
		return -ENOPARAMS;

	for(i = 0; i < 5; i++) {
		if(strtou32(argv[i + 1], &regs[i]) < 0) {
			printerror(ENAN, argv[i + 1]);
			return 0;
		}
	}
			
	/* we go slower, so first set PLL register */
	PPCR = regs[SA_PPCR];
	MDCNFG = regs[SA_MDCNFG];
	MDCAS0 = regs[SA_MDCAS0];
	MDCAS1 = regs[SA_MDCAS1];
	MDCAS2 = regs[SA_MDCAS2];
	
	/* sleep for a second */
	startTime = TimerGetTime();
	
	for(;;) {
		currentTime = TimerGetTime();
		if((currentTime - startTime) > (u32)TICKS_PER_SECOND)
			return 0;
	}
} /* SetClock */


static char clockhelp[] = "clock PPCR MDCNFG MDCAS0 MDCAS1 MDCAS2\n"
"Set the SA1100 core clock and DRAM timings\n"
"WARNING: dangerous command!\n";

__commandlist(SetClock, "clock", clockhelp);

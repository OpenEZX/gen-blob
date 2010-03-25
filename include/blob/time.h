/*-------------------------------------------------------------------------
 * Filename:      time.h
 * Version:       $Id: time.h,v 1.2 2001/10/07 20:16:57 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Timer functions
 * Created at:    Tue Aug 24 21:08:24 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Tue Sep 28 23:44:11 1999
 *-----------------------------------------------------------------------*/
/*
 * time.h: header file for time.c
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

/* $Id: time.h,v 1.2 2001/10/07 20:16:57 erikm Exp $ */

#ifndef BLOB_TIME_H
#define BLOB_TIME_H




#include <blob/types.h>




#define TICKS_PER_SECOND 3686400




void TimerInit(void);

/* returns the time in 1/TICKS_PER_SECOND seconds */
u32  TimerGetTime(void);

int TimerDetectOverflow(void);

void TimerClearOverflow(void);

/* suspend execution for interval of milliseconds */
void msleep(unsigned int msec);



#endif

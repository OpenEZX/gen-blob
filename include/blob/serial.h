/*-------------------------------------------------------------------------
 * Filename:      serial.h
 * Version:       $Id: serial.h,v 1.5 2002/01/05 20:14:34 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Header file for serial.c
 * Created at:    Tue Aug 24 20:25:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Tue Sep 28 23:43:46 1999
 *-----------------------------------------------------------------------*/
/*
 * serial.h: Serial utilities for blob
 * Copyright (C) 2003-2005 Motorola Inc.
 * Copyright (C) 1999 2002  Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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
 * 2003-Oct-20 - (Motorola) Conditional serial output
 * 2005-Dec_14 - (Motorola) For ezx platform
 */

#ident "$Id: serial.h,v 1.5 2002/01/05 20:14:34 erikm Exp $"

#ifndef BLOB_SERIAL_H
#define BLOB_SERIAL_H


#include <blob/types.h>


typedef enum {
	baud_1200 	= 1200,
	baud_2400 	= 2400,
	baud_4800 	= 4800,
	baud_9600 	= 9600,
	baud_19200 	= 19200,
	baud_38400 	= 38400,
	baud_57600 	= 57600,
	baud_115200	= 115200,
	baud_230400	= 230400
} serial_baud_t;
	

typedef int (*serial_init_func_t)(serial_baud_t);
typedef int (*serial_read_func_t)(void);
typedef int (*serial_write_func_t)(int);
typedef int (*serial_poll_func_t)(void);
typedef int (*serial_flush_input_func_t)(void);
typedef int (*serial_flush_output_func_t)(void);


typedef struct {
	serial_init_func_t init;

	serial_read_func_t read;
	serial_write_func_t write;

	serial_poll_func_t poll;

	serial_flush_input_func_t flush_input;
	serial_flush_output_func_t flush_output;
} serial_driver_t;


/* implemented serial drivers */
extern serial_driver_t sa11x0_serial_driver;
extern serial_driver_t pxa_serial_driver;


/* should be filled out by the architecture dependent files */
extern serial_driver_t *serial_driver;


#ifdef SERIAL_OUTPUT
/* exported functions */
int serial_init(serial_baud_t baudrate);
int serial_read(void);
int serial_write(int c);
int serial_write_raw(int c);
int serial_poll(void);
int serial_flush_input(void);
int serial_flush_output(void);

void SerialOutputString(const char *s);
void SerialOutputHex(const u32 h);
void SerialOutputDec(const u32 d);
void SerialOutputBlock(const char *buf, int bufsize);

int  SerialInputString(char *s, const int len, const int timeout);
int  SerialInputBlock(char *buf, int bufsize, const int timeout);

#else

#define serial_init(baudrate)			
#define serial_read()				(0)
#define serial_write(c)				
#define serial_write_raw(c)			(0)
#define serial_poll()				(0)
#define serial_flush_input()			(0)
#define serial_flush_output()			

#define SerialOutputString(s)
#define SerialOutputHex(h)
#define SerialOutputDec(d)
#define SerialOutputBlock(buf, bufsize)

#define SerialInputString(s, len, timeout)	(0)
#define SerialInputBlock(buf, size, timeout)	(0)

#endif

#endif

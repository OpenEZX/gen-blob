/*-------------------------------------------------------------------------
 * Filename:      serial.c
 * Version:       $Id: serial.c,v 1.7 2002/01/05 20:14:34 erikm Exp $
 * Copyright:     Copyright (C) 1999, Erik Mouw
 * Author:        Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Description:   Serial utilities for blob
 * Created at:    Tue Aug 24 20:25:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Mon Oct  4 20:11:14 1999
 *-----------------------------------------------------------------------*/
/*
 * serial.c: Serial utilities for blob
 *
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
 * Copyright (C) 2002-2005 Motorola
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
 * Oct 30,2002 - (Motorola) Add serial output support for hex.
 *                          Added ifdef for serial output
 *
 */

#ident "$Id: serial.c,v 1.7 2002/01/05 20:14:34 erikm Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/errno.h>
#include <blob/led.h>
#include <blob/serial.h>
#include <blob/time.h>

serial_driver_t *serial_driver;


#ifdef SERIAL_OUTPUT

/* initialise serial port at the request baudrate. returns 0 on
 * success, or a negative error number on failure
 */
int serial_init(serial_baud_t baudrate)
{
#ifdef BLOB_DEBUG
	if(serial_driver == NULL) {
		/* we can't print anything over here, so just return
		 * an error and hope the developer will figure out.
		 */
		return -ERANGE;
	}
#endif

	return serial_driver->init(baudrate);
}

/* read one character from the serial port. return character (between
 * 0 and 255) on success, or negative error number on failure. this
 * function is blocking */
int serial_read(void)
{
	return serial_driver->read();
}

/* write character to serial port and replace all \n characters by a
 * \n\r sequence. return 0 on success, or negative error number on
 * failure. this function is blocking
 */
int serial_write(int c)
{
	int rv;

	rv = serial_driver->write(c);

	/* if \n, also do \r */
	if(c == '\n')
		rv = serial_driver->write('\r');

	return rv;
}

/* write character to serial port and do not replace any
 * characters. return 0 on success, or negative error number on
 * failure. this function is blocking
 */
int serial_write_raw(int c)
{
	return serial_driver->write(c);
}

/* check if there is a character available to read. returns 1 if there
 * is a character available, 0 if not, and negative error number on
 * failure */
int serial_poll(void)
{
	return serial_driver->poll();
}

/* flush serial input queue. returns 0 on success or negative error
 * number otherwise
 */
int serial_flush_input(void)
{
	return serial_driver->flush_input();
}

/* flush output queue. returns 0 on success or negative error number
 * otherwise
 */
int serial_flush_output(void)
{
	return serial_driver->flush_output();
}

/*
 * Write a null terminated string to the serial port.
 */
void SerialOutputString(const char *s)
{
	while(*s != 0)
		serial_write(*s++);
} /* SerialOutputString */

/*
 * Write the argument of the function in hexadecimal to the serial
 * port. If you want "0x" in front of it, you'll have to add it
 * yourself.
 */
void SerialOutputHex(const u32 h)
{
	char c;
	int i;

	for(i = NIBBLES_PER_WORD - 1; i >= 0; i--) {
		c = (char)((h >> (i * 4)) & 0x0f);

		if(c > 9)
			c += ('a' - 10);
		else
			c += '0';

		serial_write(c);
	}
}


void SerialOutputHexU8(const u8 h)
{
	 char c;
        int i;

        for(i = 2 - 1; i >= 0; i--) {
                c = (char)((h >> (i * 4)) & 0x0f);

                if(c > 9)
                        c += ('A' - 10);
                else
                        c += '0';

                serial_write(c);
        }
}

/*
 * Write the argument of the function in decimal to the serial port.
 * We just assume that each argument is positive (i.e. unsigned).
 */
void SerialOutputDec(const u32 d)
{
	int leading_zero = 1;
	u32 divisor, result, remainder;

	remainder = d;

	for(divisor = 1000000000;
	    divisor > 0;
	    divisor /= 10) {
		result = remainder / divisor;
		remainder %= divisor;

		if(result != 0 || divisor == 1)
			leading_zero = 0;

		if(leading_zero == 0)
			serial_write((char)(result) + '0');
	}
}

/*
 * Write a block of data to the serial port. Similar to
 * SerialOutputString(), but this function just writes the number of
 * characters indicated by bufsize and doesn't look at termination
 * characters.
 */
void SerialOutputBlock(const char *buf, int bufsize)
{
	while(bufsize--)
		serial_write(*buf++);
}

/*
 * read a string with maximum length len from the serial port
 * using a timeout of timeout seconds
 *
 * len is the length of array s _including_ the trailing zero,
 * the function returns the number of bytes read _excluding_
 * the trailing zero
 */
int  SerialInputString(char *s, const int len, const int timeout)
{
	u32 startTime, currentTime;
	int c;
	int i;
	int numRead;
	int skipNewline = 1;
	int maxRead = len - 1;

	startTime = TimerGetTime();

	for(numRead = 0, i = 0; numRead < maxRead;) {
		/* try to get a byte from the serial port */
		while(serial_poll() == 0) {
			currentTime = TimerGetTime();

			/* check timeout value */
			if((currentTime - startTime) >
			   (timeout * TICKS_PER_SECOND)) {
				/* timeout */
				s[i++] = '\0';
				return(numRead);
			}
		}

		c = serial_read();

		/* check for errors */
		if(c < 0) {
			s[i++] = '\0';
			return c;
		}

		/* eat newline characters at start of string */
		if((skipNewline == 1) && (c != '\r') && (c != '\n'))
			skipNewline = 0;

		if(skipNewline == 0) {
			if((c == '\r') || (c == '\n')) {
				s[i++] = '\0';
				return(numRead);
			} else {
				s[i++] = (char)c;
				numRead++;
			}
		}
	}

	return(numRead);
}

/*
 * SerialInputBlock(): almost the same as SerialInputString(), but
 * this one just reads a block of characters without looking at
 * special characters.
 */
int  SerialInputBlock(char *buf, int bufsize, const int timeout)
{
	u32 startTime, currentTime;
	int c;
	int i;
	int numRead;
	int maxRead = bufsize;

	startTime = TimerGetTime();

	for(numRead = 0, i = 0; numRead < maxRead;) {
		/* try to get a byte from the serial port */
		while(serial_poll() == 0) {
			currentTime = TimerGetTime();

			/* check timeout value */
			if((currentTime - startTime) >
			   (timeout * TICKS_PER_SECOND)) {
				/* timeout! */
				return(numRead);
			}
		}

		c = serial_read();

		/* check for errors */
		if(c < 0)
			return c;

		buf[i++] = c;
		numRead ++;
	}

	return(numRead);
}

#endif

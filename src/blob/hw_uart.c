/*
 * src/blob/hw_uart.c
 *
 * Uart functions for rdl
 * 
 * Copyright (C) 2002, 2004, 2005 Motorola Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as 
 * published by the Free Software Foundation.
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
 * 2002-Jul-01 - (Motorola) Create
 * 2004-Feb-09 - (Motorola) Porting for bulverde rdl
 * 2005-Dec-14 - (Motorola) Modified for EZX platform
 */

#include "hw_uart.h"
#include "time.h"
#include "pxa.h"

#ifdef ZQ_DEBUG
#include "serial.h"
#endif /* ZQ_DEBUG */

#define  XTAL	14745600

void hwuart_init(int baud)
{
	int divisor = XTAL / (baud << 4);

//      This is HWUART (init with flow control)
	GPSR1 &= ~0x000f0000;
	GPSR1 |= 0x00010000;
	GPCR1 &= ~0x000f0000;
	GPCR1 |= 0x00080000;
	GPDR1 &= ~0x000f0000;
	GPDR1 |= 0x00090000;
	GAFR1_U &= ~0x000000ff;
	GAFR1_U |= 0x00000055;

	CKEN |= CKEN4_HWUART;

	while ((HWLSR & (0x1 << 5)) == 0) ;
//      HWLCR = 0x00;

//  DLAB = 1    1 stop bit  8 bit characters
	HWLCR = 0x83;

//  Setup divisor
	HWDLL = divisor & 0xff;
	HWDLH = (divisor & 0xff00) >> 8;
//      DLAB = 0 to allow access to FIFOs
	HWLCR = 0x03;
//  set Interrupt Trigger Level to be 1 byte
	HWFCR = 0x07;
//  disabled NRZ, disable DMA requests and enable UART 
	HWIER = 0x40;
//  Enable autoflow control
	HWMCR = 0x22;
}

void HwuartOutputByte(const char c)
{
	/* wait for room in the tx FIFO */
	while ((HWLSR & 0x40) == 0) ;

	HWRBR = c;

}

void HwuartOutputString(const char *s)
{

	while (*s != 0)
		HwuartOutputByte(*s++);

}

void HwuartOutputHex(const u32 h)
{
	char c;
	int i;

	for (i = NIBBLES_PER_WORD - 1; i >= 0; i--) {
		c = (char)((h >> (i * 4)) & 0x0f);

		if (c > 9)
			c += ('A' - 10);
		else
			c += '0';

		HwuartOutputByte(c);
	}
}

void HwuartOutputDec(const u32 d)
{
	int leading_zero = 1;
	u32 divisor, result, remainder;

	remainder = d;

	for (divisor = 1000000000; divisor > 0; divisor /= 10) {
		result = remainder / divisor;
		remainder %= divisor;

		if (result != 0 || divisor == 1)
			leading_zero = 0;

		if (leading_zero == 0)
			HwuartOutputByte((char)(result) + '0');
	}
}

void HwuartOutputBlock(const char *buf, int bufsize)
{
	while (bufsize--)
		HwuartOutputByte(*buf++);
}

int HwuartInputByte(char *c)
{
	if (HWLSR & 0x01) {
		int err = HWLSR & (0x0e);
		*c = (char)HWRBR;

		/* We currently only care about framing and parity errors */
		if ((err & (0x08 | 0x04 | 0x02)) != 0) {
			return HwuartInputByte(c);
		} else {
			return (1);
		}
	} else {
		/* no bit ready */
		return (0);
	}
}

int HwuartInputString(char *s, const int len, const int timeout)
{
	u32 startTime, currentTime;
	char c;
	int i;
	int numRead;
	int skipNewline = 1;
	int maxRead = len - 1;

	startTime = TimerGetTime();

	for (numRead = 0, i = 0; numRead < maxRead;) {
		/* try to get a byte from the serial port */
		while (!HwuartInputByte(&c)) {
			currentTime = TimerGetTime();

			/* check timeout value */
			if ((currentTime - startTime) >
			    (timeout * TICKS_PER_SECOND)) {
				/* timeout */
				s[i++] = '\0';
				return (numRead);
			}
		}

		/* eat newline characters at start of string */
		if ((skipNewline == 1) && (c != '\r') && (c != '\n'))
			skipNewline = 0;

		if (skipNewline == 0) {
			if ((c == '\r') || (c == '\n')) {
				s[i++] = '\0';
				return (numRead);
			} else {
				s[i++] = c;
				numRead++;
			}
		}
	}

	return (numRead);
}

int HwuartInputBlock(char *buf, int bufsize, const int timeout)
{
	u32 startTime, currentTime;
	char c;
	int i;
	int numRead;
	int maxRead = bufsize;

	startTime = TimerGetTime();

	for (numRead = 0, i = 0; numRead < maxRead;) {
		/* try to get a byte from the serial port */
		while (!HwuartInputByte(&c)) {
			currentTime = TimerGetTime();

			/* check timeout value */
			if ((currentTime - startTime) >
			    (timeout * TICKS_PER_SECOND / 10)) {
				/* timeout! */
				return (numRead);
			}
		}
		buf[i++] = c;
		numRead++;
	}

	return (numRead);
}

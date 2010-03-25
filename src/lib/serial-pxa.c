/*
 * serial-pxa.c: StrongARM SA11x0 serial port driver
 *
 * Copyright (C) 2002  Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 *
 * $Id: serial-pxa.c,v 1.2 2002/01/03 16:07:18 erikm Exp $
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

/* $Id: serial-pxa.c,v 1.2 2002/01/03 16:07:18 ytang5 Exp $ */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/errno.h>
#include <blob/serial.h>
#include <blob/types.h>

/*
 * PXA UART works in non-FIFO mode.
 */

//#define LUB_REG(x)	(*(volatile unsigned long*)(x))

#define UART_RBR	FFRBR
#define UART_THR	FFTHR
#define UART_LCR	FFLCR
#define UART_LSR 	FFLSR
#define UART_IER	FFIER
#define UART_FCR	FFFCR
#define UART_DLL	FFDLL
#define UART_DLH	FFDLH
#define UART_MCR	FFMCR

#define  XTAL	14745600

static int pxa_serial_change_speed(int baud) {
	int divisor = XTAL / (baud<<4);

	UART_LCR |= LCR_DLAB;

	UART_DLL = (divisor & 0xFF);
	UART_DLH = (divisor >> 8);

	UART_LCR &= ~LCR_DLAB;

	return 0;
}

/* Flush serial input queue. 
 * Returns 0 on success or negative error number otherwise
 */
static int pxa_serial_flush_input(void)
{
	volatile u32 tmp;

	/* keep on reading as long as the receiver is not empty */
	while( UART_LSR & LSR_DR ) {
		if( UART_LSR & ( LSR_PE | LSR_FE | LSR_BI) )
			return -ESERIAL;

		tmp = UART_RBR;
	}

	return 0;
}




/* Flush output queue. 
 * Returns 0 on success or negative error number otherwise
 */
static int pxa_serial_flush_output(void)
{
	/* wait until the transmitter is ready to transmit*/
	while( !(UART_LSR & LSR_TDRQ) );

	return 0;
}

/* Initialise serial port at the request baudrate. 
 * Returns 0 on success, or a negative error number otherwise.
 *
 * 		    147.7456 MHZ
 * BaudRate  =   ----------------
 * 		    16 x Divisor
 *
 * thus, if divisor is 24, the baud rate is 38400 bps.
 */
static int pxa_serial_init(serial_baud_t baud)
{
	CKEN |= CKEN6_FFUART;
	/* set the port to sensible defaults (no break, no interrupts,
	 * no parity, 8 databits, 1 stopbit, transmitter and receiver
	 * enabled)
	 */
	UART_LCR = 0x3;
	
	/* assert DTR & RTS to avoid problems of hardware handshake
	 * with serial terminals 
	 */
	UART_MCR = MCR_DTR | MCR_RTS ;

	/* disenable FIFO */
	UART_FCR = FCR_TRFIFOE;

	/* enable UART  */
	UART_IER = IER_UUE;

	pxa_serial_change_speed(baud);

	pxa_serial_flush_output();

	return 0;
}




/* Check if there is a character available to read. 
 * Returns 1 if there is a character available, 0 if not, 
 * and negative error number on failure.
 */
static int pxa_serial_poll(void)
{
	/* check for errors */
	if( UART_LSR & ( LSR_PE | LSR_FE | LSR_BI) )
		return -ESERIAL;

	if ( UART_LSR & LSR_DR )
		return 1;
	else
		return 0;
}




/* read one character from the serial port. return character (between
 * 0 and 255) on success, or negative error number on failure. this
 * function is blocking */
static int pxa_serial_read(void)
{
	int rv;
	
	for(;;) {
		rv = pxa_serial_poll();

		if(rv < 0)
			return rv;

		if(rv > 0)
			return  UART_RBR & 0xff;
	}
}




/* write character to serial port. return 0 on success, or negative
 * error number on failure. this function is blocking
 */
static int pxa_serial_write(int c)
{
	/* wait for room in the transmit FIFO */
	while( (UART_LSR & LSR_TDRQ) == 0 )  {
	}

	UART_THR = c & 0xff;

	return 0;
}

/* export serial driver */
serial_driver_t pxa_serial_driver = {
	init:		pxa_serial_init,
	read:		pxa_serial_read,
	write:		pxa_serial_write,
	poll:		pxa_serial_poll,
	flush_input:	pxa_serial_flush_input,
	flush_output:	pxa_serial_flush_output
};

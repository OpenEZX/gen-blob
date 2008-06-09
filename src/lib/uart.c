/*
 * uart.c:
 *
 * UART function
 *
 * Copyright (C) 2001-2005  Motorola
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
 * Oct 30,2002 - (Motorola) Created
 *
 */
#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/types.h>
#include <blob/pxa.h>


#define BT_CKEN7   0x00000080           // BT

void bt_uart_init(void)
{
        CKEN |= BT_CKEN7;

	GPSR1 |= 0x00000800;
	GPDR1 &= ~0x00003c00;
	GPDR1 |= 0x00000800;
	GAFR1_L &= ~0x0ff00000;
	GAFR1_L |= 0x00900000;

        //SerialOutputString("\n init BT uart start!!! \n  ");

	while(( BTLSR & (0x1 << 5)) == 0) ;

        BTLCR = 0x83;
	BTDLL = 0x08;
        BTDLH = 0;
        BTLCR = 0x03;

	/* disenable FIFO */
	BTFCR = 0x06;

	/* enable UART  */
	BTIER = 0x40;
        while((BTLSR &0x40) ==0);
}

/*
 * Output a single byte to the serial port.
 */
void BTSerialOutputByte(const char c)
{
	/* wait for room in the tx FIFO */
	while(( BTLSR & 0x40) == 0) ;

	BTTHR = c;

	/* If \n, also do \r */
	if(c == '\n')
	{
		while (( BTLSR & 0x40) == 0);
			BTTHR = '\r';
	}
}

/*
 * Write a null terminated string to the serial port.
 */
void BTSerialOutputString(const char *s)
{
	while(*s != 0)
		BTSerialOutputByte(*s++);

}

/*
 * Write the argument of the function in hexadecimal to the serial
 * port. If you want "0x" in front of it, you'll have to add it
 * yourself.
 */
void BTSerialOutputHex(const u32 h)
{
	char c;
	int i;

	for(i = NIBBLES_PER_WORD - 1; i >= 0; i--) {
		c = (char)((h >> (i * 4)) & 0x0f);

		if(c > 9)
			c += ('A' - 10);
		else
			c += '0';

		BTSerialOutputByte(c);
	}
}

/*
 * Write the argument of the function in decimal to the serial port.
 * We just assume that each argument is positive (i.e. unsigned).
 */
void BTSerialOutputDec(const u32 d)
{
	int leading_zero = 1;
	u32 divisor, result, remainder;

	remainder = d;

	for(divisor = 1000000000; divisor > 0; divisor /= 10) {
		result = remainder / divisor;
		remainder %= divisor;

		if(result != 0 || divisor == 1)
			leading_zero = 0;

		if(leading_zero == 0)
			BTSerialOutputByte((char)(result) + '0');
	}
}


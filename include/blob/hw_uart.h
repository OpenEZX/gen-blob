/*
 * include/blob/hw_uart.h
 *
 * Copyright (C) 2002, 2005 Motorola Inc.
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
 * 2002-Oct-30 - (Motorola) Created
 * 2005-Dec-14 - (Motorola) Add change for motorola products
 *
 */


#include "types.h"

extern void hwuart_init(int baud);

extern void HwuartOutputByte(const char c);

extern void HwuartOutputString(const char *s);

extern void HwuartOutputHex(const u32 h);

extern void HwuartOutputDec(const u32 d);

extern void HwuartOutputBlock(const char *buf, int bufsize);

extern int HwuartInputByte(char *c);

extern int  HwuartInputString(char *s, const int len, const int timeout);

extern int  HwuartInputBlock(char *buf, int bufsize, const int timeout);



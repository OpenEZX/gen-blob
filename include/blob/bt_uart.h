/*
 * include/blob/bt_uart.h
 *
 * header file for bt_uart.c
 *
 * Copyright (C) 2001, 2003  Motorola
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
 * 2001-Feb-7  - (Motorola) Created
 * 2003-Mar-10 - (Motorola) for EZX platform
 */
#ifndef BT_UART_H
#define BT_UART_H

#ifdef OUTPUT_LOG_TO_BTPORT

void bt_uart_init(void);
void BTSerialOutputByte(const char c);
void BTSerialOutputString(const char *s); 
void BTSerialOutputHex(const u32 h);
void BTSerialOutputDec(const u32 d);

#else

#define bt_uart_init()
#define BTSerialOutputByte(c)
#define BTSerialOutputString(s) 
#define BTSerialOutputHex(h)
#define BTSerialOutputDec(d)

#endif

#endif

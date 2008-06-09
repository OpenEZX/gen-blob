/**********************************************************************
 * lcd.h
 *
 * AUTOR:	SELETZ
 *
 * Generic lcd framework
 *
 * Copyright (C) 2001 Stefan Eletzhofer <stefan.eletzhofer@www.eletztrick.net>
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
 */

#ifndef BLOB_LCD_H
#define BLOB_LCD_H

#if defined(CONFIG_LCD_SUPPORT)

#ifdef PT_SYSTEM3
#	define	LCD_LCCR0	0x000AA03C
#	define	LCD_LCCR1	0x01000270
#	define	LCD_LCCR2	0x000004EF
#	define	LCD_LCCR3	0x0000FF16
#	define	LCD_BPP			8
#	define	LCD_COLS		640
#	define	LCD_ROWS		480
#	define	LCD_VIDEORAM_SIZE	((LCD_BPP*LCD_ROWS*LCD_COLS)>>3)
#	define	LCD_PALETTE_ENTRIES	256
#	define	LCD_PALETTE_SIZE	(LCD_PALETTE_ENTRIES * 2)
#	define	LCD_RAM_BASE		0xd0000000
#	define	LCD_PALETTE_DMA_ADR	(LCD_RAM_BASE + 0x0)
#	define	LCD_VIDEORAM_START	(LCD_RAM_BASE + LCD_PALETTE_SIZE)
#	define	LCD_VIDEORAM_END	(LCD_VIDEORAM_START + LCD_VIDEORAM_SIZE)
#	define	LCD_VIDEORAM_DMA_ADR	(LCD_PALETTE_DMA_ADR + LCD_PALETTE_SIZE + (LCD_VIDEORAM_SIZE>>1))

#else
#	error "Cowardly refusing to break your LCD, please supply correct LCD parameters."
#endif

#endif

int lcd_power_up( void );
int lcd_power_down( void );

int lcd_gpio_setup( void );

int lcd_controller_setup( void );
int lcd_controller_enable( void );
int lcd_controller_disable( void );

int lcd_backlight_on( void );
int lcd_backlight_off( void );

int lcd_palette_setup( void );

int lcd_contrast( int );
int lcd_brightness( int );

#endif

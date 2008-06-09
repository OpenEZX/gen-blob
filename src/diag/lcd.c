/**********************************************************************
 * lcd.c
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

/**********************************************************************
 * includes
 */
#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/types.h>
#include <blob/errno.h>
#include <blob/util.h>
#include <blob/command.h>
#include <blob/serial.h>
#include <blob/sa1100.h>

#include <blob/lcd.h>

/**********************************************************************
 * defines
 */
#define WEAK_SYM __attribute__ (( weak ))

#define MEM(adr)	(*((u32 *)adr))
#define SET(reg,bit)	((reg) |= (1<<(bit)))
#define CLR(reg,bit)	((reg) &= ~(1<<(bit)))

#define LCD_DEBUG 1

#define OUTHEX( val ) SerialOutputString( #val"=0x" ); \
	SerialOutputHex( val ); \
	serial_write( '\n' );

/**********************************************************************
 * program globals
 */

/**********************************************************************
 * module globals
 */

u16 *lcd_palette = (u16*)(LCD_PALETTE_DMA_ADR);
u32 *lcd_vram = (u32*)(LCD_VIDEORAM_START);

/**********************************************************************
 * Prototypes
 */
void lcd_palette_set( int no, int val );

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

/**********************************************************************
 * Do whatever necessary to power up lcd display
 */
int WEAK_SYM lcd_power_up( void )
{
	return 0;
}

int WEAK_SYM lcd_power_down( void )
{
	return 0;
}

int WEAK_SYM lcd_gpio_setup( void )
{
	GPDR |= 0x3FC;
	GAFR |= 0x3FC;

	return 0;
}

int WEAK_SYM lcd_controller_setup( void )
{
	lcd_palette[0] &= 0xcfff;
	lcd_palette[0] |= 2<<16;	// 8 bpp

	/* Sequence from 11.7.10 */
	LCCR3 = LCD_LCCR3;
	LCCR2 = LCD_LCCR2;
	LCCR1 = LCD_LCCR1;
	LCCR0 = LCD_LCCR0 & ~LCCR0_LEN;
	DBAR1 = (u32)LCD_PALETTE_DMA_ADR;
	DBAR2 = (u32)LCD_VIDEORAM_DMA_ADR;
	return 0;
}

int WEAK_SYM lcd_controller_enable( void )
{
	LCCR0 |= LCCR0_LEN;
	return 0;
}

int WEAK_SYM lcd_controller_disable( void )
{
	LCCR0 &= ~LCCR0_LEN;
	return 0;
}

int WEAK_SYM lcd_backlight_on( void )
{
	return 0;
}

int WEAK_SYM lcd_backlight_off( void )
{
	return 0;
}

int WEAK_SYM lcd_palette_setup( void )
{
	int n;

	for ( n = 0 ; n < 128 ; n++ ) {
		lcd_palette_set(n, 0x1FFF );
	}
	for ( n = 128 ; n < 256 ; n++ ) {
		lcd_palette_set(n, 0x1000 );
	}

	return 0;
}

int WEAK_SYM lcd_contrast( int value )
{
	return 0;
}

int WEAK_SYM lcd_brightness( int value )
{
	return 0;
}

/**********************************************************************
 * Statische Funktionen
 */


static int lcd_test( int argc, char *argv[] )
{
	int ret = 0;
	int x,y;
	int c;

#if LCD_DEBUG
	OUTHEX( LCD_LCCR0 );
	OUTHEX( LCD_LCCR1 );
	OUTHEX( LCD_LCCR2 );
	OUTHEX( LCD_LCCR3 );
	OUTHEX( LCD_BPP );
	OUTHEX( LCD_COLS );
	OUTHEX( LCD_ROWS );
	OUTHEX( LCD_PALETTE_ENTRIES );
	OUTHEX( LCD_PALETTE_SIZE );
	OUTHEX( LCD_RAM_BASE );
	OUTHEX( LCD_VIDEORAM_SIZE );
	OUTHEX( LCD_VIDEORAM_START );
	OUTHEX( LCD_VIDEORAM_END );
	OUTHEX( LCD_VIDEORAM_DMA_ADR );
	OUTHEX( LCD_PALETTE_DMA_ADR );
#endif

	SerialOutputString( "LCD: power up ..." );
	ret = lcd_power_up();
	if ( ret != 0 )		return ret;
	SerialOutputString( "done\n" );

	SerialOutputString( "LCD: gpio setup ..." );
	ret = lcd_gpio_setup();
	if ( ret != 0 )		return ret;
	SerialOutputString( "done\n" );

	SerialOutputString( "LCD: controller setup ..." );
	ret = lcd_controller_setup();
	if ( ret != 0 )		return ret;
	SerialOutputString( "done\n" );

	SerialOutputString( "LCD: palette setup ..." );
	ret = lcd_palette_setup();
	if ( ret != 0 )		return ret;
	SerialOutputString( "done\n" );

	SerialOutputString( "LCD: controller enable ..." );
	ret = lcd_controller_enable();
	if ( ret != 0 )		return ret;
	SerialOutputString( "done\n" );

	SerialOutputString( "LCD: backlight on ..." );
	ret = lcd_backlight_on();
	if ( ret != 0 )		return ret;
	SerialOutputString( "done\n" );


	/* vertical lines */
	SerialOutputString( "LCD: vertical lines test pattern ..." );
	for ( y=0; y<LCD_ROWS; y++ ) {
		for ( x=0; x<(LCD_COLS>>2); x++ ) {
			if ( x%2 == 0 ) {
				lcd_vram[y*(LCD_COLS>>2) + x ] = 0xffffffff;
			} else {
				lcd_vram[y*(LCD_COLS>>2) + x ] = 0x00000000;
			}
		}
	}

	SerialOutputString( "done\n" );

	SerialOutputString( "LCD: press any key to proceed\n" );
	c = serial_read();

	SerialOutputString( "LCD: backlight off ..." );
	ret = lcd_backlight_off();
	if ( ret != 0 )		return ret;
	SerialOutputString( "done\n" );

	SerialOutputString( "LCD: controller disable ..." );
	ret = lcd_controller_disable();
	if ( ret != 0 )		return ret;
	SerialOutputString( "done\n" );

	SerialOutputString( "LCD: power down ..." );
	ret = lcd_power_down();
	if ( ret != 0 )		return ret;
	SerialOutputString( "done\n" );

	return 0;
}

static char testlcdhelp[] = "lcdtest\nTests lcd display\n";
__commandlist(lcd_test, "lcdtest", testlcdhelp);

void lcd_palette_set( int no, int val )
{
	if ( no < 0 || no > 255 )
		return;

	lcd_palette[ no ] = val;

	return;
}

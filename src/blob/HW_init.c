/*
 * src/blob/HW_init.c
 * 
 * special hardware init code for ezx platform
 * 
 * Copyright (C) 2005  Motorola
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
 * 2005-Jun-06 - (Motorola) init draft
 *
 */
#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/command.h>
#include <blob/errno.h>
#include <blob/error.h>
#include <blob/init.h>
#include <blob/main.h>
#include <blob/memory.h>
#include <blob/param_block.h>
#include <blob/serial.h>
#include <blob/time.h>
#include <blob/util.h>
#include <blob/time.h>
#include <blob/lcd_ezx.h>
#include <linux/string.h>
#include <blob/log.h>
#include "usb_ctl_bvd.h"
#include "pcap.h"
#include <blob/HW_init.h>
#include <blob/mmu.h>
#include <blob/handshake.h>

void K3flash_sync_mode(void);
void USB_gpio_init(void);
void st_uart_init(void);
#ifdef BARBADOS
u32 barbados_hw_init(u32 x)
{

	set_GPIO_mode(41 | GPIO_OUT);	/* tide add here for bp flash */
	GPCR(41) = GPIO_bit(41);	/* set no flash for BP GPIO 41 low , do not force BP enter flash mode */
	GPCR(99) = GPIO_bit(99);
	set_GPIO_mode(99 | GPIO_OUT);
	st_uart_init();
	logvarhex("\nbarbados:\n hw_init():\nOSCR = ", OSCR);
	/* call subsystems */
	init_subsystems();
	K3flash_sync_mode();	// set flash memory into sync mode
	get_memory_map();
#if 1
	/* config GPIO_BB_RESET as output high */
	set_GPIO_mode(GPIO_BB_RESET | GPIO_OUT);
	GPSR(GPIO_BB_RESET) = GPIO_bit(GPIO_BB_RESET);

	/* config MCU_INT_SW GPIO  as input */
	set_GPIO_mode(GPIO_MCU_INT_SW | GPIO_IN);
#endif
	return 0;

}

#endif

#ifdef MARTINIQUE
u32 martinique_hw_init(u32 x)
{

	set_GPIO_mode(41 | GPIO_OUT);	/* tide add here for bp flash */
	GPCR(41) = GPIO_bit(41);	/* set no flash for BP GPIO 41 low , do not force BP enter flash mode */
	GPCR(99) = GPIO_bit(99);
	set_GPIO_mode(99 | GPIO_OUT);
	st_uart_init();
	logvarhex("\nmartinique:\nhw_init():\nOSCR = ", OSCR);
	/* call subsystems */
	init_subsystems();
	K3flash_sync_mode();	// set flash memory into sync mode
	get_memory_map();
#if 1
	/* config GPIO_BB_RESET as output high */
	set_GPIO_mode(GPIO_BB_RESET | GPIO_OUT);
	GPSR(GPIO_BB_RESET) = GPIO_bit(GPIO_BB_RESET);

	/* config MCU_INT_SW GPIO  as input */
	set_GPIO_mode(GPIO_MCU_INT_SW | GPIO_IN);
#endif
	return 0;

}

#endif

#ifdef HAINAN

/*
 * In order to improve performance, MMU should be turned on.
 * Note: when flash code, the attribute of flash area should be uncachable, unbufferable
 */
struct mem_area io_map[] = {
	{0x00000000, 0x04000000, 0x0e},	// flash, 64M, B=1, C=1 for barbados
	//    {0x08000000,    0x08100000,     0x2},   // DoC access window 
	{0x40000000, 0x04000000, 0x2},	// io Memory Map 
	{0x44000000, 0x04000000, 0x2},	// LCD 
	{0x48000000, 0x04000000, 0x2},	// Memory Ctl
	{0x4C000000, 0x04000000, 0x2},	// USB host  
	{0xA0000000, 0x00F00000, 0x0a},	// SDRAM Bank 0, 15M, B=0, C=1
	{0xA0F00000, 0x00100000, 0x02},	// SDRAM Bank 0, 1M, B=0, C=0 (DMA buf)
	{0xA1000000, 0x01000000, 0x0a},	// SDRAM Bank 0, 16M, B=0, C=1
	{0xAC000000, 0x01000000, 0x0a},	// SDRAM Bank 3, 16M, B=0, C=0
	{0, 0, 0x0}		// end
};

u32 hainan_hw_init(u32 x)
{

	set_GPIO_mode(41 | GPIO_OUT);	/* tide add here for bp flash */
	GPCR(41) = GPIO_bit(41);	/* set no flash for BP GPIO 41 low , do not force BP enter flash mode */
	GPCR(99) = GPIO_bit(99);
	set_GPIO_mode(99 | GPIO_OUT);
	st_uart_init();
	logvarhex("\nhainan:\n hw_init():\nOSCR = ", OSCR);
	/* call subsystems */
	init_subsystems();
	K3flash_sync_mode();	// set flash memory into sync mode
	get_memory_map();
#if 1
	/* config GPIO_BB_RESET as output high */
	set_GPIO_mode(GPIO_BB_RESET | GPIO_OUT);
	GPSR(GPIO_BB_RESET) = GPIO_bit(GPIO_BB_RESET);

	/* config MCU_INT_SW GPIO  as input */
	set_GPIO_mode(GPIO_MCU_INT_SW | GPIO_IN);
#endif
	return 0;

}

#endif

#ifdef SUMATRA

/*
 * In order to improve performance, MMU should be turned on.
 * Note: when flash code, the attribute of flash area should be uncachable, unbufferable
 */
struct mem_area io_map[] = {
	{0x00000000, 0x04000000, 0x0e},	// flash, 64M, B=1, C=1 for barbados
	//    {0x08000000,    0x08100000,     0x2},   // DoC access window 
	{0x40000000, 0x04000000, 0x2},	// io Memory Map 
	{0x44000000, 0x04000000, 0x2},	// LCD 
	{0x48000000, 0x04000000, 0x2},	// Memory Ctl
	{0x4C000000, 0x04000000, 0x2},	// USB host  
	{0xA0000000, 0x00F00000, 0x0a},	// SDRAM Bank 0, 15M, B=0, C=1
	{0xA0F00000, 0x00100000, 0x02},	// SDRAM Bank 0, 1M, B=0, C=0 (DMA buf)
	{0xA1000000, 0x01000000, 0x0a},	// SDRAM Bank 0, 16M, B=0, C=1
	{0xAC000000, 0x01000000, 0x0a},	// SDRAM Bank 3, 16M, B=0, C=0
	{0, 0, 0x0}		// end
};

u32 sumatra_hw_init(u32 x)
{
	/* For Sumatra, force GPIO79 as output high before ISP1583 is configued.  */
	GPSR(79) = GPIO_bit(79);
	set_GPIO_mode(79 | GPIO_OUT);
	/* Switch USB 1.1 mode at boot time. */
	GPCR(94) = GPIO_bit(94);
	set_GPIO_mode(94 | GPIO_OUT);
	set_GPIO_mode(41 | GPIO_OUT);	/* tide add here for bp flash */
	GPCR(41) = GPIO_bit(41);	/* set no flash for BP GPIO 41 low , do not force BP enter flash mode */
	GPCR(99) = GPIO_bit(99);
	set_GPIO_mode(99 | GPIO_OUT);
	st_uart_init();
	logvarhex("\nsumatra:\n hw_init():\nOSCR = ", OSCR);
	/* call subsystems */
	init_subsystems();
	K3flash_sync_mode();	// set flash memory into sync mode
	get_memory_map();
#if 1
	/* config GPIO_BB_RESET as output high */
	set_GPIO_mode(GPIO_BB_RESET | GPIO_OUT);
	GPSR(GPIO_BB_RESET) = GPIO_bit(GPIO_BB_RESET);

	/* config MCU_INT_SW GPIO  as input */
	set_GPIO_mode(GPIO_MCU_INT_SW | GPIO_IN);
#endif
	return 0;

}

#endif

/* -------------------------tool functions --------------------------------------------*/

//USB_gpio_init(): set gpio for USB
void USB_gpio_init(void)
{
	set_GPIO_mode(GPIO34_TXENB_MD);
	set_GPIO_mode(GPIO35_XRXD_MD);
	set_GPIO_mode(GPIO36_VMOUT_MD);
	set_GPIO_mode(GPIO39_VPOUT_MD);
	set_GPIO_mode(GPIO40_VPIN_MD);
	set_GPIO_mode(GPIO53_VMIN_MD);

	//  UP2OCR =  UP2OCR_HXOE;
	UP2OCR = 0x02000000;	/* select single end port */
	//UP2OCR_HXS|UP2OCR_HXOE|UP2OCR_SEOS;
	__REG(0x40f00044) |= 0x00008000;
}

void K3flash_sync_mode(void)
{
	volatile u32 tmp;
	int i;

	*((u32 *) 0x48000008) = 0x7FF02AF8;	//Async data flash.  // sync L18 tyax -- 0xFFF812BB;
	tmp = *((u32 *) 0x48000008);

#if (0)				//Dataflash is used in Barbados
	*((u16 *) 0x00004b84) = 0x0060;	// chip I, RCR first 
	for (i = 0; i < 10; i++) ;
	*((u16 *) 0x00004b84) = 0x0003;	// chip I, RCR second
	for (i = 0; i < 10; i++) ;

	*((u16 *) 0x01004b84) = 0x0060;	// chip 2, RCR first 
	for (i = 0; i < 10; i++) ;
	*((u16 *) 0x01004b84) = 0x0003;	// chip 2, RCR second
	for (i = 0; i < 10; i++) ;
#endif

#if (0)				//Dataflash is Asyn, so remain 0x0 in SXCNFG
	*((u32 *) 0x4800001c) = 0x000060f1;	// SXCNFG
#endif

	for (i = 0; i < 10; i++) ;
	*((u32 *) 0x48000004) |= (1 << 14) | (1 << 13) | (1 << 12);	// MDREFR
	for (i = 0; i < 10; i++) ;
}

//#define OUTPUT_LOG_TO_STPORT
#ifdef OUTPUT_LOG_TO_STPORT
void st_uart_init(void)
{
	CKEN |= 0x00000020;

	set_GPIO_mode(GPIO46_STRXD_MD);
	set_GPIO_mode(GPIO47_STTXD_MD);

	while ((BTLSR & (0x1 << 5)) == 0) ;

	STLCR = 0x83;
	STDLL = 0x08;
	STDLH = 0;
	STLCR = 0x03;

	/* disenable FIFO */
	STFCR = 0x06;
	/* enable UART  */
	STIER = 0x40;
	while ((STLSR & 0x40) == 0) ;
}

/*
 * Output a single byte to the serial port.
 */
void STSerialOutputByte(const char c)
{
	/* wait for room in the tx FIFO */
	while ((STLSR & 0x40) == 0) ;

	STTHR = c;

	/* If \n, also do \r */
	if (c == '\n') {
		while ((STLSR & 0x40) == 0) ;
		STTHR = '\r';
	}
}

/*
 * Write a null terminated string to the serial port.
 */
void STSerialOutputString(const char *s)
{

	while (*s != 0)
		STSerialOutputByte(*s++);

}				/* SerialOutputString */

/*
 * Write the argument of the function in hexadecimal to the serial
 * port. If you want "0x" in front of it, you'll have to add it
 * yourself.
 */
void STSerialOutputHex(const u32 h)
{
	char c;
	int i;

	for (i = NIBBLES_PER_WORD - 1; i >= 0; i--) {
		c = (char)((h >> (i * 4)) & 0x0f);

		if (c > 9)
			c += ('A' - 10);
		else
			c += '0';

		STSerialOutputByte(c);
	}
}

/*
 * Write the argument of the function in decimal to the serial port.
 * We just assume that each argument is positive (i.e. unsigned).
 */
void STSerialOutputDec(const u32 d)
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
			STSerialOutputByte((char)(result) + '0');
	}
}

#else
void st_uart_init(void)
{
}
void STSerialOutputByte(const char c)
{
}

void STSerialOutputString(const char *s)
{
}

void STSerialOutputHex(const u32 h)
{
}

void STSerialOutputDec(const u32 d)
{
}

#endif

void FFUART_gpio_init()
{
	set_GPIO_mode(GPIO53_FFRXD_MD);
	set_GPIO_mode(GPIO39_FFTXD_MD);
}

#ifdef SERIAL_OUTPUT
__initlist(FFUART_gpio_init, INIT_LEVEL_INITIAL_HARDWARE);
#else
#ifndef MBM
__initlist(USB_gpio_init, INIT_LEVEL_INITIAL_HARDWARE);
#endif
#endif

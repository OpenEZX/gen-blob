/*
 * regs-sa11x0.c: command to nicely debug register contents
 *
 * Copyright (C) 2001 Stefan Eletzhofer <stefan.eletzhofer@www.eletztrick.net>
 *
 * $Id: regs-sa11x0.c,v 1.3 2002/01/07 17:35:06 erikm Exp $
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

#ident "$Id: regs-sa11x0.c,v 1.3 2002/01/07 17:35:06 erikm Exp $"

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
#include <blob/init.h>
#include <blob/serial.h>
#include <blob/time.h>
#include <blob/sa1100.h>

/**********************************************************************
 * defines
 */
#define MEM(adr)	(*((u32*)adr))
#define SET(reg,bit)	((reg) |= (1<<(bit)))
#define CLR(reg,bit)	((reg) &= ~(1<<(bit)))

/**********************************************************************
 * types
 */

/* a type to hold register infos */
typedef struct _reg {
	char *name;	/* reg name */
	u32 adr;	/* reg address */
	char *desc;	/* description */
} register_t;

/* a set of registers */
typedef struct _reg_set {
	char *name;			/* name of register set */
	register_t *set;	/* register set */
} reg_set_t;

/**********************************************************************
 * module globals
 */

/* NOTE: the following register definitions are directly taken from
 *    linux/include/asm-arm/arch-sa1100/SA-1100.h
 * and should therefore be correct.*/

/* Serial IF */
static register_t	regs_uart[] = {
	{ "Ser0UDCCR", 	(0x80000000),   " Ser. port 0 UDC Control Reg. " },
	{ "Ser0UDCAR", 	(0x80000004),   " Ser. port 0 UDC Address Reg. " },
	{ "Ser0UDCOMP", 	(0x80000008),   " Ser. port 0 UDC Output Maximum Packet size reg. " },
	{ "Ser0UDCIMP", 	(0x8000000C),   " Ser. port 0 UDC Input Maximum Packet size reg. " },
	{ "Ser0UDCCS0", 	(0x80000010),   " Ser. port 0 UDC Control/Status reg. end-point 0 " },
	{ "Ser0UDCCS1", 	(0x80000014),   " Ser. port 0 UDC Control/Status reg. end-point 1 (output) " },
	{ "Ser0UDCCS2", 	(0x80000018),   " Ser. port 0 UDC Control/Status reg. end-point 2 (input) " },
	{ "Ser0UDCD0", 	(0x8000001C),   " Ser. port 0 UDC Data reg. end-point 0 " },
	{ "Ser0UDCWC", 	(0x80000020),   " Ser. port 0 UDC Write Count reg. end-point 0 " },
	{ "Ser0UDCDR", 	(0x80000028),   " Ser. port 0 UDC Data Reg. " },
	{ "Ser0UDCSR", 	(0x80000030),   " Ser. port 0 UDC Status Reg. " },
	{ "_UTCR01", 	(0x80010000 + ((1) - 1)*0x00020000),  " UART Control Reg. 0 [1..3] " },
	{ "_UTCR02", 	(0x80010000 + ((2) - 1)*0x00020000),  " UART Control Reg. 0 [1..3] " },
	{ "_UTCR03", 	(0x80010000 + ((3) - 1)*0x00020000),  " UART Control Reg. 0 [1..3] " },
	{ "_UTCR11", 	(0x80010004 + ((1) - 1)*0x00020000),  " UART Control Reg. 1 [1..3] " },
	{ "_UTCR22", 	(0x80010008 + ((2) - 1)*0x00020000),  " UART Control Reg. 2 [1..3] " },
	{ "_UTCR33", 	(0x8001000C + ((3) - 1)*0x00020000),  " UART Control Reg. 3 [1..3] " },
	{ "_UTCR42", 	(0x80010010 + ((2) - 1)*0x00020000),  " UART Control Reg. 4 [2] " },
	{ "_UTDR1", 	(0x80010014 + ((1) - 1)*0x00020000),  " UART Data Reg. [1..3] " },
	{ "_UTDR2", 	(0x80010014 + ((2) - 1)*0x00020000),  " UART Data Reg. [1..3] " },
	{ "_UTDR3", 	(0x80010014 + ((3) - 1)*0x00020000),  " UART Data Reg. [1..3] " },
	{ "_UTSR01", 	(0x8001001C + ((1) - 1)*0x00020000),  " UART Status Reg. 0 [1..3] " },
	{ "_UTSR02", 	(0x8001001C + ((2) - 1)*0x00020000),  " UART Status Reg. 0 [1..3] " },
	{ "_UTSR03", 	(0x8001001C + ((3) - 1)*0x00020000),  " UART Status Reg. 0 [1..3] " },
	{ "_UTSR11", 	(0x80010020 + ((1) - 1)*0x00020000),  " UART Status Reg. 1 [1..3] " },
	{ "_UTSR12", 	(0x80010020 + ((2) - 1)*0x00020000),  " UART Status Reg. 1 [1..3] " },
	{ "_UTSR13", 	(0x80010020 + ((3) - 1)*0x00020000),  " UART Status Reg. 1 [1..3] " },
	{ "Ser1SDCR0", 	(0x80020060),   " Ser. port 1 SDLC Control Reg. 0 " },
	{ "Ser1SDCR1", 	(0x80020064),   " Ser. port 1 SDLC Control Reg. 1 " },
	{ "Ser1SDCR2", 	(0x80020068),   " Ser. port 1 SDLC Control Reg. 2 " },
	{ "Ser1SDCR3", 	(0x8002006C),   " Ser. port 1 SDLC Control Reg. 3 " },
	{ "Ser1SDCR4", 	(0x80020070),   " Ser. port 1 SDLC Control Reg. 4 " },
	{ "Ser1SDDR", 	(0x80020078),   " Ser. port 1 SDLC Data Reg.      " },
	{ "Ser1SDSR0", 	(0x80020080),   " Ser. port 1 SDLC Status Reg. 0  " },
	{ "Ser1SDSR1", 	(0x80020084),   " Ser. port 1 SDLC Status Reg. 1  " },
	{ "Ser2HSCR0", 	(0x80040060),   " Ser. port 2 HSSP Control Reg. 0 " },
	{ "Ser2HSCR1", 	(0x80040064),   " Ser. port 2 HSSP Control Reg. 1 " },
	{ "Ser2HSDR", 	(0x8004006C),   " Ser. port 2 HSSP Data Reg.      " },
	{ "Ser2HSSR0", 	(0x80040074),   " Ser. port 2 HSSP Status Reg. 0  " },
	{ "Ser2HSSR1", 	(0x80040078),   " Ser. port 2 HSSP Status Reg. 1  " },
	{ "Ser2HSCR2", 	(0x90060028),   " Ser. port 2 HSSP Control Reg. 2 " },
	{ "Ser4MCCR0", 	(0x80060000),   " Ser. port 4 MCP Control Reg. 0 " },
	{ "Ser4MCDR0", 	(0x80060008),   " Ser. port 4 MCP Data Reg. 0 (audio) " },
	{ "Ser4MCDR1", 	(0x8006000C),   " Ser. port 4 MCP Data Reg. 1 (telecom) " },
	{ "Ser4MCDR2", 	(0x80060010),   " Ser. port 4 MCP Data Reg. 2 (CODEC reg.) " },
	{ "Ser4MCSR", 	(0x80060018),   " Ser. port 4 MCP Status Reg. " },
	{ "Ser4MCCR1", 	(0x90060030),   " Ser. port 4 MCP Control Reg. 1 " },
	{ "Ser4SSCR0", 	(0x80070060),   " Ser. port 4 SSP Control Reg. 0 " },
	{ "Ser4SSCR1", 	(0x80070064),   " Ser. port 4 SSP Control Reg. 1 " },
	{ "Ser4SSDR", 	(0x8007006C),   " Ser. port 4 SSP Data Reg. " },
	{ "Ser4SSSR", 	(0x80070074),   " Ser. port 4 SSP Status Reg. " },
	{ NULL,	0 }
};
/* Timer */
static register_t	regs_timer[] = {
	{ "OSMR0",   		(0x90000000),   " OS timer Match Reg. 0 " },
	{ "OSMR1",   		(0x90000004),   " OS timer Match Reg. 1 " },
	{ "OSMR2",   		(0x90000008),   " OS timer Match Reg. 2 " },
	{ "OSMR3",   		(0x9000000c),   " OS timer Match Reg. 3 " },
	{ "OSCR",    	(0x90000010),   " OS timer Counter Reg. " },
	{ "OSSR",    	(0x90000014),  " OS timer Status Reg. " },
	{ "OWER",    	(0x90000018),  " OS timer Watch-dog Enable Reg. " },
	{ "OIER",    	(0x9000001C),  " OS timer Interrupt Enable Reg. " },
	{ NULL,	0 }
};
/* RTC and Power Mgmt */
static register_t	regs_rtc[] = {
	{ "RTAR", 		(0x90010000),   " RTC Alarm Reg. " },
	{ "RCNR", 		(0x90010004),   " RTC CouNt Reg. " },
	{ "RTTR", 		(0x90010008),   " RTC Trim Reg. " },
	{ "RTSR", 		(0x90010010),   " RTC Status Reg. " },
	{ "PMCR", 		(0x90020000),   " PM Control Reg. " },
	{ "PSSR", 		(0x90020004),   " PM Sleep Status Reg. " },
	{ "PSPR", 		(0x90020008),   " PM Scratch-Pad Reg. " },
	{ "PWER", 		(0x9002000C),   " PM Wake-up Enable Reg. " },
	{ "PCFR", 		(0x90020010),   " PM general ConFiguration Reg. " },
	{ "PPCR", 		(0x90020014),   " PM PLL Configuration Reg. " },
	{ "PGSR", 		(0x90020018),   " PM GPIO Sleep state Reg. " },
	{ "POSR", 		(0x9002001C),   " PM Oscillator Status Reg. " },
	{ "RSRR", 		(0x90030000),   " RC Software Reset Reg. " },
	{ "RCSR", 		(0x90030004),   " RC Status Reg. " },
	{ "TUCR", 		(0x90030008),   " Test Unit Control Reg. " },
	{ NULL,	0 }
};
/* GPIO / IRQ */
static register_t	regs_gpio[] = {
	{ "GPLR", 		(0x90040000),   " GPIO Pin Level Reg.             " },
	{ "GPDR", 		(0x90040004),   " GPIO Pin Direction Reg.         " },
	{ "GPSR", 		(0x90040008),   " GPIO Pin output Set Reg.        " },
	{ "GPCR", 		(0x9004000C),   " GPIO Pin output Clear Reg.      " },
	{ "GRER", 		(0x90040010),   " GPIO Rising-Edge detect Reg.    " },
	{ "GFER", 		(0x90040014),   " GPIO Falling-Edge detect Reg.   " },
	{ "GEDR", 		(0x90040018),   " GPIO Edge Detect status Reg.    " },
	{ "GAFR", 		(0x9004001C),   " GPIO Alternate Function Reg.    " },
	{ "ICIP", 		(0x90050000),   " IC IRQ Pending reg.             " },
	{ "ICMR", 		(0x90050004),   " IC Mask Reg.                    " },
	{ "ICLR", 		(0x90050008),   " IC Level Reg.                   " },
	{ "ICCR", 		(0x9005000C),   " IC Control Reg.                 " },
	{ "ICFP", 		(0x90050010),   " IC FIQ Pending reg.             " },
	{ "ICPR", 		(0x90050020),   " IC Pending Reg.                 " },
	{ "PPDR", 		(0x90060000),   " PPC Pin Direction Reg.          " },
	{ "PPSR", 		(0x90060004),   " PPC Pin State Reg.              " },
	{ "PPAR", 		(0x90060008),   " PPC Pin Assignment Reg.         " },
	{ "PSDR", 		(0x9006000C),   " PPC Sleep-mode pin Direction Reg. " },
	{ "PPFR", 		(0x90060010),   " PPC Pin Flag Reg.               " },
	{ NULL,	0 }
};
/* MEMORY */
static register_t	regs_memory[] = {
	{ "MDCNFG", 		(0xA0000000),   "  DRAM CoNFiGuration reg. " },
	{ "MDCAS0", 		(0xA0000004),   " DRAM CAS shift reg. 0 " },
	{ "MDCAS1", 		(0xA0000008),   " DRAM CAS shift reg. 1 " },
	{ "MDCAS2", 		(0xA000000c),   " DRAM CAS shift reg. 2 " },
	{ "MSC0", 		(0xa0000010),   " Static memory Control reg. 0 " },
	{ "MSC1", 		(0xa0000014),   " Static memory Control reg. 1 " },
	{ "MSC2", 		(0xa000002c),   " Static memory Control reg. 2, not contiguous   " },
	{ "MECR", 		(0xA0000018),   "  Expansion memory bus (PCMCIA) Configuration Reg.             " },
	{ "MDREFR", 		(0xA000001C), " (SA1110 only) DRAM Refresh Control Register" },
	{ NULL,	0 }
};
/* LCD */
static register_t	regs_lcd[] = {
	{ "LCCR0", 		(0xB0100000),   " LCD Control Reg. 0 " },
	{ "LCSR", 		(0xB0100004),   " LCD Status Reg. " },
	{ "DBAR1", 		(0xB0100010),   " LCD DMA Base Address Reg. channel 1 " },
	{ "DCAR1", 		(0xB0100014),   " LCD DMA Current Address Reg. channel 1 " },
	{ "DBAR2", 		(0xB0100018),   " LCD DMA Base Address Reg.  channel 2 " },
	{ "DCAR2", 		(0xB010001C),   " LCD DMA Current Address Reg. channel 2 " },
	{ "LCCR1", 		(0xB0100020),   " LCD Control Reg. 1 " },
	{ "LCCR2", 		(0xB0100024),   " LCD Control Reg. 2 " },
	{ "LCCR3", 		(0xB0100028),   " LCD Control Reg. 3 " },
	{ NULL,	0 }
};

/* TODO: add SA1111 companion chip registers */

/* finally the available register sets */
static reg_set_t reg_sets[] = {
	{ "uart", regs_uart },
	{ "timer", regs_timer },
	{ "rtc", regs_rtc },
	{ "gpio", regs_gpio },
	{ "memory", regs_memory },
	{ "lcd", regs_lcd },
	{ NULL, NULL }
};

/**********************************************************************/
/**********************************************************************/
/**********************************************************************/

static int regs_show( int argc, char *argv[] )
{
	int i = 0;
	int set = 0;
	register_t *registers = NULL;

	/* TODO: allow user to select a specific register set to
	 * print instead of simply print all sets. */

	set = 0;
	while ( reg_sets[set].name ) {
		i = 0;
		registers = reg_sets[set].set;
		SerialOutputString( reg_sets[set].name );
		serial_write( '\n' );
		SerialOutputString( "--------------------------------\n" );
		while ( registers && registers[i].name ) {
			SerialOutputString( registers[i].name );
			SerialOutputString( "= 0x" );
			SerialOutputHex( MEM( registers[i].adr ) );
			if ( registers[i].desc ) {
				SerialOutputString( registers[i].desc );
			}
			serial_write( '\n' );
			i++;
		}
		set++;
		serial_write( '\n' );
	}

	return 0;
}
static char regshelp[] = "print register info\n";
__commandlist(regs_show, "regs", regshelp);

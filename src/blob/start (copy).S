/*
 * start.S: blob start code
 *
 * Copyright (C) 2002, 2004-2006 Motorola Inc.	
 * Copyright (C) 1999 2000 2001 Erik Mouw (J.A.K.Mouw@its.tudelft.nl) and
 *                     Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl)
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
/*
 * This is the blob start code. The SA-1100 jumps to address 0x00000000
 * after a reset. We put a single branch code at this position which jumps
 * to a safe region to do the actual setup. All other vectors just point
 * to an endless loop for the moment.
 *
 * Documentation:
 * [1] Intel Corporation, "Intel StrongARM SA-1100 Microprocessor
 *     Developer's Manual", April 1999
 * [2] S. Furber, "ARM System Architecture", Addison Wesley Longman
 *     Ltd., Essex, England, 1996.
 * [3] Intel Corporation, "Intel StrongARM SA-1110 Microprocessor
 *     Advanced Developer's manual, December 1999
 *
 * 2002-Oct-30 - (Motorola) Modifications to support USB restore
 * 2002-Dec-09 - (Motorola) Changed for exit sleep/user off 
 * 2004-Aug-30 - (Motorola) Intel pm workaround2
 * 2005-Dec-14 - (Motorola) modified for EZX platform
 */

.ident "$Id: start.S,v 1.1.1.1 2002/07/17 00:32:33 cvs Exp $"

#ifdef HAVE_CONFIG_H
# include <config.h>
#endif

#include <blob/arch/lubbock.h>
//#define E680_P3_AND_EARLY
#define  INTEL_PM_91M_WORKAROUND
.text
//???fix me: This address should be same as in linux.c, potential 
//confilicts may be happened if someone use this address in other places
//it assert a0000000+12 now, see lubbock.h for other flags in this area
#ifdef MBM //MBM module
#define POWERUP_REASON_ADDR 0xa000000c
#endif  //MBM module
/* Jump vector table as in table 3.1 in [1] */
.globl _start
_start:	b	reset
	b	undefined_instruction
	b	software_interrupt
	b	prefetch_abort
	b	data_abort
	b	not_used
	b	irq
	b	fiq

/* the actual reset code */
.align	4
reset:
//we need save r0's content from MBM for powerup reason
#if 0	//just give r0 a test value if no MBM
	ldr	r0,=0x4
#endif
#ifdef MBM	//MBM module
	ldr	r1, =POWERUP_REASON_ADDR	//set address of u32 in r1
	str	r0, [r1]
#if defined(HAINAN) || defined(MARTINIQUE)
	ldr	r1, =0x48000004
	ldr 	r0, [r1]
	and	r0,r0,#0xffefffff
	str	r0, [r1]
#endif
	
#endif	//MBM module
#ifndef MBM // remove clock setting, US MBM take it,just for products with MBM
        ldr     r1, =0x41300004
        ldr     r2, =0x00510aE0
        str     r2, [r1]          
	ldr r1, =0x40a00010             	// reset the OS Timer Count to zero
	mov r2, #0 
	str r2, [r1] 
#endif // remove clock setting, US MBM take it just for products with MBM

#if 0
// must set the GPIOs up before any chip selects will work

  /* normal 780 and 680 case */
// First, Set GPSRx,y,z HIGH for active low outputs...

#if 0        
        ldr     r1, =0x40e00018
	ldr     r2, =0x10080010     //set GPIO4(AP_WDI)as high 
        str     r2, [r1]

        ldr     r1, =0x40e0001c
	ldr     r2, =0xfe850000     //set GPIO55(sys_restart),GPIO57(MCU_INT_SW) as high 
        str     r2, [r1]
            
        ldr     r1, =0x40e00020
	ldr     r2, =0x0004ffff     //set GPIO82(BB_RESET) as high 
        str     r2, [r1]

#endif
        ldr     r1, =0x40e00118
	ldr     r2, =0x00000009     //set GPIO116(LCD_OFF) as high  to close LCD 
        str     r2, [r1]         

// ...then Set GPCR LOW for active high outputs

#if 0        
        ldr     r1, =0x40e00024
	ldr     r2, =0x00000000
        str     r2, [r1]

        ldr     r1, =0x40e00028
	ldr     r2, =0x0000c000
        str     r2, [r1]

        ldr     r1, =0x40e0002c
        ldr     r2, =0x00000000   
        str     r2, [r1]
#endif

// Next, Set GPDR accordingly for all pins

#if 0        
        ldr     r1,  =0x40e0000c
	ldr     r2,  =0x33a90610  	//GPIO21 conifged as an output(0x00200000)  
        str     r2,  [r1]
        
        ldr     r1,  =0x40e00010
	ldr     r2,  =0xff87ead7  
        str     r2,  [r1]
        
        ldr     r1,  =0x40e00014   
	ldr     r2,  =0x00c67fff 	//GPIO78 conifged as an putput(0x00200000)
        str     r2,  [r1]
#endif

        ldr     r1,  =0x40e0010c   
	ldr     r2,  =0x00001f89  
        str     r2,  [r1]

// Finally, set GAFR0,1 for alternate functions

#if 0        
        ldr     r1, =0x40e00054 
	ldr     r2, =0x003c0000
        str     r2, [r1]

        ldr     r1, =0x40e00058 
        ldr     r2, =0x00c0700a
        bic     r2, r2, #0x00000c00             // clear GPIO21 alternate function bits
        orr     r2, r2, #0x00000400             // set GPIO21 as nSDCS3
        str     r2, [r1]

        ldr     r1, =0x40e0005c
	ldr     r2, =0x0993699a	  
        str     r2, [r1]
        
        ldr     r1, =0x40e00060
	ldr     r2, =0xaaa03a48	
        str     r2, [r1]

        ldr     r1, =0x40e00064
        ldr     r2, =0x0aaaaaaa
        bic     r2, r2, #0x30000000             // clear GPIO78 alternate function bits
        orr     r2, r2, #0x20000000             // set GPIO78 as nCS2
        str     r2, [r1]

        ldr     r1, =0x40e00068
	ldr     r2, =0xa506af44  
        str     r2, [r1]
#endif

        ldr     r1, =0x40e0006c
        ldr     r2, =0x56aa953c
        str     r2, [r1]

        ldr     r1, =0x40e00070
	ldr     r2, =0x0000149d  
        str     r2, [r1]

#endif        
.set MOTOROLA_USB_RESTORE, TRUE
.set TRANSCEIVER_MODE, FALSE
#if 0
#if (MOTOROLA_USB_RESTORE)

	@ --------------------------------------------------------------------
	@ This section of code is needed by motorola when comming out of 
	@ sleep reset to restore the GPIO pins used by the transceiverless
	@ Port 3 port.  The pins which must be driven high here are the 
	@ port 3 tranceiverless output pins that are driven high when no
	@ USB traffic is going out the port.  These are pins are the following
	@ --------------------------------------------------------------------
	@            		MOTOROLA P3_Config0  	Level    P3_Config2		Level
	@ USB_P3_1	 RCV	GPIO#91		IN 		OUT			  1
	@ USB_P3_2	 OE#	GPIO#30		OUT			1	 		IN
	@ USB_P3_3	 RX D- 	GPIO#113	IN				 		OUT			  0
	@ USB_P3_4	 TX D- 	GPIO#56		OUT			0	 		IN
	@ USB_P3_5	 RX D+ 	GPIO#90		IN				 		OUT			  1
	@ USB_P3_6	 TX D+	GPIO#31		OUT			1	 		IN
	@ --------------------------------------------------------------------

#if (TRANSCEIVER_MODE)
	@ Set Driven Level using the GPDR registers.
	@ Set Level on GPIO 30/ OE# 	--> 1
	@ GPSR0 Bit 30
	@ Set Level on GPIO 31/ TX D+ 	--> 1
	@ GPSR0 Bit 31

	ldr 	R0,	=0x40E00018				@ Get the GPSR0 Register
	ldr		R1, [R0]
	orr		R1, R1, #0xC0000000
	str		R1, [r0]					@Write the register back out.

		
	@ Set Level on GPIO 56/ TX D- 	--> 0
	@ GPCR1, Bit 24

	ldr 	R0,	=0x40E00028				@ Get the GPCR1 Register
	ldr		R1, [R0]
	orr		R1, R1, #0x01000000			
	str		R1, [r0]					@Write the register back out.

	@ Set DIrectioni using the GPDR registers.
	@ Set Direction on GPIO 30/ OE# 	--> OUT
	@ GPDR0, Bit 30
	@ Set Direction on GPIO 31/ TX D+ 	--> OUT
	@ GPDR0, Bit 31

	ldr 	R0,	=0x40E0000C				@ Get the GPDR0 Register
	ldr		R1, [R0]
	orr		R1, R1, #0xC0000000
	str		R1, [r0]					@Write the register back out.

	@ Set Direction on GPIO 56/ TX D- 	--> OUT
	@ GPDR1, Bit 24

	ldr 	R0,	=0x40E00010				@ Get the GPDR1 Register
	ldr		R1, [R0]
	orr		R1, R1, #0x01000000			
	str		R1, [r0]					@Write the register back out.

#else

	@----------------------------------------------
	@ TRANCEIVER-LESS MODE FOR MOTOROLA
	@----------------------------------------------
	@ Set Driven Level using the GPDR registers.
	@ Set Level on GPIO 91/ RCV 	--> 1
	@ GPSR2 Bit 27
	@ Set Level on GPIO 90/ RX D+ 	--> 1
	@ GPSR2 Bit 26

	ldr 	R0,	=0x40E00020				@ Get the GPSR2 Register
	//ldr		R1, [R0]
	//orr		R1, R1, #0x0C000000
	ldr	r1,	=0x0c000000
	str		R1, [r0]					@ Write the register back out.

		
	@ Set Level on GPIO 113/ RX D- 	--> 0
	@ GPCR3, Bit 17

	ldr 	R0,	=0x40E00124				@ Get the GPCR1 Register
	//ldr		R1, [R0]
	//orr		R1, R1, #0x00020000
	ldr	r1,	=0x00020000			
	str		R1, [r0]					@ Write the register back out.

	@ Set DIrectioni using the GPDR registers.
	@ Set Direction on GPIO 90/ RCV 	--> OUT
	@ GPDR2, Bit 27
	@ Set Direction on GPIO 91/ TX D+ 	--> OUT
	@ GPDR2, Bit 26

	ldr 	R0,	=0x40E00014				@ Get the GPDR2 Register
	ldr		R1, [R0]
	orr		R1, R1, #0x0C000000
	str		R1, [r0]					@Write the register back out.

	@ Set Direction on GPIO 113/ RX D- 	--> OUT
	@ GPDR3, Bit 17

	ldr 	R0,	=0x40E0010C				@ Get the GPDR1 Register
	ldr		R1, [R0]
	orr		R1, R1, #0x00020000			
	str		R1, [r0]					@Write the register back out.

#endif
#endif         
#endif

#ifndef MBM // remove them, US MBM take it,just for products with MBM
//Disable the MMU and gang regardless of why we are here.
	ldr r0, =0x2001	 				/*enable access to all coprocessors*/
    	mcr p15, 0, r0, c15, c1, 0
             
/* CPWAIT */
       	mrc     p15, 0, r0, c2, c0, 0       	// arbitrary read of CP15
    	mov     r0, r0                      	// wait for it (foward dependency)
    	sub     pc, pc, #4                  	// branch to next instruction
/* CPWAIT ....*/   
	 
	mov    r0, #0x78			// turn everything off 
	mcr    p15, 0, r0, c1, c0, 0   // caches off -- MMU off or ID map


	mov		r0, #0x00000000
	mcr     p15, 0, r0, c8, c7, 0   	// flush (invalidate) I/D tlb's
	mcr		p15, 0, r0, c7, c7, 0		// flush (invalidate) I/D caches
	mcr		p15, 0, r0, c7, c10, 4	// Drain write buffer -- r0 ignored

/* CPWAIT */
       	mrc     p15, 0, r0, c2, c0, 0       	// arbitrary read of CP15
    	mov     r0, r0                      	// wait for it (foward dependency)
    	sub     pc, pc, #4                  	// branch to next instruction
/* CPWAIT ....*/    

		nop
		nop
		nop

//
// setup manager access, interrupts, etc.
//
		mvn    r0, #0                      	// grant manager access to all domains
		mcr    p15, 0, r0, c3, c0, 0  


//	 Disable (mask) all interrupts at the interrupt controller
        
        // clear the interrupt level register (use IRQ, not FIQ)
        
        mov     r1, #0
        ldr     r2,  =0x40d00008
        str     r1,  [r2]
        
        // mask all interrupts at the controller
        ldr     r2,  =0x40d00004
        str     r1,  [r2]


// enable the 32Khz oscillator for RTC and PowerManager
        ldr     r1,  =0x41300008     
	ldr	r2,  [r1]
	orr	r2,  r2,   #0x00000002
        str     r2,  [r1]

/***********************************************/

#if defined(SUMATRA)
        ldr     r0,=0x40E0000C          //GPDR0
        ldr     r1,=0x00200000
        str     r1,[r0]                 //GPIO21 AS OUTPUT
        ldr     r0,=0x40E00058          //GAFR0_U
        ldr     r1,=0x00000400
        str     r1,[r0]                 //GPIO21 AS nSDCS3

#endif				

// 	setup memory controller registers

// 	Dev Manual step 1
//	step 1 first bullet
//	Wait 200 usec for the power-on wait period. (allows internal clocks to stabilize)
	// ---- Wait 200 usec
	ldr r3, =0x40a00010             	
	ldr r2, [r3] 
        add r4, r2,#0x300
20:
	ldr r2, [r3] 
	cmp r4, r2
	bgt 20b


//	step 1 forth bullet

	ldr r3, =0x48000000		// Reset appropriately,configure, but do not enable, each SDRAM partition pair.
//	ldr r2, =0x0aac2acc
#if defined(SUMATRA)
	ldr r2, =0x0acc2aac	
#else
	ldr r2, =0x08002bcc	//only SDRAM partition0 (0x0aac2bcc change DTC value from 0b10 to 0b11)
#endif
//	ldr r2, =0x0aac2bcc
//	ldr r2, =0x29ac29cc
	str r2, [r3, #0x00]
	ldr r2, [r3, #0x00]  						;

// 	step 1 five bullet

// 	Write (Assert) MDREFR:K0RUN. At this time, MDREFR:K0DB2,K0DB4 can
//	be properly configured. Retain the current values of MDREFR:APD (deasserted)
//	and (not all 0s). MDREFR:KxFREE can be deasserted, if required.
//	MDREFR:SLFRSH (asserted). MDREFR:DRI must contain a valid value
	 
//	ldr r2, =0x204a6015	
//	ldr r2, =0x205a6015		// enable APD
#if defined(SUMATRA) 
	ldr r2, =0x000f0017	
#else
	ldr r2, =0x205a602e	//  enable APD
#endif
	str r2, [r3, #0x04]

//	Dev Manual step 2
//	No SROM

//	Dev Manual step 3 -- change SDRAM states 
//   	step 3 first bullet
//	read MDREFR value
	ldr r2, [r3, #0x04]
	orr	r2, r2, #0x00010000  //assert K1RUN
	str	r2, [r3, #0x04]			// Self-Refresh and clock-stop through Self-Refresh
	ldr	r2, [r3, #0x04] 

//    	step 3 second bullet
	bic	r2, r2, #0x00400000		// deassert self-refresh
		nop
		nop
		nop
	str	r2, [r3, #0x04]  		// Self-Refresh through power-down 
	ldr	r2, [r3, #0x04]
 
//      step 3 third bullet
	orr	r2, r2, #0x00008000		// assert E1PIN 
	str	r2, [r3, #0x04]  		// power-down through PWRDNX
	ldr	r2, [r3, #0x04] 

//	Step 3 forth bullet 		 no write required 
// 	for this state transition PWRDNX through NOP
	nop 
	nop
 
//	Dev Manual step 5 -- --- Wait 200 usec
 
	// ---- Wait 200 usec
	ldr r3, =0x40a00010             	
	ldr r2, [r3] 
        add r4, r2,#0x300
10:
	ldr r2, [r3] 
	cmp r4, r2
	bgt 10b

 
// 	Dev Manual  step 6
// 	make sure the DCACHE is off before step 7 
	mov	r0, #0x78			// turn everything off
	mcr	p15, 0, r0, c1, c0, 0		// caches off, MMU off, etc. 
	MOV 	R0, R0 				// wait for it
	SUB	PC, PC, #4 			// branch to next instruction 
	nop
	nop 
	nop
	nop 
	nop
 

//	Dev Manual step 7 
	ldr	r3, =0xa0000000
	mov	r2, #8     			// now must do 8 refresh or CBR commands before the first access  
30:
	str	r3, [r3]  
	subs	r2, r2, #1
	bne	30b 

//	Dev Manual step 8 
//	can renable DCACHE if it was turned off in step 6
 

//	Dev Manual step 9  
	ldr	r3, =0x48000000 		// sdram config -- sdram enable
	ldr	r2, [r3, #0x00]  
#if defined(SUMATRA) 
	ldr	r1, =0x00020001			// for partition 0 and 3
#else
	ldr	r1, =0x00000001			// for partition 0
#endif
	orr	r2, r2, r1        		// enable appropriate banks  
	str	r2, [r3, #0x00]
	ldr	r2, [r3, #0x00] 

 
//	Dev Manual step 10
//	write the MDMRS 
	ldr 	r2, =0x00220032
	str 	r2, [r3, #0x40]  

//      Brakes SDRAM - LV_MDMRS must not occur here! 

//	Dev Manual  step 11 (optional) 
//	enable auto-power-down

#ifdef INTEL_PM_91M_WORKAROUND  // delay about 14 msec
	ldr r3, =FLAG_ADDR
	ldr r2, [r3]
	ldr r3, =0x6b6b6b6b
      	cmp r2, r3
        bne  change_frequency
	// ---- Wait about 14 msec
	ldr r3, =0x40a00010
	ldr r2, [r3]
        add r4, r2,#0xc000
40:
	ldr r2, [r3] 
	cmp r4, r2
	bgt 40b
#endif
#if 0  // just for debug
	// clear the RDH and PH bits in the PSSR
	ldr	r0, =0x40f00004
        ldr     r1, =0x30              
    	str	r1, [r0]

	ldr r1, =0x40e0010c    //set GPIO109 output
        ldr r2, =0xFE002000     
        str r2, [r1] 

        ldr r1, =0x40e0006c    //set GPIO109 alternate function :00---general gpio 
        ldr r2, =0x00000000
        str r2, [r1]
        
        ldr r1, =0x40e00118    //set GPIO109 as high
        ldr r2, =0xFE002000    
        str r2,[r1]    
	// ---- Wait 2 msec
	ldr r3, =0x40a00010
	ldr r2, [r3]
        add r4, r2,#0x2000
50:
	ldr r2, [r3] 
	cmp r4, r2
	bgt 50b

        ldr r1, =0x40e00124 //set GPIO109 low
        ldr r2, =0x00002000
        str r2,[r1]
	// ---- Wait 2 msec
	ldr r3, =0x40a00010
	ldr r2, [r3]
        add r4, r2,#0x2000
50:
	ldr r2, [r3] 
	cmp r4, r2
	bgt 50b

        ldr r1, =0x40e00118    //set GPIO109 as high
        ldr r2, =0xFE002000    
        str r2,[r1]    
#endif    
         
/************************/
/* Change Frequency!!!	*/										 
/************************/
 
//	ldr	r1, =0x110    	 		// 208MHz L=16 N=1
//	ldr     r1, =0x18f
change_frequency:
#if defined(SUMATRA)
	ldr	r1, =0x02000190			//L=16 2N=3 A=1
#else
	ldr	r1, =0x02000110			//L=16 2N=2 A=1
#endif
    	ldr    	r2, =0x41300000 		// Core Clock Config Reg
    	str    	r1, [r2]    			// set speed 

//    	mov	r1, #0x2   	     		// set to Frequency Change Mode  
//	mov     r1, #0x3
	mov     r1, #0xb			//B=1 turbo mode
	mcr	p14, 0, r1, c6, c0, 0		// write to the CLKCFG coprocessor reg 

/********************************************************************************/ 
/*  Changing the buffer strenght values for the memory controller signals 	*/
/********************************************************************************/ 
	ldr		r2, =0x33333333
	ldr		r3, =0x48000000		// Change buffer strength 
	str     	r2, [r3, #0x4c]		// program BSCNTR0
	str		r2, [r3, #0x5c]		// program BSCNTR2
	str		r2, [r3, #0x60]		// program BSCNTR3 
	ldr		r2, =0x33333353
	str		r2, [r3, #0x50]		// program BSCNTR1 

	ldr		r2, =0x00000000
	str		r2, [r3, #0x54]		// program BSCNTR1 
/***********************************************/


//  Initialize the OST count register to zero.
#if 0 
        ldr   r1,  =0x40a00010
        mov   r2,  #0
        str   r2,  [r1]
#endif

#endif // remove them, US MBM take them,just for products with MBM

//Configure LCD bufferstrength to be 0x5
	ldr		r3, =0x48000000		// Change buffer strength 
	ldr		r2, =0x00000005
	str		r2, [r3, #0x54]		// program BSCNTR1 
//End of LCDBS modification

// Initialize the power mgr registers.
        
        ldr     r2,  =0x40f00000	// get base address of power mgr / reset control regs

/* 
		set the immediate sleep mode on batt/vdd fault
        Note:  Although this mode is NOT recommended, we do not have data abort
        logic in place yet to trap this event and induce sleep nicely. 
*/
        mov     r1, #0                       	// induce sleep immediately on Fault
        str     r1, [r2, #0x00]

        // initialize the pcfr
        mov     r1, #0x00000001               	// disable 3.6864 MHZ crystal while in sleep mode
        orr     r1, r1, #0x00000002             // enable PCMCIA pin float during sleep
        orr     r1, r1, #0x00000004             // enable static memory pin float during sleep
        bic     r1, r1, #0x00000008             // disable deep-sleep mode
        str     r1, [r2, #0x1C]			//ower manager general configuration register


//  Re-Enable On-Chip Peripheral Clocking
//  note:  Currently Turning them all on (all 17)
        ldr     r1, =0x41300004
        ldr     r2, =0x0059ffff
        str     r2, [r1]

	/* for exit sleep/user off start */

	// check sleep/user off
	ldr	r1, =FLAG_ADDR
	ldr	r2, [r1]
        
	ldr	r3, =REFLASH_FLAG
	cmp	r2, r3
	bne	rdh_normal

        ldr r4, =0x40f00030	// RCSR
	ldr r5, [r4]
	str r5, [r4]		// clear RCSR
        ldr r4, =0x4010001c    // store the reset souce information into FFSPR
        str r5, [r4]
	tst r5, #1		// hardware reset
	bne check_user_off	
        
	ldr	r3, =SLEEP_FLAG
	cmp	r2, r3
	bne	scrub_sdram
	ldr	r1, =RESUME_ADDR	// resume from sleep
	ldr	pc, [r1]
        
rdh_normal:
	ldr r4, =0x40f00030	// RCSR
	ldr r5, [r4]
	str r5, [r4]		// clear RCSR
        ldr r4, =0x4010001c    // store the reset souce information into FFSPR
        str r5, [r4]
	tst r5, #4		// sleep reset
	beq check_user_off	
        
	ldr	r3, =SLEEP_FLAG
	cmp	r2, r3
	bne	check_user_off
	ldr	r1, =RESUME_ADDR	// resume from sleep
	ldr	pc, [r1]

check_user_off:
	// clear the RDH and PH bits in the PSSR
	ldr	r0, =0x40f00004
        ldr     r1, =0x30              
    	str	r1, [r0]

	ldr	r3, =USER_OFF_FLAG
	cmp	r2, r3
	bne	scrub_sdram
	
	// check option1(GPIO6) & 2(GPIO7) shorted
	ldr	r1, =0x40e00000
	ldr	r2, [r1, #0x0c]
	orr	r2, r2, #0x00000040	// set option1 output
	bic	r2, r2, #0x00000080	// set option2 input
	str	r2, [r1, #0x0c]

	ldr	r2, =0x00000040		// set option1 0
	str	r2, [r1, #0x24]

	ldr	r2, [r1]
	tst	r2, #0x00000080		// test option2
	bne	check_code		// not 0, not shorted


	ldr	r1, =0x40e00000
	ldr	r2, [r1, #0x0c]
	bic	r2, r2, #0x00000040	// set option1 input
	orr	r2, r2, #0x00000080	// set option2 output
	str	r2, [r1, #0x0c]

	ldr	r2, =0x00000080		// set option2 0
	str	r2, [r1, #0x24]

	ldr	r2, [r1]
	tst	r2, #0x00000040		// test option1
	beq	scrub_sdram		// 0, shorted 

check_code:
	// temporary comment the sentence to test sleep/user off ****
#if 0
	ldr	r1, =BARKER_CODE_ADDRESS
	ldr	r2, [r1]
	ldr	r3, =BARKER_CODE_VALID_VAL
	cmp	r2, r3
	bne	scrub_sdram	
#endif
	// handshake		
	ldr	r1, =0x40e00000
	ldr	r2, [r1, #0x10]
	bic	r2, r2, #0x00000002
	str	r2, [r1, #0x10]		// set MCU_INT_SW input

	mov	r0, #0
	ldr	r3, =BP_RDY_TIMEOUT
10:
	ldr	r2, [r1, #0x04]
	tst	r2, #0x00000002		// test MCU_INT_SW = 0
	beq	handshake
	ldr	r2, [r1]
	tst	r2, #0x00000001		// test BP_RDY = 0
	beq	handshake
	sub	r3, r3, #1
	cmp	r3, r0
	bne	10b

	// timeout, just power off
	ldr	r2, =0x00800000	// set GPIO87(WDI_AP) 0
	str	r2, [r1, #0x2c]

handshake:
	ldr	r2, [r1]
	tst	r2, #0x00000001		// wait BP_RDY = 0
	bne	handshake

	ldr	r2, [r1, #0x10]
	orr	r2, r2, #0x00000002	// set MCU_INT_SW output
	str	r2, [r1, #0x10]

	ldr	r2, =0x00000002	// set MCU_INT_SW = 0
	str	r2, [r1, #0x28]
20:
	ldr	r2, [r1]
	tst	r2, #0x00004000		// test BP_WDI2 = 0
	moveq	r0, #3
	mcreq	p14, 0, r0, c7, c0, 0	// go to sleep
	tst	r2, #0x00000001		// wait BP_RDY = 1
	beq	20b

	ldr	r2, =0x00000002	// set MCU_INT_SW = 1
	str	r2, [r1, #0x1c]

	ldr	r2, =RESUME_ADDR	// resume from user off
	ldr	pc, [r2]

scrub_sdram:
	
	/* for exit sleep/user off end */
         
	//   Scrub SDRAM iff Hard-Reset.
	//   32MB: Cotulla  16MB: Sabinal

/*	ldr 	r0, =0xa0000000     // phy start Adx
	mov 	r1, #1
	mov     r2, #0x100000
    	//mla     r3, r1, r2, r0      
    	ldr 	r3, =0xa1000000
    	ldr     r2, =0x00000000         // source data
100:    
    	sub     r3, r3, #4              // word pre-decrement
    	str     r2, [r3]                // 32-bit zero fill
    	cmp     r3, r0
    	bne     100b  
*/          	                  
	/* get a clue where we are running, so we know what to copy */
//	and	r0, pc, #0xff000000	/* we don't care about the low bits */
	adr	r0, _start
	
	/* relocate the second stage loader */
	add	r2, r0, #(128 * 1024)	/* blob maximum size is 128kB */
	add	r0, r0, #0x1000		/* skip first 4*1024 bytes */
	ldr	r1, =0xa0000000
	add	r1, r1, #0x1000		/* skip over here as well */

	/* r0 = source address
	 * r1 = target address
	 * r2 = source end address
	 */
copy_loop:
	ldmia	r0!, {r3-r10}
	stmia	r1!, {r3-r10}
	cmp	r0, r2
	ble	copy_loop


	/* set up the stack pointer */
	ldr	r0, =0xa0000000
	add	r1, r0, #(1024 * 1024)
	sub	sp, r1, #0x04
	
	/* blob is copied to ram, so jump to it */
	add	r0, r0, #0x1000
	mov	pc, r0
         

badram:
	b	blinky
	
blinky:
	/* This is test code to blink the LED
           very useful if nothing else works */
	b	blinky
undefined_instruction:
	b	blinky

software_interrupt:
	/* NOTE: This is NOT an error! If you think that blob should return
	 * from software interrupts, you're plain WRONG. The source of the
	 * problem is in the kernel: you should *disable* CONFIG_ANGELBOOT
	 * simply because blob is not angel. -- Erik
	 */
	b	blinky

prefetch_abort:
	b	blinky

data_abort:
	b	blinky

not_used:
	b	blinky

irq:
	b	blinky

fiq:
	b	blinky







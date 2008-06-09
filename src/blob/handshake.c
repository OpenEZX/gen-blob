/*
 * src/blob/handshake.c
 *
 * handshake with BP
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

#include <blob/types.h>
#include <blob/handshake.h>
#include <blob/pxa.h>

extern void set_GPIO_mode(u32 gpio_mode);
#ifdef BARBADOS
/* barbados don't do handshake in blob */
#endif

#ifdef MARTINIQUE
/* martinique don't do handshake in blob */
#endif

#ifdef HAINAN
u32 hainan_handshake(u32 x){
  /* not implement yet... */
  return 0;
}
#endif

/* -----------tool functions--------------------------------- */
#if 0
static u32 old_handshake(u32 data){
#if 0
#ifdef BARBADOS_HANDSHAKE
	waitTimeout = BARBADOS_BP_RDY_TIMEOUT;
	while ( waitTimeout > 0 ) {

		if ((GPLR(GPIO_BP_RDY)&GPIO_bit(GPIO_BP_RDY))) {
			break;
		}

		waitTimeout--;
	}
#else
	waitTimeout = BP_RDY_TIMEOUT;
	while ( waitTimeout > 0 ) {

		if ( (!( GPLR(GPIO_MCU_INT_SW) & GPIO_bit(GPIO_MCU_INT_SW) ))
			|| (!(  GPLR(GPIO_BP_RDY) & GPIO_bit(GPIO_BP_RDY) )) ) {
			break;
		}

		if(check_bb_wdi2_is_low()) {
			GPCR(GPIO_WDI_AP) = GPIO_bit(GPIO_WDI_AP);
			while(1);
		}

		waitTimeout--;
	}
#endif
     

	if ( waitTimeout <= 0 ) {
#ifndef FLASH_TOOL_TEST_VERSION
                /*  try to restart bp one time !  */
	        if(0 == flag) {
			flag = 1;
                    restart_bp();
			goto bprestart;
		}
#endif
		/* enter reflash mode */
		EnableLCD_8bit_active();
		printlcd("\n timeout reset BP! ");

		/* reset BP  and keep MCU_INT_SW as output low */
                set_GPIO_mode(GPIO_MCU_INT_SW | GPIO_OUT);
                GPCR(GPIO_MCU_INT_SW) = GPIO_bit(GPIO_MCU_INT_SW);
		restart_bp();
		printlcd("\ntimeout end reset BP ");

		output_reset_inf(sleepflag);

		// clear flash section descriptor C,B bits for burn code
		set_uncache_unbuffer(&io_map[0]);

        	/* enter reflash mode */

		printlcd("\n timeoutstart reflash code! ");             
		printlcd("\n\nEZX AP bootloader.\nVersion 3.0 2004-05-18");

		/* hwuart_init(230400); */
		for(; ;) {
			if(ICPR & 0x800) {
				udc_int_hndlr(0x11, 0);
			}
		}
	
		Disablelcd();
		Disablebklight();

    		return 0;
	}
#endif
	return 0;
}
#endif

void restart_bp(void)
{
	int waitTimeout;
        GPCR(GPIO_BB_RESET) = GPIO_bit(GPIO_BB_RESET);
#ifdef BARBADOS_HANDSHAKE
        GPSR(GPIO_BB_FLASHMODE_EN) = GPIO_bit(GPIO_BB_FLASHMODE_EN);
#endif
        waitTimeout = BB_RESET_TIMEOUT;
        while ( waitTimeout > 0 )
            waitTimeout--;
        GPSR(GPIO_BB_RESET) = GPIO_bit(GPIO_BB_RESET);
}


BOOL check_bb_wdi2_is_low(void)
{
        u32 i;
        set_GPIO_mode(GPIO_BB_WDI2 | GPIO_IN );
        
        for(i=0; i<1000; i++);

        for(i=0; i<3; i++) {
		if(GPLR(GPIO_BB_WDI2) & GPIO_bit(GPIO_BB_WDI2)) {
			return FALSE;
		}   
	}

        return TRUE; 
}


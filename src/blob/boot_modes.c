/*
 * src/blob/boot_modes.c
 * 
 * support more boot modes in blob
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
#include <blob/config.h>
#endif


#include <blob/types.h>
#include <blob/pxa.h>
#include <blob/handshake.h>
#include <blob/flash.h>
#include <blob/arch/lubbock.h>
#include <blob/boot_modes.h>
#include "usb_ctl_bvd.h"
#include <blob/lcd_ezx.h>
#include <blob/mmu.h>
#include "pcap.h"
#include <blob/log.h>
#include <common.h>
#include <part.h>
#include <mmc.h>

/* check BP_WDI flag */
#define bp_flag_not_set()	(*(unsigned long *)(BPSIG_ADDR) == NO_FLAG)
#define bp_flag_is_set()	(*(unsigned long *)(BPSIG_ADDR) == WDI_FLAG)


/* keypad macros */

//for P2 board //if( KPAS == (0x280000f4) ) 
//for 64MB dataflash P1 board 
//for P1 work -- if( KPAS == (0x04000022) )
#if defined(HAINAN) || defined(SUMATRA) 
#define FLASH_KEY      0x04000025
#define VOL_PLUS_KEY   0x04000042
#define VOL_MINUS_KEY  0x04000002
#define VOL_ENTER_KEY  0x04000025
#define KEY_DEBOUNCE 40000	/* debounce 40ms */
#else

#define FLASH_KEY 0x04000013
#define KEY_DEBOUNCE 40000	/* debounce 40ms */
#endif

static void naked_usb_flash(u32 data); /* a tool function just start usb flash loop */
extern int is_key_press_down(u32 key_val,u32 Deb_val);
extern void Delay(int uSec);
extern void udc_int_hndlr(int, void *);
extern int usbctl_init( void );
extern void USB_gpio_init(void);
extern struct mem_area io_map[];
/* structure note:
 This module includes all non-boot-linux boot modes in blob,
 such as flash, pass-through mode...etc*/

BOOL check_flash_flag(void)
{
  return (*(unsigned long *)(FLAG_ADDR) == REFLASH_FLAG);
}

BOOL check_valid_code(void)
{
  BOOL rev = TRUE;

#if 0	// wait for ZQ
  u32* pbarkercode = BARKER_CODE_ADDRESS;

  if ( (*pbarkercode) != BARKER_CODE_VALID_VAL )
    { 
      rev = FALSE;
    }
#endif
  return rev; 
}	
/*
 check_pass_through_flag()
 check if flag in memory for pass through mode is set
 return TRUE: pass-through mode
 return FALSE: no pass-thru mode
*/


BOOL check_pass_through_flag(void){
#if 1
  return (*(unsigned long *)(PASS_THRU_FLAG_ADDR) == PASS_THRU_FLAG);
#else  /* just for test phase */
  return TRUE;
#endif
}

/* 
 enter_pass_through_mode
 enter USB pass_through mode and do all initial work for emulator.
 blob will stay in this function in life time
*/
void enter_pass_through_mode(void){
  EnableLCD_8bit_active();
  printlcd("barbado_1223.\n ");
  //  STSerialOutputString("Enter pass-through mode.\n ");
  printlcd("Enter pass-through mode.\n ");

#if 0  //wave form test.we output square wave to test validaty of pin under test
  {
    u32 gpio=GPIO_BP_RDY;
    set_GPIO_mode(gpio | GPIO_OUT); 
   
    while(1){
      u32 i=0x80000;
     
      while(i<=0)
	i--;
      GPSR(gpio) = GPIO_bit(gpio); /* hign level output */
     
      i=0x80000;			/* restore timeout */

      while(i<=0)
	i--;     
      GPCR(gpio) = GPIO_bit(gpio); /* low level output */ 
     


    }
  }


#endif //end waveform test

  //step 1 finish last BP hand shake
#if 1   //we do handshake step 1 again, to make sure
  //   set_GPIO_mode(GPIO_MCU_INT_SW | GPIO_OUT);
  //  GPSR(GPIO_MCU_INT_SW) = GPIO_bit(GPIO_MCU_INT_SW); 
  //reset BP step by step
  GPSR(GPIO_BB_RESET) = GPIO_bit(GPIO_BB_RESET);

  GPCR(GPIO_BB_RESET) = GPIO_bit(GPIO_BB_RESET);

  set_GPIO_mode(GPIO_MCU_INT_SW | GPIO_OUT);

  GPSR(GPIO_MCU_INT_SW) = GPIO_bit(GPIO_MCU_INT_SW);  

  printlcd("bpreset low\n");
  //   restart_bp();
  //  set_GPIO_mode(GPIO_MCU_INT_SW | GPIO_IN);

  {
    int i=BB_RESET_TIMEOUT;
    while(i>0)
      i--;

  }
  if(!( GPLR(GPIO_MCU_INT_SW) & GPIO_bit(GPIO_MCU_INT_SW) ))
    printlcd("error sw low\n");
  set_GPIO_mode(GPIO_MCU_INT_SW | GPIO_IN);
  GPSR(GPIO_BB_RESET) = GPIO_bit(GPIO_BB_RESET);  
  printlcd("bpreset high\n");

  while( (( GPLR(GPIO_MCU_INT_SW) & GPIO_bit(GPIO_MCU_INT_SW) ))
	 && ((  GPLR(GPIO_BP_RDY) & GPIO_bit(GPIO_BP_RDY) )) );
#endif
  printlcd("pass-thru stab 0.\n ");
  while(GPLR(GPIO_BP_RDY) & GPIO_bit(GPIO_BP_RDY) );

#if 0
  while(1);
#endif 
  /*  wait some times before clear MCU_INT_SW */
  //  STSerialOutputString("pass-through mode. stab 1.\n ");
  {
    u32 i=0x1;
    u32 count=0xccccccc;
    while (i>0){
      while(count>0){
       
	count--;
      }
      i--;
     
    }
  }
  printlcd("pass-thru stab 1.\n ");
  //config MCU_INT_SW as output low

  set_GPIO_mode(GPIO_MCU_INT_SW | GPIO_OUT);
  GPCR(GPIO_MCU_INT_SW) = GPIO_bit(GPIO_MCU_INT_SW);
  
  while(!(GPLR(GPIO_BP_RDY) & GPIO_bit(GPIO_BP_RDY) ))
    {
#if 0
      static int i=0;
      if(!( GPLR(GPIO_MCU_INT_SW) & GPIO_bit(GPIO_MCU_INT_SW) )&& i<=10){
	
	  
	printlcd("MCU_int_sw=low\n");
	i++;

      }
#endif 

    }

  //  STSerialOutputString(" pass-through mode. stab 2.\n ");
  //  printlcd(" pass-thru stab 2.\n ");
  //config MCU_INT_SW as output high
  GPSR(GPIO_MCU_INT_SW) = GPIO_bit(GPIO_MCU_INT_SW); 
  printlcd(" Handshake with BP...Pass\n ");
  printlcd(" init USB gpio\n ");


#if 0
  while(1);
#endif 
  //step 2 set USB to pass-through mode
  
  usb_gpio_pass_through_init();

  EnableLCD_8bit_active();
  printlcd("Now blob in\npass-through mode\n");
  {
    u32 i=0x1;
    u32 count=0xccccccc;
    while (i>0){
      while(count>0){
       
	count--;
      }
      i--;
     
    }
  }
  pcap_switch_off_usb();
  {
    int i=5000;
    while(i--);

  }
  //  printlcd(" Auto plug USB cable\n ");  
  pcap_switch_on_usb();

  /*  now enter pass-through mode successfully, clear flag bit */
  *(unsigned long *)(PASS_THRU_FLAG_ADDR) =OFF_FLAG;
//  while(1);

}

//this function just set usb to pass-through mode
//for general flash setting, see usb_gpio_init()
void usb_gpio_pass_through_init(void){
  GPCR3=0x40E00124;
  GPDR0=0xC0000000;
  GPDR1=0x01000094;
  GPDR2=0x00040000;
  GPDR3=0x01800000;
  GAFR0_U=0xF0000000;
  GAFR1_L=0x00034190;
  GAFR1_U=0x00010800;
  GAFR2_U=0x00A00000;
  GAFR3_U=0x0000000C;
#ifdef SUMATRA
  GPCR(94) = GPIO_bit(94);
  set_GPIO_mode(94 | GPIO_OUT);
#endif
  PSSR=0x00000020;
  UP2OCR=0x01000000;
  __REG(0x40600024)=1;


}

#include <menu.h>
#define CONFIG_FILE "/boot/menu.lst"
#define LINE_HEIGHT 14 

static menu_entry_t *go_menu_entry = NULL;
void show_menu(menu_t *menu)
{
	int i;
	menu_entry_t *entry;
	clrScreen();
	printf("===============================\n");
	int y = menu->curr_entry * (LINE_HEIGHT+4) + 20;
	for (i = 0; i < LINE_HEIGHT; i++)
		drawline(i+y, 0, i+y, 230);

	entry = menu->entry;
	for (i = 0; i < menu->num; i++) {
		if (entry == NULL)
			break;
		printlcd(" ");
		if (menu->curr_entry == entry->id) {
			set_transparence(1);
			printlcd_rev(entry->title);
			set_transparence(0);
			go_menu_entry = entry;
		}
		else {
			printlcd(entry->title);
		}
		printlcd("\n");
#if 0
		printf("[%d], %08x\n", entry->id, entry->next);
		printf("%08x, %08x\n", entry->title, entry->kernel);
		printf("%08x, %08x\n", entry->param, entry->initrd);
		printf("[%d]%s\n", entry->id, entry->title);
		printf("   +---%s\n", entry->kernel);
		printf("   +---%s\n", entry->param);
		printf("   +---%s\n", entry->initrd);
#endif
		entry = entry->next;
	}
	printf("===============================\n");
}

int boot_menu(menu_t *menu)
{
	int ret;

	ret = parse_conf_file(CONFIG_FILE, menu);
	printf("num=%d, default=%d, curr=%08x, \ntimeout=%d, entry=%08x\n", 
	         menu->num, menu->default_entry, menu->curr_entry, menu->timeout, menu->entry);
	menu->curr_entry = menu->default_entry;
	if (menu->num == 0 || ret < 0)
		return -1;
	show_menu(menu);
	while (1) {
		u8 key = read_key();
		/* enter */
		if (key==0x25 || key==0x21 || key==0x14 || key==0x13)
			break;
		/* up */
		else if (key==0x42 || key==0x31 || key==0x04 || key==0x03) {
			if (menu->curr_entry == 0)
				menu->curr_entry = menu->num-1;
			else
				menu->curr_entry--;
			show_menu(menu);
		/* down */
		} else if (key==0x02 || key==0x11 || key==0x43 || key==0x44) {
			menu->curr_entry++;
			if (menu->curr_entry == menu->num)
				menu->curr_entry = 0;
			show_menu(menu);
		}
		Delay(300000);
	}
	return 0;
}

/* 
 * add enter_simple_pass_through_mode 
 * enter USB pass_through mode and do all initial work for emulator.
 * blob will stay in this function in life time
 */
void enter_simple_pass_through_mode(void)
{
	int ret;
	menu_t menu;
	char *kernel = "/boot/default";

  keypad_init();
//  EnableLCD_8bit_active();
  USB_gpio_init();
  usbctl_init();

  if (is_key_press_down(0x04000031, 0) ||
          is_key_press_down(0x04000003, 0)) {
  EnableLCD_8bit_active();

  GPCR(99) = GPIO_bit(99);  // USB_READY=low
  {
    int i=5000*4;
    while(i--);
  }
  GPSR(99) = GPIO_bit(99);  // USB_READY =high
  /* hwuart_init(230400); */
  printlcd ("USB ready\n");
  for(; ;) {
    if(ICPR & 0x800) {
      udc_int_hndlr(0x11, 0);
    }
  }
  }

  /* turn on the power */
  pcap_mmc_power_on(1);
  udelay(1000);
  ret = mmc_init(0);
  if (ret != 0) {
        EnableLCD_8bit_active();
  	printlcd("Cannot find MMC card\n");
	while (1);
  }

  file_detectfs();
  if (is_key_press_down(0x04000002, 0) ||
      is_key_press_down(0x04000043, 0)) {
  EnableLCD_8bit_active();
  ret = boot_menu(&menu);
  if (ret < 0 || !go_menu_entry) {
  	printlcd("Config file not found!\n");
	while (1);
  }
  kernel = go_menu_entry->kernel;
  }
  {
 int size;
#define KERNEL_RAM_BASE1 0xa0300000
  char *buf = (char *)KERNEL_RAM_BASE1;
  void (*theKernel)(void) = KERNEL_RAM_BASE1;
  printf("Loading %s...\n", kernel);
  size = file_fat_read(kernel, buf, 0x200000);  // 2MB

  /* turn off mmc controler, otherwise 2.4 kernel freezes */
  MMC_STRPCL = MMC_STRPCL_STOP_CLK;
  MMC_I_MASK = ~MMC_I_MASK_CLK_IS_OFF;
  CKEN &= ~(CKEN12_MMC);
  pcap_mmc_power_on(0);

  if (size > 0) {
  	printf("read %d bytes\nbooting...\n", size);
  	theKernel();
  }
	printf("Cannot load kernel from:\n   %s\n", kernel);
	while (1);
  }
}




/* from E680/A780 EMU , we must have the feature  VA+slide for E680 and VA+scroll key for A780 to trigger flash mode */
BOOL check_manual_flash_mode(void)
{
  BOOL rev = TRUE;


  if(is_key_press_down(FLASH_KEY,KEY_DEBOUNCE)) 
    {
      logstr("\nezx_test: REC button is pressed!!!.\n");
      rev = TRUE;
    }
  else
    {
      logstr("\nezx_test: REC button is NOT pressed!!!.\n");
      rev = FALSE;
    }

  return rev; 
}

void enter_manual_flash_mode(u32 data){
  u32 hold_count=0;	/* for hold key checking */
  EnableLCD_8bit_active();
#ifdef MBM
  printlcd("\n reset BP\nturn off usb\n");
#endif
#ifndef MBM
  printlcd("\n reset BP ");
#endif
  /* Force BP enter FLASH mode */
  GPSR(41) = GPIO_bit(41);   /* tide add the line to flash BP */
  
  restart_bp();
  printlcd("\n end reset BP ");
  
 
  /* check if key is holding, 4-5 sec to pass-thru BP mode */
  /* now use a long debounce to implement this delay */
  hold_count=8;
  do{
    if(!is_key_press_down(FLASH_KEY,KEY_DEBOUNCE))
      break; /* key released before time out, enter AP flash mode */
    hold_count--;
    //count*Delay=8*0x5=4 sec
    Delay(500*1000); /* check key per 0.5 s */
  }
  while(hold_count>0);

  if (hold_count>0)
    naked_usb_flash(0);
 
  printlcd("Enter BP Pass-thru\n");
  //timeout, enter bp only mode
  enter_simple_pass_through_mode();                    
  while(1) ; //stop here, AP has nothing to do...
  

 
  
}

void enter_tcmd_flash_mode(u32 data){
  *(unsigned long *)(FLAG_ADDR) = 0;
  EnableLCD_8bit_active();
  printlcd("\n reflash flag is set ");
  naked_usb_flash(0);

}
static void naked_usb_flash(u32 data){
  
  
  if (bp_flag_is_set())
    printlcd("\n BP assert BP_WDI before reset");
  if (bp_flag_not_set())
    printlcd("\n No BP_WDI before reset");
  *(unsigned long *)(BPSIG_ADDR) = 0;

  // clear flash section descriptor C,B bits for burn code
  //change mem map after mbm did can cause problem
  //now seems MBM turn off cache buffer, so skip this
#ifndef MBM
  set_uncache_unbuffer(&io_map[0]);
#endif
#ifdef MBM
		
  USB_gpio_init();
		
  usbctl_init();
#endif
  printlcd("\n AP+BP HAB reflash code! ");
#ifdef BARBADOS             
  printlcd("\n\nEZX AP bootloader.\n2005-JUN-07:2\n");
#endif
#ifndef BARBADOS
  printlcd("\n\nEZX AP bootloader.\n2005-JUN-07:2\n");
#endif		
  printlcd("switch off usb cable\n");
  GPCR(99) = GPIO_bit(99);  // USB_READY=low
  {
    int i=5000*4;
    while(i--);
		  
  }
  GPSR(99) = GPIO_bit(99);  // USB_READY =high
  printlcd("switch on usb cable\n");
  /* hwuart_init(230400); */
  for(; ;) {
    if(ICPR & 0x800) {
      udc_int_hndlr(0x11, 0);
    }
  }

  Disablelcd();
  Disablebklight();

}

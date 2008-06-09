/*-------------------------------------------------------------------------
 * Filename:      main.c
 * Version:       $Id: main.c,v 1.23 2002/01/07 14:48:25 seletz Exp $
 * Copyright:     Copyright (C) 1999, Jan-Derk Bakker
 * Author:        Jan-Derk Bakker <J.D.Bakker@its.tudelft.nl>
 * Description:   Main file for the trivially simple bootloader for the
 *                LART boards
 * Created at:    Mon Aug 23 20:00:00 1999
 * Modified by:   Erik Mouw <J.A.K.Mouw@its.tudelft.nl>
 * Modified at:   Sat Mar 25 14:31:16 2000
 *-----------------------------------------------------------------------*/
/*
 * main.c: main file for the blob bootloader
 * Copyright (C) 2004, 2005 Motorola Inc.
 * Copyright (C) 1999 2000 2001
 *   Jan-Derk Bakker (J.D.Bakker@its.tudelft.nl) and
 *   Erik Mouw (J.A.K.Mouw@its.tudelft.nl)
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
 * 2004-May-18 - (Motorola) Changed enter flash mode
 * 2004-Dec-13 - (Motorola) Added support for command line config and reflash 
                            options Changes made to support our platform
 * 2004-Dec-15 - (Motorola) Barbados power on and hanmdshake 
 * 2005-May-18 - (Motorola) Changes to enable manual flash mode
 *
 */
 

#ident "$Id: main.c,v 1.23 2002/01/07 14:48:25 seletz Exp $"

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/command.h>
#include <blob/errno.h>
#include <blob/error.h>

#include <blob/init.h>
#include <blob/led.h>
#include <blob/main.h>
#include <blob/md5.h>
#include <blob/md5support.h>
#include <blob/handshake.h>
#include <blob/param_block.h>
#include <blob/serial.h>
#include <blob/time.h>
#include <blob/util.h>
#include <blob/uucodec.h>
#include <blob/xmodem.h>
#include <blob/time.h>
#include <blob/lcd_ezx.h>
#include <blob/bt_uart.h>
#include <linux/string.h>


#include <blob/log.h>
#include <blob/HW_init.h>
#include <blob/SW_init.h>
#include <blob/boot_modes.h>

/* we need update to new config table design */
#define KCT_FLASH_ADDR  	0x67000// new start address of KCT tabe         
#define CMDL_TAG_ADDRESS        KCT_FLASH_ADDR //now, first tag is command line, and its addr is same as kct header       
#define CMDL_TAG_VALUE   	0x8000    
u32 cmd_flag=0;
/* ------------KCT tool functions------------ */
//get KCT head address, possible to a flexible design later
static u32 get_KCT_addr(void){
  return KCT_FLASH_ADDR;

}
//get first tag address based on a offset, possible to a flexible design later
static u32 get_cmdl_tag_addr(u32 offset){
  return CMDL_TAG_ADDRESS;

}
static u16 get_tag_value(u32 tag_addr){
  return *((u16 *)tag_addr);

}
static u16 get_tag_size(u32 tag_addr){
  u16 * p=(u16 *)tag_addr;
  p++;
  return *p;

}
static void* get_tag_content_addr(u32 tag_addr){
  u16 * p=(u16 *)tag_addr;
  p++;				/* skip tag value */
  p++;				/* skip tag size */
  return p;

}
//get and set blob tag in new kct design,
static void parse_blob_tag(void)
{
  u16 tag_val=0, tag_size=0;
  //get kct head addr
  u32 kct_head_addr=get_KCT_addr();

  //get first (command line) tag address
  u32 cmdl_tag_addr = get_cmdl_tag_addr(kct_head_addr);

  //get command_line_value and size
  tag_val=get_tag_value(cmdl_tag_addr);
  logvarhex("parse_blob_tag: tag_val=0x",tag_val);
  tag_size=get_tag_size(cmdl_tag_addr);
  logvarhex("parse_blob_tag: tag_size=0x",tag_size);
  //set first tag if possible

  if (CMDL_TAG_VALUE ==tag_val){
    char * cmdline=get_tag_content_addr(cmdl_tag_addr);
    cmd_flag = 1;
    memcpy(blob_status.cmdline, cmdline, tag_size);
    logstr("parse_blob_tag:cmdl ok\n");
  }else {
    cmd_flag = 0;
    blob_status.cmdline[0] = '\0';
  }

  //go to next tag....

}

/* end of -------KCT tool functions----------- */
#define FLASH_TOOL_TEST_VERSION
#define BARBADOS_HANDSHAKE
#ifdef BARBADOS_HANDSHAKE  //waiting for about 4s
#define BARBADOS_BP_RDY_TIMEOUT  40000000
#endif
//#define OUTPUT_LOG_TO_STPORT

#ifdef MBM
extern int boot_linux(int,char*);

#endif

#ifdef FLASH_TOOL_TEST_VERSION

char value_trans_to_char(u8 data )
{
	char ret ='F';

        if(data>15)
		return ret;

        if( data <10) {
		ret = '0'+data;
        } else {
		ret = 'a'+data-10;
        }

        return ret;
}

int translate_xnum_to_string( u8 * pStr,u32 num)
{
        int i;

        pStr[8]=0;
        for(i=0;i<8;i++) {
		pStr[7-i] = value_trans_to_char(num%16);
		num = num/16;           
        }
        return 0;
}

/* output reset information for debug */
void output_reset_inf(u32 sleepflag)
{
        u8 mystr[20];

        printlcd("\nFLAG=0x");
        translate_xnum_to_string( mystr,sleepflag);  
        printlcd(mystr);

        printlcd("\nRESET=0x");
        translate_xnum_to_string( mystr,FFSPR);  
        printlcd(mystr);

        printlcd("\nBRESET=0x");        
        translate_xnum_to_string( mystr,PSPR);  
        printlcd(mystr);

        printlcd("\nARESET=0x");        
        translate_xnum_to_string( mystr,RCSR);  
        printlcd(mystr);
}

/* save reset cause at previous time for debug */
void save_reset_inf(void)
{
        PSPR = RCSR;
}


#else

#define output_reset_inf(sleepflag)
#define save_reset_inf()
	
#endif


blob_status_t blob_status;

static int do_reload(char *what);

static char *boot_params[] = { // ALEX
	"root=3e00", 
	"rootfstype=cramfs",
	"ip=off",
	"paniclog=on",
};

#define NR_BOOT_PARAMS 4 //(sizeof(boot_params) / sizeof(char *))

int main(void)
{
	int retval = 0;

    u32 sleepflag;
#ifdef PARAM_START
	u32 conf;
#endif

	/* do hw init for product */
//	hw_init(0);  // ALEX don't need to initial hardware again

	/* do sw init for product */
//	sw_init(0);
	/* parse the core tag, for critical things like terminal speed */

#if 1 // ALEX test codes
{
	enter_simple_pass_through_mode();
}
#else
EnableLCD_8bit_active();
#endif // ALEX

#if 0
#ifdef PARAM_START
	parse_ptag((void *) PARAM_START, &conf);
#endif	
#if 1 //ALEX
  	printlcd("parse_blob_tag\n");      
	parse_blob_tag();
#endif

        sleepflag = *(unsigned long *)(FLAG_ADDR);
#ifndef HAINAN
	if ( check_flash_flag() )
	  enter_tcmd_flash_mode(0);
#endif	
  	printlcd("check_pass_through_flag\n");      
	/* add decision to pass-through mode */
	if(check_pass_through_flag())
	  enter_pass_through_mode();

#ifndef HAINAN
        if ( !check_valid_code()|| check_manual_flash_mode() ) 
	  enter_manual_flash_mode(0);
#endif
#endif
  	printlcd("loading kernel\n");      
#if 1 //ALEX
	/* Load kernel and ramdisk from flash to RAM */
	do_reload("kernel");
//      logstr("\nezx_test: Now, boot linux!\n");
      printlcd("ezx_test: Now, boot linux!\n");
//void (*theKernel)(void) = KERNEL_RAM_BASE;
//  	theKernel();
#endif
{
	long *p = (long *)0xa0800;
	int i;
	for (i = 0; i < 1000000; i++) {
		printf("%08x, %08x\n", *p++, *p++); 
		Delay(50);
	}

	while (1);
}

	save_reset_inf();

      printlcd("boot\n");
	if(retval == 0) {
#ifndef MBM
	  parse_command("boot");
#else
	  boot_linux(0,0);
//	  boot_linux(NR_BOOT_PARAMS+1, boot_params);
#endif
	}
printlcd("what????\n");
while(1);
	return 0;
} /* main */



static int do_reload(char *what)
{
	u32 *dst = 0;
	u32 *src = 0;
	int numWords;

	if(strncmp(what, "blob", 5) == 0) {
		dst = (u32 *)BLOB_RAM_BASE;
		src = (u32 *)BLOB_FLASH_BASE;
		numWords = BLOB_FLASH_LEN / 4;
		blob_status.blobSize = 0;
		blob_status.blobType = fromFlash;
		//SerialOutputString("Loading blob from flash ");
#ifdef PARAM_START
	} else if(strncmp(what, "param", 6) == 0) {
		dst = (u32 *)PARAM_RAM_BASE;
		src = (u32 *)PARAM_FLASH_BASE;
		numWords = PARAM_FLASH_LEN / 4;
		blob_status.paramSize = 0;
		blob_status.paramType = fromFlash;
		//SerialOutputString("Loading paramater block from flash ");
#endif
	} else if(strncmp(what, "kernel", 7) == 0) {
		dst = (u32 *)KERNEL_RAM_BASE;
		src = (u32 *)KERNEL_FLASH_BASE;
		numWords = KERNEL_FLASH_LEN / 4;
		blob_status.kernelSize = 0;
		blob_status.kernelType = fromFlash;
		//SerialOutputString("Loading kernel from flash ");
	} else if(strncmp(what, "ramdisk", 8) == 0) {
		dst = (u32 *)RAMDISK_RAM_BASE;
		src = (u32 *)RAMDISK_FLASH_BASE;
		numWords = RAMDISK_FLASH_LEN / 4;
		blob_status.ramdiskSize = 0;
		blob_status.ramdiskType = fromFlash;
		//SerialOutputString("Loading ramdisk from flash ");
	} else {
		printerror(EINVAL, what);
		return 0;
	}

	MyMemCpy(dst, src, numWords);
	//SerialOutputString(" done\n");

	return 0;
}


	


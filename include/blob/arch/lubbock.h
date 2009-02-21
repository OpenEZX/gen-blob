/* 
 * Copyright (C) 2002, 2005 Motorola Inc.
 * lubbock.h: Lubbock specific defines
 *
 * CPU type
 * o) PXA210 : sa
 * o) PXA250 : Cottula (A0/B0)
 * o) PXM260 : Cottula (C0)
 * o) PXM261 : Dalhart (A0) 
 * o) PXM262 : Dalhart (B0)
 * o) PXM263
 *
 * 2002-Dec-18 - add resume support to exiting sleep and user off mode.
 *               these definitions should be the same as kernel 
 * 2005-Mar-04 - add boot reason flag address define 
 * 2005-Dec-14 - add change for motorola products, based on original code.
 *
 */

#ifndef BLOB_ARCH_LUBBOCK_H
#define BLOB_ARCH_LUBBOCK_H

/* the base address were BLOB is loaded by the first stage loader */
#define BLOB_ABS_BASE_ADDR		(0xA1201000)

/* where do various parts live in RAM */
#define BLOB_RAM_BASE		(0xA0200000)
#define KERNEL_RAM_BASE		(0xA0300000)
#define PARAM_RAM_BASE		(0xA0140000)
#define RAMDISK_RAM_BASE	(0xA0400000)


/* and where do they live in flash */
#define BLOB_FLASH_BASE		(0xa0de0000)
#define BLOB_FLASH_LEN		(128 * 1024)
#define PARAM_FLASH_BASE	(BLOB_FLASH_BASE + BLOB_FLASH_LEN)
// #define PARAM_FLASH_LEN		(256 * 1024)
#define PARAM_FLASH_LEN		(0)
//#define KERNEL_FLASH_BASE	(PARAM_FLASH_BASE + PARAM_FLASH_LEN)
#define KERNEL_FLASH_BASE	(0xa0800)
#define KERNEL_FLASH_LEN	(1024 * 1024)
#define RAMDISK_FLASH_BASE	(KERNEL_FLASH_BASE + KERNEL_FLASH_LEN)
#define RAMDISK_FLASH_LEN	(4 * 1024 * 1024)

#define GEN_BLOB_MAX_SIZE	(128 * 1024)
#define KERN_MAX_SIZE		(896 * 1024) - 4
#define KERN_ON_FLASH_FLAG	0xDEADB007
#define GEN1_KERN_FLAG_ADDR	0x20000 + GEN_BLOB_MAX_SIZE
#define GEN2_KERN_FLAG_ADDR	0xa0000 + GEN_BLOB_MAX_SIZE



/* the position of the kernel boot parameters */
#define BOOT_PARAMS		(0xA0000100)


/* the size (in kbytes) to which the compressed ramdisk expands */
#define RAMDISK_SIZE		(8 * 1024)

#include "../pxa.h"

/* 
 * CPU specific Area 
 */

#define MEMC_BASE_PHYS	0x48000000
#define SDRAM_BASE_PHYS	0xA0000000
#define OSCR_BASE_PHYS	0x40A00010
#define FPGA_BASE_PHYS	0x08000000

#define MDCNFG_OFFSET	0x00
#define MDREFR_OFFSET	0x04
#define MSC0_OFFSET	0x08
#define MSC1_OFFSET	0x0C
#define MSC2_OFFSET	0x10
#define MECR_OFFSET	0x14
#define SXLCR_OFFSET	0x18
#define SXCNFG_OFFSET	0x1C
#define FLYCNFG_OFFSET	0x20
#define SXMRS_OFFSET	0x24
#define MCMEM0_OFFSET	0x28
#define MCMEM1_OFFSET	0x2C
#define MCATT0_OFFSET	0x30
#define MCATT1_OFFSET	0x34
#define MCIO0_OFFSET	0x38
#define MCIO1_OFFSET	0x3C
#define MDMRS_OFFSET	0x40
#define BOOT_DEF_OFFSET 0x44

#if ( defined(CPU_pxa250) || defined(CPU_pxa260) ) 

/* FPGA */
#define __LUB_REG(x)	(*(volatile unsigned long*)(x))
#define WHOAMI_OFFSET		0x00
#define HEX_LED_OFFSET		0x10
#define LED_BLANK_OFFSET 	0x40
#define CNFG_SW_OFFSET		0x50
#define USER_SW_OFFSET		0x60
#define MISC_WR_OFFSET		0x80

/* SDRAM */
#define MDCNFG_VAL	0x00001AC9
#define MDREFR_VAL	0x00018018 
#define MDMRS_VAL	0x00000000

/* Static Memory */
//#define MSC0_VAL	0x23F223F2
#define MSC0_VAL	0x23D223D2
#define MSC1_VAL	0x3FF1A441
#define MSC2_VAL	0x7FF07FF0

#elif (defined(CPU_pxa210) || defined(CPU_pxa261) )

/* FPGA */
#define __LUB_REG(x)		(*(volatile unsigned short*)(x))
#define WHOAMI_OFFSET		0x00
#define HEX_LED_OFFSET		0x08
#define LED_BLANK_OFFSET 	0x20
#define CNFG_SW_OFFSET		0x28
#define USER_SW_OFFSET		0x30
#define MISC_WR_OFFSET		0x40

/* SDRAM  */
#define MDCNFG_VAL	0x000009AC
#define MDREFR_VAL	0x000BF018 
#define MDMRS_VAL	0x00000000

/* Static Memory */
#define MSC0_VAL	0x24FA24FA
#define MSC1_VAL	0x3FF9A449
#define MSC2_VAL	0x7FF17FF1

#elif (defined(CPU_pxa262) )  /* Dalhart B0 */

/* FPGA */
#define __LUB_REG(x)	(*(volatile unsigned long*)(x))
#define WHOAMI_OFFSET		0x00
#define HEX_LED_OFFSET		0x10
#define LED_BLANK_OFFSET 	0x40
#define CNFG_SW_OFFSET		0x50
#define USER_SW_OFFSET		0x60
#define MISC_WR_OFFSET		0x80

/* SDRAM */
#define MDCNFG_VAL	0x000009AC
#define MDREFR_VAL	0x00018018 
#define MDMRS_VAL	0x00000000

/* Static Memory */
#define MSC0_VAL	0x24FA24FA
#define MSC1_VAL	0x3FF1A441
#define MSC2_VAL	0x7FF17FF1

#endif

/* GPIO settings */
#define GPSR0_VAL	0x00008000
#define GPSR1_VAL	0x00FC0382
#define GPSR2_VAL	0x0001FFFF

#define GPDR0_VAL	0x0060A800
#define GPDR1_VAL	0x00FF0382
#define GPDR2_VAL	0x0001C000

#define GPCR0_VAL	0x00000000
#define GPCR1_VAL	0x00000000
#define GPCR2_VAL	0x00000000

#define GAFR0_L_VAL	0x98400000
#define GAFR0_U_VAL	0x00002950
#define GAFR1_L_VAL	0x000A9558
#define GAFR1_U_VAL	0x0005AAAA
#define GAFR2_L_VAL	0xA0000000
#define GAFR2_U_VAL	0x00000002

#define PSSR_VAL	0x00000030

/* PCMCIA and CF Interfaces */
#define MECR_VAL	0x00000000
#define MCMEM0_VAL	0x00010504
#define MCMEM1_VAL	0x00010504
#define MCATT0_VAL	0x00010504
#define MCATT1_VAL	0x00010504
#define MCIO0_VAL	0x00004715
#define MCIO1_VAL	0x00004715
/* Sync Static Memory */
#define SXCNFG_VAL	0x00000000
#define SXMRS_VAL	0x00000000	/* NOT USED */
#define SXLCR_VAL	0x00000000	/* NOT USED */
#define FLYCNFG_VAL	0x01FE01FE	/* NOT USED */


#define WHOAMI		__LUB_REG(FPGA_BASE_PHYS + WHOAMI_OFFSET)
#define HEX_LED 	__LUB_REG(FPGA_BASE_PHYS + HEX_LED_OFFSET)
#define LED_BLANK 	__LUB_REG(FPGA_BASE_PHYS + LED_BLANK_OFFSET)
#define CNFG_SW		__LUB_REG(FPGA_BASE_PHYS + CFNG_SW_OFFSET)
#define USER_SW		__LUB_REG(FPGA_BASE_PHYS + USER_SW_OFFSET)
#define MISC_WR		__LUB_REG(FPGA_BASE_PHYS + MISC_WR_OFFSET)


/*
 * add resume support to exiting sleep and user off mode.
 * these definitions should be the same as kernel
 */

#define FLAG_ADDR		SDRAM_BASE_PHYS
#define RESUME_ADDR		(SDRAM_BASE_PHYS + 4)
#define BPSIG_ADDR		(SDRAM_BASE_PHYS + 8)
#define PASS_THRU_FLAG_ADDR FLAG_ADDR
/*
 * using boot reason flag at 0xa000000c (SDRAM_BASE_PHYS+12), please do not 
 * use 4 bytes from this address. see start.S and linux.c for more info
 */
#define USER_OFF_FLAG		0x5a5a5a5a
#define SLEEP_FLAG		0x6b6b6b6b
#define OFF_FLAG		0x7c7c7c7c
#define REFLASH_FLAG		0x0C1D2E3F
#define PASS_THRU_FLAG          0x12345678
#define WDI_FLAG		0xbb00dead
#define NO_FLAG			0xaa00dead
#define DUMPKEYS_FLAG		0x1EE7F1A6
#define USBMODE_FLAG		0x0D3ADCA7

#define BARKER_CODE_ADDRESS	0x1FE0000
#define BARKER_CODE_VALID_VAL	0x000000B1

#define BP_RDY_TIMEOUT		0x000C0000

#define GPIO0_BP_RDY     	0x00000001
#define GPIO27_BB_RESET		0x08000000
#define GPIO33_MCU_INT_SW	0x00000002
#define GPIO87_AP_WDI		0x00800000
#define GPIO5_OPTION1		0x00000020
#define GPIO6_OPTION2		0x00000040

#define GPIO6_OPTION1		0x00000040
#define GPIO7_OPTION2		0x00000080

#endif

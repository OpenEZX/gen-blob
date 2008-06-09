/*
 * include/blob/lcd_ezx.h
 *
 * Copyright (C) 2000,  Intel Corporation.
 * Copyright (C) 2003-2005 Motorola Inc.
 *
 * 2003-Mar-18 - (Motorola) Remove lcd descriptor and frambuffer struct defs
 * 2004-May-17 - (Motorola) add the GPIO macro for EZX.
 * 2005-Jun-07 - (Motorola) add BARBADOS HANDSHAKE
 */

#ifndef HEADER_LCD_EZX
#define HEADER_LCD_EZX

#define  BARBADOS_HANDSHAKE


#define GPIO_TC_MM_EN          99      /* TC_MM_EN   */
#if defined(BARBADOS_HANDSHAKE)
#define GPIO_BB_FLASHMODE_EN   41      /*for Barbados*/
#endif

/* control PCAP direct PIN */
#define GPIO_WDI_AP            4       /* WDI_AP                       */
#define GPIO_SYS_RESTART       55      /* restart PCAP power           */

/* communicate with PCAP's PIN  */
#define GPIO_PCAP_SEC_INT      1       /* PCAP interrupt PIN to AP     */ 
#define GPIO_SPI_CLK           29      /* PCAP SPI port clock          */ 
#define GPIO_SPI_CE            24      /* PCAP SPI port SSPFRM         */  
#define GPIO_SPI_MOSI          25      /* PCAP SPI port SSPTXD         */ 
#define GPIO_SPI_MISO          26      /* PCAP SPI port SSPRXD         */ 

/*  blue tooth control PIN   */
#define GPIO_BT_WAKEUP         28      /* AP wake up bluetooth module  */
#define GPIO_BT_HOSTWAKE       14      /* AP wake up bluetooth module  */
#define GPIO_BT_RESET          48      /* AP reset bluetooth module    */

/* control LCD high - OFF low -- ON  */
#define GPIO_LCD_OFF           116     /* control LCD                */

#define GPIO_EMU_MUX1      120
#define GPIO_EMU_MUX2      119
#define GPIO_SNP_INT_CTL   86


/* VA select pin  */
#define GPIO_VA_SEL_BUL     79

/* out filter select pin */
#define GPIO_FLT_SEL_BUL    80

/* USB client related GPIO pin */
#define GPIO34_TXENB_MD     (34 | GPIO_ALT_FN_1_OUT)
#define GPIO35_XRXD_MD      (35 | GPIO_ALT_FN_2_IN )
#define GPIO36_VMOUT_MD     (36 | GPIO_ALT_FN_1_OUT)
#define GPIO39_VPOUT_MD     (39 | GPIO_ALT_FN_1_OUT)
#define GPIO40_VPIN_MD      (40 | GPIO_ALT_FN_3_IN )
#define GPIO53_VMIN_MD      (53 | GPIO_ALT_FN_2_IN )



//#define SDRAM_B0 0xA0C00000 /*change to after blob in RAM*/
#define SDRAM_B0 0xA0070000
/* LCD Control Register 0 Bits */
#define LCD0_V_OUM 21
#define LCD0_V_BM  20
#define LCD0_V_PDD 12
#define LCD0_V_QDM 11
#define LCD0_V_DIS 10
#define LCD0_V_DPD 9
#define LCD0_V_BLE 8
#define LCD0_V_PAS 7
#define LCD0_V_EFM 6
#define LCD0_V_IUM 5
#define LCD0_V_SFM 4
#define LCD0_V_LDM 3
#define LCD0_V_SDS 2
#define LCD0_V_CMS 1
#define LCD0_V_ENB 0


/* LCD control Register 1 Bits */
#define LCD1_M_PPL 0x000003FF
#define LCD1_M_HSW 0x0000FC00
#define LCD1_M_ELW 0x00FF0000
#define LCD1_M_BLW 0xFF000000
#define LCD1_V_PPL 0
#define LCD1_V_HSW 10
#define LCD1_V_ELW 16
#define LCD1_V_BLW 24

/* LCD control Register 2 Bits */
#define LCD2_M_LPP 0x000003FF
#define LCD2_M_VSW 0x0000FC00
#define LCD2_M_EFW 0x00FF0000
#define LCD2_M_BFW 0xFF000000
#define LCD2_V_LPP 0
#define LCD2_V_VSW 10
#define LCD2_V_EFW 16
#define LCD2_V_BFW 24

/* LCD control Register 3 Bits */
#define LCD3_M_PCD 0x000000FF
#define LCD3_M_ACB 0x0000FF00
#define LCD3_M_API 0x000F0000
#define LCD3_M_VSP 0x00100000
#define LCD3_M_HSP 0x00200000
#define LCD3_M_PCP 0x00400000
#define LCD3_M_OEP 0x00800000
#define LCD3_V_PCD 0
#define LCD3_V_ACB 8
#define LCD3_V_API 16
#define LCD3_V_VSP 20
#define LCD3_V_HSP 21
#define LCD3_V_PCP 22
#define LCD3_V_OEP 23
#define LCD3_V_BPP 24
#define LCD3_V_DPC 27

/*
struct FrameBuffer_8bit {
  unsigned short palette [256];
  unsigned char pixel[320][240];
} *fb_8bit;


struct _LCD_Descriptor {
	unsigned long FDADR;				// frame descriptor address
	unsigned long FSADR;				// frame start address
	unsigned long FIDR;					// frame id reg
	unsigned long LDCMD;				// LCD command reg
} *LCD_Descriptor_PAL, *LCD_Descriptor_HI, *LCD_Descriptor_LO;
*/

void EnableLCD_8bit_active(void);
void printlcd(char *cntrl_string);
void Delay(int uSec);
int checkkey(void);
void Disablelcd();
void Disablebklight();
#endif

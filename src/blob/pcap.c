/*
 * src/blob/pcap.c
 *
 * PCAP APIs
 *
 * Copyright (C) 2002, 2004, 2005 Motorola Inc.
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
 * 2002-Jul-02 - (Motorola) Create
 * 2004-Jan-20 - (Motorola) Porting the pcap code to pcap 2
 * 2005-Dec-14 - (Motorola) Modified for EZX platform
 *
 */

#include <config.h>
#include <arch.h>
/*#include <errno.h>
#include <error.h>
#include <init.h>
#include <led.h>
#include <main.h>
#include <memory.h>
#include <serial.h>
#include <util.h>
#include <time.h>
#include <lcd_ezx.h>*/
#include <linux/string.h>
#include <pxa.h>
#include <blob/types.h>
#include <blob/log.h>
#include <pcap.h>

static u32 ssp_pcap_operate_pin_with_inverter;
static u32 ssp_pcap_rxd_pin_with_inverter;

static u32 ssp_pcap_registerValue[SSP_PCAP_REGISTER_NUMBER] = { 0 };

SSP_PCAP_STATUS
SSP_PCAP_write_data_to_PCAP(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER
			    ssp_pcap_register, u32 ssp_pcap_register_value);
SSP_PCAP_STATUS
SSP_PCAP_read_data_from_PCAP(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER
			     ssp_pcap_register,
			     u32 * p_ssp_pcap_register_value);

SSP_PCAP_STATUS
SSP_PCAP_bit_set(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE ssp_pcap_bit);
SSP_PCAP_STATUS
SSP_PCAP_bit_clean(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE ssp_pcap_bit);
SSP_PCAP_BIT_STATUS
    SSP_PCAP_get_bit_from_buffer
    (SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE ssp_pcap_bit);
SSP_PCAP_BIT_STATUS
    SSP_PCAP_get_bit_from_PCAP
    (SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE ssp_pcap_bit);
u32 SSP_PCAP_get_register_value_from_buffer
    (SSP_PCAP_SECONDARY_PROCESSOR_REGISTER ssp_pcap_register);

void ssp_pcap_init(void);

void set_GPIO_mode(u32 gpio_mode)
{
	int gpio = gpio_mode & GPIO_MD_MASK_NR;
	int fn = (gpio_mode & GPIO_MD_MASK_FN) >> 8;
	int gafr;

	if (gpio_mode & GPIO_MD_MASK_DIR) {
		GPDR(gpio) |= GPIO_bit(gpio);
	} else {
		GPDR(gpio) &= ~GPIO_bit(gpio);
	}

	gafr = GAFR(gpio) & ~(0x3 << (((gpio) & 0xf) * 2));
	GAFR(gpio) = gafr | (fn << (((gpio) & 0xf) * 2));
}

/*---------------------------------------------------------------------------
  DESCRIPTION:

   INPUTS:
      
       
   OUTPUTS:
      
       
   IMPORTANT NOTES:
       
     
---------------------------------------------------------------------------*/

static u32 test_pcap_cs_rxd(void)
{
	u32 ret = FALSE;
	u32 temp;
	/* write a data 0x0003ffff to IMR */
	SSP_PCAP_write_data_to_PCAP(1, 0x0003ffff);
	SSP_PCAP_read_data_from_PCAP(1, &temp);
	if (0x0003ffff == temp)
		ret = TRUE;
	return ret;
}

static void ssp_pcap_gpioInit(void)
{
	u32 i;
	u32 tempValue;
	u32 ret;
	/* stop SPI port work mode  disable SPI clock */
	SSCR0 = 0x00000000;
	CKEN &= ~CKEN23_SSP1;

	/* the four PIN as GPIO mode */
	set_GPIO_mode(GPIO_SPI_CLK | GPIO_OUT);
	set_GPIO_mode(GPIO_SPI_CE | GPIO_OUT);
	set_GPIO_mode(GPIO_SPI_MOSI | GPIO_OUT);
	set_GPIO_mode(GPIO_SPI_MISO | GPIO_IN);

	/* get GPDR0 GPLR0 GAFR0_L GAFR0_U */

#if 0
	STSerialOutputString(" \n GPDR0 =0x");
	STSerialOutputHex(GPDR0);

	STSerialOutputString(" \n GPLR0 =0x");
	STSerialOutputHex(GPLR0);

	STSerialOutputString(" \n GAFR0_L =0x");
	STSerialOutputHex(GAFR0_L);

	STSerialOutputString(" \n GAFR0_U =0x");
	STSerialOutputHex(GAFR0_U);

	STSerialOutputString(" \n  init PCAP spi !!! ");
#endif

	/* test pcap's CE with inverter??? */
	ssp_pcap_operate_pin_with_inverter = 0;
	ssp_pcap_rxd_pin_with_inverter = 0;

	/* deassert SEC_CE  */
	GPCR(GPIO_SPI_CE) = GPIO_bit(GPIO_SPI_CE);
	for (i = 0; i < 500; i++) ;
	ret = test_pcap_cs_rxd();

	if (FALSE == ret) {
		ssp_pcap_operate_pin_with_inverter = 0;
		ssp_pcap_rxd_pin_with_inverter = 1;
		/* deassert SEC_CE  */
		GPCR(GPIO_SPI_CE) = GPIO_bit(GPIO_SPI_CE);
		for (i = 0; i < 500; i++) ;
		ret = test_pcap_cs_rxd();
	}

	if (FALSE == ret) {
		ssp_pcap_operate_pin_with_inverter = 1;
		ssp_pcap_rxd_pin_with_inverter = 0;
		/* deassert SEC_CE  */
		GPSR(GPIO_SPI_CE) = GPIO_bit(GPIO_SPI_CE);
		for (i = 0; i < 500; i++) ;
		ret = test_pcap_cs_rxd();
	}

	if (FALSE == ret) {
		ssp_pcap_operate_pin_with_inverter = 1;
		ssp_pcap_rxd_pin_with_inverter = 1;
		/* deassert SEC_CE  */
		GPSR(GPIO_SPI_CE) = GPIO_bit(GPIO_SPI_CE);
		for (i = 0; i < 500; i++) ;
		ret = test_pcap_cs_rxd();
	}

	if (FALSE == ret) {
		logstr(" \n can't communicate with bulverde !!! ");
		return;
	}

	SSP_PCAP_write_data_to_PCAP(1, 0x0013ffff);

	for (i = 0; i < 32; i++) {
		if (SSP_PCAP_SUCCESS ==
		    SSP_PCAP_read_data_from_PCAP(i, &tempValue)) {
			ssp_pcap_registerValue[i] = tempValue;
		} else {
			ssp_pcap_registerValue[i] = 0;
		}
	}
	return;
}

static void ssp_pcap_gpioClkControl(u32 bitValue)
{
	if (bitValue) {
		GPSR(GPIO_SPI_CLK) = GPIO_bit(GPIO_SPI_CLK);
	} else {
		GPCR(GPIO_SPI_CLK) = GPIO_bit(GPIO_SPI_CLK);
	}
}

static void ssp_pcap_gpioFrmControl(u32 bitValue)
{
	if (ssp_pcap_operate_pin_with_inverter) {
		if (bitValue) {
			GPCR(GPIO_SPI_CE) = GPIO_bit(GPIO_SPI_CE);
		} else {
			GPSR(GPIO_SPI_CE) = GPIO_bit(GPIO_SPI_CE);
		}
	} else {
		if (bitValue) {
			GPSR(GPIO_SPI_CE) = GPIO_bit(GPIO_SPI_CE);
		} else {
			GPCR(GPIO_SPI_CE) = GPIO_bit(GPIO_SPI_CE);
		}
	}
}

static void ssp_pcap_gpioTxdControl(u32 bitValue)
{
	if (bitValue) {
		GPSR(GPIO_SPI_MOSI) = GPIO_bit(GPIO_SPI_MOSI);
	} else {
		GPCR(GPIO_SPI_MOSI) = GPIO_bit(GPIO_SPI_MOSI);
	}
}

static u32 ssp_pcap_gpioRxdGetStatus(void)
{
	if (ssp_pcap_rxd_pin_with_inverter) {
		if (GPLR(GPIO_SPI_MISO) & GPIO_bit(GPIO_SPI_MISO)) {
			return SSP_PCAP_BIT_ZERO;
		} else {
			return SSP_PCAP_BIT_ONE;
		}
	} else {
		if (GPLR(GPIO_SPI_MISO) & GPIO_bit(GPIO_SPI_MISO)) {
			return SSP_PCAP_BIT_ONE;
		} else {
			return SSP_PCAP_BIT_ZERO;
		}
	}
}

static void ssp_pcap_gpioWrite(u32 pcapValue)
{
	u32 tempValue;
	u32 loopValue = 0x80000000;
	int i;
	/*  prepare for starting the frame  */
	ssp_pcap_gpioClkControl(SSP_PCAP_BIT_ZERO);
	ssp_pcap_gpioFrmControl(SSP_PCAP_BIT_ZERO);
	ssp_pcap_gpioTxdControl(SSP_PCAP_BIT_ZERO);

	/*   start the data transfering frame  */
	ssp_pcap_gpioFrmControl(SSP_PCAP_BIT_ONE);
	for (i = 0; i < 32; i++) {
		tempValue = pcapValue & loopValue;
		/* setup data bit to TXD line    */
		ssp_pcap_gpioTxdControl(tempValue);
		/*  create clock   */
		ssp_pcap_gpioClkControl(SSP_PCAP_BIT_ONE);
		loopValue = loopValue / 2;
		ssp_pcap_gpioClkControl(SSP_PCAP_BIT_ZERO);
	}
	/*   end the frame */
	ssp_pcap_gpioFrmControl(SSP_PCAP_BIT_ZERO);
	return;
}

/*---------------------------------------------------------------------------
  DESCRIPTION:

   INPUTS:
   OUTPUTS:

   IMPORTANT NOTES:

---------------------------------------------------------------------------*/

static void ssp_pcap_gpioRead(u32 registerNumber, u32 * pPcapValue)
{
	int i;
	u32 tempValue;
	u32 loopValue = 0x80000000;

	/*  prepare for starting the frame  */
	ssp_pcap_gpioClkControl(SSP_PCAP_BIT_ZERO);
	ssp_pcap_gpioFrmControl(SSP_PCAP_BIT_ZERO);
	ssp_pcap_gpioTxdControl(SSP_PCAP_BIT_ZERO);

	/* start the data transfering frame   */
	ssp_pcap_gpioFrmControl(SSP_PCAP_BIT_ONE);
	/* indictaor it's a read data command */
	ssp_pcap_gpioTxdControl(SSP_PCAP_BIT_ZERO);
	ssp_pcap_gpioClkControl(SSP_PCAP_BIT_ONE);
	loopValue = loopValue / 2;
	ssp_pcap_gpioClkControl(SSP_PCAP_BIT_ZERO);

	for (i = 0; i < 5; i++) {
		/*  here maybe can be optimized */
		tempValue = registerNumber & (loopValue / 0x04000000);
		ssp_pcap_gpioTxdControl(tempValue);
		ssp_pcap_gpioClkControl(SSP_PCAP_BIT_ONE);
		loopValue = loopValue / 2;
		ssp_pcap_gpioClkControl(SSP_PCAP_BIT_ZERO);
	}

	/*  the dead bit  */
	ssp_pcap_gpioClkControl(SSP_PCAP_BIT_ONE);
	loopValue = loopValue / 2;
	ssp_pcap_gpioClkControl(SSP_PCAP_BIT_ZERO);

	tempValue = 0;
	for (i = 0; i < 25; i++) {
		tempValue |= loopValue * ssp_pcap_gpioRxdGetStatus();
		ssp_pcap_gpioClkControl(SSP_PCAP_BIT_ONE);
		loopValue = loopValue / 2;
		ssp_pcap_gpioClkControl(SSP_PCAP_BIT_ZERO);
	}
	/*   end the data frame   */
	ssp_pcap_gpioFrmControl(SSP_PCAP_BIT_ZERO);
	if (pPcapValue) {
		*pPcapValue = tempValue;
	}
	return;
}

/*---------------------------------------------------------------------------
  DESCRIPTION:
      
   INPUTS:
      
       
   OUTPUTS:
      
       
   IMPORTANT NOTES:
       
     
---------------------------------------------------------------------------*/
static unsigned char pcap_is_init = 0;

void pcap_switch_off_usb(void)
{
	if (!pcap_is_init) {
		pcap_is_init = 1;
		ssp_pcap_init();
	}
	SSP_PCAP_get_bit_from_PCAP(SSP_PCAP_ADJ_BIT_BUSCTRL_USB_PU);
	SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_BUSCTRL_USB_PU);
	return;
}

void pcap_switch_on_usb(void)
{
	if (!pcap_is_init) {
		pcap_is_init = 1;
		ssp_pcap_init();
	}
	SSP_PCAP_get_bit_from_PCAP(SSP_PCAP_ADJ_BIT_BUSCTRL_USB_PU);
	SSP_PCAP_bit_set(SSP_PCAP_ADJ_BIT_BUSCTRL_USB_PU);
	return;
}

void ssp_pcap_init(void)
{
	ssp_pcap_gpioInit();
	return;
}

void ssp_pcap_open(SSP_PCAP_INIT_DRIVER_TYPE portType)
{
	switch (portType) {
	case SSP_PCAP_TS_OPEN:
		SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_ADC1_TS_REFENB);
		SSP_PCAP_bit_set(SSP_PCAP_ADJ_BIT_ISR_ADCDONE2I);
		SSP_PCAP_bit_set(SSP_PCAP_ADJ_BIT_ISR_TSI);
		SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_MSR_TSM);
		SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_MSR_ADCDONE2M);
		SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_ADC2_ADINC1);
		SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_ADC2_ADINC2);
		SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_ADC1_ATO0);
		SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_ADC1_ATO1);
		SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_ADC1_ATO2);
		SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_ADC1_ATO3);
		SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_ADC1_ATOX);
		SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_ADC1_MTR1);
		SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_ADC1_MTR2);
		break;
	case SSP_PCAP_AUDIO_OPEN:
		break;
	default:
		break;
	}
	return;
}

/*---------------------------------------------------------------------------
  DESCRIPTION:
      
   INPUTS:
      
       
   OUTPUTS:
      
       
   IMPORTANT NOTES:
       
     
---------------------------------------------------------------------------*/
SSP_PCAP_STATUS
SSP_PCAP_write_data_to_PCAP(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER
			    sspPcapRegister, u32 registerValue)
{
	u32 pcapTempValue;
	/* prevent the process schedule out and mask the PCAP's interrupt handler */

	switch (sspPcapRegister) {
	case SSP_PCAP_ADJ_ISR_REGISTER:
	case SSP_PCAP_ADJ_MSR_REGISTER:
	case SSP_PCAP_ADJ_PSTAT_REGISTER:
	case SSP_PCAP_ADJ_AUX_VREG_REGISTER:
	case SSP_PCAP_ADJ_ADC1_REGISTER:
	case SSP_PCAP_ADJ_ADC2_REGISTER:
	case SSP_PCAP_ADJ_AUD_CODEC_REGISTER:
	case SSP_PCAP_ADJ_AUD_RX_AMPS_REGISTER:
	case SSP_PCAP_ADJ_ST_DAC_REGISTER:
	case SSP_PCAP_ADJ_BUSCTRL_REGISTER:
	case SSP_PCAP_ADJ_PERIPH_REGISTER:
	case SSP_PCAP_ADJ_TX_AUD_AMPS_REGISTER:
	case SSP_PCAP_ADJ_GP_REG_REGISTER:
		ssp_pcap_registerValue[sspPcapRegister] =
		    registerValue & SSP_PCAP_REGISTER_VALUE_MASK;
		pcapTempValue =
		    SSP_PCAP_REGISTER_WRITE_OP_BIT | (sspPcapRegister <<
						      SSP_PCAP_REGISTER_ADDRESS_SHIFT)
		    | (ssp_pcap_registerValue[sspPcapRegister]);

		ssp_pcap_gpioWrite(pcapTempValue);
		return SSP_PCAP_SUCCESS;
		/* maybe here should NOT be a break */
		break;
	default:

		return SSP_PCAP_ERROR_REGISTER;
		/* maybe here should NOT be a break */
		break;
	}
	return SSP_PCAP_NOT_RUN;
}

/*---------------------------------------------------------------------------
  DESCRIPTION:
      
   INPUTS:
      
       
   OUTPUTS:
      
       
   IMPORTANT NOTES:
       
     
---------------------------------------------------------------------------*/
SSP_PCAP_STATUS
SSP_PCAP_read_data_from_PCAP(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER
			     sspPcapRegister, u32 * pSspPcapRegisterValue)
{
	/* prevent the process schedule out and mask the PCAP's interrupt handler */
	switch (sspPcapRegister) {
	case SSP_PCAP_ADJ_ISR_REGISTER:
	case SSP_PCAP_ADJ_MSR_REGISTER:
	case SSP_PCAP_ADJ_PSTAT_REGISTER:
	case SSP_PCAP_ADJ_AUX_VREG_REGISTER:
	case SSP_PCAP_ADJ_ADC1_REGISTER:
	case SSP_PCAP_ADJ_ADC2_REGISTER:
	case SSP_PCAP_ADJ_AUD_CODEC_REGISTER:
	case SSP_PCAP_ADJ_AUD_RX_AMPS_REGISTER:
	case SSP_PCAP_ADJ_ST_DAC_REGISTER:
	case SSP_PCAP_ADJ_BUSCTRL_REGISTER:
	case SSP_PCAP_ADJ_PERIPH_REGISTER:
	case SSP_PCAP_ADJ_TX_AUD_AMPS_REGISTER:
	case SSP_PCAP_ADJ_GP_REG_REGISTER:
		ssp_pcap_gpioRead(sspPcapRegister, pSspPcapRegisterValue);
		ssp_pcap_registerValue[sspPcapRegister] =
		    *pSspPcapRegisterValue & SSP_PCAP_REGISTER_VALUE_MASK;
		return SSP_PCAP_SUCCESS;
		/* maybe here should NOT be a break */
		break;
	default:
		return SSP_PCAP_ERROR_REGISTER;
		/* maybe here should NOT be a break */
		break;
	}
}

/*---------------------------------------------------------------------------
  DESCRIPTION:
      
   INPUTS:
      
       
   OUTPUTS:
      
       
   IMPORTANT NOTES:
       
     
---------------------------------------------------------------------------*/
SSP_PCAP_STATUS
SSP_PCAP_bit_set(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE sspPcapBit)
{
	u32 sspPcapRegisterBitValue;
	SSP_PCAP_STATUS ret;
	u8 sspPcapRegister =
	    (sspPcapBit & SSP_PCAP_REGISTER_ADDRESS_MASK) >>
	    SSP_PCAP_REGISTER_ADDRESS_SHIFT;

	switch (sspPcapRegister) {
	case SSP_PCAP_ADJ_ISR_REGISTER:
		ssp_pcap_registerValue[sspPcapRegister] = 0;
	case SSP_PCAP_ADJ_MSR_REGISTER:
	case SSP_PCAP_ADJ_PSTAT_REGISTER:
	case SSP_PCAP_ADJ_AUX_VREG_REGISTER:
	case SSP_PCAP_ADJ_ADC1_REGISTER:
	case SSP_PCAP_ADJ_ADC2_REGISTER:
	case SSP_PCAP_ADJ_AUD_CODEC_REGISTER:
	case SSP_PCAP_ADJ_AUD_RX_AMPS_REGISTER:
	case SSP_PCAP_ADJ_ST_DAC_REGISTER:
	case SSP_PCAP_ADJ_BUSCTRL_REGISTER:
	case SSP_PCAP_ADJ_PERIPH_REGISTER:
	case SSP_PCAP_ADJ_TX_AUD_AMPS_REGISTER:
	case SSP_PCAP_ADJ_GP_REG_REGISTER:

		sspPcapRegisterBitValue =
		    (sspPcapBit & SSP_PCAP_REGISTER_VALUE_MASK);
		ssp_pcap_registerValue[sspPcapRegister] |=
		    sspPcapRegisterBitValue;
		/* should care the return value */
		ret =
		    SSP_PCAP_write_data_to_PCAP(sspPcapRegister,
						ssp_pcap_registerValue
						[sspPcapRegister]);
		return SSP_PCAP_SUCCESS;
		break;
	default:
		return SSP_PCAP_ERROR_REGISTER;
		break;
	}
	return SSP_PCAP_SUCCESS;
}

/*---------------------------------------------------------------------------
  DESCRIPTION:

   INPUTS:
      
       
   OUTPUTS:
      
       
   IMPORTANT NOTES:
       
     
---------------------------------------------------------------------------*/
SSP_PCAP_STATUS
SSP_PCAP_bit_clean(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE sspPcapBit)
{
	u32 sspPcapRegisterBitValue;
	SSP_PCAP_STATUS ret;
	u8 sspPcapRegister =
	    (sspPcapBit & SSP_PCAP_REGISTER_ADDRESS_MASK) >>
	    SSP_PCAP_REGISTER_ADDRESS_SHIFT;

	switch (sspPcapRegister) {
	case SSP_PCAP_ADJ_ISR_REGISTER:
		ssp_pcap_registerValue[sspPcapRegister] = 0;
	case SSP_PCAP_ADJ_MSR_REGISTER:
	case SSP_PCAP_ADJ_PSTAT_REGISTER:
	case SSP_PCAP_ADJ_AUX_VREG_REGISTER:
	case SSP_PCAP_ADJ_ADC1_REGISTER:
	case SSP_PCAP_ADJ_ADC2_REGISTER:
	case SSP_PCAP_ADJ_AUD_CODEC_REGISTER:
	case SSP_PCAP_ADJ_AUD_RX_AMPS_REGISTER:
	case SSP_PCAP_ADJ_ST_DAC_REGISTER:
	case SSP_PCAP_ADJ_BUSCTRL_REGISTER:
	case SSP_PCAP_ADJ_PERIPH_REGISTER:
	case SSP_PCAP_ADJ_TX_AUD_AMPS_REGISTER:
	case SSP_PCAP_ADJ_GP_REG_REGISTER:
		sspPcapRegisterBitValue =
		    (sspPcapBit & SSP_PCAP_REGISTER_VALUE_MASK);
		ssp_pcap_registerValue[sspPcapRegister] &=
		    ~sspPcapRegisterBitValue;
		/* should care the return value */
		ret =
		    SSP_PCAP_write_data_to_PCAP(sspPcapRegister,
						ssp_pcap_registerValue
						[sspPcapRegister]);
		return SSP_PCAP_SUCCESS;
		break;
	default:
		return SSP_PCAP_ERROR_REGISTER;
		break;
	}
	return SSP_PCAP_SUCCESS;
}

/*---------------------------------------------------------------------------
  DESCRIPTION:
      
   INPUTS:
      
       
   OUTPUTS:
      
       
   IMPORTANT NOTES:

     
---------------------------------------------------------------------------*/
SSP_PCAP_BIT_STATUS
    SSP_PCAP_get_bit_from_buffer
    (SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE sspPcapBit) {
	u32 sspPcapRegisterBitValue;
	u8 sspPcapRegister =
	    (sspPcapBit & SSP_PCAP_REGISTER_ADDRESS_MASK) >>
	    SSP_PCAP_REGISTER_ADDRESS_SHIFT;
	switch (sspPcapRegister) {
	case SSP_PCAP_ADJ_ISR_REGISTER:
	case SSP_PCAP_ADJ_MSR_REGISTER:
	case SSP_PCAP_ADJ_PSTAT_REGISTER:
	case SSP_PCAP_ADJ_AUX_VREG_REGISTER:
	case SSP_PCAP_ADJ_ADC1_REGISTER:
	case SSP_PCAP_ADJ_ADC2_REGISTER:
	case SSP_PCAP_ADJ_AUD_CODEC_REGISTER:
	case SSP_PCAP_ADJ_AUD_RX_AMPS_REGISTER:
	case SSP_PCAP_ADJ_ST_DAC_REGISTER:
	case SSP_PCAP_ADJ_BUSCTRL_REGISTER:
	case SSP_PCAP_ADJ_PERIPH_REGISTER:
	case SSP_PCAP_ADJ_TX_AUD_AMPS_REGISTER:
	case SSP_PCAP_ADJ_GP_REG_REGISTER:
		sspPcapRegisterBitValue =
		    (sspPcapBit & SSP_PCAP_REGISTER_VALUE_MASK);
		sspPcapRegisterBitValue &=
		    ssp_pcap_registerValue[sspPcapRegister];
		if (sspPcapRegisterBitValue) {
			return SSP_PCAP_BIT_ONE;
		} else {
			return SSP_PCAP_BIT_ZERO;
		}
		break;
	default:
		return SSP_PCAP_BIT_ERROR;
		break;
	}
	return SSP_PCAP_BIT_ERROR;
}

/*---------------------------------------------------------------------------
  DESCRIPTION:
      
   INPUTS:
      
       
   OUTPUTS:
      
       
   IMPORTANT NOTES:
       
     
---------------------------------------------------------------------------*/
SSP_PCAP_BIT_STATUS
    SSP_PCAP_get_bit_from_PCAP
    (SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE sspPcapBit) {
	u32 sspPcapTempRegisterValue;
	u32 sspPcapRegisterBitValue;
	SSP_PCAP_STATUS ret;
	u8 sspPcapRegister =
	    (sspPcapBit & SSP_PCAP_REGISTER_ADDRESS_MASK) >>
	    SSP_PCAP_REGISTER_ADDRESS_SHIFT;

	switch (sspPcapRegister) {
	case SSP_PCAP_ADJ_ISR_REGISTER:
	case SSP_PCAP_ADJ_MSR_REGISTER:
	case SSP_PCAP_ADJ_PSTAT_REGISTER:
	case SSP_PCAP_ADJ_AUX_VREG_REGISTER:
	case SSP_PCAP_ADJ_ADC1_REGISTER:
	case SSP_PCAP_ADJ_ADC2_REGISTER:
	case SSP_PCAP_ADJ_AUD_CODEC_REGISTER:
	case SSP_PCAP_ADJ_AUD_RX_AMPS_REGISTER:
	case SSP_PCAP_ADJ_ST_DAC_REGISTER:
	case SSP_PCAP_ADJ_BUSCTRL_REGISTER:
	case SSP_PCAP_ADJ_PERIPH_REGISTER:
	case SSP_PCAP_ADJ_TX_AUD_AMPS_REGISTER:
	case SSP_PCAP_ADJ_GP_REG_REGISTER:
		sspPcapRegisterBitValue =
		    (sspPcapBit & SSP_PCAP_REGISTER_VALUE_MASK);
		/* should care the return value */
		ret =
		    SSP_PCAP_read_data_from_PCAP(sspPcapRegister,
						 &sspPcapTempRegisterValue);
		sspPcapRegisterBitValue &= sspPcapTempRegisterValue;
		if (sspPcapRegisterBitValue) {
			return SSP_PCAP_BIT_ONE;
		} else {
			return SSP_PCAP_BIT_ZERO;
		}
		break;
	default:
		return SSP_PCAP_BIT_ERROR;
		break;
	}
	return SSP_PCAP_BIT_ERROR;
}

/*---------------------------------------------------------------------------
  DESCRIPTION:
      
   INPUTS:
      
       
   OUTPUTS:
      
       
   IMPORTANT NOTES:
       
     
---------------------------------------------------------------------------*/
u32 SSP_PCAP_get_register_value_from_buffer
    (SSP_PCAP_SECONDARY_PROCESSOR_REGISTER sspPcapRegister) {
	switch (sspPcapRegister) {
	case SSP_PCAP_ADJ_ISR_REGISTER:
	case SSP_PCAP_ADJ_MSR_REGISTER:
	case SSP_PCAP_ADJ_PSTAT_REGISTER:
	case SSP_PCAP_ADJ_AUX_VREG_REGISTER:
	case SSP_PCAP_ADJ_ADC1_REGISTER:
	case SSP_PCAP_ADJ_ADC2_REGISTER:
	case SSP_PCAP_ADJ_AUD_CODEC_REGISTER:
	case SSP_PCAP_ADJ_AUD_RX_AMPS_REGISTER:
	case SSP_PCAP_ADJ_ST_DAC_REGISTER:
	case SSP_PCAP_ADJ_BUSCTRL_REGISTER:
	case SSP_PCAP_ADJ_PERIPH_REGISTER:
	case SSP_PCAP_ADJ_TX_AUD_AMPS_REGISTER:
	case SSP_PCAP_ADJ_GP_REG_REGISTER:
		return ssp_pcap_registerValue[sspPcapRegister];
		break;
	default:
		return SSP_PCAP_ERROR_REGISTER;
		break;
	}
}

/*==================================== ALEX =============================== */
#define DEBUG printf

#define VAP_TF		0x00000B00
#define VAP_SD		0x00000060
#define VAP_TF_MASK	0xFFFFF0FF
#define VAP_SD_MASK	0xFFFFFF9F

void pcap_mmc_power_on(int on)
{
	unsigned long ssp_pcap_register_val;
	if (!pcap_is_init) {
		pcap_is_init = 1;
		ssp_pcap_init();
	}

	/* close VAP_MMC (PCAP_VAUX3 && PCAP_VAUX2) */
	SSP_PCAP_write_data_to_PCAP(SSP_PCAP_ADJ_AUX_VREG_REGISTER, 0);

	Delay(10000);
	if (on == 0)		// Power off only
		return;

	/* open VAP_MMC (PCAP VAUX3 && VAUX2) */
	SSP_PCAP_read_data_from_PCAP(SSP_PCAP_ADJ_AUX_VREG_REGISTER,
				     &ssp_pcap_register_val);
	ssp_pcap_register_val =
	    (ssp_pcap_register_val & (VAP_TF_MASK | VAP_SD_MASK)) |
	    (VAP_TF | VAP_SD);
	SSP_PCAP_write_data_to_PCAP(SSP_PCAP_ADJ_AUX_VREG_REGISTER,
				    ssp_pcap_register_val);
	SSP_PCAP_bit_set(SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX3_EN);
	SSP_PCAP_bit_set(SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX2_EN);

	SSP_PCAP_read_data_from_PCAP(SSP_PCAP_ADJ_AUX_VREG_REGISTER,
				     &ssp_pcap_register_val);
//      DEBUG("AUX_VREG=0x%x\n", ssp_pcap_register_val);
}

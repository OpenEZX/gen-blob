/*
 * src/blob/pcap.h
 *
 * PCAP header
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
 * 2002-Jul-01 - (Motorola) Create
 * 2004-Feb-09 - (Motorola) Porting for bulverde rdl
 * 2005-Dec-14 - (Motorola) Modified for EZX platform
 *
 */

#ifndef SSP_PCAP_H
#define SSP_PCAP_H

#ifdef __cplusplus
extern "C" {
#endif
#include "types.h"
#define SSP_vibrate_start_command()                SSP_PCAP_bit_set(SSP_PCAP_ADJ_BIT_AUX_VREG_V_VIB_EN); \
                                                   SSP_PCAP_bit_set(SSP_PCAP_ADJ_BIT_AUX_VREG_V_VIB_EN)    

#define SSP_vibrate_stop_command()                 SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_AUX_VREG_V_VIB_EN); \
                                                   SSP_PCAP_bit_clean(SSP_PCAP_ADJ_BIT_AUX_VREG_V_VIB_EN) 

#define SSP_PCAP_REGISTER_VALUE_LENGTH            16

#define SSP_PCAP_REGISTER_WRITE_OP_BIT            0x80000000
#define SSP_PCAP_REGISTER_READ_OP_BIT             0x00000000

#define SSP_PCAP_REGISTER_VALUE_UP_WORD_MASK      0xffff0000
#define SSP_PCAP_REGISTER_VALUE_DOWN_WORD_MASK    0x0000ffff

#define SSP_PCAP_REGISTER_VALUE_MASK              0x01ffffff
#define SSP_PCAP_REGISTER_VALUE_MASK              0x01ffffff
#define SSP_PCAP_REGISTER_ADDRESS_MASK            0x7c000000
#define SSP_PCAP_REGISTER_ADDRESS_SHIFT           26
#define SSP_PCAP_REGISTER_NUMBER                  32

#define SSP_PCAP_ADC_START_VALUE_SET_MASK                  0xfffffc00
#define SSP_PCAP_ADC_START_VALUE                           0x000001dd


#define SSP_PCAP_PHONE_CDC_CLOCK_MASK                      0x000001c0
#define SSP_PCAP_STEREO_SAMPLE_RATE_MASK                   0x00000f00
#define SSP_PCAP_STEREO_BCLK_TIME_SLOT_MASK                0x00018000
#define SSP_PCAP_STEREO_CLOCK_MASK                         0x0000001c
#define SSP_PCAP_DIGITAL_AUDIO_MODE_MASK                   0x00180000
#define SSP_PCAP_TOUCH_PANEL_POSITION_DETECT_MODE_MASK     0x000e0000
#define SSP_PCAP_MONO_PGA_MASK                             0x00006000

#define SSP_PCAP_VIBRATOR_VOLTAGE_LEVEL_MASK               0x00300000

#define SSP_PCAP_AUDIO_IN_GAIN_MASK                        0x0000001f
#define SSP_PCAP_AUDIO_IN_GAIN_SHIFT                       0
#define SSP_PCAP_AUDIO_OUT_GAIN_MASK                       0x0001e000
#define SSP_PCAP_AUDIO_OUT_GAIN_SHIFT                      13


#define SSP_PCAP_ADD1_VALUE_MASK                           0x000003ff
#define SSP_PCAP_ADD1_VALUE_SHIFT                          0
#define SSP_PCAP_ADD2_VALUE_MASK                           0x000ffc00
#define SSP_PCAP_ADD2_VALUE_SHIFT                          10


#define PCAP_AUDIO_IN_GAIN_MAX_VALUE                       31
#define PCAP_AUDIO_OUT_GAIN_MAX_VALUE                      15

#define PCAP_CLEAR_INTERRUPT_REGISTER                      0x00141fdf
#define PCAP_MASK_ALL_INTERRUPT                            0x0013ffff

#define SSP_PCAP_BASE                                      0x10000000



/************************ STRUCTURES, ENUMS, AND TYPEDEFS **************************/
typedef enum pcapReturnStatus
{
	SSP_PCAP_SUCCESS                    = 0,
	SSP_PCAP_ERROR_REGISTER             = SSP_PCAP_BASE+1,
	SSP_PCAP_ERROR_VALUE                = SSP_PCAP_BASE+2,

	SSP_PCAP_NOT_RUN                    = SSP_PCAP_BASE+0xff
}SSP_PCAP_STATUS;

typedef enum pcapPortType
{
        SSP_PCAP_SERIAL_PORT                = 0x00000000,
        SSP_PCAP_LOW_USB_PORT               = 0x00000001,
        SSP_PCAP_HIGH_USB_PORT              = 0x00000002,
        SSP_PCAP_UNKNOW_PORT                = 0x000000ff
}SSP_PCAP_PORT_TYPE;

typedef enum pcapInitDriverType
{
        SSP_PCAP_TS_OPEN                    = 0x00000000,
        SSP_PCAP_AUDIO_OPEN                 = 0x00000001,
        SSP_PCAP_UNKNOW_DRIVER_OPEN         = 0x000000ff
}SSP_PCAP_INIT_DRIVER_TYPE;


typedef enum pcapReturnBitStatus
{
	SSP_PCAP_BIT_ZERO                   = 0x00000000,
	SSP_PCAP_BIT_ONE                    = 0x00000001,
	SSP_PCAP_BIT_ERROR                  = 0xff000000
}SSP_PCAP_BIT_STATUS;

typedef enum pcapCDCClkType
{
	PCAP_CDC_CLK_IN_13M0                  = 0x00000000,
	PCAP_CDC_CLK_IN_15M36                 = 0x00000040,
	PCAP_CDC_CLK_IN_16M8                  = 0x00000080,
	PCAP_CDC_CLK_IN_19M44                 = 0x000000c0,
	PCAP_CDC_CLK_IN_26M0                  = 0x00000100
}PHONE_CDC_CLOCK_TYPE;

typedef enum pcapST_SR
{
	PCAP_ST_SAMPLE_RATE_8K                = 0x00000000,
	PCAP_ST_SAMPLE_RATE_11K               = 0x00000100,
	PCAP_ST_SAMPLE_RATE_12K               = 0x00000200,
	PCAP_ST_SAMPLE_RATE_16K               = 0x00000300,
	PCAP_ST_SAMPLE_RATE_22K               = 0x00000400,
	PCAP_ST_SAMPLE_RATE_24K               = 0x00000500,
	PCAP_ST_SAMPLE_RATE_32K               = 0x00000600,
	PCAP_ST_SAMPLE_RATE_44K               = 0x00000700,
	PCAP_ST_SAMPLE_RATE_48K               = 0x00000800
}ST_SAMPLE_RATE_TYPE;

typedef enum pcapST_BCLK
{
	PCAP_ST_BCLK_SLOT_16                  = 0x00000000,
	PCAP_ST_BCLK_SLOT_8                   = 0x00008000,
	PCAP_ST_BCLK_SLOT_4                   = 0x00010000,
	PCAP_ST_BCLK_SLOT_2                   = 0x00018000,
}ST_BCLK_TIME_SLOT_TYPE;

typedef enum pcapST_CLK
{
	PCAP_ST_CLK_PLL_CLK_IN_13M0           = 0x00000000,
	PCAP_ST_CLK_PLL_CLK_IN_15M36          = 0x00000004,
	PCAP_ST_CLK_PLL_CLK_IN_16M8           = 0x00000008,
	PCAP_ST_CLK_PLL_CLK_IN_19M44          = 0x0000000c,
	PCAP_ST_CLK_PLL_CLK_IN_26M0           = 0x00000010,
	PCAP_ST_CLK_PLL_CLK_IN_EXT_MCLK       = 0x00000014,
	PCAP_ST_CLK_PLL_CLK_IN_FSYNC          = 0x00000018,
	PCAP_ST_CLK_PLL_CLK_IN_BITCLK         = 0x0000001c
}ST_CLK_TYPE;

typedef enum pcapDigitalAudioInterfaceMode
{
	PCAP_DIGITAL_AUDIO_INTERFACE_NORMAL   = 0x00000000,
	PCAP_DIGITAL_AUDIO_INTERFACE_NETWORK  = 0x00002000,
	PCAP_DIGITAL_AUDIO_INTERFACE_I2S      = 0x00004000
}DIG_AUD_MODE_TYPE;

typedef enum pcapMono
{
	PCAP_MONO_PGA_R_L_STEREO             = 0x00000000,
	PCAP_MONO_PGA_RL                     = 0x00080000,
	PCAP_MONO_PGA_RL_3DB                 = 0x00100000,
	PCAP_MONO_PGA_RL_6DB                 = 0x00180000
}MONO_TYPE;

typedef enum pcapVibratorVoltageLevel
{
	PCAP_VIBRATOR_VOLTAGE_LEVEL0         = 0x00000000,
	PCAP_VIBRATOR_VOLTAGE_LEVEL1         = 0x00100000,
	PCAP_VIBRATOR_VOLTAGE_LEVEL2         = 0x00200000,
	PCAP_VIBRATOR_VOLTAGE_LEVEL3         = 0x00300000
}VibratorVoltageLevel_TYPE;

typedef enum pcapTouchScreenMode
{
	PCAP_TS_POSITION_X_MEASUREMENT       = 0x00000000,
	PCAP_TS_POSITION_XY_MEASUREMENT      = 0x00020000,
	PCAP_TS_PRESSURE_MEASUREMENT         = 0x00040000,
	PCAP_TS_PLATE_X_MEASUREMENT          = 0x00060000,
	PCAP_TS_PLATE_Y_MEASUREMENT          = 0x00080000,
	PCAP_TS_STANDBY_MODE                 = 0x000a0000,
	PCAP_TS_NONTS_MODE                   = 0x000c0000
}TOUCH_SCREEN_DETECT_TYPE;

typedef enum pcapADJRegister
{
	SSP_PCAP_ADJ_ISR_REGISTER           = 0x00,
	SSP_PCAP_ADJ_MSR_REGISTER           = 0x01,
	SSP_PCAP_ADJ_PSTAT_REGISTER         = 0x02,
	SSP_PCAP_ADJ_AUX_VREG_REGISTER      = 0x07,
	SSP_PCAP_ADJ_ADC1_REGISTER          = 0x09,
	SSP_PCAP_ADJ_ADC2_REGISTER          = 0x0a,
	SSP_PCAP_ADJ_AUD_CODEC_REGISTER     = 0x0b,
	SSP_PCAP_ADJ_AUD_RX_AMPS_REGISTER   = 0x0c,
	SSP_PCAP_ADJ_ST_DAC_REGISTER        = 0x0d,
	SSP_PCAP_ADJ_BUSCTRL_REGISTER       = 0x14,
	SSP_PCAP_ADJ_PERIPH_REGISTER        = 0x15,
	SSP_PCAP_ADJ_TX_AUD_AMPS_REGISTER   = 0x1a,
	SSP_PCAP_ADJ_GP_REG_REGISTER        = 0x1b
}SSP_PCAP_SECONDARY_PROCESSOR_REGISTER;

typedef enum pcapADJBit_SetType
{
	SSP_PCAP_ADJ_BIT_ISR_ADCDONEI               = 0x00000001,
	SSP_PCAP_ADJ_BIT_ISR_TSI                    = 0x00000002,
	SSP_PCAP_ADJ_BIT_ISR_1HZI                   = 0x00000004,
	SSP_PCAP_ADJ_BIT_ISR_WHI                    = 0x00000008,
	SSP_PCAP_ADJ_BIT_ISR_WLI                    = 0x00000010,
	SSP_PCAP_ADJ_BIT_ISR_TODAI                  = 0x00000020,
	SSP_PCAP_ADJ_BIT_ISR_USB4VI                 = 0x00000040,
	SSP_PCAP_ADJ_BIT_ISR_ONOFFI                 = 0x00000080,
	SSP_PCAP_ADJ_BIT_ISR_ONOFF2I                = 0x00000100,
	SSP_PCAP_ADJ_BIT_ISR_USB1VI                 = 0x00000200,
	SSP_PCAP_ADJ_BIT_ISR_MOBPORTI               = 0x00000400,
	SSP_PCAP_ADJ_BIT_ISR_MB2I                   = 0x00000800,
	SSP_PCAP_ADJ_BIT_ISR_A1I                    = 0x00001000,
	SSP_PCAP_ADJ_BIT_ISR_STI                    = 0x00002000,
	SSP_PCAP_ADJ_BIT_ISR_PCI                    = 0x00004000,
	SSP_PCAP_ADJ_BIT_ISR_WARMI                  = 0x00008000,
	SSP_PCAP_ADJ_BIT_ISR_EOLI                   = 0x00010000,
	SSP_PCAP_ADJ_BIT_ISR_CLKI                   = 0x00020000,
	SSP_PCAP_ADJ_BIT_ISR_SYS_RSTI               = 0x00040000,
	SSP_PCAP_ADJ_BIT_ISR_ADCDONE2I              = 0x00100000,

	SSP_PCAP_ADJ_BIT_MSR_ADCDONEM               = 0x04000001,
	SSP_PCAP_ADJ_BIT_MSR_TSM                    = 0x04000002,
	SSP_PCAP_ADJ_BIT_MSR_1HZM                   = 0x04000004,
	SSP_PCAP_ADJ_BIT_MSR_WHM                    = 0x04000008,
	SSP_PCAP_ADJ_BIT_MSR_WLM                    = 0x04000010,
	SSP_PCAP_ADJ_BIT_MSR_TODAM                  = 0x04000020,
	SSP_PCAP_ADJ_BIT_MSR_USB4VM                 = 0x04000040,
	SSP_PCAP_ADJ_BIT_MSR_ONOFFM                 = 0x04000080,
	SSP_PCAP_ADJ_BIT_MSR_ONOFF2M                = 0x04000100,
	SSP_PCAP_ADJ_BIT_MSR_USB1VM                 = 0x04000200,
	SSP_PCAP_ADJ_BIT_MSR_MOBPORTM               = 0x04000400,
	SSP_PCAP_ADJ_BIT_MSR_MB2M                   = 0x04000800,
	SSP_PCAP_ADJ_BIT_MSR_A1M                    = 0x04001000,
	SSP_PCAP_ADJ_BIT_MSR_STM                    = 0x04002000,
	SSP_PCAP_ADJ_BIT_MSR_PCM                    = 0x04004000,
	SSP_PCAP_ADJ_BIT_MSR_WARMM                  = 0x04008000,
	SSP_PCAP_ADJ_BIT_MSR_EOLM                   = 0x04010000,
	SSP_PCAP_ADJ_BIT_MSR_CLKM                   = 0x04020000,
	SSP_PCAP_ADJ_BIT_MSR_SYS_RSTM               = 0x04040000,
	SSP_PCAP_ADJ_BIT_MSR_ADCDONE2M              = 0x04100000,

	SSP_PCAP_ADJ_BIT_PSTAT_USBDET_4V            = 0x08000040,
	SSP_PCAP_ADJ_BIT_PSTAT_ONOFFSNS             = 0x08000080,
	SSP_PCAP_ADJ_BIT_PSTAT_ONOFFSNS2            = 0x08000100,
	SSP_PCAP_ADJ_BIT_PSTAT_USBDET_1V            = 0x08000200,
	SSP_PCAP_ADJ_BIT_PSTAT_MOBSENSB             = 0x08000400,
	SSP_PCAP_ADJ_BIT_PSTAT_MB2SNS               = 0x08000800,
	SSP_PCAP_ADJ_BIT_PSTAT_A1SNS                = 0x08001000,
	SSP_PCAP_ADJ_BIT_PSTAT_MSTB                 = 0x08002000,
	SSP_PCAP_ADJ_BIT_PSTAT_EOL_STAT             = 0x08010000,
	SSP_PCAP_ADJ_BIT_PSTAT_CLK_STAT             = 0x08020000,
	SSP_PCAP_ADJ_BIT_PSTAT_SYS_RST              = 0x08040000,

	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX1_EN          = 0x1c000002,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX1_0           = 0x1c000004,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX1_1           = 0x1c000008,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX2_EN          = 0x1c000010,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX2_0           = 0x1c000020,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX2_1           = 0x1c000040,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX3_EN          = 0x1c000080,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX3_0           = 0x1c000100,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX3_1           = 0x1c000200,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX3_2           = 0x1c000400,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX3_3           = 0x1c000800,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX4_EN          = 0x1c001000,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX4_0           = 0x1c002000,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VAUX4_1           = 0x1c004000,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VSIM2_EN          = 0x1c010000,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VSIM_EN           = 0x1c020000,
	SSP_PCAP_ADJ_BIT_AUX_VREG_VSIM_0            = 0x1c040000,
	SSP_PCAP_ADJ_BIT_AUX_VREG_V_VIB_EN          = 0x1c080000,
	SSP_PCAP_ADJ_BIT_AUX_VREG_V_VIB_0           = 0x1c100000,
	SSP_PCAP_ADJ_BIT_AUX_VREG_V_VIB_1           = 0x1c200000,

	SSP_PCAP_ADJ_BIT_ADC1_ADEN                  = 0x24000001,
	SSP_PCAP_ADJ_BIT_ADC1_RAND                  = 0x24000002,
	SSP_PCAP_ADJ_BIT_ADC1_AD_SEL1               = 0x24000004,
	SSP_PCAP_ADJ_BIT_ADC1_AD_SEL2               = 0x24000008,
	SSP_PCAP_ADJ_BIT_ADC1_ADA10                 = 0x24000010,
	SSP_PCAP_ADJ_BIT_ADC1_ADA11                 = 0x24000020,
	SSP_PCAP_ADJ_BIT_ADC1_ADA12                 = 0x24000040,
	SSP_PCAP_ADJ_BIT_ADC1_ADA20                 = 0x24000080,
	SSP_PCAP_ADJ_BIT_ADC1_ADA21                 = 0x24000100,
	SSP_PCAP_ADJ_BIT_ADC1_ADA22                 = 0x24000200,
	SSP_PCAP_ADJ_BIT_ADC1_ATO0                  = 0x24000400,
	SSP_PCAP_ADJ_BIT_ADC1_ATO1                  = 0x24000800,
	SSP_PCAP_ADJ_BIT_ADC1_ATO2                  = 0x24001000,
	SSP_PCAP_ADJ_BIT_ADC1_ATO3                  = 0x24002000,
	SSP_PCAP_ADJ_BIT_ADC1_ATOX                  = 0x24004000,
	SSP_PCAP_ADJ_BIT_ADC1_MTR1                  = 0x24008000,
	SSP_PCAP_ADJ_BIT_ADC1_MTR2                  = 0x24010000,
	SSP_PCAP_ADJ_BIT_ADC1_TS_M0                 = 0x24020000,
	SSP_PCAP_ADJ_BIT_ADC1_TS_M1                 = 0x24040000,
	SSP_PCAP_ADJ_BIT_ADC1_TS_M2                 = 0x24080000,
	SSP_PCAP_ADJ_BIT_ADC1_LIADC                 = 0x24100000,
	SSP_PCAP_ADJ_BIT_ADC1_TS_REFENB             = 0x24200000,

	SSP_PCAP_ADJ_BIT_ADC2_ADD10                 = 0x28000001,
	SSP_PCAP_ADJ_BIT_ADC2_ADD11                 = 0x28000002,
	SSP_PCAP_ADJ_BIT_ADC2_ADD12                 = 0x28000004,
	SSP_PCAP_ADJ_BIT_ADC2_ADD13                 = 0x28000008,
	SSP_PCAP_ADJ_BIT_ADC2_ADD14                 = 0x28000010,
	SSP_PCAP_ADJ_BIT_ADC2_ADD15                 = 0x28000020,
	SSP_PCAP_ADJ_BIT_ADC2_ADD16                 = 0x28000040,
	SSP_PCAP_ADJ_BIT_ADC2_ADD17                 = 0x28000080,
	SSP_PCAP_ADJ_BIT_ADC2_ADD18                 = 0x28000100,
	SSP_PCAP_ADJ_BIT_ADC2_ADD19                 = 0x28000200,
	SSP_PCAP_ADJ_BIT_ADC2_ADD20                 = 0x28000400,
	SSP_PCAP_ADJ_BIT_ADC2_ADD21                 = 0x28000800,
	SSP_PCAP_ADJ_BIT_ADC2_ADD22                 = 0x28001000,
	SSP_PCAP_ADJ_BIT_ADC2_ADD23                 = 0x28002000,
	SSP_PCAP_ADJ_BIT_ADC2_ADD24                 = 0x28004000,
	SSP_PCAP_ADJ_BIT_ADC2_ADD25                 = 0x28008000,
	SSP_PCAP_ADJ_BIT_ADC2_ADD26                 = 0x28010000,
	SSP_PCAP_ADJ_BIT_ADC2_ADD27                 = 0x28020000,
	SSP_PCAP_ADJ_BIT_ADC2_ADD28                 = 0x28040000,
	SSP_PCAP_ADJ_BIT_ADC2_ADD29                 = 0x28080000,
	SSP_PCAP_ADJ_BIT_ADC2_ADINC1                = 0x28100000,
	SSP_PCAP_ADJ_BIT_ADC2_ADINC2                = 0x28200000,
	SSP_PCAP_ADJ_BIT_ADC2_ASC                   = 0x28400000,

	SSP_PCAP_ADJ_BIT_AUD_CODEC_AUDIHPF          = 0x2c000001,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_SMB              = 0x2c000002,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_AUDOHPF          = 0x2c000004,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_CD_TS            = 0x2c000008,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_DLM              = 0x2c000010,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_ADIT             = 0x2c000020,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_CDC_CLK0         = 0x2c000040,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_CDC_CLK1         = 0x2c000080,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_CDC_CLK2         = 0x2c000100,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_CLK_INV          = 0x2c000200,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_FS_INV           = 0x2c000400,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_DF_RESET         = 0x2c000800,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_CDC_EN           = 0x2c001000,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_CDC_CLK_EN       = 0x2c002000,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_FS_8K_16K        = 0x2c004000,
	SSP_PCAP_ADJ_BIT_AUD_CODEC_DIG_AUD_IN       = 0x2c008000,

	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_A1_EN          = 0x30000001,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_A2_EN          = 0x30000002,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_A4_EN          = 0x30000010,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_ARIGHT_EN      = 0x30000020,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_ALEFT_EN       = 0x30000040,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_CD_BYP         = 0x30000080,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_CDC_SW         = 0x30000100,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_ST_DAC_SW      = 0x30000200,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_PGA_IN_SW      = 0x30000400,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_PGA_R_EN       = 0x30000800,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_PGA_L_EN       = 0x30001000,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_AUDOG0         = 0x30002000,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_AUDOG1         = 0x30004000,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_AUDOG2         = 0x30008000,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_AUDOG3         = 0x30010000,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_A1CTRL         = 0x30020000,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_MONO0          = 0x30080000,
	SSP_PCAP_ADJ_BIT_AUD_RX_AMPS_MONO1          = 0x30100000,

	SSP_PCAP_ADJ_BIT_ST_DAC_SMB_ST_DAC          = 0x34000001,
	SSP_PCAP_ADJ_BIT_ST_DAC_STDET_EN            = 0x34000002,
	SSP_PCAP_ADJ_BIT_ST_DAC_ST_CLK0             = 0x34000004,
	SSP_PCAP_ADJ_BIT_ST_DAC_ST_CLK1             = 0x34000008,
	SSP_PCAP_ADJ_BIT_ST_DAC_ST_CLK2             = 0x34000010,
	SSP_PCAP_ADJ_BIT_ST_DAC_ST_CLK_EN           = 0x34000020,
	SSP_PCAP_ADJ_BIT_ST_DAC_DF_RESET_ST_DAC     = 0x34000040,
	SSP_PCAP_ADJ_BIT_ST_DAC_ST_DAC_EN           = 0x34000080,
	SSP_PCAP_ADJ_BIT_ST_DAC_SR0                 = 0x34000100,
	SSP_PCAP_ADJ_BIT_ST_DAC_SR1                 = 0x34000200,
	SSP_PCAP_ADJ_BIT_ST_DAC_SR2                 = 0x34000400,
	SSP_PCAP_ADJ_BIT_ST_DAC_SR3                 = 0x34000800,
	SSP_PCAP_ADJ_BIT_ST_DAC_DIG_AUD_IN_ST_DAC   = 0x34001000,
	SSP_PCAP_ADJ_BIT_ST_DAC_DIG_AUD_FS0         = 0x34002000,
	SSP_PCAP_ADJ_BIT_ST_DAC_DIG_AUD_FS1         = 0x34004000,
	SSP_PCAP_ADJ_BIT_ST_DAC_BCLK0               = 0x34008000,
	SSP_PCAP_ADJ_BIT_ST_DAC_BCLK1               = 0x34010000,
	SSP_PCAP_ADJ_BIT_ST_DAC_ST_CLK_INV          = 0x34020000,
	SSP_PCAP_ADJ_BIT_ST_DAC_ST_FS_INV           = 0x34040000,

	SSP_PCAP_ADJ_BIT_BUSCTRL_FSENB              = 0x50000001,
	SSP_PCAP_ADJ_BIT_BUSCTRL_USB_SUSPEND        = 0x50000002,
	SSP_PCAP_ADJ_BIT_BUSCTRL_USB_PU             = 0x50000004,
	SSP_PCAP_ADJ_BIT_BUSCTRL_USB_PD             = 0x50000008,
	SSP_PCAP_ADJ_BIT_BUSCTRL_VUSB_EN            = 0x50000010,
	SSP_PCAP_ADJ_BIT_BUSCTRL_USB_PS             = 0x50000020,
	SSP_PCAP_ADJ_BIT_BUSCTRL_VUSB_MSTR_EN       = 0x50000040,
	SSP_PCAP_ADJ_BIT_BUSCTRL_VBUS_PD_ENB        = 0x50000080,
	SSP_PCAP_ADJ_BIT_BUSCTRL_CURRLIM            = 0x50000100,
	SSP_PCAP_ADJ_BIT_BUSCTRL_RS232ENB           = 0x50000200,

	SSP_PCAP_ADJ_BIT_PERIPH_BL_CTRL0            = 0x54000001,
	SSP_PCAP_ADJ_BIT_PERIPH_BL_CTRL1            = 0x54000002,
	SSP_PCAP_ADJ_BIT_PERIPH_BL_CTRL2            = 0x54000004,
	SSP_PCAP_ADJ_BIT_PERIPH_BL2_CTRL0           = 0x54000008,
	SSP_PCAP_ADJ_BIT_PERIPH_BL2_CTRL1           = 0x54000010,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDR_EN             = 0x54000020,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDG_EN             = 0x54000040,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDR_CTRL0          = 0x54000080,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDR_CTRL1          = 0x54000100,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDR_CTRL2          = 0x54000200,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDR_CTRL3          = 0x54000400,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDG_CTRL0          = 0x54000800,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDG_CTRL1          = 0x54001000,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDG_CTRL2          = 0x54002000,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDG_CTRL3          = 0x54004000,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDR_I0             = 0x54008000,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDR_I1             = 0x54010000,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDG_I0             = 0x54020000,
	SSP_PCAP_ADJ_BIT_PERIPH_LEDG_I1             = 0x54040000,
	SSP_PCAP_ADJ_BIT_PERIPH_SKIP                = 0x54080000,
	SSP_PCAP_ADJ_BIT_PERIPH_BL2_EN              = 0x54100000,

	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_AUDIG0         = 0x68000001,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_AUDIG1         = 0x68000002,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_AUDIG2         = 0x68000004,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_AUDIG3         = 0x68000008,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_AUDIG4         = 0x68000010,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_A3_EN          = 0x68000020,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_A3_MUX         = 0x68000040,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_A5_EN          = 0x68000080,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_A5_MUX         = 0x68000100,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_EXT_MIC_MUX    = 0x68000200,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_MB_ON2         = 0x68000400,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_MB_ON1         = 0x68000800,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_AHS_CONFIG     = 0x68004000,
	SSP_PCAP_ADJ_BIT_TX_AUD_AMPS_V2_EN_2        = 0x68200000,

	SSP_PCAP_ADJ_BIT_SYS_RST_CLR                = 0x6c000001,
	SSP_PCAP_ADJ_BIT_SYS_RST_MODE0              = 0x6c000002,
	SSP_PCAP_ADJ_BIT_SYS_RST_MODE1              = 0x6c000004
}SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE;

#ifdef E680_P3_AND_EARLY

/* control PCAP direct PIN */
#define GPIO_WDI_AP            4       /* WDI_AP                       */
#define GPIO_SYS_RESTART       55      /* restart PCAP power           */
#define GPIO_AP_STANDBY        28      /* make pcap enter standby mode */ 

/* communicate with PCAP's PIN  */
#define GPIO_PCAP_SEC_INT      1       /* PCAP interrupt PIN to AP     */ 
#define GPIO_SPI_CLK           29      /* PCAP SPI port clock          */ 
#define GPIO_SPI_CE            24      /* PCAP SPI port SSPFRM         */  
#define GPIO_SPI_MOSI          25      /* PCAP SPI port SSPTXD         */ 
#define GPIO_SPI_MISO          26      /* PCAP SPI port SSPRXD         */ 

#elif defined(A780_P1_AND_EARLY)

/* control PCAP direct PIN */
#define GPIO_WDI_AP            4       /* WDI_AP                       */
#define GPIO_SYS_RESTART       55      /* restart PCAP power           */
#define GPIO_AP_STANDBY        28      /* make pcap enter standby mode */ 

/* communicate with PCAP's PIN  */
#define GPIO_PCAP_SEC_INT      1       /* PCAP interrupt PIN to AP     */ 
#define GPIO_SPI_CLK           29      /* PCAP SPI port clock          */ 
#define GPIO_SPI_CE            24      /* PCAP SPI port SSPFRM         */  
#define GPIO_SPI_MOSI          25      /* PCAP SPI port SSPTXD         */ 
#define GPIO_SPI_MISO          26      /* PCAP SPI port SSPRXD         */ 

#else

/* control PCAP direct PIN */
#define GPIO_WDI_AP            4       /* WDI_AP                       */
#define GPIO_SYS_RESTART       55      /* restart PCAP power           */
//#define GPIO_AP_STANDBY        28      /* make pcap enter standby mode */ 

/* communicate with PCAP's PIN  */
#define GPIO_PCAP_SEC_INT      1       /* PCAP interrupt PIN to AP     */ 
#define GPIO_SPI_CLK           29      /* PCAP SPI port clock          */ 
#define GPIO_SPI_CE            24      /* PCAP SPI port SSPFRM         */  
#define GPIO_SPI_MOSI          25      /* PCAP SPI port SSPTXD         */ 
#define GPIO_SPI_MISO          26      /* PCAP SPI port SSPRXD         */ 


#endif

/************************ FUNCTION PROTOTYPES **************************************/
extern void set_GPIO_mode(u32 gpio_mode);

extern void ssp_pcap_init(void);
extern void ssp_pcap_release(void);

extern void ssp_pcap_open(SSP_PCAP_INIT_DRIVER_TYPE portType);
extern void ssp_pcap_close(void);

extern void ssp_pcap_intoSleep_callBack(void);
extern void ssp_pcap_wakeUp_callBack(void);
extern void pcap_switch_off_usb(void);
extern void pcap_switch_on_usb(void);

extern SSP_PCAP_STATUS SSP_PCAP_write_data_to_PCAP(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER ssp_pcap_register,u32 ssp_pcap_register_value);
extern SSP_PCAP_STATUS SSP_PCAP_read_data_from_PCAP(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER ssp_pcap_register,u32* p_ssp_pcap_register_value);

extern SSP_PCAP_STATUS SSP_PCAP_bit_set(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE ssp_pcap_bit ) ;
extern SSP_PCAP_STATUS SSP_PCAP_bit_clean(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE ssp_pcap_bit ) ;
extern SSP_PCAP_BIT_STATUS SSP_PCAP_get_bit_from_buffer(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE ssp_pcap_bit ) ;
extern SSP_PCAP_BIT_STATUS SSP_PCAP_get_bit_from_PCAP(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER_BIT_TYPE ssp_pcap_bit ) ;
extern u32 SSP_PCAP_get_register_value_from_buffer(SSP_PCAP_SECONDARY_PROCESSOR_REGISTER ssp_pcap_register ) ;

extern SSP_PCAP_STATUS SSP_PCAP_TSI_mode_set(TOUCH_SCREEN_DETECT_TYPE mode_Type );
extern SSP_PCAP_STATUS SSP_PCAP_TSI_start_XY_read(void);
extern SSP_PCAP_STATUS SSP_PCAP_TSI_get_XY_value(u32* p_x,u32* p_y);
extern SSP_PCAP_STATUS SSP_PCAP_CDC_CLK_set(PHONE_CDC_CLOCK_TYPE clkType);

extern SSP_PCAP_STATUS SSP_PCAP_CDC_SR_set(ST_SAMPLE_RATE_TYPE srType);
extern SSP_PCAP_STATUS SSP_PCAP_BCLK_set(ST_BCLK_TIME_SLOT_TYPE bclkType);
extern SSP_PCAP_STATUS SSP_PCAP_STCLK_set(ST_CLK_TYPE stClkType);
extern SSP_PCAP_STATUS SSP_PCAP_DIG_AUD_FS_set(DIG_AUD_MODE_TYPE fsType);
extern SSP_PCAP_STATUS SSP_PCAP_AUDIG_set(u32 audioInGain);
extern SSP_PCAP_STATUS SSP_PCAP_MONO_set(MONO_TYPE monoType);
extern SSP_PCAP_STATUS SSP_PCAP_AUDOG_set(u32 audioOutGain);

extern SSP_PCAP_STATUS SSP_PCAP_V_VIB_level_set(VibratorVoltageLevel_TYPE VIBLevelType);
extern SSP_PCAP_STATUS SSP_PCAP_configure_USB_UART_transeiver(SSP_PCAP_PORT_TYPE portType);

#ifdef __cplusplus
}
#endif

#endif	

#ifndef BLOB_BL_HEADER_H
#define BLOB_BL_HEADER_H
/*
 * bl_flash_header.h
 *
 * The bootloader flash header and code group info
 * 
 * Copyright (C) 2002-2005 Motorola
 * 
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * 
 * Oct 30, 2002 - (Motorola) Created
 * 
 */
/*================================================================================
INCLUDE FILES
==============================================================================*/
#include "types.h"

/*==============================================================================
CONSTANTS
==============================================================================*/
/* This value is used to determine if a valid flash bootloader exists. */
   
#define FLASH_BL_BARKER_VALUE        177
#define PU_MAIN_CODE_BARKER_VALUE    177

#define FLASH_BASE_ADDR                 0x00000000

/*==============================================================================
GLOBAL MACROS DECLARATION
==============================================================================*/


/*==============================================================================
ENUMS
==============================================================================*/

typedef enum
{
   BLOADER_BOOT,
   //BLOADER_MAIN,        /* Code group 0 and Code group 1 */
   BLOADER_CODEGROUP0,
   BLOADER_CODEGROUP1,
   BLOADER_CODEGROUP2,  /* reserved for FDI  */
   BLOADER_CODEGROUP3,  /* DSP Patches       */
   BLOADER_CODEGROUP4,  /* Language Package  */
   BLOADER_CODEGROUP5,  /* eg. Resource Pack */
   BLOADER_CODEGROUP6,  /* eg. Resource Pack */
   BLOADER_CODEGROUP7,  /* eg. Resource Pack */
   BLOADER_CODEGROUP8,  /* eg. Resource Pack */
   BLOADER_CODEGROUP9,  /* eg. Resource Pack */
   BLOADER_CODEGROUP10, /* eg. Resource Pack */
   BLOADER_CODEGROUP11, /* eg. Resource Pack */
   BLOADER_CODEGROUP12, /* eg. Resource Pack */
   BLOADER_CODEGROUP13, /* eg. Resource Pack */
   BLOADER_CODEGROUP14, /* eg. Resource Pack */
   BLOADER_CODEGROUP15, /* eg. Resource Pack */
   BLOADER_CODEGROUP16, /* eg. Resource Pack */
   BLOADER_CODEGROUP17, /* eg. Resource Pack */
   BLOADER_CODEGROUP18, /* eg. Resource Pack */
   BLOADER_CODEGROUP19, /* eg. Resource Pack */
   BLOADER_CODEGROUP20, /* eg. Resource Pack */
   /*
    * NOTE: Add logical sectors above here so count remains accurate.
    *       (reserved will always follow at the end of the list so that
    *        it is a "non-value")
    */
   BLOADER_NUM_SECTIONS,
   BLOADER_RESERVED
}BLOADER_SECTIONS;


/*==============================================================================
STRUCTURES AND OTHER TYPEDEFS
==============================================================================*/

/* define the location for all values in the bootloader header */
typedef struct _FLASH_HEADER 
{
    u32 flash_bl_start_pc_value;

    u32 flash_bl_barker_value;
    u8  flash_bl_sw_descriptor_descriptor_type;
    u8  flash_bl_sw_descriptor_hw_chipset_type; 
    u8  flash_bl_sw_descriptor_ma_type;
    u8  flash_bl_sw_descriptor_code_type;
    u8  flash_bl_sw_descriptor_growth;
    u8  flash_bl_sw_descriptor_product_sub_type;
    u16 flash_bl_sw_descriptor_version_number;
    u8  flash_bl_hardware_descriptor_descriptor_type;
    u8  flash_bl_hardware_descriptor_hw_chipset_type;
    u8  flash_bl_hardware_descriptor_ma_type;
    u8  flash_bl_hardware_descriptor_growth0;
    u8  flash_bl_hardware_descriptor_growth1;
    u8  flash_bl_hardware_descriptor_product_sub_type; 
    u16 flash_bl_hardware_descriptor_version_number;
    /* Just for compatible with SIERRA boot protocol */
    u32 flash_bl_hardware_test_command_entrance_pc_value;

    u32 flash_bl_checksum_start_address;
    u32 flash_bl_checksum_end_address;

    u32 flash_bl_begin_address_of_code_group_0;

} TFLASH_HEADER, *PFLASH_HEADER;

/* define the structure where all the values will be placed in memory */
typedef struct _CODE_GROUP_0 
{
   u32 pu_start_pc_value;
   u32 pu_main_code_barker_value; 
   u8  code_group_0_sw_descriptor_descriptor_type;
   u8  code_group_0_sw_descriptor_hw_chipset_type;
   u8  code_group_0_sw_descriptor_ma_type;
   u8  code_group_0_sw_descriptor_code_type;
   u8  code_group_0_sw_descriptor_growth;
   u8  code_group_0_sw_descriptor_product_sub_type;
   u16 code_group_0_sw_descriptor_version_number;
   u8  phone_code_sw_descriptor_descriptor_type;
   u8  phone_code_sw_descriptor_hw_chipset_type;
   u8  phone_code_sw_descriptor_ma_type;
   u8  phone_code_sw_descriptor_growth0;
   u8  phone_code_sw_descriptor_growth1;
   u8  phone_code_sw_descriptor_product_sub_type; 
   u16 phone_code_sw_descriptor_version_number;
   u8  code_group_number_of_code_groups;
   u8  pad[3];
   u32 code_group_0_start_address;
   u32 code_group_0_end_address;
   u32 code_group_1_start_address;
   u32 code_group_1_end_address;
   u32 code_group_2_start_address;
   u32 code_group_2_end_address;
   u32 code_group_3_start_address;
   u32 code_group_3_end_address;
   u32 code_group_4_start_address;
   u32 code_group_4_end_address;
   u32 code_group_5_start_address;
   u32 code_group_5_end_address;
   u32 code_group_6_start_address;
   u32 code_group_6_end_address;
   u32 code_group_7_start_address;
   u32 code_group_7_end_address;
   u32 code_group_8_start_address;
   u32 code_group_8_end_address;
   u32 code_group_9_start_address;
   u32 code_group_9_end_address;
   u32 code_group_10_start_address;
   u32 code_group_10_end_address;
   u32 code_group_11_start_address;
   u32 code_group_11_end_address;
   u32 code_group_12_start_address;
   u32 code_group_12_end_address;
   u32 code_group_13_start_address;
   u32 code_group_13_end_address;
   u32 code_group_14_start_address;
   u32 code_group_14_end_address;
   u32 code_group_15_start_address;
   u32 code_group_15_end_address;
   u32 code_group_16_start_address;
   u32 code_group_16_end_address;
   u32 code_group_17_start_address;
   u32 code_group_17_end_address;
   u32 code_group_18_start_address;
   u32 code_group_18_end_address;
   u32 code_group_19_start_address;
   u32 code_group_19_end_address;
   u32 code_group_20_start_address;
   u32 code_group_20_end_address;

} TCODE_GROUP_0, *PCODE_GROUP_0;

typedef struct
{
   u32   start_addr;
   u32   end_addr;
}BLOADER_SECTION_ADDR_TBL;

/*==============================================================================
UNION DECLARATION
==============================================================================*/


/*==============================================================================
GLOBAL VARIABLE DECLARATIONS
==============================================================================*/



/*==============================================================================
GLOBAL POINTER DECLARATIONS
==============================================================================*/



/*==============================================================================
GLOBAL FUNCTION PROTOTYPES
==============================================================================*/


/*============================================================================*/
#endif  /* BLOB_BL_HEADER_H */



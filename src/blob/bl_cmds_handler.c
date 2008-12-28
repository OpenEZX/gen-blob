/*
 * bl_cmds_handler.c
 *
 * Command Handler for boot commands
 * 
 * In BLOB, we only support these commands:
 *         ADDR        command
 *         BIN         command
 *         JUMP        command
 *         POWER_DOWN  command
 *         RQRC        query
 *         RQVN        query
 *         RQHW        query
 *         RQCS        query
 * 
 * Copyright (C) 2002-2005 Motorola
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
 * Oct 30,2002 - (Motorola) Created
 * 
 */
 /*================================================================================
INCLUDE FILES
==============================================================================*/
#include "bl_cmds_handler.h"
#include "bl_parser.h"
#include "bl_utility.h"
#include "bl_flash_header.h"
#include "types.h"

#ifdef ZQ_DEBUG
#include "bl_debug.h"
#endif /* ZQ_DEBUG */
//#include "bt_uart.h"

/*==============================================================================
LOCAL CONSTANTS
==============================================================================*/
#define ZQ_DEBUG2
#ifdef ZQ_DEBUG2
u8 debug_bootloader_code[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x02, 0x01, 0xff, 0x00, 0x00, 0x01,
	/* the following is the bootloader code */
	0x0c, 0x00, 0x02, 0x05, 0x04, 0x07, 0x0e, 0xcc,
	0x00, 0xf0, 0x30, 0x00, 0x08, 0x0c, 0x0d, 0xbb,
	0xc0, 0x67, 0x34, 0x11, 0x10, 0x20, 0xd0, 0xaa,
	0xcd, 0x55, 0x02, 0x04, 0xa0, 0x04, 0x0e, 0x0a,
	0xd0, 0x44, 0x22, 0x03, 0xd0, 0x03, 0xee, 0xff,
	0xdc, 0x40, 0x23, 0x30, 0x0e, 0x30, 0x0f, 0xff
};
u8 debug_code_group1[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x02, 0x01, 0xff, 0x00, 0x00, 0x01,
	/* the following is the code group 1 code */
	0x0c, 0x00, 0x02, 0x05, 0x04, 0x07, 0x0e, 0xcc,
	0x00, 0xf0, 0x30, 0x00, 0x08, 0x0c, 0x0d, 0xbb,
	0xc0, 0x67, 0x34, 0x11, 0x10, 0x20, 0xd0, 0xaa,
	0xcd, 0x55, 0x02, 0x04, 0xa0, 0x04, 0x0e, 0x0a,
	0xd0, 0x44, 0x22, 0x03, 0xd0, 0x03, 0xee, 0xff,
	0xdc, 0x40, 0x23, 0x30, 0x0e, 0x30, 0x0f, 0xff
};
u8 debug_code_group2[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x02, 0x01, 0xff, 0x00, 0x00, 0x01,
	/* the following is the code group 1 code */
	0x0c, 0x00, 0x02, 0x05, 0x04, 0x07, 0x0e, 0xcc,
	0x00, 0xf0, 0x30, 0x00, 0x08, 0x0c, 0x0d, 0xbb,
	0xc0, 0x67, 0x34, 0x11, 0x10, 0x20, 0xd0, 0xaa,
	0xcd, 0x55, 0x02, 0x04, 0xa0, 0x04, 0x0e, 0x0a,
	0xd0, 0x44, 0x22, 0x03, 0xd0, 0x03, 0xee, 0xff,
	0xdc, 0x40, 0x23, 0x30, 0x0e, 0x30, 0x0f, 0xff
};
u8 debug_code_group3[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x02, 0x01, 0xff, 0x00, 0x00, 0x01,
	/* the following is the code group 1 code */
	0x0c, 0x00, 0x02, 0x05, 0x04, 0x07, 0x0e, 0xcc,
	0x00, 0xf0, 0x30, 0x00, 0x08, 0x0c, 0x0d, 0xbb,
	0xc0, 0x67, 0x34, 0x11, 0x10, 0x20, 0xd0, 0xaa,
	0xcd, 0x55, 0x02, 0x04, 0xa0, 0x04, 0x0e, 0x0a,
	0xd0, 0x44, 0x22, 0x03, 0xd0, 0x03, 0xee, 0xff,
	0xdc, 0x40, 0x23, 0x30, 0x0e, 0x30, 0x0f, 0xff
};
u8 debug_code_group6[] = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x10, 0x02, 0x01, 0xff, 0x00, 0x00, 0x01,
	/* the following is the code group 1 code */
	0x0c, 0x00, 0x02, 0x05, 0x04, 0x07, 0x0e, 0xcc,
	0x00, 0xf0, 0x30, 0x00, 0x08, 0x0c, 0x0d, 0xbb,
	0xc0, 0x67, 0x34, 0x11, 0x10, 0x20, 0xd0, 0xaa,
	0xcd, 0x55, 0x02, 0x04, 0xa0, 0x04, 0x0e, 0x0a,
	0xd0, 0x44, 0x22, 0x03, 0xd0, 0x03, 0xee, 0xff,
	0xdc, 0x40, 0x23, 0x30, 0x0e, 0x30, 0x0f, 0xff
};

/*extern TCODE_GROUP_0 debug_code_group0;*/

TCODE_GROUP_0 debug_code_group0 = {
	0,
	177,
	0,
	0x02,
	0x02,
	0x06,
	0xFF,
	0x00,
	0x01,
	0x00,
	0x02,
	0x02,
	0xFF,
	0xFF,
	0x00,
	0x01,
	5,
	{0, 0, 0}
	,
	(u32) & debug_code_group0,
	(u32) & debug_code_group0 + sizeof(TCODE_GROUP_0),
	(u32) & debug_code_group1,
	(u32) & debug_code_group1 + sizeof(debug_code_group1),
	(u32) & debug_code_group2,
	(u32) & debug_code_group2 + sizeof(debug_code_group2),
	(u32) & debug_code_group3,
	(u32) & debug_code_group3 + sizeof(debug_code_group3),
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	0xFFFFFFFF,
	(u32) & debug_code_group6,
	(u32) & debug_code_group6 + sizeof(debug_code_group6),
	0,
	0
};

TFLASH_HEADER debug_flash_header = {
	0,
	177,
	0,
	0x0D,
	0x02,
	0x01,
	0xFF,
	0x00,
	0x01,
	0x00,
	0x0D,
	0x02,
	0x01,
	0xFF,
	0x00,
	0x01,
	0x00,
	(u32) & debug_bootloader_code,
	(u32) & debug_bootloader_code + sizeof(debug_bootloader_code),
	(u32) & debug_code_group0
};

#define BOOTLOADER_HEADER_ADDR      &debug_flash_header
#else
#define BOOTLOADER_HEADER_ADDR      0x00000000	/*??? TBD */
#endif /* ZQ_DEBUG */

/*==============================================================================
LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==============================================================================*/
typedef enum {
	BLOADER_BOOT_CSUM = 0,
	BLOADER_CODE_GROUP0_CSUM,
	BLOADER_CODE_GROUP1_CSUM,
	BLOADER_CODE_GROUP2_CSUM,
	BLOADER_CODE_GROUP3_CSUM,
	BLOADER_CODE_GROUP4_CSUM,
	BLOADER_CODE_GROUP5_CSUM,
	BLOADER_CODE_GROUP6_CSUM,
	BLOADER_CODE_GROUP7_CSUM,
	BLOADER_CODE_GROUP8_CSUM,
	BLOADER_CODE_GROUP9_CSUM,
	BLOADER_CODE_GROUP10_CSUM,
	BLOADER_CODE_GROUP11_CSUM,
	BLOADER_CODE_GROUP12_CSUM,
	BLOADER_CODE_GROUP13_CSUM,
	BLOADER_CODE_GROUP14_CSUM,
	BLOADER_CODE_GROUP15_CSUM,
	BLOADER_CODE_GROUP16_CSUM,
	BLOADER_CODE_GROUP17_CSUM,
	BLOADER_CODE_GROUP18_CSUM,
	BLOADER_CODE_GROUP19_CSUM,
	BLOADER_CODE_GROUP20_CSUM,
	BLOADER_TOTAL_CSUM
} BLOADER_CHECKSUMS;

/*==============================================================================
LOCAL MACROS
==============================================================================*/
#define ADD_INFO_DATA_SIZE 240
/* #define ADD_INFO_SOURCE_PTR 0xa020ef00 */
/* #define ADD_INFO_DEST_PTR 0xa0300000 */
/* index in header for where code groups addresses start */
#define CODE_GROUP_START_ADDR_INDEX 7

#define MAX_CODE_GROUP_SIZE (BLOADER_SECTIONS)BLOADER_NUM_SECTIONS

#define UNDEFINED_ADDRESS          0xFFFFFFFF
#define UNDEFINED_VALUE            0xFFFFFFFF

/* ASCII space character */
#define CHAR_SPACE   0x20

/* Response buffer size: Maximal size is (3 + MAX_CODE_GROUP_SIZE) Fields. 10 bytes margin. */
#define MAX_RESPONSE_SIZE          ((3+MAX_CODE_GROUP_SIZE)*17 + 10)

#define RESPONSE_FIELD_SEPARATOR   ','
/* From start of Flash Bootloader, Ram Downloader header.*/
#define SW_GROUP_DESRIPTOR_OFFSET  0x08
#define SW_DESCRIPTOR_SIZE         0x08
#define SW_PACKAGE_DESCRIPTOR_SIZE 0x08
#define UNDEFINED_BYTE             0xFF
#define HW_DESCR_BLOCK_LENGTH      0x08	/* No. of Bytes in Flash. */
#define RQVN_RESPONSE_KEYWORD_STRING "RSVN"
#define RQCS_RESPONSE_KEYWORD_STRING "RSCS"
#define RQHW_RESPONSE_KEYWORD_STRING "RSHW"
#define RQRC_RESPONSE_KEYWORD_STRING "RSRC"
#define BIN_DATA_SIZE_FIELD_LENGTH 0x02

/* Additional definition per FAGAN Code Inspection */
#define ASCII_HEX_CHECKSUM_LENGTH  4
#define RAM_CHECKSUM_RESPONSE_SIZE 20
#define MIN_RAM_SIZE               1024
#define EVEN_NUMBER                2
#define MIN_NUMB_BYTES             8

#define RAM_START                   0xa0000000
#define RAM_END                     0xa1000000

/*==============================================================================
LOCAL FUNCTION PROTOTYPES
==============================================================================*/

static BOOLEAN parser_valid_address(u32 addr);
static u16 util_calc_csum(BLOADER_SECTIONS section,
			  u8 * size, BOOLEAN * valid_chksm);
static BOOLEAN flash_bootloader_barker_is_valid(void);
static u16 add_bin_ascii_response(const u8 * source_ptr,
				  u8 * response_ptr, u8 numb_bytes);
static u16 get_running_boot_info(u8 * response_ptr);

static BOOLEAN main_code_barker_is_valid(void);
static BOOLEAN just_flashed_main_code_barker_is_valid(void);

/*==============================================================================
LOCAL VARIABLES
==============================================================================*/

u16 *blvar_received_address_ptr;

/*==============================================================================
GLOBAL VARIABLES
==============================================================================*/

/*==============================================================================
LOCAL POINTER DECLARATIONS
==============================================================================*/

/*==============================================================================
LOCAL FUNCTIONS
==============================================================================*/
/*==============================================================================

FUNCTION: parser_valid_address

DESCRIPTION: Validates a received target flash address by checking that
 it is an area of Flash/RAM memory that has been previously erased.

ARGUMENTS PASSED:
   addr - address value to check

REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   TRUE - address is valid
   FALSE - address is invalid

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/

static BOOL parser_valid_address(u32 addr)
{
	BOOLEAN addr_ok = FALSE;

	if ((addr >= (u32) RAM_START) && (addr <= (u32) RAM_END)) {
		addr_ok = TRUE;
	}

	return (addr_ok);
}

/*==============================================================================

FUNCTION: flash_bootloader_barker_is_valid

DESCRIPTION: 

   Utility to check for valid Flash Bootloader Barker. 

ARGUMENTS PASSED:
   None
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   TRUE  if Flash Barker is found to be valid.
   FALSE otherwise.

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/
static BOOL flash_bootloader_barker_is_valid(void)
{

	PFLASH_HEADER flash_bl_header_ptr;
	BOOLEAN bValidFlashBarker;

	flash_bl_header_ptr = (PFLASH_HEADER) BOOTLOADER_HEADER_ADDR;
	bValidFlashBarker =
	    (flash_bl_header_ptr->flash_bl_barker_value ==
	     FLASH_BL_BARKER_VALUE)
	    ? TRUE : FALSE;

	return (bValidFlashBarker);

}

/*==============================================================================
FUNCTION: add_bin_ascii_response

DESCRIPTION: 

   Utility to convert 'n' binary bytes to ASCII and write them to a response 
   character array. 2*n ASCII bytes are added starting at response_ptr. The 
   response is NULL terminated. 
 
ARGUMENTS PASSED:
   source_ptr    - Pointer to binary input data. If source_ptr is NULL, the 
                   entire response field is filled with "FF"'s.
   response_ptr  - Pointer to ASCII output character array.
   numb_bytes    - Number of binary bytes to convert.
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   Number of ASCII bytes added to response, not counting the NULL terminator.

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/
static u16
add_bin_ascii_response(const u8 * source_ptr, u8 * response_ptr, u8 numb_bytes)
{

	u8 i;
	u8 response_byte_count;

	response_byte_count = 0;

	if (source_ptr) {
		for (i = 0; i < numb_bytes; i++) {
			util_u8_to_hexasc(*source_ptr,
					  &response_ptr[response_byte_count]);
			source_ptr++;	/* Point to next byte to convert. */
			response_byte_count += 2;	/* Point to next ASCII HEX field. */
		}
	} else {
		for (i = 0; i < numb_bytes; i++) {
			util_u8_to_hexasc(UNDEFINED_BYTE,
					  &response_ptr[response_byte_count]);
			response_byte_count += 2;	/* Point to next ASCII HEX field. */
		}
	}

	/* Terminate response. */
	response_ptr[response_byte_count] = NULL;

	return response_byte_count;
}

/*==============================================================================
FUNCTION: get_running_boot_info

DESCRIPTION: 

   Utility to read the Software descriptor of the running boot: 
        Flash Bootloader 
              or 
        Ram Downloader for ROM 
              or
        Ram Downloader for Flash.
                
   16 Bytes of ASCII HEX data is written starting at response_ptr after which 
   the response is NULL terminated. For Flash Bootloader, 'FF's are returned if 
   the Barker value is invalid.
   
   
ARGUMENTS PASSED:
   response_ptr - Pointer to response data frame.
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   Number of ASCII bytes added to response, not including the NULL terminator.

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None
==============================================================================*/
static u16 get_running_boot_info(u8 * response_ptr)
{

	u16 response_byte_count;
	const u8 *source_ptr;

	source_ptr = NULL;
	if (flash_bootloader_barker_is_valid()) {
		source_ptr =
		    (u8 *) BOOTLOADER_HEADER_ADDR + SW_GROUP_DESRIPTOR_OFFSET;
	}

	response_byte_count =
	    add_bin_ascii_response(source_ptr, &response_ptr[0],
				   SW_DESCRIPTOR_SIZE);

	return response_byte_count;

}

/*==============================================================================
FUNCTION: main_code_barker_is_valid

DESCRIPTION: Local utility to check if there is a valid phone code barker

ARGUMENTS PASSED:
   None

REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   BOOLEAN - TRUE   -  valid phone code barker
             FALSE  -  invalid phone code barker

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/
static BOOL main_code_barker_is_valid(void)
{
	PCODE_GROUP_0 cg0_ptr;

	PFLASH_HEADER pFlashHeader = (PFLASH_HEADER) BOOTLOADER_HEADER_ADDR;
	/* assign pointer to address of CG0 */
	cg0_ptr = (PCODE_GROUP_0) * ((u32 *)
				     (&pFlashHeader->
				      flash_bl_begin_address_of_code_group_0));

	/* if we just flashed,i.e., the phone code is just stored in the flash 
	 * but the barker value is not stored in the memory yet, OR 
	 * there is phone code and a valid phone barker then OK 
	 */
	if (cg0_ptr->pu_main_code_barker_value == PU_MAIN_CODE_BARKER_VALUE) {
		return TRUE;
	}

	return FALSE;
}

/*==============================================================================
FUNCTION: just_flashed_main_code_barker_is_valid

DESCRIPTION: Local utility to check if there is a valid phone code barker

ARGUMENTS PASSED:
   None

REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   BOOLEAN - TRUE   -  valid phone code barker OR phone just has been flashed 
                       but phone code barker has not been updated yet.
             FALSE  -  invalid phone code barker

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/
static BOOL just_flashed_main_code_barker_is_valid(void)
{
	PCODE_GROUP_0 cg0_ptr;

	PFLASH_HEADER pFlashHeader = (PFLASH_HEADER) BOOTLOADER_HEADER_ADDR;
	/* assign pointer to address of CG0 */
	cg0_ptr = (PCODE_GROUP_0) * ((u32 *)
				     (&pFlashHeader->
				      flash_bl_begin_address_of_code_group_0));

	/* if we just flashed,i.e., the phone code is just stored in the flash 
	 * but the barker value is not stored in the memory yet, OR 
	 * there is phone code and a valid phone barker then OK 
	 */
	if (((cg0_ptr->pu_main_code_barker_value == UNDEFINED_VALUE) &&
	     (cg0_ptr->code_group_0_start_address != UNDEFINED_ADDRESS)) ||
	    (cg0_ptr->pu_main_code_barker_value == PU_MAIN_CODE_BARKER_VALUE)) {
		return TRUE;
	}

	return FALSE;
}

/*==============================================================================

FUNCTION: util_calc_csum

DESCRIPTION: Local utility to calculate a standard memory checksum (16-bit csum, 8-bit memory scan)

ARGUMENTS PASSED:
   section - logical section to calculate the checksum on
   
REFERENCE ARGUMENTS PASSED:
   *number_of_codegroup - hold the number of available code group in the memory 
   *valid_chksm - valid checksum flag

RETURN VALUE:
   u16 - calculated checksum

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/
static u16
util_calc_csum(BLOADER_SECTIONS section, u8 * number_of_codegroup,
	       BOOLEAN * valid_check_sum)
{
	u8 *section_start_ptr, *section_end_ptr;
	u16 checksum = 0;	/* Calculated check sum */
	/* used to determine whether the program is existed */
	u32 *section_start_value;
	/* used to determine whether the program is existed */
	u32 *section_end_value;
	u32 i;			/* checksum loop index */
	u32 number_of_bytes_to_sum;	/* counter for something */
	/* point to the Flash loader section in the Flash memory */
	TFLASH_HEADER *fl_loader_ptr;
	/* point to the Code group 0 in the Flash memory */
	TCODE_GROUP_0 *code_group0_ptr;

	u32 offset;		/* Offset to the next starting address of the next code group */

	fl_loader_ptr = (TFLASH_HEADER *) (BOOTLOADER_HEADER_ADDR);

	/* Check for the right section to point to the proper Flash location */
	if (section == BLOADER_BOOT_CSUM) {
		/* Check for existing Flash loader code */
		/* if we just erased and flashed, or there is already a good barker,OK */
		if (((fl_loader_ptr->flash_bl_barker_value == UNDEFINED_ADDRESS)
		     && (fl_loader_ptr->flash_bl_checksum_start_address !=
			 UNDEFINED_ADDRESS))
		    || (fl_loader_ptr->flash_bl_barker_value ==
			FLASH_BL_BARKER_VALUE)) {
			/* point to the start and end address value */
			section_start_ptr =
			    (u8 *) fl_loader_ptr->
			    flash_bl_checksum_start_address;
			section_end_ptr =
			    (u8 *) fl_loader_ptr->flash_bl_checksum_end_address;
		} else {
			return (checksum);	/* No Flash loader code, return 0 */
		}
	} else {
		/* Point to the beginning of code group 0 if available */
		code_group0_ptr =
		    (TCODE_GROUP_0 *) fl_loader_ptr->
		    flash_bl_begin_address_of_code_group_0;

		/* Get the code group counter from code group 0 */
		if (section == BLOADER_CODE_GROUP0_CSUM) {
			/* if there is a valid phone code barker */
			if (just_flashed_main_code_barker_is_valid()) {
				/* First get the number of code group */
				*number_of_codegroup =
				    code_group0_ptr->
				    code_group_number_of_code_groups;
			} else {
				/* invalid barker code,i.e., bad phone code, stop here */
				*number_of_codegroup = 0;

				/* the checksum is invalid or undefined for this code group */
				*valid_check_sum = FALSE;

				return (checksum);	/* No phone code, return 0 */
			}
		}
		/* Calculate the index into the next code group */
		/* index into the next code group */
		offset = CODE_GROUP_START_ADDR_INDEX + (2 * (section - 1));
		/* point to the start and end address value */
		section_start_value = (u32 *) code_group0_ptr;
		section_start_value += offset;
		section_start_ptr = (u8 *) * section_start_value;
		/* index to the end address */
		section_end_value = section_start_value + 1;
		section_end_ptr = (u8 *) * section_end_value;

		if (((*section_start_value == UNDEFINED_ADDRESS)
		     && (*section_end_value == UNDEFINED_ADDRESS)) ||
		    ((*section_end_value - *section_start_value) == 0)) {
			/* the checksum is invalid or undefined for this code group */
			*valid_check_sum = FALSE;
			return (checksum);
		} else {
			(*number_of_codegroup)--;	/* decrement for each existed CG */
		}
	}

	number_of_bytes_to_sum =
	    ((u32) section_end_ptr - (u32) section_start_ptr + 1);

	for (i = 0; i < number_of_bytes_to_sum; i++) {
		/* sum up this division of the code group */
		checksum += *section_start_ptr++;
	}

	return (checksum);
}

/*==============================================================================
GLOBAL FUNCTIONS
==============================================================================*/

void handle_command_RBIN(u8 *data_ptr)
{
	u16 size;
	u32 addr;
	u8 rx_csum = 0;
	u8 calc_csum = 0;
	u8 response[8192];
	u16 i;			/* address bytes counter */

	if (data_ptr == NULL) {
		parse_err_response(BLOADER_ERR_DATA_INVALID);
		return;
	}

	/* Calculate the checksum based on received data */
	for (i = 0; i < 12; i++) {
		calc_csum += data_ptr[i];
	}

	/* Converted the received address from ASCII string to number */
	addr = util_hexasc_to_u32(&data_ptr[0], 8);
	size = util_hexasc_to_u32(&data_ptr[8], 4);

	/* Converted the received checksum from ASCII string to number */
	rx_csum = (u8) util_hexasc_to_u32(&data_ptr[12], 2);

	/* Validate the checksum */
	if (rx_csum != calc_csum) {
		/* Bad checksum, return error */
		parse_err_response(BLOADER_ERR_BAD_CHECKSUM);
	} else {
		calc_csum = 0;
		for (i = 0; i < size; i++) {
	                response[i] = ((u8 *)addr)[i];
			calc_csum += response[i];
	        }
		response[i] = calc_csum;
		parse_send_packet("RBIN", (u8 *) response, size + 1);
	}
}

/*==============================================================================

FUNCTION: handle_command_ADDR

DESCRIPTION: Handler for the ADDRess command.

  The ADDR command is used to validate that the sent address is in valid flash
  ROM space and that space was previously erased.  The command also contains an 
  8-bit checksum to validate it's contents.
   
ARGUMENTS PASSED:
   data_ptr - Pointer to data frame (if any). 

RETURN VALUE:
   None

PRE-CONDITIONS:
   This function is called only after the ERASE command is sent and the phone successfully
   erase the memory sector in which the address is located.

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/

void handle_command_ADDR(u8 * data_ptr)
{

	u32 addr;
	u8 rx_csum = 0;
	u8 calc_csum = 0;
	u8 response[MAX_RESP_DATA_SIZE];
	u8 i;			/* address bytes counter */

	if (data_ptr == NULL) {
		parse_err_response(BLOADER_ERR_DATA_INVALID);
		return;
	}

	/* Calculate the checksum based on received data */
#ifdef ZQ_DEBUG
	PRINT_STR("Entering ADDR Handler, the data is: ");
	PRINT_STR(data_ptr);
	PRINT_STR("\n");
#endif /*ZQ_DEBUG */

	for (i = 0; i < ADDR_CMD_ADDR_SIZE; i++) {
		calc_csum += data_ptr[i];
	}

	/* Converted the received address from ASCII string to number */
	addr = util_hexasc_to_u32(&data_ptr[0], ADDR_CMD_ADDR_SIZE);
	/* Converted the received checksum from ASCII string to number */
	rx_csum = (u8) util_hexasc_to_u32(&data_ptr[ADDR_CMD_CHKSUM_INDEX],
					  ADDR_CMD_CHKSUM_SIZE);

	/* Validate the checksum */
	if (rx_csum != calc_csum) {
		/* Bad checksum, return error */
		parse_err_response(BLOADER_ERR_BAD_CHECKSUM);
	} else {
#ifdef ZQ_DEBUG
		PRINT_STR("The checksum is OK, should do something here\n");
#endif /* ZQ_DEBUG */

		if (parser_valid_address(addr)) {
			blvar_received_address_ptr = (u16 *) addr;
			util_copy_str(&response[0], data_ptr);
			parse_ack_response(response);
		} else {
			parse_err_response(BLOADER_ERR_ADDRESS_INVALID);
		}

	}
}

/*==============================================================================

FUNCTION: handle_command_RQRC

DESCRIPTION: Handler for the RQRC command.

   Return the RAM Downloader checksums.  

ARGUMENTS PASSED:
   data_ptr - Pointer to data frame (if any) that came with this command

REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   None

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/

void handle_command_RQRC(u8 * data_ptr)
{
	u8 *data_start_ptr, *data_end_ptr;
	u32 data_start, data_end;
	u8 response[RAM_CHECKSUM_RESPONSE_SIZE];
	u8 *response_ptr = &response[0];
	u16 csum = 0;

	/* JOSEFIX */
	/* must add some check for the data here */
	if (data_ptr == NULL) {
		parse_err_response(BLOADER_ERR_DATA_INVALID);
		return;
	}
#ifdef ZQ_DEBUG
	PRINT_STR("Entering RQRC Handler, the data is: ");
	PRINT_STR(data_ptr);
	PRINT_STR("\n");
#endif /* ZQ_DEBUG */

	/* Converted the received starting address from ASCII string to number */
	data_start = util_hexasc_to_u32(&data_ptr[0], ADDR_CMD_ADDR_SIZE);
	data_end =
	    util_hexasc_to_u32(&data_ptr[ADDR_CMD_ADDR_SIZE + 1],
			       ADDR_CMD_ADDR_SIZE);

	data_start_ptr = (u8 *) data_start;

	/* Converted the received ending address from ASCII string to number */
	data_end_ptr = (u8 *) data_end;

#ifdef ZQ_DEBUG
	PRINT_STR("The start addr of the RQRC is: ");
	PRINT_HEX(data_start_ptr);
	PRINT_STR("\nThe end addr of the RQRC is: ");
	PRINT_HEX(data_end_ptr);
	PRINT_STR("\n");
#endif

	/* check for the 1K minimum boundary size of the RAM */
	if ((data_end - data_start) < MIN_RAM_SIZE) {
		parse_err_response(BLOADER_ERR_DATA_INVALID);
		return;
	}

	while (data_start_ptr <= data_end_ptr) {
		csum += *data_start_ptr;
		/* Increment to the next data byte */
		data_start_ptr++;
	}
	/* Add ASCII equivalent of csum to response */
	util_u16_to_hexasc(csum, response_ptr);
	parse_send_packet(RQRC_RESPONSE_KEYWORD_STRING, response, strlen(response));
}

/*==============================================================================

FUNCTION: handle_command_RQHW

DESCRIPTION: Handler for the RQHW command.

   The RQHW command requests return of Hardware Descriptor from the Flash 
   Bootloader sector. Data is returned in HEX-ASCII encoded values. If Flash 
   Bootloader is erased, i.e. invalid barker value, "FFFFFFFF" is returned. 
   The response consists of 16 ASCII HEX characters, followed by the NULL 
   terminator. 
  

ARGUMENTS PASSED:
   data_ptr - Pointer to data frame (if any) that came with this command
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   None

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/
void handle_command_RQHW(u8 * data_ptr)
{
	const u8 *source_ptr;
	u8 response[(2 * HW_DESCR_BLOCK_LENGTH) + 1];
	//u8       response_byte_count;
	BOOLEAN bValidFlashBarker;
#ifdef ZQ_DEBUG
	PRINT_STR("handle_command_RQHW is called\n");
	PRINT_STR("Entering RQHW Handler, the data is: ");
	PRINT_STR(data_ptr);
	PRINT_STR("\n");
#endif /* ZQ_DEBUG */

	bValidFlashBarker = flash_bootloader_barker_is_valid();

	/* Generate query response based on validity of barker. */
	source_ptr = NULL;
	if (bValidFlashBarker == TRUE) {
		source_ptr = (u8 *) & ((PFLASH_HEADER) BOOTLOADER_HEADER_ADDR)
		    ->flash_bl_hardware_descriptor_descriptor_type;
	}
	//response_byte_count = 
	add_bin_ascii_response(source_ptr, &response[0], HW_DESCR_BLOCK_LENGTH);

	parse_send_packet((u8 *) RQHW_RESPONSE_KEYWORD_STRING, response, strlen(response));
}

/*==============================================================================

FUNCTION: handle_command_RQVN

DESCRIPTION: Handler for the RQVN command.

   The RQVN command requests return of version numbers.  

   It returns (in the order shown):

      1. Software Descriptor of the running boot.(Bootloader or RAM Downloader).
      2. Flash Bootloader Software Descriptor.   'F's if not programmed.
      3. Phone Code Software Package Descriptor. 'F's if invalid Barker. 
      4. Code Group 0 Software Descriptor.       'F's if CG0 not defined.   
     ...                                            ...
    4+n-1. Code Group n Software Decriptor.         'FF's if CGn not defined.
          
   The length of the response is variable and based upon 'n', the number of code
   groups specified in the Code Group 0 Header data. Each of the comma separated
   fields has a fixed size of 16 ASCII HEX characters. The response is NULL 
   terminated.  
   
ARGUMENTS PASSED:
   data_ptr - Pointer to data frame (if any) that came with this command
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/

void handle_command_RQVN(u8 * data_ptr)
{
	u8 response[MAX_RESPONSE_SIZE];
	u8 nr_of_code_groups;
	u8 code_group;
	u16 response_byte_count;
	const u8 *code_group_sw_descriptor_ptr;
	const u8 *source_ptr;
	BOOLEAN bValidFlashBarker;
	PFLASH_HEADER pFlashHeader;
	PCODE_GROUP_0 pCodeGroup0Header;

	u8 hardcode_response[] =
	    "000D0203340F0001,000D0201340F0001,000D02010100340F,000D0206340F0001,000D0206340F0001,000D0206340F0001,000D0206340F0001,000D0206340F0001,000D0206340F0001";

#ifdef ZQ_DEBUG
	PRINT_STR("Entering RQVN Handler, the data is: ");
	PRINT_STR(data_ptr);
	PRINT_STR("\n");
#endif /* ZQ_DEBUG */

	pFlashHeader = (PFLASH_HEADER) BOOTLOADER_HEADER_ADDR;
	response_byte_count = 0;

	/* Add to response RUNNING BOOT SOFTWARE DESCRIPTOR. */
	/* Start 0:  16 bytes. */
	response_byte_count += get_running_boot_info(&response[0]);

	/* Add to response FLASH BOOTLOADER SOFTWARE DESCRIPTOR. */
	source_ptr = NULL;
	response[response_byte_count++] = RESPONSE_FIELD_SEPARATOR;
	if ((bValidFlashBarker = flash_bootloader_barker_is_valid()) == TRUE) {
		/* Point to beginning of software descriptor for Flash Bootloader. */
		source_ptr =
		    (u8 *) & pFlashHeader->
		    flash_bl_sw_descriptor_descriptor_type;
	}
	response_byte_count +=
	    add_bin_ascii_response
	    (source_ptr, &response[response_byte_count], SW_DESCRIPTOR_SIZE);

	/* Add to response Phone Code SW PACKAGE DESCRIPTOR. */
	source_ptr = NULL;
	response[response_byte_count++] = RESPONSE_FIELD_SEPARATOR;
	nr_of_code_groups = 0;

	/* JOSEFIX */
	/* This must be initialized here ?? */
	pCodeGroup0Header = (PCODE_GROUP_0) *
	    ((u32 *) (&pFlashHeader->flash_bl_begin_address_of_code_group_0));

	if (bValidFlashBarker && (main_code_barker_is_valid() == TRUE)) {
		/* Bootloader must be OK. since we use one of its address pointers. */
		source_ptr =
		    &pCodeGroup0Header->
		    phone_code_sw_descriptor_descriptor_type;
		nr_of_code_groups =
		    *(u8 *) & pCodeGroup0Header->
		    code_group_number_of_code_groups;
		/* bounds check nr_of_code_groups */
		if (nr_of_code_groups > MAX_CODE_GROUP_SIZE) {
			nr_of_code_groups = MAX_CODE_GROUP_SIZE;
		}
	}
	response_byte_count +=
	    add_bin_ascii_response
	    (source_ptr, &response[response_byte_count],
	     SW_PACKAGE_DESCRIPTOR_SIZE);

	/* Add to response the SOFTWARE DESCRIPTOR FOR EACH CODE GROUP (if any).  */
	for (code_group = 0;
	     ((code_group < MAX_CODE_GROUP_SIZE) && (nr_of_code_groups != 0));
	     code_group++) {
		/* Iterate through list and convert Code Group[j] Code SW Descriptor. */
		code_group_sw_descriptor_ptr = NULL;

		/* Point to location where start address of this group is stored. */
		source_ptr =
		    ((u8 *) & pCodeGroup0Header->code_group_0_start_address) +
		    (2 * code_group * sizeof(u32));

		if (*(u32 *) source_ptr != UNDEFINED_ADDRESS) {
			/* CG does exist - point to start of SW descriptor of this CG. */
			code_group_sw_descriptor_ptr =
			    ((u8 *) * (u32 *) source_ptr) +
			    SW_GROUP_DESRIPTOR_OFFSET;

			nr_of_code_groups--;
		}

		response[response_byte_count++] = RESPONSE_FIELD_SEPARATOR;
		response_byte_count +=
		    add_bin_ascii_response(code_group_sw_descriptor_ptr,
					   &response[response_byte_count],
					   SW_DESCRIPTOR_SIZE);

	}

	parse_send_packet((u8 *) RQVN_RESPONSE_KEYWORD_STRING,
			  "gen-blob", 8);
}

/*==============================================================================

FUNCTION: handle_command_JUMP

DESCRIPTION: Jumps to new address
   This transfers control to the code specified in the parameter 

ARGUMENTS PASSED:
   Address to jump to - usually a RAM address.
    
RETURN VALUE:
   None

PRE-CONDITIONS:
   None
 
POST-CONDITIONS:
   Will never return.  Processor control is transferred to new address

IMPORTANT NOTES:
   None
==============================================================================*/

void handle_command_JUMP(u8 * data_ptr)
{

	/* checksum received from host */
	u8 nRX_csum;
	/* calculated checksum */
	u8 nCalc_csum;
	u8 i;
	u32 nJumpAddr;
	/* u32 *jump_addr_ptr = NULL; */
/*
    u8 *source_ptr = (u8 *) ADD_INFO_SOURCE_PTR;
    u8 *dest_ptr = (u8 *) ADD_INFO_DEST_PTR;
*/
	/* Calculate the checksum based on received data */
	nCalc_csum = 0;

	if (data_ptr == NULL) {
		parse_err_response(BLOADER_ERR_DATA_INVALID);
		return;
	}

#ifdef ZQ_DEBUG
	PRINT_STR("Entering JUMP Handler, the data is: ");
	PRINT_STR(data_ptr);
	PRINT_STR("\n");
#endif /* ZQ_DEBUG */

	for (i = 0; i < JUMP_CMD_ADDR_SIZE; i++) {
		nCalc_csum += data_ptr[i];
	}

	/* determine the address */
	nJumpAddr = util_hexasc_to_u32(data_ptr, JUMP_CMD_ADDR_SIZE);

	nRX_csum = (u8) util_hexasc_to_u32(&data_ptr[JUMP_CMD_CHKSUM_INDEX],
					   JUMP_CMD_CHKSUM_SIZE);
	/* jump_addr_ptr = (u32 *)nJumpAddr; */
	/* terminate data */
	/* data_ptr[JUMP_CMD_CHKSUM_INDEX] = NULL; */

	if (nCalc_csum == nRX_csum) {
		/* send correct ACK response back */
		parse_ack_response(data_ptr);

#if 0				/* a17400 : we remove this copy function, group info addr'll be set to another macro */
		/* The CGs address information is sent as a part of RDL to the SU */
		/* We copy it to another part of RAM to save these data */
		for (i = 0; i < ADD_INFO_DATA_SIZE; i++) {
			*dest_ptr++ = *source_ptr++;
		}

#endif
		pxa_usb_stop();
		util_jump_to_ram(nJumpAddr);
	} else {
		/* send bad checksum error */
		parse_err_response(BLOADER_ERR_BAD_CHECKSUM);
	}

}

/*==============================================================================

FUNCTION: handle_command_BIN

DESCRIPTION: Handler for the BIN command.

   For Ram Downloaders: Program Flash with data starting at a preset address in 
   the global variable 'blvar_received_address_ptr'. Encryption of data is 
   performed for Ram Downloader for FLASH but not for Ram Downloader for ROM.
   
   The Data buffer contains binary packet as outlined in Sierra Boot
   Interface Specification:
     First 2 Bytes: Size, MSB first. 8 to 8K Bytes.
     Data Block: Between 8 Bytes to 8 KBytes of Data. (Should be Word aligned).
     Last Byte:     Checksum.
             
   DECRYPTION: Not supported in initial version. 
             
ARGUMENTS PASSED:
   data_ptr - pointer to data block.
    
REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   None

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/
void handle_command_BIN(u8 * data_ptr)
{
	u16 data_size;
	const u8 *source_ptr;
	u8 *dest_ptr;
	u8 *temp_ptr;
	/* Realign 'data_ptr' to u32 to supress access errors. */
	u8 nr_shift_right;
	u32 i;

	if (data_ptr == NULL) {
		parse_err_response(BLOADER_ERR_DATA_INVALID);
		return;
	}

#ifdef ZQ_DEBUG
	PRINT_STR("Entering BIN Handler\n");
#endif /* ZQ_DEBUG */

	/* Point to MSB of data size field. */
	source_ptr = data_ptr;

	/* Compute number of data bytes in this block. */
	data_size = ((source_ptr[BIN_DATA_SIZE_MSB] << SHIFT_MSB) +
		     source_ptr[BIN_DATA_SIZE_LSB]);

	/* Decrypted data: Data block size must be multiple of 8 bytes. */
	if (data_size % MIN_NUMB_BYTES) {
		/* ERROR - Not multiple of 8 bytes. */
		parse_err_response(BLOADER_ERR_PACKET_SZ_INVALID);
		return;
	}

	/* Advance to first data byte. */
	source_ptr += BIN_DATA_SIZE_FIELD_LENGTH;

	/* 
	 * Force data alignment to MCORE WORD (u32) boundary. Data is shifted to the     * right up to 3 bytes.
	 */
	nr_shift_right = sizeof(u32) - (((u32) source_ptr) % sizeof(u32));
	nr_shift_right %= sizeof(u32);	/* Within 0..3. */
	if (nr_shift_right != 0) {
		/* Odd address boundary - shift left data by 1 byte. */
		temp_ptr = (u8 *) source_ptr + data_size - 1;
		for (i = 0; i < data_size; i++, temp_ptr--) {
			temp_ptr[nr_shift_right] = temp_ptr[0];
		}
		source_ptr += nr_shift_right;	/* Point to u32 aligned value. */
	}
	/* Copy to RAM. */
	dest_ptr = (u8 *) blvar_received_address_ptr;
	for (i = 0; i < data_size; i++) {
		*dest_ptr++ = *source_ptr++;
	}
#ifdef ZQ_DEBUG
	dest_ptr -= i;
	PRINT_STR("The data_size is:");
	PRINT_DEC(data_size);
	PRINT_STR("\nThe dest_ptr is: ");
	PRINT_HEX(dest_ptr);
/*    PRINT_STR("\nThe data on the dest_ptr is:\n"); */
/*    for( i = 0; i < data_size; i++)
    {
       if(( i % 16) == 0)
	      PRINT_STR("\n");

       SerialOutputHexU8(*dest_ptr++);
       PRINT_STR(" ");
    }
    */
#endif
	parse_ack_response(NULL);
	return;
}

/*==============================================================================
FUNCTION: handle_command_RQCS

DESCRIPTION: Handler for the RQCS command.

   The RQCS command requests return of the FLASH memory checksums.  A returned 
   string that represents the ASCII hex number of the 16-bit checksum of each of
   the code in the memory,i.e., boot, code group 0, code group 1, code group 2, 
   etc... If one of the code group is not available or undefined in anyway,
   then four spaces is return for that code group.

ARGUMENTS PASSED:
   data_ptr - Pointer to data frame (if any). 

REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   None

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/

void handle_command_RQCS(u8 * data_ptr)
{
	u8 response[(MAX_CODE_GROUP_SIZE + 1) * (ASCII_HEX_CHECKSUM_LENGTH +
						 1)];
	u8 *response_ptr = &response[0];
	u8 i;
	u8 j;
	u8 numb_valid_codegroup = 0;	/* total number of valid code group */
	u16 total_csum = 0;
	u16 csum[BLOADER_TOTAL_CSUM + 1];
	/* add 1 for the Flash Bootloader checksum */
	BOOLEAN valid_check_sum[MAX_CODE_GROUP_SIZE + 1];

#ifdef ZQ_DEBUG
	PRINT_STR("Entering RQCS Handler, the data is: ");
	PRINT_STR(data_ptr);
	PRINT_STR("\n");
#endif /* ZQ_DEBUG */

	/* Calc CSUM on bootloader and/or code group individually */
	for (i = 0; i < BLOADER_TOTAL_CSUM; i++) {
		/* init the checksum flag before calculating the checksum */
		valid_check_sum[i] = TRUE;
		csum[i] =
		    util_calc_csum(i, &numb_valid_codegroup,
				   &valid_check_sum[i]);
		if (i == BLOADER_BOOT_CSUM) {
			/* bootloader is not existed, stop calculating further checksum */
			if (csum[i] == 0) {
				break;
			}
		} else {
			/* if CG0 is not existed, stop calculating further checksum */
			/* if( (i == BLOADER_CODE_GROUP0_CSUM) && csum[i] == 0 )
			   break; */
			/* No more valid code group then exit the for loop */
			if (numb_valid_codegroup == 0) {
				/* Add the last one */
				total_csum++;
				break;
			}
		}
		/* the actual number of CGs that will be sent back to the host */
		total_csum++;
	}

	/*
	 * Loop through block checksums (only if they are existed), 
	 * adding them to the response string.     
	 */
	if (total_csum > 0) {
		for (i = 0; i < (total_csum - 1); i++) {
			/* Insert spaces if the code group is not available or undefined */
			if ((i >= BLOADER_CODE_GROUP0_CSUM) &&
			    (valid_check_sum[i] == FALSE)) {
				for (j = 0; j < 4; j++) {
					*response_ptr++ = CHAR_SPACE;	/* Insert space character */
				}
			} else {
				/* Add ASCII equivalent of csum to response */
				util_u16_to_hexasc(csum[i], response_ptr);
				/* Move index past the ASCII hex csum string */
				response_ptr += (2 * sizeof(u16));
			}

			*response_ptr++ = RESPONSE_FIELD_SEPARATOR;	/* Add delimitter */
		}

		/* Add ASCII equivalent of last csum to response */
		util_u16_to_hexasc(csum[i], response_ptr);
		/* Move index past the ASCII hex csum string */
		response_ptr += (2 * sizeof(u16));
	}

	*response_ptr = NULL;	/* Terminate string */

	parse_send_packet(RQCS_RESPONSE_KEYWORD_STRING, response, strlen(response));
}

/*==============================================================================
FUNCTION: handle_command_PWR_DOWN

DESCRIPTION: Handler for the POWER_DOWN command.

   The POWER_DOWN command requests the SU to power down.

ARGUMENTS PASSED:
   None

REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   None

PRE-CONDITIONS:
   None

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/

void handle_command_PWR_DOWN(u8 * data_ptr)
{

#ifdef ZQ_DEBUG
	PRINT_STR("Entering POWER_DOWN Handler, the data is: ");
	PRINT_STR(data_ptr);
	PRINT_STR("\n");
#endif /* ZQ_DEBUG */

	/* Acknowledge the POWER_DOWN request */
	parse_ack_response((u8 *) NULL);

	/* call power down routine */
	util_power_down();

}

#ifndef RDL_FLASH_H
#define RDL_FLASH_H
/*
 * bl_flash.h
 *
 * The flash driver header file for RDL.
 * 
 * Copyright (C) 2001-2005 Motorola
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


/*==============================================================================
GLOBAL MACROS DECLARATION
==============================================================================*/
#define K3_ONEBLOCK_SIZE            0x20000
#define ERASE_BLOCK_NUM             1
#define DEFAULT_FLASH_SIZE          (K3_ONEBLOCK_SIZE*ERASE_BLOCK_NUM)
#define DEFAULT_ERASE_SIZE          DEFAULT_FLASH_SIZE
#define DEFAULT_FLASH_ADDR          ( (volatile void *) 0x0)

#define MAX_ERASE_SIZE              0x600000
#define MAX_INFO_SIZE               1024





/* Status Register Bits */
#define BIT_0						0x1
#define BIT_1						0x2
#define BIT_2						0x4
#define BIT_3						0x8
#define BIT_4						0x10
#define BIT_5						0x20
#define BIT_6						0x40
#define BIT_7						0x80


/*==============================================================================
ENUMS
==============================================================================*/
typedef enum
{
   FLASH_STATUS_OK = 2,
   FLASH_STATUS_VPP_ERR,
   FLASH_STATUS_PROGRAM_ERR,
   FLASH_STATUS_ERASE_ERR,
   FLASH_STATUS_BLOCK_PROTECT_ERR,
   FLASH_STATUS_CMD_SEQ_ERR,  
   FLASH_STATUS_ADDR_ERR
} FLASH_STATUS;

typedef enum	  
{
	K3RC28f128k,				/* K3 */

	/* MAX_FLASH_TYPES is used to allocate array size--must be last entry */
	MAX_FLASH_TYPES
} E_FLASH_TYPE;


/*==============================================================================
STRUCTURES AND OTHER TYPEDEFS
==============================================================================*/

typedef struct ST_FLASH_OP	  
{
    E_FLASH_TYPE   e_flash_type;	/* flash_type code (from ENUM list) */
    volatile void  *flash_addr;		/* flash memory base address (to pointer) */
    unsigned int   image_size;		/* No. of bytes to program into flash */
    unsigned int   erase_size;
	void           *image_buffer;	/* pointer to image in RAM (from pointer) */
	unsigned int   unlock_blocks;	/* clear all locked blocks before erasing the flash */

} S_FLASH_OP;


/* Struct for the command sequence (in the vendor`s algorithm.) */

typedef struct ST_FLASH_CMD	  
{
    volatile int   flash_address;
	unsigned char  flash_code;
	
} S_FLASH_CMD;


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
#ifdef __cplusplus
extern "C" {
#endif

extern FLASH_STATUS flash_erase_blocks(u32 start_addr, u32 end_addr);

extern FLASH_STATUS flash_program_K3RC28f128k (volatile u16 *flash_addr,
							u32 length, u16 *flash_buf);


#ifdef __cplusplus
}
#endif


/*============================================================================*/
#endif  /* RDL_FLASH_H */



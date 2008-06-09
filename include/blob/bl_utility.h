#ifndef BLOB_BL_UTL_H
#define BLOB_BL_UTL_H
/*
 * bl_utility.h
 *
 * Bootloader utility functions header
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


/*==============================================================================
GLOBAL MACROS DECLARATION
==============================================================================*/
#define num_elements(a) (sizeof(a)/sizeof(a[0]))

/*==============================================================================
ENUMS
==============================================================================*/



/*==============================================================================
STRUCTURES AND OTHER TYPEDEFS
==============================================================================*/


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

/* Hex conversions. */
extern u32  util_hexasc_to_u32(u8 *str,u8 size);
extern void    util_u32_to_hexasc(u16 val,u8 *str_ptr);  /* not used yet */
extern void    util_u16_to_hexasc(u16 val,u8 *str_ptr);  
extern void    util_u8_to_hexasc(u8  val,u8 *str_ptr);

/* String utilities. */
extern BOOLEAN util_string_equal(u8 *str1_ptr,u8 *str2_ptr);
extern void util_copy_str(u8 *dest_ptr,u8 *src_ptr);

extern void util_jump_to_ram(u32 ram_addr);

extern void util_power_down ( void );

extern void util_restart ( void );

#ifdef __cplusplus
}
#endif


/*============================================================================*/
#endif  /* BLOB_BL_UTL_H */



/*
 * bl_utility.h
 *
 * Entry point of flash loader feature in BLOB
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
 * Oct 30, 2002 - (Motorola) Created
 * 
 */
/*================================================================================
INCLUDE FILES
==============================================================================*/
#include  "bl_utility.h"
#include "pxa.h"
#ifdef ZQ_DEBUG
#include "bl_debug.h"
#endif /* ZQ_DEBUG */

/*==============================================================================
LOCAL CONSTANTS
==============================================================================*/

/*==============================================================================
LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==============================================================================*/

/*==============================================================================
LOCAL MACROS
==============================================================================*/

/*==============================================================================
LOCAL FUNCTION PROTOTYPES
==============================================================================*/

/*==============================================================================
LOCAL VARIABLES
==============================================================================*/

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
GLOBAL FUNCTIONS
==============================================================================*/

/*==============================================================================

FUNCTION: util_hexasc_to_ui32

DESCRIPTION: Local utility to interpret an ASCII string as a hexidecimal value.

ARGUMENTS PASSED:
   str_ptr - pointer to ASCII string
   size - number of chars to translate

REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   hexidecimal value as a u32

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/

u32 util_hexasc_to_u32(u8 * str_ptr, u8 size)
{
	u8 digit;
	u32 val = 0L;

	while (size--) {
		digit = *str_ptr++;

		val <<= 4;	/* Shift previous digit over */
		val += (digit >= 'A') ? (digit - '7') : (digit - '0');
	}

	return (val);
}

/*==============================================================================

FUNCTION: util_ui8_to_hexasc

DESCRIPTION: Local utility to convert a word into a hex ASCII string

ARGUMENTS PASSED:
   val - byte to be converted
   str_ptr - pointer to where string should be placed
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/

void util_u8_to_hexasc(u8 val, u8 * str_ptr)
{
	u8 i, digit;

	for (i = 0; i < 2; i++) {
		digit = (val >> 4) & 0x0f;
		val <<= 4;
		*str_ptr++ = (digit > 9) ? (digit + '7') : (digit + '0');
	}
	*str_ptr = NULL;
}

/*==============================================================================

FUNCTION: util_ui16_to_hexasc 

DESCRIPTION: Local utility to convert a word into a hex ASCII string

ARGUMENTS PASSED:
   val - word to be converted
   str_ptr - pointer to where string should be placed
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/

void util_u16_to_hexasc(u16 val, u8 * str_ptr)
{
	u8 i, digit;

	for (i = 0; i < 4; i++) {
		digit = (val >> 12) & 0x0f;
		val <<= 4;
		*str_ptr++ = (digit > 9) ? (digit + '7') : (digit + '0');
	}
	*str_ptr = NULL;
}

/*==============================================================================

FUNCTION: util_ui32_to_hexasc

DESCRIPTION: Local utility to convert a word into a hex ASCII string

ARGUMENTS PASSED:
   val - double word to be converted
   str_ptr - pointer to where string should be placed

REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/

void util_u32_to_hexasc(u16 val, u8 * str_ptr)
{
	u8 i, digit;

	for (i = 0; i < 8; i++) {
		digit = (val >> 28) & 0x0f;
		val <<= 4;
		*str_ptr++ = (digit > 9) ? (digit + '7') : (digit + '0');
	}
	*str_ptr = NULL;
}

/*==============================================================================

FUNCTION: util_string_equal

DESCRIPTION: Local utility to test if two strings match

ARGUMENTS PASSED:
   str1_ptr, str2_ptr - pointers to strings to compare
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   TRUE - strings match
   FALSE - strings differ

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/

BOOL util_string_equal(u8 * str1_ptr, u8 * str2_ptr)
{
	BOOLEAN match = FALSE;

	while (*str1_ptr && *str2_ptr && (*str1_ptr == *str2_ptr)) {
		str1_ptr++;
		str2_ptr++;
	}

	if (*str1_ptr == *str2_ptr) {
		match = TRUE;
	}

	return (match);
}

/*==============================================================================

FUNCTION: util_string_copy

DESCRIPTION: Local utility to copy a string

ARGUMENTS PASSED:
   dest_ptr - destination pointer (where to copy string to)
   src_ptr - source pointer (where to copy string from)
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/

void util_copy_str(u8 * dest_ptr, u8 * src_ptr)
{
	do {
		*dest_ptr++ = *src_ptr;
	}
	while (*src_ptr++);	/* do-while will copy null terminator! */
}

/*============================================================================*/

void util_jump_to_ram(u32 ram_addr)
{
	void (*ramloader) (void) = (void (*)(void))ram_addr;

#ifdef ZQ_DEBUG
	PRINT_STR("Jump to the new address: ");
	PRINT_HEX(ram_addr);
	PRINT_STR("\n");
	PRINT_STR("\nRuning ramloader ......\n");
#endif /* ZQ_DEBUG */

	ramloader();
}

void util_power_down(void)
{

#ifdef ZQ_DEBUG
	PRINT_STR("Power down the phone\n");
#endif /* ZQ_DEBUG */

	GAFR2_U &= 0xffff3fff;
	GAFR2_U |= 0x00004000;
	GPCR2 = 0x00800000;
	GPDR2 &= 0xff7fffff;
	while (1) {
#ifdef ZQ_DEBUG
		PRINT_STR("Power down infinite looping\n");
#endif /* ZQ_DEBUG */
	}
}

void util_restart(void)
{
#ifdef ZQ_DEBUG
	PRINT_STR("Restart the phone\n");
#endif /* ZQ_DEBUG */

	GAFR0_U = GAFR0_U & 0xffffcfff;
	GPCR0 = 0x00400000;
	GPDR0 = GPDR0 | 0x00400000;
	while (1) {
#ifdef ZQ_DEBUG
		PRINT_STR("Re-Start infinite looping\n");
#endif /* ZQ_DEBUG */
	}

}

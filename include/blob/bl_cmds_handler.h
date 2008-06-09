#ifndef BLOB_BL_CMDHANDLER_H
#define BLOB_BL_CMDHANDLER_H
/*
 * bl_cmds_handler.h
 *
 * Command Handler for boot commands
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
 * Oct 30,2002 - (Motorola) Created
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

/* Description of the purpose or use of this macro, if needed. */
//#define    MF_SF_MacroName (x,y)    ( ((x) > (y)) ? (x) : (y) )

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

/* Command handler protoptypes. */


/* Query Version info. */
extern void handle_command_RQVN     (u8 *data_ptr);  
/* Query HW descriptor. */
extern void handle_command_RQHW     (u8 *data_ptr);  
extern void handle_command_ADDR     (u8 *data_ptr);
/* Binary data for RAM,FLASH. */
extern void handle_command_BIN      (u8 *data_ptr);  


/* Power down phone. */
extern void handle_command_PWR_DOWN (u8 *data_ptr);  
/* Query FLASH checksum(s). */
extern void handle_command_RQCS     (u8 *data_ptr);  
extern void handle_command_RQRC    (u8 *data_ptr);
/* JUMP command */
extern void handle_command_JUMP  (u8 *data_ptr);  

#ifdef __cplusplus
}
#endif


/*============================================================================*/
#endif  /* BLOB_BL_CMDHANDLER_H */



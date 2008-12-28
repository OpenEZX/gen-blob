#ifndef BLOB_BL_PARSER_H
#define BLOB_BL_PARSER_H
/*
 * bl_parser.h
 *
 * Bootloader command data parser and support header
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



#define USB_HEADER_MIN_SIZE     6       /* MIN HEADER/COMMAND SIZE */
/* MAX HEADER/COMMAND SIZE,i.e., not counting for DATA */
#define USB_HEADER_SIZE        12       
#define USB_DATA_ARRAY_SIZE    64
#define USB_MAX_PACKET_SIZE    255
#define USB_INITIALIZED_STATE   0


#define MAX_NUMBER_OF_COMMAND  10
#define MAX_COMMAND_STR_SIZE   12
/* DWORD align(3)+STX+"BIN"+RS+size(2)+data(8192)+checksum(1)+ETX */
#define MAX_RX_DATA_SIZE     8204    
#define MAX_TX_DATA_SIZE     2048
#define MAX_DATA_FIELD_SIZE     2
#define MAX_BIN_PACKET_SIZE  8192
#define MIN_BIN_PACKET_SIZE     8
#define BIN_DATA_SIZE_MSB       0
#define BIN_DATA_SIZE_LSB       1
#define SHIFT_MSB               8
#define MAX_RESP_DATA_SIZE     64

#define ADDR_CMD_ADDR_SIZE      8
#define ADDR_CMD_CHKSUM_INDEX   8
#define ADDR_CMD_CHKSUM_SIZE    2
#define JUMP_CMD_ADDR_SIZE      8
#define JUMP_CMD_CHKSUM_INDEX   8
#define JUMP_CMD_CHKSUM_SIZE    2

/**** Generic Bit Codes **/
#define BLOADER_BIT0            0x01
#define BLOADER_BIT1            0x02
#define BLOADER_BIT2            0x04
#define BLOADER_BIT3            0x08
#define BLOADER_BIT4            0x10
#define BLOADER_BIT5            0x20
#define BLOADER_BIT6            0x40
#define BLOADER_BIT7            0x80

#define BLOADER_ALWAYS_SET_BIT     BLOADER_BIT7

/* Command Index */
#define BLOADER_ERASE_INDEX         0
#define BLOADER_ADDR_INDEX          1
#define BLOADER_BIN_INDEX           2
#define BLOADER_BAUD_INDEX          3
#define BLOADER_POWER_DOWN_INDEX    4
#define BLOADER_RQCS_INDEX          5
#define BLOADER_RQHW_INDEX          6
#define BLOADER_RQRC_INDEX          7
#define BLOADER_RQSN_INDEX          8
#define BLOADER_RQVN_INDEX          9
#define BLOADER_JUMP_INDEX         10
#define BLOADER_WUPID_INDEX        11
#define BLOADER_RUPID_INDEX        12


/* Error Codes */
#define BLOADER_ERR_BAD_CHECKSUM           (BLOADER_ALWAYS_SET_BIT | 0x00)
#define BLOADER_ERR_ERASE_FAILED           (BLOADER_ALWAYS_SET_BIT | 0x01)
#define BLOADER_ERR_ERASE_SUSPENDED        (BLOADER_ALWAYS_SET_BIT | 0x02)
#define BLOADER_ERR_ADDRESS_INVALID        (BLOADER_ALWAYS_SET_BIT | 0x03)
#define BLOADER_ERR_PACKET_SZ_INVALID      (BLOADER_ALWAYS_SET_BIT | 0x04)
#define BLOADER_ERR_UNKNOWN_COMMAND        (BLOADER_ALWAYS_SET_BIT | 0x05)
#define BLOADER_ERR_PROGRAMMING            (BLOADER_ALWAYS_SET_BIT | 0x06)
#define BLOADER_ERR_FLASH_NOT_READY        (BLOADER_ALWAYS_SET_BIT | 0x07)
#define BLOADER_ERR_FLASH_NOT_ERASED       (BLOADER_ALWAYS_SET_BIT | 0x08)
#define BLOADER_ERR_VPP_LOW                (BLOADER_ALWAYS_SET_BIT | 0x09)
#define BLOADER_ERR_GENERAL_FAIL           (BLOADER_ALWAYS_SET_BIT | 0x0A)
#define BLOADER_ERR_DATA_INVALID           (BLOADER_ALWAYS_SET_BIT | 0x0B)
#define BLOADER_ERR_ADDRESS_NOT_UPDATED    (BLOADER_ALWAYS_SET_BIT | 0x0C)
#define BLOADER_ERR_COMMAND_SEQ_INVALID    (BLOADER_ALWAYS_SET_BIT | 0x0D)
#define BLOADER_ERR_FLASH_RAM_VER_MISMATCH (BLOADER_ALWAYS_SET_BIT | 0x0E)
#define BLOADER_ERR_RECEIVE_TIMEOUT        (BLOADER_ALWAYS_SET_BIT | 0x0F)
#define BLOADER_ERR_UPID_AREA_FULL         (BLOADER_ALWAYS_SET_BIT | 0x10)
#define BLOADER_ERR_INVALID_CG0_BARKER     (BLOADER_ALWAYS_SET_BIT | 0X11)
#define BLOADER_ERR_CG0_COPY_FAILED        (BLOADER_ALWAYS_SET_BIT | 0X12)
#define BLOADER_ERR_INVALID_CG4_ADDRESS    (BLOADER_ALWAYS_SET_BIT | 0X13)
#define BLOADER_ERR_CG1_COPY_FAILED        (BLOADER_ALWAYS_SET_BIT | 0X14)
#define BLOADER_ERR_UPID_LAST_SLOT         (BLOADER_ALWAYS_SET_BIT | 0x20)
#define BLOADER_ERR_RETRY_TRANSMIT         (BLOADER_ALWAYS_SET_BIT | 0x21)
#define BLOADER_ERR_UPGRADE_CMD_FAILED     (BLOADER_ALWAYS_SET_BIT | 0x22)
#define BLOADER_ERR_FLASH_SIZE_INVALID     (BLOADER_ALWAYS_SET_BIT | 0x24)

#define BUFSZ   5000

/*==============================================================================
GLOBAL MACROS DECLARATION
==============================================================================*/


/*==============================================================================
ENUMS
==============================================================================*/

typedef enum
{
   BLOADER_STATUS_OK,
   BLOADER_STATUS_VPP_LOW,
   BLOADER_STATUS_CS_ERROR,
   BLOADER_STATUS_PROGRAMMING_ERROR,
   BLOADER_STATUS_ERASE_ERROR,
   BLOADER_STATUS_ERASE_SUSPENDED,
   BLOADER_STATUS_FLASH_NOT_READY,
   BLOADER_STATUS_INVALID_PACKET_SIZE,
   BLOADER_STATUS_UNKNOWN_COMMAND,
   BLOADER_STATUS_NO_ADDRESS,
   BLOADER_STATUS_UNKNOWN_FLASH_PART,
   BLOADER_STATUS_PART_PROTECTED,
   BLOADER_STATUS_CG0_UNINITIALIZED,
   BLOADER_STATUS_SHARED_PAGE_UNINITIALIZED,
   BLOADER_STATUS_CG4_ADDRESS_UNMATCHED
} BLOADER_STATUS;

typedef enum
{
   BLOADER_COMMAND_ERASE,
   BLOADER_COMMAND_ADDR,
   BLOADER_COMMAND_BIN,
   BLOADER_COMMAND_BAUD,
   BLOADER_COMMAND_POWER_DOWN,
   BLOADER_COMMAND_RQCS,
   BLOADER_COMMAND_RQHW,
   BLOADER_COMMAND_RQRC,
   BLOADER_COMMAND_RQSN,
   BLOADER_COMMAND_RQVN,
   BLOADER_COMMAND_JUMP,
   BLOADER_COMMAND_WUPID,
   BLOADER_COMMAND_RUPID,
   BLOADER_COMMAND_RBRK,
   BLOADER_COMMAND_UPGRADE,
   BLOADER_COMMAND_RESTART,
   BLOADER_COMMAND_RBIN,
   BLOADER_UNKNOWN_COMMAND
} BLOADER_COMMANDS;


/*==============================================================================
STRUCTURES AND OTHER TYPEDEFS
==============================================================================*/
typedef struct
{
   const u8 *cmd;
   BLOADER_COMMANDS cmd_id;
} BLOADER_COMMAND_TBL;

struct mybuf
{
  u8 buf[BUFSZ];
  u16 len;
};


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

extern void parse_data(void);

extern void parse_err_response (u8 error_code);
extern BOOL parse_send_packet(u8 *command_ptr, u8 *data_ptr, u16 data_size);
extern void parse_ack_response(u8 *data);
extern struct mybuf * get_buf(int io);
void mybuf_reset(void);

/*
extern void power_down_phone ( void );
extern void restart_phone ( void );
*/

#ifdef __cplusplus
}
#endif


/*============================================================================*/
#endif  /* BLOB_FL_PARSER_H */



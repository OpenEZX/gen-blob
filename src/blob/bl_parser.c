/*
 * bl_parser.c
 *
 * Bootloader data parser and support functions
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
 * 2002-Oct-03 - (Motorola) Created
 * 2005-May-14 - (Motorola) Modified the mybuf initial
 * 
 */
/*================================================================================
INCLUDE FILES
==============================================================================*/
#include "bl_parser.h"
#include "bl_utility.h"
#include "bl_cmds_handler.h"

#include "usb_ctl.h"

#ifdef ZQ_DEBUG
#include "bl_debug.h"
#endif

/*==============================================================================
LOCAL CONSTANTS
==============================================================================*/

//#define MAX_TIMER_COUNT         0x6000
//#define MAX_RETRY_COUNT         0x0003

/* maximum size of ACK response data */
#define MAX_ACK_RESPONSE_SIZE         32
#define MAX_NR_OF_CODE_GROUPS         21

#define MAX_RESPONSE_HEADER_SIZE 10	/* STX + keyword eq. "RSVN" + RS + margin. */
/* Max size RQVN response. */
#define MAX_RESPONSE_DATA_SIZE    ((3+MAX_NR_OF_CODE_GROUPS)*17 + 10)
#define MAX_RESPONSE_SIZE    (MAX_RESPONSE_HEADER_SIZE + MAX_RESPONSE_DATA_SIZE)

/* ASCII values */
#define NUL  0x00
#define STX  0x02
#define ETX  0x03
#define RS   0x1E

/*==============================================================================
LOCAL TYPEDEFS (STRUCTURES, UNIONS, ENUMS)
==============================================================================*/

/*==============================================================================
LOCAL MACROS
==============================================================================*/

/*==============================================================================
LOCAL FUNCTION PROTOTYPES
==============================================================================*/

static void handle_command(u8 * pCommand, u8 * pData);
static BLOADER_COMMANDS map_command
    (BLOADER_COMMAND_TBL * table_ptr, u8 table_size, u8 * command_ptr);

void reset_global(void);

/*==============================================================================
LOCAL VARIABLES
==============================================================================*/

static BLOADER_COMMAND_TBL command_tbl[] = {
	{(u8 *) "ERASE", BLOADER_COMMAND_ERASE},
	{(u8 *) "ADDR", BLOADER_COMMAND_ADDR},
	{(u8 *) "BIN", BLOADER_COMMAND_BIN},
	{(u8 *) "BAUD", BLOADER_COMMAND_BAUD},
	{(u8 *) "POWER_DOWN", BLOADER_COMMAND_POWER_DOWN},
	{(u8 *) "RQCS", BLOADER_COMMAND_RQCS},
	{(u8 *) "RQHW", BLOADER_COMMAND_RQHW},
	{(u8 *) "RQRC", BLOADER_COMMAND_RQRC},
	{(u8 *) "RQSN", BLOADER_COMMAND_RQSN},
	{(u8 *) "RQVN", BLOADER_COMMAND_RQVN},
	{(u8 *) "JUMP", BLOADER_COMMAND_JUMP},
	{(u8 *) "WUPID", BLOADER_COMMAND_WUPID},
	{(u8 *) "RUPID", BLOADER_COMMAND_RUPID},
	{(u8 *) "UPGRADE", BLOADER_COMMAND_UPGRADE},
	{(u8 *) "RESTART", BLOADER_COMMAND_RESTART},
	{(u8 *) "RBRK", BLOADER_COMMAND_RBRK}
};

static const u8 ackStr[] = "ACK";
static const u8 errStr[] = "ERR";
static const u8 binStr[] = "BIN";
static const u8 commaStr[] = ",";

static u8 recvCmd[MAX_COMMAND_STR_SIZE];	/* Rx command buffer */
static u8 recvData[MAX_RX_DATA_SIZE];	/* Rx data buffer */
static u16 data_offset = 0;
static u16 cmd_offset = 0;

static u8 start_flag = 0;
static u8 cmd_flag = 0;
static u8 separator_clear = 0;	/* if the separator has been processed */
static u8 BIN_cmd_flag = 0;
static u16 BIN_bytes_to_read = 0;	/* number of bytes to read in BIN cmd */

static struct mybuf in = { "", 0 }, out1 = {
"", 0}, out2 = {
"", 0};

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

FUNCTION: reset_global

DESCRIPTION: reset all the global var

ARGUMENTS PASSED:
   None
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/
void reset_global(void)
{
	data_offset = 0;
	cmd_offset = 0;

	start_flag = 0;
	cmd_flag = 0;
	separator_clear = 0;

	BIN_cmd_flag = 0;
	BIN_bytes_to_read = 0;	/* number of bytes to read in BIN cmd */
}

/*==============================================================================

FUNCTION: parser_map_command

DESCRIPTION: Checks for the command in the table.

ARGUMENTS PASSED:
   table_ptr - Points to the command table to search
   table_size - Number of elements in table
   command_ptr - Points to the command string
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   Enumerated value of command

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/

static BLOADER_COMMANDS
map_command(BLOADER_COMMAND_TBL * table_ptr, u8 table_size, u8 * command_ptr)
{
	u8 i = 0;
	BLOADER_COMMANDS mapped_value = BLOADER_UNKNOWN_COMMAND;
	/*
	 * Default mapped value is "unknown" command,
	 * so if we don't find command we will return that.
	 */
	for (i = 0; i < table_size; i++, table_ptr++) {
		if (util_string_equal((u8 *) (table_ptr->cmd), command_ptr)) {
			/*
			 * Return the enum value for the matched command
			 */
			mapped_value = table_ptr->cmd_id;

			/*
			 * ABORT for loop upon match
			 */
			break;
		}
	}

	return (mapped_value);
}

/*==============================================================================

FUNCTION: handle_command

DESCRIPTION:  Performs the received command if valid or else respond with an err

ARGUMENTS PASSED:
   command_ptr  - Pointer to the command string
   data_ptr  -  Pointer to the data string  
                (will be NULL in case of command with no data)

REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/

static void handle_command(u8 * command_ptr, u8 * data_ptr)
{

	switch (map_command
		(&command_tbl[0], num_elements(command_tbl), command_ptr)) {
	case BLOADER_COMMAND_ADDR:
		{
			handle_command_ADDR(data_ptr);
		}
		break;

	case BLOADER_COMMAND_BIN:
		{
			handle_command_BIN(data_ptr);
		}
		break;

	case BLOADER_COMMAND_POWER_DOWN:
		{
			handle_command_PWR_DOWN(data_ptr);
		}
		break;

	case BLOADER_COMMAND_RQCS:
		{
			handle_command_RQCS(data_ptr);
		}
		break;

	case BLOADER_COMMAND_RQHW:
		{
			handle_command_RQHW(data_ptr);
		}
		break;

	case BLOADER_COMMAND_RQRC:
		{
			handle_command_RQRC(data_ptr);
		}
		break;

	case BLOADER_COMMAND_RQVN:
		{
			handle_command_RQVN(data_ptr);
		}
		break;

	case BLOADER_COMMAND_JUMP:
		{
			handle_command_JUMP(data_ptr);
		}
		break;

	default:
		{
			/* bad command */
			parse_err_response(BLOADER_ERR_UNKNOWN_COMMAND);
		}
		break;
	}
}

/*==============================================================================
GLOBAL FUNCTIONS
==============================================================================*/

/*==============================================================================

FUNCTION: parser_send_packet

DESCRIPTION: Form and send a completed packet to the Host

ARGUMENTS PASSED:
   command_ptr  - Pointer to the command string
   data_ptr  - Pointer to the data string  
               (will be NULL in case of command without data)

REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   TRUE -- success send the data to buffer
   FALSE -- some error occurs

PRE-CONDITIONS:
   command_ptr and data_ptr are NUL terminated.

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   Never exits

==============================================================================*/
BOOL parse_send_packet(u8 * command_ptr, u8 * data_ptr)
{
	u16 i = 0;
	/* u8 empty_packet[] = {0x00, 0x00}; */
	struct mybuf *out;
	u8 *temp_ptr;

	out = get_buf(1);
	if (!out)		/* can't get buffer */
		return FALSE;

#ifdef ZQ_DEBUG
	PRINT_STR("parse_send_packet is called\n");
#endif
	/* save the original command */
	temp_ptr = command_ptr;

	/* Attach the starting control character first */
	out->buf[i++] = STX;

	/* Load in the response command field */
	while ((*temp_ptr != NUL) && (i < MAX_RESPONSE_SIZE)) {
		out->buf[i++] = *(temp_ptr++);
	}

	/* check to see if any data attached to the command */
	if (data_ptr != NULL) {
		/* there is data, put a marker between command and data */
		out->buf[i++] = RS;
		/* Now load in the data field */
		while ((*data_ptr != NUL) && (i < MAX_RESPONSE_SIZE)) {
			out->buf[i++] = *data_ptr++;
		}
	}
	/* Attach the end control character to the respond packet */
	out->buf[i++] = ETX;
	/* Mark the end of the packet string */
	out->buf[i] = NUL;

	out->len = i;

	return ezx_usb_xmit(out);
}

/*==============================================================================

FUNCTION: parse_data

DESCRIPTION: Event handler and message parsing.  Fully parses BIN command

ARGUMENTS PASSED:
   None

REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   None

PRE-CONDITIONS:
   Transport has already been initialized

POST-CONDITIONS:
   None

IMPORTANT NOTES:
   Never exits

==============================================================================*/
void parse_data(void)
{
	/* the data from usb is stored in the struct "in" */
	u16 bytesRecv = in.len;	/* number of bytes received per reading */
	u16 sumOfBytesRecv = 0;	/* number of bytes read totally */

	u8 *pRecvData = in.buf;	/* pointer to data from USB */
	u8 *pCurrentPtr = pRecvData;	/* pointer to cur data position */

	u8 *pData = NULL;	/* pointer to the data packet buffer */
	u8 *pCommand = recvCmd;	/* pointer to the command buffer */
	u16 i = 0;

#ifdef ZQ_DEBUG
	PRINT_STR("parse_data is called\n");
#endif

	if (bytesRecv == 0)
		return;

	/* the start bit has not been found yet */
	if (start_flag == 0) {
		/* throw out all data read until an STX is found */
		while ((*(pCurrentPtr++) != STX) && (bytesRecv != 0)) {
			bytesRecv--;
		}		/* while STX not found */

		if (bytesRecv == 0)
			return;
		else {
			/* found the start bit, set the start flag */
			start_flag = 1;
			/* decrement count by one for STX */
			bytesRecv--;
		}
	}

	/* the command has not been parsed yet */
	if (cmd_flag == 0) {
		if (bytesRecv != 0) {
			i = 0;
			pCommand = recvCmd + cmd_offset;
			/* copy remainder of the first read until RS or ETX is found */
			while ((*pCurrentPtr != ETX) && (*pCurrentPtr != RS)) {
				*(pCommand++) = *(pCurrentPtr++);
				i++;
				bytesRecv--;
				/* if out of data, return */
				if (bytesRecv == 0) {
					cmd_offset += i;
					return;
				}
			}	/* while not found ETX or RS */

			/* done reading in command, terminate it */
			*pCommand = NUL;
			/* set the command flag */
			cmd_flag = 1;
		} else
			return;
	}

	/* The first packet that contains the data */
	if (separator_clear == 0) {
		/* The packet only contains command */
		if (*pCurrentPtr == ETX) {
			pData = NULL;
			handle_command(recvCmd, pData);
			reset_global();
			return;
		}

		/* The packet contains data */
		else if (*pCurrentPtr == RS) {
			/* set up data buffer */
			pData = recvData;
			sumOfBytesRecv = 0;
			/* skip RS and go into data */
			pCurrentPtr++;
			bytesRecv--;
			separator_clear = 1;

			/* save all read bytes into data buffer */
			while (bytesRecv != 0) {
				bytesRecv--;
				*(pData++) = *(pCurrentPtr++);
				sumOfBytesRecv++;
				data_offset++;
			}	/* while data to save */

			/* if NOT BIN command, but still has DATA field */
			if (!util_string_equal((u8 *) binStr, recvCmd)) {
				pData = recvData;
				/* scan for end of data */
				while ((sumOfBytesRecv != 0)
				       && (*pData != ETX)) {
					pData++;
					sumOfBytesRecv--;
				}
				/* check if ETX was found */
				if (sumOfBytesRecv != 0) {
					/* ETX found */
					/* skip ETX */
					*pData = NUL;
					pData = recvData;
					/* handle the command and data */
					handle_command(recvCmd, pData);
					reset_global();
					return;
				} else	/* ETX not found yet */
					return;
			}
			/* if the command is not BIN */
			else {	/* is BIN command */

				BIN_cmd_flag = 1;
				if (sumOfBytesRecv < MAX_DATA_FIELD_SIZE)
					return;
				else {
					/* determine the number of bytes to read */
					BIN_bytes_to_read =
					    ((recvData[BIN_DATA_SIZE_MSB]
					      << SHIFT_MSB) +
					     (recvData[BIN_DATA_SIZE_LSB]));

					/* check for a valid data packet size */
					if ((BIN_bytes_to_read <
					     MIN_BIN_PACKET_SIZE)
					    || (BIN_bytes_to_read >
						MAX_BIN_PACKET_SIZE)) {
						/* send Invalid Packet Size Error */
						parse_err_response
						    (BLOADER_ERR_PACKET_SZ_INVALID);
						reset_global();
						return;
					} else {	/* data size is ok */

						/* 
						 * adjust size of bytes to read, one byte for 
						 * checksum, one byte for ETX and 
						 * MAX_DATA_FIELD_SIZE for data count 
						 */
						BIN_bytes_to_read +=
						    (MAX_DATA_FIELD_SIZE + 2);

						/* has read all of the data */
						if (data_offset >=
						    BIN_bytes_to_read) {
							/* check that BIN command was good */
							if (recvData
							    [BIN_bytes_to_read -
							     1] == ETX) {
								recvData
								    [BIN_bytes_to_read
								     - 1] = NUL;
								handle_command
								    (recvCmd,
								     recvData);
								reset_global();
								return;
							} else {
								/* 
								 * ETX was not where it was supposed to be 
								 * in BIN command,send Invalid Packet Size 
								 * Error
								 */
								parse_err_response
								    (BLOADER_ERR_PACKET_SZ_INVALID);
							}
						} else	/* need to read more data */
							return;
					}	/* BIN data size is OK */
				}	/* sumOfBytesRecv < MAX_DATA_FIELD_SIZE */
			}	/* is BIN command */
		}		/* The packet contains data field */
	}
	/* if (data_offset == 0) */
	else {			/* separator_clear != 0, not the first enter of this function */

		if (BIN_cmd_flag != 1) {
			/* other commands */
			pData = recvData + data_offset;
			sumOfBytesRecv = 0;

			while (bytesRecv != 0) {
				bytesRecv--;
				*(pData++) = *(pCurrentPtr++);
				sumOfBytesRecv++;
			}
			data_offset += sumOfBytesRecv;
			pData -= sumOfBytesRecv;	/* only need to check the received data */
			while ((sumOfBytesRecv != 0) && (*pData != ETX)) {
				pData++;
				sumOfBytesRecv--;
			}
			if (sumOfBytesRecv != 0) {
				*pData = NUL;
				pData = recvData;
				handle_command(recvCmd, pData);
				reset_global();
				return;
			} else	/* not reach the end of the data yet */
				return;
		}
		/* is not BIN command */
		else {		/* is BIN command */

			pData = recvData + data_offset;
			sumOfBytesRecv = 0;

			while (bytesRecv != 0) {
				bytesRecv--;
				*(pData++) = *(pCurrentPtr++);
				sumOfBytesRecv++;
			}
			data_offset += sumOfBytesRecv;

			if (data_offset < MAX_DATA_FIELD_SIZE)
				return;
			else {
				if (BIN_bytes_to_read == 0) {
					/* determine the number of bytes to read */
					BIN_bytes_to_read =
					    ((recvData[BIN_DATA_SIZE_MSB]
					      << SHIFT_MSB) +
					     (recvData[BIN_DATA_SIZE_LSB]));
					/* check for a valid data packet size */
					if ((BIN_bytes_to_read <
					     MIN_BIN_PACKET_SIZE)
					    || (BIN_bytes_to_read >
						MAX_BIN_PACKET_SIZE)) {
						/* send Invalid Packet Size Error */
						parse_err_response
						    (BLOADER_ERR_PACKET_SZ_INVALID);
						reset_global();
						return;
					} else {	/* data size is ok */

						/* 
						 * adjust size of bytes to read, one byte for 
						 * checksum, one byte for ETX and 
						 * MAX_DATA_FIELD_SIZE for data count 
						 */
						BIN_bytes_to_read +=
						    (MAX_DATA_FIELD_SIZE + 2);
					}
				}

				/* has read all of the data */
				if (data_offset >= BIN_bytes_to_read) {
					/* check that BIN command was good */
					if (recvData[BIN_bytes_to_read - 1] ==
					    ETX) {
						recvData[BIN_bytes_to_read -
							 1] = NUL;
						handle_command(recvCmd,
							       recvData);
						reset_global();
						return;
					} else {
						/* 
						 * ETX was not where it was supposed to be 
						 * in BIN command,send Invalid Packet Size 
						 * Error
						 */
						parse_err_response
						    (BLOADER_ERR_PACKET_SZ_INVALID);
					}
				} /* Has read all of the BIN data */
				else	/* need to read more data */
					return;
			}	/* data_offset > MAX_DATA_FIELD_SIZE */
		}		/* is BIN command */
	}			/* data offset != 0 */
}				/* parse_data */

/*============================================================================*/

/*==============================================================================

FUNCTION: parser_err_response

DESCRIPTION:  Send an Error response back to the Host

ARGUMENTS PASSED:
   error_code  - Error code per Boot Interface Spec.

REFERENCE ARGUMENTS PASSED:
   None

RETURN VALUE:
   None

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/
void parse_err_response(u8 error_code)
{
	u8 error_code_str[2];

	error_code_str[0] = error_code;
	error_code_str[1] = NUL;

	parse_send_packet((u8 *) errStr, error_code_str);
}

/*==============================================================================

FUNCTION: parser_ack_response

DESCRIPTION: Sends an ACKnowledgement response back to the master.

      The ACK response is used as a positive response, that the slave successfully completed the
   command given by the master.

ARGUMENTS PASSED:
   data_ptr - pointer to data string to include with the ACK response.
   
REFERENCE ARGUMENTS PASSED:
   None
 
RETURN VALUE:
   None

PRE-CONDITIONS:
   rx_command is NUL terminated
   data_ptr   is NUL terminated
 
POST-CONDITIONS:
   None

IMPORTANT NOTES:
   None

==============================================================================*/
void parse_ack_response(u8 * data_ptr)
{
	/* ACK response data */
	u8 resp_data[MAX_ACK_RESPONSE_SIZE];
	u8 *pResp;
	u8 *command_ptr;
	u8 *end_ptr = &(resp_data[MAX_ACK_RESPONSE_SIZE - 1]);

	pResp = resp_data;
	command_ptr = recvCmd;

	/* copy last command to ACK response */
	while ((*command_ptr != NUL) && (pResp != end_ptr)) {
		*(pResp++) = *(command_ptr++);
	}

	/* if there is data, add a comma, then the data */
	if (data_ptr) {
		/* copy comma after command */
		*(pResp++) = commaStr[0];

		/* copy data into response */
		while ((*data_ptr != NUL) && (pResp != end_ptr)) {
			*(pResp++) = *(data_ptr++);
		}
	}

	/* end if data */
	/* terminate data */
	*pResp = NUL;

	parse_send_packet((u8 *) ackStr, resp_data);
}

/*==============================================================================
FUNCTION: get_buf

DESCRIPTION: Get buffer for USB transfer

ARGUMENTS PASSED:
   io - in or out
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   The mybuf struct

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/
struct mybuf *get_buf(int io)
{
	if (io == 1) {
		if (out1.len == 0)
			return &out1;
		if (out2.len == 0)
			return &out2;
	} else
		return &in;

	/*SerialOutputString("can't get buffer\n"); */
	return 0;
}

/*==============================================================================
FUNCTION: mybuf_reset

DESCRIPTION: reset the buffer

ARGUMENTS PASSED:
   None
    
REFERENCE ARGUMENTS PASSED:
   None
  
RETURN VALUE:
   The mybuf struct

DEPENDENCIES:
   None

SIDE EFFECTS:
   None

==============================================================================*/
void mybuf_reset(void)
{
	out1.len = 0;
	out2.len = 0;
	in.len = 0;
}

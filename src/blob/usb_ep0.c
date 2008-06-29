/*
 *  Copyright (C) Extenex Corporation 2001
 *  Copyright (C) Compaq Computer Corporation, 1998, 1999
 *  Copyright (C) Intrinsyc, Inc., 2002
 *  Copyright (C) Motorola Inc., 2005
 *  PXA USB controller driver - Endpoint zero management
 *
 *  Please see:
 *    linux/Documentation/arm/SA1100/SA1100_USB 
 *  for more info.
 *
 *  02-May-2002
 *   Frank Becker (Intrinsyc) - derived from sa1100 usb_ctl.c
 *  14-Dec-2005
 *   (Motorola) - Added change for motorola products
 *
 */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/types.h>
#include <blob/serial.h>

#include "linux/errno.h"
#include "pxa_usb.h"		/* public interface */
#include "usb_ctl_bvd.h"	/* private stuff */
#include "usb_ep0_bvd.h"
//#include "bulverde.h"
extern void msleep(unsigned int msec);
extern void memcpy(void *desc, const void *src, size_t n);
#if DEBUG
static unsigned int usb_debug = DEBUG;
#else
#define usb_debug 0		/* gcc will remove all the debug code for us */
#endif

// 1 == lots of trace noise,  0 = only "important' stuff
#define VERBOSITY 0

enum { true = 1, false = 0 };
typedef int bool;
#ifndef MIN
#define MIN( a, b ) ((a)<(b)?(a):(b))
#endif

#if 1 && !defined( ASSERT )
#  define ASSERT(expr) \
if(!(expr)) { \
	printk( "Assertion failed! %s,%s,%s,line=%d\n",\
#expr,__FILE__,__FUNCTION__,__LINE__); \
}
#else
#  define ASSERT(expr)
#endif

#if VERBOSITY
#define PRINTKD(fmt, args...) printk( fmt , ## args)
#else
#define PRINTKD(fmt, args...)
#endif

static EP0_state ep0_state = EP0_IDLE;

/***************************************************************************
  Prototypes
 ***************************************************************************/
/* "setup handlers" -- the main functions dispatched to by the
   .. isr. These represent the major "modes" of endpoint 0 operation */
static void sh_setup_begin(void);	/* setup begin (idle) */
static void sh_write(void);	/* writing data */
static int read_fifo(usb_dev_request_t * p);
static void write_fifo(void);
static void get_descriptor(usb_dev_request_t * pReq);
static void queue_and_start_write(void *p, int req, int act);

/***************************************************************************
  Inline Helpers
 ***************************************************************************/

inline int type_code_from_request(__u8 by)
{
	return ((by >> 4) & 3);
}

/* print string descriptor */
static inline void psdesc(string_desc_t * p)
{
	int i;
	int nchars = (p->bLength - 2) / sizeof(__u16);
	//printk( "'" );
	for (i = 0; i < nchars; i++) {
		//printk( "%c", (char) p->bString[i] );
	}
	//printk( "'\n" );
}

#if VERBOSITY
/* "pcs" == "print control status" */
static inline void pcs(void)
{
/*
	__u32 foo = UDCCS0;

	printk( "%08x: %s %s %s %s %s %s\n",
			foo,
			foo & UDCCS0_SA   ? "SA" : "",
			foo & UDCCS0_OPR  ? "OPR" : "",
			foo & UDCCS0_RNE  ? "RNE" : "",
			foo & UDCCS0_SST  ? "SST" : "",
			foo & UDCCS0_FST  ? "FST" : "",
			foo & UDCCS0_DRWF ? "DRWF" : ""
	      );
*/
}
static inline void preq(usb_dev_request_t * pReq)
{
	static char *tnames[] = { "dev", "intf", "ep", "oth" };
	static char *rnames[] = { "std", "class", "vendor", "???" };
	char *psz;
	switch (pReq->bRequest) {
	case GET_STATUS:
		psz = "get stat";
		break;
	case CLEAR_FEATURE:
		psz = "clr feat";
		break;
	case SET_FEATURE:
		psz = "set feat";
		break;
	case SET_ADDRESS:
		psz = "set addr";
		break;
	case GET_DESCRIPTOR:
		psz = "get desc";
		break;
	case SET_DESCRIPTOR:
		psz = "set desc";
		break;
	case GET_CONFIGURATION:
		psz = "get cfg";
		break;
	case SET_CONFIGURATION:
		psz = "set cfg";
		break;
	case GET_INTERFACE:
		psz = "get intf";
		break;
	case SET_INTERFACE:
		psz = "set intf";
		break;
	case SYNCH_FRAME:
		psz = "synch frame";
		break;
	default:
		psz = "unknown";
		break;
	}
	/* printk( "- [%s: %s req to %s. dir=%s]\n", psz,
	   rnames[ (pReq->bmRequestType >> 5) & 3 ],
	   tnames[ pReq->bmRequestType & 3 ],
	   ( pReq->bmRequestType & 0x80 ) ? "in" : "out" );
	 */
}

#else
static inline void pcs(void)
{
}
static inline void preq(usb_dev_request_t * x)
{
}
#endif

/***************************************************************************
  Globals
 ***************************************************************************/
static const char pszMe[] = "usbep0: ";

/* pointer to current setup handler */
static void (*current_handler) (void) = sh_setup_begin;

/* global write struct to keep write
   ..state around across interrupts */
static struct {
	unsigned char *p;
	int bytes_left;
} wr;

/***************************************************************************
  Public Interface
 ***************************************************************************/

/* reset received from HUB (or controller just went nuts and reset by itself!)
   so udc core has been reset, track this state here  */
void ep0_reset(void)
{
	/* reset state machine */
	current_handler = sh_setup_begin;
	wr.p = 0;
	wr.bytes_left = 0;
	usbd_info.address = 0;
}

/* handle interrupt for endpoint zero */
void ep0_int_hndlr(void)
{
	pcs();
	(*current_handler) ();
}

/***************************************************************************
  Setup Handlers
 ***************************************************************************/
/*
 * sh_setup_begin()
 * This setup handler is the "idle" state of endpoint zero. It looks for OPR
 * (OUT packet ready) to see if a setup request has been been received from the
 * host. 
 *
 */
static void sh_setup_begin(void)
{
	usb_dev_request_t req;
	int request_type;
	int n;
	__u32 cs_reg_in = UDCCSR0;

/*	SerialOutputString("in sh_setup_begin UDCCSR0=");SerialOutputDec( UDCCSR0 );SerialOutputString("\n");
 */
	/* Be sure out packet ready, otherwise something is wrong */
	if ((cs_reg_in & UDCCSR0_OPC) == 0) {
		/* we can get here early...if so, we'll int again in a moment  */
		/*      SerialOutputString("setup begin: no OUT packet avaible. Exting\n");     */
		goto sh_sb_end;
	}

	if (((cs_reg_in & UDCCSR0_SA) == 0) && (ep0_state == EP0_IN_DATA_PHASE)) {
/*		SerialOutputString(" premature status\n"); */

		/* premature status, reset tx fifo and go back to idle state */
		UDCCSR0 = UDCCSR0_OPC | UDCCSR0_FTF;

		ep0_state = EP0_IDLE;
		return;
	}

	if ((UDCCSR0 & UDCCSR0_RNE) == 0) {
		/* zero-length OUT? */
/*		SerialOutputString(" zero-length OUT\n"); */
		goto sh_sb_end;
	}

	/* read the setup request */
	n = read_fifo(&req);
	if (n != sizeof(req)) {
/*		SerialOutputString("req != request\n"); */
		/* force stall, serviced out */
		UDCCSR0 = UDCCSR0_FST;
		goto sh_sb_end;
	}

	/* Is it a standard request? (not vendor or class request) */
	request_type = type_code_from_request(req.bmRequestType);
	if (request_type != 0) {
		if (usb_debug)
			SerialOutputString("unsupported bmRequestType\n");
		goto sh_sb_end;
	}

	/* Handle it */
	switch (req.bRequest) {
	case SET_ADDRESS:
/*			SerialOutputString("SET_ADDRESS\n");*/
		break;

	case GET_DESCRIPTOR:
/*			SerialOutputString("GET_DESCRIPTOR\n"); */
		get_descriptor(&req);
		break;

	case SET_INTERFACE:
		if (usb_debug)
			SerialOutputString("SET_INTERFACE\n");
		break;

	case SET_DESCRIPTOR:
		if (usb_debug)
			SerialOutputString("SET_DESCRIPTOR\n");
		break;

	case SET_CONFIGURATION:
		if (usb_debug)
			SerialOutputString("SET_CONFIGURATION\n");
/*
 * FIXME: Something is not quite right here... I only ever get a 
 * de-configure from the host. Ignoring it for now, since usb
 * ethernet won't do anything unless usb is 'configured'.
 *
 */
#if 0
		switch (req.wValue) {
		case 0:
			/* configured */
			usbctl_next_state_on_event(kEvConfig);
			break;
		case 1:
			/* de-configured */
			usbctl_next_state_on_event(kEvDeConfig);
			break;
		default:
			PRINTKD
			    ("%sSET_CONFIGURATION: unknown configuration value (%d)\n",
			     pszMe, req.wValue);
			break;
		}
#endif
		break;
	default:
		break;
	}			/* switch( bRequest ) */
/*
	SerialOutputString("exit from sh_setup_begin UDCCSR0=");SerialOutputDec( UDCCSR0 );SerialOutputString("\n");
*/
      sh_sb_end:
	return;
}

/*
 * sh_write()
 * 
 * Due to UDC bugs we push everything into the fifo in one go.
 * Using interrupts just didn't work right...
 * This should be ok, since control request are small.
 */
static void sh_write()
{
	do {
		write_fifo();
	} while (ep0_state != EP0_END_XFER);
}

/***************************************************************************
  Other Private Subroutines
 ***************************************************************************/
/*
 * queue_and_start_write()
 * data == data to send
 * req == bytes host requested
 * act == bytes we actually have
 *
 * Sets up the global "wr"-ite structure and load the outbound FIFO 
 * with data.
 *
 */
static int req_flag = 0;	//1: data sent == request's length, do not send 0 package
static void queue_and_start_write(void *data, int req, int act)
{
	wr.p = (unsigned char *)data;
	wr.bytes_left = MIN(act, req);
	if (wr.bytes_left == req)
		req_flag = 1;
	ep0_state = EP0_IN_DATA_PHASE;
	sh_write();

	return;
}

/*
 * write_fifo()
 * Stick bytes in the endpoint zero FIFO.
 *
 */
static void write_fifo(void)
{
	int bytes_this_time = MIN(wr.bytes_left, EP0_FIFO_SIZE);
	int bytes_written = 0;
	__u32 tmp[EP0_FIFO_SIZE >> 2];
	__u8 *pData;

/*
	SerialOutputString("byts_this_time=");
	SerialOutputHex( bytes_this_time );
	SerialOutputString("\n");
*/
	/* something goes poopy if I dont wait here ... */
	msleep(1);

	if ((__u32) wr.p & 0x03) {
		memcpy((void *)tmp, wr.p, bytes_this_time);
		pData = (__u8 *) tmp;
	} else {
		pData = (__u8 *) wr.p;
	}

	while (bytes_this_time >= 4) {
		UDCDR0 = *(__u32 *) pData;
		pData += 4;
		bytes_written += 4;
		bytes_this_time -= 4;
	}

	if (bytes_this_time != 0) {
		volatile unsigned char *pReg =
		    (volatile unsigned char *)&UDCDR0;
		while (bytes_this_time > 0) {
			*pReg = *pData;
			pData++;
			bytes_written++;
			bytes_this_time--;
		}
	}

	wr.bytes_left -= bytes_written;
	wr.p += bytes_written;
	usbd_info.stats.ep0_bytes_written += bytes_written;

	if (wr.bytes_left == 0) {
		int count;
		wr.p = NULL;

		//ep0_state = EP0_END_XFER;
		current_handler = sh_setup_begin;

		if (req_flag == 1) {
			ep0_state = EP0_END_XFER;	//no data left and len=request's, no 0 package.
			req_flag = 0;
		}
		if (bytes_written < EP0_FIFO_SIZE) {
//                      SerialOutputString("set IPR\n");
			ep0_state = EP0_END_XFER;
			UDCCSR0 = UDCCSR0_IPR;
//                      SerialOutputString("UDCCSR0=");SerialOutputDec(UDCCSR0);SerialOutputString("\n");
		}

		/* Wait until we get to status-stage, then ack.
		 *
		 * When the UDC sets the UDCCS0[OPR] bit, an interrupt
		 * is supposed to be generated (see 12.5.1 step 14ff, PXA Dev Manual).   
		 * That approach didn't work out. Usually a new SETUP command was
		 * already in the fifo. I tried many approaches but was always losing 
		 * at least some OPR interrupts. Thus the polling below...
		 */
		count = 1000;
		do {
//                      SerialOutputString("UDCCSR0=");SerialOutputDec(UDCCSR0);SerialOutputString("\n");
#if 0
			if ((UDCCSR0 & SETUP_READY) == UDCCSR0_OPC) {
				UDCCSR0 = UDCCSR0_OPC;
				SerialOutputString("Clear OPC.\n");
				break;
			} else if ((UDCCSR0 & SETUP_READY) == SETUP_READY) {
				SerialOutputString
				    ("Next Setup command arrive.\n");
				return;
			}
#endif
			if (UDCCSR0 & UDCCSR0_OPC) {
				UDCCSR0 = UDCCSR0_OPC;
//                              SerialOutputString("Clear OPC.\n");
				break;
			}

			count--;
		} while (count);

		if (count == 0) {
			if (usb_debug)
				SerialOutputString
				    ("waiting for host's ack. But time out.\n");
		}
	}
/*	msleep(1);*/

}

/*
 * read_fifo()
 * Read bytes out of FIFO and put in request.
 * Called to do the initial read of setup requests
 * from the host. Return number of bytes read.
 *
 */
static int read_fifo(usb_dev_request_t * request)
{
	int bytes_read = 0;
	__u32 *pOut = (__u32 *) request;

	int udccs0 = UDCCSR0;

	if ((__u32) pOut & 0x03) {
		//      SerialOutputString("Align error\n");
	}

	if ((udccs0 & SETUP_READY) == SETUP_READY) {
		/* ok it's a setup command */
		while (UDCCSR0 & UDCCSR0_RNE) {
			if (bytes_read >= sizeof(usb_dev_request_t)) {
				/* We've already read enought o fill usb_dev_request_t.
				 * Our tummy is full. Go barf... 
				 */
				usbd_info.stats.ep0_fifo_read_failures++;
				break;
			}
			/* We may write some data into an unallocated area. */
			*pOut++ = UDCDR0;
			bytes_read += 4;
		}
	}

	/* clear SA & OPR */
	UDCCSR0 = SETUP_READY;
/*
	SerialOutputString(" clear SA & OPR, read_fifo UDCCSR0=");SerialOutputDec( UDCCSR0 );SerialOutputString("\n");
*/
	usbd_info.stats.ep0_bytes_read += bytes_read;
	return bytes_read;
}

/*
 * get_descriptor()
 * Called from sh_setup_begin to handle data return
 * for a GET_DESCRIPTOR setup request.
 *
 * +-----+------------------------------------------------+-----------------+
 * | dev | cfg1 | intf 1 | ep 1..N | intf 2 | ep 1..N |...| cfg2 .......... |
 * +-----+------------------------------------------------+-----------------+
 */
static void get_descriptor(usb_dev_request_t * pReq)
{
	string_desc_t *pString;

	desc_t *pDesc = pxa_usb_get_descriptor_ptr();
	int type = pReq->wValue >> 8;
	int idx = pReq->wValue & 0xFF;

	switch (type) {
	case USB_DESC_DEVICE:
/*			SerialOutputString("Device\n");*/
		/* return device descritpor */
		queue_and_start_write(&pDesc->dev,
				      pReq->wLength, pDesc->dev.bLength);
		break;

		// return config descriptor buffer, cfg, intf 1..N,  ep 1..N
	case USB_DESC_CONFIG:
		{
			int i, len;
			config_desc_t *cfg = (config_desc_t *) (pDesc->cdb);
			len = 0;
			for (i = 0; i < pDesc->dev.bNumConfigurations; i++) {
				len += __le16_to_cpu(cfg->wTotalLength);
				cfg = (config_desc_t *) ((unsigned char *)cfg
							 +
							 __le16_to_cpu(cfg->
								       wTotalLength));
			}

			queue_and_start_write(pDesc->cdb, pReq->wLength, len);
		}
		break;

		// not quite right, since doesn't do language code checking
	case USB_DESC_STRING:
/*
			SerialOutputString("String\n");
*/
		pString = pxa_usb_get_string_descriptor(idx);
		if (pString) {
			if (idx != 0) {	// if not language index
				psdesc(pString);
			}
			queue_and_start_write(pString,
					      pReq->wLength, pString->bLength);
		} else {
		}
		break;

		/*
		   case USB_DESC_INTERFACE:
		   SerialOutputString("interface\n");
		   for( i = 0; i < pDesc->intf_num; i++) {
		   if ( idx == pDesc->intf[i].bInterfaceNumber ) {
		   queue_and_start_write( &pDesc->intf[i],
		   pReq->wLength,
		   pDesc->intf[i].bLength );
		   }
		   }
		   break;

		   case USB_DESC_ENDPOINT: 
		   SerialOutputString("Endpoint\n");
		   for( i = 0; i < pDesc->ep_num; i++) {
		   if ( idx == (0x0F & pDesc->ep[i].bEndpointAddress)) {
		   queue_and_start_write( &pDesc->ep[i],
		   pReq->wLength,
		   pDesc->ep[i].bLength );
		   }
		   }
		   break;
		 */

	default:
		break;

	}
}

/* end usb_ep0.c - who needs this comment? */

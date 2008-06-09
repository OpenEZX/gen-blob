/*
 *  Copyright (C) Compaq Computer Corporation, 1998, 1999
 *  Copyright (C) Extenex Corporation 2001
 *  Copyright (C) Intrinsyc, Inc., 2002
 *  Copyright (C) Motorola Inc., 2005
 *  usb_ctl.h
 *
 *  PRIVATE interface used to share info among components of the PXA USB
 *  core: usb_ctl, usb_ep0, usb_recv and usb_send. Clients of the USB core
 *  should use pxa_usb.h.
 *
 *  02-May-2002 
 *   Frank Becker (Intrinsyc) - derived from sa1100 usb_ctl.h
 *  14-Dec-2005
 *   (Motorola) Added change for motorola products
 *
 */
#include "pxa_usb.h"
#ifndef _USB_CTL_H
#define _USB_CTL_H

/* Interrupt mask bits and UDC enable bit */
#define UDCCR_MASK_BITS         (UDCCR_UDE | UDCCR_UDR | UDCCR_EMCE | UDCCR_SMAC )

/*
 * These states correspond to those in the USB specification v1.0
 * in chapter 8, Device Framework.
 */
enum { 
	USB_STATE_NOTATTACHED	=0,
	USB_STATE_ATTACHED	=1,
	USB_STATE_POWERED	=2,
	USB_STATE_DEFAULT	=3,
	USB_STATE_ADDRESS	=4,
	USB_STATE_CONFIGURED	=5,
	USB_STATE_SUSPENDED	=6
};

struct usb_stats_t {
	 unsigned long ep0_fifo_write_failures;
	 unsigned long ep0_bytes_written;
	 unsigned long ep0_fifo_read_failures;
	 unsigned long ep0_bytes_read;
};

struct usb_info_t
{
	 char * client_name;
	 int state;
	 unsigned char address;
	 struct usb_stats_t stats;
};

/* in usb_ctl.c */
extern struct usb_info_t usbd_info;

/*
 * Function Prototypes
 */
enum { 
	kError		=-1,
	kEvSuspend	=0,
	kEvReset	=1,
	kEvResume	=2,
	kEvAddress	=3,
	kEvConfig	=4,
	kEvDeConfig	=5 
};
int usbctl_next_state_on_event( int event );

/* endpoint zero */
void ep0_reset(void);
void ep0_int_hndlr(void);

/* receiver */
void ep2_state_change_notify( int new_state );
int  ep2_recv(char *buf, int len, usb_callback_t callback);
int  ep2_init(int chn);
void ep2_int_hndlr(int status);
void ep2_reset(void);
void ep2_stall(void);
void ep7_state_change_notify( int new_state );
int  ep7_recv(char *buf, int len, usb_callback_t callback);
int  ep7_init(int chn);
void ep7_int_hndlr(int status);
void ep7_reset(void);
void ep7_stall(void);

/* xmitter */
void ep1_state_change_notify( int new_state );
int  ep1_send(char *buf, int len, usb_callback_t callback);
void ep1_reset(void);
int  ep1_init(int chn);
void ep1_int_hndlr(int status);
void ep1_stall(void);
void ep6_state_change_notify( int new_state );
int  ep6_send(char *buf, int len, usb_callback_t callback);
void ep6_reset(void);
int  ep6_init(int chn);
void ep6_int_hndlr(int status);
void ep6_stall(void);
#endif /* _USB_CTL_H */

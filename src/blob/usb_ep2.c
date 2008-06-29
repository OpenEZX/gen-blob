/*
 * Generic receive layer for the PXA USB client function
 *
 * This code was loosely inspired by the original version which was
 * Copyright (c) Compaq Computer Corporation, 1998-1999
 * Copyright (c) 2001 by Nicolas Pitre
 *
 * This program is free
 software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * 02-May-2002
 *   Frank Becker (Intrinsyc) - derived from sa1100 usb_recv.c
 * 
 * TODO: Add support for DMA.
 *
 */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>

#include <linux/errno.h>
#include "pxa_usb.h"
#include "usb_ctl.h"
//#include "bulverde.h"

#if DEBUG
static unsigned int usb_debug = DEBUG;
#else
#define usb_debug 0		/* gcc will remove all the debug code for us */
#endif

static char *ep2_buf;
static int ep2_len;
static int ep2_remain;
static usb_callback_t ep2_callback;
static int rx_pktsize;

static void ep2_start(void)
{
	/* disable DMA, RPC & SST should not be cleared */

	/* enable interrupts for endpoint 2 (bulk out) */
	UDCICR0 = 0x3f;
}

static void ep2_done(int flag)
{
	int size = ep2_len - ep2_remain;

	if (!ep2_len)
		return;

	ep2_len = 0;
	if (ep2_callback) {
		ep2_callback(flag, size);
	}
}

void ep2_state_change_notify(int new_state)
{
}

void ep2_stall(void)
{

//      UDCCSRB |= UDCCSR_FST;
}

int ep2_init(int chn)
{
	/* FIXME */
	rx_pktsize = 64;
	ep2_done(-EAGAIN);
	return 0;
}

void ep2_reset(void)
{
	/* FIXME */
	rx_pktsize = 64;

//      UDCCSRB &= ~UDCCSR_FST;
	ep2_done(-EINTR);
}

void ep2_int_hndlr(int udcsr)
{
	int status = UDCCSRB;

	if ((status & (UDCCSR_PC | UDCCSR_SP)) == UDCCSR_SP) {
		/* zero-length packet */

		UDCCSRB |= UDCCSR_PC;
	}

	if (status & UDCCSR_PC) {
		int len;
		__u32 *buf = (__u32 *) (ep2_buf + ep2_len - ep2_remain);

		/* bytes in FIFO */
		len = UDCBCRB & 0x3ff;

		if (len > ep2_remain) {
			/* FIXME: if this happens, we need a temporary overflow buffer */
			//printk("usb_recv: Buffer overwrite warning...\n");
			len = ep2_remain;
		}

		/* read data out of fifo */
		while ((UDCCSRB & UDCCSR_BNEF) != 0) {
			*buf++ = UDCDRB;
		}

		/* ack RPC - FIXME: '|=' we may ack SST here, too */
		UDCCSRB |= UDCCSR_PC;
		ep2_remain -= len;
		ep2_done((len) ? 0 : -EPIPE);
	}

	return;
}

int ep2_recv(char *buf, int len, usb_callback_t callback)
{

	if (ep2_len)
		return -EBUSY;

	ep2_buf = buf;
	ep2_len = len;
	ep2_callback = callback;
	ep2_remain = len;
	ep2_start();

	return 0;
}

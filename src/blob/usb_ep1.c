/*
 * Generic xmit layer for the PXA USB client function
 * This code was loosely inspired by the original version which was
 * Copyright (c) Compaq Computer Corporation, 1998-1999
 * Copyright (c) 2001 by Nicolas Pitre
 * Copyright (C) 2005 Motorola Inc.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * 02-May-2002
 *   Frank Becker (Intrinsyc) - derived from sa1100 usb_send.c
 *
 * TODO: Add support for DMA.
 *
 * 14-Dec-2005
 *   (Motorola) - Added change for motorola products
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

static char *ep1_buf;
static int ep1_len;
static int ep1_remain;
static usb_callback_t ep1_callback;
static int tx_pktsize;

/* device state is changing, async */
void ep1_state_change_notify(int new_state)
{
}

/* set feature stall executing, async */
void ep1_stall(void)
{

//      UDCCSRA |= UDCCSR_FST;
}

static void ep1_send_packet(void)
{
	__u8 *pBuf = (__u8 *) (ep1_buf + ep1_len - ep1_remain);
	__u32 *pData;
	int OutSize;
	//      int tmp,i;
	int i = 0;
	int whole_4_cnt = 0;	/* counter for u32 */
	int mod_4_cnt = 0;	/* count for u8 */

	if (tx_pktsize > ep1_remain)
		OutSize = ep1_remain;
	else
		OutSize = tx_pktsize;
	mod_4_cnt = OutSize % 4;
	whole_4_cnt = OutSize - mod_4_cnt;

	pData = (__u32 *) pBuf;
	/* We may access an unallocated area. */
	//      for( i=0; i<OutSize; i+=4)
	for (i = 0; i < whole_4_cnt; i += 4) {
		UDCDRA = *pData++;
	}

	if (mod_4_cnt) {
		pBuf += whole_4_cnt;
		//    tmp = OutSize&0x03;
		for (i = 0; i < mod_4_cnt; i++) {

#define EP_A_REG_ADDR  (0x40600304)

			//      UDCDRA = *pBuf++;
			__u8 *reg = (__u8 *) EP_A_REG_ADDR;
			*reg = *pBuf++;
		}

	}
	UDCCSRA = UDCCSR_PC;
	ep1_remain -= OutSize;

	if (OutSize < tx_pktsize) {
		/* short packet */
		UDCCSRA = UDCCSR_SP;
	}
//      msleep(0);
//      ep1_remain -= OutSize;
#if 0				/* seems bad in rdl, remove it also in blob */
	/*  add intel patch, do zero package... */
	if (ep1_remain == 0 && OutSize == tx_pktsize) {
		UDCCSRA = UDCCSR_SP;
	}
#endif

}

static void ep1_start(void)
{
	if (!ep1_len)
		return;
	UDCICR0 = 0x3f;
	ep1_send_packet();
}

static void ep1_done(int flag)
{
	int size = ep1_len - ep1_remain;
	if (ep1_len) {
		ep1_len = 0;
		if (ep1_callback)
			ep1_callback(flag, size);
	}
}

int ep1_init(int chn)
{
	/* FIXME */
	tx_pktsize = 64;
	ep1_done(-EAGAIN);
	return 0;
}

void ep1_reset(void)
{
	/* FIXME */
	tx_pktsize = 64;

//      UDCCSRA = UDCCSR_FST;
	ep1_done(-EINTR);
}

void ep1_int_hndlr(int usir0)
{
	int status = UDCCSRA;
	if (ep1_remain != 0) {
		/* more data to go */
		ep1_start();
	} else {
		if (status & UDCCSR_PC) {
			UDCCSRA = UDCCSR_PC;
		}
		ep1_done(0);
	}
}

int ep1_send(char *buf, int len, usb_callback_t callback)
{

	if (usbd_info.state != USB_STATE_CONFIGURED) {

		return -ENODEV;
	}

	if (ep1_len) {
		return -EBUSY;
	}

	ep1_buf = buf;
	ep1_len = len;
	ep1_callback = callback;
	ep1_remain = len;
	ep1_start();

	return 0;
}

int ep1_xmitter_avail(void)
{
	if (usbd_info.state != USB_STATE_CONFIGURED)
		return -ENODEV;
	if (ep1_len)
		return -EBUSY;
	return 0;
}

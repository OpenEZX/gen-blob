/*
 * usb-mot1.c
 *
 * Motorola USB driver
 *
 *
 * Copyright (C) 2002 Motorola
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
 * Oct 30,2002 - (Motorola) Created
 *
 */
#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif

#include <blob/arch.h>
#include <blob/types.h>
#include <blob/serial.h>
#include <blob/util.h>

#include <pxa_usb.h>

#include "usb_ctl.h"
#include "bl_parser.h"
#ifdef ZQ_DEBUG
#include "bl_debug.h"
#endif

static int usb_rsize = 64;

#define VENDOR_ID 0x22B8
#define PRODUCT_ID 0xBEEF

static struct mybuf *cur_tx_buf = NULL;
static struct mybuf *next_tx_buf = NULL;
static struct mybuf *rx_buf = NULL;
static char sd_buf[2048];

static char *ezx_str[] = {
	"Motorola Inc.",
	"Motorola Ezx Bootloader",
	"Blank Ezx",		/* Not used */
	"Flashed Ezx",		/* Not used */
	"Motorola Ezx Flash",
	"Motorola Ezx AP Flash",
	"Motorola Flash",
	NULL
};

static void ezx_usb_open();
static void rx_callback(int flag, int size);
static void tx_callback(int flag, int size);

/* Blob USB driver for Bootloader */
void usb_mot_open(int flag)
{
	desc_t *pdesc = pxa_usb_get_descriptor_ptr();
	char *str = ezx_str[0];
	string_desc_t *sd = (string_desc_t *) sd_buf;
	int i = 0, index = 1;

	/* setup string descriptor, language identifier is already setten */
	while (str) {
		sd->bDescriptorType = USB_DESC_STRING;
		sd->bLength = 2 * strlen(str) + 2;

		i = 0;

		while ('\0' != str[i]) {
			sd->bString[i] = make_word_c(str[i]);
			i++;
		}

		pxa_usb_set_string_descriptor(index, sd);

		/* next... */
		sd = (string_desc_t *) ((unsigned char *)sd + sd->bLength);
		str = ezx_str[index];
		index++;
	}

	/* setup device descriptor */
	pdesc->dev.iManufacturer = 0x1;	/* Motorola Inc. */
	pdesc->dev.iProduct = 0x2;	/* Motorola Ezx Bootloader */
	pdesc->dev.idVendor = VENDOR_ID;
	pdesc->dev.idProduct = PRODUCT_ID;
	pdesc->dev.bNumConfigurations = 1;

	ezx_usb_open();
}

/* send data with EP1 */
BOOL ezx_usb_xmit(struct mybuf *out)
{
	int state = 0;

#ifdef ZQ_DEBUG
	PRINT_STR("ezx_usb_xmit is called \n");
#endif

	if (cur_tx_buf) {
		if (next_tx_buf) {	/* the buf is overflow */
			return FALSE;
		} else {
			/* the data is about to send, return true here */
			next_tx_buf = out;
			return TRUE;
		}
	} else {
		cur_tx_buf = out;
		state = ep1_send(out->buf, out->len, tx_callback);
		if (0 == state)
			return TRUE;
		else
			return FALSE;
	}

}

static void tx_callback(int flag, int size)
{
	if (cur_tx_buf) {
		cur_tx_buf->len = 0;
		cur_tx_buf = 0;
	} else {
		return;
	}

	if (next_tx_buf) {
		cur_tx_buf = next_tx_buf;
		next_tx_buf = 0;

		ep1_send(cur_tx_buf->buf, cur_tx_buf->len, tx_callback);
	}
}

static void rx_callback(int flag, int size)
{
	if (flag != 0)
		return;

	rx_buf->len = size;
#ifdef ZQ_DEBUG
	PRINT_STR("The recived data is: ");
	PRINT_STR(rx_buf->buf);
	PRINT_STR("\nabout to call parse_data()\n");
#endif

	parse_data();

	/* reset buffer */
	rx_buf->len = 0;
	memset(rx_buf->buf, 0, BUFSZ);
	ep2_recv(rx_buf->buf, usb_rsize, rx_callback);

	return;

}

static void ezx_usb_open()
{
	desc_t *pdesc = pxa_usb_get_descriptor_ptr();
	config_desc_t *cfg;
	intf_desc_t *intf;
	ep_desc_t *ep;
	cfg = (config_desc_t *) (pdesc->cdb);

	cfg->bLength = sizeof(config_desc_t);
	cfg->bDescriptorType = USB_DESC_CONFIG;
	cfg->wTotalLength = make_word_c(sizeof(config_desc_t) +
					sizeof(intf_desc_t) * 1 +
					sizeof(ep_desc_t) * 2);
	cfg->bNumInterfaces = 1;
	cfg->bConfigurationValue = 1;
	cfg->bmAttributes = USB_CONFIG_SELFPOWERED;
	cfg->MaxPower = USB_POWER(10);
	cfg->iConfiguration = 0x5;

	intf = (intf_desc_t *) (cfg + 1);
	intf->bLength = sizeof(intf_desc_t);
	intf->bDescriptorType = USB_DESC_INTERFACE;
	intf->bInterfaceNumber = 0;
	intf->bAlternateSetting = 0;
	intf->bNumEndpoints = 2;
	intf->bInterfaceClass = 0xFF;
	intf->bInterfaceSubClass = 0x32;
	intf->bInterfaceProtocol = 0xFF;
	intf->iInterface = 0x6;

	ep = (ep_desc_t *) (intf + 1);
	ep[0].bLength = sizeof(ep_desc_t);
	ep[0].bDescriptorType = USB_DESC_ENDPOINT;
	ep[0].bEndpointAddress = USB_EP_ADDRESS(1, USB_IN);
	ep[0].bmAttributes = USB_EP_BULK;
	ep[0].wMaxPacketSize = make_word(64);
	ep[0].bInterval = 0;

	ep[1].bLength = sizeof(ep_desc_t);
	ep[1].bDescriptorType = USB_DESC_ENDPOINT;
	ep[1].bEndpointAddress = USB_EP_ADDRESS(2, USB_OUT);
	ep[1].bmAttributes = USB_EP_BULK;
	ep[1].wMaxPacketSize = make_word(64);
	ep[1].bInterval = 0;

	mybuf_reset();
	/* hook callback */
	rx_buf = get_buf(0);
	rx_buf->len = 0;
	cur_tx_buf = 0;
	next_tx_buf = 0;
	ep2_recv(rx_buf->buf, usb_rsize, rx_callback);
	return;
}

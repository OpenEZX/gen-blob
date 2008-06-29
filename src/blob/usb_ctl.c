/*
 *  Copyright (C) Compaq Computer Corporation, 1998, 1999
 *  Copyright (C) Extenex Corporation, 2001
 *  Copyright (C) Intrinsyc, Inc., 2002
 *  Copyright (C) Motorola Inc., 2002, 2005, 2006
 *  PXA USB controller core driver.
 *
 *  This file provides interrupt routing and overall coordination
 *  of the endpoints.
 *
 *  Please see:
 *    linux/Documentation/arm/SA1100/SA1100_USB 
 *  for more info.
 *
 *  02-May-2002
 *   Frank Becker (Intrinsyc) - derived from sa1100 usb_ctl.c
 *  30-Oct-2002 
 *   (Motorola) - Changes made to support Bulverde part
 *  14-Dec-2005
 *   (Motorola) - Added change for motorola products
 *  12-Jan-2006
 *   (Motorola) - Added usb_mot_open function
 *
 */

#ifdef HAVE_CONFIG_H
# include <blob/config.h>
#endif
#include <blob/arch.h>
#include <blob/init.h>
#include <blob/types.h>
#include <blob/time.h>

#include "linux/errno.h"
#include "pxa_usb.h"
#include "usb_ctl_bvd.h"

//#define DEBUG 1

#if DEBUG
static unsigned int usb_debug = DEBUG;
#else
#define usb_debug 0		/* gcc will remove all the debug code for us */
#endif

//////////////////////////////////////////////////////////////////////////////
// Prototypes
//////////////////////////////////////////////////////////////////////////////

int usbctl_next_state_on_event(int event);
void udc_int_hndlr(int, void *);
static void initialize_descriptors(void);
static void soft_connect_hook(int enable);
static void udc_disable(void);
static void udc_enable(void);
extern void usb_mot_open(int flag);
extern void memset(void *start, unsigned char val, unsigned long size);
#if CONFIG_PROC_FS
#define PROC_NODE_NAME "driver/pxausb"
static int usbctl_read_proc(char *page, char **start, off_t off,
			    int count, int *eof, void *data);
#endif

//////////////////////////////////////////////////////////////////////////////
// Globals
//////////////////////////////////////////////////////////////////////////////
static const char pszMe[] = "usbctl: ";
struct usb_info_t usbd_info;	/* global to ep0, usb_recv, usb_send */

/* device descriptors */
static desc_t desc;

#define MAX_STRING_DESC 16
static string_desc_t *string_desc_array[MAX_STRING_DESC];
static string_desc_t sd_zero;	/* special sd_zero holds language codes */

// called when configured
static usb_notify_t configured_callback = NULL;

enum {
	kStateZombie = 0,
	kStateZombieSuspend = 1,
	kStateDefault = 2,
	kStateDefaultSuspend = 3,
	kStateAddr = 4,
	kStateAddrSuspend = 5,
	kStateConfig = 6,
	kStateConfigSuspend = 7
};

/*
 * FIXME: The PXA UDC handles several host device requests without user 
 * notification/intervention. The table could be collapsed quite a bit...
 */
static int device_state_machine[8][6] = {
//              suspend               reset          resume         adddr       config        deconfig
/* zombie */ {kStateZombieSuspend, kStateDefault, kStateZombie, kError,
		      kError, kError},
/* zom sus */ {kStateZombieSuspend, kStateDefault, kStateZombie, kError,
		       kError, kError},
/* default */ {kStateDefaultSuspend, kStateDefault, kStateDefault,
		       kStateAddr, kStateConfig, kError},
/* def sus */ {kStateDefaultSuspend, kStateDefault, kStateDefault,
		       kError, kError, kError},
/* addr */ {kStateAddrSuspend, kStateDefault, kStateAddr, kError,
		    kStateConfig, kError},
/* addr sus */ {kStateAddrSuspend, kStateDefault, kStateAddr, kError,
			kError, kError},
/* config */ {kStateConfigSuspend, kStateDefault, kStateConfig, kError,
		      kError, kStateDefault},
/* cfg sus */ {kStateConfigSuspend, kStateDefault, kStateConfig, kError,
		       kError, kError}
};

/* "device state" is the usb device framework state, as opposed to the
   "state machine state" which is whatever the driver needs and is much
   more fine grained
*/
static int sm_state_to_device_state[8] = {
//  zombie            zom suspend       
	USB_STATE_POWERED, USB_STATE_SUSPENDED,
//  default           default sus
	USB_STATE_DEFAULT, USB_STATE_SUSPENDED,
//  addr              addr sus         
	USB_STATE_ADDRESS, USB_STATE_SUSPENDED,
//  config            config sus
	USB_STATE_CONFIGURED, USB_STATE_SUSPENDED
};

#if 0
static char *state_names[8] = { "zombie", "zombie suspended",
	"default", "default suspended",
	"address", "address suspended",
	"configured", "config suspended"
};

static char *event_names[6] = { "suspend", "reset", "resume",
	"address assigned", "configure", "de-configure"
};

static char *device_state_names[] =
    { "not attached", "attached", "powered", "default",
	"address", "configured", "suspended"
};
#endif

static int sm_state = kStateZombie;

//////////////////////////////////////////////////////////////////////////////
// Async
//////////////////////////////////////////////////////////////////////////////

/* The UDCCR reg contains mask and interrupt status bits,
 * so using '|=' isn't safe as it may ack an interrupt.
 */

void udc_set_mask_UDCCR(int mask)
{
	UDCCR = (UDCCR & UDCCR_MASK_BITS) | (mask & UDCCR_MASK_BITS);
}

void udc_clear_mask_UDCCR(int mask)
{
	UDCCR = (UDCCR & UDCCR_MASK_BITS) & ~(mask & UDCCR_MASK_BITS);
}

void udc_ack_int_UDCCR(int mask)
{
	/* udccr contains the bits we dont want to change */
	__u32 udccr = UDCCR & UDCCR_MASK_BITS;

	UDCCR = udccr | (mask & ~UDCCR_MASK_BITS);
}

void udc_int_hndlr(int irq, void *dev_id)
{
	__u32 isr0 = UDCISR0;
	__u32 isr1 = UDCISR1;
	__u32 icr0 = UDCICR0;
	__u32 icr1 = UDCICR1;

	if (isr0 == 0x0 && isr1 == 0x0)
		return;

	UDCICR0 = 0;
	UDCICR1 = 0;

	/* SUSpend Interrupt Request */
	if (isr1 & UDCISR1_IRSU) {
		UDCISR1 = UDCISR1_IRSU;	/* clear the bit */
//              SerialOutputString("suspend irq\n");
		usbctl_next_state_on_event(kEvSuspend);
	}

	/* RESume Interrupt Request */
	if (isr1 & UDCISR1_IRRU) {
		UDCISR1 = UDCISR1_IRRU;
//              SerialOutputString("resume irq\n");
		usbctl_next_state_on_event(kEvResume);
	}

	/* Configuration Changed Interrupt */
	if (isr1 & UDCISR1_IRCC) {
		UDCISR1 = UDCISR1_IRCC;
		UDCCR |= UDCCR_SMAC;
		//logstr("caught config change\n");
#if 0
		if (usbctl_next_state_on_event(kEvReset) != kError) {
			ep0_reset();
			ep1_reset();
			ep2_reset();
			usbctl_next_state_on_event(kEvConfig);

			//usb_driver_reset();
			SerialOutputString
			    ("USB is available. Please config the host usb0 interface.\n");
			SerialOutputString("blob>");
		}
#endif
	}

	/* ReSeT Interrupt Request - UDC has been reset */
	if (isr1 & UDCISR1_IRRS) {
		UDCISR1 = UDCISR1_IRRS;
		if (usbctl_next_state_on_event(kEvReset) != kError) {
			ep0_reset();
			ep1_reset();
			ep2_reset();
			usbctl_next_state_on_event(kEvConfig);

			usb_mot_open(0x01);
		}
	} else {
		/* ep0 int */
		if (isr0 & 0x2) {
			UDCISR0 = 0x2;
		}
		if (isr0 & UDCISR0_IR0_PC) {
			UDCISR0 = 0x1;
			ep0_int_hndlr();
		}

		if (isr0 & 0x8) {
			UDCISR0 = 0x8;
		}
		if (isr0 & UDCISR0_IRA_PC) {
			UDCISR0 = 0x4;
			ep1_int_hndlr(isr0);
			//      logstr("ep1 int caught\n");
		}

		if (isr0 & 0x20) {
			UDCISR0 = 0x20;
		}
		if (isr0 & UDCISR0_IRB_PC) {
			UDCISR0 = 0x10;
			ep2_int_hndlr(isr0);
			//      logstr("ep2 int caught\n");
		}

		while (UDCCSRB & UDCCSR_BNEF) {
			ep2_int_hndlr(isr0);
		}
	}

	UDCICR0 = icr0;
	UDCICR1 = icr1;
}

//////////////////////////////////////////////////////////////////////////////
// Public Interface
//////////////////////////////////////////////////////////////////////////////

/* Open PXA usb core on behalf of a client, but don't start running */
int pxa_usb_open(const char *client)
{
	if (usbd_info.client_name != NULL) {
		return -EBUSY;
	}

	usbd_info.client_name = (char *)client;
	memset(&usbd_info.stats, 0, sizeof(struct usb_stats_t));
	memset(string_desc_array, 0, sizeof(string_desc_array));

	sm_state = kStateZombieSuspend;
	usbd_info.state = USB_STATE_SUSPENDED;

	initialize_descriptors();

	return 0;
}

/* Start running. Must have called usb_open (above) first */
int pxa_usb_start(void)
{
	if (usbd_info.client_name == NULL) {
		return -EPERM;
	}

	/* start UDC internal machinery running */
	udc_enable();
	msleep(1);

	/* give endpoint notification we are starting */
	ep1_state_change_notify(USB_STATE_SUSPENDED);
	ep2_state_change_notify(USB_STATE_SUSPENDED);

	/* enable any platform specific hardware */
	soft_connect_hook(1);

	return 0;
}

/* Stop USB core from running */
int pxa_usb_stop(void)
{
	if (usbd_info.client_name == NULL) {
		return -EPERM;
	}

	ep1_reset();
	ep2_reset();

	udc_disable();

	return 0;
}

/* Tell PXA core client is through using it */
int pxa_usb_close(void)
{
	if (usbd_info.client_name == NULL) {
		return -EPERM;
	}

	usbd_info.client_name = NULL;
	return 0;
}

/* set a proc to be called when device is configured */
usb_notify_t pxa_set_configured_callback(usb_notify_t func)
{
	usb_notify_t retval = configured_callback;
	configured_callback = func;
	return retval;
}

/*====================================================
 * Descriptor Manipulation.
 * Use these between open() and start() above to setup
 * the descriptors for your device.
 *
 */

/* get pointer to static default descriptor */
desc_t *pxa_usb_get_descriptor_ptr(void)
{
	return &desc;
}

/* optional: set a string descriptor */
int pxa_usb_set_string_descriptor(int i, string_desc_t * p)
{
	int retval;
	if (i < MAX_STRING_DESC) {
		string_desc_array[i] = p;
		retval = 0;
	} else {
		retval = -EINVAL;
	}
	return retval;
}

/* optional: get a previously set string descriptor */
string_desc_t *pxa_usb_get_string_descriptor(int i)
{
	return (i < MAX_STRING_DESC)
	    ? string_desc_array[i]
	    : NULL;
}

config_desc_t *pxa_usb_get_config(int cfgval)
{
	int i;
	desc_t *pdesc = pxa_usb_get_descriptor_ptr();
	config_desc_t *cfg = (config_desc_t *) (pdesc->cdb);

	for (i = 0; i < pdesc->dev.bNumConfigurations; i++) {
		if (cfg->bConfigurationValue == cfgval)
			return cfg;
		cfg =
		    (config_desc_t *) ((unsigned char *)cfg +
				       cfg->wTotalLength);
	}

	return NULL;
}

intf_desc_t *pxa_usb_get_interface(config_desc_t * cfg, int idx)
{
	int i;
	intf_desc_t *intf = (intf_desc_t *) (cfg + 1);

	for (i = 0; i < cfg->bNumInterfaces; i++) {
		if (idx == intf->bInterfaceNumber)
			return intf;
		intf++;
	}

	return NULL;
}

ep_desc_t *pxa_usb_get_endpoint(intf_desc_t * intf, int idx)
{
	int i;
	ep_desc_t *ep = (ep_desc_t *) (intf + 1);

	for (i = 0; i < intf->bNumEndpoints; i++) {
		if (idx == (ep->bEndpointAddress & 0xF))
			return ep;
		ep++;
	}
	return NULL;
}

//////////////////////////////////////////////////////////////////////////////
// Exports to rest of driver
//////////////////////////////////////////////////////////////////////////////

/* called by the int handler here and the two endpoint files when interesting
   .."events" happen */
int usbctl_next_state_on_event(int event)
{
	int next_state = device_state_machine[sm_state][event];
	if (next_state == kStateConfig) {
//              SerialOutputString(" ---> kStateConfigured\n");
	}

	if (next_state != kError) {
		int next_device_state = sm_state_to_device_state[next_state];
/*
		SerialOutputString(pszMe);
		SerialOutputString(state_names[sm_state]);
		SerialOutputString(" --> ");
		SerialOutputString(event_names[event]);
		SerialOutputString(" --> ");
		SerialOutputString(state_names[next_state]);
		SerialOutputString("device in state:");
		SerialOutputString(device_state_names[next_device_state]);
		SerialOutputString("\n");
*/
		sm_state = next_state;
		if (usbd_info.state != next_device_state) {
			if (configured_callback != NULL
			    &&
			    next_device_state == USB_STATE_CONFIGURED
			    && usbd_info.state != USB_STATE_SUSPENDED) {
				configured_callback();
			}
			usbd_info.state = next_device_state;

			ep1_state_change_notify(next_device_state);
			ep2_state_change_notify(next_device_state);
		}
	}

	return next_state;
}

//////////////////////////////////////////////////////////////////////////////
// Private Helpers
//////////////////////////////////////////////////////////////////////////////

/* setup default descriptors */

static void initialize_descriptors(void)
{

	desc.dev.bLength = sizeof(device_desc_t);
	desc.dev.bDescriptorType = USB_DESC_DEVICE;
	desc.dev.bcdUSB = 0x110;	/* 1.1 */
	desc.dev.bDeviceClass = 0x00;	/* vendor specific */
	desc.dev.bDeviceSubClass = 0;
	desc.dev.bDeviceProtocol = 0;
	desc.dev.bMaxPacketSize0 = 16;	/* ep0 max fifo size */
	desc.dev.idVendor = 0;	/* vendor ID undefined */
	desc.dev.idProduct = 0;	/* product */
	desc.dev.bcdDevice = 1;	/* vendor assigned device release num */
	desc.dev.iManufacturer = 0;	/* index of manufacturer string */
	desc.dev.iProduct = 0;	/* index of product description string */
	desc.dev.iSerialNumber = 0;	/* index of string holding product s/n */
	desc.dev.bNumConfigurations = 1;	/* configurations we have */

	/* -stanley */
	//usb_driver_reset();

	/* set language */
	/* See: http://www.usb.org/developers/data/USB_LANGIDs.pdf */
	sd_zero.bDescriptorType = USB_DESC_STRING;
	sd_zero.bLength = sizeof(string_desc_t);
	sd_zero.bString[0] = make_word_c(0x409);	/* American English */
	pxa_usb_set_string_descriptor(0, &sd_zero);
}

/* soft_connect_hook()
 * Some devices have platform-specific circuitry to make USB
 * not seem to be plugged in, even when it is. This allows
 * software to control when a device 'appears' on the USB bus
 * (after Linux has booted and this driver has loaded, for
 * example). If you have such a circuit, control it here.
 */
static void soft_connect_hook(int enable)
{
}

/* disable the UDC at the source */
static void udc_disable(void)
{
	soft_connect_hook(0);
	/* clear UDC-enable */
	udc_clear_mask_UDCCR(UDCCR_UDE);

	/* Disable clock for USB device */
	/* Does it useful? */
	CKEN &= ~CKEN11_USB;
}

/* configure the udc endpoint configuration registers */
static void udc_configure(void)
{
	/* endpoint A ~ BULK, IN, MPS: 64bytes, Double-buffering, configuration 1, interface 0,
	   altsetting 0, endpoint 1
	 */
	UDCCRA = 0x0200d103;

	/* endpoint B ~ BULK, OUT,MPS: 64bytes, Double-buffering, configuration 1, interface 0,
	   altsetting 0, endpoint 2
	 */
	UDCCRB = 0x02014103;
}

/*  enable the udc at the source */
static void udc_enable(void)
{
	/* Enable clock for USB device */
	/* is it right */
	CKEN |= CKEN11_USB;

	udc_configure();

	/* enable endpoint 0, A, B's Packet Complete Interrupt. */
	UDCICR0 = 0x0000003f;
	UDCICR1 = 0xa8000000;

	/* clear the interrupt status/control registers */
	UDCISR0 = 0xffffffff;
	UDCISR1 = 0xffffffff;

	/* set UDC-enable */
	udc_set_mask_UDCCR(UDCCR_UDE);

	if ((UDCCR & UDCCR_EMCE) == UDCCR_EMCE) {

	}

	if ((UDCCR & UDCCR_UDA) == 0) {

	}
}

//////////////////////////////////////////////////////////////////////////////
// Module Initialization and Shutdown
//////////////////////////////////////////////////////////////////////////////
/*
 * usbctl_init()
 * Module load time. Allocate dma and interrupt resources. Setup /proc fs
 * entry. Leave UDC disabled.
 */
int usbctl_init(void)
{
	int rc = 0;

	udc_disable();

	memset(&usbd_info, 0, sizeof(usbd_info));
/*	SerialOutputString("PXA USB Controller Core Initialized\n"); */

	rc = pxa_usb_open("myusb");
	pxa_usb_start();

	return 0;
}

/*
 * usbctl_exit()
 * Release DMA and interrupt resources
 */
void usbctl_exit(void)
{
	udc_disable();
}

#ifndef MBM
__initlist(usbctl_init, INIT_LEVEL_INITIAL_HARDWARE);
__exitlist(usbctl_exit, INIT_LEVEL_INITIAL_HARDWARE);
#endif

/*
 * pxa_usb.h
 *
 * Public interface to the pxa USB core. For use by client modules
 * like usb-eth and usb-char.
 *
 * 02-May-2002
 *   Frank Becker (Intrinsyc) - derived from sa1100_usb.h
 *
 */

#ifndef _PXA_USB_H
#define _PXA_USB_H
#include <asm/byteorder.h>

#define MAX_CONFIGS	3
#define MAX_INTERFACES	2
#define MAX_ENDPOINTS	4

typedef void (*usb_callback_t)(int flag, int size);

/* in usb_ctl.c (see also descriptor methods at bottom of file) */

// Open the USB client for client and initialize data structures
// to default values, but _do not_ start UDC.
int pxa_usb_open( const char * client_name );

// Start UDC running
int pxa_usb_start( void );

// Immediately stop udc, fire off completion routines w/-EINTR
int pxa_usb_stop( void ) ;

// Disconnect client from usb core
int pxa_usb_close( void ) ;

// set notify callback for when core reaches configured state
// return previous pointer (if any)
typedef void (*usb_notify_t)(void);
usb_notify_t pxa_set_configured_callback( usb_notify_t callback );

/* in usb_send.c */
int pxa_usb_xmitter_avail( void );
int pxa_usb_send(char *buf, int len, usb_callback_t callback);
void sa110a_usb_send_reset(void);

/* in usb_recev.c */
int pxa_usb_recv(char *buf, int len, usb_callback_t callback);
void pxa_usb_recv_reset(void);

//////////////////////////////////////////////////////////////////////////////
// Descriptor Management
//////////////////////////////////////////////////////////////////////////////

#define DescriptorHeader \
	__u8 bLength;        \
	__u8 bDescriptorType


// --- Device Descriptor -------------------

typedef struct {
	 DescriptorHeader;
	 __u16 bcdUSB;		   	/* USB specification revision number in BCD */
	 __u8  bDeviceClass;	/* USB class for entire device */
	 __u8  bDeviceSubClass; /* USB subclass information for entire device */
	 __u8  bDeviceProtocol; /* USB protocol information for entire device */
	 __u8  bMaxPacketSize0; /* Max packet size for endpoint zero */
	 __u16 idVendor;        /* USB vendor ID */
	 __u16 idProduct;       /* USB product ID */
	 __u16 bcdDevice;       /* vendor assigned device release number */
	 __u8  iManufacturer;	/* index of manufacturer string */
	 __u8  iProduct;        /* index of string that describes product */
	 __u8  iSerialNumber;	/* index of string containing device serial number */
	 __u8  bNumConfigurations; /* number fo configurations */
} __attribute__ ((packed)) device_desc_t;

// --- Configuration Descriptor ------------

typedef struct {
	 DescriptorHeader;
	 __u16 wTotalLength;	    /* total # of bytes returned in the cfg buf 4 this cfg */
	 __u8  bNumInterfaces;      /* number of interfaces in this cfg */
	 __u8  bConfigurationValue; /* used to uniquely ID this cfg */
	 __u8  iConfiguration;      /* index of string describing configuration */
	 __u8  bmAttributes;        /* bitmap of attributes for ths cfg */
	 __u8  MaxPower;		    /* power draw in 2ma units */
} __attribute__ ((packed)) config_desc_t;

// bmAttributes:
enum { 
	USB_CONFIG_REMOTEWAKE=0xA0, 
	USB_CONFIG_SELFPOWERED=0xC0,
	USB_CONFIG_BUSPOWERED=0x80 
};

// MaxPower:
#define USB_POWER( x)  ((x)>>1) /* convert mA to descriptor units of A for MaxPower */

// --- Interface Descriptor ---------------

typedef struct {
	 DescriptorHeader;
	 __u8  bInterfaceNumber;   /* Index uniquely identfying this interface */
	 __u8  bAlternateSetting;  /* ids an alternate setting for this interface */
	 __u8  bNumEndpoints;      /* number of endpoints in this interface */
	 __u8  bInterfaceClass;    /* USB class info applying to this interface */
	 __u8  bInterfaceSubClass; /* USB subclass info applying to this interface */
	 __u8  bInterfaceProtocol; /* USB protocol info applying to this interface */
	 __u8  iInterface;         /* index of string describing interface */
} __attribute__ ((packed)) intf_desc_t;

// --- Endpoint  Descriptor ---------------

typedef struct {
	 DescriptorHeader;
	 __u8  bEndpointAddress;  /* 0..3 ep num, bit 7: 0 = 0ut 1= in */
	 __u8  bmAttributes;      /* 0..1 = 0: ctrl, 1: isoc, 2: bulk 3: intr */
	 __u16 wMaxPacketSize;    /* data payload size for this ep in this cfg */
	 __u8  bInterval;         /* polling interval for this ep in this cfg */
} __attribute__ ((packed)) ep_desc_t;

// bEndpointAddress:
enum { 
	USB_OUT	=0, 
	USB_IN	=1 
};

#define USB_EP_ADDRESS(a,d) (((a)&0xf) | ((d) << 7))
// bmAttributes:
enum { 
	USB_EP_CNTRL	=0, 
	USB_EP_BULK	=2, 
	USB_EP_INT	=3, 
	USB_EP_ISO	=4 
};

// --- String Descriptor -------------------

typedef struct {
	 DescriptorHeader;
	 __u16 bString[1];		  /* unicode string .. actaully 'n' __u16s */
} __attribute__ ((packed)) string_desc_t;

/*=======================================================
 * Handy helpers when working with above
 *
 */
// these are x86-style 16 bit "words" ...
#define make_word_c( w ) __constant_cpu_to_le16(w)
#define make_word( w )   __cpu_to_le16(w)

// descriptor types
enum { 
    USB_DESC_DEVICE	= 1,
    USB_DESC_CONFIG	= 2,
    USB_DESC_STRING	= 3,
    USB_DESC_INTERFACE	= 4,
    USB_DESC_ENDPOINT	= 5
};


/*=======================================================
 * Default descriptor layout for SA-1100 and SA-1110 UDC
 */

enum {
  UNUSED = 0,

  BULK_IN1  =  1,
  BULK_OUT1 =  2,
  ISO_IN1   =  3,
  ISO_OUT1  =  4,
  INT_IN1   =  5,

  BULK_IN2  =  6,
  BULK_OUT2 =  7,
  ISO_IN2   =  8,
  ISO_OUT2  =  9,
  INT_IN2   = 10,

  BULK_IN3  = 11,
  BULK_OUT3 = 12,
  ISO_IN3   = 13,
  ISO_OUT3  = 14,
  INT_IN3   = 15
} /*endpoint_type*/;

/* "config descriptor buffer" - that is, one config,
   ..one interface and 2 endpoints */
//struct cdb {
//	 config_desc_t cfg;
//	 intf_desc_t   intf;
//	 ep_desc_t     ep1;
//	 ep_desc_t     ep2;
//} __attribute__ ((packed));

/* all SA device descriptors */
//typedef struct {
//	 device_desc_t dev;   /* device descriptor */
//	 struct cdb b;        /* bundle of descriptors for this cfg */
//} __attribute__ ((packed)) desc_t;


typedef struct {
	device_desc_t 	dev;	/* device descriptor */	
	char		cdb[1024]; /* FIXME */
} desc_t;

/*=======================================================
 * Descriptor API
 */

/* Get the address of the statically allocated desc_t structure
   in the usb core driver. Clients can modify this between
   the time they call pxa_usb_open() and pxa_usb_start()
*/
desc_t *
pxa_usb_get_descriptor_ptr( void );

/* Set a pointer to the string descriptor at "index". The driver
 ..has room for 8 string indicies internally. Index zero holds
 ..a LANGID code and is set to US English by default. Inidices
 ..1-7 are available for use in the config descriptors as client's
 ..see fit. This pointer is assumed to be good as long as the
 ..SA usb core is open (so statically allocate them). Returnes -EINVAL
 ..if index out of range */
int pxa_usb_set_string_descriptor( int index, string_desc_t * p );

/* reverse of above */
string_desc_t *
pxa_usb_get_string_descriptor( int index );

config_desc_t * 
pxa_usb_get_config(int cfgval);

intf_desc_t *
pxa_usb_get_intf(config_desc_t *cfg, int idx);

ep_desc_t * 
pxa_usb_get_endpoint(intf_desc_t *intf, int idx);



#endif /* _PXA_USB_H */

#pragma once

#define LIBUSB_PATH_MAX 512
#define USB_MAXALTSETTING 128
#define USB_MAXINTERFACES 32
#define USB_MAXCONFIG 8
#pragma pack(1)
struct usb_interface_descriptor{
	unsigned char  bLength;
	unsigned char  bDescriptorType;
	unsigned char  bInterfaceNumber;
	unsigned char  bAlternateSetting;
	unsigned char  bNumEndpoints;
	unsigned char  bInterfaceClass;
	unsigned char  bInterfaceSubClass;
	unsigned char  bInterfaceProtocol;
	unsigned char  iInterface;

	struct usb_endpoint_descriptor *endpoint;

	unsigned char *extra;	/* Extra descriptors */
	int extralen;
};

struct usb_interface{
	struct usb_interface_descriptor *altsetting;
	int num_altsetting;
};

/* Configuration descriptor information.. */
struct usb_config_descriptor{
	unsigned char  bLength;
	unsigned char  bDescriptorType;
	unsigned short wTotalLength;
	unsigned char  bNumInterfaces;
	unsigned char  bConfigurationValue;
	unsigned char  iConfiguration;
	unsigned char  bmAttributes;
	unsigned char  MaxPower;

	struct usb_interface *uinterface;

	unsigned char *extra;	/* Extra descriptors */
	int extralen;
};

/* Device descriptor */
struct usb_device_descriptor{
	unsigned char  bLength;
	unsigned char  bDescriptorType;
	unsigned short bcdUSB;
	unsigned char  bDeviceClass;
	unsigned char  bDeviceSubClass;
	unsigned char  bDeviceProtocol;
	unsigned char  bMaxPacketSize0;
	unsigned short idVendor;
	unsigned short idProduct;
	unsigned short bcdDevice;
	unsigned char  iManufacturer;
	unsigned char  iProduct;
	unsigned char  iSerialNumber;
	unsigned char  bNumConfigurations;
};

struct usb_ctrl_setup{
	unsigned char  bRequestType;
	unsigned char  bRequest;
	unsigned short wValue;
	unsigned short wIndex;
	unsigned short wLength;
};


struct usb_device{
	struct usb_device *next, *prev;

	char filename[LIBUSB_PATH_MAX];

	struct usb_bus *bus;

	struct usb_device_descriptor descriptor;
	struct usb_config_descriptor *config;

	void *dev;		/* Darwin support */

	unsigned char devnum;

	unsigned char num_children;
	struct usb_device **children;
};

struct usb_bus {
	struct usb_bus *next, *prev;

	char dirname[LIBUSB_PATH_MAX];

	struct usb_device *devices;
	unsigned long location;

	struct usb_device *root_dev;
};

/* Version information, Windows specific */
struct usb_version {
	struct
	{
		int major;
		int minor;
		int micro;
		int nano;
	} dll;
	struct
	{
		int major;
		int minor;
		int micro;
		int nano;
	} driver;
};


struct usb_dev_handle;
typedef struct usb_dev_handle usb_dev_handle;

void patch_libusb(void);
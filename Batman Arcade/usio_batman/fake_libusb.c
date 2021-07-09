// Fake Libusb Device [Windows]
#include <windows.h>
#include "../shared/mem.h"
#include "usio_batman.h"
#include "fake_libusb.h"

// Definitions


static struct usb_bus bus;
static struct usb_device dev;
static struct usb_config_descriptor config;
static struct usb_interface uinterface;
static struct usb_interface_descriptor idesc;
static struct usb_bus *usb_busses;

// Hooked function prototypes
static int(*real_usb_init)(void) = NULL;
static void *(*real_usb_open)(struct usb_device *hdev) = NULL;
static int(*real_usb_bulk_read)(void * dev, int ep, char *bytes, int size, int timeout) = NULL;
static int(*real_usb_bulk_write)(void * dev, int ep, char *bytes, int size, int timeout) = NULL;
static int(*real_usb_find_devices)(void) = NULL;
static int(*real_usb_release_interface)(void * dev, int uinterface) = NULL;
static int(*real_usb_set_configuration)(void *hdev, int configuration) = NULL;
static int(*real_usb_find_busses)(void) = NULL;
static struct usb_bus * (*real_usb_get_busses)(void) = NULL;
static int(*real_usb_close)(void *hdev) = NULL;
static int(*real_usb_claim_interface)(DWORD *hdev, int uinterface) = NULL;


// Replacement functions
int hk_usb_claim_interface(DWORD *hdev, int uinterface) {return 0;}
int hk_usb_close(void *hdev) {return 0;}
int hk_usb_release_interface(void * dev, int uinterface) { return 0; }
int hk_usb_set_configuration(void *hdev, int configuration) { return 0; }
int hk_usb_init(void) { return 0; }
void * hk_usb_open(struct usb_device *hdev) { return hdev; }
struct usb_bus* hk_usb_get_busses(void) {return (usb_busses);}
int hk_usb_find_busses(void){return 1;}
int hk_usb_find_devices(void) {return 1;}

int hk_usb_bulk_write(void * dev, int ep, char *bytes, int size, int timeout) {
	process_usio_output(bytes,size);
	return size;
}

int hk_usb_bulk_read(void * dev, int ep, char *bytes, int size, int timeout) {
	get_usio_input(bytes);
	return size;
}

void init_device(){
	usb_busses = &bus;
	bus.devices = &dev;
	dev.config = &config;
	config.uinterface = &uinterface;
	uinterface.altsetting = &idesc;
	dev.descriptor.idVendor = DEVICE_VID;
	dev.descriptor.idProduct = DEVICE_PID;
}

void patch_libusb(void) {
	init_device();
	inline_hook("libusb0.dll", "usb_init", 0, (void*)hk_usb_init, (void**)&real_usb_init);
	inline_hook("libusb0.dll", "usb_find_busses", 0, (void*)hk_usb_find_busses, (void**)&real_usb_find_busses);
	inline_hook("libusb0.dll", "usb_get_busses", 0, (void*)hk_usb_get_busses, (void**)&real_usb_get_busses);		
	inline_hook("libusb0.dll", "usb_open", 0,(void*) hk_usb_open, (void**)&real_usb_open);
	inline_hook("libusb0.dll", "usb_set_configuration", 0, (void*)hk_usb_set_configuration, (void**)&real_usb_set_configuration);
	inline_hook("libusb0.dll", "usb_claim_interface", 0, (void*)hk_usb_claim_interface, (void**)&real_usb_claim_interface);
	inline_hook("libusb0.dll", "usb_bulk_read", 0, (void*)hk_usb_bulk_read, (void**)&real_usb_bulk_read);
	inline_hook("libusb0.dll", "usb_bulk_write", 0, (void*)hk_usb_bulk_write, (void**)&real_usb_bulk_write);
	inline_hook("libusb0.dll", "usb_find_devices", 0, (void*)hk_usb_find_devices, (void**)&real_usb_find_devices);
	inline_hook("libusb0.dll", "usb_release_interface", 0, (void*)hk_usb_release_interface, (void**)&real_usb_release_interface);
	inline_hook("libusb0.dll", "usb_close", 0, (void*)hk_usb_close, (void**)&real_usb_close);
}

// Entry-Point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		patch_libusb();		
	}
	return TRUE;
}

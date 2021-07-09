/*
    Dead Heat Riders Arcade Patch by rFx
    2016 Team HackItUp
    
    compile: cc -shared -m32 dhr_emu.c -w -ldl -D_GNU_SOURCE -o ./dhr_emu.so
*/ 

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <dlfcn.h>
#include <string.h>
#include <sys/mman.h>

/*------------------------------
 * MEMORY MODIFICATON FUNCTIONS
 *----------------------------*/
// Memory Modification Macro
#define UNPROTECT(addr,len) (mprotect((void*)(addr-(addr%len)),len,PROT_READ|PROT_WRITE|PROT_EXEC))
void detour_function(void * new_adr, int addr)
{
    int call = (int)(new_adr - addr - 5);
    UNPROTECT(addr, 4096);
    *((unsigned char*)(addr)) = 0xE9;
    *((int*)(addr + 1)) = call;
}
void detour_function2(void * new_adr, int addr)
{
    int call = (int)(new_adr - addr - 5);
    UNPROTECT(addr, 4096);
    *((unsigned char*)(addr)) = 0xE8;
    *((int*)(addr + 1)) = call;
}

/*--------------------------
 * DONGLE WRAPPER FUNCTIONS
 *------------------------*/
 static struct usbdd{ 
   unsigned char bLength;              // length in unsigned chars of this descriptor 
   unsigned char bDescriptor;          // descriptor ID (1) 
   unsigned short bcdUSB;              // USB version (BCD format) 
   unsigned char bDeviceClass;         // the device's class 
   unsigned char bDeviceSubClass;      // and subclass 
   unsigned char bDeviceProtocol;      // and protocol 
   unsigned char bMaxPacketSize0;      // default control pipe max packet len 
   unsigned short idVendor;            // vendor ID 
   unsigned short idProduct;           // product ID 
   unsigned short bcdDevice;           // device's version 
   unsigned char iManufacturer;        // index of vendor's string descriptor 
   unsigned char iProduct;             // index of product's string descriptor 
   unsigned char iSerialNumber;        // index of product's serial number 
   unsigned char bNumConfigurations;   // number of available configurations 
} usbdevdsc;

int libusb_init (int  *ctx){return 0;} // Don't care about this
void libusb_exit (int  *ctx){}; // Don't care about this, either
void libusb_close (int *dev_handle){}; // See above
int libusb_open (int *dev, int **handle){return 0;} // See above
unsigned int libusb_get_device_list (unsigned int *ctx, unsigned int ***list){
return 1;
}

int libusb_get_device_descriptor (int *dev, int *desc){ // Now things actually do... stuff...
	usbdevdsc.idVendor  = 0x0B9A;
	usbdevdsc.idProduct = 0x0C10;
	memcpy(desc,&usbdevdsc,sizeof(usbdevdsc));
	return 0;
}

int libusb_get_string_descriptor_ascii (int *dev, unsigned char desc_index, unsigned char *data, int length){
	if(!getenv("DHR_SERIAL")){
		printf("No Dongle Serial set in DHR_SERIAL - Failing...");
		return 0; // You didn't set the serial... BAD MONKEY!!!
	}
	
	strcpy(data,getenv("DHR_SERIAL"));
	length = 13; // It's always 12+1
	return length;
}

int system(unsigned char* cmd){

    //printf("SYSTEM: %s\n",cmd);
    return 0;
}

/*-----------------
 * HOOK FUNCTIONS
 *----------------*/
// ioctl Hook Stub
static int (*real_ioctl)(int fd, unsigned long int request, int *data) = NULL;

// ioctl Definitions
#define CMD64_VIDIOC_S_CROP 0x4014563C
int ioctl(int fd, int request, unsigned char* data){
    switch(request){
        case CMD64_VIDIOC_S_CROP:
            return 0;
        default:
            return real_ioctl(fd, request, data);
    }
}

int generic_retzero(){return 0;}
int get_total_machine(int* this){

    return 1;
}
/*-----------------
 * INITIAL PATCHES
 *----------------*/
void patch_binary()
{
    // Hook ioctl
    real_ioctl = dlsym(RTLD_NEXT, "ioctl");
        
    // Unsupported cameras with <= 96k buffer size will fail for whatever reason... let's fix this.
	UNPROTECT(0x084B74A5, 4096);
	*((int*)0x084B74A5) = 0x00;
 
    // Shader fix for non ati cards.
    UNPROTECT(0x080D03D4,4096);
    *(unsigned short*)0x080D03D4 = 0x9090;

    // Fake USB Device De-Reference Fix because I'm lazy.
    UNPROTECT(0x08092546,4096);
    memset(0x08092546,0x90,3);
    
    // Network Mode Group ID Patch
    UNPROTECT(0x082B9B81,4096);
    if(getenv("DHR_GROUP_ID") == NULL){
        *(unsigned int*)0x082B9B81 = 1;
    }else{
        int group_id = atoi(getenv("DHR_GROUP_ID"));
        *(unsigned int*)0x082B9B81 = group_id;        
    }
    // Network Mode Total Machine IP Patch (Horrifying Hack Warning)
    detour_function(&get_total_machine,0x08354480);

    // Network Mode Anti-Fuckup Patches
    detour_function2(&generic_retzero,0x0835B66A); // Route Add Change Patch
    detour_function2(&generic_retzero,0x08360769); // Route Add Change Patch 2
    
    // Network Python ifconfig patch
    UNPROTECT(0x08512FE4,4096);
    strcpy(0x08512FE4,"python ./data/ifconf.py ");

    // Network Python Pinger
    UNPROTECT(0x08512F40,4096);
    strcpy(0x08512F40,"python ./data/pinger.py &");
    // Turn off ARP IP Conflict Checking
    UNPROTECT(0x08351530,4096);
    memset(0x08351530,0x90,5);

}

// Entry Point.
void __attribute__((constructor)) initialize(void)
{
    printf("Starting up Wrapper...\n");
    patch_binary();
}



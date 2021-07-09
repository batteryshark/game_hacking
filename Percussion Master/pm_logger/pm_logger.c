// PM ASIC27 Logger

/*

Design - Wraps read and write to look for the pccard.
On open - capture file handle for pccard0.
On read - passthrough to real_read, then records the data structure of the read area.
On write - records the data structure of the write area, passes to real write, and records the response.
Data structure would be R/W_MODE : all the data - this would have to be done with append mode to ensure we capture the right stuff... this may get rather big depending.
Everything would be dumped to a pm_log.bin

*/

#include <stdio.h>
#include <string.h>
#include <dlfcn.h>
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#define PCCARD_PATH "/dev/pccard0"
#define IO_READ_BUFFER_ADDR  0x0842FB00
#define IO_WRITE_BUFFER_ADDR 0x0842BAC0

static unsigned int pccard_fd;
static unsigned int pccard_log_fd;
// Some functions we need to detour
static int (*real_open)(char *, int) = NULL;
static unsigned int (*real_read)(int fd, void *buf, size_t count) = NULL;
static unsigned int (*real_write)(int fd, void *buf, size_t count) = NULL;

static struct sreadBuffer{
    unsigned int buffer_size;
    unsigned int cmdport;
    unsigned char coin_inserted;
    unsigned char asic_iserror;
    unsigned short asic_errnum;
    unsigned int some_io[4];
    unsigned int drum_io;
    unsigned int some_other_io;
    unsigned short input_matrix_read; // This has to be set to > 4 each time.
    unsigned char protection_value;
    unsigned char protection_offset;
    unsigned short game_region;
    unsigned short align_1;
    char pch_in_rom_version_name[8];
    char pch_ext_rom_version_name[8];
    unsigned short inet_password_data;
    unsigned short a27_has_message;
    unsigned char is_light_io_reset;
    unsigned char pci_card_version;
    unsigned char checksum_1;
    unsigned char checksum_2;
    unsigned char a27_message[0x40];
    unsigned short enum_cmd;
    unsigned char data_buffer[0x4000];
}readBuffer;

static struct swriteBuffer{
unsigned int buffer_offset;
unsigned int system_mode;
unsigned int key_input_flag;
unsigned short trackball_data[7];
unsigned char light_disable;
unsigned char key_sensitivity_value;
unsigned int light_1;
unsigned int light_2;
unsigned char data_buffer[0x4000];
}writeBuffer;





void a27_read_operation(){
	unsigned char marker = "R";
	memcpy(&readBuffer,IO_READ_BUFFER_ADDR,sizeof(readBuffer));
	if(!pccard_log_fd){
		pccard_log_fd = open("a27log.bin",0x1101);
	}
	
	real_write(pccard_log_fd,marker,1);
	real_write(pccard_log_fd,&readBuffer,sizeof(readBuffer));
return;
}

void a27_write_operation(){
	unsigned char marker = "W";
	memcpy(&writeBuffer,IO_WRITE_BUFFER_ADDR,sizeof(writeBuffer));
	if(!pccard_log_fd){
		pccard_log_fd = open("a27log.bin",0x1101);
	}
	
	real_write(pccard_log_fd,marker,1);
	real_write(pccard_log_fd,&writeBuffer,sizeof(writeBuffer));
	return;
}


// Open Wrapper For ASIC27 Emulation
int open(const char * filename, int oflag){

    if (real_open == NULL){
      real_open = dlsym(RTLD_NEXT, "open");
    }
    
    if (strcmp(filename, PCCARD_PATH) == 0){
        pccard_fd = real_open(filename,oflag);
    	return pccard_fd;    	
    }

    return real_open(filename, oflag);
}


unsigned int read(int fd, void *buf, unsigned int count){
    unsigned int retval;
    if (real_read == NULL){
      real_read = dlsym(RTLD_NEXT, "read");
    }
 
    if(fd == pccard_fd)
    	
    	retval = real_read(fd,buf,count);
        a27_read_operation();
        return retval;
    
    return real_read(fd,buf,count);
}
unsigned short curcmd;

unsigned int write(int fd, unsigned char *buf, unsigned int count){
    if (real_write == NULL){
      real_write = dlsym(RTLD_NEXT, "write");
    }
    
    if(fd == pccard_fd){
		a27_write_operation();
		real_write(fd,buf,count);
    }
    
    return real_write(fd,buf,count);
}


// Entry Point.
void __attribute__((constructor)) initialize(void)
{}
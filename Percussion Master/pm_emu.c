/*
    PercussionMaster Arcade Patch by r4x
    2016 Team HackItUp
    
    compile: cc -shared -m32 pm_emu.c -w -ldl -D_GNU_SOURCE -o ./pm_emu.so
*/ 
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <linux/input.h>
#include <pthread.h>


// Hardware Identification
#define PCCARD_PATH "/dev/pccard0"
#define PCI_CARD_VERSION 187
#define PCH_IN_ROM_VERSION_NAME "FUCK"
#define PCH_PROGRAM_VERSION_NAME "HUE"
#define PCH_EXT_ROM_VERSION_NAME "YOU"
#define A27_HANDLE 0x0A271337

// MISC
#define UNPROTECT(addr,len) (mprotect((void*)(addr-(addr%len)),len,PROT_READ|PROT_WRITE|PROT_EXEC))
#define IO_READ_BUFFER_ADDR  0x0842FB00
#define IO_WRITE_BUFFER_ADDR 0x0842BAC0

// Some globals we'll need
static unsigned int mbADecodeTable[10] = {6, 7, 3, 4, 8, 0, 1, 2, 9, 5}; // Because this was a good idea
static unsigned char game_region = 4; // USA! USA!
static struct input_event ev;
static pthread_t hthread;

// Some functions we need to detour
static int (*real_open)(char *, int) = NULL;
static int (*real_lseek)(int fd, unsigned int offset, int whence) = NULL;
static unsigned int (*real_read)(int fd, void *buf, size_t count) = NULL;
static unsigned int (*real_write)(int fd, void *buf, size_t count) = NULL;


// Helpers
void detour_function(void * new_adr, int addr)
{
    int call = (int)(new_adr - addr - 5);
    UNPROTECT(addr, 4096);
    *((unsigned char*)(addr)) = 0xE9;
    *((int*)(addr + 1)) = call;
}





// Redirection for stuff that shouldn't do anything
void do_nothing(){
 return;   
}

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

static struct iostate {
unsigned char service;
unsigned char coin;
unsigned char p1_center_1;
unsigned char p1_center_2;
unsigned char p1_center_left;
unsigned char p1_center_right;
unsigned char p1_upper_left;
unsigned char p1_upper_right;
unsigned char p2_center_1;
unsigned char p2_center_2;
unsigned char p2_center_left;
unsigned char p2_center_right;
unsigned char p2_upper_left;
unsigned char p2_upper_right;
}iost;

unsigned int get_drum_iostate(){
    unsigned int niostate = 0;
    
    // Player 1
    (iost.p1_center_1) ? (niostate |= (1 << 0)) : (niostate &= ~(1 << 0));
    (iost.p1_center_2) ? (niostate |= (1 << 1)) : (niostate &= ~(1 << 1));
    (iost.p1_center_right) ? (niostate |= (1 << 2)) : (niostate &= ~(1 << 2));
    (iost.p1_upper_left) ? (niostate |= (1 << 5)) : (niostate &= ~(1 << 5));
    (iost.p1_upper_right) ? (niostate |= (1 << 6)) : (niostate &= ~(1 << 6));
    (iost.p1_center_left) ? (niostate |= (1 << 7)) : (niostate &= ~(1 << 7));
    
    // Player 2
    (iost.p2_upper_left) ? (niostate |= (1 << 8)) : (niostate &= ~(1 << 8));
    (iost.p2_upper_right) ? (niostate |= (1 << 9)) : (niostate &= ~(1 << 9));
    (iost.p2_center_left) ? (niostate |= (1 << 10)) : (niostate &= ~(1 << 10));
    (iost.p2_center_1) ? (niostate |= (1 << 11)) : (niostate &= ~(1 << 11));
    (iost.p2_center_2) ? (niostate |= (1 << 12)) : (niostate &= ~(1 << 12));
    (iost.p2_center_right) ? (niostate |= (1 << 13)) : (niostate &= ~(1 << 13));

    return niostate;
}
void get_service_button_state(){
    (iost.service) ? (*((unsigned char*)0x0843C620) = 1) : (*((unsigned char*)0x0843C620) = 0);
}
unsigned char get_coin_state(){
 if(iost.coin) return 1;

 return 0;
}

unsigned char io_checksum(){
 unsigned int working_checksum;
 working_checksum = readBuffer.a27_has_message+readBuffer.inet_password_data+readBuffer.is_light_io_reset;
 working_checksum += readBuffer.asic_errnum+readBuffer.asic_iserror+readBuffer.coin_inserted+readBuffer.buffer_size;
 working_checksum += (readBuffer.cmdport & 0xFF);
 return working_checksum & 0xFF;
}

unsigned char get_protection_value(){
 return mbADecodeTable[readBuffer.protection_offset % 0x0A];
}

/*
 *  A27 Init Test state
 *  Protcol:
 *  Request: sets 1 at offset 0, allocates 0x1504 bytes
 *    allocates additional 4 bytes at offset 0x1504, sets 2 uint16s to 0x00
 *    The first uint16 may be the write count again... not sure.
 *    Second value I'm not sure of, either.
 *  Response: Not sure how this is used - no data yet...
 */
void process_io_state_1(){
 return;   
}

/*
 * I/O Key Test Screen State 
 * Protocol:
 * Request: Nothing
 * Response: Looks like a similar structure to light test.
 */
void process_io_state_2(){
     memcpy(readBuffer.data_buffer,writeBuffer.data_buffer,writeBuffer.buffer_offset);
     memset(readBuffer.data_buffer,0x00,0x4000);
     readBuffer.buffer_size = 68;
     
     
     readBuffer.enum_cmd = 2;//readBuffer.data_buffer[0];         
}

/*
 * Light Test screen state
 *  Protcol:
 *  Request: depends on m_iLightMode:
 *      0 - Do Nothing
 *      1 - allocate 4 bytes, 0x00 0x00 0x02 0x00
 *      2 - allocate 4 bytes, 0x00 0x00 0x41 0x00
 *      3 - allocate 68 bytes, 0x01 0x00 
 *      4 - allocate 4 bytes, 0x00 0x00 0x00 0x00
 *  Response: ???
 */
void process_io_state_3(){
 return;   
}

/*
 * Counter Test screen state
 *  Protcol:
 *  Request: depends on m_bCounterTestMode:
 *      0 - allocate 4 bytes, 0x00 0x00 0x00 0x00
 *      1 - allocate 4 bytes, 0x01 0x00 0x01 0x00
 *      
 *  Response: ???
 */
void process_io_state_4(){
 return;   
}
/*
 * Coin/Title screen state (1 init, 2 during, 3 end). 
 *  Protcol:
 *  Request: 4 bytes (two uint16s) - 0: current read count 1: 0x04... no idea
 *  Response: 1 byte: updated read count - enum_cmd = updated read count.
 */
void process_io_state_11(){
     readBuffer.buffer_size = 1; // Response Buffer size in bytes.
     readBuffer.data_buffer[0] = writeBuffer.data_buffer[0]; // Get the current read count.
     readBuffer.data_buffer[0]++; // Update the read count.
     readBuffer.enum_cmd = readBuffer.data_buffer[0];  // EnumCmd is basically read count.
}
    
/*
 * Mode Select screen state (1 init, 2 during, 3 end). 
 *  Protcol:
 *  Request: 4 bytes (two uint16s) - 0: current read count 1: 0x04... no idea
 *  Response: 1 byte: updated read count - enum_cmd = updated read count.
 */
void process_io_state_13(){
     readBuffer.buffer_size = 1; // Response Buffer size in bytes.
     readBuffer.data_buffer[0] = writeBuffer.data_buffer[0]; // Get the current read count.
     readBuffer.data_buffer[0]++; // Update the read count.
     readBuffer.enum_cmd = readBuffer.data_buffer[0];     
}

/*
 * Song Select screen state (1 init, 2 during, 3 end). 
 *  Protcol:
 *  Request: 4 bytes (two uint16s) - 0: current read count 1: 0x04... no idea
 *  Response: 1 byte: updated read count - enum_cmd = updated read count.
 */
void process_io_state_14(){
     readBuffer.buffer_size = 1; // Response Buffer size in bytes.
     readBuffer.data_buffer[0] = writeBuffer.data_buffer[0]; // Get the current read count.
     readBuffer.data_buffer[0]++; // Update the read count.
     readBuffer.enum_cmd = readBuffer.data_buffer[0];     
}

/*
 * Ranking screen state (1 init, 2 during, 3 end). 
 *  Protcol:
 *  Request: 4 bytes (two uint16s) - 0: current read count 1: 0x04... no idea
 *  Response: 1 byte: updated read count - enum_cmd = updated read count.
 */
void process_io_state_20(){
     readBuffer.buffer_size = 1; // Response Buffer size in bytes.
     readBuffer.data_buffer[0] = writeBuffer.data_buffer[0]; // Get the current read count.
     readBuffer.data_buffer[0]++; // Update the read count.
     readBuffer.enum_cmd = readBuffer.data_buffer[0];     
}
/*
 * Song Processing - The Big one
 * Protocol: 
 * Request - Depends on m_bSongCmd -- written as buffer value 0
 * 0 - allocate 0x94 bytes: 
 *     [0] 0x00 
 *     [1] @ 0x843FC44
 *     [2] @ 0x843FC46
 *     [4] Assuming this is 0x00, but could also be some kind of offset address.
 *     [6] @ 0x843FC48
 *     [7] @ 0x843FC49
 *     [8] @ 0x843FC4A
 *     [9] @ 0x843FC4B
 *    [10] @ 0x843FC4C
 *    [11] @ 0x843FC43
 *     [6] @ 0x843FC4E ?
 *     [7] @ 0x843FC50 ?
 *    [18] @ 0x843FC52
 *    [19] @ 0x843FC7C
 *    [20] memcpy 0x843FC54 0x28 bytes -- maybe song information
 *    [60] memcpy 0x0843FC7E 0x28 bytes -- appears to be path to sound file.
 *    Then it pulls a bunch of stuff from g_rSongRecord and sets offsets 7 or 8 times.
 * 1 - allocate 0xFA4 bytes, 0x01 0x00 m_bUpLoadIndex 
 *     buffer[4] = memcpy 0x83EB880(m_raNoteRam) + (4000 * m_bUpLoadIndex)
 * 
 * 3 - Allocates / Clears 96 bytes 
 *     [0] 0x03
 *     [1] 0x00
 *     [2] 0x00
 *     [3] @ 0x843E74C  // Current stage   
 *     [4] @ 0x843E874 // m_gameMode
 *     [5] @ 0x843E87E // Some kind of offset... maybe the mtv bga offset?
 *    [94] @0x843E874 != 5 // m_gameMode
 *    [95] m_bPlayType @ 0x083EB860
 * 
 * 4 - allocate 96 bytes - 0x04
 * 5 - allocate 96 bytes - 0x05
 * 6 - allocate 96 bytes - 0x06 
 * 9 - allocate 20 bytes - 0x09
 * 
 * 11 - 
 * 
 * Response - Depends on the normal enumcmd after the data.
 *  0, 1 - These will crash... they don't do anything, either.
 *  2 - instruction_offset + 4, resets buffer offset, loops back
 *  3 - instruction_offset + 0xA00, resets buffer offset, loops back
 *  4 - instruction_offset + 0xA00, resets buffer offset, loops back
 *  5, 6 - 
 *  7 - instruction_offset + 0xA00, resets buffer offset, loops back
 *  8 - instruction_offset + 0xA00, resets buffer offset, loops back
 *  9 -
 *  11 - instruction_offset + 0x50, continues loop
 *  default - instruction_offset += read_buffer_offset, continues loop 
 */
static unsigned char read_count_15;
void process_io_state_15(){
    printf("SongState = %d\n",writeBuffer.data_buffer[0]);
    
    
    switch(writeBuffer.data_buffer[0]){
        case 2:
            readBuffer.enum_cmd = writeBuffer.data_buffer[0];
            readBuffer.buffer_size = 0;
            return;
        case 3:
            readBuffer.enum_cmd = writeBuffer.data_buffer[0];
            readBuffer.buffer_size = 0;
            return;        
        case 4:
            readBuffer.enum_cmd = writeBuffer.data_buffer[0];
            readBuffer.buffer_size = 96;
            return;
        case 6:
            readBuffer.enum_cmd = writeBuffer.data_buffer[0];
            readBuffer.buffer_size = 96;            
            return;
        default:
            printf("Unhandled Song_OP: %d\n",writeBuffer.data_buffer[0]);
            int fd;
            fd = open("out.bin",1);
            real_write(fd,writeBuffer.data_buffer,writeBuffer.buffer_offset);
            close(fd);
            exit(0);
            break;
        
    }
    

    //readBuffer.buffer_size = writeBuffer.buffer_offset;
    //memcpy(readBuffer.data_buffer+0xA00,writeBuffer.data_buffer+5,writeBuffer.buffer_offset-5);
    //memset(readBuffer.data_buffer,current_songcmd,0x1FFF);
    readBuffer.enum_cmd = writeBuffer.data_buffer[0];
    
}

/*
 * Camera Test screen state
 *  Protcol:
 *  Request: Three modes in theory:
 *      0 - do nothing
 *      1 - allocate 4 bytes, 0x00 0x00 0x03 0x00
 *      2 - allocate 4 bytes, 0x00 0x00 0x00 0x00
 *  Response: ???
 */
void process_io_state_25(){
 return;   
}

static int last_mode;
static int initmode;
int a27_read_operation(int fd, void* buf, size_t count){
     
     // Get the readBuffer from the game
    if(!initmode){
        memcpy(&readBuffer,IO_READ_BUFFER_ADDR,sizeof(readBuffer));
        initmode = 1;
    }
     // Do some basic stuff to it
     readBuffer.game_region = game_region;
     strcpy(readBuffer.pch_ext_rom_version_name,PCH_EXT_ROM_VERSION_NAME);
     strcpy(readBuffer.pch_in_rom_version_name,PCH_IN_ROM_VERSION_NAME);
     readBuffer.pci_card_version = PCI_CARD_VERSION;
     
     // Do some control IO stuff
     readBuffer.coin_inserted = get_coin_state();
     readBuffer.drum_io = get_drum_iostate();
     readBuffer.input_matrix_read = 5;
     get_service_button_state();
     
     // TODO - Do some voodoo bullshit that the real ASIC does...
        
    readBuffer.cmdport = writeBuffer.system_mode;
    //printf("SYSTEM MODE: %d\n",writeBuffer.system_mode);
    
     /*
      * There are 27 possible operations (heh) - most of them do nothing. 
      */
     switch(readBuffer.cmdport){
         case 0x01: // A27 Init Test Mode
             process_io_state_1();
             break;
         case 0x02: // I/O Key Test Mode
             process_io_state_2(); 
             break;
         case 0x03: // Light Test Screen
             process_io_state_3(); 
             break;
         case 0x04: // Counter Test Screen
             process_io_state_4(); 
         case 0x05: // Trackball test - never used... probably for another game.
             break;
         case 0x0B: // Coin Page / Title Screen
             process_io_state_11();
             break;
         case 0x0D: // Mode Select Screen
             process_io_state_13();
             break;
         case 0x0E: // Song Select Screen
             process_io_state_14();
             break;
         case 0x0F: // Gameplay / Opening Screen
             process_io_state_15();
         case 0x14: // Ranking Screen
             process_io_state_20();
             break;
         case 0x19: // Camera Test Screen
             process_io_state_25();
             break;
         default:
             break;
         
     }
     
     
     
     
     
     // Fix our checksums
     readBuffer.checksum_1 = io_checksum();
     readBuffer.checksum_2 = readBuffer.checksum_1;
     readBuffer.protection_value = get_protection_value();
     
     // Write dat shit back to the game
     memcpy(IO_READ_BUFFER_ADDR,&readBuffer,sizeof(readBuffer));
     
          
     return 0xF1; // 0xF1 appears to be the only non-error return value.
}

int a27_write_operation(int fd, void* buf, size_t count){
    // TODO - EVERYTHING!!!
    
    memcpy(&writeBuffer,IO_WRITE_BUFFER_ADDR,sizeof(writeBuffer));
     
     /*
     int i;
     printf("Wrote:\n");
     for(i=0;i<writeBuffer.buffer_offset;i++){
         printf("%#X :  ",writeBuffer.data_buffer[i]);
     }
     printf("\n");
     */
     
    
    
    return 1; // I think it doesn't matter what it returns except that it's > 0;
}

static unsigned long a27_offset;
int a27_seek_operation(int fd, unsigned int offset, int whence){
 a27_offset = offset;
 return 0;   
}

int a27_open_operation(int oflag){
    return A27_HANDLE;   
}

// LSEEK WRAPPER
int lseek(int fd, unsigned int offset, int whence){
    if(real_lseek == NULL){
     patch_binary();
     real_lseek = dlsym(RTLD_NEXT, "lseek");
    }
    
    if(fd == A27_HANDLE)
     return a27_seek_operation(fd, offset, whence);
    
    return real_lseek(fd,offset,whence);    
    
    
}



unsigned int read(int fd, void *buf, unsigned int count){
    
    if (real_read == NULL){
      patch_binary();
      real_read = dlsym(RTLD_NEXT, "read");
    }
 
    if(fd == A27_HANDLE)
        return a27_read_operation(fd,buf,count);
    
    return real_read(fd,buf,count);
}
unsigned short curcmd;

unsigned int write(int fd, unsigned char *buf, unsigned int count){
    if (real_write == NULL){
      patch_binary();
      real_write = dlsym(RTLD_NEXT, "write");
    }
    
    if(fd == A27_HANDLE)
        return a27_write_operation(fd,buf,count);
    
    return real_write(fd,buf,count);
}

// Open Wrapper For ASIC27 Emulation
int open(const char * filename, int oflag){

    if (real_open == NULL){
      patch_binary();
      real_open = dlsym(RTLD_NEXT, "open");
    }
    
    if (strcmp(filename, PCCARD_PATH) == 0)
        return a27_open_operation(oflag);

    return real_open(filename, oflag);
}


/*-----------------
 * INITIAL PATCHES
 *----------------*/
void patch_binary()
{
    
    
    
    if(getenv("PM_WINDOWED") != NULL){
        UNPROTECT(0x08056BC8, 4096);
        *((char*)0x08056BC8) = 0x00; //
    }
    
    // Get Game Region Setting
    if(getenv("PM_REGION") != NULL){
     int new_region = atoi(getenv("PM_REGION"));
     game_region = new_region & 0xFF;
    }
    
    // CAS CARD READER FUNCTIONS
    detour_function(&do_nothing, (int)0x08055D4C);
    detour_function(&do_nothing, (int)0x08054E7C);
    detour_function(&do_nothing, (int)0x0805494C);
    detour_function(&do_nothing, (int)0x08054C2C);
    detour_function(&do_nothing, (int)0x0805393C);
    // Webcam functionality -- it's buggy, disabling...
    detour_function(&do_nothing, (int)0x0806B3AC);
}


static void *input_thread(void *arg)
{
    
 int res;
 int fd = open("/run/kbdhook", 0);
    if (fd == -1) {
        perror("Opening input");
        exit(1);
    }else{
     //printf("IO EMULATOR STARTED!\n");   
    }
 while(1){
    res = read(fd, &ev, sizeof(ev));
     if (ev.type == EV_KEY) {
         switch(ev.code){
             // System Controls
             case KEY_ESC:
                 exit(0);
             case KEY_F2:
                 iost.service = (ev.value) ? 1:0;
                 break;
             case KEY_5:
                 iost.coin = (ev.value) ? 1:0;
                 break;
            // Player 1 Controls
             case KEY_X:
                 iost.p1_center_1 = (ev.value) ? 1:0;
                 break;
             case KEY_C:
                 iost.p1_center_2 = (ev.value) ? 1:0;
                 break;
             case KEY_Z:
                 iost.p1_center_left = (ev.value) ? 1:0;
                 break;
             case KEY_V:
                 iost.p1_center_right = (ev.value) ? 1:0;
                 break;
             case KEY_S:
                 iost.p1_upper_left = (ev.value) ? 1:0;
                 break;
             case KEY_F:
                 iost.p1_upper_right = (ev.value) ? 1:0;
                 break;
            // Player 2 Controls
             case KEY_J:
                 iost.p2_center_1 = (ev.value) ? 1:0;
                 break;
             case KEY_K:
                 iost.p2_center_2 = (ev.value) ? 1:0;
                 break;
             case KEY_H:
                 iost.p2_center_left = (ev.value) ? 1:0;
                 break;
             case KEY_L:
                 iost.p2_center_right = (ev.value) ? 1:0;
                 break;
             case KEY_U:
                 iost.p2_upper_left = (ev.value) ? 1:0;
                 break;
             case KEY_I:
                 iost.p2_upper_right = (ev.value) ? 1:0;
                 break;
             default:
                 break;
         }
      if(ev.code == KEY_ESC) exit(0);   // Kill it with FIRE!!!
     }
 }
}

// Entry Point.
void __attribute__((constructor)) initialize(void)
{
    
    printf("Starting up ASIC27_IO_EMU...\n");
    pthread_create(&hthread, 0, input_thread, 0);
    patch_binary();
}

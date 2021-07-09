//
// Created by root on 9/24/16.
//

#include "global.h"

//---[Helper Functions]---

// Generally used to override the start of a function with a jump to a replacement function.
// Note: YOU are responsible for knowing what parameters go into the replacement, if you break the stack,
// it's your fault; with great power comes great responsibility.
void rfx_patch_jmp(void *new_addr, int addr) {
    int call = (int)(new_addr - addr - 5);
    UNPROTECT(addr, 4096);
    *((unsigned char*)(addr)) = 0xE9;
    *((int*)(addr + 1)) = call;
}

// Creates a call to the address at the specified location, use this replace actual calls if you don't want to hook
// all calls or need to hook one instance of an internal call.
void rfx_patch_call(void *new_addr, int addr) {
    int call = (int)(new_addr - addr - 5);
    UNPROTECT(addr, 4096);
    *((unsigned char*)(addr)) = 0xE8;
    *((int*)(addr + 1)) = call;
}

// Used in certain cases such as when a program starts a new thread or process and contains a function address
// parameter (e.g. startNewThread(thread_pool,blah,blah,some_thread_func());.
// Again, point away from face.
void rfx_patch_push_addr(void *new_addr, int addr) {
    UNPROTECT(addr, 4096);
    *((unsigned char*)(addr)) = 0x68;
    *((int*)(addr + 1)) = (int)(new_addr);
}

// Used to statically patch some hardcoded string constant; useful for hardcoded paths, URLs, etc.
void rfx_patch_str(int addr, char* str){
    UNPROTECT(addr,4096);
    strcpy(addr,str);
}

// Used to patch a 1 byte value in a normally non-writeable section (e.g. .text)
void rfx_patch_uint8(int addr,unsigned char val){
    UNPROTECT(addr,4096);
    *(unsigned char*)addr = val;
}

// Used to patch a 2 byte value in a normally non-writeable section (e.g. .text)
void rfx_patch_uint16(int addr,unsigned short val){
    UNPROTECT(addr,4096);
    *(unsigned short*)addr = val;
}

// Used to patch a 4 byte value in a normally non-writeable section (e.g. .text)
void rfx_patch_uint32(int addr,unsigned short val){
    UNPROTECT(addr,4096);
    *(unsigned int*)addr = val;
}

// --- [Nuke Functions] ---

// Returns one, useful for functions like "isDonglePresent", lol.
int rfx_generic_uint32_retone(){return 1;}

// Returns Zero, useful for functions like "isErrorBADBADDIEDIEDIE".
int rfx_generic_uint32_retzero(){return 0;}


// --- [Debugging Functions] ---

// Used to filter debugging printfs
void rfx_printf(char* format,...){
    if(RFX_DEBUG){
        va_list va;
        va_start(va, format);
        char msg[4096] = {0x00};
        vsprintf(msg,format,va);
        printf("%s\n",msg);
    }
}

// Dumps a Hex / Ascii Output block to the console, useful for watching values.
void rfx_dumphex(const void* data, size_t size) {
    char ascii[17];
    size_t i, j;
    ascii[16] = '\0';
    for (i = 0; i < size; ++i) {
        rfx_printf("%02X ", ((unsigned char*)data)[i]);
        if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
            ascii[i % 16] = ((unsigned char*)data)[i];
        } else {
            ascii[i % 16] = '.';
        }
        if ((i+1) % 8 == 0 || i+1 == size) {
            rfx_printf(" ");
            if ((i+1) % 16 == 0) {
                rfx_printf("|  %s \n", ascii);
            } else if (i+1 == size) {
                ascii[(i+1) % 16] = '\0';
                if ((i+1) % 16 <= 8) {
                    rfx_printf(" ");
                }
                for (j = (i+1) % 16; j < 16; ++j) {
                    rfx_printf("   ");
                }
                rfx_printf("|  %s \n", ascii);
            }
        }
    }
}
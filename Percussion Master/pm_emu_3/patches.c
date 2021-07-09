
#include "patches.h"

#include <string.h>
#include <sys/mman.h>

// Helpers
#define UNPROTECT(addr,len) (mprotect((void*)(addr-(addr%len)),len,PROT_READ|PROT_WRITE|PROT_EXEC))

void detour_function(void * new_adr, unsigned int addr){
    int call = (int)(new_adr - addr - 5);
    UNPROTECT(addr, 4096);
    *((unsigned char*)(addr)) = 0xE9;
    *((int*)(addr + 1)) = call;
}


void patch_binary(){
#ifdef PATCH_WINDOWED_ENABLE
    UNPROTECT(PATCH_WINDOWED_OFFSET, 4096);
    *((char*)PATCH_WINDOWED_OFFSET) = 0;
#endif
}
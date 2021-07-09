// Global Patching Tools and Macros by rFx

#ifndef GLOBAL_H
#define GLOBAL_H
// Global Defines
#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

// Controls print statements to clean up output
#define RFX_DEBUG 1


// Global Includes
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdarg.h>

// Global Variables


//---[Helper Macros]---

// Unprotects protected .text regions for static modification of the binary.
#define UNPROTECT(addr,len) (mprotect((void*)(addr-(addr%len)),len,PROT_READ|PROT_WRITE|PROT_EXEC))

// The opposite of UNPROTECT, sets .text regions back to what they normally are.
// This is rarely used, only if the target checks should you ever "re-protect" a region, no point otherwise...
#define PROTECT(addr,len) (mprotect((void*)(addr-(addr%len)),len,PROT_READ|PROT_EXEC))

// ---[Runtime Value Macros]---

// Setter Functions
#define rfx_float_set(addr,val)(*(float*)addr = val)
#define rfx_uint64_set(addr,val)(*(unsigned long long*)addr = val)
#define rfx_uint32_set(addr,val)(*(unsigned int*)addr = val)
#define rfx_uint16_set(addr,val)(*(unsigned short*)addr = val)
#define rfx_uint8_set(addr,val)(*(unsigned char*)addr = val)
#define rfx_uint8_inc(addr)(*(unsigned char*)addr +=1)
#define rfx_uint8_dec(addr)(*(unsigned char*)addr -=1)
#define rfx_int32_set(addr,val)(*(int*)addr = val)
#define rfx_int16_set(addr,val)(*(short*)addr = val)
#define rfx_int8_set(addr,val)(*(char*)addr = val)

// Getter Functions
#define rfx_float_get(addr)(*(float*)addr)
#define rfx_uint64_get(addr)(*(unsigned long long*)addr)
#define rfx_uint32_get(addr)(*(unsigned int*)addr)
#define rfx_uint16_get(addr)(*(unsigned short*)addr)
#define rfx_uint8_get(addr)(*(unsigned char*)addr)
#define rfx_int32_get(addr)(*(int*)addr)
#define rfx_int16_get(addr)(*(short*)addr)
#define rfx_int8_get(addr)(*(char*)addr)

// Function Definitions
void rfx_patch_jmp(void * new_addr, int addr);
void rfx_patch_call(void * new_addr, int addr);
void rfx_patch_push_addr(void* new_addr, int addr);
void rfx_patch_str(int addr, char* str);
void rfx_patch_uint8(int addr,unsigned char val);
void rfx_patch_uint16(int addr,unsigned short val);
void rfx_patch_uint32(int addr,unsigned short val);
int rfx_generic_uint32_retone();
int rfx_generic_uint32_retzero();
void rfx_dumphex(const void* data, size_t size);
void rfx_printf(char* format,...);

#endif // GLOBAL_H

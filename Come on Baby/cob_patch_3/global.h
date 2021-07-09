//
// Created by batte on 4/29/2017.
//

#ifndef STORM_PATCH_GLOBALS_H
#define STORM_PATCH_GLOBALS_H

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

// ZMQ Stuff

// 64K should be more than enough for pretty much everything.
#define TRANSACT_PACKET_MAX 0x1000
// Our shared context for atomic transactions.
static void *z_socket_atomic_ctx;



int init_global(void);
int patch_return_zero(void);
int patch_return_one(void);
void patch_memory_nop(void* addr,unsigned int sz);
void patch_memory_buffer(void* addr, unsigned char* buffer, unsigned int sz);
void detour_jmp(void* new_addr,void* addr);
void detour_call(void* new_addr,void* addr);
void detour_function_ds(void * new_adr, void* addr);
void patch_memory_str(void* addr,char* str);
BOOL Hook_IAT_Name(char* dll_name, char* func_name,DWORD replacement_function_ptr);
BOOL Hook_IAT_Ordinal(char* dll_name,char*func_name,unsigned int ordinal,DWORD replacement_function_ptr);
void engine_init(void);
void engine_shutdown(void);
bool rfx_zmq_transact(unsigned char* request_buffer,unsigned int request_sz,unsigned char* response_buffer, unsigned int* response_size,const char* socket_endpoint);


#endif //STORM_PATCH_GLOBALS_H

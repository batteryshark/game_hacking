#pragma once

#include "../common/target.h"

#define PAGE_SIZE 0x1000

#if TARGET_ARCH_64
#define HOTPATCH_ADDRESS_OFFSET 2
static unsigned char hotpatch_stub[] = {
	0x48,0xB8,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // mov rax, [Abs Jump Address]
	0xFF,0xE0,                                         // jmp rax
	0xC3,                                              // ret
};
#else
#define HOTPATCH_ADDRESS_OFFSET 1
static unsigned char hotpatch_stub[] = {
        0xB8, 0x00, 0x00, 0x00, 0x00, // mov eax, [Abs Jump Address]
        0xFF,0xE0,                    // jmp eax
        0xC3                          // ret
};
#endif

struct HotPatch_Info{
    void*  replacement_function_address;
    void*  original_function_trampoline_address;
    size_t original_function_trampoline_size;
    size_t original_function_padding_size;
    void*  target_address;
    unsigned char target_orig_bytes[16];
};

#ifdef __cplusplus
extern "C" {
#endif
unsigned char MemUtils__get_function_address(const char* lib_name, const char* func_name, void** func_address);
unsigned char MemUtils__heap_alloc_exec_page(void** page_addr);
unsigned char MemUtils__heap_clear_exec_page(void* page_addr);

unsigned char  MemUtils__patch_ret0(void* target_addr);
unsigned char  MemUtils__patch_ret1(void* target_addr);
unsigned char MemUtils__patch_memory(void* target_addr, void* data_ptr, size_t data_len,unsigned char is_write, unsigned char is_exec);
unsigned char MemUtils__hotpatch_function(struct HotPatch_Info* info);
#ifdef __cplusplus
}
#endif

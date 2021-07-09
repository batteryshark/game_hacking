#pragma once
#include <stddef.h>

#if __x86_64__
#define SYSCALL_STUB_SIZE 0x15
#else
#define SYSCALL_STUB_SIZE 0x10
#endif

struct HotPatch_Info {
    void* target_function_address;
    void* replacement_function_address;
    void* trampoline_address;
    size_t trampoline_size;
    unsigned char* target_original_bytes;
    size_t target_original_bytes_size;
};


int load_library(const char* lib_name, void** h_library);
int get_function_address(const char* lib_name, const char* func_name, void** func_address);
int heap_alloc_rwe_page(void** page_addr);
int heap_alloc_rw_page(void** page_addr);
int heap_clear_page(void* page_addr);
int set_permission(void* target_addr, size_t data_len, unsigned char enable_read, unsigned char enable_write, unsigned char enable_execute, unsigned int* old_prot);
int patch_memory(void* target_addr, void* data_ptr, size_t data_len, int is_write, int is_exec);
int patch_ret0(void* target_addr);
int patch_ret1(void* target_addr);
int unhotpatch_function(struct HotPatch_Info* ctx);
int hotpatch_function(void* target_function_address, void* replacement_function_address, size_t target_original_bytes_size, struct HotPatch_Info* ctx, void** ptrampoline_address);
int inline_hook(const char* module_name, const char* func_name, size_t target_original_bytes_size, void* replacement_function, void** original_function);
int inline_hook_memory(void* target_address, void* replacement_address, size_t original_bytes_size, void** original_function);
void detour_jmp(void* new_addr,void* addr);
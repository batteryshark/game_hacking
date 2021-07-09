#include <windows.h>
#include <stddef.h>
#include <stdint.h>
#include <stdarg.h>
#include "ntdll.h"

#include "mem.h"

#define PAGE_SIZE 0x1000

#if __x86_64__
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

    
int load_library(const char* lib_name, void** h_library){
    // If we didn't specify a handle, use a throwaway one.
    if(!h_library){
        void* tmp_h_library = NULL;
        h_library = &tmp_h_library;
    }

    ANSI_STRING astr;
    astr.Length = (USHORT)strlen(lib_name);
    astr.MaximumLength = (USHORT)strlen(lib_name)+1;
    astr.Buffer = (PCHAR)lib_name;
    UNICODE_STRING u_lib_name;
    
    RtlAnsiStringToUnicodeString(&u_lib_name, &astr, TRUE);

    // NTSTATUS res = LdrGetDllHandle(NULL, 0, &u_lib_name, h_library);
    NTSTATUS res = LdrLoadDll(NULL,0,&u_lib_name,h_library);
    RtlFreeUnicodeString(&u_lib_name);
    if (res || !h_library) {return FALSE; }
    return TRUE;
   
}


int get_function_address(const char* lib_name, const char* func_name, void** func_address) {
    if (!func_address) { return 0; }
    *func_address = NULL;
    void* hLibrary = NULL;
    if(!load_library(lib_name,&hLibrary)){return 0;}
    ANSI_STRING astr;
    astr.Length = (USHORT)strlen(func_name);
    astr.MaximumLength = (USHORT)strlen(func_name) + 1;
    astr.Buffer = (PCHAR)func_name;
    if (LdrGetProcedureAddress(hLibrary, &astr, 0, func_address)) {return FALSE;}
    if (!*func_address) { return FALSE; }
    return TRUE;
   
}

int alloc(size_t amt, int is_exec, void** addr) {
    unsigned int access_mask = 0;
    if (!amt) {amt = PAGE_SIZE;}

    if (is_exec) { access_mask = PAGE_EXECUTE_READWRITE; }
    else { access_mask = PAGE_READWRITE; }
    if (NtAllocateVirtualMemory((HANDLE)0xFFFFFFFFFFFFFFFF, addr, 0,(PSIZE_T)&amt, MEM_RESERVE | MEM_COMMIT & 0xFFFFFFC0, access_mask) >= 0) {return 1;}
 
    return 0;
}



int heap_alloc_rwe_page(void** page_addr) {
    *page_addr = NULL;
    return alloc(PAGE_SIZE, 1, page_addr);
}

int heap_alloc_rw_page(void** page_addr) {
    *page_addr = NULL;
    return alloc(PAGE_SIZE, 0, page_addr);
}

int set_permission(void* target_addr, size_t data_len, unsigned char enable_read, unsigned char enable_write, unsigned char enable_execute, unsigned int* old_prot) {
    // Default to PAGE_SIZE if no len.
    if (!data_len) { data_len = PAGE_SIZE; }
    unsigned int n_prot = 0;

    if (*old_prot) {
        n_prot = *old_prot;
    }else{
        if (enable_write && enable_execute) { n_prot = PAGE_EXECUTE_READWRITE; }
        else if (enable_write && enable_read && !enable_execute) { n_prot = PAGE_READWRITE; }
        else { n_prot = PAGE_READONLY; } // IDK
    }
    return NtProtectVirtualMemory((HANDLE)0xFFFFFFFFFFFFFFFF, &target_addr, (PSIZE_T)&data_len, n_prot, (PULONG)old_prot) == 0;
  
    return 1;
}

int patch_memory(void* target_addr, void* data_ptr, size_t data_len, int is_write, int is_exec) {
    unsigned int old_prot = 0;
    if (!is_write) {
        if (!set_permission(target_addr, data_len, 1, 1, is_exec, &old_prot)) { return 0; }
    }

    memcpy(target_addr, data_ptr, data_len);
    if (!is_write) {
        if (!set_permission(target_addr, data_len, 1, 0, is_exec, &old_prot)) { return 0; }
    }

    return 1;
}

int patch_ret0(void* target_addr) {
    unsigned char x86_ret_0[3] = { 0x31,0xC0, 0xC3 };
    return patch_memory(target_addr, x86_ret_0, sizeof(x86_ret_0), 0, 1);
}

int patch_ret1(void* target_addr) {
    unsigned char x86_ret_1[4] = { 0x31,0xC0, 0x40, 0xC3 };
    return patch_memory(target_addr, x86_ret_1, sizeof(x86_ret_1), 0, 1);
}

int unhotpatch_function(struct HotPatch_Info* ctx) {
    return patch_memory(ctx->target_function_address, ctx->target_original_bytes, ctx->target_original_bytes_size, 0, 1);
}

int hotpatch_function(void* target_function_address, void* replacement_function_address, size_t target_original_bytes_size, struct HotPatch_Info* ctx, void** ptrampoline_address) {

    if (!ptrampoline_address || !target_function_address || !replacement_function_address) {return 0; }


    unsigned char hook_stub[sizeof(hotpatch_stub)] = { 0 };
    unsigned char trampoline_stub[sizeof(hotpatch_stub)] = { 0 };

    memcpy(hook_stub, hotpatch_stub, sizeof(hotpatch_stub));
    memcpy(trampoline_stub, hotpatch_stub, sizeof(hotpatch_stub));
    ctx->target_function_address = target_function_address;
    ctx->replacement_function_address = replacement_function_address;
    ctx->target_original_bytes_size = target_original_bytes_size;

    // We'll default the original bytes size to the size of our stub if it's not specified.
    if (!ctx->target_original_bytes_size || ctx->target_original_bytes_size < sizeof(hotpatch_stub)) {
        ctx->target_original_bytes_size = sizeof(hotpatch_stub);
    }
    ctx->target_original_bytes = NULL;
    alloc(ctx->target_original_bytes_size, 0, (void**)&ctx->target_original_bytes);

    // Create our Hook Stub - Backup instructions we're about to modify.
    memcpy(hook_stub + HOTPATCH_ADDRESS_OFFSET, &ctx->replacement_function_address, sizeof(void*));
    memcpy(ctx->target_original_bytes, ctx->target_function_address, ctx->target_original_bytes_size);

    // Allocate an executable page for our trampoline.
    if (!heap_alloc_rwe_page(ptrampoline_address)) {return 0; }
    ctx->trampoline_address = *ptrampoline_address;
    // Write our stolen instructions to the head of the trampoline.
    memcpy(ctx->trampoline_address, ctx->target_original_bytes, ctx->target_original_bytes_size);

    // Calculate what address we're coming back to (the original offset post jump stub).
    unsigned char* actual_return_address = (unsigned char*)ctx->target_function_address;
    actual_return_address = (unsigned char*)(actual_return_address + ctx->target_original_bytes_size);
    memcpy(trampoline_stub + HOTPATCH_ADDRESS_OFFSET, &actual_return_address, sizeof(void*));

    // Write this to our return trampoline.
    memcpy((unsigned char*)ctx->trampoline_address + ctx->target_original_bytes_size, trampoline_stub, sizeof(hotpatch_stub));

    ctx->trampoline_size = ctx->target_original_bytes_size + sizeof(hotpatch_stub);

    // Finally, write the hook stub.
    if (!patch_memory(ctx->target_function_address, hook_stub, sizeof(hotpatch_stub), 0, 1)) { return 0; }

    return 1;
}

int inline_hook(const char* module_name, const char* func_name, size_t target_original_bytes_size, void* replacement_function, void** original_function) {
    void* cf_address = NULL;
    void* throwaway_function_addr = NULL;
    if (!original_function) { original_function = &throwaway_function_addr; }
    struct HotPatch_Info ctx;
    memset(&ctx,0x00,sizeof(struct HotPatch_Info));
    if (!get_function_address(module_name, func_name, &cf_address)) { return 0; }
    if (!hotpatch_function(cf_address, replacement_function, target_original_bytes_size, &ctx, original_function)) { return 0; }
    return 1;
}

int inline_hook_memory(void* target_address, void* replacement_address, size_t original_bytes_size, void** original_function){
    struct HotPatch_Info ctx;
    memset(&ctx,0x00,sizeof(struct HotPatch_Info));    
    void* throwaway_function_addr = NULL;
    if (!original_function) { original_function = &throwaway_function_addr; }    
    return hotpatch_function(target_address, replacement_address, original_bytes_size, &ctx, original_function);
}


// Patch memory to add a jump to an arbitrary address.
void detour_jmp(void* new_addr,void* addr){
    // Calculate Jump Distance.
    DWORD OldProtect = 0;
    size_t jmp_distance = ((size_t)new_addr - (size_t)addr) - 5;
    // Unprotect Memory to Write.
    VirtualProtect((void*)addr,4096, PAGE_EXECUTE_READWRITE,&OldProtect);
    *((unsigned char*)(addr)) = 0xE9;
    *((int*)((DWORD)addr + 1)) = jmp_distance;
}

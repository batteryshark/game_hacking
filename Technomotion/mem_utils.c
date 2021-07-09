#include <string.h>

#include "../common/target.h"

#ifdef TARGET_OS_WINDOWS
#include <libloaderapi.h>
#include <memoryapi.h>
#include <debug.h>

#elif TARGET_OS_LINUX ||TARGET_OS_OSX
#include <dlfcn.h>
#include <sys/mman.h>
// Stupid GNU Thing
#ifndef MAP_ANONYMOUS
#define MAP_ANONYMOUS 0x20
#endif
#endif

#include "mem_utils.h"



unsigned char MemUtils__alloc(size_t amt,unsigned int is_exec, void** addr){
    unsigned int access_mask = 0;
#ifdef TARGET_OS_WINDOWS
    if(is_exec){access_mask = PAGE_EXECUTE_READWRITE;}
    else{access_mask = PAGE_READWRITE;}
    *addr = VirtualAlloc(NULL, PAGE_SIZE, MEM_RESERVE | MEM_COMMIT, PAGE_EXECUTE_READWRITE);
#elif TARGET_OS_LINUX || TARGET_OS_OSX
    if(is_exec){access_mask = PROT_EXEC|PROT_READ|PROT_WRITE;}
    else{access_mask = PROT_READ|PROT_WRITE;}
    *addr = mmap(NULL,amt,access_mask,MAP_ANONYMOUS|MAP_PRIVATE,0,0);
#endif
    return 1;
}

unsigned char MemUtils__free(void* page_addr,size_t num){
#ifdef TARGET_OS_WINDOWS
    VirtualFree(page_addr,num,MEM_RELEASE);
#elif TARGET_OS_LINUX || TARGET_OS_OSX
    munmap(page_addr, num);
#endif
    return 1;
}


unsigned char MemUtils__get_function_address(const char* lib_name, const char* func_name, void** func_address){
    if(!func_address){return 0;}
    *func_address = NULL;
#ifdef TARGET_OS_WINDOWS
    *func_address = GetProcAddress(GetModuleHandleA(lib_name), func_name);
#elif TARGET_OS_LINUX || TARGET_OS_OSX
    void* lib_handle = dlopen(lib_name, RTLD_LOCAL | RTLD_LAZY);
    *func_address = dlsym(lib_handle,func_name);
#endif
    return 1;
}

unsigned char MemUtils__heap_alloc_exec_page(void** page_addr){
    size_t page_sz = PAGE_SIZE;
    *page_addr = NULL;
    if(!MemUtils__alloc(PAGE_SIZE,1,page_addr)){return 0;}
    return 1;
}

unsigned char MemUtils__heap_clear_exec_page(void* page_addr){
    memset(page_addr,0x00,PAGE_SIZE);
    if(!MemUtils__free(page_addr,PAGE_SIZE)){return 0;}
    return 1;
}

unsigned char MemUtils__set_permission(void* target_addr, size_t data_len,  unsigned char enable_read, unsigned char enable_write, unsigned char enable_execute,unsigned int* old_prot){
// Default to PAGE_SIZE if no len.
    if(!data_len){data_len = PAGE_SIZE;}
    unsigned int n_prot = 0;

#ifdef TARGET_OS_WINDOWS
    if(*old_prot){
    n_prot = *old_prot;
}else{
    if(enable_write && enable_execute){n_prot = PAGE_EXECUTE_READWRITE;}
    else if(enable_write && enable_read && !enable_execute){n_prot = PAGE_READWRITE;}
    else{n_prot = PAGE_READONLY;} // IDK
}
VirtualProtect(target_addr,data_len,n_prot,(PDWORD)old_prot);

#else
// TODO: Get Old Protection Flags
    if(enable_write)  {n_prot |= PROT_WRITE;}
    if(enable_read)   {n_prot |= PROT_READ;}
    if(enable_execute){n_prot |= PROT_EXEC;}
    mprotect((void*)(target_addr - ((size_t)target_addr % PAGE_SIZE)),data_len,n_prot);

#endif
    return 1;
}

// TODO: Set Failure conditions.
unsigned char MemUtils__patch_memory(void* target_addr, void* data_ptr, size_t data_len, unsigned char is_write, unsigned char is_exec){
    unsigned int old_prot = 0;
    if(!is_write){
        if(!MemUtils__set_permission(target_addr,data_len,1,1,is_exec,&old_prot)){return 0;}
    }

    memcpy(target_addr,data_ptr,data_len);
    if(!is_write){
        if(!MemUtils__set_permission(target_addr,data_len,1,0,is_exec,&old_prot)){return 0;}
    }

    return 1;
}

unsigned char MemUtils__patch_ret0(void* target_addr){
    unsigned char x86_ret_0[3] = {0x31,0xC0, 0xC3};
    return MemUtils__patch_memory(target_addr,x86_ret_0,sizeof(x86_ret_0),0,1);
}

unsigned char MemUtils__patch_ret1(void* target_addr){
    unsigned char x86_ret_1[4] = {0x31,0xC0, 0x40, 0xC3};
    return MemUtils__patch_memory(target_addr,x86_ret_1,sizeof(x86_ret_1),0,1);
}

unsigned char MemUtils__unhotpatch_function(struct HotPatch_Info* info){
    MemUtils__patch_memory(info->target_address,info->target_orig_bytes,sizeof(info->target_orig_bytes),0,1);
    return 1;
}

unsigned char MemUtils__hotpatch_function(struct HotPatch_Info* info){
    unsigned char stub_code[sizeof(hotpatch_stub)] = { 0 };
    unsigned char trampoline_stub[sizeof(hotpatch_stub)] = {0};
    // We'll default the padding to the size of our hotpatch if it's not specified.
    if(!info->original_function_padding_size){
        info->original_function_padding_size = sizeof(hotpatch_stub);
    }
    DBG_printf("Original Function Padding Size = %d Bytes",info->original_function_padding_size);
    memcpy(stub_code, hotpatch_stub, sizeof(hotpatch_stub));
    memcpy(trampoline_stub, hotpatch_stub, sizeof(hotpatch_stub));
    memcpy(stub_code + HOTPATCH_ADDRESS_OFFSET, &info->replacement_function_address, sizeof(void*));
    memcpy(info->target_orig_bytes,info->target_address,sizeof(info->target_orig_bytes));


    // Allocate another executable page for our original return StubCode
    if(!MemUtils__heap_alloc_exec_page(&info->original_function_trampoline_address)){return 0;}
    memset(info->original_function_trampoline_address,0x00,PAGE_SIZE);
    // Write our stolen bytes to the head of the stubcode address.
    memcpy(info->original_function_trampoline_address,info->target_orig_bytes,info->original_function_padding_size);
    // Calculate what address we're coming back to (the original offset post jump stub).
    void* actual_return_address = info->target_address;
    actual_return_address += info->original_function_padding_size;
    memcpy(trampoline_stub + HOTPATCH_ADDRESS_OFFSET, &actual_return_address, sizeof(void*));
    // Write this to our return trampoline.
    memcpy(info->original_function_trampoline_address+info->original_function_padding_size,trampoline_stub,sizeof(stub_code));

    info->original_function_trampoline_size = info->original_function_padding_size + sizeof(stub_code);
    if(!MemUtils__patch_memory(info->target_address,stub_code,sizeof(hotpatch_stub),0,1)){
        DBG_printf("MemUtils__patch_memory Failed!");
        return 0;
    }

    return 1;
}


#include <memoryapi.h>
#include  <libloaderapi.h>

#include "debug.h"

#include "patching.h"


void patch_process(void* target_addr, unsigned char* patch_data, unsigned int patch_length){
    DWORD old_protect;
    VirtualProtect(target_addr, patch_length, PAGE_EXECUTE_READWRITE, &old_protect);
    memcpy(target_addr, patch_data, patch_length);
    VirtualProtect(target_addr,1,old_protect,&old_protect);
}

BOOL hook_iat_by_name (char* target_module, char* dll_name, char* func_name,DWORD replacement_function_ptr){
    IMAGE_DOS_HEADER *pDOSHeader;
    IMAGE_NT_HEADERS *pNTHeader;
    IMAGE_IMPORT_DESCRIPTOR *ImportDirectory;
    DWORD *OriginalFirstThunk;
    DWORD *FirstThunk;
    DWORD *address;
    DWORD *func_address;
    char *module_name="";
    DWORD overwrite;
    char *name;
    HANDLE hHandle;
    DWORD oldProtect;
    DWORD PEHeaderOffset;
    int i=0;
// Let's make sure the target is loaded first.

    if(!target_module){
        hHandle = GetModuleHandle(NULL);
    }else{
        hHandle = GetModuleHandleA(target_module);
    }



    if(hHandle == NULL){
        DBG_printf("there was an error in retrieving the handle");
        return FALSE;
    }
    DBG_printf("Got Module Handle");
    pDOSHeader = (IMAGE_DOS_HEADER *) hHandle;

    PEHeaderOffset = (DWORD) pDOSHeader->e_lfanew;

    pNTHeader = (IMAGE_NT_HEADERS *) ((DWORD) hHandle + PEHeaderOffset);

    ImportDirectory = (IMAGE_IMPORT_DESCRIPTOR *) ((DWORD) pNTHeader->OptionalHeader.DataDirectory[1].VirtualAddress + (DWORD) hHandle);
    module_name = (char *)(ImportDirectory->Name + (DWORD) hHandle);
    DBG_printf("Searching Modules...");
    while(stricmp(module_name, dll_name) != 0){
        DBG_printf(module_name);
        ImportDirectory++;
        module_name = (char *)(ImportDirectory->Name + (DWORD) hHandle);
    }
    DBG_printf("Found DLL Name");

    OriginalFirstThunk = (DWORD *)((DWORD) ImportDirectory->OriginalFirstThunk + (DWORD) hHandle);
    FirstThunk = (DWORD *)((DWORD) ImportDirectory->FirstThunk + (DWORD) hHandle);


    while(*(OriginalFirstThunk+i) != 0x00000000){
        name = (char *) (*(OriginalFirstThunk+i) + (DWORD) hHandle + 0x2);

        if(stricmp(name, func_name) == 0){
            DBG_printf("Found Function Name");
            address=OriginalFirstThunk+i;
            break;
        }
        i++;
    }

    func_address = FirstThunk - OriginalFirstThunk + address;
    patch_process(func_address, (unsigned char*)&replacement_function_ptr ,sizeof(void*));
    return TRUE;
}
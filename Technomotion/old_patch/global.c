#include <Windows.h>
#include <stdio.h>
#include <debugapi.h>
#include <stdarg.h>

#include "global.h"

unsigned char patch_running;

void DBG_printf(const char* format, ...) {
#ifdef ENABLE_DEBUG
    char s[8192];
    va_list args;
    ZeroMemory(s, 8192 * sizeof(s[0]));
    va_start(args, format);
    vsnprintf(s, 8191, format, args);
    va_end(args);
    s[8191] = 0;
    OutputDebugStringA(s);
#endif
}


BOOL Hook_IAT_Name (char* dll_name, char* func_name,DWORD replacement_function_ptr){
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

    hHandle = GetModuleHandle(NULL);

    if(hHandle == NULL){
        OutputDebugStr("there was an error in retrieving the handle");
       return FALSE;
    }

    pDOSHeader = (IMAGE_DOS_HEADER *) hHandle;

    PEHeaderOffset = (DWORD) pDOSHeader->e_lfanew;

    pNTHeader = (IMAGE_NT_HEADERS *) ((DWORD) hHandle + PEHeaderOffset);

    ImportDirectory = (IMAGE_IMPORT_DESCRIPTOR *) ((DWORD) pNTHeader->OptionalHeader.DataDirectory[1].VirtualAddress + (DWORD) hHandle);
    module_name = (char *)(ImportDirectory->Name + (DWORD) hHandle);

    while(stricmp(module_name, dll_name) != 0){
        ImportDirectory++;
        module_name = (char *)(ImportDirectory->Name + (DWORD) hHandle);
    }


    OriginalFirstThunk = (DWORD *)((DWORD) ImportDirectory->OriginalFirstThunk + (DWORD) hHandle);
    FirstThunk = (DWORD *)((DWORD) ImportDirectory->FirstThunk + (DWORD) hHandle);


    while(*(OriginalFirstThunk+i) != 0x00000000){
        name = (char *) (*(OriginalFirstThunk+i) + (DWORD) hHandle + 0x2);

        if(stricmp(name, func_name) == 0)
        {
            address=OriginalFirstThunk+i;
            break;
        }
        i++;
    }

    func_address = FirstThunk - OriginalFirstThunk + address;
    VirtualProtect(func_address, 0x4, 0x40, &oldProtect);
    overwrite = (DWORD) replacement_function_ptr;
    WriteProcessMemory(0xffffffff, func_address, &overwrite, 0x4, NULL);
    VirtualProtect(func_address, 0x4, 0x20, &oldProtect);
    return TRUE;
}
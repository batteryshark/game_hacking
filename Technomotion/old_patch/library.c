// Technomotion IO Emulator
#include <Windows.h>
#include <string.h>

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "global.h"
#include "cmos_rtc.h"

#include "technomotion_io.h"

#define EXPORTABLE __declspec(dllexport)

#define ADDR_WINDOWED_FLAG 0x402A9A
#define ADDR_ENABLE_EAX2_FLAG 0x440357
#define ADDR_REL_PATCH_1 0x462AA0
#define HANGUL_CODEPAGE 949

bool is_ascii(const char* inpath){
    int i = 0;
    for(i=0;i<strlen(inpath);i++){
        if(!isprint(inpath[i])){
            return false;
        }
    }
    return true;
}
// Convert CodePage and Do Unicode Instead!
static inline void WININET_find_data_WtoA(LPWIN32_FIND_DATAW dataW, LPWIN32_FIND_DATAA dataA)
{
    dataA->dwFileAttributes = dataW->dwFileAttributes;
    dataA->ftCreationTime   = dataW->ftCreationTime;
    dataA->ftLastAccessTime = dataW->ftLastAccessTime;
    dataA->ftLastWriteTime  = dataW->ftLastWriteTime;
    dataA->nFileSizeHigh    = dataW->nFileSizeHigh;
    dataA->nFileSizeLow     = dataW->nFileSizeLow;
    dataA->dwReserved0      = dataW->dwReserved0;
    dataA->dwReserved1      = dataW->dwReserved1;
    WideCharToMultiByte(HANGUL_CODEPAGE, 0, dataW->cFileName, -1,
        dataA->cFileName, sizeof(dataA->cFileName),
        NULL, NULL);
    WideCharToMultiByte(HANGUL_CODEPAGE, 0, dataW->cAlternateFileName, -1,
        dataA->cAlternateFileName, sizeof(dataA->cAlternateFileName),
        NULL, NULL);
}

BOOL __stdcall (*real_SetCurrentDirectoryA)(LPCSTR lpPathName) = NULL;
BOOL __stdcall HK_SetCurrentDirectoryA(LPCSTR lpPathName){
    if(lpPathName != NULL && ! is_ascii(lpPathName)) {
        wchar_t w_file_path[1024] = {0x00};
        memset(w_file_path,0x00,sizeof(w_file_path));
        if(!MultiByteToWideChar(HANGUL_CODEPAGE, 0, lpPathName, -1, w_file_path, sizeof(w_file_path))){
            return real_SetCurrentDirectoryA(lpPathName);
        }
        BOOL result = SetCurrentDirectoryW(w_file_path);
        memset(w_file_path,0x00,sizeof(w_file_path));
        return result;
    }
    return real_SetCurrentDirectoryA(lpPathName);
}

HANDLE  __stdcall (*real_FindFirstFileA)(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData) = NULL;
HANDLE __stdcall HK_FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData){
if(lpFileName != NULL && ! is_ascii(lpFileName)){
    wchar_t w_file_path[1024] = {0x00};
    memset(w_file_path,0x00,sizeof(w_file_path));
    if(!MultiByteToWideChar(HANGUL_CODEPAGE, 0, lpFileName, -1, w_file_path, sizeof(w_file_path))){
    // If it doesn't work, we'll just skip it.
    return real_FindFirstFileA(lpFileName, lpFindFileData);
    }
    // DO IT
    LPWIN32_FIND_DATAW lpFindFileDataW = NULL;
    HANDLE hFile = FindFirstFileW(w_file_path,lpFindFileDataW);
    WININET_find_data_WtoA(lpFindFileDataW,lpFindFileData);

    memset(w_file_path,0x00,sizeof(w_file_path));
    return hFile;

}
return real_FindFirstFileA(lpFileName,lpFindFileData);
}

HANDLE __stdcall (*real_CreateFileA)(LPCSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile) = NULL;
HANDLE __stdcall HK_CreateFileA(LPCSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile){
    if(lpFileName != NULL && !is_ascii(lpFileName)){
        OutputDebugStringA("Unprintable CreatefileA Reached!");
        wchar_t w_file_path[1024] = {0x00};
        memset(w_file_path,0x00,sizeof(w_file_path));
        if(!MultiByteToWideChar(HANGUL_CODEPAGE, 0, lpFileName, -1, w_file_path, sizeof(w_file_path))){
            // If it doesn't work, we'll just skip it.
            OutputDebugStringA("CreatefileA Fallback!");
            return real_CreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
        }
        OutputDebugStringA("Unprintable CreatefileW GOGOGO!");
        HANDLE hFile = CreateFileW(w_file_path,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
        OutputDebugStringA("CreatefileW Returned!");
        memset(w_file_path,0x00,sizeof(w_file_path));
        return hFile;
    }

    return real_CreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
}

void patch_windowed(){
    DWORD old_protect;
    VirtualProtect((void*)ADDR_WINDOWED_FLAG,1,PAGE_EXECUTE_READWRITE,&old_protect);
    memset((void*)ADDR_WINDOWED_FLAG,0x00,1);
    VirtualProtect((void*)ADDR_WINDOWED_FLAG,1,old_protect,&old_protect);
}

void patch_eax2(){
    DWORD old_protect;
    VirtualProtect((void*)ADDR_ENABLE_EAX2_FLAG,1,PAGE_EXECUTE_READWRITE,&old_protect);
    memset((void*)ADDR_ENABLE_EAX2_FLAG,0x00,1);
    VirtualProtect((void*)ADDR_ENABLE_EAX2_FLAG,1,old_protect,&old_protect);
}

// The Win95 application assumed ANSI and CP949 which will break on most non-korean applications... let's fix this!
void patch_fs(){
    real_CreateFileA = (void*)GetProcAddress(LoadLibraryA("KERNEL32.DLL"),"CreateFileA");
    real_FindFirstFileA = (void*)GetProcAddress(LoadLibraryA("KERNEL32.DLL"),"FindFirstFileA");
    real_SetCurrentDirectoryA = (void*)GetProcAddress(LoadLibraryA("KERNEL32.DLL"),"SetCurrentDirectoryA");
    Hook_IAT_Name("KERNEL32.dll","CreateFileA",(DWORD)&HK_CreateFileA);
    Hook_IAT_Name("KERNEL32.dll","FindFirstFileA",(DWORD)&HK_FindFirstFileA);
    Hook_IAT_Name("KERNEL32.dll","SetCurrentDirectoryA",(DWORD)&HK_SetCurrentDirectoryA);
}
void patch_rel_path_1(){
    DWORD old_protect;
    VirtualProtect((void*)ADDR_REL_PATCH_1,24,PAGE_EXECUTE_READWRITE,&old_protect);
    strcpy((void*)ADDR_REL_PATCH_1,".\\BlackBox.tab");
    VirtualProtect((void*)ADDR_REL_PATCH_1,24,old_protect,&old_protect);
}



#define ASM_OUT_DX_AL 0xEE
#define ASM_IN_AL_DX  0xEC

#define ASM_IN_AL  0xE4
#define ASM_OUT_AL 0xE6



LONG WINAPI priv_instr_exception_handler(PEXCEPTION_POINTERS pExceptionInfo){
    // If this isn't what we want, skip it...
    if(pExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_PRIV_INSTRUCTION) {
        return EXCEPTION_EXECUTE_HANDLER;
    }
    // Determine OpCode
    unsigned char opcode = *(unsigned char*)pExceptionInfo->ContextRecord->Eip;
    unsigned short port_16 = 0;
    unsigned char port_8 = 0;
    switch(opcode){
        case ASM_OUT_DX_AL: // Technomotion IO Output
            port_16 = pExceptionInfo->ContextRecord->Edx & 0xFFFF;
            if(port_16 >= TIO_OUT_CHANNEL_0 && port_16 <= TIO_OUT_CHANNEL_4){
                tio_set_value(port_16,pExceptionInfo->ContextRecord->Eax & 0xFF);
            }
            break;
        case ASM_IN_AL_DX: // Technomotion IO Input
            port_16 = pExceptionInfo->ContextRecord->Edx & 0xFFFF;
            if(port_16 >= TIO_IN_CHANNEL_0 && port_16 <= TIO_IN_CHANNEL_3){
                pExceptionInfo->ContextRecord->Eax = tio_get_value(port_16);
            }
            break;
        case ASM_OUT_AL: // CMOS RTC OUTPUT
            port_8 = *(unsigned char*)pExceptionInfo->ContextRecord->Eip+1;
            pExceptionInfo->ContextRecord->Eip++;
            if(port_8 == PORT_CMOS_RTOS_OUT){
                set_rtc_register_index(pExceptionInfo->ContextRecord->Eax & 0xFF);
            }
            break;
        case ASM_IN_AL: // CMOS RTC INPUT
            port_8 = *(unsigned char*)pExceptionInfo->ContextRecord->Eip+1;
            pExceptionInfo->ContextRecord->Eip++;
            if(port_8 == PORT_CMOS_RTOS_IN){
                pExceptionInfo->ContextRecord->Eax = get_rtc_register();
            }
            break;
        default:
            DBG_printf("ERROR: Unsupported OpCode: %02X",opcode);
            return EXCEPTION_EXECUTE_HANDLER;

    }

    pExceptionInfo->ContextRecord->Eip++;
    return EXCEPTION_CONTINUE_EXECUTION;
}



bool patch_game(){

    // Initialize our CMOS RTC Thread.
    init_cmos_rtc();
    // Initialize our IO Thread
    tio_init();
    // Set any Binary Patches
    //patch_windowed();
    patch_eax2();
    patch_rel_path_1();
    // Patch to Support OG Korean Paths.
    patch_fs();

    // Set Exception Filter for IO
    SetUnhandledExceptionFilter(priv_instr_exception_handler);
    patch_running = 1;
    return true;
}


// Entry-Point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        if(patch_game()){
            DBG_printf("TechnoIO Attached!");
            return TRUE;
        }
        return FALSE;
    }
    return TRUE;
}

// DirectDraw Entry Points
EXPORTABLE void fuckstick(){}
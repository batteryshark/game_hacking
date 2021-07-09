

#include "fs.h"

#include "global.h"
#include <string.h>

#define SWP_HANDLE 0xB00B79

#define SWP_SZ 84

unsigned char swp_data[84] = {
0x09, 0x00, 0x00, 0x00, 0x38, 0x34, 0x30, 0x37, 0x36, 0x33, 0x36, 0x34, 0x35, 0x00, 0x00, 0x00,
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x73, 0x03, 0x5F, 0x02, 0x8B, 0x03, 0x6B, 0x02, 0x00, 0x00,
0x4E, 0x15, 0xC8, 0x0C, 0x43, 0x01, 0x67, 0x03, 0x5F, 0x02, 0xAF, 0x03, 0x6B, 0x02, 0x67, 0x03,
0x5F, 0x02, 0xA8, 0x03, 0x6B, 0x02, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
0x30, 0x36, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xDE, 0x15, 0xD0, 0x00, 0x5E, 0x09, 0x3E, 0x87,
0xBF, 0x2B, 0x2E, 0x87
};

HANDLE __stdcall rfx_CreateFileA(LPCTSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile){

    if(strstr(lpFileName,"c:\\windows\\win386p.swp") != 0){
        return SWP_HANDLE;
    }

    return CreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
}

// ReadFile Hook
BOOL __stdcall rfx_ReadFile(HANDLE hFile, LPVOID lpBuffer, DWORD nNumberOfBytesToRead, LPDWORD lpNumberOfBytesRead, LPOVERLAPPED lpOverlapped){
    if(hFile == SWP_HANDLE){
        memcpy(lpBuffer,swp_data,SWP_SZ);
        *lpNumberOfBytesRead = SWP_SZ;
        return true;
    }

    return ReadFile(hFile,lpBuffer,nNumberOfBytesToRead,lpNumberOfBytesRead,lpOverlapped);
}

BOOL __stdcall rfx_CloseHandle(HANDLE hObject){
    if(hObject != SWP_HANDLE){return CloseHandle(hObject);}
    return TRUE;
}

void patch_fs(){
    Hook_IAT_Name("KERNEL32.dll","CreateFileA",&rfx_CreateFileA);
    Hook_IAT_Name("KERNEL32.dll","ReadFile",&rfx_ReadFile);
    Hook_IAT_Name("KERNEL32.dll","CloseHandle",&rfx_CloseHandle);
}
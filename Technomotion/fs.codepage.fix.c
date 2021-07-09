// Codepage Conversion Library for Process FileIO
#include <Windows.h>
#include "kitchen_sink/kitchen_sink.h"


typedef HANDLE __stdcall tCreateFileA(LPCSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile);
typedef HANDLE __stdcall tFindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData);
typedef BOOL   __stdcall tSetCurrentDirectoryA(LPCSTR lpPathName);

static tSetCurrentDirectoryA* real_SetCurrentDirectoryA = NULL;
static tFindFirstFileA* real_FindFirstFileA = NULL;
static tCreateFileA* real_CreateFileA = NULL;



// Default CodePage
static unsigned int target_codepage = 65001;

// For a given string, evaluate if it's printable ascii.
#define PRINTABLE_ASCII_MAX 0x7E
#define PRINTABLE_ASCII_MIN 0x20
unsigned char is_printable_ascii(const char* instr){
    int i = 0;
    for(i=0;i<strlen(instr);i++){
        if(instr[i] > PRINTABLE_ASCII_MAX || instr[i] < PRINTABLE_ASCII_MIN){ return FALSE;}
    }
    return TRUE;
}

BOOL __stdcall x_SetCurrentDirectoryA(LPCSTR lpPathName){

    if(lpPathName && !is_printable_ascii(lpPathName)){
        wchar_t w_file_path[1024] = {0x00};
        if(MultiByteToWideChar(target_codepage, 0, lpPathName, -1, w_file_path, sizeof(w_file_path))){
            return SetCurrentDirectoryW(w_file_path);
        }
    }
    // Bypass Conversion
    return real_SetCurrentDirectoryA(lpPathName);
}

HANDLE __stdcall x_FindFirstFileA(LPCSTR lpFileName, LPWIN32_FIND_DATAA lpFindFileData){

    if(lpFileName && !is_printable_ascii(lpFileName)){
        wchar_t w_file_path[1024] = {0x00};
        if(MultiByteToWideChar(target_codepage, 0, lpFileName, -1, w_file_path, sizeof(w_file_path))){
            LPWIN32_FIND_DATAW lpFindFileDataW = NULL;
            HANDLE hFile = FindFirstFileW(w_file_path,lpFindFileDataW);
            lpFindFileData->dwFileAttributes = lpFindFileDataW->dwFileAttributes;
            lpFindFileData->ftCreationTime   = lpFindFileDataW->ftCreationTime;
            lpFindFileData->ftLastAccessTime = lpFindFileDataW->ftLastAccessTime;
            lpFindFileData->ftLastWriteTime  = lpFindFileDataW->ftLastWriteTime;
            lpFindFileData->nFileSizeHigh    = lpFindFileDataW->nFileSizeHigh;
            lpFindFileData->nFileSizeLow     = lpFindFileDataW->nFileSizeLow;
            lpFindFileData->dwReserved0      = lpFindFileDataW->dwReserved0;
            lpFindFileData->dwReserved1      = lpFindFileDataW->dwReserved1;
            WideCharToMultiByte(target_codepage, 0, lpFindFileDataW->cFileName, -1,
                                lpFindFileData->cFileName, sizeof(lpFindFileData->cFileName),
                                NULL, NULL);
            WideCharToMultiByte(target_codepage, 0, lpFindFileDataW->cAlternateFileName, -1,
                                lpFindFileData->cAlternateFileName, sizeof(lpFindFileData->cAlternateFileName),
                                NULL, NULL);
            return hFile;
        }
    }
    return real_FindFirstFileA(lpFileName, lpFindFileData);
}


HANDLE __stdcall x_CreateFileA(LPCSTR lpFileName,DWORD dwDesiredAccess,DWORD dwShareMode,LPSECURITY_ATTRIBUTES lpSecurityAttributes,DWORD dwCreationDisposition,DWORD dwFlagsAndAttributes,HANDLE hTemplateFile){

    if(lpFileName && !is_printable_ascii(lpFileName)){
        wchar_t w_file_path[1024] = {0x00};
        if(MultiByteToWideChar(target_codepage, 0, lpFileName, -1, w_file_path, sizeof(w_file_path))){
            return CreateFileW(w_file_path,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
        }
    }
    return real_CreateFileA(lpFileName,dwDesiredAccess,dwShareMode,lpSecurityAttributes,dwCreationDisposition,dwFlagsAndAttributes,hTemplateFile);
}


// Set our relative bindings and default codepage.
BOOL codepage_convert__init(unsigned int codepage){
    target_codepage = codepage;

    HotPatch_patch("kernel32.dll","CreateFileA",0x0A,x_CreateFileA,(void**)&real_CreateFileA);
    HotPatch_patch("kernel32.dll","FindFirstFileA",0x08,x_FindFirstFileA,(void**)&real_FindFirstFileA);
    HotPatch_patch("kernel32.dll","SetCurrentDirectoryA",0x0D,x_SetCurrentDirectoryA,(void**)&real_SetCurrentDirectoryA);

}

void init_fs_codepage_fix(){
    codepage_convert__init(949);
}

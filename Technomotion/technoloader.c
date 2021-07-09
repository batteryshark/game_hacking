#include <Windows.h>
#include <stdio.h>

static char* dll_path = "D:\\techmo\\technotools.dll";

static char full_exe_path[4096];

void createShellcode(int ret, int str, unsigned char** shellcode, int* shellcodeSize){
    HMODULE k32 = GetModuleHandleA("kernel32.dll");
    if (!k32) { return; }
    FARPROC lla_addr = GetProcAddress(k32, "LoadLibraryA");
    if (!lla_addr) { return; }

    unsigned char* retChar = (unsigned char*)&ret;
    unsigned char* strChar = (unsigned char*)&str;
    int api = (int)lla_addr;
    unsigned char* apiChar = (unsigned char*)&api;
    unsigned char sc[] = {
            // Push ret
            0x68, retChar[0], retChar[1], retChar[2], retChar[3],
            // Push all flags
            0x9C,
            // Push all register
            0x60,
            // Push 0x66666666 (later we convert it to the string of injected dll)
            0x68, strChar[0], strChar[1], strChar[2], strChar[3],
            // Mov eax, 0x66666666 (later we convert it to LoadLibrary adress)
            0xB8, apiChar[0], apiChar[1], apiChar[2], apiChar[3],
            // Call eax
            0xFF, 0xD0,
            // Pop all register
            0x61,
            // Pop all flags
            0x9D,
            // Ret
            0xC3
    };

    *shellcodeSize = 22;
    *shellcode = (unsigned char*)malloc(22);
    memcpy(*shellcode, sc, 22);
}

static BOOL GetDirectoryPath(LPSTR lpFilename, LPSTR in_path, DWORD nSize) {
    if (!in_path) {
        GetModuleFileNameA(GetModuleHandleA(0), lpFilename, nSize);
    }
    else {
        strcpy(lpFilename, full_exe_path);
    }

    char* last_delimiter = strrchr(lpFilename, 0x5C);
    if (!last_delimiter) { return FALSE; }
    memset(last_delimiter + 1, 0x00, 1);
    return TRUE;
}

void usage() {
    printf("Usage: loader.exe path_to_exe\n");
    exit(-1);
}

int main(int argc, char* argv[]) {

    if (argc < 2) { usage(); }

    unsigned char* shellcode;
    int shellcodeLen;

    LPVOID remote_dllStringPtr;
    LPVOID remote_shellcodePtr;

    CONTEXT ctx;
    // Get the Full EXE path for the exe.
    GetFullPathName(argv[1], 4096, full_exe_path, NULL);

    // Get the Full DLL Path for our injection.

    SetEnvironmentVariableA("CONFIG_INI_PATH","D:\\techmo\\technotools.ini");


    char* exe_base_path = (char*)malloc(1024);
    GetDirectoryPath(exe_base_path,full_exe_path, 1024);

    size_t lib_path_len = strlen(dll_path) + 1;

    // Create Process SUSPENDED
    PROCESS_INFORMATION pi;
    STARTUPINFOA Startup;
    ZeroMemory(&Startup, sizeof(Startup));
    ZeroMemory(&pi, sizeof(pi));

    CreateProcessA(full_exe_path, NULL, NULL, NULL, FALSE, CREATE_SUSPENDED, NULL, exe_base_path, &Startup, &pi);
    free(exe_base_path);

    remote_dllStringPtr = VirtualAllocEx(pi.hProcess, NULL, lib_path_len, MEM_COMMIT, PAGE_READWRITE);

    ctx.ContextFlags = CONTEXT_CONTROL;
    GetThreadContext(pi.hThread, &ctx);


    createShellcode(ctx.Eip, (int)remote_dllStringPtr, &shellcode, &shellcodeLen);
    if (remote_dllStringPtr) {
        WriteProcessMemory(pi.hProcess, remote_dllStringPtr, dll_path, lib_path_len, NULL);
    }

    // Allocate Memory for Shellcode
    remote_shellcodePtr = VirtualAllocEx(pi.hProcess, NULL, shellcodeLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
    if (remote_shellcodePtr) {
        WriteProcessMemory(pi.hProcess, remote_shellcodePtr, shellcode, shellcodeLen, NULL);
    }


    // Set EIP To Shellcode
    ctx.Eip = (DWORD)remote_shellcodePtr;
    ctx.ContextFlags = CONTEXT_CONTROL;
    SetThreadContext(pi.hThread, &ctx);

    ResumeThread(pi.hThread);

    Sleep(800); // Might want to turn this down... 8 seconds is a lot.

    if (remote_dllStringPtr) {
#pragma warning(suppress: 28160)
#pragma warning(suppress: 6250)
        VirtualFreeEx(pi.hProcess, remote_dllStringPtr, lib_path_len, MEM_DECOMMIT);
    }
    if(remote_shellcodePtr){
#pragma warning(suppress: 28160)
#pragma warning(suppress: 6250)
        VirtualFreeEx(pi.hProcess, remote_shellcodePtr, shellcodeLen, MEM_DECOMMIT);
    }

    return 0;
}
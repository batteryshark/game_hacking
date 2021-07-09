#include <Windows.h>
#include "kitchen_sink.h"

void create_shellcode(int ret, int str, unsigned char** shellcode, int* shellcodeSize){
	unsigned char* retChar = (unsigned char*)&ret;
	unsigned char* strChar = (unsigned char*)&str;
	int api = (int)GetProcAddress(LoadLibraryA("kernel32.dll"), "LoadLibraryA");
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

// ContextSwitch a suspended process and then resume with a remote thread.
BOOL InjectProcessCtxSwitch(LPPROCESS_INFORMATION lpProcessInformation, char* path_to_dll, int jiggle_handle, int leave_suspended) {
	CONTEXT ctx;
	ctx.ContextFlags = CONTEXT_CONTROL;
	GetThreadContext(lpProcessInformation->hThread, &ctx);
	if (jiggle_handle) {
		ResumeThread(lpProcessInformation->hThread);
		Sleep(10);
		SuspendThread(lpProcessInformation->hThread);
	}

	size_t lib_path_len = strlen(path_to_dll) + 1;
	LPVOID dll_string_ptr = VirtualAllocEx(lpProcessInformation->hProcess, NULL, lib_path_len, MEM_COMMIT, PAGE_READWRITE);
	// Build Shellcode
	unsigned char* shellcode;
	int shellcodeLen;

	create_shellcode(ctx.Eip, (int)dll_string_ptr, &shellcode, &shellcodeLen);
	// Allocate heap memory for shellcode.
	LPVOID shellcode_ptr = VirtualAllocEx(lpProcessInformation->hProcess, NULL, shellcodeLen, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	// Write DLL Path To Remote Process
	
	if (!WriteProcessMemory(lpProcessInformation->hProcess, dll_string_ptr, path_to_dll, lib_path_len, NULL)) {
		return FALSE;
	}
	
	// Write Shellcode To Remote Process
	if (!WriteProcessMemory(lpProcessInformation->hProcess, shellcode_ptr, shellcode, shellcodeLen, NULL)) {
		return FALSE;
	}
	
	// Set EIP To Shellcode
	DWORD backup_ctx_flags = ctx.ContextFlags;
	ctx.Eip = (DWORD)shellcode_ptr;
	ctx.ContextFlags = CONTEXT_CONTROL;
	SetThreadContext(lpProcessInformation->hThread, &ctx);
	
	// Run The Shellcode
	ResumeThread(lpProcessInformation->hThread);
	ctx.ContextFlags = backup_ctx_flags;
	// Wait until code is executed
	Sleep(800);

	
	// Cleanup
	VirtualFreeEx(lpProcessInformation->hProcess, dll_string_ptr, lib_path_len, MEM_DECOMMIT);
	VirtualFreeEx(lpProcessInformation->hProcess, shellcode_ptr, shellcodeLen, MEM_DECOMMIT);

	return TRUE;

}
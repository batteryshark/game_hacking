// EXPotato AC Emulator
#include <Windows.h>
#include <string.h>

#include <stdint.h>
#include <stdbool.h>
#include <ctype.h>

#include "global.h"


#include "expotato_io.h"


#define EXPORTABLE __declspec(dllexport)

#define ASM_OUT_DX_AL 0xEE
#define ASM_IN_AL_DX  0xEC

HANDLE __stdcall (*real_CreateFileA)(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) = NULL;
HANDLE __stdcall HK_CreateFileA(LPCSTR lpFileName, DWORD dwDesiredAccess, DWORD dwShareMode, LPSECURITY_ATTRIBUTES lpSecurityAttributes, DWORD dwCreationDisposition, DWORD dwFlagsAndAttributes, HANDLE hTemplateFile) {
	if (lpFileName != NULL && !is_ascii(lpFileName)) {
		OutputDebugStringA("Unprintable CreatefileA Reached!");
		wchar_t w_file_path[1024] = { 0x00 };
		memset(w_file_path, 0x00, sizeof(w_file_path));
		if (!MultiByteToWideChar(HANGUL_CODEPAGE, 0, lpFileName, -1, w_file_path, sizeof(w_file_path))) {
			// If it doesn't work, we'll just skip it.
			OutputDebugStringA("CreatefileA Fallback!");
			return real_CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		}
		OutputDebugStringA("Unprintable CreatefileW GOGOGO!");
		HANDLE hFile = CreateFileW(w_file_path, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
		OutputDebugStringA("CreatefileW Returned!");
		memset(w_file_path, 0x00, sizeof(w_file_path));
		return hFile;
	}

	return real_CreateFileA(lpFileName, dwDesiredAccess, dwShareMode, lpSecurityAttributes, dwCreationDisposition, dwFlagsAndAttributes, hTemplateFile);
}


// The Win95 application assumed ANSI and CP949 which will break on most non-korean applications... let's fix this!
void patch_hddcheck() {
	real_CreateFileA = (void*)GetProcAddress(LoadLibraryA("KERNEL32.DLL"), "CreateFileA");
	Hook_IAT_Name("KERNEL32.dll", "CreateFileA", (DWORD)& HK_CreateFileA);
}


LONG WINAPI priv_instr_exception_handler(PEXCEPTION_POINTERS pExceptionInfo) {
	// If this isn't what we want, skip it...
	if (pExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_PRIV_INSTRUCTION) {
		return EXCEPTION_EXECUTE_HANDLER;
	}
	// Determine OpCode
	unsigned char opcode = *(unsigned char*)pExceptionInfo->ContextRecord->Eip;
	unsigned short port_16 = 0;
	unsigned char port_8 = 0;
	switch (opcode) {
	case ASM_OUT_DX_AL: // IO Output
		port_16 = pExceptionInfo->ContextRecord->Edx & 0xFFFF;
		io_set_value(port_16, pExceptionInfo->ContextRecord->Eax & 0xFF);
		break;
	case ASM_IN_AL_DX: // IO Input
		port_16 = pExceptionInfo->ContextRecord->Edx & 0xFFFF;
		pExceptionInfo->ContextRecord->Eax = tio_get_value(port_16);
		break;
	default:
		DBG_printf("ERROR: Unsupported OpCode: %02X", opcode);
		return EXCEPTION_EXECUTE_HANDLER;

	}

	pExceptionInfo->ContextRecord->Eip++;
	return EXCEPTION_CONTINUE_EXECUTION;
}



bool do_patch() {

	// Initialize our IO Thread
	io_init();

	// Init our HDD Protection Patch
	hdd_patch();

	// Set Exception Filter for IO
	SetUnhandledExceptionFilter(priv_instr_exception_handler);
	patch_running = 1;
	return true;
}


// Entry-Point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		if (do_patch()) {
			return TRUE;
		}
		return FALSE;
	}
	return TRUE;
}

// DirectDraw Entry Points
EXPORTABLE void fuckstick() {}
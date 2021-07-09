// Batman HASP Handling
// TODO: Replace this with actual HASP emulator instead of api hooking.
#include <windows.h>
#include <stdio.h>


#include "../shared/mem.h"


//Definitions
#define ADDR_HASP_LOGIN     (void*)0x16F5A39
#define ADDR_HASP_LOGOUT    (void*)0x16A5C7C
#define ADDR_HASP_READ      (void*)0x16D3C6C
#define ADDR_HASP_WRITE     (void*)0x16E7A74
#define ADDR_MISC_HASPCHECK (void*)0x1384D80
#define HASP_FAKE_HANDLE 1337

static unsigned char hasp_data[32] = {
	0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
	0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
	0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00 ,0x00,
	0x00 ,0x00 ,0x00 ,0x00 ,0x01 ,0x00 ,0x00 ,0x00
};

// Log into hasp dongle.
unsigned int __stdcall hk_hasp_login(int feature_id, void* vendor_code, int* hasp_handle) {	
	*hasp_handle = HASP_FAKE_HANDLE;
	return 0;
}

// Read HASP Data from buffer.
unsigned int __stdcall hk_hasp_read(unsigned int hasp_handle, int hasp_fileid, unsigned int offset, unsigned int length, unsigned char* buffer) {
	memcpy(buffer, hasp_data, length);
	return 0;
}

// Write HASP Data to buffer.
unsigned int __stdcall hk_hasp_write(unsigned int hasp_handle, int hasp_fileid, unsigned int offset, unsigned int length, unsigned char* buffer) {
		memcpy(hasp_data, buffer, length);
	return 0;
}

void patch_hasp() {
	detour_jmp(ADDR_HASP_LOGIN,(DWORD*)&hk_hasp_login);
	patch_ret0(ADDR_HASP_LOGOUT);
	detour_jmp(ADDR_HASP_READ,(DWORD*)&hk_hasp_write);
	detour_jmp(ADDR_HASP_WRITE,(DWORD*)&hk_hasp_read);
	patch_ret1(ADDR_MISC_HASPCHECK);

}



// Entry-Point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
	if (fdwReason == DLL_PROCESS_ATTACH) {
		patch_hasp();		
	}
	return TRUE;
}

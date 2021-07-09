// Come on Baby Arcade Patch
// Compile (mingw): gcc -shared -m32 -w ./cob_patch.c -lws2_32 -o ./cob_patch.dll
#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <TlHelp32.h>
#include <string.h>



// General Defines
#define EXAMPLE_DLL __declspec(dllexport)
#define IMAGE_EXE_MODULE_NAME "COBDEMO.EXE"
#define IMAGEBASE_RAW_ALIGN 0x000

// Static Globals
static unsigned int cpid = NULL;
static DWORD_PTR base_address = NULL;



void detour_function(void * new_adr, int addr)
{
	unsigned int OldProtect = 0;
	unsigned int Temp = 0;
    int call = (int)(new_adr - addr - 5);
    //UNPROTECT(addr, 4096);
	VirtualProtect(addr,4096, PAGE_EXECUTE_READWRITE, &OldProtect);
    *((unsigned char*)(addr)) = 0xE9;
    *((int*)(addr + 1)) = call;
	VirtualProtect(addr,4096, OldProtect, &Temp);
}


/* Helper Functions */
int generic_ret1(){return 1;}
int generic_ret0(){return 0;}
void generic_ret(){}

#define VK_F2 0x71
#define VK_F3 0x72
#define VK_F12 0x7B


#define INPUT_MODE_P1_CONTROL 0
#define INPUT_MODE_P1_BUTTONS 1
#define INPUT_MODE_P2_CONTROL 2
#define INPUT_MODE_P2_BUTTONS 3
#define INPUT_MODE_SYSTEM 4
#define IO_OK 5
#define INPUT_MODE_CHECK 8

/*
Reads an input code (optionally, a value from @eax, but that is always 0).
Depending on the input code, a byte is returned and inverted to signal which buttons
are held.

In addition, a mode of 8 signals an IO check function which expects a return value of 
5 (most likely, 5 meaning bit 0 and bit 2 are set).

*/
int get_hardware_input(unsigned char input_code){
	
	// Added a way to suicide the game.
	if(GetAsyncKeyState(VK_F12) & 0x8000){
		OutputDebugString("Closing Game");
		exit(0);
	}
	
	int value = 0;
	// Note - Controls flags are bit-inverted.
	switch(input_code){
		
		case INPUT_MODE_P1_CONTROL:
			if(!(GetAsyncKeyState(VK_UP) & 0x8000)){value |= 1;}      // P1_UP
			if(!(GetAsyncKeyState(VK_DOWN) & 0x8000)){value |= 2;}    // P1_DOWN
			if(!(GetAsyncKeyState(VK_LEFT) & 0x8000)){value |= 4;}    // P1_LEFT
			if(!(GetAsyncKeyState(VK_RIGHT) & 0x8000)){value |= 8;}   // P1_RIGHT
			return value;
		
		case INPUT_MODE_P1_BUTTONS:
			if(!(GetAsyncKeyState('Z') & 0x8000)){value |= 1;}       // P1_A
			if(!(GetAsyncKeyState('X') & 0x8000)){value |= 2;}       // P1_B
			if(!(GetAsyncKeyState('C') & 0x8000)){value |= 4;}       // P1_C+D
			return value;
			
		case INPUT_MODE_P2_CONTROL:
			if(!(GetAsyncKeyState('W') & 0x8000)){value |= 1;}       // P2_UP
			if(!(GetAsyncKeyState('S') & 0x8000)){value |= 2;}       // P2_DOWN
			if(!(GetAsyncKeyState('A') & 0x8000)){value |= 4;}       // P2_LEFT
			if(!(GetAsyncKeyState('D') & 0x8000)){value |= 8;}       // P2_RIGHT
			return value;
			
		case INPUT_MODE_P2_BUTTONS:
			if(!(GetAsyncKeyState('G') & 0x8000)){value |= 1;}       // P2_A
			if(!(GetAsyncKeyState('H') & 0x8000)){value |= 2;}       // P2_B
			if(!(GetAsyncKeyState('J') & 0x8000)){value |= 4;}       // P2_C+D		
			return value;
			
		case INPUT_MODE_SYSTEM:
			if(!(GetAsyncKeyState(VK_F2) & 0x8000)){value |= 1;}      // SYS_TEST
			if(!(GetAsyncKeyState(VK_F3) & 0x8000)){value |= 2;}      // SYS_SERVICE
			if(!(GetAsyncKeyState('5') & 0x8000)){value |= 4;}       // SYS_COIN	
			return value;
			
		case INPUT_MODE_CHECK:
			return IO_OK;
			
		default:
			return value;
	}	
}

// Sends hardware output signals (maybe for lights etc) and normally returns 15.
// TODO: Optimize this code to look less shit.
char set_hardware_output(int input_mode,unsigned char output_data){
	// out dword_532744, output_data
	char result;
	char msg[1024]={0x00};
	sprintf(msg,"Hardware Output Mode: %d",input_mode);
	OutputDebugString(msg);
	if(input_mode){
		if(input_mode == 1){
			// out dword_5326FC, 7
			result = 15;
			// out dword_5326FC, 0x0F
			return result;
		}
		if(input_mode == 2){
			// out dword_5326FC, 0x0B
			result = 15;
			// out dword_5326FC, 0x0F
			return result;
		}
		
	}else{
		// out dword_5326FC, 3
	}
	result = 15;
	// out dword_5326FC, 0x0F
	return result;
}

// Initializes hardware and generally returns 15.
int check_hw_output(){
	// out dword_5326FC, 0x0F
	set_hardware_output(0,0);
	set_hardware_output(1,0);
	int result = set_hardware_output(2,0);
	return result;
}

// Some offsets of functions we are going to hijack.
#define COB_HW_OUTPUT_OFFSET 0x125A0
#define COB_CHECK_OUTPUT 0x125F0
#define COB_SERIAL_CHECK_OFFSET 0x12790
#define COB_GET_HW_INPUT_OFFSET 0x126D0

// COB 2 Offsets
//#define COB_HW_OUTPUT_OFFSET 0x1F640
//#define COB_CHECK_OUTPUT 0x1F690
//#define COB_SERIAL_CHECK_OFFSET 0x1F6F0
//#define COB_GET_HW_INPUT_OFFSET 0x1F6C0

/* Initialization Logic */
void patch_binary(){
	// Patch out game serial check.
	detour_function(&generic_ret1,base_address+COB_SERIAL_CHECK_OFFSET);
	detour_function(&get_hardware_input,base_address+COB_GET_HW_INPUT_OFFSET);
	detour_function(&set_hardware_output,base_address+COB_HW_OUTPUT_OFFSET);
	detour_function(&check_hw_output,base_address+COB_CHECK_OUTPUT);
}

// Returns base offset of the executable module with the raw alignment offset.
void resolve_base_address(void){
	if(cpid == NULL){
		cpid = GetCurrentProcessId();
		unsigned char generic_debug_msgbuf[1024] = {0x00};
		sprintf(generic_debug_msgbuf,"Patch: PID is %d",cpid);
		OutputDebugString(generic_debug_msgbuf);
		if(cpid == NULL){
			OutputDebugString("FATAL: Couldn't Get Process ID");
			exit(1);
		}
		// Get Base Address for Module
		base_address = (DWORD_PTR)GetModuleHandleA(IMAGE_EXE_MODULE_NAME) + IMAGEBASE_RAW_ALIGN;
	}
}


// Entry-Point Function
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){
    if (fdwReason == DLL_PROCESS_ATTACH){
		resolve_base_address();
		patch_binary();
    }
    if (fdwReason == DLL_PROCESS_DETACH){}
    return TRUE;
}

// Piggyback Function
void EXAMPLE_DLL fuckstick(void){}
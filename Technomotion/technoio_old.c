// Technomotion IO Emulator
#include <Windows.h>
#include <string.h>
#include <stdio.h>
#define EXPORTABLE __declspec(dllexport)

unsigned char coin_value = 0x00;
unsigned char p1_scratch = 0x00;
unsigned char p2_scratch = 0x00;


#define OPCODE_OUT 0xEE
#define OPCODE8_OUT 0xE6
#define OPCODE8_IN 0xE4
#define OPCODE_IN  0xEC

#define PORT_SYSTEM_SWITCHES 0x2C0
#define PORT_P1_KEYS         0x2C1
#define PORT_P1_TT           0x2C2
#define PORT_P2_TT           0x104
#define PORT_COIN            0x105
#define PORT_P2_KEYS         0x2C3
#define CMOS_RTOS_IN           0x71
#define CMOS_RTOS_OUT       0x70

unsigned char cmos_data[128] = {
	0x03, 0xC1, 0x30, 0x44, 0x00, 0x40, 0x10, 0x01, 0x01, 0x00, 0x26, 0x02, 0x40, 0x80, 0x00, 0x00,
	0x00, 0x26, 0xF0, 0x00, 0x0F, 0x80, 0x02, 0x00, 0xFC, 0x2F, 0x00, 0x10, 0xF7, 0x01, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAD, 0x4D, 0x10, 0xFF, 0xFF, 0xAC, 0x4D, 0x3F, 0x08, 0x1A,
	0x00, 0xFC, 0x20, 0x80, 0x00, 0x32, 0xCD, 0x06, 0x64, 0x4C, 0xCD, 0x0F, 0x99, 0xD1, 0xA9, 0x11,
	0xC2, 0x51, 0x70, 0x14, 0xC0, 0x40, 0x01, 0x00, 0x6F, 0xF0, 0x85, 0x28, 0x1C, 0xDC, 0x40, 0x07,
	0x88, 0x00, 0x00, 0x00, 0x05, 0x80, 0x56, 0x15, 0x6A, 0x05, 0xE3, 0xBD, 0x32, 0x00, 0x03, 0x90,
	0x4C, 0x27, 0xDF, 0xF7, 0x01, 0x0B, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x6E, 0x0E, 0x51, 0x00, 0x00, 0x7C
};

unsigned char current_cmos_address = 0x00;

int get_cmos_input(unsigned char input_port) {
	return cmos_data[current_cmos_address];
}

void set_cmos_output(unsigned char index, unsigned char value) {
	current_cmos_address = index;
	cmos_data[index] = value;
	return;
}

int get_hardware_input(unsigned short input_code) {
	unsigned char msg[1024] = { 0x00 };
	// Added a way to suicide the game.
	if (GetAsyncKeyState(VK_F12) & 0x8000) {
		OutputDebugString("Closing Game");
		exit(0);
	}

	int value = 0;
	// Note - Controls flags are bit-inverted.
	switch (input_code) {

	case PORT_SYSTEM_SWITCHES:
		if ((GetAsyncKeyState('1') & 0x8000)) { value |= 1; }      // P1 Start
		if ((GetAsyncKeyState('2') & 0x8000)) { value |= 2; }      // P2 Start
		if ((GetAsyncKeyState(VK_F5) & 0x8000)) { value |= 4; }    // Effect 1
		if ((GetAsyncKeyState(VK_F6) & 0x8000)) { value |= 8; }    // Effect 2 
		if ((GetAsyncKeyState(VK_F7) & 0x8000)) { value |= 0x10; } // Effect 3
		if ((GetAsyncKeyState(VK_F8) & 0x8000)) { value |= 0x20; } // Effect 4
		if ((GetAsyncKeyState(VK_F1) & 0x8000)) { value |= 0x40; } // Service Button
		if ((GetAsyncKeyState(VK_F2) & 0x8000)) { value |= 0x80; } // Test Button
		return value;

	case PORT_P1_KEYS:
		if ((GetAsyncKeyState('X') & 0x8000)) { value |= 1; }       // P1 Key 1
		if ((GetAsyncKeyState('C') & 0x8000)) { value |= 2; }       // P1 Key 2
		if ((GetAsyncKeyState('V') & 0x8000)) { value |= 4; }       // P1 Key 3
		if ((GetAsyncKeyState('B') & 0x8000)) { value |= 8; }       // P1 Key 4
		if ((GetAsyncKeyState('N') & 0x8000)) { value |= 0x10; }    // P1 Key 5
		if ((GetAsyncKeyState('M') & 0x8000)) { value |= 0x80; }    // P1 Key Pedal
		return value;

	case PORT_P1_TT:
		value = p1_scratch;
		if ((GetAsyncKeyState('Z') & 0x8000)) { value += 1; }     // P1 Scratch Gameplay
		if ((GetAsyncKeyState(VK_DOWN) & 0x8000)) { value += 1; } // P1 Scratch+ Menu
		if ((GetAsyncKeyState(VK_UP) & 0x8000)) { value -= 1; } // P1 Scratch- Menu

		p1_scratch = value % 255;
		return p1_scratch;

	case PORT_P2_TT: // Scratch 2?
		value = p2_scratch;
		if ((GetAsyncKeyState('A') & 0x8000)) { value += 1; }     // P2 Scratch Gameplay
		if ((GetAsyncKeyState(VK_DOWN) & 0x8000)) { value += 1; } // P2 Scratch+ Menu
		if ((GetAsyncKeyState(VK_UP) & 0x8000)) { value -= 1; }   // P2 Scratch- Menu

		p2_scratch = value % 255;
		return p2_scratch;

	case PORT_COIN: // Coin 1 Mech
		value = coin_value;
		if ((GetAsyncKeyState('5') & 0x8000)) { value++; }
		coin_value = value % 255;
		return ~coin_value;

	case PORT_P2_KEYS:
		if ((GetAsyncKeyState('S') & 0x8000)) { value |= 1; }       // P2 Key 1
		if ((GetAsyncKeyState('D') & 0x8000)) { value |= 2; }       // P2 Key 2
		if ((GetAsyncKeyState('F') & 0x8000)) { value |= 4; }       // P2 Key 3
		if ((GetAsyncKeyState('G') & 0x8000)) { value |= 8; }       // P2 Key 4
		if ((GetAsyncKeyState('H') & 0x8000)) { value |= 0x10; }    // P2 Key 5
		if ((GetAsyncKeyState('J') & 0x8000)) { value |= 0x80; }    // P2 Key Pedal
		return value;

	default:

		sprintf_s(msg, sizeof(msg), "Unhandled Input Port: %04X", input_code);
		OutputDebugString(msg);
		return value;
	}
	return value;
}

// TODO: Give a shit about Neon Lights and output in general.
void set_hardware_output(unsigned short output_port, unsigned char output_value) {
	//unsigned char msg[1024] = { 0x00 };
	//sprintf_s(msg,sizeof(msg), "-Out- Port: %04X, Value: %04X", output_port, output_value);
	//OutputDebugString(msg);
	// 0x100 -> 0x103

}

// Retrieves the current instruction byte to determine input/output.
unsigned char get_opcode_8(const void* address) {
	unsigned char opcode_8 = 0x00;
	memcpy(&opcode_8, address, 1);
	return opcode_8;
}

// Privileged Instruction Vector Handler
LONG WINAPI pinst_handler(PEXCEPTION_POINTERS pExceptionInfo) {

	DWORD dwCode = pExceptionInfo->ExceptionRecord->ExceptionCode;


	if (dwCode & EXCEPTION_PRIV_INSTRUCTION) {
		unsigned char msg[1024] = { 0x00 };
		unsigned short port_number = pExceptionInfo->ContextRecord->Edx & 0xFFFF;

		unsigned char opcode = get_opcode_8((const void*)pExceptionInfo->ContextRecord->Eip);
		unsigned char port_8 = get_opcode_8((const void*)(pExceptionInfo->ContextRecord->Eip+1));
		unsigned char val_8 = pExceptionInfo->ContextRecord->Eax & 0xFF;
		switch (opcode) {
		case OPCODE_OUT:
			set_hardware_output(port_number, pExceptionInfo->ContextRecord->Eax & 0xFF);
			break;
		case OPCODE_IN:
			pExceptionInfo->ContextRecord->Eax = ~get_hardware_input(port_number);
			break;

		case OPCODE8_IN:
			// Bump the bitch ahead one byte due to 2 byte calls.
			sprintf_s(msg, sizeof(msg), "IN8 %02X", port_8);
			OutputDebugString(msg);
			pExceptionInfo->ContextRecord->Eip++;			
			pExceptionInfo->ContextRecord->Eax = get_cmos_input(port_8);
			
			break;
		case OPCODE8_OUT:
			// Bump the bitch ahead one byte due to 2 byte calls.
			sprintf_s(msg, sizeof(msg), "OUT8 %02X %02X", port_8,val_8);
			OutputDebugString(msg);
			pExceptionInfo->ContextRecord->Eip++;
			set_cmos_output(port_8, val_8);
			break;

		default:
			sprintf_s(msg, sizeof(msg), "Unhandled PI Opcode: %02X", opcode);
			OutputDebugString(msg);
			break;
		}

		// Bump the bitch ahead one byte because these are always 1 byte hardware IO calls.
		pExceptionInfo->ContextRecord->Eip++;
		return EXCEPTION_CONTINUE_EXECUTION;
	}
	return EXCEPTION_EXECUTE_HANDLER;
}




// Entry-Point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {

	if (fdwReason == DLL_PROCESS_ATTACH) {
		OutputDebugStringA("TechnoIO Attached!");
		SetUnhandledExceptionFilter(pinst_handler);
		//AddVectoredExceptionHandler(TRUE, (PVECTORED_EXCEPTION_HANDLER)pinst_handler);
	}
	return TRUE;
}

// Our Drop
void EXPORTABLE fuckstick(void) {};
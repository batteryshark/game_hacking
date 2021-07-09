#include <windows.h>
#include <xinput.h>
#include "xinput_gamepad.h"

#include "usio_batman.h"

#define USIO_READ_INPUTS 2
#define USIO_UNK_OP 1
#define USIO_BUFFER_MAX 0x40
#define KEYPAD_OFFSET 20
#define KEYPAD_HASH 0x40
#define KEYPAD_STAR 0x50
#define KEYPAD_9 0x60
#define KEYPAD_8 0x70
#define KEYPAD_7 0x80
#define KEYPAD_6 0x90
#define KEYPAD_5 0xA0
#define KEYPAD_4 0xB0
#define KEYPAD_3 0xC0
#define KEYPAD_2 0xD0
#define KEYPAD_1 0xE0
#define KEYPAD_0 0xF0

// Get Keypad Switch State (Use a Real Keyboard for This)
unsigned char get_keypad_input() {
	unsigned char keypad_val = 0xFF;
	if ((GetAsyncKeyState(VK_NUMPAD0) & 0x8000)) { keypad_val &= ~KEYPAD_0; }
	if ((GetAsyncKeyState(VK_NUMPAD1) & 0x8000)) { keypad_val &= ~KEYPAD_1; }
	if ((GetAsyncKeyState(VK_NUMPAD2) & 0x8000)) { keypad_val &= ~KEYPAD_2; }
	if ((GetAsyncKeyState(VK_NUMPAD3) & 0x8000)) { keypad_val &= ~KEYPAD_3; }
	if ((GetAsyncKeyState(VK_NUMPAD4) & 0x8000)) { keypad_val &= ~KEYPAD_4; }
	if ((GetAsyncKeyState(VK_NUMPAD5) & 0x8000)) { keypad_val &= ~KEYPAD_5; }
	if ((GetAsyncKeyState(VK_NUMPAD6) & 0x8000)) { keypad_val &= ~KEYPAD_6; }
	if ((GetAsyncKeyState(VK_NUMPAD7) & 0x8000)) { keypad_val &= ~KEYPAD_7; }
	if ((GetAsyncKeyState(VK_NUMPAD8) & 0x8000)) { keypad_val &= ~KEYPAD_8; }
	if ((GetAsyncKeyState(VK_NUMPAD9) & 0x8000)) { keypad_val &= ~KEYPAD_9; }
	if ((GetAsyncKeyState(VK_DECIMAL) & 0x8000)) { keypad_val &= ~KEYPAD_STAR; }
	if ((GetAsyncKeyState(VK_RETURN) & 0x8000)) { keypad_val &= ~KEYPAD_HASH; }
	return keypad_val;
}

/* Get control state based on XInput values and keystate values.
--[IO LAYOUT]--
00 - Input ID - must be 2 or the input won't work, 1 is alternative input, 0 crashes (they use an index table to find the function -_-)
01 - *UNUSED*
02 - *UNUSED*
03 - COIN 1: 0x10, COIN 2: 0x01 -- Upper bits are the coinslot state stuff (jamming etc)
05 - TEST: 0x10, SERVICE CREDIT: 0x01
06 - Volume Up: 0x10 , Volume Down: 0x01
07 - *UNUSED*
08 - Fire: 0x10, Digital Brake: 0x01
09 - P1 START: 0x10, P2 START: 0x01
10 - *UNUSED*
11 - *UNUSED*
12 - *UNUSED*
13 - digital analog 1
14 - digital analog 2
16 - analog 1
17 - analog 2
18 - analog 3
19 - analog 4
20 - Keypad
*/

void get_usio_input(unsigned char* input_data) {
	update_gamepad();
	memset(input_data,0x00,USIO_BUFFER_MAX);
	input_data[0] = USIO_READ_INPUTS;
	// Get Switches
	if (gamepad_p1.btn_back) { input_data[3] |= 0x10; } // coin_1
	if (gamepad_p1.btn_y) { input_data[3] |= 0x01; } // coin_2
	if (gamepad_p1.btn_l3) { input_data[5] |= 0x01; } // service
	if (gamepad_p1.btn_r3) { input_data[5] |= 0x10; } // test
	if (gamepad_p1.dpad_down) { input_data[6] |= 0x01; } // volume down
	if (gamepad_p1.dpad_up) { input_data[6] |= 0x10; } // volume up
	if (gamepad_p1.btn_x) { input_data[8] |= 0x01; } // digital brake
	if (gamepad_p1.btn_a) { input_data[8] |= 0x10; } // fire
	if (gamepad_p1.btn_start) { input_data[9] |= 0x10; } // start



	// Get-Set Analog
	input_data[16] = get_analog_stick_scaled(gamepad_p1.analog_lx, (float) 0.0, (float) 255.0) & 0xFF;
	input_data[17] = gamepad_p1.trigger_r2;
	input_data[18] = get_analog_stick_scaled(gamepad_p1.analog_ry, (float) 0.0, (float) 255.0) & 0xFF;
	input_data[19] = gamepad_p1.trigger_l2;
	// Get-Set Keypad
	input_data[KEYPAD_OFFSET] = get_keypad_input();
}

void process_usio_output(unsigned char* output_data, unsigned int output_length){
	return;
}
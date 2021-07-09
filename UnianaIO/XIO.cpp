#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "XIO.h"

// Added to the brake whenever the brake is depressed.
#define HARD_BRAKE_MULTIPLIER 128
#define SOFT_BRAKE_MULTIPLIER 64

/*
Globals
*/
XIO_API unsigned char g_bKeyImm = 63; // Last input state of buttons or maybe from game?
XIO_API unsigned char g_bKeyExc; // Current input state of buttons to be sent back to game.
XIO_API unsigned char g_bBrake; // Guessing the Brake state.
XIO_API unsigned char g_bHandle = 128; // Guessing the Handlebar state.
XIO_API unsigned char g_bAccelerator; // Accelerator State?
XIO_API unsigned char g_bLAccel; // Left Side Accelerator?
XIO_API unsigned char g_bLAccelDif; // Last state of Left Accelerator?
XIO_API unsigned char g_bLAccelEnd; // no idea...
XIO_API unsigned char g_bRAccel; // Right Side Accelerator?
XIO_API unsigned char g_bRAccelDif; // Last state of Right Accelerator?
XIO_API unsigned char g_bRAccelEnd; // no idea...
XIO_API unsigned char lastLeftPedalVelocity; // no idea...
XIO_API unsigned char lastRightPedalVelocity; // no idea...
XIO_API unsigned char g_bLAccelDifLast; // no idea...
XIO_API unsigned char g_bRAccelDifLast; // no idea...
XIO_API unsigned char leftPedalVelocity = 0;
XIO_API unsigned char rightPedalVelocity = 0;

/*
Simple Constructor for XIO Object.
*/
XIO::XIO() {
	m3 = 3;
	m4 = 3;
	pedalData = 0;
}

/*
Destructor for XIO Object.
*/
XIO::~XIO() {
	closeInterface();
}

/*
Name: XIO::openInterface
Purpose: Sets up a new interface to the hardware.
Input: None
Output: int result
*/
int XIO::openInterface() {
	initialize();
	return 1;
}

/*
Name: XIO::closeInterface
Purpose: Resets hardware.
Input: None
Output: None
*/
void XIO::closeInterface() {
	initLamp();
	setVibration(3, 1);
	resetPedal();
}

/*
Name: XIO::initialize
Purpose: Initializes the IO Device.
Inputs: None
Outputs: None
*/
void XIO::initialize() {
	initLamp();
	initSteering();
	initPedal();
	initVibration();
}



/*
Name:XIO::initLamp
Purpose: Initializes the lamp state.
Input:Nothing
Output: Nothing
*/
void XIO::initLamp() {
	lampCoinIOData = 0xD0;
	lamp1 = 0xFF;
	lamp2 = 0xFF;
}

/*
Name:XIO::initVibration
Purpose: Initializes the Vibration State.
Input: Nothing
Output: Nothing
*/
void XIO::initVibration() {
	vibrationData = 3;
}

/*
Name:XIO::initSteering
Purpose: Initializes the Steering State.
Input: Nothing
Output: Nothing
*/
void XIO::initSteering() {
	steeringData = 128;
}

/*
Name:XIO::initPedal
Purpose: Initializes the Pedal State.
Input: Nothing
Output: Nothing
*/
void XIO::initPedal() {
	lastLeftPedalVelocity = 0;
	lastRightPedalVelocity = 0;
	g_bLAccelEnd = 0;
	g_bRAccelEnd = 0;
	g_bLAccelDif = 0;
	g_bRAccelDif = 0;
	g_bLAccelDifLast = 0;
	g_bRAccelDifLast = 0;
	g_bLAccel = 0;
	g_bRAccel = 0;
	resetPedal();
}

/*
Name:XIO::resetPedal
Purpose: Resets the Pedal State.
Input: Nothing
Output: Nothing
*/
void XIO::resetPedal() {
	pedalData |= 0x22;
	pedalData &= 0xEE;
	pedalData |= 0x11;
	pedalData &= 0xDD;
}

/*
Name:XIO::selectLamp
Purpose: Sets lamp state for the select button?
Input: lampState (uchar)
Output: Nothing
*/
void XIO::selectLamp(int lampState) {
	unsigned char lampDataNew;
	if (lampState) {
		lampDataNew = lampCoinIOData & 0xEF;
	}
	else {
		lampDataNew = lampCoinIOData | 0x10;
	}
	lampCoinIOData = lampDataNew;
}

/*
Name:XIO::startLamp
Purpose: Sets lamp state for the start button?
Input: lampState (uchar)
Output: Nothing
*/
void XIO::startLamp(int lampState) {
	unsigned char lampDataNew;
	if (lampState) {
		lampDataNew = lampCoinIOData | 8;
	}
	else {
		lampDataNew = lampCoinIOData & 0xF7;
	}
	lampCoinIOData = lampDataNew;
}

/*
Name:XIO::leftLamp
Purpose: Sets lamp state for the left button?
Input: lampState (uchar)
Output: Nothing
*/
void XIO::leftLamp(unsigned char lampState) {
	unsigned char lampDataNew;
	if (lampState) {
		lampDataNew = lampCoinIOData & 0xBF;
	}
	else {
		lampDataNew = lampCoinIOData | 0x40;
	}
	lampCoinIOData = lampDataNew;
}

/*
Name:XIO::rightLamp
Purpose: Sets lamp state for the right button?
Input: lampState (uchar)
Output: Nothing
*/
void XIO::rightLamp(unsigned char lampState) {
	unsigned char lampDataNew;
	if (lampState) {
		lampDataNew = lampCoinIOData & 0x7F;
	}
	else {
		lampDataNew = lampCoinIOData | 0x80;
	}
	lampCoinIOData = lampDataNew;
}

/*
Name:XIO::lamp
Purpose: No Idea Yet
Input: lampState
Output: Nothing
*/
void XIO::lamp(unsigned char lampState) {
	if (lampState & 1) {
		lampCoinIOData |= 0xD8;
		return;
	}
	if (lampState & 2) {
		lampCoinIOData = lampCoinIOData & 0xE7 | 0xC0;
		return;
	}
	if (lampState & 4) {
		lampCoinIOData = lampCoinIOData & 0x27 | 0x90;
	}
	else {
		if (lampState & 8) {
			lampCoinIOData = lampCoinIOData & 0x27 | 0x50;
			return;
		}
		lampCoinIOData = lampCoinIOData & 0xF7 | 0xD0;
	}
}

/*
Scan Keyboard Input and set gvals.
*/
#define VK_1 0x31
#define VK_Z 0x5A
#define VK_X 0x58
static const unsigned char playerKeys[6] = { VK_F3,VK_F2,VK_1,VK_RETURN,VK_LEFT,VK_RIGHT };
void XIO::keyboardInput() {
	unsigned char result = 0xFF;
	//Button Logic
	for (int i = 0; i < sizeof(playerKeys); i++) {
		if (GetAsyncKeyState(playerKeys[i])) {
			result ^= (1 << i);
		}
	}
	g_bKeyExc = result;

	// Steering  Logic
	result = steeringData;
	if (GetAsyncKeyState(VK_LEFT)) {
		if (result - 32 <= 0) {
			result = 0;
		}
		else {
			result -= 32;
		}
	}

	if (GetAsyncKeyState(VK_RIGHT)) {
		if (result + 32 >= 254) {
			result = 254;
		}
		else {
			result += 32;
		}
	}
	
	if (!GetAsyncKeyState(VK_RIGHT) && !GetAsyncKeyState(VK_LEFT)) {
		if (result <= 128) {
			if (result + 32 >= 128) {
				result = 128;
			}
			else {
				result += 32;
			}
		}
		else {
			if (result - 32 <= 128) {
				result = 128;
			}
			else {
				result -= 32;
			}
		}
	}
	steeringData = result;
	
	// Accelerator Logic
	if (GetAsyncKeyState(VK_X)) {
		if (rightPedalVelocity >= 254) {
			rightPedalVelocity = 254;
		}
		else {
			rightPedalVelocity += 2;

		}
		
	}
	else {
		if (rightPedalVelocity <= 0) {
			rightPedalVelocity = 0;
		}
		else {
			rightPedalVelocity -= 2;
			
		}
		
	}
	if (GetAsyncKeyState(VK_Z)) {
		if (leftPedalVelocity >= 254) {
			leftPedalVelocity = 254;
		}
		else {
			leftPedalVelocity += 2;
		}

	}
	else {
		if (leftPedalVelocity <= 0) {
			leftPedalVelocity = 0;
		}
		else {
			leftPedalVelocity -= 2;
		}

	}


	// Brake Logic
	if (GetAsyncKeyState(VK_LCONTROL)) {
		g_bBrake = 0x80;
	}

	if (GetAsyncKeyState(VK_SHIFT)) {
		g_bBrake = 0xFF;
	}

	if (!GetAsyncKeyState(VK_LCONTROL) && !GetAsyncKeyState(VK_LSHIFT)) {
		g_bBrake = 0;
	}


}

/*
Name: XIO::update
Purpose: Used in an event loop to update internal IO states.
Input: Nothing
Output: Nothing
*/
void XIO::update() {
	keyboardInput();
	if (g_bKeyExc & 8) {
		updateCoinCounter();
	}
	updateInputState();
	updateHandleState();
	updatePedalState();
}

/*
Name: XIO::updateCoinCounter
Purpose: Updates the coin counter state.
Input: Nothing
Output: Nothing
*/
void XIO::updateCoinCounter() {
	int ctr = 4096;
	do {
		lampCoinIOData | 2;
		--ctr;
	} while (ctr);
	lampCoinIOData &= 0xFD;
}

/*
Name: XIO::updateLampState
Purpose: Updates the lamp state.
Input: Nothing
Output: Nothing
*/
void XIO::updateLampState() {
	unsigned char cLampState1 = lampCoinIOData;
	
	if (g_bLAccelDif) {
		if (cLampState1 & 0x40) {
			leftLamp(1);
		}
		lamp1 = 63;
	}
	else if (!(cLampState1 & 0x40)) {
		lamp1 -= 1;
		if (!lamp1) {
			leftLamp(0);
		}
	}
	unsigned char cLampState2 = lampCoinIOData;
	if (g_bRAccelDif) {
		if (cLampState2 & 0x80) {
			rightLamp(1);
		}
		lamp2 = 63;
	}
	else if (!(cLampState2 & 0x80)) {
		lamp2 -= 1;
		if (!lamp2) {
			rightLamp(0);
		}
	}
}

/*
Name: XIO::updateInputState()
Purpose: Reads a byte from the I/O hardware, uses the previous state to calculate the new state, and stores both.
Inputs: None {global g_bKeyExc , g_bKeyImm}
Outputs: None
*/

void XIO::updateInputState() {

	char deviceResponse;
	unsigned char lastState;
	unsigned char g_bKeyExcNew = 0;

	// returns the byte representation of key states.
	deviceResponse = ~g_bKeyExc;
	if (deviceResponse & 1) {
		g_bKeyExcNew = 4;
	}
	if (deviceResponse & 2) {
		g_bKeyExcNew |= 2;
	}
	if (deviceResponse & 4) {
		g_bKeyExcNew |= 8;
	}
	if (deviceResponse & 8) {
		g_bKeyExcNew |= 1;
	}
	if (deviceResponse & 16) {
		g_bKeyExcNew |= 16;
	}
	if (deviceResponse & 32) {
		g_bKeyExcNew |= 32;
	}
	lastState = g_bKeyImm;
	g_bKeyImm = g_bKeyExcNew;
	g_bKeyExc = g_bKeyExcNew & (g_bKeyExcNew ^ lastState);
}

/*
Name: XIO::updateHandleState
Purpose: Updates the handle state.
Input: Nothing
Output: Nothing
*/
void XIO::updateHandleState() {
	g_bHandle = steeringData;
}

/*
Name: XIO::updatePedalState
Purpose: Updates the pedal state.
Input: Nothing
Output: Nothing
*/
void XIO::updatePedalState() {

	//pedalData &= 0xDD;
	g_bLAccelDif = abs(leftPedalVelocity - lastLeftPedalVelocity);
	
	g_bLAccelEnd = leftPedalVelocity;
	lastLeftPedalVelocity = leftPedalVelocity;
	unsigned char g_bLAccelDif_copy = g_bLAccelDif;
	if (leftPedalVelocity >= 0xFF) {
		XIO::resetPedal();
		g_bLAccelDif = g_bLAccelDif_copy;
	}
	g_bLAccel = 0;
	if (g_bLAccelDifLast < g_bLAccelDif) {
		g_bLAccel = 1;
	}
	g_bLAccelDifLast = g_bLAccelDif;

	g_bRAccelDif = abs(rightPedalVelocity - lastRightPedalVelocity) ;
	g_bRAccelEnd = rightPedalVelocity;
	unsigned char g_bRAccelDif_copy = g_bRAccelDif;
	lastRightPedalVelocity = rightPedalVelocity;
	if (rightPedalVelocity >= 0xFF) {
		XIO::resetPedal();
		g_bRAccelDif = g_bRAccelDif_copy;
	}
	g_bRAccel = 0;
	if (g_bRAccelDifLast < g_bRAccelDif) {
		g_bRAccel = 1;
	}
	g_bRAccelDifLast = g_bRAccelDif;
	return;

}

/*
Name: XIO::setVibration
Purpose: Sets Vibration Level.
Input: char a2, int a3
Output: None
*/
void XIO::setVibration(unsigned char a2, int a3) {
	int vibrationDataNew;
	if (a3) {
		vibrationDataNew = a2;
	}
	else {
		vibrationDataNew = 3;
	}
	if (vibrationData != vibrationDataNew) {
		vibrationData = vibrationDataNew;
	}
}

/*
Name: XIO::setSteeringBar
Purpose: Sets the steering bar state.
Input: unsigned char inputData
Output: None
*/
void XIO::setSteeringBar(unsigned char inputData) {
	//steeringData = inputData;
}

/*
Name: XIO::getSteeringBar
Purpose: Returns the steering bar state.
Input: None
Output: unsigned char SteeringState
*/
unsigned char XIO::getSteeringBar() {
	return steeringData;
}

/*
Name: XIO::getVibration
Purpose: Returns the vibration state.
Input: None
Output: unsigned char vibrationState
*/
unsigned char XIO::getVibration() {
	return vibrationData;
}

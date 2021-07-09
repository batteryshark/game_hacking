//
// Created by batte on 3/1/2020.
//

#include <windows.h> // TODO: Dynamically call winuser to get AsyncKeyState
#include "KeyboardIO.h"
#include "EXPotatoIO.h"
#include <processthreadsapi.h>

void input_loop(){
    while(true) {
        // Added a way to suicide the game.
        if (GetAsyncKeyState(VK_F12) & 0x8000) { exit(0); }
        EXIORegs.p1_up = (GetAsyncKeyState(VK_UP) & 0x8000) ? 1 : 0;
        EXIORegs.p1_down = (GetAsyncKeyState(VK_DOWN) & 0x8000) ? 1 : 0;
        EXIORegs.p1_left = (GetAsyncKeyState(VK_LEFT) & 0x8000) ? 1 : 0;
        EXIORegs.p1_right = (GetAsyncKeyState(VK_RIGHT) & 0x8000) ? 1 : 0;
        EXIORegs.p1_a = (GetAsyncKeyState('Z') & 0x8000) ? 1 : 0;
        EXIORegs.p1_b = (GetAsyncKeyState('X') & 0x8000) ? 1 : 0;
        EXIORegs.p1_c = (GetAsyncKeyState('C') & 0x8000) ? 1 : 0;

        EXIORegs.p2_up = (GetAsyncKeyState('W') & 0x8000) ? 1 : 0;
        EXIORegs.p2_down = (GetAsyncKeyState('S') & 0x8000) ? 1 : 0;
        EXIORegs.p2_left = (GetAsyncKeyState('A') & 0x8000) ? 1 : 0;
        EXIORegs.p2_right = (GetAsyncKeyState('D') & 0x8000) ? 1 : 0;
        EXIORegs.p2_a = (GetAsyncKeyState('G') & 0x8000) ? 1 : 0;
        EXIORegs.p2_b = (GetAsyncKeyState('H') & 0x8000) ? 1 : 0;
        EXIORegs.p2_c = (GetAsyncKeyState('J') & 0x8000) ? 1 : 0;

        EXIORegs.test = (GetAsyncKeyState(VK_F2) & 0x8000) ? 1 : 0;
        EXIORegs.service = (GetAsyncKeyState(VK_F3) & 0x8000) ? 1 : 0;
        EXIORegs.coin = (GetAsyncKeyState('5') & 0x8000) ? 1 : 0;
    }
}

void init_keyboard_io(){
    CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(input_loop), nullptr, 0, nullptr);
}
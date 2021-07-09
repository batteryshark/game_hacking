
#include <Windows.h>

#include "kitchen_sink.h"
#include "io.hanoi.h"


#define DRIVER_ACTIVATION_IOCTL 0x222004
#define IOCTL_READWRITE 0x222059
static BOOL kbd_thread = FALSE;
static unsigned char io_matrix_in[8] = {0xFF};
static unsigned char io_matrix_out[8] = {0x00};
static unsigned char last_buffer[0x0A] = {0x00};
void ProcessUnityIOIoctlWindows(unsigned int IoControlCode, unsigned char* InputBuffer,unsigned int InputBufferLength,unsigned char* OutputBuffer,unsigned int OutputBufferLength){
    int test_index = 3;
    switch (IoControlCode) {
        case DRIVER_ACTIVATION_IOCTL:            
            break;
        case IOCTL_READWRITE:       



 if(InputBuffer[0] == 1){     
                 // Needs to exist due to prize sensor fuckup.

                // Sensor State 1
                if(!(GetAsyncKeyState('V') & 0x8000)) {
                    OutputBuffer[3] &= ~0x80;
                }else{
                    OutputBuffer[3] |= 0x80;
                }

                // Sensor State 2
                if((GetAsyncKeyState('B') & 0x8000)) {
                    OutputBuffer[3] &= ~0x40;
                }else{
                    OutputBuffer[3] |= 0x40;
                }


                if((GetAsyncKeyState('Z') & 0x8000)) { // Button 1
                    OutputBuffer[2] &= ~4;
                }else{
                    OutputBuffer[2] |= 4;
                }
                if((GetAsyncKeyState('X') & 0x8000)) { // Button 2
                    OutputBuffer[2] &= ~2;
                }else{
                    OutputBuffer[2] |= 2;
                }       
                if((GetAsyncKeyState('C') & 0x8000)) { // Button 3
                    OutputBuffer[2] &= ~1;
                }else{
                    OutputBuffer[2] |= 1;
                }

                if(!(GetAsyncKeyState('5') & 0x8000)) { // Insert Coin 1
                    OutputBuffer[3] &= ~2;
                }else{
                    OutputBuffer[3] |= 2;
                }
                if(!(GetAsyncKeyState('6') & 0x8000)) { // Insert Coin 2
                    OutputBuffer[3] &= ~1;
                }else{
                    OutputBuffer[3] |= 1;
                }       
                if((GetAsyncKeyState('7') & 0x8000)) { // Insert Bill
                    OutputBuffer[3] &= ~0x08;
                }else{
                    OutputBuffer[3] |= 0x08;
                }       
                

                if(!(GetAsyncKeyState(VK_F1) & 0x8000)) { // Service
                    OutputBuffer[4] &= ~0x04;
                }else{
                    OutputBuffer[4] |= 0x04;
                }   
                if(!(GetAsyncKeyState(VK_F2) & 0x8000)) { // SW 2
                    OutputBuffer[4] &= ~0x02;
                }else{
                    OutputBuffer[4] |= 0x02;
                }   
                if(!(GetAsyncKeyState(VK_F3) & 0x8000)) { // SW 1
                    OutputBuffer[4] &= ~0x01;
                }else{
                    OutputBuffer[4] |= 0x01;
                }                                                                                                                            
               
            }else{
                // DBG_print_buffer(OutputBuffer, OutputBufferLength);
                if(memcmp(last_buffer,InputBuffer,InputBufferLength)){
                    memcpy(last_buffer,InputBuffer,InputBufferLength);
                   
                }

            }

            break;
        
        default:
            DBG_printfW(L"[io.hanoi]: UnityIO Unhandled IOCTL: %04X", IoControlCode);
            DBG_print_buffer(InputBuffer, InputBufferLength);
            break;
    }
}

void thread_keyboard_io(){
    while(kbd_thread) {
        // Kill the Process.
        if (GetAsyncKeyState(VK_F12) & 0x8000) {
            kbd_thread = FALSE;
            exit(-1);
        }

        //io_matrix_in[0] = (GetAsyncKeyState('Z') & 0x8000) ? 1:0xFF;
        //io_matrix_in[1] = (GetAsyncKeyState('X') & 0x8000) ? 1:0xFF;
        // V is start
       // io_matrix_in[2] = (GetAsyncKeyState('V') & 0x8000) ? 2:0x00;
        // byte 3 - service credit.
       // io_matrix_in[3] = (GetAsyncKeyState('B') & 0x8000) ? 2:0x00;
       // io_matrix_in[4] = (GetAsyncKeyState('N') & 0x8000) ? 1:0xFF;
      //  io_matrix_in[5] = (GetAsyncKeyState('M') & 0x8000) ? 1:0xFF;
      //  io_matrix_in[6] = (GetAsyncKeyState('A') & 0x8000) ? 1:0xFF;
      //  io_matrix_in[7] = (GetAsyncKeyState('S') & 0x8000) ? 1:0xFF;


/*
        io_matrix_out[0] = (GetAsyncKeyState('D') & 0x8000) ? 1:0;
        io_matrix_out[1] = (GetAsyncKeyState('F') & 0x8000) ? 1:0;
        io_matrix_out[2] = (GetAsyncKeyState('G') & 0x8000) ? 1:0;
        io_matrix_out[3] = (GetAsyncKeyState('H') & 0x8000) ? 1:0;
        io_matrix_out[4] = (GetAsyncKeyState('J') & 0x8000) ? 1:0;
        io_matrix_out[5] = (GetAsyncKeyState('K') & 0x8000) ? 1:0;
        io_matrix_out[6] = (GetAsyncKeyState('L') & 0x8000) ? 1:0;
        io_matrix_out[7] = (GetAsyncKeyState('Q') & 0x8000) ? 1:0;
        */
    }

}

void io_init(){
      kbd_thread = TRUE;
    CreateThread(NULL, 0, (void*)thread_keyboard_io, NULL, 0, NULL);
}
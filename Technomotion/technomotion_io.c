#include <processthreadsapi.h>
#include <rpc.h>
#include "technomotion_io.h"
#include "key_bindings.h"
#include "global.h"

struct TIO_Output io_output;
struct TIO_Input io_input;
BOOL patch_running = FALSE;

void tio_set_value(unsigned short port, unsigned char value){
    value = ~value;
    switch(port){
        case TIO_OUT_CHANNEL_0:
            io_output.p1_lamp_fl = (value & 0x01) ? 1:0;
            io_output.p1_lamp_fc = (value & 0x02) ? 1:0;
            io_output.p1_lamp_fr = (value & 0x04) ? 1:0;
            io_output.p1_lamp_cl = (value & 0x08) ? 1:0;
            io_output.p1_lamp_cc = (value & 0x10) ? 1:0;
            io_output.p1_lamp_cr = (value & 0x20) ? 1:0;
            io_output.p1_lamp_rl = (value & 0x40) ? 1:0;
            io_output.p1_lamp_rc = (value & 0x80) ? 1:0;
            break;
        case TIO_OUT_CHANNEL_1:
            io_output.p1_lamp_rr = (value & 0x01) ? 1:0;
            io_output.control_m0 = (value & 0x02) ? 1:0;
            io_output.control_m1 = (value & 0x04) ? 1:0;
            io_output.p2_lamp_fl = (value & 0x08) ? 1:0;
            io_output.p2_lamp_fc = (value & 0x10) ? 1:0;
            io_output.p2_lamp_fr = (value & 0x20) ? 1:0;
            io_output.p2_lamp_cl = (value & 0x40) ? 1:0;
            io_output.p2_lamp_cc = (value & 0x80) ? 1:0;
            break;
        case TIO_OUT_CHANNEL_2:
            io_output.p2_lamp_cr = (value & 0x01) ? 1:0;
            io_output.p2_lamp_rl = (value & 0x02) ? 1:0;
            io_output.p2_lamp_rc = (value & 0x04) ? 1:0;
            io_output.p2_lamp_rr = (value & 0x08) ? 1:0;
            io_output.button_1   = (value & 0x10) ? 1:0;
            io_output.button_2   = (value & 0x20) ? 1:0;
            io_output.button_3   = (value & 0x40) ? 1:0;
            io_output.button_4   = (value & 0x80) ? 1:0;
            break;
        case TIO_OUT_CHANNEL_3:
            io_output.button_5   = (value & 0x01) ? 1:0;
            io_output.button_6   = (value & 0x02) ? 1:0;
            io_output.button_7   = (value & 0x04) ? 1:0;
            io_output.button_8   = (value & 0x08) ? 1:0;
            io_output.button_9   = (value & 0x10) ? 1:0;
            io_output.coin_ctr   = (value & 0x20) ? 1:0;
            io_output.lamp_1     = (value & 0x40) ? 1:0;
            io_output.lamp_2     = (value & 0x80) ? 1:0;
            break;
        case TIO_OUT_CHANNEL_4:
            io_output.lamp_3     = (value & 0x01) ? 1:0;
            io_output.lamp_4     = (value & 0x02) ? 1:0;
            io_output.lamp_5     = (value & 0x04) ? 1:0;
            io_output.lamp_6     = (value & 0x08) ? 1:0;
            io_output.neon_1     = (value & 0x10) ? 1:0;
            io_output.neon_2     = (value & 0x20) ? 1:0;
            break;
    }
}

unsigned char tio_get_value(unsigned short port){
    unsigned  char value = 0;
    switch(port){
        case TIO_IN_CHANNEL_0:
            if(io_input.p1_fl){value |= 0x01;}
            if(io_input.p1_fc){value |= 0x02;}
            if(io_input.p1_fr){value |= 0x04;}
            if(io_input.p1_cl){value |= 0x08;}
            if(io_input.p1_cc){value |= 0x10;}
            if(io_input.p1_cr){value |= 0x20;}
            if(io_input.p1_rl){value |= 0x40;}
            if(io_input.p1_rc){value |= 0x80;}
            break;
        case TIO_IN_CHANNEL_1:
            if(io_input.p1_rr){value |= 0x01;}
            if(io_input.p2_fl){value |= 0x02;}
            if(io_input.p2_fc){value |= 0x04;}
            if(io_input.p2_fr){value |= 0x08;}
            if(io_input.p2_cl){value |= 0x10;}
            if(io_input.p2_cc){value |= 0x20;}
            if(io_input.p2_cr){value |= 0x40;}
            if(io_input.p2_rl){value |= 0x80;}
            break;
        case TIO_IN_CHANNEL_2:
            if(io_input.p2_rc){value |= 0x01;}
            if(io_input.p2_rr){value |= 0x02;}
            if(io_input.btn_1){value |= 0x04;}
            if(io_input.btn_2){value |= 0x08;}
            if(io_input.btn_3){value |= 0x10;}
            if(io_input.btn_4){value |= 0x20;}
            if(io_input.btn_5){value |= 0x40;}
            if(io_input.btn_6){value |= 0x80;}
            break;
        case TIO_IN_CHANNEL_3:
            if(io_input.btn_7){value |= 0x01;}
            if(io_input.btn_8){value |= 0x02;}
            if(io_input.btn_9){value |= 0x04;}
            if(io_input.coin_1){value |= 0x08;}
            if(io_input.coin_2){value |= 0x10;}
            if(io_input.test){value |= 0x20;}
            if(io_input.svc){value |= 0x40;}
            break;
    }
    return ~value;
}


void thread_keyboard_io(){
    while(patch_running){
        // Kill the Process.
        if (GetAsyncKeyState(BINDING_KEY_QUIT) & 0x8000) {
            patch_running = FALSE;
            kill_process();
        }
        // Iterate all Inputs
        io_input.p1_fl = (GetAsyncKeyState(BINDING_P1_FL) & 0x8000) ? 1:0;
        io_input.p1_fc = (GetAsyncKeyState(BINDING_P1_FC) & 0x8000) ? 1:0;
        io_input.p1_fr = (GetAsyncKeyState(BINDING_P1_FR) & 0x8000) ? 1:0;
        io_input.p1_cl = (GetAsyncKeyState(BINDING_P1_CL) & 0x8000) ? 1:0;
        io_input.p1_cc = (GetAsyncKeyState(BINDING_P1_CC) & 0x8000) ? 1:0;
        io_input.p1_cr = (GetAsyncKeyState(BINDING_P1_CR) & 0x8000) ? 1:0;
        io_input.p1_rl = (GetAsyncKeyState(BINDING_P1_RL) & 0x8000) ? 1:0;
        io_input.p1_rc = (GetAsyncKeyState(BINDING_P1_RC) & 0x8000) ? 1:0;
        io_input.p1_rr = (GetAsyncKeyState(BINDING_P1_RR) & 0x8000) ? 1:0;

        io_input.p2_fl = (GetAsyncKeyState(BINDING_P2_FL) & 0x8000) ? 1:0;
        io_input.p2_fc = (GetAsyncKeyState(BINDING_P2_FC) & 0x8000) ? 1:0;
        io_input.p2_fr = (GetAsyncKeyState(BINDING_P2_FR) & 0x8000) ? 1:0;
        io_input.p2_cl = (GetAsyncKeyState(BINDING_P2_CL) & 0x8000) ? 1:0;
        io_input.p2_cc = (GetAsyncKeyState(BINDING_P2_CC) & 0x8000) ? 1:0;
        io_input.p2_cr = (GetAsyncKeyState(BINDING_P2_CR) & 0x8000) ? 1:0;
        io_input.p2_rl = (GetAsyncKeyState(BINDING_P2_RL) & 0x8000) ? 1:0;
        io_input.p2_rc = (GetAsyncKeyState(BINDING_P2_RC) & 0x8000) ? 1:0;
        io_input.p2_rr = (GetAsyncKeyState(BINDING_P2_RR) & 0x8000) ? 1:0;

        io_input.btn_1 = (GetAsyncKeyState(BINDING_BUTTON_LL) & 0x8000) ? 1:0;
        io_input.btn_2 = (GetAsyncKeyState(BINDING_BUTTON_LC) & 0x8000) ? 1:0;
        io_input.btn_3 = (GetAsyncKeyState(BINDING_BUTTON_LR) & 0x8000) ? 1:0;
        io_input.btn_4 = (GetAsyncKeyState(BINDING_BUTTON_CL) & 0x8000) ? 1:0;
        io_input.btn_5 = (GetAsyncKeyState(BINDING_BUTTON_CC) & 0x8000) ? 1:0;
        io_input.btn_6 = (GetAsyncKeyState(BINDING_BUTTON_CR) & 0x8000) ? 1:0;
        io_input.btn_7 = (GetAsyncKeyState(BINDING_BUTTON_RL) & 0x8000) ? 1:0;
        io_input.btn_8 = (GetAsyncKeyState(BINDING_BUTTON_RC) & 0x8000) ? 1:0;
        io_input.btn_9 = (GetAsyncKeyState(BINDING_BUTTON_RR) & 0x8000) ? 1:0;

        io_input.coin_1 = (GetAsyncKeyState(BINDING_COIN_1) & 0x8000) ? 1:0;
        io_input.coin_2 = (GetAsyncKeyState(BINDING_COIN_2) & 0x8000) ? 1:0;
        io_input.test   = (GetAsyncKeyState(BINDING_TEST) & 0x8000) ? 1:0;
        io_input.svc    = (GetAsyncKeyState(BINDING_SERVICE) & 0x8000) ? 1:0;
    }
};

// Thread Spawner for Keyboard IO
void tio_init(){
    patch_running = TRUE;
    CreateThread(NULL, 0, (void*)thread_keyboard_io, NULL, 0, NULL);
}
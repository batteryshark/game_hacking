#include <Windows.h>
#include "kitchen_sink/kitchen_sink.h"

#include "io.technoinput.h"

static struct TIO_Input{
    unsigned char p1_fl;  // 2c0 0x01
    unsigned char p1_fc;  // 2c0 0x02
    unsigned char p1_fr;  // 2c0 0x04
    unsigned char p1_cl;  // 2c0 0x08
    unsigned char p1_cc;  // 2c0 0x10
    unsigned char p1_cr;  // 2c0 0x20
    unsigned char p1_rl;  // 2c0 0x40
    unsigned char p1_rc;  // 2c0 0x80
    unsigned char p1_rr;  // 2c1 0x01

    unsigned char p2_fl;  // 2c1 0x02
    unsigned char p2_fc;  // 2c1 0x04
    unsigned char p2_fr;  // 2c1 0x08
    unsigned char p2_cl;  // 2c1 0x10
    unsigned char p2_cc;  // 2c1 0x20
    unsigned char p2_cr;  // 2c1 0x40
    unsigned char p2_rl;  // 2c1 0x80
    unsigned char p2_rc;  // 2c2 0x01
    unsigned char p2_rr;  // 2c2 0x02

    unsigned char btn_1;  // 2c2 0x04
    unsigned char btn_2;  // 2c2 0x08
    unsigned char btn_3;  // 2c2 0x10
    unsigned char btn_4;  // 2c2 0x20
    unsigned char btn_5;  // 2c2 0x40
    unsigned char btn_6;  // 2c2 0x80
    unsigned char btn_7;  // 2c3 0x01
    unsigned char btn_8;  // 2c3 0x02
    unsigned char btn_9;  // 2c3 0x04

    unsigned char coin_1; // 2c3 0x08
    unsigned char coin_2; // 2c3 0x10

    unsigned char test;   // 2c3 0x20
    unsigned char svc;    // 2c3 0x40
    unsigned char suicide; // Kill the Game
}INBindings,INState;
static int kbd_thread = 0;


void thread_keyboard_io(){
    while(kbd_thread) {
        // Kill the Process.
        if (GetAsyncKeyState(INBindings.suicide) & 0x8000) {
            kbd_thread = FALSE;
            exit(-1);
        }
        // Iterate all Inputs
        INState.p1_fl = (GetAsyncKeyState(INBindings.p1_fl) & 0x8000) ? 1 : 0;
        INState.p1_fc = (GetAsyncKeyState(INBindings.p1_fc) & 0x8000) ? 1 : 0;
        INState.p1_fr = (GetAsyncKeyState(INBindings.p1_fr) & 0x8000) ? 1 : 0;
        INState.p1_cl = (GetAsyncKeyState(INBindings.p1_cl) & 0x8000) ? 1 : 0;
        INState.p1_cc = (GetAsyncKeyState(INBindings.p1_cc) & 0x8000) ? 1 : 0;
        INState.p1_cr = (GetAsyncKeyState(INBindings.p1_cr) & 0x8000) ? 1 : 0;
        INState.p1_rl = (GetAsyncKeyState(INBindings.p1_rl) & 0x8000) ? 1 : 0;
        INState.p1_rc = (GetAsyncKeyState(INBindings.p1_rc) & 0x8000) ? 1 : 0;
        INState.p1_rr = (GetAsyncKeyState(INBindings.p1_rr) & 0x8000) ? 1 : 0;

        INState.p2_fl = (GetAsyncKeyState(INBindings.p2_fl) & 0x8000) ? 1 : 0;
        INState.p2_fc = (GetAsyncKeyState(INBindings.p2_fc) & 0x8000) ? 1 : 0;
        INState.p2_fr = (GetAsyncKeyState(INBindings.p2_fr) & 0x8000) ? 1 : 0;
        INState.p2_cl = (GetAsyncKeyState(INBindings.p2_cl) & 0x8000) ? 1 : 0;
        INState.p2_cc = (GetAsyncKeyState(INBindings.p2_cc) & 0x8000) ? 1 : 0;
        INState.p2_cr = (GetAsyncKeyState(INBindings.p2_cr) & 0x8000) ? 1 : 0;
        INState.p2_rl = (GetAsyncKeyState(INBindings.p2_rl) & 0x8000) ? 1 : 0;
        INState.p2_rc = (GetAsyncKeyState(INBindings.p2_rc) & 0x8000) ? 1 : 0;
        INState.p2_rr = (GetAsyncKeyState(INBindings.p2_rr) & 0x8000) ? 1 : 0;

        INState.btn_1 = (GetAsyncKeyState(INBindings.btn_1) & 0x8000) ? 1 : 0;
        INState.btn_2 = (GetAsyncKeyState(INBindings.btn_2) & 0x8000) ? 1 : 0;
        INState.btn_3 = (GetAsyncKeyState(INBindings.btn_3) & 0x8000) ? 1 : 0;
        INState.btn_4 = (GetAsyncKeyState(INBindings.btn_4) & 0x8000) ? 1 : 0;
        INState.btn_5 = (GetAsyncKeyState(INBindings.btn_5) & 0x8000) ? 1 : 0;
        INState.btn_6 = (GetAsyncKeyState(INBindings.btn_6) & 0x8000) ? 1 : 0;
        INState.btn_7 = (GetAsyncKeyState(INBindings.btn_7) & 0x8000) ? 1 : 0;
        INState.btn_8 = (GetAsyncKeyState(INBindings.btn_8) & 0x8000) ? 1 : 0;
        INState.btn_9 = (GetAsyncKeyState(INBindings.btn_9) & 0x8000) ? 1 : 0;

        INState.coin_1 = (GetAsyncKeyState(INBindings.coin_1) & 0x8000) ? 1 : 0;
        INState.coin_2 = (GetAsyncKeyState(INBindings.coin_2) & 0x8000) ? 1 : 0;
        INState.test = (GetAsyncKeyState(INBindings.test) & 0x8000) ? 1 : 0;
        INState.svc = (GetAsyncKeyState(INBindings.svc) & 0x8000) ? 1 : 0;
    }
};

unsigned char tio_get_value(unsigned short port){
    unsigned  char value = 0;

    switch(port){
        case TIO_IN_CHANNEL_0:
            if(INState.p1_fl){value |= 0x01;}
            if(INState.p1_fc){value |= 0x02;}
            if(INState.p1_fr){value |= 0x04;}
            if(INState.p1_cl){value |= 0x08;}
            if(INState.p1_cc){value |= 0x10;}
            if(INState.p1_cr){value |= 0x20;}
            if(INState.p1_rl){value |= 0x40;}
            if(INState.p1_rc){value |= 0x80;}
            break;
        case TIO_IN_CHANNEL_1:
            if(INState.p1_rr){value |= 0x01;}
            if(INState.p2_fl){value |= 0x02;}
            if(INState.p2_fc){value |= 0x04;}
            if(INState.p2_fr){value |= 0x08;}
            if(INState.p2_cl){value |= 0x10;}
            if(INState.p2_cc){value |= 0x20;}
            if(INState.p2_cr){value |= 0x40;}
            if(INState.p2_rl){value |= 0x80;}
            break;
        case TIO_IN_CHANNEL_2:
            if(INState.p2_rc){value |= 0x01;}
            if(INState.p2_rr){value |= 0x02;}
            if(INState.btn_1){value |= 0x04;}
            if(INState.btn_2){value |= 0x08;}
            if(INState.btn_3){value |= 0x10;}
            if(INState.btn_4){value |= 0x20;}
            if(INState.btn_5){value |= 0x40;}
            if(INState.btn_6){value |= 0x80;}
            break;
        case TIO_IN_CHANNEL_3:
            if(INState.btn_7){value |= 0x01;}
            if(INState.btn_8){value |= 0x02;}
            if(INState.btn_9){value |= 0x04;}
            if(INState.coin_1){value |= 0x08;}
            if(INState.coin_2){value |= 0x10;}
            if(INState.test){value |= 0x20;}
            if(INState.svc){value |= 0x40;}
            break;
    }
    return ~value;
}




static void load_config(){
    char ini_path[1024] = {0x00};
    if(!GetEnvironmentVariableA("CONFIG_INI_PATH", ini_path,sizeof(ini_path))){
        strcpy(ini_path,"D:\\Techmo\\technotools.ini");
    }
    char current_data[64] = { 0x00 };
    GetPrivateProfileStringA("TECHNO_IO", "IN_LEFT_PAD_1", "51", current_data, sizeof(current_data), ini_path);
    INBindings.p1_fl = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_LEFT_PAD_2", "57", current_data, sizeof(current_data), ini_path);
    INBindings.p1_fc = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_LEFT_PAD_3", "45", current_data, sizeof(current_data), ini_path);
    INBindings.p1_fr = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_LEFT_PAD_4", "41", current_data, sizeof(current_data), ini_path);
    INBindings.p1_cl = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_LEFT_PAD_5", "53", current_data, sizeof(current_data), ini_path);
    INBindings.p1_cc = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_LEFT_PAD_6", "44", current_data, sizeof(current_data), ini_path);
    INBindings.p1_cr = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_LEFT_PAD_7", "5A", current_data, sizeof(current_data), ini_path);
    INBindings.p1_rl = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_LEFT_PAD_8", "58", current_data, sizeof(current_data), ini_path);
    INBindings.p1_rc = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_LEFT_PAD_9", "43", current_data, sizeof(current_data), ini_path);
    INBindings.p1_rr = strtoul(current_data, NULL, 16) & 0xFFu;



    GetPrivateProfileStringA("TECHNO_IO", "IN_RIGHT_PAD_1", "67", current_data, sizeof(current_data), ini_path);
    INBindings.p2_fl = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_RIGHT_PAD_2", "68", current_data, sizeof(current_data), ini_path);
    INBindings.p2_fc = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_RIGHT_PAD_3", "69", current_data, sizeof(current_data), ini_path);
    INBindings.p2_fr = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_RIGHT_PAD_4", "64", current_data, sizeof(current_data), ini_path);
    INBindings.p2_cl = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_RIGHT_PAD_5", "65", current_data, sizeof(current_data), ini_path);
    INBindings.p2_cc = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_RIGHT_PAD_6", "66", current_data, sizeof(current_data), ini_path);
    INBindings.p2_cr = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_RIGHT_PAD_7", "61", current_data, sizeof(current_data), ini_path);
    INBindings.p2_rl = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_RIGHT_PAD_8", "62", current_data, sizeof(current_data), ini_path);
    INBindings.p2_rc = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_RIGHT_PAD_9", "63", current_data, sizeof(current_data), ini_path);
    INBindings.p2_rr = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_BUTTONBOARD_1", "31", current_data, sizeof(current_data), ini_path);
    INBindings.btn_1 = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_BUTTONBOARD_2", "32", current_data, sizeof(current_data), ini_path);
    INBindings.btn_2 = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_BUTTONBOARD_3", "33", current_data, sizeof(current_data), ini_path);
    INBindings.btn_3 = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_BUTTONBOARD_4", "25", current_data, sizeof(current_data), ini_path);
    INBindings.btn_4 = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_BUTTONBOARD_5", "0D", current_data, sizeof(current_data), ini_path);
    INBindings.btn_5 = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_BUTTONBOARD_6", "27", current_data, sizeof(current_data), ini_path);
    INBindings.btn_6 = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_BUTTONBOARD_7", "37", current_data, sizeof(current_data), ini_path);
    INBindings.btn_7 = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_BUTTONBOARD_8", "38", current_data, sizeof(current_data), ini_path);
    INBindings.btn_8 = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_BUTTONBOARD_9", "39", current_data, sizeof(current_data), ini_path);
    INBindings.btn_9 = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_COIN_1", "35", current_data, sizeof(current_data), ini_path);
    INBindings.coin_1 = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_COIN_2", "36", current_data, sizeof(current_data), ini_path);
    INBindings.coin_2 = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_TEST", "70", current_data, sizeof(current_data), ini_path);
    INBindings.test = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_SERVICE", "71", current_data, sizeof(current_data), ini_path);
    INBindings.svc = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "IN_SUICIDE", "7B", current_data, sizeof(current_data), ini_path);
    INBindings.suicide = strtoul(current_data, NULL, 16) & 0xFFu;

}


void init_input_emu(){
    // Get our Bindings
    kbd_thread = TRUE;
    CreateThread(NULL, 0, (void*)thread_keyboard_io, NULL, 0, NULL);
    load_config();
}

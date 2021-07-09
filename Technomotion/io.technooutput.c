#include <Windows.h>
#include <winternl.h>
#include <stdbool.h>
#include "kitchen_sink/kitchen_sink.h"

#include "io.technooutput.h"


struct TIO_Output{
    unsigned char p1_lamp_fl; // 2d0 0x01
    unsigned char p1_lamp_fc; // 2d0 0x02
    unsigned char p1_lamp_fr; // 2d0 0x04
    unsigned char p1_lamp_cl; // 2d0 0x08
    unsigned char p1_lamp_cc; // 2d0 0x10
    unsigned char p1_lamp_cr; // 2d0 0x20
    unsigned char p1_lamp_rl; // 2d0 0x40
    unsigned char p1_lamp_rc; // 2d0 0x80
    unsigned char p1_lamp_rr; // 2d1 0x01

    unsigned char control_m0; // 2d1 0x02
    unsigned char control_m1; // 2d1 0x04

    unsigned char p2_lamp_fl; // 2d1 0x08
    unsigned char p2_lamp_fc; // 2d1 0x10
    unsigned char p2_lamp_fr; // 2d1 0x20
    unsigned char p2_lamp_cl; // 2d1 0x40
    unsigned char p2_lamp_cc; // 2d1 0x80
    unsigned char p2_lamp_cr; // 2d2 0x01
    unsigned char p2_lamp_rl; // 2d2 0x02
    unsigned char p2_lamp_rc; // 2d2 0x04
    unsigned char p2_lamp_rr; // 2d2 0x08

    unsigned char button_1;   // 2d2 0x10
    unsigned char button_2;   // 2d2 0x20
    unsigned char button_3;   // 2d2 0x40
    unsigned char button_4;   // 2d2 0x80
    unsigned char button_5;   // 2d3 0x01
    unsigned char button_6;   // 2d3 0x02
    unsigned char button_7;   // 2d3 0x04
    unsigned char button_8;   // 2d3 0x08
    unsigned char button_9;   // 2d3 0x10

    unsigned char coin_ctr;   // 2d3 0x20

    unsigned char lamp_1;     // 2d3 0x40
    unsigned char lamp_2;     // 2d3 0x80
    unsigned char lamp_3;     // 2d4 0x01
    unsigned char lamp_4;     // 2d4 0x02
    unsigned char lamp_5;     // 2d4 0x04
    unsigned char lamp_6;     // 2d4 0x08

    unsigned char neon_1;     // 2d4 0x10
    unsigned char neon_2;     // 2d4 0x20
}OUTState;

struct CorsairLedColor {
    int ledId;				// identifier of LED to set.
    int r;							// red   brightness[0..255].
    int g;							// green brightness[0..255].
    int b;							// blue  brightness[0..255].
};

struct CorsairProtocolDetails{
    const char* sdkVersion;			// null - terminated string containing version of SDK(like “1.0.0.1”). Always contains valid value even if there was no CUE found.
    const char* serverVersion;		// null - terminated string containing version of CUE(like “1.0.0.1”) or NULL if CUE was not found.
    int sdkProtocolVersion;			// integer number that specifies version of protocol that is implemented by current SDK. Numbering starts from 1. Always contains valid value even if there was no CUE found.
    int serverProtocolVersion;		// integer number that specifies version of protocol that is implemented by CUE. Numbering starts from 1. If CUE was not found then this value will be 0.
    bool breakingChanges;			// boolean value that specifies if there were breaking changes between version of protocol implemented by server and client.
};
typedef struct CorsairProtocolDetails tCorsairPerformProtocolHandshake();

bool fake_CorsairSetLedsColors(int size, struct CorsairLedColor* ledsColors){
    return true;
}
typedef bool tCorsairSetLedsColors(int size, struct CorsairLedColor* ledsColors);
static tCorsairSetLedsColors* CorsairSetLedsColors = fake_CorsairSetLedsColors;
static tCorsairPerformProtocolHandshake* CorsairPerformProtocolHandshake = 0;

int OFF_COLOR[3] = { 0,0,0 };

// Arrow Colors
int RED_ARR_COLOR[3] = {242,25,36};
int YEL_ARR_COLOR[3] = {250, 255, 11};
int GRN_ARR_COLOR[3] = {11, 222, 47};
int PNK_ARR_COLOR[3] = {250, 124, 189};
int PNK_ARR_COLOR_ALT[3] = {182, 57, 134}; // Alternate one to see how it looks.
int BLU_ARR_COLOR[3] = {0, 177, 254};

// ButtonBoard Colors
int RED_BB_COLOR[3] = {228,39,44};
int YEL_BB_COLOR[3] = {255,242,0};
int BLU_BB_COLOR[3] = {16,111,216};

// System Cabinet Colors
int BLU_NEON_COLOR[3] = { 243,242,255 };
int RED_NEON_COLOR[3] = {242,39,44};
int COIN_COLOR[3] = { 253,0,32 };
int SPOT_1_COLOR[3] = {245,1,0};
int SPOT_2_COLOR[3] = {108, 252, 61};

void SetColor(struct CorsairLedColor* cled, int settings[3]) {
    cled->r = settings[0];
    cled->g = settings[1];
    cled->b = settings[2];
}



struct CorsairLedColor Lights_Channel_0[] = {
        // P1 Front Left
        {0,0,0,0},
        // P1 Front Center
        {0,0,0,0},
        // P1 Front Right
        {0,0,0,0},
        // P1 Center Left
        {0,0,0,0},
        // P1 Center Center
        {0, 0,0,0},
        // P1 Center Right
        {0, 0,0,0},
        // P1 Rear Left
        {0,0,0,0},
        // P1 Rear Center
        {0, 0,0,0},
};

struct CorsairLedColor Lights_Channel_1[] = {
        // P1 Rear Right
        {0, 0,0,0},
        // CONTROL M0
        {0, 0,0,0},
        // CONTROL M1
        {0, 0,0,0},
        // P2 Front Left
        {0,0,0,0},
        // P2 Front Center
        {0,0,0,0},
        // P2 Front Right
        {0,0,0,0},
        // P2 Center Left
        {0,0,0,0},
        // P2 Center Center
        {0, 0,0,0},
};

struct CorsairLedColor Lights_Channel_2[] = {
        // P2 Center Right
        {0, 0, 0, 0},
        // P2 Rear Left
        {0, 0, 0, 0},
        // P2 Rear Center
        {0, 0, 0, 0},
        // P2 Rear Right
        {0, 0, 0, 0},
        // ButtonBoard Left - Left
        {0, 0, 0, 0},
        // ButtonBoard Left - Center
        {0, 0, 0, 0},
        // ButtonBoard Left - Right
        {0, 0, 0, 0},
        // ButtonBoard Center - Left
        {0, 0, 0, 0},
};

struct CorsairLedColor Lights_Channel_3[] = {
        // ButtonBoard Center - Center
        {0, 0, 0, 0},
        // ButtonBoard Center - Right
        {0, 0, 0, 0},
        // ButtonBoard Right - Left
        {0, 0, 0, 0},
        // ButtonBoard Right - Center
        {0, 0, 0, 0},
        // ButtonBoard Right - Right
        {0, 0, 0, 0},
        // Coin
        {0, 0, 0, 0},
        // Lamp 1
        {0, 0, 0, 0},
        // Lamp 2
        {0, 0, 0, 0},

};

struct CorsairLedColor Lights_Channel_4[] = {
        // Lamp 3
        {0, 0, 0, 0},
        // Lamp 4
        {0, 0, 0, 0},
        // Lamp 5
        {0, 0, 0, 0},
        // Lamp 6
        {0, 0, 0, 0},
        // Neon 1
        {0, 0, 0, 0},
        // Neon 2 - LEFT
        {0, 0, 0, 0},
        // Neon 2 - RIGHT
        {0, 0, 0, 0},
};

void UpdateChannel_0() {
    OUTState.p1_lamp_fl ? SetColor(&Lights_Channel_0[0], RED_ARR_COLOR) : SetColor(&Lights_Channel_0[0], OFF_COLOR);
    OUTState.p1_lamp_fc ? SetColor(&Lights_Channel_0[1], YEL_ARR_COLOR) : SetColor(&Lights_Channel_0[1], OFF_COLOR);
    OUTState.p1_lamp_fr ? SetColor(&Lights_Channel_0[2], RED_ARR_COLOR) : SetColor(&Lights_Channel_0[2], OFF_COLOR);
    OUTState.p1_lamp_cl ? SetColor(&Lights_Channel_0[3], GRN_ARR_COLOR) : SetColor(&Lights_Channel_0[3], OFF_COLOR);
    OUTState.p1_lamp_cc ? SetColor(&Lights_Channel_0[4], PNK_ARR_COLOR) : SetColor(&Lights_Channel_0[4], OFF_COLOR);
    OUTState.p1_lamp_cr ? SetColor(&Lights_Channel_0[5], GRN_ARR_COLOR) : SetColor(&Lights_Channel_0[5], OFF_COLOR);
    OUTState.p1_lamp_rl ? SetColor(&Lights_Channel_0[6], BLU_ARR_COLOR) : SetColor(&Lights_Channel_0[6], OFF_COLOR);
    OUTState.p1_lamp_rc ? SetColor(&Lights_Channel_0[7], YEL_ARR_COLOR) : SetColor(&Lights_Channel_0[7], OFF_COLOR);
    CorsairSetLedsColors(8, Lights_Channel_0);
}

void UpdateChannel_1() {
    OUTState.p1_lamp_rr ? SetColor(&Lights_Channel_1[0], BLU_ARR_COLOR) : SetColor(&Lights_Channel_1[0], OFF_COLOR);
    OUTState.control_m0 ? SetColor(&Lights_Channel_1[1], COIN_COLOR) : SetColor(&Lights_Channel_1[1], OFF_COLOR);
    OUTState.control_m1 ? SetColor(&Lights_Channel_1[2], COIN_COLOR) : SetColor(&Lights_Channel_1[2], OFF_COLOR);
    OUTState.p2_lamp_fl ? SetColor(&Lights_Channel_1[3], RED_ARR_COLOR) : SetColor(&Lights_Channel_1[3], OFF_COLOR);
    OUTState.p2_lamp_fc ? SetColor(&Lights_Channel_1[4], YEL_ARR_COLOR) : SetColor(&Lights_Channel_1[4], OFF_COLOR);
    OUTState.p2_lamp_fr ? SetColor(&Lights_Channel_1[5], RED_ARR_COLOR) : SetColor(&Lights_Channel_1[5], OFF_COLOR);
    OUTState.p2_lamp_cl ? SetColor(&Lights_Channel_1[6], GRN_ARR_COLOR) : SetColor(&Lights_Channel_1[6], OFF_COLOR);
    OUTState.p2_lamp_cc ? SetColor(&Lights_Channel_1[7], PNK_ARR_COLOR) : SetColor(&Lights_Channel_1[7], OFF_COLOR);
    CorsairSetLedsColors(8, Lights_Channel_1);
}

void UpdateChannel_2() {
    OUTState.p2_lamp_cr ? SetColor(&Lights_Channel_2[0], GRN_ARR_COLOR) : SetColor(&Lights_Channel_2[0], OFF_COLOR);
    OUTState.p2_lamp_rl ? SetColor(&Lights_Channel_2[1], BLU_ARR_COLOR) : SetColor(&Lights_Channel_2[1], OFF_COLOR);
    OUTState.p2_lamp_rc ? SetColor(&Lights_Channel_2[2], YEL_ARR_COLOR) : SetColor(&Lights_Channel_2[2], OFF_COLOR);
    OUTState.p2_lamp_rr ? SetColor(&Lights_Channel_2[3], BLU_ARR_COLOR) : SetColor(&Lights_Channel_2[3], OFF_COLOR);
    OUTState.button_1 ? SetColor(&Lights_Channel_2[4], RED_BB_COLOR) : SetColor(&Lights_Channel_2[4], OFF_COLOR);
    OUTState.button_2 ? SetColor(&Lights_Channel_2[5], YEL_BB_COLOR) : SetColor(&Lights_Channel_2[5], OFF_COLOR);
    OUTState.button_3 ? SetColor(&Lights_Channel_2[6], BLU_BB_COLOR) : SetColor(&Lights_Channel_2[6], OFF_COLOR);
    OUTState.button_4 ? SetColor(&Lights_Channel_2[7], RED_BB_COLOR) : SetColor(&Lights_Channel_2[7], OFF_COLOR);
    CorsairSetLedsColors(8, Lights_Channel_2);
}

void UpdateChannel_3() {
    OUTState.button_5 ? SetColor(&Lights_Channel_3[0], BLU_BB_COLOR) : SetColor(&Lights_Channel_3[0], OFF_COLOR);
    OUTState.button_6 ? SetColor(&Lights_Channel_3[1], RED_BB_COLOR) : SetColor(&Lights_Channel_3[1], OFF_COLOR);
    OUTState.button_7 ? SetColor(&Lights_Channel_3[2], RED_BB_COLOR) : SetColor(&Lights_Channel_3[2], OFF_COLOR);
    OUTState.button_8 ? SetColor(&Lights_Channel_3[3], YEL_BB_COLOR) : SetColor(&Lights_Channel_3[3], OFF_COLOR);
    OUTState.button_9 ? SetColor(&Lights_Channel_3[4], BLU_BB_COLOR) : SetColor(&Lights_Channel_3[4], OFF_COLOR);
    OUTState.coin_ctr ? SetColor(&Lights_Channel_3[5], COIN_COLOR) : SetColor(&Lights_Channel_3[5], OFF_COLOR);
    OUTState.lamp_1 ? SetColor(&Lights_Channel_3[6], SPOT_1_COLOR) : SetColor(&Lights_Channel_3[6], OFF_COLOR);
    OUTState.lamp_2 ? SetColor(&Lights_Channel_3[7], SPOT_1_COLOR) : SetColor(&Lights_Channel_3[7], OFF_COLOR);
    CorsairSetLedsColors(8, Lights_Channel_3);
}

void UpdateChannel_4() {
    OUTState.lamp_3 ? SetColor(&Lights_Channel_4[0], SPOT_2_COLOR) : SetColor(&Lights_Channel_4[0], OFF_COLOR);
    OUTState.lamp_4 ? SetColor(&Lights_Channel_4[1], SPOT_2_COLOR) : SetColor(&Lights_Channel_4[1], OFF_COLOR);
    OUTState.lamp_5 ? SetColor(&Lights_Channel_4[2], OFF_COLOR) : SetColor(&Lights_Channel_4[2], OFF_COLOR);
    OUTState.lamp_6 ? SetColor(&Lights_Channel_4[3], OFF_COLOR) : SetColor(&Lights_Channel_4[3], OFF_COLOR);
    OUTState.neon_1 ? SetColor(&Lights_Channel_4[4], BLU_NEON_COLOR) : SetColor(&Lights_Channel_4[4], OFF_COLOR);
    OUTState.neon_2 ? SetColor(&Lights_Channel_4[5], RED_NEON_COLOR) : SetColor(&Lights_Channel_4[5], OFF_COLOR);
    OUTState.neon_2 ? SetColor(&Lights_Channel_4[6], RED_NEON_COLOR) : SetColor(&Lights_Channel_4[6], OFF_COLOR);
    CorsairSetLedsColors(7, Lights_Channel_4);
}

void tio_set_value(unsigned short port, unsigned char value){
    value = ~value;
    switch(port){
        case TIO_OUT_CHANNEL_0:
            OUTState.p1_lamp_fl = (value & 0x01u) ? 1:0;
            OUTState.p1_lamp_fc = (value & 0x02u) ? 1:0;
            OUTState.p1_lamp_fr = (value & 0x04u) ? 1:0;
            OUTState.p1_lamp_cl = (value & 0x08u) ? 1:0;
            OUTState.p1_lamp_cc = (value & 0x10u) ? 1:0;
            OUTState.p1_lamp_cr = (value & 0x20u) ? 1:0;
            OUTState.p1_lamp_rl = (value & 0x40u) ? 1:0;
            OUTState.p1_lamp_rc = (value & 0x80u) ? 1:0;
            //UpdateChannel_0();
            break;
        case TIO_OUT_CHANNEL_1:
            OUTState.p1_lamp_rr = (value & 0x01u) ? 1:0;
            OUTState.control_m0 = (value & 0x02u) ? 1:0;
            OUTState.control_m1 = (value & 0x04u) ? 1:0;
            OUTState.p2_lamp_fl = (value & 0x08u) ? 1:0;
            OUTState.p2_lamp_fc = (value & 0x10u) ? 1:0;
            OUTState.p2_lamp_fr = (value & 0x20u) ? 1:0;
            OUTState.p2_lamp_cl = (value & 0x40u) ? 1:0;
            OUTState.p2_lamp_cc = (value & 0x80u) ? 1:0;
            //UpdateChannel_1();
            break;
        case TIO_OUT_CHANNEL_2:
            OUTState.p2_lamp_cr = (value & 0x01u) ? 1:0;
            OUTState.p2_lamp_rl = (value & 0x02u) ? 1:0;
            OUTState.p2_lamp_rc = (value & 0x04u) ? 1:0;
            OUTState.p2_lamp_rr = (value & 0x08u) ? 1:0;
            OUTState.button_1   = (value & 0x10u) ? 1:0;
            OUTState.button_2   = (value & 0x20u) ? 1:0;
            OUTState.button_3   = (value & 0x40u) ? 1:0;
            OUTState.button_4   = (value & 0x80u) ? 1:0;
            //UpdateChannel_2();
            break;
        case TIO_OUT_CHANNEL_3:
            OUTState.button_5   = (value & 0x01u) ? 1:0;
            OUTState.button_6   = (value & 0x02u) ? 1:0;
            OUTState.button_7   = (value & 0x04u) ? 1:0;
            OUTState.button_8   = (value & 0x08u) ? 1:0;
            OUTState.button_9   = (value & 0x10u) ? 1:0;
            OUTState.coin_ctr   = (value & 0x20u) ? 1:0;
            OUTState.lamp_1     = (value & 0x40u) ? 1:0;
            OUTState.lamp_2     = (value & 0x80u) ? 1:0;
            //UpdateChannel_3();
            break;
        case TIO_OUT_CHANNEL_4:
            OUTState.lamp_3     = (value & 0x01u) ? 1:0;
            OUTState.lamp_4     = (value & 0x02u) ? 1:0;
            OUTState.lamp_5     = (value & 0x04u) ? 1:0;
            OUTState.lamp_6     = (value & 0x08u) ? 1:0;
            OUTState.neon_1     = (value & 0x10u) ? 1:0;
            OUTState.neon_2     = (value & 0x20u) ? 1:0;
            //UpdateChannel_4();
            break;
    }
}



void init_cue() {
    HMODULE csdk = LoadLibraryA("CUESDK_2017.dll");

    if (csdk) {
        CorsairSetLedsColors = (tCorsairSetLedsColors*)GetProcAddress(csdk, "CorsairSetLedsColors");
        CorsairPerformProtocolHandshake = (tCorsairPerformProtocolHandshake*)GetProcAddress(csdk, "CorsairPerformProtocolHandshake");
    }
    if (!csdk || !CorsairSetLedsColors || !CorsairPerformProtocolHandshake) { return; }

    CorsairPerformProtocolHandshake();

    // Reset all the Lights
    UpdateChannel_0();
    UpdateChannel_1();
    UpdateChannel_2();
    UpdateChannel_3();
    UpdateChannel_4();

}

int light_thread_running = 0;
void light_thread(){
    while(light_thread_running){
        UpdateChannel_0();
        UpdateChannel_1();
        UpdateChannel_2();
        UpdateChannel_3();
        UpdateChannel_4();
    }
}



static void load_config() {
    char ini_path[1024] = {0x00};
    if(!GetEnvironmentVariableA("CONFIG_INI_PATH", ini_path,sizeof(ini_path))){
        strcpy(ini_path,"D:\\Techmo\\technotools.ini");
    }
    char current_data[64] = { 0x00 };

    GetPrivateProfileStringA("TECHNO_IO", "OUT_LEFT_PAD_1", "1A", current_data, sizeof(current_data), ini_path);
    Lights_Channel_0[0].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_LEFT_PAD_2", "1B", current_data, sizeof(current_data), ini_path);
    Lights_Channel_0[1].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_LEFT_PAD_3", "1C", current_data, sizeof(current_data), ini_path);
    Lights_Channel_0[2].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_LEFT_PAD_4", "26", current_data, sizeof(current_data), ini_path);
    Lights_Channel_0[3].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_LEFT_PAD_5", "27", current_data, sizeof(current_data), ini_path);
    Lights_Channel_0[4].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_LEFT_PAD_6", "28", current_data, sizeof(current_data), ini_path);
    Lights_Channel_0[5].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_LEFT_PAD_7", "33", current_data, sizeof(current_data), ini_path);
    Lights_Channel_0[6].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_LEFT_PAD_8", "34", current_data, sizeof(current_data), ini_path);
    Lights_Channel_0[7].ledId = strtoul(current_data, NULL, 16) & 0xFFu;


    GetPrivateProfileStringA("TECHNO_IO", "OUT_LEFT_PAD_9", "35", current_data, sizeof(current_data), ini_path);
    Lights_Channel_1[0].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_CONTROL_M0", "4A", current_data, sizeof(current_data), ini_path);
    Lights_Channel_1[1].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_CONTROL_M1", "4C", current_data, sizeof(current_data), ini_path);
    Lights_Channel_1[2].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_RIGHT_PAD_1", "6D", current_data, sizeof(current_data), ini_path);
    Lights_Channel_1[3].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_RIGHT_PAD_2", "6E", current_data, sizeof(current_data), ini_path);
    Lights_Channel_1[4].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_RIGHT_PAD_3", "6F", current_data, sizeof(current_data), ini_path);
    Lights_Channel_1[5].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_RIGHT_PAD_4", "71", current_data, sizeof(current_data), ini_path);
    Lights_Channel_1[6].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_RIGHT_PAD_5", "72", current_data, sizeof(current_data), ini_path);
    Lights_Channel_1[7].ledId = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "OUT_RIGHT_PAD_6", "73", current_data, sizeof(current_data), ini_path);
    Lights_Channel_2[0].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_RIGHT_PAD_7", "74", current_data, sizeof(current_data), ini_path);
    Lights_Channel_2[1].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_RIGHT_PAD_8", "75", current_data, sizeof(current_data), ini_path);
    Lights_Channel_2[2].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_RIGHT_PAD_9", "76", current_data, sizeof(current_data), ini_path);
    Lights_Channel_2[3].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_BUTTONBOARD_1", "0E", current_data, sizeof(current_data), ini_path);
    Lights_Channel_2[4].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_BUTTONBOARD_2", "0F", current_data, sizeof(current_data), ini_path);
    Lights_Channel_2[5].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_BUTTONBOARD_3", "10", current_data, sizeof(current_data), ini_path);
    Lights_Channel_2[6].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_BUTTONBOARD_4", "5E", current_data, sizeof(current_data), ini_path);
    Lights_Channel_2[7].ledId = strtoul(current_data, NULL, 16) & 0xFFu;

    GetPrivateProfileStringA("TECHNO_IO", "OUT_BUTTONBOARD_5", "53", current_data, sizeof(current_data), ini_path);
    Lights_Channel_3[0].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_BUTTONBOARD_6", "60", current_data, sizeof(current_data), ini_path);
    Lights_Channel_3[1].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_BUTTONBOARD_7", "14", current_data, sizeof(current_data), ini_path);
    Lights_Channel_3[2].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_BUTTONBOARD_8", "15", current_data, sizeof(current_data), ini_path);
    Lights_Channel_3[3].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_BUTTONBOARD_9", "16", current_data, sizeof(current_data), ini_path);
    Lights_Channel_3[4].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_COIN", "12", current_data, sizeof(current_data), ini_path);
    Lights_Channel_3[5].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_LAMP_1", "4D", current_data, sizeof(current_data), ini_path);
    Lights_Channel_3[6].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_LAMP_2", "58", current_data, sizeof(current_data), ini_path);
    Lights_Channel_3[7].ledId = strtoul(current_data, NULL, 16) & 0xFFu;


    GetPrivateProfileStringA("TECHNO_IO", "OUT_LAMP_3", "59", current_data, sizeof(current_data), ini_path);
    Lights_Channel_4[0].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_LAMP_4", "4E", current_data, sizeof(current_data), ini_path);
    Lights_Channel_4[1].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_LAMP_5", "5A", current_data, sizeof(current_data), ini_path);
    Lights_Channel_4[2].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_LAMP_6", "4F", current_data, sizeof(current_data), ini_path);
    Lights_Channel_4[3].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_NEON_1", "41", current_data, sizeof(current_data), ini_path);
    Lights_Channel_4[4].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_NEON_2_LEFT", "4B", current_data, sizeof(current_data), ini_path);
    Lights_Channel_4[5].ledId = strtoul(current_data, NULL, 16) & 0xFFu;
    GetPrivateProfileStringA("TECHNO_IO", "OUT_NEON_2_RIGHT", "4C", current_data, sizeof(current_data), ini_path);
    Lights_Channel_4[6].ledId = strtoul(current_data, NULL, 16) & 0xFFu;


}

void init_lights_emu(){
    init_cue();
    load_config();
    light_thread_running = TRUE;
    CreateThread(NULL, 0, (void*)light_thread, NULL, 0, NULL);
}
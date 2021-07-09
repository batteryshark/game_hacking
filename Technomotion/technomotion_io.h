#pragma once


#define TIO_IN_CHANNEL_0 0x2C0
#define TIO_IN_CHANNEL_1 0x2C1
#define TIO_IN_CHANNEL_2 0x2C2
#define TIO_IN_CHANNEL_3 0x2C3

#define TIO_OUT_CHANNEL_0 0x2D0
#define TIO_OUT_CHANNEL_1 0x2D1
#define TIO_OUT_CHANNEL_2 0x2D2
#define TIO_OUT_CHANNEL_3 0x2D3
#define TIO_OUT_CHANNEL_4 0x2D4


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
};

struct TIO_Input{
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
};

void tio_set_value(unsigned short port, unsigned char value);
unsigned char tio_get_value(unsigned short port);
void tio_init();
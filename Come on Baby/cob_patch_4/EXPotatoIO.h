#pragma once
#define EXPOTATO_PORT_DATA   0x378
#define EXPOTATO_PORT_STATUS 0x379
#define EXPOTATO_PORT_CTRL   0x37A

#define INPUT_MODE_P1_CONTROL 0
#define INPUT_MODE_P1_BUTTONS 1
#define INPUT_MODE_P2_CONTROL 2
#define INPUT_MODE_P2_BUTTONS 3
#define INPUT_MODE_SYSTEM 4
#define INPUT_MODE_CHECK 8

#define IO_OK 0x50

#define MASK_IO_UP    0x10u
#define MASK_IO_DOWN  0x20u
#define MASK_IO_LEFT  0x40u
#define MASK_IO_RIGHT 0x80u

#define MASK_IO_A 0x10u
#define MASK_IO_B 0x20u
#define MASK_IO_C 0x40u

#define MASK_IO_TEST    0x10u
#define MASK_IO_SERVICE 0x20u
#define MASK_IO_COIN    0x40u

struct io_registers{
    unsigned char p1_up;
    unsigned char p1_down;
    unsigned char p1_left;
    unsigned char p1_right;
    unsigned char p1_a;
    unsigned char p1_b;
    unsigned char p1_c;
    unsigned char p2_up;
    unsigned char p2_down;
    unsigned char p2_left;
    unsigned char p2_right;
    unsigned char p2_a;
    unsigned char p2_b;
    unsigned char p2_c;
    unsigned char test;
    unsigned char service;
    unsigned char coin;
};

extern io_registers EXIORegs;

void data_port_read(unsigned int* value);
void data_port_write(unsigned int* value);
void status_port_read(unsigned int* value);
void status_port_write(unsigned int* value);
void ctrl_port_read(unsigned int* value);
void ctrl_port_write(unsigned int* value);

void init_ioregs();
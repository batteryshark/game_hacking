#pragma once
#define TIO_OUT_CHANNEL_0 0x2D0
#define TIO_OUT_CHANNEL_1 0x2D1
#define TIO_OUT_CHANNEL_2 0x2D2
#define TIO_OUT_CHANNEL_3 0x2D3
#define TIO_OUT_CHANNEL_4 0x2D4

#define ASM_OUT_DX_AL 0xEE


void init_lights_emu();
void tio_set_value(unsigned short port, unsigned char value);
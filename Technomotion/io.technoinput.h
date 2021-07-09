#pragma once

#define ASM_IN_AL_DX  0xEC

#define TIO_IN_CHANNEL_0 0x2C0
#define TIO_IN_CHANNEL_1 0x2C1
#define TIO_IN_CHANNEL_2 0x2C2
#define TIO_IN_CHANNEL_3 0x2C3


void init_input_emu();
unsigned char tio_get_value(unsigned short port);
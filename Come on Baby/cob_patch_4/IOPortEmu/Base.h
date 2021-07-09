#pragma once

#include "IOPortHandler.h"

// OUT Instructions (to hardware from application)
// -- 8bit ports
#define OPCODE_OUT_IMM8_AL  0xE6
#define OPCODE_OUT_IMM8_AX  0xE7
#define OPCODE_OUT_IMM8_EAX 0xE7
// -- 16bit ports
#define OPCODE_OUT_DX_AL    0xEE
#define OPCODE_OUT_DX_AX    0xEF
#define OPCODE_OUT_DX_EAX   0xEF

// IN Instructions (to application from hardware)
// -- 8bit ports
#define OPCODE_IN_AL_IMM8   0xE4
#define OPCODE_IN_AX_IMM8   0xE5
#define OPCODE_IN_EAX_IMM8  0xE5
// -- 16bit ports
#define OPCODE_IN_AL_DX     0xEC
#define OPCODE_IN_AX_DX     0xED
#define OPCODE_IN_EAX_DX    0xED

#define OPERAND_PREFIX 0x66


class Base {
public:
    IOPortHandler* input_handler;
    IOPortHandler* output_handler;
    unsigned char handle_io(const unsigned char* inst_ptr,unsigned int* xax, unsigned int xdx);
    Base(){input_handler = new IOPortHandler();output_handler = new IOPortHandler();}
};
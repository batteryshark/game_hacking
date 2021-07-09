#include "Base.h"
#include "IOPortHandler.h"

unsigned char Base::handle_io(const unsigned char* inst_ptr, unsigned int* xax, unsigned int xdx){

    // For 16bit imm8 calls, there is a preface instruction of 0x66
    // Probably due to the fact that Intel re-used the instruction 
    // for 32bit?
    unsigned int inst_indx = 0;
    if(inst_ptr[0] == OPERAND_PREFIX){inst_indx++;}
    unsigned short current_port;

    switch(inst_ptr[inst_indx]){
    // -- OUTPUTS --
        // E6: OUT 8bit port, 8bit data, +2 ip.
        case OPCODE_OUT_IMM8_AL:
            current_port = inst_ptr[inst_indx+1];
            output_handler->process(current_port,xax, 0xFF);
            return 2 + inst_indx;
        // E7: OUT 8bit port, 16/32bit data, +2 ip.
        case OPCODE_OUT_IMM8_EAX:
            current_port = inst_ptr[inst_indx+1];
            output_handler->process(current_port,xax,0xFFFF);
            return 2 + inst_indx;
            // EE: OUT 16bit port, 8bit data, +1 ip.
        case OPCODE_OUT_DX_AL:
            output_handler->process(xdx, xax, 0xFF);
            return 1;
            // EF: OUT 16bit port, 16/32bit data, +1 ip.
        case OPCODE_OUT_DX_EAX:
            output_handler->process(xdx, xax, 0xFFFF);
            return 1;
    // -- INPUTS --
        // E4  IN 8bit port, 8bit data, +2 ip.
        case OPCODE_IN_AL_IMM8:
            current_port = inst_ptr[inst_indx+1];
            input_handler->process(current_port, xax, 0xFF);
            return 2 + inst_indx;
        // E5  IN 8bit port, 16/32bit data, +2 ip.
        case OPCODE_IN_EAX_IMM8:
            current_port = inst_ptr[inst_indx+1];
            input_handler->process(current_port, xax, 0xFFFF);
            return 2 + inst_indx;
        // EC: IN 16bit port, 8bit data, +1 ip.
        case OPCODE_IN_AL_DX:
            input_handler->process(xdx, xax, 0xFF);
            return 1;
        // EC: IN 16bit port, 32bit data, +1 ip.
        case OPCODE_IN_EAX_DX:
            input_handler->process(xdx, xax, 0xFFFF);
            return 1;
        default:
            return 0;

    }
}
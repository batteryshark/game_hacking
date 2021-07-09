#include <cstdlib>
#include "EXPotatoIO.h"
#include "dbg_utils.h"
static unsigned char data_register = 0;
static unsigned char status_register = 0;
static unsigned char ctrl_register = 0;
io_registers EXIORegs;
void data_port_read(unsigned int* value){
    DbgUtils__dbg_printf("Data Port Read\n");
    *value = data_register;
}
void data_port_write(unsigned int* value){
   // DbgUtils__dbg_printf("Data Port Write\n");
    data_register = *(unsigned char*)value;
}
void status_port_read(unsigned int* value){
  //  DbgUtils__dbg_printf("Status Port Read [%#02x | %#02x | %#02x]\n",data_register, status_register, ctrl_register);
    switch(data_register){
        case INPUT_MODE_P1_CONTROL:
            status_register = 0;
            if(EXIORegs.p1_up)    {status_register |= MASK_IO_UP;}
            if(EXIORegs.p1_down)  {status_register |= MASK_IO_DOWN;}
            if(EXIORegs.p1_left)  {status_register |= MASK_IO_LEFT;}
            if(EXIORegs.p1_right) {status_register |= MASK_IO_RIGHT;}
            status_register = ~status_register;
            break;
        case INPUT_MODE_P1_BUTTONS:
            status_register = 0;
            if(EXIORegs.p1_a)  {status_register |= MASK_IO_A;}
            if(EXIORegs.p1_b)  {status_register |= MASK_IO_B;}
            if(EXIORegs.p1_c)  {status_register |= MASK_IO_C;}
            status_register = ~status_register;
            break;
        case INPUT_MODE_P2_CONTROL:
            status_register = 0;
            if(EXIORegs.p2_up)    {status_register |= MASK_IO_UP;}
            if(EXIORegs.p2_down)  {status_register |= MASK_IO_DOWN;}
            if(EXIORegs.p2_left)  {status_register |= MASK_IO_LEFT;}
            if(EXIORegs.p2_right) {status_register |= MASK_IO_RIGHT;}
            status_register = ~status_register;
            break;
        case INPUT_MODE_P2_BUTTONS:
            status_register = 0;
            if(EXIORegs.p2_a)  {status_register |= MASK_IO_A;}
            if(EXIORegs.p2_b)  {status_register |= MASK_IO_B;}
            if(EXIORegs.p2_c)  {status_register |= MASK_IO_C;}
            status_register = ~status_register;
            break;
        case INPUT_MODE_SYSTEM:
            status_register = 0;
            if(EXIORegs.test)   { status_register |= MASK_IO_TEST;}
            if(EXIORegs.service){ status_register |= MASK_IO_SERVICE;}
            if(EXIORegs.coin)   { status_register |= MASK_IO_COIN;}
            status_register = ~status_register;
            break;
        case INPUT_MODE_CHECK:
            status_register = IO_OK;
            break;
        default:
            break;
    }
    *value = status_register;
}
void status_port_write(unsigned int* value){
   // DbgUtils__dbg_printf("Status Port Write\n");
    status_register = *(unsigned char*)value;
}
void ctrl_port_read(unsigned int* value){
    DbgUtils__dbg_printf("Control Port Read\n");
    *value = ctrl_register;
}
void ctrl_port_write(unsigned int* value){
   // DbgUtils__dbg_printf("Control Port Write\n");
    ctrl_register = *(unsigned char*)value;
}

void init_ioregs(){
    EXIORegs.coin = 0;
    EXIORegs.service = 0;
    EXIORegs.test = 0;
    EXIORegs.p1_up = 0;
    EXIORegs.p1_down = 0;
    EXIORegs.p1_left = 0;
    EXIORegs.p1_right = 0;
    EXIORegs.p1_a = 0;
    EXIORegs.p1_b = 0;
    EXIORegs.p1_c = 0;
    EXIORegs.p2_up = 0;
    EXIORegs.p2_down = 0;
    EXIORegs.p2_left = 0;
    EXIORegs.p2_right = 0;
    EXIORegs.p2_a = 0;
    EXIORegs.p2_b = 0;
    EXIORegs.p2_c = 0;
}
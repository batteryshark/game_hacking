#include <memoryapi.h>
#include "IOPortEmu/Win32.h"
#include "EXPotatoIO.h"
#include "KeyboardIO.h"

static int is_init = 0;
static IOPortEmuWin32* ioport_emu = nullptr;

// Temporarily Patch the HDD Check
void patch_ret1(void* target_addr){
    unsigned char x86_ret_1[4] = {0x31,0xC0, 0x40, 0xC3};
    DWORD old_prot = 0;
    VirtualProtect(target_addr,sizeof(x86_ret_1),PAGE_READWRITE,&old_prot);
    memcpy(target_addr,x86_ret_1,sizeof(x86_ret_1));
    VirtualProtect(target_addr,sizeof(x86_ret_1),old_prot,&old_prot);
}

void patch_binary(){
    patch_ret1((void*)0x412790);
}

void init_io(){
    init_ioregs();
    init_keyboard_io();
    ioport_emu = new IOPortEmuWin32();
    ioport_emu->input_handler->register_handler(EXPOTATO_PORT_DATA,data_port_read);
    ioport_emu->output_handler->register_handler(EXPOTATO_PORT_DATA,data_port_write);
    ioport_emu->input_handler->register_handler(EXPOTATO_PORT_STATUS,status_port_read);
    ioport_emu->output_handler->register_handler(EXPOTATO_PORT_STATUS,status_port_write);
    ioport_emu->input_handler->register_handler(EXPOTATO_PORT_CTRL,ctrl_port_read);
    ioport_emu->output_handler->register_handler(EXPOTATO_PORT_CTRL,ctrl_port_write);
}

// Entry-Point
static void __attribute__((constructor)) init_library(){
    if(!is_init){
        init_io();
        patch_binary(); // TODO: Remove
        is_init = 1;
    }
}

static void __attribute__((destructor)) deinit_library(){
    if(ioport_emu){
        delete ioport_emu;
    }
}




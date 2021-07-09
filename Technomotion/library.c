// Technomotion IO Emulator
#include <Windows.h>

#include <stdbool.h>


#include "global.h"
#include "io.cmos.h"

#include "technomotion_io.h"
#include "patches.h"
#include "debug.h"

#include <patching.h>

#define EXPORTABLE __declspec(dllexport)
EXPORTABLE void fuckstick(){}

#define ASM_OUT_DX_AL 0xEE
#define ASM_IN_AL_DX  0xEC

#define ASM_IN_AL  0xE4
#define ASM_OUT_AL 0xE6



LONG WINAPI priv_instr_exception_handler(PEXCEPTION_POINTERS pExceptionInfo){
    // If this isn't what we want, skip it...
    if(pExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_PRIV_INSTRUCTION) {
        return EXCEPTION_EXECUTE_HANDLER;
    }
    // Determine OpCode
    unsigned char opcode = *(unsigned char*)pExceptionInfo->ContextRecord->Eip;
    unsigned short port_16 = 0;
    unsigned char port_8 = 0;
    switch(opcode){
        case ASM_OUT_DX_AL: // Technomotion IO Output
            port_16 = pExceptionInfo->ContextRecord->Edx & 0xFFFF;
            if(port_16 >= TIO_OUT_CHANNEL_0 && port_16 <= TIO_OUT_CHANNEL_4){
                tio_set_value(port_16,pExceptionInfo->ContextRecord->Eax & 0xFF);
            }
            break;
        case ASM_IN_AL_DX: // Technomotion IO Input
            port_16 = pExceptionInfo->ContextRecord->Edx & 0xFFFF;
            if(port_16 >= TIO_IN_CHANNEL_0 && port_16 <= TIO_IN_CHANNEL_3){
                pExceptionInfo->ContextRecord->Eax = tio_get_value(port_16);
            }
            break;
        case ASM_OUT_AL: // CMOS RTC OUTPUT
            port_8 = *(unsigned char*)pExceptionInfo->ContextRecord->Eip+1;
            pExceptionInfo->ContextRecord->Eip++;
            if(port_8 == INST_CMOS_CTRL){
                rtc_set_register(pExceptionInfo->ContextRecord->Eax & 0xFF);
            }
            break;
        case ASM_IN_AL: // CMOS RTC INPUT
            port_8 = *(unsigned char*)pExceptionInfo->ContextRecord->Eip+1;
            pExceptionInfo->ContextRecord->Eip++;
            if(port_8 == INST_CMOS_DATA){
                pExceptionInfo->ContextRecord->Eax = rtc_get_register_value();
            }
            break;
        default:
            DBG_printf("ERROR: Unsupported OpCode: %02X",opcode);
            return EXCEPTION_EXECUTE_HANDLER;

    }

    pExceptionInfo->ContextRecord->Eip++;
    return EXCEPTION_CONTINUE_EXECUTION;
}



BOOL patch_game(){
    // Initialize our CMOS RTC Thread.
    rtc_init();
    // Initialize our IO Thread
    tio_init();
    // Set any Binary Patches
    patch_binary();

    SetUnhandledExceptionFilter(priv_instr_exception_handler);
    return TRUE;
}


// Entry-Point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        // Patch to Support OG Korean Paths.
        // The Win95 application assumed ANSI and CP949 which will break on most non-korean applications... let's fix this!
        LoadLibraryA("filesystem.locale.cp949.dll");
        LoadLibraryA("dsound.dll");
        if(patch_game()){
            DBG_printf("TechnoIO Attached!");
            return TRUE;
        }
        return FALSE;
    }
    return TRUE;
}


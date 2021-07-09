#include <Windows.h>

#include "io.cmos.h"

#include "io.technoio.h"
#include "io.technoinput.h"
#include "io.technooutput.h"
#include "kitchen_sink/kitchen_sink.h"

static int emu_cmos = 0;
static int emu_input = 0;
static int emu_lights = 0;

LONG WINAPI priv_instr_exception_handler(PEXCEPTION_POINTERS pExceptionInfo){
    // If this isn't what we want, skip it...
    if(pExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_PRIV_INSTRUCTION) {
           if(pExceptionInfo->ExceptionRecord->ExceptionCode!= DBG_PRINTEXCEPTION_C){
               DBG_printfA("ExceptionCode: %04X", pExceptionInfo->ExceptionRecord->ExceptionCode);

        }
        return EXCEPTION_EXECUTE_HANDLER;
    }
    // Determine OpCode
    unsigned char opcode = *(unsigned char*)pExceptionInfo->ContextRecord->Eip;
    unsigned short port_16 = 0;
    unsigned char port_8 = 0;
    switch(opcode){
        case ASM_OUT_DX_AL: // Technomotion IO Output
            if(!emu_lights){return EXCEPTION_EXECUTE_HANDLER;}
            port_16 = pExceptionInfo->ContextRecord->Edx & 0xFFFF;
            if(port_16 >= TIO_OUT_CHANNEL_0 && port_16 <= TIO_OUT_CHANNEL_4){
                tio_set_value(port_16,pExceptionInfo->ContextRecord->Eax & 0xFF);
            }
            break;
        case ASM_IN_AL_DX: // Technomotion IO Input
            if(!emu_input){return EXCEPTION_EXECUTE_HANDLER;}
            port_16 = pExceptionInfo->ContextRecord->Edx & 0xFFFF;
            if(port_16 >= TIO_IN_CHANNEL_0 && port_16 <= TIO_IN_CHANNEL_3){
                pExceptionInfo->ContextRecord->Eax = tio_get_value(port_16);
            }
            break;
        case ASM_OUT_AL: // CMOS RTC OUTPUT
            if(!emu_cmos){return EXCEPTION_EXECUTE_HANDLER;}
            port_8 = *(unsigned char*)pExceptionInfo->ContextRecord->Eip+1;
            pExceptionInfo->ContextRecord->Eip++;
            if(port_8 == INST_CMOS_CTRL){
                cmos_set_register(pExceptionInfo->ContextRecord->Eax & 0xFF);
            }else if(port_8 == INST_CMOS_DATA){
                cmos_set_data(pExceptionInfo->ContextRecord->Eax & 0xFF)
            }
            break;
        case ASM_IN_AL: // CMOS RTC INPUT
            if(!emu_cmos){return EXCEPTION_EXECUTE_HANDLER;}
            port_8 = *(unsigned char*)pExceptionInfo->ContextRecord->Eip+1;
            pExceptionInfo->ContextRecord->Eip++;
            pExceptionInfo->ContextRecord->Eax = rtc_get_register_value(port_8);
            if(port_8 == INST_CMOS_CTRL){
                pExceptionInfo->ContextRecord->Eax = cmos_get_register();
            }else if(port_8 == INST_CMOS_DATA){
                pExceptionInfo->ContextRecord->Eax = cmos_get_data();
            }
            break;
        default:
            return EXCEPTION_EXECUTE_HANDLER;

    }


    pExceptionInfo->ContextRecord->Eip++;
    return EXCEPTION_CONTINUE_EXECUTION;
}

void technoio_init(int cmos, int input, int lights){
    emu_cmos = cmos;
    emu_input = input;
    if(input){init_input_emu();}
    emu_lights = lights;
    if(lights){init_lights_emu();}
    SetUnhandledExceptionFilter(priv_instr_exception_handler);
}


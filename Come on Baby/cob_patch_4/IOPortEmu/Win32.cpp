#include <cstdio>
#include <cstdlib>

#include "Win32.h"

#ifdef TARGET_ARCH_64
    #define XIP_PTR pExceptionInfo->ContextRecord->Rip
    #define XAX pExceptionInfo->ContextRecord->Rax
    #define XDX pExceptionInfo->ContextRecord->Rdx
#else 
    #define XIP_PTR pExceptionInfo->ContextRecord->Eip
    #define XAX pExceptionInfo->ContextRecord->Eax
    #define XDX pExceptionInfo->ContextRecord->Edx  
#endif

#define EXCEPTION_PRIV_INSTRUCTION 0xC0000096

static Base* handler_base = nullptr;

static __stdcall long pexception_handler(PEXCEPTION_POINTERS pExceptionInfo){
    // If this isn't what we want, skip it...
    if(pExceptionInfo->ExceptionRecord->ExceptionCode != EXCEPTION_PRIV_INSTRUCTION) {
        return EXCEPTION_EXECUTE_HANDLER;
    }
    unsigned char bump_ip = handler_base->handle_io((unsigned char*)XIP_PTR,(unsigned int*)&XAX, XDX);
    if(!bump_ip){
        return EXCEPTION_EXECUTE_HANDLER;
    }

    XIP_PTR += bump_ip;
    return EXCEPTION_CONTINUE_EXECUTION;
}

IOPortEmuWin32::~IOPortEmuWin32(){
    RemoveVectoredExceptionHandler( (PVOID)pexception_handler);
    if(handler_base){
        delete handler_base;
    }
}

IOPortEmuWin32::IOPortEmuWin32() {
    handler_base = new Base();
    // SEH and VEH calls cannot be from class members, so
    // we have to do this janky ass shit.
    this->input_handler = handler_base->input_handler;
    this->output_handler = handler_base->output_handler;
    AddVectoredExceptionHandler(1, (PVECTORED_EXCEPTION_HANDLER)pexception_handler);
}

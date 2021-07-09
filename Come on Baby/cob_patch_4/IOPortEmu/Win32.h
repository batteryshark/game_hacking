#pragma once

#include "Base.h"
#include <errhandlingapi.h>

class IOPortEmuWin32{
    public:
    IOPortHandler* input_handler;
    IOPortHandler* output_handler;
    IOPortEmuWin32();
    ~IOPortEmuWin32();
};

// Target-Specific Misc Functions

#include "global.h"
#include "target.h"


// Logging to the screen.
void ttg_msg(char *format, ...){
    va_list va;
    va_start(va, format);
    char msg[128] = {0x00};
    vsprintf(msg,format,va);
    printf("%s\n",msg);
}

//

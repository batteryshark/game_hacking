#include <stdio.h>
#include <debugapi.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>

#include "debug.h"

void DBG_printf(const char* format, ...) {
#ifdef ENABLE_DEBUG
    char s[8192];
    va_list args;
    memset(s,0x00, 8192 * sizeof(s[0]));
    va_start(args, format);
    vsnprintf(s, 8191, format, args);
    va_end(args);
    s[8191] = 0;
    OutputDebugStringA(s);
#endif
}

void DBG_print_buffer(unsigned char* data, size_t size){
#ifdef ENABLE_DEBUG
    size_t ansi_sz = (size * 2) + 1;
    char* a_buffer = NULL;
    a_buffer = (char*)calloc(1,ansi_sz);
    if(!a_buffer){return;}
    int i = 0;
    for(i=0;i<size;i++){
        snprintf(a_buffer + (i*2),ansi_sz,"%02x",data[i]);
    }
    OutputDebugStringA(a_buffer);
    free(a_buffer);
#endif
}

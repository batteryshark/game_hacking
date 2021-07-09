#include <cstdio>
#include <cstdarg>
#include <cstdlib>

#include "dbg_utils.h"

#define ENABLE_DEBUG
#define TARGET_PLATFORM_WINDOWS
#define DBG_BUFFER_MAX 8192

#ifdef TARGET_PLATFORM_WINDOWS
#include <debugapi.h>
    #define DBGPRINT(x) OutputDebugStringA(x)
#else
#include <syslog.h>
#define DBGPRINT(x) syslog(LOG_DEBUG,"%s\n",x)
#endif

void DbgUtils__dbg_printf(const char* fmt, ...){
#ifdef ENABLE_DEBUG
    char s[DBG_BUFFER_MAX] = {0x00};
    va_list args;
    va_start(args,fmt);
    vsnprintf(s,DBG_BUFFER_MAX-1,fmt,args);
    va_end(args);
    s[DBG_BUFFER_MAX-1] = 0x00;
    DBGPRINT(s);
#endif
}

void DbgUtils__dbg_print_buffer(unsigned char* data, size_t size){
#ifdef ENABLE_DEBUG
    size_t ansi_sz = (size * 2) + 1;
    char* a_buffer = NULL;
    a_buffer = (char*)calloc(1,ansi_sz);
    if(!a_buffer){return;}
    int i = 0;
    for(i=0;i<size;i++){
        snprintf(a_buffer + (i*2),ansi_sz,"%02x",data[i]);
    }
    DBGPRINT(a_buffer);
    free(a_buffer);
#endif
}

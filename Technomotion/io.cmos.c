// CMOS RTC Emulation
// Creates a faux CMOS buffer with populated RTC info from system time.
// Mostly for use in applications that call IOPORT 0x70-0x71 directly for
// RTC.

#include <time.h>

#ifndef _WIN32
    #include <pthread.h> 
#else 
    #include <processthreadsapi.h>    
#endif

#include "io.cmos.h"


// Misc Defines
#define CMOS_DATA_SZ 0x80
#define BIN2BCD(n) ( (((n) / 10) << 4) + ((n) % 10) )

// We'll only need one copy of these.
static unsigned char cmos_data[CMOS_DATA_SZ] = {
        0x03, 0xC1, 0x30, 0x44, 0x00, 0x40, 0x10, 0x01, 0x01, 0x00, 0x26, 0x02, 0x40, 0x80, 0x00, 0x00,
        0x00, 0x26, 0xF0, 0x00, 0x0F, 0x80, 0x02, 0x00, 0xFC, 0x2F, 0x00, 0x10, 0xF7, 0x01, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xAD, 0x4D, 0x10, 0xFF, 0xFF, 0xAC, 0x4D, 0x3F, 0x08, 0x1A,
        0x00, 0xFC, 0x20, 0x80, 0x00, 0x32, 0xCD, 0x06, 0x64, 0x4C, 0xCD, 0x0F, 0x99, 0xD1, 0xA9, 0x11,
        0xC2, 0x51, 0x70, 0x14, 0xC0, 0x40, 0x01, 0x00, 0x6F, 0xF0, 0x85, 0x28, 0x1C, 0xDC, 0x40, 0x07,
        0x88, 0x00, 0x00, 0x00, 0x05, 0x80, 0x56, 0x15, 0x6A, 0x05, 0xE3, 0xBD, 0x32, 0x00, 0x03, 0x90,
        0x4C, 0x27, 0xDF, 0xF7, 0x01, 0x0B, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xFF, 0x6E, 0x0E, 0x51, 0x00, 0x00, 0x7C
};
static unsigned char ctrl_register_port      = 0;
static unsigned char update_thread_running   = 0;

// Updates our CMOS Buffer's RTC registers from system time.
void update_rtc(){
    time_t rawtime;
    struct tm * today;
    update_thread_running = 1;
    while(update_thread_running){
        time ( &rawtime );
        today = localtime ( &rawtime );
        cmos_data[RTC_SECONDS]      = BIN2BCD(today->tm_sec);
        cmos_data[RTC_MINUTES]      = BIN2BCD(today->tm_min);
        cmos_data[RTC_HOURS]        = BIN2BCD(today->tm_hour);
        cmos_data[RTC_DAY_OF_MONTH] = BIN2BCD(today->tm_mday);
        cmos_data[RTC_MONTH]        = BIN2BCD(today->tm_mon + 1);
        // Note: Coming back from BCD this will show 2019 as '19'.
        // This is expected due to Y2K
        cmos_data[RTC_YEAR]         = BIN2BCD((today->tm_year + 1900) % 100);        
    }
}

void rtc_init(){
    #ifndef _WIN32
        pthread_t thread;
        pthread_create(&thread,NULL, update_rtc, NULL);
    #else
        CreateThread(NULL, 0, (void*)update_rtc, NULL, 0, NULL);
    #endif
}

void rtc_shutdown(){
    update_thread_running = 0;
}


unsigned char cmos_get_register(){
    return ctrl_register_port;
}

void cmos_set_register(unsigned char value){
    ctrl_register_port = value;
}

unsigned char cmos_get_data(){
    return cmos_data[ctrl_register_port];
}

void cmos_set_data(unsigned char value){
    cmos_data[ctrl_register_port] = value;
}


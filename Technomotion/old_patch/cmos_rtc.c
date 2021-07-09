#include <time.h>
#include <processthreadsapi.h>
#include <stdbool.h>
#include "cmos_rtc.h"
#include "global.h"

static inline unsigned char bin2bcd(unsigned val){ return ((val / 10) << 4) + val % 10;}

static struct rtc_device rtc;

unsigned char patch_running;

void thread_update_rtc(){
    struct tm *tm;
    time_t ti;

    while(patch_running){
        time(&ti);
        tm = localtime(&ti);
        rtc.cmos_data[RTC_SECONDS] = bin2bcd(tm->tm_sec);
        rtc.cmos_data[RTC_MINUTES] = bin2bcd(tm->tm_min);
        rtc.cmos_data[RTC_HOURS] = bin2bcd(tm->tm_hour);
        rtc.cmos_data[RTC_DAY_OF_MONTH] = bin2bcd(tm->tm_mday);
        rtc.cmos_data[RTC_MONTH] =  bin2bcd(tm->tm_mon + 1);
        rtc.cmos_data[RTC_YEAR] =  bin2bcd((tm->tm_year + 1900) % 100);
    }
}

unsigned char get_rtc_register() {
    return rtc.cmos_data[rtc.cmos_idx];
}

void set_rtc_register_index(unsigned char index){
    rtc.cmos_idx = index;
}

void init_cmos_rtc(){
    CreateThread(NULL, 0, (void*)thread_update_rtc, NULL, 0, NULL);
}
#pragma once

// IOPORT Defines
#define INST_CMOS_CTRL  0x70
#define INST_CMOS_DATA  0x71


// RTC Defines
#define RTC_SECONDS		    0x00
#define RTC_SECONDS_ALARM	0x01
#define RTC_MINUTES		    0x02
#define RTC_MINUTES_ALARM	0x03
#define RTC_HOURS		    0x04
#define RTC_HOURS_ALARM		0x05
#define RTC_DAY_OF_WEEK		0x06
#define RTC_DAY_OF_MONTH	0x07
#define RTC_MONTH		    0x08
#define RTC_YEAR		    0x09
#define RTC_REG_A		    0x0A
#define RTC_REG_B		    0x0B
#define RTC_REG_C		    0x0C
#define RTC_REG_D		    0x0D
#define RTC_CENTURY		    0x32


// API Exports
void rtc_init();
void rtc_shutdown();
unsigned char cmos_get_register();
void cmos_set_register(unsigned char value);
unsigned char cmos_get_data();
void cmos_set_data(unsigned char value);
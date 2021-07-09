#pragma once


#define PORT_CMOS_RTOS_IN  0x71
#define PORT_CMOS_RTOS_OUT 0x70

/*
 * MC146818 RTC registers
 */
#define RTC_SECONDS			0x00
#define RTC_SECONDS_ALARM	0x01
#define RTC_MINUTES			0x02
#define RTC_MINUTES_ALARM	0x03
#define RTC_HOURS			0x04
#define RTC_HOURS_ALARM		0x05
#define RTC_DAY_OF_WEEK		0x06
#define RTC_DAY_OF_MONTH	0x07
#define RTC_MONTH			0x08
#define RTC_YEAR			0x09
#define RTC_CENTURY			0x32

#define RTC_REG_A			0x0A
#define RTC_REG_B			0x0B
#define RTC_REG_C			0x0C
#define RTC_REG_D			0x0D


struct rtc_device {
    unsigned char cmos_idx;
    unsigned char cmos_data[128];
};


unsigned char get_rtc_register();
void init_cmos_rtc();
void set_rtc_register_index(unsigned char index);
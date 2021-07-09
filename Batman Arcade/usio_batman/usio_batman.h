#pragma once

#define DEVICE_VID  0xC70
#define DEVICE_PID  0x780

void get_usio_input(unsigned char* input_data);
void process_usio_output(unsigned char* output_data, unsigned int output_length);
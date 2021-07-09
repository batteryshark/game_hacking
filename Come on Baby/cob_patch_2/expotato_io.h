#pragma once

static struct LPT_REGISTERS {
	unsigned char data_register;    // x378
	unsigned char status_register;  // x379
	unsigned char control_register; // x37A
}LPTDEV;




void io_set_value(unsigned short port, unsigned char value);
unsigned char io_get_value(unsigned short port);
void io_init();
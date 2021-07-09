#include <Windows.h>

#include "global.h"
#include "expotato_io.h"

void io_set_value(unsigned short port, unsigned char value) {
	DBG_printf("IO_OUT: %02X %02X", port, value);
}

unsigned char io_get_value(unsigned short port) {
	DBG_printf("IO_IN: %02X", port);
	return 0;
}

void thread_keyboard_io() {
	while (patch_running) {

	}

}
void io_init() {
	CreateThread(NULL, 0, (void*)thread_keyboard_io, NULL, 0, NULL);
}
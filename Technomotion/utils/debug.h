#pragma once

#define ENABLE_DEBUG
void DBG_printf(const char* format, ...);
void DBG_print_buffer(unsigned char* data, size_t size);
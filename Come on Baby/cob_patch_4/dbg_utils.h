#pragma once

#include <cstddef>

#ifdef __cplusplus
extern "C" {
#endif
void DbgUtils__dbg_printf(const char* fmt, ...);
void DbgUtils__dbg_print_buffer(unsigned char* data, size_t size);
#ifdef __cplusplus
}
#endif

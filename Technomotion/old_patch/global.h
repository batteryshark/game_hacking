#pragma once

unsigned char patch_running;

#define ENABLE_DEBUG
void DBG_printf(const char* format, ...);
BOOL Hook_IAT_Name (char* dll_name, char* func_name,DWORD replacement_function_ptr);



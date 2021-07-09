#pragma once
#include <minwindef.h>
BOOL hook_iat_by_name (char* target_module, char* dll_name, char* func_name,DWORD replacement_function_ptr);

void patch_process(void* target_addr, unsigned char* patch_data, unsigned int patch_length);
#pragma once
void ProcessUnityIOIoctlWindows(unsigned int IoControlCode, unsigned char* InputBuffer,unsigned int InputBufferLength,unsigned char* OutputBuffer,unsigned int OutputBufferLength);
void io_init();
#include "library.h"
#include <stdio.h>
#include "global.h"
#include "fs.h"
#include "hdd.h"
#include "io.h"

#define EXPORTABLE __declspec(dllexport)

void patch_binary(){
    OutputDebugStr("Patching 1...");
    engine_init();
    OutputDebugStr("Patching 2...");
    patch_hdd();
    OutputDebugStr("Patching 3 ...");
    patch_io();
    OutputDebugStr("Patching 4...");
    patch_fs();
}
// Entry-Point
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved){

if (fdwReason == DLL_PROCESS_ATTACH){
    OutputDebugStr("Patching...");
    patch_binary(); }
return TRUE;
}

// Our Drop
void EXPORTABLE fuckstick(void){};
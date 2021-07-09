// PercussionMaster Emulation Framework by
// rFx - 2020

#include <stdio.h>

#include "patches.h"

// Entry Point.
void __attribute__((constructor)) initialize(void){
    printf("Starting PercussionMaster Emulator...\n");
    patch_binary();
}
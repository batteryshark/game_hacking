#include <string.h>

#include "./utils/patching.h"

#include "patches.h"

#define ADDR_WINDOWED_FLAG    0x402A9A
#define ADDR_ENABLE_EAX2_FLAG 0x440357
#define ADDR_REL_PATCH_1      0x462AA0

void patch_binary(){
    unsigned char nval = 0x00;


    // Patch to Skip EAX2.0 not Detected Error
  //  patch_process((void*)ADDR_ENABLE_EAX2_FLAG,&nval,1);

    // Patch to not read file from absolute path.
    char* rel_path_to_patch = ".\\BlackBox.tab\x00";
    patch_process((void*)ADDR_REL_PATCH_1,(unsigned char*)rel_path_to_patch,strlen(rel_path_to_patch)+1);
}
#ifndef TT_EMU_RTFS_H
#define TT_EMU_RTFS_H
#include "global.h"

// Structure Definitions
typedef struct lzwobj{
    unsigned int is_rb;
    unsigned int uncompressed_filesize;
    unsigned int idk_8;
    unsigned char* uncompressed_buffer;
};

// Function Definitions
void RTFS_TranslateFilename(char*src,char*dest);
struct lzwobj* RTFS_LZWfopen(char*src,char* a2);
int rtfs_open(char* src);
FILE* rtfs_fopen(char* src, int modes);
void force_unpack(char* srcz,char*destpath);

#endif // TT_EMU_RTFS_H


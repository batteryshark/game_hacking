//
// Created by root on 9/24/16.
//

#ifndef TT_EMU_RTFS_UTIL_H
#define TT_EMU_RTFS_UTIL_H
#include "global.h"


// Function Definitions
char* rtfs_convert_name(char* src);
void rtfs_fixfile(char* src,char* fpath,char is_compressed);
void create_path(char* src);
void copy_file(char*src,char*dest);
char check_decode_name(char* src,char* fromfile);
void fixslashes(char* src);
char path_exists(char* src); // used
signed int  RTFS_StandardizeFilename(char *s);

#endif //TT_EMU_RTFS_UTIL_H

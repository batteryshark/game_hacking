#include "rtfs_util.h"
#include <libgen.h>

// WARNING - COMPILE THIS FOR 32 BIT!!!

// Lookup 2 Hashing Function - http://burtleburtle.net/bob/c/lookup2.c
#define BYTE unsigned char
#define LOWORD(l) ((WORD)(l))
#define HIWORD(l) ((WORD)(((DWORD)(l) >> 16) & 0xFFFF))
#define LOBYTE(w) ((BYTE)(w))
#define HIBYTE(w) ((BYTE)(((WORD)(w) >> 8) & 0xFF))
typedef  unsigned long int  u4;   /* unsigned 4-byte type */ // <-- This is probably why 32 bit only.
typedef  unsigned     char  u1;   /* unsigned 1-byte type */

/* The mixing step */
#define mix(a,b,c) \
{ \
  a=a-b;  a=a-c;  a=a^(c>>13); \
  b=b-c;  b=b-a;  b=b^(a<<8);  \
  c=c-a;  c=c-b;  c=c^(b>>13); \
  a=a-b;  a=a-c;  a=a^(c>>12); \
  b=b-c;  b=b-a;  b=b^(a<<16); \
  c=c-a;  c=c-b;  c=c^(b>>5);  \
  a=a-b;  a=a-c;  a=a^(c>>3);  \
  b=b-c;  b=b-a;  b=b^(a<<10); \
  c=c-a;  c=c-b;  c=c^(b>>15); \
}

/* The whole new hash function */
u4 hash( k, length, initval)
        register u1 *k;        /* the key */
        u4           length;   /* the length of the key in bytes */
        u4           initval;  /* the previous hash, or an arbitrary value */
{

    register u4 a,b,c;  /* the internal state */
    u4          len;    /* how many key bytes still need mixing */

    /* Set up the internal state */
    len = length;
    a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
    c = initval;         /* variable initialization of internal state */

    /*---------------------------------------- handle most of the key */
    while (len >= 12)
    {
        a=a+(k[0]+((u4)k[1]<<8)+((u4)k[2]<<16) +((u4)k[3]<<24));
        b=b+(k[4]+((u4)k[5]<<8)+((u4)k[6]<<16) +((u4)k[7]<<24));
        c=c+(k[8]+((u4)k[9]<<8)+((u4)k[10]<<16)+((u4)k[11]<<24));
        mix(a,b,c);
        k = k+12; len = len-12;
    }

    /*------------------------------------- handle the last 11 bytes */
    c = c+length;
    switch(len)              /* all the case statements fall through */
    {
        case 11: c=c+((u4)k[10]<<24);
        case 10: c=c+((u4)k[9]<<16);
        case 9 : c=c+((u4)k[8]<<8);
            /* the first byte of c is reserved for the length */
        case 8 : b=b+((u4)k[7]<<24);
        case 7 : b=b+((u4)k[6]<<16);
        case 6 : b=b+((u4)k[5]<<8);
        case 5 : b=b+k[4];
        case 4 : a=a+((u4)k[3]<<24);
        case 3 : a=a+((u4)k[2]<<16);
        case 2 : a=a+((u4)k[1]<<8);
        case 1 : a=a+k[0];
            /* case 0: nothing left to add */
    }
    mix(a,b,c);
    /*-------------------------------------------- report the result */
    return c;
}

// Converts a 6bit value to a char.
int RTFS_6bitstochar(char a1) {
    int result; // eax@2

    if ( a1 & 0xC0 ) {
        result = 0x20;
    }else{
        result = a1 + 0x21;
        if ( a1 == 1 ){
            result = 0x7B;
        }else{
            switch ( result ) {
                case 0x2A:
                    result = 0x7D;
                    break;
                case 0x2F:
                    result = 0xA3u;
                    break;
                case 0x3A:
                    result = 0xA5u;
                    break;
                case 0x3C:
                    result = 0xBDu;
                    break;
                case 0x3E:
                    result = 0xBCu;
                    break;
                case 0x3F:
                    result = 0xDFu;
                    break;
                case 0x5C:
                    result = 0xB5u;
                    break;
            }
        }
    }
    return result;
}

// Converts a 32 bit uint to ASCII
int  RTFS_u32toASCII(unsigned int a1, char* a2){
    unsigned int v2; // esi@1
    int v3; // edi@1
    unsigned char *v4; // ebx@1
    int result; // eax@2

    v2 = a1;
    v3 = 0;
    *a2 = 0;
    *(a2 + 4) = 0;
    *(a2 + 6) = 0;
    v4 = a2;
    do{
        ++v3;
        *v4 = RTFS_6bitstochar(v2 & 0x3F);
        result = v3;
        v2 >>= 6;
        ++v4;
    }
    while ( v3 <= 5u );
    return result;
}

// Basically changes "\" to "/" and sets paths to lowercase.
signed int  RTFS_StandardizeFilename(char *s){
    signed int result; // eax@1
    int v2; // edx@1
    signed int v3; // ecx@1
    signed int i; // esi@1
    //rfx_printf("Standardizing %s\n",s);
    result = strlen(s);
    v3 = 0;
    for ( i = result; v3 < i; ++v3 ){
        v2 = s[v3];
        if ( v2 == 0x5C ){
            s[v3] = 0x2F;
        }else{
            result = v2 - 0x41;
            if ( (v2 - 0x41) <= 0x19u ){
                result = v2 + 32;
                s[v3] = v2 + 32;
            }
        }
    }
    return result;
}


// Checks if a path exists, what else?
char path_exists(char* src){
    if( access( src, F_OK ) != -1 )
        return 1;

    return NULL;
}

// Manually convert "\" -> "/"
void fixslashes(char* src){
    int i;
    for(i=0;i<strlen(src);i++){
        if(src[i] == 0x5C){
            src[i] = 0x2F;
        }
    }
}

// Compressed lzwtiff files embed their full path, this checks to make sure it's a valid compressed file.
char check_decode_name(char* src,char* fromfile){
    int i;
    for(i=0;i<strlen(fromfile);i++){
        fromfile[i] = ~fromfile[i];
    }
    for(i=0;i<strlen(src);i++){
        if(src[i] == 0x5C){
            src[i] = 0x2F;
        }

    }
    rfx_printf("Decoded Compressed Filename is: %s\n",fromfile);
    rfx_printf("Given Filename is: %s\n",src);
    if(strcasecmp(src,fromfile)){
        return 1;
    }
    return 0;
}

// Copies a file... what else?
void copy_file(char*src,char*dest){
    int f;
    int g;
    f = fopen(src,"rb");
    if(f == NULL){
        rfx_printf("Can't Copy, file doesn't exist\n");
        exit(1);
    }

    fseek(f,0,2);
    unsigned int file_size = ftell(f);
    fseek(f,0,0);
    g = fopen(dest,"wb");
    int i;
    unsigned char c[1] = {0x00};
    unsigned char* dstbuf = (unsigned char*)malloc(file_size);
    fread(dstbuf,file_size,1,f);
    fclose(f);
    fwrite(dstbuf,file_size,1,g);
    fclose(g);
    free(dstbuf);
    return;
}

// Given a file path, this creates the necessary directory structure.
void create_path(char* src){
    char* src2 = malloc(1024);
    strcpy(src2,src);
    unsigned char* cmd = (unsigned char*)malloc(2048);
    char* dir = dirname(src2);
    sprintf(cmd,"mkdir -p %s",dir);
    system(cmd);
    free(cmd);
    free(src2);
    return;
}

// Converts the path to an rtfs filename and copies the file to a new directory.
void rtfs_fixfile(char* src,char* fpath,char is_compressed){
    // First Priority - get the mangled-ass filename.
    unsigned char* crpt = (unsigned char*)malloc(16);
    char* bfpath = (char*)malloc(1024);
    strcpy(bfpath,fpath);
    crpt = rtfs_convert_name(src);
    if(path_exists(crpt) == NULL){
        printf("Fuck - Something went wrong with getting the file: %s aka %s\n",fpath,crpt);
        exit(1);
    }

    unsigned char* cmd = (unsigned char*)malloc(2048);
    char* dir = dirname(bfpath);
    sprintf(cmd,"mkdir -p %s",dir);
    system(cmd);
    memset(cmd,0x00,2048);
    sprintf(cmd,"cp %s %s",crpt,fpath);
    copy_file(crpt,fpath);
    if(path_exists(fpath) == NULL){
        printf("Fuck - Something went wrong with copying: %s\n",fpath);
        exit(1);
    }
    printf("Fixed File: %s -> %s\n",crpt,fpath);
    free(crpt);
    free(cmd);
    return;
}

// Converts a standard file path requested by the game into its mangled-ass counterpart.
char* rtfs_convert_name(char* src){
    // Create a buffer to hold the new name.
    char* output_buf = (char*)malloc(1024);
    char* tsrc = (char*)malloc(1024);
    strcpy(tsrc,src);

    RTFS_StandardizeFilename(tsrc);
    strcpy(output_buf,tsrc);
    unsigned int nh = hash(tsrc,strlen(src),0x1337D00D);
    output_buf[0] = 0x2F;
    RTFS_u32toASCII(nh,output_buf);
    return output_buf;
}
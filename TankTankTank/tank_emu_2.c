/*
    Tank!Tank!Tank! Arcade Patch by r4x
    2016 Team HackItUp
    
    compile: cc -shared -m32 tank_emu.c -w -ldl -D_GNU_SOURCE -o ./tank_emu.so
*/ 

//-- Visual Options - Patch to flip screen to landscape (launch with window)
#define LANDSCAPE

//-- Dongle Serial Tag - MAX 8 Characters
#define DONGLE_SERIAL "HACKITUP"

//-- Camera options - ONLY define one!
//#define NO_CAMERA
#define UNSUPPORTED_CAMERA
//#define SUPPORTED_CAMERA // Enable this if your camera supports cropping (VIDEOC_S_CROP) for proper camera support.

//-- Sound Options - ONLY define if you don't have surround51, you won't have all audio, anyway.
#define SOUND_FIX "sysdefault"

//-- IO Options - ONLY define if you aren't using the real I/O.
//#define IO_EMU

//-- Network Options - ONLY define if you're feeling lucky.
//#define NO_ETH0

//-- FS Options - Define these to clean up some paths.
#define SAVE_NOSAFE_FIX
#define LOCAL_SAVE

#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <stdint.h>
#include <stdarg.h>
#include <stdio.h>
//#include <GL/glx.h>
#include <time.h>

/*------------------------------
 * MEMORY MODIFICATON FUNCTIONS
 *----------------------------*/
// Memory Modification Macro
#define UNPROTECT(addr,len) (mprotect((void*)(addr-(addr%len)),len,PROT_READ|PROT_WRITE|PROT_EXEC))
void detour_function(void * new_adr, int addr)
{
    int call = (int)(new_adr - addr - 5);
    UNPROTECT(addr, 4096);
    *((unsigned char*)(addr)) = 0xE9;
    *((int*)(addr + 1)) = call;
}

/*------------------------
 * HASP WRAPPER FUNCTIONS
 *----------------------*/
int hasp_init_game()
{
    return 1;
}

// Returns the values stored in the HL MAX Flash Memory.
int hasp_read_dec_val(void *src) 
{
    unsigned char hasp_data[64] = {0x00};
    strcpy(hasp_data,DONGLE_SERIAL);
    
    memcpy(src,hasp_data,64);
    return 1;
}

int use_hasp_val(int hasp_decoded_buf)
{
    return 1;
}

int get_percent_num(char *str) {
    int cnt = 0;
    char *pch=strchr(str,'%');
    while (pch!=NULL) {
        cnt++;
        pch=strchr(pch+1,'%');
    }
    return cnt;
}
/*
int sprintf(char *str, const char *format, ...) {
    int ret, cnt;
    va_list va;
    
    cnt = get_percent_num(format);
    va_start(va, cnt);
    ret = vsprintf(str, format, va);

    printf("sprintf: %s\n", str);
    va_end(va);
    return ret;
}*/
/*
int snprintf(char *str, size_t size, const char *format, ...) {
    int ret, cnt;
    va_list va;
    
    cnt = get_percent_num(format);
    va_start(va, cnt);
    ret = vsnprintf(str, size, format, va);
    printf("snprintf: %s\n", str);
    va_end(va);
    return ret;
}
*/
// Another isDonglePresent Check.
int hasp_game_check()
{
    return 1;
}


/*-----------------
 * HOOK FUNCTIONS
 *----------------*/
// ioctl Hook Stub
static int (*real_ioctl)(int fd, unsigned long int request, int *data) = NULL;
// open Hook Stub -- Needed for IO EMU to work for, some reason...
static int (*real_open)(char *, int) = NULL;
static int (*real_xstat)(int ver, const char * path, struct stat * stat_buf) = NULL;
// ioctl Definitions
#define CMD64_VIDIOC_QUERYCAP 0x80685600
#define CMD64_VIDIOC_STREAMOFF 0x40045613
#define CMD64_VIDIOC_CROPCAP 0xc02c563a
#define CMD64_VIDIOC_S_CROP 0x4014563c
#define CMD64_VIDIOC_REQBUFS 0xc0145608
#define CMD32_VIDIOC_QUERYBUF 0xc0445609
#define CMD32_VIDIOC_S_FMT 0xc0cc5605
#define CMD32_VIDIOC_QBUF 0xc044560f
#define CMD32_VIDIOC_DQBUF 0xc0445611


int ioctl(int fd, int request, void * data)
{

    #ifdef IO_EMU
        // Fixes the "Call an Attendant" I/O Not Detected Error
        *((char*)0x0EC09200) = 0x00;
    #endif    
    switch(request){
        case CMD32_VIDIOC_DQBUF:
            #ifdef NO_CAMERA
                return 0;
            #else
                return real_ioctl(fd, request, data);
            #endif
        case CMD32_VIDIOC_QBUF:
            #ifdef NO_CAMERA
                return 0;
            #else
                return real_ioctl(fd, request, data);
            #endif           
        case CMD64_VIDIOC_STREAMOFF:
            #ifdef NO_CAMERA
                return 0;
            #else
                return real_ioctl(fd, request, data);
            #endif       
        case CMD64_VIDIOC_QUERYCAP:
            #ifdef NO_CAMERA
                return 0;
            #else
                return real_ioctl(fd, request, data);
            #endif               
        case CMD64_VIDIOC_CROPCAP:
            #ifdef NO_CAMERA
                return 0;
            #else
                return real_ioctl(fd, request, data);
            #endif               
        case CMD64_VIDIOC_S_CROP:
            #ifdef SUPPORTED_CAMERA
                real_ioctl(fd, request, data);
            #else
                return 0;
            #endif               
        case CMD32_VIDIOC_S_FMT:
            #ifdef NO_CAMERA
                return 0;
            #else
                return real_ioctl(fd, request, data);
            #endif               
        case CMD64_VIDIOC_REQBUFS:
            #ifdef NO_CAMERA
                return 0;
            #else
                return real_ioctl(fd, request, data);
            #endif               
        case CMD32_VIDIOC_QUERYBUF:
            #ifdef NO_CAMERA
                return 0;
            #else
                return real_ioctl(fd, request, data);
            #endif               
        default:
            return real_ioctl(fd, request, data);
    }
}

// Open Wrapper -- Needed for IO EMULATOR
int open(const char * filename, int oflag)
{
    if (real_open == NULL)
    {
      patch_binary();
      real_open = dlsym(RTLD_NEXT, "open");
      if (real_open == NULL)
        {
          printf("error: open not found\n");
          exit(-1);
        }
    }
    
#ifdef NO_CAMERA
    if (strcmp(filename, "/dev/video0") == 0)
    {
        printf("Hooking camera file\n");
        return real_open("info", oflag);
    }
#endif
    
    return real_open(filename, oflag);
}

int __xstat(int ver, const char * path, struct stat * stat_buf)
{
    if (real_xstat == NULL) {
        real_xstat = dlsym(RTLD_NEXT, "__xstat");
        if (real_xstat == NULL)
        {
          printf("error: __xstat not found\n");
          exit(-1);
        }
    }

    int ret;
    ret = real_xstat(ver, path, stat_buf);

#ifdef NO_CAMERA    
    if (strcmp(path, "/dev/video0") == 0) {
        printf("xstat camera mode!\n");
        ret = real_xstat(ver, "info", stat_buf);
        stat_buf->st_mode = 8624;
    }
#endif
    
    return ret;
}
static long last_time_ms;
static unsigned int sync_count;

int glXGetVideoSyncSGI(unsigned int* count){
     struct timeval current;
    long mtime, seconds, useconds;
    sync_count++;
    gettimeofday(&current, NULL);
    mtime = ((current.tv_sec) * 1000 + current.tv_usec/1000.0) + 0.5;
    if(mtime < last_time_ms + 23){
        long diff = (23 -(mtime-last_time_ms)) % 23;  
        usleep(diff * 1000);
    }
  	
    last_time_ms = mtime;
    *count = sync_count;
    return 1;
}

/*-----------------
 * INITIAL PATCHES
 *----------------*/
void patch_binary()
{
    // Hook ioctl
    real_ioctl = dlsym(RTLD_NEXT, "ioctl");
    
    
    #ifdef LANDSCAPE
        UNPROTECT(0x0804FE00, 4096);
        *((char*)0x0804FD26) = 0x00;
    #endif

    #ifdef SOUND_FIX
        char* sndstr_loc;
        char* nsndstr = SOUND_FIX;
        sndstr_loc = 0x0866A543;
        UNPROTECT(0x0866A543, 4096);
        strcpy(sndstr_loc,nsndstr);    
    #endif
            
    #ifdef IO_EMU
        // IO PCB Error Fix
        UNPROTECT((int)0x080A79C2, 4096);
        *((char*)0x080A79C2) = 0x00;
    #endif
        
    // Various Static Detours
    detour_function(&hasp_init_game, (int)0x080ABB60);
    detour_function(&hasp_read_dec_val, (int)0x080ABAC0);
    detour_function(&use_hasp_val, (int)0x080ABA00);
    detour_function(&hasp_game_check, (int)0x080ABA60);
    


    #ifdef NO_CAMERA
        UNPROTECT((int)0x080F5630, 4096);
        *((char*)0x080F5630) = 0x75;
        UNPROTECT((int)0x086D0966, 4096);
        *((char*)0x086D0966) = 0x02;
    #endif
    
    #ifdef NO_ETH0
        UNPROTECT((int)0x080A1329, 4096);
        *((char*)0x080A1329) = 0x00;        
    #endif

    #ifdef SAVE_NOSAFE_FIX
        UNPROTECT((int)0x080A7940, 4096);
        *((char*)0x080A7940) = 0xC3;
    #endif
    
    #ifdef LOCAL_SAVE
        unsigned char* save_path = "./pt";
        UNPROTECT((int)0x080A5470, 4096);
        memcpy(0x080A54D6,save_path,4);
        memcpy(0x080A5470,save_path,4);
    #endif
    
}

// Entry Point.
void __attribute__((constructor)) initialize(void)
{
    printf("Starting up Wrapper...\n");
    patch_binary();
}

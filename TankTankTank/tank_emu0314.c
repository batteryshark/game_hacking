/*
    Tank!Tank!Tank! Arcade Patch by r4x
    2016 Team HackItUp
    
    compile: cc -shared -m32 tank_emu.c -w -ldl -D_GNU_SOURCE -o ./tank_emu.so
*/ 

//-- Visual Options - Patch to flip screen to landscape (launch with window)
//#define LANDSCAPE

//-- Dongle Serial Tag - MAX 8 Characters
#define DONGLE_SERIAL "HACKITUP"

//-- Camera options - ONLY define one!
#define NO_CAMERA
//#define UNSUPPORTED_CAMERA
//#define SUPPORTED_CAMERA // Enable this if your camera supports cropping (VIDEOC_S_CROP) for proper camera support.

//-- Sound Options - ONLY define if you don't have surround51, you won't have all audio, anyway.
#define SOUND_FIX "sysdefault"

//-- IO Options - ONLY define if you aren't using the real I/O.
#define IO_EMU

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
        *((char*)0x0ED82D00) = 0x00; //
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
    
    printf("%s\n",filename);
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

/*-----------------
 * INITIAL PATCHES
 *----------------*/
void patch_binary()
{
    // Hook ioctl
    real_ioctl = dlsym(RTLD_NEXT, "ioctl");
    
    
    #ifdef LANDSCAPE
        UNPROTECT(0x0804FF00, 4096);
        *((char*)0x0804FF2A) = 0x00; //
    #endif

    #ifdef SOUND_FIX
        char* sndstr_loc;
        char* nsndstr = SOUND_FIX;
        sndstr_loc = 0x08776E23; //
        UNPROTECT(0x08776E23, 4096); //
        strcpy(sndstr_loc,"sysdefault");    
    #endif
            
    #ifdef IO_EMU
        // IO PCB Error Fix
        UNPROTECT((int)0x080A82A0, 4096); //
        *((char*)0x080A82A2) = 0x00; //
    #endif
        
    // Various Static Detours
    detour_function(&hasp_init_game, (int)0x080AC440); //
    detour_function(&hasp_read_dec_val, (int)0x080AC3A0); //
    detour_function(&use_hasp_val, (int)0x080AC2E0); //
    detour_function(&hasp_game_check, (int)0x080AC340); //
    
    //SAVE FIX
    UNPROTECT(0x086BB7AC,4096);
    strcpy(0x086BB7AC,"./rankdata/rank.dat");
    UNPROTECT(0x086BB68F,4096);
    strcpy(0x086BB68F,"./scoredata/");


    #ifdef NO_CAMERA
        UNPROTECT((int)0x080F61B0, 4096); //
        *((char*)0x080F61B0) = 0x75; //
        UNPROTECT((int)0x087E0AC6, 4096); //
        *((char*)0x087E0AC6) = 0x02;
    #endif
    
    #ifdef NO_ETH 
        UNPROTECT((int)0x080A1BC9, 4096); //
        *((char*)0x080A1BC9) = 0x00;        
    #endif
 
    #ifdef SAVE_NOSAFE_FIX
        UNPROTECT((int)0x080A8220, 4096); //
        *((char*)0x080A8220) = 0xC3;
    #endif
    
    #ifdef LOCAL_SAVE
        unsigned char* save_path = "./pt";
        UNPROTECT((int)0x080A5D4A, 4096);
        memcpy(0x080A5DB6,save_path,4); //
        memcpy(0x080A5D50,save_path,4); //
    #endif
    
}

// Entry Point.
void __attribute__((constructor)) initialize(void)
{
    printf("Starting up Wrapper...\n");
    patch_binary();
}

/*
    Tank!Tank!Tank! Arcade Patch by r4x
    2016 Team HackItUp
    
    compile: cc -shared -m32 dh_emu.c -w -ldl -D_GNU_SOURCE -o ./dh_emu.so
*/ 


//-- Dongle Serial Tag - MAX 8 Characters
#define DONGLE_SERIAL "267619508096"


#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include <stdio.h>
#include <dlfcn.h>

#include <string.h>
#include <sys/mman.h>
#include <time.h>
#define NOSYSTEM

/* Random bytestream for key */
unsigned char *gen_rdm_bytestream (int num_bytes)
{
  srand(time(NULL));
  unsigned char *stream = malloc (num_bytes);
  int i;

  for (i = 0; i < num_bytes; i++){
    stream[i] = rand();
  }
  return stream;
}



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
#define HASP_SUCCESS 0
int hasp_login(int feature_id,void* vendor_code, int handle){
    printf("hasp_login\n");
    return HASP_SUCCESS;
}

int hasp_read(int handle, int fileid,int offset, int length,unsigned char* buffer){
    printf("hasp_read\n");
    unsigned char * hasp_data = (unsigned char *) malloc(length);
    memcpy(buffer,gen_rdm_bytestream(length), length);
    strcpy(buffer,DONGLE_SERIAL);
    return HASP_SUCCESS;
}

int hasp_decrypt(int handle, unsigned char* buffer, int length){
    unsigned char * hasp_data = (unsigned char *) malloc(length);
    memcpy(buffer,gen_rdm_bytestream(length), length);
    strcpy(buffer,DONGLE_SERIAL);
    return HASP_SUCCESS;
}


static int global_pcbid = 1;
int get_pcbid(int obj){
 
    return global_pcbid;
}

int get_link_groupid(int obj, int something){
    return 1;
}


int system(unsigned char* cmd){
    printf("SYSTEM:: %s\n",cmd);
    return 0;

}

/*-----------------
 * HOOK FUNCTIONS
 *----------------*/
// ioctl Hook Stub
static int (*real_ioctl)(int fd, unsigned long int request, int *data) = NULL;
// open Hook Stub -- Needed for IO EMU to work for, some reason...
static int (*real_open)(char *, int) = NULL;

// ioctl Definitions
#define CMD64_VIDIOC_S_CROP 0x4014563c

unsigned int chandle = 0;
int ioctl(int fd, int request, unsigned char* data)
{
    int result;
    int i;
    switch(request){
        case CMD64_VIDIOC_S_CROP:
            if(getenv("DH_CAMERAFIX") == NULL){
                return real_ioctl(fd, request, data);
            }else{
                return 0;
            }        
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
   
    return real_open(filename, oflag);
}

int generic_retone(){
return 1;
}
int generic_retzero(){
return 0;
}

    
/*-----------------
 * INITIAL PATCHES
 *----------------*/
void patch_binary()
{
    // Shut system() up.
   
    
	if(getenv("DH_GFX_FIX") != NULL){
		UNPROTECT(0x080CCF12, 4096);
        *((char*)0x080CCF12) = 0x90;
		*((char*)0x080CCF13) = 0x90;
	}
	
    // Get PCBID from Environment Variable
    if(getenv("DH_PCB_ID") != NULL){
     global_pcbid = atoi(getenv("DH_PCB_ID"));
    }
    // Hook ioctl
    real_ioctl = dlsym(RTLD_NEXT, "ioctl");
        
    // Unsupported cameras with <= 96k buffer size will fail for whatever reason... let's fix this.
    if(getenv("DH_CAMERAFIX") != NULL){
        UNPROTECT(0x080A685B, 4096);
        *((int*)0x080A685B) = 0x00;
    }


    if(getenv("DH_SOUND_FIX") != NULL){
        char* sndstr_loc;
        char* nsndstr = "sysdefault";
        sndstr_loc = 0x0858E84F;
        UNPROTECT(0x0858E84F, 4096);
        strcpy(sndstr_loc,nsndstr);  
    }
    
    
    // Proper Quit with Fucked up Network Object
    //UNPROTECT(0x0845D9D0, 4096);
    //*((int*)0x0845D9D0) = 0xC3; 
    
    // Proper Quit with Fucked up Audio Object
    //UNPROTECT(0x08465C70, 4096);
    //*((int*)0x08465C70) = 0xC3; 
    
 
    // HASP Static Detours
    detour_function(&hasp_login, (int)0x084B9FD0);
    detour_function(&hasp_read, (int)0x084BADC8);
    detour_function(&hasp_decrypt, (int)0x084BA248);
    // Custom PCBID Redirection
    //detour_function(&generic_retone, (int)0x08452890);
    //detour_function(&get_link_groupid, (int)0x08458500);
    
}

// Entry Point.
void __attribute__((constructor)) initialize(void)
{
    printf("Starting up Wrapper...\n");
    patch_binary();
}



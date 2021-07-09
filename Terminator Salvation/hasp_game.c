#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#ifndef _LARGEFILE64_SOURCE
#define _LARGEFILE64_SOURCE
#endif
// cc -shared ./hasp_game.c -w -m32 -ldl -D_GNU_SOURCE -o ./hasp_game.so

#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
#include <GL/freeglut.h>
#include <stdarg.h>
#define UNPROTECT(addr,len) (mprotect((void*)(addr-(addr%len)),len,PROT_READ|PROT_WRITE|PROT_EXEC))

#define GFX_FRAMERATE 60
FGAPI int FGAPIENTRY glutEnterGameMode(void) {
    rfx_printf("Drawing window instead of entering game mode...\n");
    glutCreateWindow("Terminator Salvation");
    // If the user specified an environment variable, we go fullscreen.
    // The game does have an option in prefs.txt, but it no longer works.
    if(getenv("TS_FULLSCREEN"))
        glutFullScreen();
    // Tells glut to set our rendering framerate - VSYNC Method 1.
    setFPS(GFX_FRAMERATE);
    return 1;
}

// Rendering timer callback for VSYNC - Method 1.
static unsigned int redisplay_interval;
void timer() {
    glutPostRedisplay();
    glutTimerFunc(redisplay_interval, timer, 0);
}
void setFPS(unsigned int fps) {
    redisplay_interval = 1000 / fps;
    glutTimerFunc(redisplay_interval, timer, 0);
}

void DumpHex(const void* data, size_t size) {
	char ascii[17];
	size_t i, j;
	ascii[16] = '\0';
	for (i = 0; i < size; ++i) {
		printf("%02X ", ((unsigned char*)data)[i]);
		if (((unsigned char*)data)[i] >= ' ' && ((unsigned char*)data)[i] <= '~') {
			ascii[i % 16] = ((unsigned char*)data)[i];
		} else {
			ascii[i % 16] = '.';
		}
		if ((i+1) % 8 == 0 || i+1 == size) {
			printf(" ");
			if ((i+1) % 16 == 0) {
				printf("|  %s \n", ascii);
			} else if (i+1 == size) {
				ascii[(i+1) % 16] = '\0';
				if ((i+1) % 16 <= 8) {
					printf(" ");
				}
				for (j = (i+1) % 16; j < 16; ++j) {
					printf("   ");
				}
				printf("|  %s \n", ascii);
			}
		}
	}
}

/* 15 */
typedef struct _DongleHandle
{
  int type;
  int opened;
  void *data;
  void *initData;
  int api_err_code;
} DongleHandle;

/* 16 */
//typedef _DongleHandle DongleHandle;

//083AF750
// 83BBD60 <-- Hasp Encrypt
// 81C5E70 < -- Game Hasp Encrypt
static int (*hasp_encrypt)(size_t, void *, size_t) = (void*)(0x083BBD60); //1.25
static int (*hasp_decrypt)(size_t, void *, size_t) = (void*)(0x083BBCE0); //1.25
static int (*hasp_read)(size_t, int, size_t, size_t, void *) = (void*)(0x083BAD30); //1.25
/*
static int (*hasp_encrypt)(size_t, void *, size_t) = (void*)(0x083AF750);
static int (*hasp_decrypt)(size_t, void *, size_t) = (void*)(0x083AF6D0);
static int (*hasp_read)(size_t, int, size_t, size_t, void *) = (void*)(0x083AE720);
*/
static int done = 0;
//*hasp_encrypt = (void*)0x083AF750;


void detour_function(void * new_adr, int addr)
{
    int call = (int)(new_adr - addr - 5);
    UNPROTECT(addr, 4096);
    *((unsigned char*)(addr)) = 0xE9;
    *((int*)(addr + 1)) = call;
}

void detour_call(void * new_adr, int addr)
{
    int call = (int)(new_adr - addr - 5);
    UNPROTECT(addr, 4096);
    *((unsigned char*)(addr)) = 0xE8;
    *((int*)(addr + 1)) = call;
}

char * gethash(unsigned char * data, size_t size) {
    char * buffer = malloc(33);
    strcpy(buffer, "nohash");
    if (size >= 32) {
        for (int i = 0; i < 16; i++)  {
            sprintf(&buffer[i*2], "%02x", data[i]);
        }
    }
    return buffer;
}

void writefile(char * filename, char * data, size_t size) {
    FILE *fp;
    char str[128];
    strcpy(str, "/g3/hasp/");
    strcat(str, filename);
    fp = fopen(str,"w");
    if (fp != NULL) {
        fwrite(data, 1, size, fp);
        fclose(fp);
    }
    else {
        printf("Cant open file %s!\n", str);
    }
}

void readfile(char * filename, char * data, size_t size, int offset) {
    FILE *fp;
    char str[128];
    strcpy(str, "/g3/hasp/");
    strcat(str, filename);
    fp = fopen(str,"r");
    if (fp != NULL) {
        fseek(fp, offset, SEEK_SET);
        fread(data, 1, size, fp);
        fclose(fp);
    }
    else {
        printf("Cant open file %s!\n", str);
    }
}

int FileExist(char * filename) {
    char str[128];
    strcpy(str, "/g3/hasp/");
    strcat(str, filename);
    return access(str, F_OK) != -1;
}

FILE *fopen(const char *path, const char *mode) {
   // printf("In our own fopen, opening %s\n", path);

    FILE *(*original_fopen)(const char*, const char*);
    original_fopen = dlsym(RTLD_NEXT, "fopen");
    return (*original_fopen)(path, mode);
}

int DongleEncryptDecrypt(DongleHandle * handle, char * inoutdata, size_t size, int decrypt) {
    char * filename = gethash(inoutdata, size);
    //printf("Hash: %s\n", filename);
    //if (decrypt)
    //    printf("%s, size=%d\n", "DongleDecrypt!", size);
    //else
    //    printf("%s, size=%d\n", "DongleEncrypt!", size);
    //DumpHex(inoutdata, size);
    
    int ret;
    size_t t;
    t = *((size_t*) handle->data + 1);

    if (FileExist(filename)) {
        //printf("Found HASP data for %s, using it\n", filename);
        readfile(filename, inoutdata, size, 0);
        //return 0;
    }
    else {
        if (decrypt)
            ret = hasp_decrypt(t, inoutdata, size);
        else
            ret = hasp_encrypt(t, inoutdata, size);

        handle->api_err_code = ret;
        if (!ret) {
            writefile(filename, inoutdata, size);
        }
        else {
            printf("Dongle error! Wont save data!\n");
        }
    }
    
    //printf("After hasp_endecrypt:\n");
    //DumpHex(inoutdata, size);
    
    free(filename);
    
    return 0;
}

int DongleEncrypt(DongleHandle * handle, char * inoutdata, size_t size) {
    return DongleEncryptDecrypt(handle, inoutdata, size, 0);
}

int DongleDecrypt(DongleHandle * handle, char * inoutdata, size_t size) {
    return DongleEncryptDecrypt(handle, inoutdata, size, 1);
}

int hasp_read_wrapper(size_t handle, int fileid, size_t offset, size_t length, void * buffer) {
    if (fileid == 0xfff0) {
        // HASP_FILEID_MAIN
        //printf("Reading HASP_FILEID_MAIN, %d bytes from %d offset\n", length, offset);
        readfile("hhl_mem.dmp", buffer, length, offset);
        //DumpHex(buffer, length);
    }
    else if (fileid == 0xfff2) {
        // HASP_FILEID_LICENSE
       // printf("Reading HASP_FILEID_LICENSE, %d bytes from %d offset\n", length, offset);
        readfile("hhl_mem.dmp", buffer, length, offset+56);
        //DumpHex(buffer, length);
    }
    else {
        printf("Unknown hasp_read fileid %d, %d bytes from %d offset\n", fileid, length, offset);
    }
    return 0;
}

int hasp_login_wrapper(int feature, void * vendor_code, size_t handle) {
    if (feature == 0xFFFF0000) {
       // printf("Got hasp_login feature 0xFFFF, logging in\n");
        return 0;
    }
    else {
        //printf("Got hasp_login feature %d, DENYING\n", feature);
        return 1;
    }
}

int hasp_logout_wrapper(size_t handle) {
    //printf("hasp_logout, logging out\n");
    return 0;
}

int hasp_get_sessioninfo_wrapper(size_t handle, const char * format, char ** info) {
    //printf("hasp_get_sessioninfo\n");
    char * buffer = malloc(32);
    strcpy(buffer, "<haspid>1391295869</haspid>");
    *info = buffer;
    //52ED7D7D
    return 0;
}
// Logging to the screen.
void trm_msg(char *format, ...){
    va_list va;
    va_start(va, format);
    char msg[128] = {0x00};
    vsprintf(msg,format,va);
    printf("%s\n",msg);
}

void trm_msg2(int wat, char *format, ...){
    va_list va;
    va_start(va, format);
    char msg[128] = {0x00};
    vsprintf(msg,format,va);
    printf("%s\n",msg);
}

// Used to filter debugging printfs
void rfx_printf(char* format,...){
    if(1){
        va_list va;
        va_start(va, format);
        char msg[4096] = {0x00};
        vsprintf(msg,format,va);
        printf("%s\n",msg);
    }
}

int generic_retzero(){
    return 0;
}
void __attribute__((constructor)) initialize(void) {
    if (done == 0) {
	detour_function(&trm_msg,0x081DA734); // 1.25
    detour_function(&generic_retzero,0x082F2EDA); //1.25
    detour_function(&trm_msg2,0x081F1534); //1.25
        detour_function(&DongleEncrypt, 0x081C5E70); // 1.25
        detour_function(&DongleDecrypt, 0x081C5E00); // 1.25
        detour_function(&hasp_read_wrapper, 0x083BAD30); // 1.25
        detour_function(&hasp_login_wrapper, 0x083BBE60); // 1.25
        detour_function(&hasp_logout_wrapper, 0x083BBE00); // 1.25
        detour_function(&hasp_get_sessioninfo_wrapper, 0x083BAE80); // 1.25
	/*
	detour_function(&trm_msg,0x081D5AC4);
    detour_function(&generic_retzero,0x082edc5a);
    detour_function(&trm_msg2,0x081EC7E4);
        detour_function(&DongleEncrypt, 0x081C1670);
        detour_function(&DongleDecrypt, 0x081C1600);
        detour_function(&hasp_read_wrapper, 0x083AE720);
        detour_function(&hasp_login_wrapper, 0x083AF850);
        detour_function(&hasp_logout_wrapper, 0x083AF7F0);
        detour_function(&hasp_get_sessioninfo_wrapper, 0x083AE870);
        
        detour_call(&hasp_read_wrapper2, 0x081C2115);
        detour_call(&hasp_read_wrapper2, 0x081C1E1E);
        detour_call(&hasp_read_wrapper2, 0x081C1EF7);
        detour_call(&hasp_read_wrapper2, 0x081C1FDE);*/
        
        done = 1;
    }
}

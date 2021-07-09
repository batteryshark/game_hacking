/*
	Star Wars Voyager Game Patch for Version e.4
	compile: cc -shared -m32 -w stv_emu.c -w -ldl -lpthread -D_GNU_SOURCE -o ./stv_emu.so
*/
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
#include <linux/input.h>
#include <pthread.h>

/*------------------------------
 * MEMORY MODIFICATON FUNCTIONS
 *----------------------------*/
// Memory Modification Macro
#define UNPROTECT(addr,len) (mprotect((void*)(addr-(addr%len)),len,PROT_READ|PROT_WRITE|PROT_EXEC))
#define PROTECT(addr,len) (mprotect((void*)(addr-(addr%len)),len,PROT_READ|PROT_EXEC))
void detour_function(void * new_adr, int addr)
{
    int call = (int)(new_adr - addr - 5);
    UNPROTECT(addr, 4096);
    *((unsigned char*)(addr)) = 0xE9;
    *((int*)(addr + 1)) = call;
}


/*
	I/O EMULATION SECTION
*/
static struct input_event ev;
static pthread_t hthread;
#define MJF_HANDLE 0x9999
// some of the bits in 'miscgundata'
#define MISCGUNDATA_GUNA_SHOT     0x1000
#define MISCGUNDATA_GUNB_SHOT     0x2000
#define MISCGUNDATA_FLASHING      0x4000
#define MISCGUNDATA_PULSES_MASKA  0x7
#define MISCGUNDATA_PULSE_SHIFTA  0
#define MISCGUNDATA_PULSES_MASKB  0x70
#define MISCGUNDATA_PULSE_SHIFTB  4

// these are the bits in 'switches';  also the switch id
#define SW_P2SP2          1
#define SW_P1SP2          2
#define SW_P2SP1          4
#define SW_P1SP1          8
#define SW_P2B3        0x10
#define SW_P1B3        0x20
#define SW_P2B2        0x40
#define SW_P1B2        0x80
#define SW_P2B1       0x100
#define SW_P1B1       0x200
#define SW_P2R        0x400
#define SW_P1R        0x800
#define SW_P2L       0x1000
#define SW_P1L       0x2000
#define SW_P2D       0x4000
#define SW_P1D       0x8000
#define SW_P2U      0x10000
#define SW_P1U      0x20000
#define SW_P2START  0x40000
#define SW_P1START  0x80000
#define SW_P2COIN  0x100000
#define SW_P1COIN  0x200000
#define SW_TILT    0x400000
#define SW_TEST    0x800000
#define SW_SERV   0x1000000
#define SW_TKTSNS 0x2000000
#define SW_TRIG1  0x4000000
#define SW_TRIG2  0x8000000

#define SW_DIP1  0x10000000
#define SW_DIP2  0x20000000
#define SW_DIP3  0x40000000
#define SW_DIP4  0x80000000


typedef struct  {
	// this is the state of the read process
	int mode;
	// this stuff is stuffed by the read process
	int famcode;
	unsigned char sn[6];
	int crc;
	unsigned char skd[8];		// the "public data"
	unsigned char pd[48];		// the "private data"
	// this stuff is passed by the command to read
	int key;
	unsigned char pw[8];
	int keyreadid;		// this counts up with each new read
}  keydata;


// this is the struct read from '/dev/mj' when data is ready
static struct mjd{
	int miscgundata;
	int switches;
	int scratchreg;
	int irqcnt;
	int guntp[8];		// this is the 'raw' data from the device
	int cc[2];			// the running coin counters
	int version;		// the version of the driver 
	int recordnum;	  	
	int vc_at_start;
	int vc_at_finish;
	keydata Kd;        // this is the ibutton stuff
	int error;					
}mjdata;

// There are a ton of other buttons as defined above - that's up to you to do those.
static struct iostate {
unsigned char service;
unsigned char p1_coin;
unsigned char p1_start;
unsigned char dip_1;
unsigned char dip_2;
unsigned char dip_3;
unsigned char dip_4;
unsigned char test;
unsigned char p2_coin;
unsigned char p2_start;

}iost;

void update_io_state(){
	
	int niostate = 0;
	(iost.p1_coin) ? (niostate |= SW_P1COIN) : (niostate &= ~SW_P1COIN);
	(iost.p1_start) ? (niostate |= SW_P1START) : (niostate &= ~SW_P1START);
	(iost.p2_coin) ? (niostate |= SW_P2COIN) : (niostate &= ~SW_P2COIN);
	(iost.p2_start) ? (niostate |= SW_P2START) : (niostate &= ~SW_P2START);
	(iost.service) ? (niostate |= SW_SERV) : (niostate &= ~SW_SERV);
	(iost.test) ?  (niostate |= SW_TEST) : (niostate &= ~SW_TEST);
	(iost.dip_1) ?  (niostate |= SW_DIP1) : (niostate &= ~SW_DIP1);
	(iost.dip_2) ?  (niostate |= SW_DIP2) : (niostate &= ~SW_DIP2);
	(iost.dip_3) ?  (niostate |= SW_DIP3) : (niostate &= ~SW_DIP3);
	(iost.dip_4) ?  (niostate |= SW_DIP4) : (niostate &= ~SW_DIP4);
	if(iost.p1_coin){
		mjdata.cc[0]++; // This is a hack - normally, the game would send this back to the IO, but... fuck it.
	}
	mjdata.switches = niostate;
	
	
	
	
}

// read() wrapper for IO to work properly.
static int last_response; // The game pulses between returning 0 and the IO struct from the jamma.
static unsigned int (*real_read)(int fd, void *buf, size_t count) = NULL;
unsigned int read(int fd, unsigned char *buf, unsigned int count){
    unsigned int retval;
	
    if (real_read == NULL){
      real_read = dlsym(RTLD_NEXT, "read");
    }
 	if(fd == MJF_HANDLE){
		if(count == 168){
			mjdata.irqcnt++;
			mjdata.error = 0xFFFFFF9C; // The game expects this... don't know why.
			if(last_response){
				last_response = 0;
			}else{
				memcpy(buf,&mjdata,168);
				last_response = count;
			}
			return last_response;
		}	
	}
    return real_read(fd,buf,count);
}

// Although I could have just wrapped the two functions that call out to the I/O - I'm lazy.
static int (*real_open)(char *, int) = NULL;
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
	if(strstr(filename,"/dev/mjf") != NULL){
		return MJF_HANDLE;
	}
    return real_open(filename, oflag);
}

// Our emulated input thread
static void *input_thread(void *arg)
{
   
 int res;
 int fd = open("/run/kbdhook", 0);
    if (fd == -1) {
        perror("Opening input");
        exit(1);
    }else{
     //printf("IO EMULATOR STARTED!\n");   
    }
 while(1){

    res = read(fd, &ev, sizeof(ev));
     if (ev.type == EV_KEY) {
         switch(ev.code){
            // System Controls
            case KEY_ESC:
                exit(0);
				break;
			case KEY_F1:
				iost.test = (ev.value) ? 1:0;
				break;
            case KEY_F2:
               iost.service = (ev.value) ? 1:0;
               break;
			case KEY_F5:
				iost.dip_1 =  (ev.value) ? 1:0;
				break;
			case KEY_F6:
				iost.dip_2 =  (ev.value) ? 1:0;
				break;
			case KEY_F7:
				iost.dip_3 =  (ev.value) ? 1:0;
				break;
			case KEY_F8:
				iost.dip_4 =  (ev.value) ? 1:0;
				break;
            // Player 1 Controls
            case KEY_1:
               iost.p1_start = (ev.value) ? 1:0;
               break;
            case KEY_5:
               iost.p1_coin = (ev.value) ? 1:0;
               break;
			// Player 2 Controls - Player 2 cannot really play due to mouse being only for p1.
			case KEY_2:
				iost.p2_start = (ev.value) ? 1:0;
				break;
			case KEY_6:
				iost.p2_coin = (ev.value) ? 1:0;
				break;
            default:
               break;
			
			
         }
     }
	 update_io_state();
 }
}



// Only to force windowed mode.
int fullscreen(){
	*(int*)0x089B7AE0 = 0;
	return 0;
}

// Yay always true
int generic_retone(){
	return 1;
}
// Yay always not 
int generic_retzero(){
	return 0;
}
/*
	The game uses a super old janky-ass way to figure out vsync... it hasn't worked in like a decade.
	A bit of a hack - make the cooldown delay higher if you feel like the game runs too fast.
	The default value I found pretty reasonable.
*/
static unsigned int frame_counter;
static unsigned int frame_cooldown_delay = 3000000;
static unsigned int frame_cooldown;
unsigned int get_frame_number(){
	if(frame_cooldown < frame_cooldown_delay){
		frame_cooldown++;
	}else{
	frame_counter++;
	frame_cooldown = 0;	
	}
	
	
	return frame_counter;
}

/*
	The game normally calls out to a fifo of another process to do its sound thing.
	Not sure why it doesn't work on modern linux, maybe the way they do it no longer works.
	This works though: starting the sound daemon with a named pipe strapped to it and pushing
	the audio command whenever sound has to play.
*/
int sound_play(unsigned int sound_code){
	char sc_cmd_string[64];
	sprintf(sc_cmd_string,"echo %d > /tmp/stv_snd",sound_code);
	system(sc_cmd_string);	
	return 0;
}

// Some static patches on init.
void patch_binary()
{
	// Patching relative Directories
	UNPROTECT(0x080AC0E9,4096);
	strcpy(0x080AC0E9,"padsp ./stvsnd ./arom/");
	UNPROTECT(0x080A59E2,4096);
	strcpy(0x080A59E2,"./anims/");
	strcpy(0x080A59F3,"./models/");
	strcpy(0x080A5A05,"./textures/");
	strcpy(0x080A5A19,"./behaviors/");
	strcpy(0x080A5A2E,"./segments/");
	UNPROTECT(0x080A9CE0,4096);
	strcpy(0x080A9CE0,"./stv.settings");
	strcpy(0x080A9CF6,"./stv.hiscores");
	strcpy(0x080A9D0C,"./stv.audits");
	UNPROTECT(0x080A97CC,4096);
	strcpy(0x080A97CC,"./stv.log");
	
	// Patch out IButton IOCTL Check
	UNPROTECT(0x08098725,4096);
	memset(0x08098725,0x90,0x9E);

	// Patch out internal keyboard commands - they just get in the way...
	UNPROTECT(0x080986B6,4096);
	memset(0x080986B6,0x90,0x6);

	// Function Detours
	if(getenv("STV_FULL") == NULL){
		detour_function(&fullscreen,0x08093D00); // The game defaults to Fullscreen, but that's dumb.
	}

	detour_function(&generic_retone,0x08056530); // Patch out the iButton Check - returns true.
	detour_function(&generic_retone,0x08097B20); // Number of lines the IO can register... this might break fullscreen.
	detour_function(&get_frame_number,0x08094268); // VBLANK
	detour_function(&generic_retzero,0x08099554); // Read Keyboard Raw
	detour_function(&sound_play,0x0809D290); // Push the sound ourselves because the game is dumb.
}




// Entry Point.
void __attribute__((constructor)) initialize(void)
{
	char* buffer[1024] = {0x00};
	unsigned int buf_sz = readlink("/proc/self/exe", buffer, sizeof buffer - 1);
	if(strstr(buffer,"/bin/dash") == NULL){ // The audio likes to fork itself... it can fork off.	
		last_response = 0;
		pthread_create(&hthread, 0, input_thread, 0);
		patch_binary();
	}
}

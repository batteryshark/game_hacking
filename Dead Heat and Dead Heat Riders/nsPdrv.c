// Namco Sound Pulse Driver - Because forcing alsa is a fuckin joke
#include <stdio.h>
#include <pulse/simple.h>
#include <pulse/error.h>
// cc -shared -m32 ./nsPdrv.c -lpulse-simple -w -o ./nsAdrv.dll


extern int nsAdrv_write(void *bufptr, int frames);
extern int nsAdrv_term(const char* card_id);
extern int nsAdrv_wait(int waitms);
extern int nsAdrv_mixsts(int channel);
extern int nsAdrv_mixup(int* hwmp);
extern int nsAdrv_start(int something, int something2);
extern int nsAdrv_init(const char *card_id, const char *device, unsigned int channels, unsigned int bits, unsigned int rate, unsigned int period_size, unsigned int *hwmp);

/* The Sample format to use */
    static const pa_sample_spec ss = {
        .format = PA_SAMPLE_S32LE,
        .rate = 48000,
        .channels = 6
    };
    static pa_simple *s = NULL;
    static int ret = 1;
    static int error;
int nsAdrv_write(void *bufptr, int frames){
//printf("WRITE %d Frames\n",frames);
/*
Why *24? 
each frame is 24 bytes:
6 channels, 32 bit audio / 4 bytes... simple math
*/
pa_simple_write(s, bufptr, frames*24, &error);
return 0;
}

int nsAdrv_term(const char* card_id){
return 0;
}

int nsAdrv_wait(int waitms){
//printf("nsPdrv wait:\n");
// runs snd_pcm_wait(waitms)// 0 means timeout, 1 means ready
// snd pcm avail update -- returns number of frames ready to be read (256)
return 256;
}

int nsAdrv_mixsts(int channel){
//printf("nsPdrv mixsts:\n");
return 0;//hwm_elems[channel] != 0; // not sure
}

int nsAdrv_mixup(int* hwmp){
//printf("nsPdrv mixup:\n");
return 0;
}

int nsAdrv_start(int something, int something2){
// something1 is normally hwmp[29], something 2 is 256
// basically it calls snd_pcm_state(handle) and  snd_pcm_start(handle) and checks for errors, returns 0
return 0;
}

static unsigned int hwm_elems[16];
int nsAdrv_init(const char *card_id, const char *device, unsigned int channels, unsigned int bits, unsigned int rate, unsigned int period_size, unsigned int *hwmp){
printf("nsPdrv Init:\n");
printf("Card: %s Device: %s Channels: %d Bits: %d Rate: %d PeriodSize: %d\n",card_id,device,channels,bits,rate,period_size);
s = pa_simple_new(NULL, "nsPdrv", PA_STREAM_PLAYBACK, NULL, "playback", &ss, NULL, NULL, &error);
if(s == NULL){
printf("Error - Creating Simple New Failed: %s\n", pa_strerror(error));
exit(1);
}
int i;
//for(i = 0; i <= 64; i+=4){
//hwm_elems[i/4] = 0;
//memset(hwmp+i,0x00,0x16);
//}

return 0;
}

// Entry Point.
void __attribute__((constructor)) initialize(void)
{
    printf("Starting up nsPdrv...\n");
}


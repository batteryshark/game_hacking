//
// Test IO Crap - Not Used anymore.
//
#include "io.h"
#include "global.h"
#include <pthread.h>
#include <linux/input.h>
#include <GL/freeglut.h>

/*
  I/O MEMORY CONSTANTS
*/
#define TRIGGER_RIGHT  0x081DC1A0
#define RELOAD_RIGHT   0x081DC1C0
#define TRIGGER_LEFT   0x081DC188
#define RELOAD_LEFT    0x081DC1C1
// ??
#define BILL_VALIDATOR 0x081DC1BF
#define SWITCH_SERVICE 0x081DC1BE
#define SWITCH_TEST    0x081DC1BD
#define COIN_RIGHT     0x081DC1BC
#define COIN_LEFT      0x081DC1BB
#define START_RIGHT    0x081DC1B9
#define START_LEFT     0x081DC1BA
#define VOLUME_DOWN    0x081DC1B8
#define VOLUME_UP      0x081DC1B7
#define GUN_X_LEFT     0x081DC180
#define GUN_X_RIGHT    0x081DC182




unsigned short scale_value_uint16(unsigned short old_min,unsigned short  old_max,unsigned short  new_min,unsigned short  new_max,unsigned short value){
    unsigned short old_range = old_max - old_min;
    unsigned short new_range = new_max - new_min;
    return (((value - old_min) * new_range) / old_range) + new_min;
}


static unsigned int down = NULL;
void update_gun(int button, int state, int x, int y){
    rfx_printf("Mouse X/Y %d:%d\n",x,y);
    //long double gx = *(long double*)0x081E1684;
    //long double gy = *(long double*)0x081E1688;
    //*(unsigned int*)0x081E1660 = 0x7700E6;
    //*(unsigned int*)0x081E1664 = 0x7700E6;
    //*(unsigned int*)0x081E1668 = 0x7700E6;
    //*(unsigned int*)0x081E166C = 0x7700E6;
    //*(unsigned int*)0x081E1680 = 0xE6;
    //*(unsigned int*)0x081E1684 = 0x77;
    *(unsigned short*)0x081DC180 = scale_value_uint16(0,glutGet(GLUT_WINDOW_WIDTH),0,640,x);
    *(unsigned short*)0x081DC182 = scale_value_uint16(0,glutGet(GLUT_WINDOW_HEIGHT),0,480,y);
    *(unsigned short*)0x81DC19C = scale_value_uint16(0,glutGet(GLUT_WINDOW_WIDTH),0,640,x);
    *(unsigned short*)0x81DC19E = scale_value_uint16(0,glutGet(GLUT_WINDOW_HEIGHT),0,480,y);
    // Direct Calls
    typedef int func2(int player_num);
    func2* Player_ReloadWeapon = (func2*)0x08094E20;



    //gx = 0.5;//x;
    //gy = 0.5;//y;
    printf("Button %d\n",button);
    if(button == 0){
        if(state){
            //*(unsigned char*)0x081DC188 = 0;
            // *(unsigned char*)0x081DC195 = 1;
            *(unsigned char*)0x081DC196 = 1;

            //*(unsigned char*)0x81DC1AE = 1;

            *(unsigned char*)0x81DC1A0 = 0; // This one makes the flash happen...
            if(*(unsigned char*)0x81DC18D == 2){
                *(unsigned char*)0x81DC18D = 3;
            }
            //*(unsigned char*)0x081DC1B9= 1;
        }else{
            //*(unsigned char*)0x081DC188 = 1;
            *(unsigned char*)0x081DC195 = 0;
            *(unsigned char*)0x081DC196 = 0;

            *(unsigned char*)0x81DC1A0 = 1;
            //*(unsigned char*)0x81DC1AE = 0;
            //*(unsigned char*)0x081DC1B9= 0;
        }


    }

    if(button == 1){
        printf("state:%d\n",state);
        if(!state){
            //*(unsigned char*)START_LEFT = 0;
            //*(unsigned char*)0x081DC1A4 += 1;
            //*(unsigned char*)0x81DC18C += 1;

            if(!down){
                printf("FIRE\n");
                rfx_uint8_set(0x081DC1B0,1);
                down = 1;
            }

        }else{
            //*(unsigned char*)START_LEFT = 1;

            rfx_uint8_set(0x081DC1B0,0);
            down = 0;

        }
    }

    if(button == 2){
        if(state){
            char* v1;
            v1 = (char *)0x081DDF00 + 1368 * 0;
            *(unsigned char*)0x081DC189 = 1;
            *(unsigned char*)0x81DC1B6 +=1;
            *(unsigned char*)0x81DC1B5 +=1;
            *(unsigned char*)0x81DC1C0 = 1;
            *(unsigned char*)0x81DC1C1 = 1;
            //Player_ReloadWeapon(1);
            Player_ReloadWeapon((int)(v1+52));


        }else{
            *(unsigned char*)0x081DC189 = 0;
            *(unsigned char*)0x81DC1C0 = 0;
            *(unsigned char*)0x81DC1C1 = 0;
        }

    }


}
#define TTG_ADDR_IO_BUFFER 0x081DC1B0
#define START_RIGHT_CTR 0x081DC184
#define START_LEFT_CTR 0x081DC18C
#define TEST_SW        0x081DC1BD
#define SERVICE_SW     0x081DC1BE
#define TEST_SW_CTR    0x081DC1B2
#define TEST_SW_G 0x08138620
#define SERVICE_SW_CTR 0x081DC1B3
static struct internal_io{
    unsigned char start_left;
    unsigned char start_right;
    unsigned char test_sw_last;
    unsigned char test_sw;
    unsigned char test_sw_down;
    unsigned char service_sw_down;
    unsigned char service_sw_last;
    unsigned char coin_0;
    unsigned char service_sw;
}iio;

#define KEY_UP 0
#define KEY_DOWN 1
static unsigned char coin1_up = 1;

static void *process_keyboard(void* arg){
    printf("WATDAFUCK\n");
    int keybrdfd = -1;
    struct input_event ev;
    keybrdfd = open("/run/kbdhook", O_RDONLY);
    if (keybrdfd == -1) {
        perror("Opening input");
        exit(1);
    }else{
        printf("IO EMULATOR STARTED!\n");
    }
    rfx_uint8_set(0x081DC1B0,0);
    while (1){
        // Do anything you need here to run all the time, IO will block after this.
        iio.test_sw_last = iio.test_sw;
        if(!read(keybrdfd, &ev, sizeof(ev))){
            exit(1);
        }
        usleep(3000);
        unsigned char nv;
        // NOTE - The game reads from the IO faster than the IO processes, this requires a bit of 'de-bouncing' on our
        // part.
        if (ev.type == EV_KEY) {
            if (ev.code == KEY_F2) {
                if (ev.value) {
                    iio.test_sw = KEY_DOWN;
                } else {
                    iio.test_sw = KEY_UP;
                }

            } else {
                iio.test_sw = KEY_UP;
            }
            if(ev.code == KEY_5){
                if(ev.value){
                    iio.coin_0 = 1;
                }else{
                    iio.coin_0 = 0;
                }

            }else{
                iio.coin_0 = 0;
            }

        }

    }
}

static void *process_io(void *arg){
    unsigned short gun_x;
    unsigned short last_gun_x;
    unsigned short gun_y;
    unsigned short last_gun_y;
    while(1){
        gun_x = rfx_uint16_get(0x081DC19C);
        gun_y = rfx_uint16_get(0x081DC19E);
        if(gun_x != last_gun_x || gun_y != last_gun_y){
            rfx_printf("GUN_X: %04X %04X\n",gun_x,gun_y);
            last_gun_x = gun_x;
            last_gun_y = gun_y;
        }

    }
}
//glutMouseFunc(update_gun); // add this back in later.
static void *process_io2(void *arg)
{





    while(1){
        // Clear IO Buffers
        rfx_uint8_set(0x081DC1A4,0);
        // Clear IO Buffers 2
        memset(TTG_ADDR_IO_BUFFER,0x00,0x10);

        //glutMouseFunc(update_gun);
        // Update the game's IO state based on external structures.
        //printf("TEST:%d LAST:%d\n",iio.test_sw,iio.test_sw_last);
        rfx_uint8_set(0x081DC1B0,iio.coin_0);
        if(iio.test_sw == KEY_DOWN && iio.test_sw_last == KEY_UP){

            rfx_uint32_set(0x08138620,1);
        }
        if(iio.test_sw == KEY_UP && iio.test_sw_last == KEY_DOWN){
            rfx_uint32_set(0x08138620,0);
        }


        sleep(1);
    }
}


// Our input loop entrypoint. The game normally starts one, but this function will block if it contains the loop itself.
static pthread_t input_thread;
static pthread_t keyboard_thread;
unsigned int started = NULL;
void start_io_loop(){
    if(started == NULL){
        pthread_create(&input_thread, 0, process_io, 0);
        //pthread_create(&keyboard_thread, 0, process_keyboard, 0);
        started = 1;
    }

}
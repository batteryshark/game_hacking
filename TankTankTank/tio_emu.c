// IO EMULATOR FOR TANKTANKTANK

// COMPILE: cc -shared -m32 ./tio_emu.c -l dl -o ./tio_emu.so

#include <pthread.h>
#include <linux/input.h>
#include <stdio.h>
#include <fcntl.h>


static const char device_path[] = "/run/kbdhook";
static struct iostate {
    float pedal_r_state;
    float pedal_l_state;
    float steering_state;
    char enter_button_state;
    char select_button_down_state;
    char select_button_up_state;
    char fire_button_l_state;
    char fire_button_r_state;
    char safety_button_state;
    char test_button_state;
    char coin_button_state;
    char service_button_state;
}iost;
static int fd = -1;
static pthread_t hthread;
#define bBUTTON_ADDR_1 0xEC09204
#define bBUTTON_ADDR_2 0xEC09205
#define bTRIGGER_ADDR 0xEC070D9
#define fSTEERING_ADDR 0xEC070E4
#define fPEDAL_ADDR_R 0xEC070EC
#define fPEDAL_ADDR_L 0xEC070F0
#define wCOIN_ADDR 0xEC09220
#define STEERING_MID 0.5
static void *input_thread(void *arg)
{
    // initialize our states.
    iost.pedal_l_state = 0.0;
    iost.pedal_r_state = 0.0;
    iost.steering_state = 0.5;
    iost.enter_button_state = 0;
    iost.select_button_up_state = 0;
    iost.select_button_down_state = 0;
    iost.fire_button_l_state = 0;
    iost.fire_button_r_state = 0;
    iost.safety_button_state = 0;
    iost.test_button_state = 0;
    iost.coin_button_state = 0;
    iost.service_button_state = 0;
    unsigned char nstate;
    struct input_event ev;

    
    fd = open(device_path, O_RDONLY);
    if (fd == -1) {
        perror("Opening input");
        exit(1);
    }else{
     printf("IO EMULATOR STARTED!\n");   
    }
    

    while (1) {
        
        int res = read(fd, &ev, sizeof(ev));
        
        if (ev.type == EV_KEY) {
            
           if(ev.code == KEY_F2)
               iost.test_button_state = (ev.value) ? 1:0;

           if(ev.code == KEY_F1)
               iost.service_button_state = (ev.value) ? 1:0;
           

           if(ev.code == KEY_1)
               iost.enter_button_state = (ev.value) ? 1:0;
        
           if(ev.code == KEY_5)
               iost.coin_button_state = (ev.value) ? 1:0;

           if(ev.code == KEY_UP)
            iost.select_button_up_state = (ev.value) ? 1:0;
          
           
           if(ev.code == KEY_DOWN)
            iost.select_button_down_state = (ev.value) ? 1:0;
       
           if(ev.code == KEY_Z)
               iost.fire_button_l_state = (ev.value) ? 1:0;

           if(ev.code == KEY_X)
               iost.fire_button_r_state = (ev.value) ? 1:0;
           
           if(ev.code == KEY_C)
               iost.safety_button_state = (ev.value) ? 1:0;
           
           if(ev.code == KEY_RIGHT){
                if(ev.value){
                    iost.steering_state = 1.0;
                }else{
                    iost.steering_state = STEERING_MID;
                }
           }

           if(ev.code == KEY_LEFT){
                if(ev.value){
                    iost.steering_state = 0;
                }else{
                    iost.steering_state = STEERING_MID;
                }
            }
            
           if(ev.code == KEY_LEFTSHIFT){
                if(ev.value){
                    iost.pedal_l_state = 1.0;
                }else{
                    iost.pedal_l_state = 0.0;
                }
            }
   
           if(ev.code == KEY_LEFTCTRL){
                if(ev.value){
                    iost.pedal_r_state = 1.0;
                }else{
                    iost.pedal_r_state = 0.0;
                }
            }
           
           // Button Group 1 
            nstate = 0;
           if(iost.enter_button_state)
               nstate |= 0x40;
           if(iost.select_button_up_state){
               nstate |= 1;
           }else if(iost.select_button_down_state){
               nstate |=8;
           }
           *((unsigned char*)bBUTTON_ADDR_1) = nstate;
           
           // Button Group 2
           nstate = 0;
           if(iost.test_button_state)
               nstate |= 2;
           if(iost.service_button_state)
               nstate |= 0x10;
           *((unsigned char*)bBUTTON_ADDR_2) = nstate;
           
           // Coin Group
           if(iost.coin_button_state)
               *((unsigned char*)wCOIN_ADDR)+=1;

            
      
           // Steering
           *((float*)fSTEERING_ADDR) = iost.steering_state;
         
           // Trigger L + R + Safety
           nstate = 0;
           if(iost.fire_button_l_state)
               nstate |= 0x10;
           if(iost.fire_button_r_state)
               nstate |= 0x8;
           if(iost.safety_button_state)
               nstate |= 0x20;
           *((unsigned char*)bTRIGGER_ADDR) = nstate;
        
           
           
           // PEDAL L
           *((float*)fPEDAL_ADDR_L) = iost.pedal_l_state;
           // PEDAL R
           *((float*)fPEDAL_ADDR_R) = iost.pedal_r_state;
           
        }
    }
}
    

void __attribute__((constructor)) initialize(void)
{
    pthread_create(&hthread, 0, input_thread, 0);
}
// IO EMULATOR FOR PercussionMaster

// COMPILE: cc -shared -w -m32 ./pio_emu.c -l dl -o ./pio_emu.so

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
#define bBUTTON_ADDR_1 0xED82D04 //
#define bBUTTON_ADDR_2 0xED82D05 //
#define bTRIGGER_ADDR 0xED80BD9 //
#define fSTEERING_ADDR 0xED80BE4 //
#define fPEDAL_ADDR_R 0xED80BEC //
#define fPEDAL_ADDR_L 0xED80BF0 //
#define wCOIN_ADDR 0xED82D20 //
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
     //printf("IO EMULATOR STARTED!\n");   
    }
    
    short* offset;
    offset = 0x0843C686;
    
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
          
           if(ev.code == KEY_ESC)
               exit(0);
           
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
                   // *((unsigned char*)0x083EA640+(11*4)) = 1;
                }else{
                   // *((unsigned char*)0x083EA640+(11*4)) = 0;
                }
           }

           if(ev.code == KEY_LEFT){
                if(ev.value){
                   // *((unsigned char*)0x083EA640+(8*4)) = 1;
                }else{
                   // *((unsigned char*)0x083EA640+(8*4)) = 0;
                }
            }
            
           if(ev.code == KEY_LEFTSHIFT){
                if(ev.value){
                   // *((unsigned char*)0x083EA640+(0*4)) = 1;
                }else{
                    //*((unsigned char*)0x083EA640+(0*4)) = 0;
                }
            }
   
           if(ev.code == KEY_LEFTCTRL){
                if(ev.value){
                   // *((unsigned char*)0x083EA640+(10*4)) = 1;
                }else{
                    //*((unsigned char*)0x083EA640+(10*4)) = 0;
                }
            }
           
           // Button Group 1 
            nstate = 0;
           if(iost.select_button_down_state)
               nstate= 1;
           //*((unsigned char*)0x083EA640+(6*4)) = nstate;
           
		   nstate = 0;
		   
		   // Test Button Logic
		   nstate = 0;
		   if(iost.test_button_state)
                    nstate= 1;
                   
		  *((unsigned char*)0x0843C620) = nstate;

		   // Coin Logic
		   nstate = 0;
		   if(iost.coin_button_state)
			   nstate = 1;
		   *((unsigned char*)0x0842FB08) = nstate;
		   *((unsigned char*)0x0842FB42) = nstate;
		   
		   nstate = 0;		   
            
           
               
                  
		   
		   unsigned int bstate = *((unsigned int*)0x0842FB1C);
                   if(iost.fire_button_r_state){
                        bstate |=(1 << 5);
                    }else{
                       bstate &=~(1 << 5);
                   }
                   if(iost.fire_button_l_state){
                        bstate |=(1 << 6);
                    }else{
                       bstate &=~(1 << 6);
                   }
                   if(iost.enter_button_state){
                       bstate |=(1 << 0);
                    }else{
                       bstate &=~(1 << 0);
                   }
                   if(iost.select_button_up_state){
                       bstate |=(1 << 2);
                    }else{
                       bstate &=~(1 << 2);
                   }
                   if(iost.select_button_down_state){
                       bstate |=(1 << 7);
                    }else{
                       bstate &=~(1 << 7);
                   }
                   if(iost.safety_button_state){
                      bstate |=(1 << 1);
                    }else{
                       bstate &=~(1 << 1);  
                   }
                   *((unsigned int*)0x0842FB1C) = bstate;
                   *((unsigned char*)0x0842FB24) = 5;
		   //*((unsigned char*)0x083EA640+(27*4)) = nstate;
        }
    }
}
 //0 1 27 11 12    

void __attribute__((constructor)) initialize(void)
{
    pthread_create(&hthread, 0, input_thread, 0);
}

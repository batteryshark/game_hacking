
#include "global.h"
#include "io.h"


#define COB_HW_OUTPUT_OFFSET 0x4125A0
#define COB_CHECK_OUTPUT 0x4125F0
#define EXPOTATO_IO_ENDPOINT "tcp://localhost:22101"
#define COB_GET_HW_INPUT_OFFSET 0x4126D0


int get_hardware_input(unsigned char input_code){
    unsigned char* response_buffer = (unsigned char*)malloc(TRANSACT_PACKET_MAX);
    int response_code;
    unsigned int response_size;
    unsigned char request[1];
    request[0] = input_code;
    rfx_zmq_transact(request,1,response_buffer,&response_size,EXPOTATO_IO_ENDPOINT);
    memcpy(&response_code,response_buffer,response_size);
    return response_code;
}

int check_hw_output(){
    // TODO: Other Logic
    return 15;
}

char set_hardware_output(int input_mode,unsigned char output_data){
    char result;
    int hw_result = check_hw_output();
    result = hw_result & 0xFF;
    return result; }

void patch_io(){
detour_jmp(&set_hardware_output,COB_HW_OUTPUT_OFFSET);
detour_jmp(&get_hardware_input,COB_GET_HW_INPUT_OFFSET);
detour_jmp(&check_hw_output,COB_CHECK_OUTPUT);

}

#include "GunIOBoard.h"
#include "GunIOUtils.h"

GunIOBoard::~GunIOBoard() {
    this->bpc = 0;
    this->dipsw_1_direct = false;
    this->dipsw_2_12_led = false;
    this->inputs.clear();
    this->firmware_revision = 0;
}

/*
    Firmware Version 0x01-0x31:  8 bits per coordinate, 2 player
    Firmware Version 0x32-0x45: 10 bits per coordinate, 2 player
    Firmware Version 0x46-0xFE: 10 bits per coordinate, 4 player
    [Examples]
    0x3C - Panic M / Haunted Museum
    0x3E - Elevator Action Death Parade
    0x45 - Haunted Museum 2
    0x46 - Gaia Attack 4
*/

GunIOBoard::GunIOBoard(uint8_t firmware_revision, uint16_t vscreen_width, uint16_t vscreen_height) {
    this->firmware_revision = firmware_revision;
    this->vscreen_height = vscreen_height;
    this->vscreen_width = vscreen_width;
    if(firmware_revision > 0 && firmware_revision <= 0x31){
        // Generation 1 - 8bit, 2player
        this->bpc = 8;
        this->enabled_players = 2;
    }else if(firmware_revision > 0x31 && firmware_revision <= 0x45){
        // Generation 2 - 10bit, 2player
        this->bpc = 10;
        this->enabled_players = 2;
    }else if(firmware_revision > 0x45){
        // Generation 3 - 10bit, 4player
        this->bpc = 10;
        this->enabled_players = 2;
    }

    for(int i = 0; i < this->enabled_players; i++){
        this->inputs.push_back(new GunIOInput());
    }

}


// Protocol Handlers

// Determine ROM Version Setting - 0x04
void GunIOBoard::Protocol_GetRomVersion(uint8_t *response_data, uint32_t* response_length) {
    *response_length = 1;
    response_data[0] = this->firmware_revision;
}

// Determine DipSwitch State - 0x10
void GunIOBoard::Protocol_GetDipSw(uint8_t *response_data, uint32_t *response_length) {
    *response_length = 1;
    response_data[0] = 0;
    if(this->dipsw_1_direct){response_data[0] |= 0x80;}
    if(this->dipsw_2_12_led){response_data[0] |= 0x40;}
}

// Set DipSwitch 1 - 0x14
void GunIOBoard::Protocol_SetDipsw_1(uint8_t* response_data,uint32_t* response_length){
    (this->dipsw_1_direct) ? false:true;
    *response_length = 1;
    response_data[0] = STATUS_WRITE_OK;
}
// Set DipSwitch 2 - 0x18
void GunIOBoard::Protocol_SetDipsw_2(uint8_t* response_data,uint32_t* response_length){
    (this->dipsw_2_12_led) ? false:true;
    *response_length = 1;
    response_data[0] = STATUS_WRITE_OK;
}

// Get Sensor State P1+P2 - 0x1C
// Get Sensor State P3+P4 - 0x1D
void GunIOBoard::Protocol_GetSensorState(uint8_t* response_data, uint32_t* response_length,uint8_t cmd){

    sensor_map state_0;
    sensor_map state_1;

    switch(cmd){
        case GUNIO_OPCODE_GET_LED_STATE_12:
            this->inputs[0]->ReadInput_1(&state_0);
            this->inputs[1]->ReadInput_1(&state_1);
            break;
        case GUNIO_OPCODE_GET_LED_STATE_34:
            this->inputs[2]->ReadInput_1(&state_0);
            this->inputs[3]->ReadInput_1(&state_1);
            break;
        default:
            // Unsupported CMD
            return;
    }

    *response_length = 6;
    uint8_t num_leads_to_read = (this->dipsw_2_12_led) ? 12:10;
    uint16_t* s0_offset = (uint16_t*)response_data;
    uint16_t* s1_offset = (uint16_t*)response_data+3;
    for(int i =0; i< num_leads_to_read; i++){

        if(state_0.led[i]){
            *(uint16_t*)s0_offset |= 1 << i;
        }
        if(state_1.led[i]){
            *(uint16_t*)s1_offset |= 1 << i;
        }
    }
}

bool GunIOBoard::ProcessRequest(uint8_t* request_data, uint32_t request_length, uint8_t* response_data, uint32_t* response_length) {
    if(!request_length){return false;}

    // Identify Request Packet Type
    // Construct Response and Return Status
    switch(request_data[0]){
        case GUNIO_OPCODE_ROM_VERSION:
            Protocol_GetRomVersion(response_data,response_length);
            break;
        case GUNIO_OPCODE_GET_DIPSW:
            Protocol_GetDipSw(response_data,response_length);
            break;
        case GUNIO_OPCODE_SET_DIPSW_1:
            Protocol_SetDipsw_1(response_data,response_length);
            break;
        case GUNIO_OPCODE_SET_DIPSW_2:
            Protocol_SetDipsw_2(response_data,response_length);
            break;
        case GUNIO_OPCODE_GET_LED_STATE_12:
            Protocol_GetSensorState(response_data,response_length,GUNIO_OPCODE_GET_LED_STATE_12);
            break;
        case GUNIO_OPCODE_GET_LED_STATE_34:
            Protocol_GetSensorState(response_data,response_length,GUNIO_OPCODE_GET_LED_STATE_34);
            break;
        default:
            // Unhandled Request Type
            return false;
    }
    // If we have exceeded the response buffer, error out because we still need the checksum.
    if(*response_length >= MAX_RESPONSE_SIZE){return false;}

    response_data[*response_length] = GunIOUtils::CalculateChecksum(response_data,*response_length);
    *response_length+=1;
    return true;
}



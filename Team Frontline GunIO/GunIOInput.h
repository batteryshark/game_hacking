#pragma once
#include <cstdint>
#include <cstring>

struct input_map{
    uint16_t x;
    uint16_t y;
    bool trigger;
    // Purely for Emulation - will force a FRAME-OUT
    bool reload;
};

// 16 Sensors Per Player
//  1 byte reserved Per Player
struct sensor_map{
    bool led[16];
    uint8_t reserved;
};


class GunIOInput {
private:
    input_map input;
    sensor_map sensors;
    void reset(){
        memset(&this->input,0x00,sizeof(input_map));
        memset(&this->sensors,0x00,sizeof(sensor_map));
    }
public:
    GunIOInput();
    ~GunIOInput(){this->reset();}
    bool ReadInput_0   (input_map* state)     {memcpy(state,&this->input,sizeof(input_map));}
    bool ReadInput_1   (sensor_map* state)    {memcpy(state,&this->sensors,sizeof(sensor_map));}
    bool WriteInput_0  (input_map* n_state)   {memcpy(&this->input,n_state,sizeof(input_map));}
    bool WriteInput_1  (sensor_map* n_state)  {memcpy(&this->input,n_state,sizeof(sensor_map));}
};

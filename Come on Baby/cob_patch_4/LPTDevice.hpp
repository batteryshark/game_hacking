#pragma once

enum lpt_mode{
    STANDARD,
    EPP,
    ECP
};

class LPTDevice {
private:
    unsigned char register_data;
    unsigned char register_status;
    unsigned char register_control;
public:
    unsigned short port_data;
    unsigned short port_status;
    unsigned short port_control;
    lpt_mode mode;
    unsigned char get_data_register(){return this->register_data;}
    unsigned char get_status_register(){return this->register_status;}
    unsigned char get_control_register(){return this->register_control;}
    void set_data_register(unsigned char value){this->register_data = value;}
    void set_status_register(unsigned char value){this->register_status = value;}
    void set_control_register(unsigned char value){this->register_control = value;}
    void set_mode(lpt_mode value){this->mode = value;}
    LPTDevice(unsigned short base_port, lpt_mode mode);
    ~LPTDevice();

};

LPTDevice::~LPTDevice(){
    this->port_control = 0x00;
    this->port_data = 0x00;
    this->port_status = 0x00;
    this->register_control = 0x00;
    this->register_data = 0x00;
    this->register_status = 0x00;
}

LPTDevice::LPTDevice(unsigned short base_port, lpt_mode mode){
    this->mode = mode;
    this->port_data = base_port;
    this->port_status = base_port + 1;
    this->port_control = base_port + 2;
    this->register_control = 0x00;
    this->register_data = 0x00;
    this->register_status = 0x00;
}

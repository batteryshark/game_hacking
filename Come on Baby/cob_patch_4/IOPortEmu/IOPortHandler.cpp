#include "IOPortHandler.h"
#include "../dbg_utils.h"
void IOPortHandler::register_handler(unsigned short port, io_function handler){
    auto* ph = new PortHandler();
    ph->port = port;
    ph->func_handler = handler;
    this->handlers.push_back(ph);
}

void IOPortHandler::deregister_handler(unsigned short port){
    for(int i = 0; i < this->handlers.size();i++){
        if(port == this->handlers[i]->port){
            this->handlers.erase(this->handlers.begin()+i);
        }
    }
}

void IOPortHandler::process(unsigned short port, unsigned int* pvalue, unsigned int value_mask){
    unsigned int value = 0;
    switch(value_mask){
        case 0xFF:
            value = *(unsigned char*)pvalue;
            break;
        case 0xFFFF:
            value = *(unsigned short*)pvalue;
            break;
        case 0xFFFFFFFF:
            value = *(unsigned int*)pvalue;
            break;
    }
    //DbgUtils__dbg_printf("IOPort Call [%#02X / %#04X] \n",port,value);
    for(auto & handler : this->handlers){
        if(port == handler->port){
            handler->func_handler(&value);
            *pvalue = value;
            return;
        }
    }
}

IOPortHandler::IOPortHandler() = default;

IOPortHandler::~IOPortHandler() {handlers.clear();}





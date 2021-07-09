#pragma once
#include <vector>

typedef void (*io_function)(unsigned int* value);

struct PortHandler{
    unsigned short port;
    io_function func_handler;
};

class IOPortHandler{
private:
    std::vector<PortHandler*> handlers;
public:
    void register_handler(unsigned short port, io_function handler);
    void deregister_handler(unsigned short port);
    void process(unsigned short port, unsigned int* pvalue, unsigned int value_mask);
    IOPortHandler();
    ~IOPortHandler();
};
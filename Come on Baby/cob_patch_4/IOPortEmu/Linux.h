#pragma once

#ifdef TARGET_PLATFORM_LINUX
#include <cstdio>
#include <cstdlib>
#include <cstring>

#include <signal.h>
#include <sys/io.h>
#include <sys/ucontext.h>

#include "./Base.h"

class IOPortEmuLinux : public Base {
    public:
    void pexception_handler(int signal, siginfo_t *si, void *arg);
    IOPortEmuLinux();
    ~IOPortEmuLinux();
};


#endif
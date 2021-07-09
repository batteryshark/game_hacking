
#ifdef TARGET_PLATFORM_LINUX

#ifdef TARGET_ARCH_64
    #define XIP_PTR ctx->uc_mcontext.gregs[REG_RIP]
    #define XAX ctx->uc_mcontext.gregs[REG_RAX]
    #define XDX ctx->uc_mcontext.gregs[REG_RDX]
#else
    #define XIP_PTR ctx->uc_mcontext.gregs[REG_EIP]
    #define XAX ctx->uc_mcontext.gregs[REG_EAX]
    #define XDX ctx->uc_mcontext.gregs[REG_EDX]
#endif  


IOPortEmuLinux::IOPortEmuLinux(){
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = &this->pexception_handler;
    sa.sa_flags   = SA_SIGINFO;
    sigaction(SIGSEGV, &sa, 0);    
}

IOPortEmuLinux::~IOPortEmuLinux(){
    // TODO: set to SIG_DFL
}

void IOPortEmuLinux::pexception_handler(int signal, siginfo_t *si, void *arg){
    ucontext_t *ctx = (ucontext_t *)arg;

    unsigned char bump_ip = this->handle_io(XIP_PTR, &XAX, XDX);
    if(!bump_ip){
        printf("Signal %d received\n", signal);
        exit(signal);
    }

    XIP_PTR += bump_ip;
}

#endif
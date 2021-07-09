#include <Windows.h>
#include "kitchen_sink.h"
#include "fs_redirect.h"

#include "io.hanoi.h"

#define EXPORTABLE __declspec(dllexport)
EXPORTABLE void dropkick(){}



BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
    if (fdwReason == DLL_PROCESS_ATTACH) {
        DBG_printfW(L"[HANOI_PATCH]: LOADING...");
        io_init();
        init_fs_redirect();
        DBG_printfW(L"[HANOI_PATCH]: LOADED!");
    }
    return TRUE;
}

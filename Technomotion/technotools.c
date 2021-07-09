#include <Windows.h>
#include "kitchen_sink/kitchen_sink.h"
#include "fs.redirect.h"
#include "fs.codepage.fix.h"
#include "io.technoio.h"
#include "gfx.h"

#define EXPORTABLE __declspec(dllexport)
EXPORTABLE void technotools(){}

static struct _OPTIONS{
    int cmos_rtc_emu;
    int input_emu;
    int lights_emu;
    int eax2_fix;
    int fs_redirect;
    int fs_codepage_fix;
    int windowed_mode;
}Options;



void load_config(){
    char ini_path[1024] = {0x00};
    if(!GetEnvironmentVariableA("CONFIG_INI_PATH", ini_path,sizeof(ini_path))){
        strcpy(ini_path,"D:\\Techmo\\technotools.ini");
    }
    Options.cmos_rtc_emu = GetPrivateProfileIntA("OPTIONS", "emulate_cmos", 0, ini_path);
    Options.input_emu = GetPrivateProfileIntA("OPTIONS", "emulate_io_input", 0, ini_path);
    Options.lights_emu = GetPrivateProfileIntA("OPTIONS", "emulate_io_lights", 0, ini_path);
    Options.eax2_fix = GetPrivateProfileIntA("OPTIONS", "eax2_fix", 0, ini_path);
    Options.fs_redirect = GetPrivateProfileIntA("OPTIONS", "filesystem_path_fix", 0, ini_path);
    Options.fs_codepage_fix = GetPrivateProfileIntA("OPTIONS", "filesystem_codepage_fix", 0, ini_path);
    Options.windowed_mode = GetPrivateProfileIntA("OPTIONS", "windowed_mode", 0, ini_path);
    DBG_printfA("Options: ");
    DBG_printfA("CMOS RTC EMULATOR: %s", Options.cmos_rtc_emu ? "ENABLED":"DISABLED");
    DBG_printfA("INPUT EMULATOR: %s", Options.input_emu ? "ENABLED":"DISABLED");
    DBG_printfA("LIGHTS EMULATOR: %s", Options.lights_emu ? "ENABLED":"DISABLED");
    DBG_printfA("EAX 2.0 FIX: %s", Options.eax2_fix ? "ENABLED":"DISABLED");
    DBG_printfA("FILESYSTEM REDIRECT: %s", Options.fs_redirect ? "ENABLED":"DISABLED");
    DBG_printfA("FILESYSTEM CP949 FIX: %s", Options.fs_codepage_fix ? "ENABLED":"DISABLED");

}

void eax2_fix(){
    HMODULE dsound_dll = GetModuleHandleA("dsound.dll");
    if(dsound_dll){FreeLibrary(dsound_dll);}
    LoadLibraryA("dsound.dll");
}

void init(){
    technoio_init(Options.cmos_rtc_emu,Options.input_emu,Options.lights_emu);
    if(Options.eax2_fix){eax2_fix();}
    if(Options.fs_redirect){init_fs_redirect();}
    if(Options.fs_codepage_fix){init_fs_codepage_fix();}
    if(Options.windowed_mode){init_gfx();}

}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved) {
if (fdwReason == DLL_PROCESS_ATTACH) {
    DBG_printfW(L"[TECHNOTOOLS]: LOADING...");
    load_config();
    init();
    DBG_printfW(L"[TECHNOTOOLS]: LOADED!");
}
return TRUE;
}

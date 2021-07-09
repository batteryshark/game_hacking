#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "global.h"
#include "gfx.h"
#include "io.h"
#include "rtfs.h"
#include "target.h"




// Address Defines - Functions
#define TTG_ADDR_FUNC_IS_DONGLE_PRESENT           0x080D8E90
#define TTG_ADDR_FUNC_MSG                         0x080DB710
#define TTG_ADDR_FUNC_SCRSHAKE_STOP               0x08067D20
#define TTG_ADDR_FUNC_SCRSHAKE_START              0x08067BE0
#define TTG_ADDR_FUNC_SCRSHAKE_UPDATE             0x08067C10
#define TTG_ADDR_FUNC_RTFS_LZWFOPEN               0x080DCD80
#define TTG_ADDR_FUNC_RTFS_TRANSLATEFILENAME      0x080DCB10
#define TTG_ADDR_FUNC_RTFS_OPEN                   0x080DCBC0
#define TTG_ADDR_FUNC_RTFS_FOPEN                  0x080DCCA0
#define TTG_ADDR_FUNC_IO_INITLOOP                 0x0806068A
#define TTG_ADDR_FUNC_IOINIT                      0x080C6BC0
#define TTG_ADDR_FUNC_IOCHECK                     0x080C6C90
#define TTG_ADDR_FUNC_IOREAD                      0x080C69F0
#define TTG_ADDR_FUNC_IOWRITE                     0x080C69A0

// Address Defines - Values
#define TTG_ADDR_UINT8_CROSSHAIR_VAL_1  0x080C9A7F
#define TTG_ADDR_UINT8_CROSSHAIR_VAL_2  0x080C9A8B
#define TTG_ADDR_CSTR_DISPLAY           0x08128406
#define TTG_ADDR_CSTR_SAVE01            0x08121430
#define TTG_ADDR_CSTR_SAVE02            0x0812143C
#define TTG_ADDR_CSTR_SAVE03            0x08121448
#define TTG_ADDR_CSTR_SAVE04            0x0811DAA0
#define TTG_ADDR_CSTR_SAVE05            0x0811D448
#define TTG_ADDR_CSTR_SAVE06            0x0811D454
#define TTG_ADDR_CSTR_SAVE07            0x08125138
#define TTG_ADDR_CSTR_SAVE08            0x0811ED3C
#define TTG_ADDR_CSTR_SAVE09            0x0811ED48
#define TTG_ADDR_CSTR_SAVE10            0x08129A87
#define TTG_ADDR_CSTR_SAVE11            0x08129A7B
#define TTG_ADDR_CSTR_SAVE12            0x0811DA94
#define TTG_ADDR_UINT8_SERIALTIMEOUT_1  0x080C8CFD
#define TTG_ADDR_UINT8_SERIALTIMEOUT_2  0x080C8D48

// Static Patches to the image on linkage.
void patch_binary(){

    // --- [I/O Patches] ---
    // Up the serial timeout rate for our external IO Emulator to give it a chance.
    rfx_patch_uint8(TTG_ADDR_UINT8_SERIALTIMEOUT_1,0xEB);
    rfx_patch_uint8(TTG_ADDR_UINT8_SERIALTIMEOUT_2,0x40);


/*
    // Opens the I/O handle from /dev/ttyS0, nope...
    rfx_patch_jmp(&rfx_generic_uint32_retone, TTG_ADDR_FUNC_IOINIT);
    // Reads Message from I/O handle, nope...
    rfx_patch_jmp(&rfx_generic_uint32_retone,TTG_ADDR_FUNC_IOREAD);
    // Conditional Checks for I/O Response, nope...
    rfx_patch_jmp(&rfx_generic_uint32_retone, TTG_ADDR_FUNC_IOCHECK);
    // Sends info to I/O through handle, nope...
    rfx_patch_jmp(&rfx_generic_uint32_retone,TTG_ADDR_FUNC_IOWRITE);
    // I/O Processing loop in its own thread, not today, champ!
    rfx_patch_push_addr(&start_io_loop,TTG_ADDR_FUNC_IO_INITLOOP);
*/
    // --- [Graphical Patches] ---

    //Sometimes GLUT fucks up and can't get your display settings for... reasons; this should fix it.
    if(getenv("DISPLAY") != NULL)
        rfx_patch_str(TTG_ADDR_CSTR_DISPLAY,getenv("DISPLAY"));

    // Screen Shake Resize Fuckup Mitigation Patch
    rfx_patch_jmp(&ScrShake_Start,TTG_ADDR_FUNC_SCRSHAKE_START);
    rfx_patch_jmp(&ScrShake_Stop,TTG_ADDR_FUNC_SCRSHAKE_STOP);
    rfx_patch_jmp(&ScrShake_Update,TTG_ADDR_FUNC_SCRSHAKE_UPDATE);

    // Crosshair Cursor Patch - for funsies :)
    rfx_patch_uint8(TTG_ADDR_UINT8_CROSSHAIR_VAL_1,0x09);
    rfx_patch_uint8(TTG_ADDR_UINT8_CROSSHAIR_VAL_2,0x09);

    // VSYNC Method 2 (disabled for now)
    //rfx_patch_jmp(&rtTime_ReadSysTime,0x080E4F20);

    // --- [Filesystem Patches] ---
    rfx_patch_jmp(&rtfs_fopen,TTG_ADDR_FUNC_RTFS_FOPEN);
    rfx_patch_jmp(&rtfs_open,TTG_ADDR_FUNC_RTFS_OPEN);
    rfx_patch_jmp(&RTFS_TranslateFilename,TTG_ADDR_FUNC_RTFS_TRANSLATEFILENAME);
    rfx_patch_jmp(&RTFS_LZWfopen,TTG_ADDR_FUNC_RTFS_LZWFOPEN);

    // --- [Misc Patches] ---

    // Debug msg patch, normally written to a file... nicer to have it pushed to stdout.
    rfx_patch_jmp(&ttg_msg,TTG_ADDR_FUNC_MSG);

    // Game checks for the dongle, nope...
    rfx_patch_jmp(&rfx_generic_uint32_retone,TTG_ADDR_FUNC_IS_DONGLE_PRESENT);

    // Save Data Location Patches
    rfx_patch_str(TTG_ADDR_CSTR_SAVE01,"./save/01");
    rfx_patch_str(TTG_ADDR_CSTR_SAVE02,"./save/02");
    rfx_patch_str(TTG_ADDR_CSTR_SAVE03,"./save/03");
    rfx_patch_str(TTG_ADDR_CSTR_SAVE04,"./save/04");
    rfx_patch_str(TTG_ADDR_CSTR_SAVE05,"./save/05");
    rfx_patch_str(TTG_ADDR_CSTR_SAVE06,"./save/06");
    rfx_patch_str(TTG_ADDR_CSTR_SAVE07,"./save/07");
    rfx_patch_str(TTG_ADDR_CSTR_SAVE08,"./save/08");
    rfx_patch_str(TTG_ADDR_CSTR_SAVE09,"./save/09");
    rfx_patch_str(TTG_ADDR_CSTR_SAVE10,"./save/10");
    rfx_patch_str(TTG_ADDR_CSTR_SAVE11,"./save/11");
    rfx_patch_str(TTG_ADDR_CSTR_SAVE12,"./save/12");
}

/*
We only need to patch the binary once,
LD_PRELOAD will generally fire for every new library loaded
as well which doesn't appear to ever break anything; this
is just housekeeping...
*/
static unsigned char patched = NULL;

// Our Entry Point
void __attribute__((constructor)) initialize(void){
    printf("Starting up Wrapper...\n");
    if(!patched){
        patch_binary();
        patched = 1;
    }

}



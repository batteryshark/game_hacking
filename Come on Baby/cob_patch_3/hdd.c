

#include "global.h"
#include "hdd.h"
#include <string.h>
#define COB_SERIAL_CHECK_OFFSET 0x00412790

BOOL __stdcall rfx_GetVolumeInformationA(LPCSTR lpRootPathName, LPSTR lpVolumeNameBuffer, DWORD nVolumeNameSize, LPDWORD lpVolumeSerialNumber, LPDWORD lpMaximumComponentLength, LPDWORD lpFileSystemFlags, LPSTR lpFileSystemNameBuffer, DWORD nFileSystemNameSize){
    memset(lpVolumeSerialNumber,0x00,strlen(HDD_SERIAL)+1);
    strcpy(lpVolumeSerialNumber,HDD_SERIAL);

    return true;
}

void patch_hdd(){
    //Hook_IAT_Name("KERNEL32.dll","GetVolumeInformationA",&rfx_GetVolumeInformationA);
    detour_jmp(&patch_return_one,COB_SERIAL_CHECK_OFFSET);
}
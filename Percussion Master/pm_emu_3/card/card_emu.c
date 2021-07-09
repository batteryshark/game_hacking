#include "card_emu.h"


long emu_CasDisconnect(long hCard, unsigned long dwDisposition){

    return 0;
}

long emu_CasListReaders(long hContext, const char *mszGroups, char *mszReaders, unsigned long *pcchReaders){

    return 0;
}

long emu_CasEstablishContext(unsigned long dwScope, const void *pvReserved1, const void *pvReserved2, long *phContext){

    return 0;
}

long emu_CasConnect(long hContext, const char *szReader, unsigned long dwShareMode, unsigned long dwPreferredProtocols, long *phCard, unsigned long *pdwActiveProtocol){
    return 0;
}

long emu_CasReleaseContext(long hContext){

    return 0;
}

long emu_SLE4442_Update_Main_MemoryA(long hCard, LPBYTE pbTransmitBuffer, MM_ADDRESS aStartAddr, unsigned int TransmitLength){

    return 0;
}
long emu_SLE4442_Read_Main_Memory(long hCard, LPBYTE pbRecvBuffer, MM_ADDRESS aStartAddr, LPDWORD pcbRecvLength){

    return 0;
}
long emu_SLE4442_Read_Protection_Memory(long hCard, LPBYTE pbRecvBuffer){

    return 0;
}
long emu_SLE4442_Compare_Verification_Data(long hCard, unsigned char PSC1, unsigned char PSC2, unsigned char PSC3){

    return 0;
}
long emu_SLE4442_Write_Protection_Memory(long hCard, LPBYTE pbTransmitBuffer, MM_ADDRESS aStartAddr, unsigned int TransmitLength){

    return 0;
}
long emu_SLE4442_Update_Security_Memory(long hCard, LPBYTE pbTransmitBuffer, MM_ADDRESS aStartAddr, unsigned int TransmitLength){

    return 0;
}
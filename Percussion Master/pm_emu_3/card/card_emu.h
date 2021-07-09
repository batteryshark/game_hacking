#pragma once

#define LPBYTE unsigned char*
#define MM_ADDRESS long
#define LPDWORD unsigned long*

long emu_CasDisconnect(long hCard, unsigned long dwDisposition);
long emu_CasListReaders(long hContext, const char *mszGroups, char *mszReaders, unsigned long *pcchReaders);
long emu_CasEstablishContext(unsigned long dwScope, const void *pvReserved1, const void *pvReserved2, long *phContext);
long emu_CasConnect(long hContext, const char *szReader, unsigned long dwShareMode, unsigned long dwPreferredProtocols, long *phCard, unsigned long *pdwActiveProtocol);
long emu_CasReleaseContext(long hContext);

// SLE4442 Memory Card API
#define MAX_MAIN_MEMORY 256
#define MAX_PROT_MEMORY 32

long emu_SLE4442_Update_Main_MemoryA(long hCard, LPBYTE pbTransmitBuffer, MM_ADDRESS aStartAddr, unsigned int TransmitLength);
long emu_SLE4442_Read_Main_Memory(long hCard, LPBYTE pbRecvBuffer, MM_ADDRESS aStartAddr, LPDWORD pcbRecvLength);
long emu_SLE4442_Read_Protection_Memory(long hCard, LPBYTE pbRecvBuffer);
long emu_SLE4442_Compare_Verification_Data(long hCard, unsigned char PSC1, unsigned char PSC2, unsigned char PSC3);
long emu_SLE4442_Write_Protection_Memory(long hCard, LPBYTE pbTransmitBuffer, MM_ADDRESS aStartAddr, unsigned int TransmitLength);
long emu_SLE4442_Update_Security_Memory(long hCard, LPBYTE pbTransmitBuffer, MM_ADDRESS aStartAddr, unsigned int TransmitLength);
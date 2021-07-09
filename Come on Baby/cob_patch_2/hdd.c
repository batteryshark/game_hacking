#include <Windows.h>
#include "global.h"
#include "hdd.h"


pGetVolumeInformationA real_GetVolumeInformationA = NULL;
pCloseHandle real_CloseHandle = NULL;

// Note: We could also syscall hook this as NtQueryVolumeInformationFile(FileHandle, &IoStatusBlock, stuff, wat, FileFsVolumeInformation) and get the serial out of stuff or tamper with it.
BOOL HK_GetVolumeInformationA(
	LPCSTR  lpRootPathName,
	LPSTR   lpVolumeNameBuffer,
	DWORD   nVolumeNameSize,
	LPDWORD lpVolumeSerialNumber,
	LPDWORD lpMaximumComponentLength,
	LPDWORD lpFileSystemFlags,
	LPSTR   lpFileSystemNameBuffer,
	DWORD   nFileSystemNameSize
) {
	BOOL result = real_GetVolumeInformationA(lpRootPathName, lpVolumeNameBuffer, nVolumeNameSize, lpVolumeSerialNumber, lpMaximumComponentLength, lpFileSystemFlags, lpFileSystemNameBuffer, nFileSystemNameSize);
	// If we're asking for the volume serial number, then get the expected serial number.
	if (lpVolumeSerialNumber && lpRootPathName[0] == HDD_ROOT) {
		lpVolumeSerialNumber = win386p_file + 4;
	}
}


BOOL HK_CloseHandle(HANDLE hObject) {
	if (hObject == FAKE_HDDFILE_HANDLE) { return TRUE; }
	return real_CloseHandle(hObject);
}

void hdd_patch() {
	// Dynamically Resolve Library Stuff

	// Patch CreateFileA

	// Patch ReadFile

	// Patch CloseFile

	// Patch GetVolumeInformationA
}
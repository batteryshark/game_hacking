#include <Windows.h>
#include <winternl.h>
#include "kitchen_sink.h"
#include "io.hanoi.h"
#include "fs_redirect.h"

#define EZSUSB_PATH L"Ezusb-0"
#define FAKE_EZUSB_HANDLE (HANDLE)0x1337

typedef NTSTATUS __stdcall tNtOpenFile(PHANDLE FileHandle, ACCESS_MASK DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, ULONG ShareAccess, ULONG OpenOptions);
typedef NTSTATUS __stdcall tNtDeviceIoControlFile(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, ULONG IoControlCode, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength);
typedef NTSTATUS __stdcall tNtCreateFile(PHANDLE FileHandle, DWORD DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength);
static tNtDeviceIoControlFile* ntdll_NtDeviceIoControlFile = 0;
static tNtCreateFile* ntdll_NtCreateFile = 0;
static tNtOpenFile* ntdll_NtOpenFile = 0;
NTSTATUS NTAPI x_NtCreateFile(PHANDLE FileHandle, DWORD DesiredAccess, POBJECT_ATTRIBUTES ObjectAttributes, PIO_STATUS_BLOCK IoStatusBlock, PLARGE_INTEGER AllocationSize, ULONG FileAttributes, ULONG ShareAccess, ULONG CreateDisposition, ULONG CreateOptions, PVOID EaBuffer, ULONG EaLength) {
    if (ObjectAttributes && ObjectAttributes->ObjectName && ObjectAttributes->ObjectName->Buffer) {
       
        if (wcsstr(ObjectAttributes->ObjectName->Buffer,EZSUSB_PATH)) {
            *FileHandle = FAKE_EZUSB_HANDLE;
            return 0;
        }
    }

    return ntdll_NtCreateFile(FileHandle, DesiredAccess, ObjectAttributes, IoStatusBlock, AllocationSize, FileAttributes, ShareAccess, CreateDisposition, CreateOptions, EaBuffer, EaLength);
}



NTSTATUS NTAPI x_NtDeviceIoControlFile(HANDLE FileHandle, HANDLE Event, PIO_APC_ROUTINE ApcRoutine, PVOID ApcContext, PIO_STATUS_BLOCK IoStatusBlock, ULONG IoControlCode, PVOID InputBuffer, ULONG InputBufferLength, PVOID OutputBuffer, ULONG OutputBufferLength) {

    if (FileHandle != FAKE_EZUSB_HANDLE) {
        return ntdll_NtDeviceIoControlFile(FileHandle, Event, ApcRoutine, ApcContext, IoStatusBlock, IoControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength);
    }

    ProcessUnityIOIoctlWindows(IoControlCode, InputBuffer, InputBufferLength, OutputBuffer, OutputBufferLength);
    IoStatusBlock->Information = 8;
    return 0;
}

void init_fs_redirect(){
    HotPatch_patch("ntdll.dll", "NtCreateFile", 0x10, x_NtCreateFile, (void**)&ntdll_NtCreateFile);
   
    HotPatch_patch("ntdll.dll", "NtDeviceIoControlFile", 0x10, x_NtDeviceIoControlFile, (void**)&ntdll_NtDeviceIoControlFile);
}